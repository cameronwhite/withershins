#include "withershins.hpp"

#include <cxxabi.h>
#include <execinfo.h>
#include <exception>
#include <memory>

/// Custom delete to use free() with std::unique_ptr.
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

/// Extract the demangled symbol name from a string formatted as:
/// module(function+offset) [address].
static std::string find_symbol_name(const std::string &trace)
{
    const std::string::size_type name_begin = trace.find('(');
    if (name_begin == std::string::npos)
        return "";

    const std::string::size_type name_end = trace.find('+', name_begin);
    if (name_end == std::string::npos)
        return "";

    return demangle(trace.substr(name_begin + 1, name_end - name_begin - 1));
}

/// Extract the file name from a string formatted as:
/// module(function+offset) [address].
static std::string find_file_name(const std::string &/*trace*/)
{
    // TODO - implement.
    return "";
}

/// Find the line number from a string formatted as:
/// module(function+offset) [address].
static int find_line_number(const std::string &/*trace*/)
{
    // TODO - implement.
    return -1;
}

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
        frames.emplace_back(find_symbol_name(symbol), find_file_name(symbol),
                            find_line_number(symbol));
    }

    return frames;
}
