#include "withershins.hpp"

#include <exception>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

std::once_flag is_initialized;

/// Retrieve the error message corresponding to GetLastError() and throw an
/// exception.
static void throw_last_error()
{
    const DWORD error = GetLastError();

    LPTSTR buffer = nullptr;
    DWORD size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, error, 0, reinterpret_cast<LPTSTR>(&buffer), 0, nullptr);

    std::string message(buffer, size);
    LocalFree(buffer);

    throw std::runtime_error(message);
}

std::vector<withershins::frame> withershins::trace(int max_frames)
{
    const HANDLE process = GetCurrentProcess();
    std::vector<withershins::frame> frames;

    // Initialize the symbol handler.
    std::call_once(is_initialized, [=]() {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS |
                      SYMOPT_LOAD_LINES);

        if (!SymInitialize(process, NULL, true))
            throw_last_error();
    });

    std::vector<PVOID> addresses(max_frames);
    // Capture the stack trace, but skip the first entry since we don't need to
    // include the current function.
    const USHORT num_frames =
        CaptureStackBackTrace(1, max_frames, addresses.data(), 0);

    // Set up a buffer large enough to hold the symbol information and the
    // variable length name.
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(TCHAR)];
    PSYMBOL_INFO info = reinterpret_cast<PSYMBOL_INFO>(buffer);
    info->SizeOfStruct = sizeof(SYMBOL_INFO);
    info->MaxNameLen = MAX_SYM_NAME;

    // Find the symbols for each address.
    DWORD64 displacement = 0;
    for (USHORT i = 0; i < num_frames; ++i)
    {
        const DWORD64 address = reinterpret_cast<DWORD64>(addresses[i]);
        if (!SymFromAddr(process, address, &displacement, info))
            throw_last_error();

        const std::string function_name(info->Name, info->NameLen);
        std::string file_name;
        int line_number = -1;

        IMAGEHLP_LINE64 line_info;
        line_info.SizeOfStruct = sizeof(line_info);
        DWORD displacement = 0;
        if (SymGetLineFromAddr64(process, address, &displacement, &line_info))
        {
            file_name = std::string(line_info.FileName);
            line_number = line_info.LineNumber;
        }

        // Find the module name.
        IMAGEHLP_MODULE64 module_info;
        module_info.SizeOfStruct = sizeof(module_info);
        std::string module_name;
        if (SymGetModuleInfo64(process, address, &module_info))
            module_name = module_info.LoadedImageName;

        frames.emplace_back(std::move(module_name), std::move(function_name),
                            std::move(file_name), line_number);
    }

    return frames;
}
