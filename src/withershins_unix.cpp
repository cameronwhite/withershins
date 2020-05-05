#include "withershins.hpp"

#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <execinfo.h>
#include <exception>
#include <memory>
#include <mutex>

#ifdef __APPLE__
#include <sstream>
#endif

#ifdef WITHERSHINS_ENABLE_LIBBFD

// Fix issues with including newer versions of bfd.h
// (https://sourceware.org/bugzilla/show_bug.cgi?id=14243).
#define PACKAGE "withershins"

#include <bfd.h>

// Binutils 2.34 introduced API changes ...
#ifndef bfd_get_section_vma
#define bfd_get_section_vma(H, S) bfd_section_vma(S)
#define bfd_section_size(H, S) bfd_section_size(S)
#endif

std::once_flag initialized_bfd;

/// Custom deleter to use bfd_close() with std::unique_ptr.
struct bfd_delete
{
    void operator()(bfd *abfd)
    {
        bfd_close(abfd);
    }
};

#endif

/// Custom deleter to use free() with std::unique_ptr.
struct free_delete
{
    void operator()(void *p)
    {
        free(p);
    }
};

/// Demangle the given symbol.
static std::string demangle(std::string mangled)
{
    int status = 0;
    std::unique_ptr<char, free_delete> name(
        abi::__cxa_demangle(mangled.c_str(), 0, 0, &status));

    if (status != 0)
        return mangled;

    // Extract only the name of the function.
    std::string demangled(name.get());
    const std::string::size_type name_end = demangled.find('(');
    if (name_end != std::string::npos)
        demangled.erase(name_end, demangled.length() - name_end);

    return demangled;
}

static bool find_module_and_function(const std::string &trace,
                                     std::string &module_name,
                                     std::string &function_name)
{
#ifdef __APPLE__
    // Extract the module name and demangled function name from a string
    // formatted like "index module address function + offset".
    std::istringstream str(trace);

    int index;
    std::string address;
    str >> index >> module_name >> address >> function_name;

    if (!str)
        return false;

    function_name = demangle(function_name);
    return true;
#else
    // Extract the module name and demangled function name from a string
    // formatted as "module(function+offset) [address]".
    const std::string::size_type module_end = trace.find('(');
    if (module_end == std::string::npos)
        return false;

    module_name = trace.substr(0, module_end);

    const std::string::size_type name_begin = module_end + 1;
    const std::string::size_type name_end = trace.find('+', name_begin);
    if (name_end == std::string::npos)
        return false;

    function_name =
        demangle(trace.substr(module_end + 1, name_end - name_begin));

    return true;
#endif
}

#ifdef WITHERSHINS_ENABLE_LIBBFD
static void throw_bfd_error(std::string msg)
{
    msg += ": ";

    const bfd_error error = bfd_get_error();
    msg += std::to_string(static_cast<int>(error));
    msg += " (";

    if (error == bfd_error_system_call)
    {
        char buf[1024];
#ifdef _GNU_SOURCE
        msg += strerror_r(errno, buf, sizeof(buf));
#else
        int result = strerror_r(errno, buf, sizeof(buf));
        msg += (result == 0) ? buf : "error in strerror_r";
#endif
    }
    else
        msg += bfd_errmsg(error);

    msg += ')';
    throw std::runtime_error(msg);
}

/// Attempt to find the file name, function name, and line number.
static bool find_file_info(const std::string &module, const void *address,
                           std::string &file_name, std::string &function_name,
                           int &line_number)
{
    const bfd_vma vma = reinterpret_cast<bfd_vma>(address);

    // Initialize the bfd library.
    std::call_once(initialized_bfd, bfd_init);

    // Open the bfd. TODO - cache this?
    std::unique_ptr<bfd, bfd_delete> abfd(bfd_openr(module.c_str(), 0));
    if (!abfd)
        throw_bfd_error("bfd_openr - " + module);
    if (!bfd_check_format(abfd.get(), bfd_object))
        throw_bfd_error("bfd_check_format");

    // Iterate through the linked list of sections.
    for (asection *section = abfd->sections; section != nullptr;
         section = section->next)
    {
        // Check whether the address is in this section.
        const bfd_vma section_vma = bfd_get_section_vma(abfd.get(), section);
        if (vma >= section_vma &&
            vma < section_vma + bfd_section_size(abfd.get(), section))
        {
            // Get the file and line number.
            const bfd_vma offset = vma - section_vma;
            const char *file = 0;
            const char *func = 0;
            unsigned int line = 0;
            // TODO - should 'symbols' be non-null?
            if (!bfd_find_nearest_line(abfd.get(), section, /*symbols*/ 0,
                                       offset, &file, &func, &line))
            {
                continue;
            }

            if (file)
                file_name = file;
            if (func)
                function_name = demangle(func);
            line_number = line;
            return true;
        }
    }

    return false;
}

#else
static bool find_file_info(const std::string &, const void *,
                           std::string &file_name, std::string &,
                           int &line_number)
{
    file_name = "";
    line_number = -1;
    return true;
}
#endif

std::vector<withershins::frame> withershins::trace(int max_frames)
{
    std::vector<withershins::frame> frames;

    // Get the addresses for the backtrace.
    std::vector<void *> addresses(max_frames);
    const int n_frames = backtrace(addresses.data(), max_frames);
    if (!n_frames)
        throw std::runtime_error("Error in backtrace");

    // Find symbols for the addresses.
    std::unique_ptr<char *, free_delete> symbols(
        backtrace_symbols(addresses.data(), n_frames));

    if (!symbols)
        throw std::runtime_error("Error in backtrace_symbols");

    // Skip the first entry, since we don't want to include ourself.
    for (int i = 1; i < n_frames; ++i)
    {
        const std::string symbol = symbols.get()[i];

        // Attempt to determine a function name from the string returned by
        // backtrace_symbols.
        std::string module_name;
        std::string function_name;
        find_module_and_function(symbol, module_name, function_name);

        std::string file_name;
        int line_number = -1;
        // Attempt to find a file name and line number.
        // For things like static functions, we may also be able to get a
        // function name through libbfd but not through backtrace().
        find_file_info(module_name, addresses[i], file_name, function_name,
                       line_number);

        frames.emplace_back(std::move(module_name), std::move(function_name),
                            std::move(file_name), line_number);
    }

    return frames;
}
