#include "withershins.hpp"

#include <exception>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

withershins::frame::frame(std::string symbol_name)
    : m_symbol_name(std::move(symbol_name))
{
}

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
    std::vector<withershins::frame> frames;

    // Initialize the symbol handler.
    const HANDLE process = GetCurrentProcess();
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    if (!SymInitialize(process, NULL, true))
        throw_last_error();

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
        if (SymFromAddr(process, reinterpret_cast<DWORD64>(addresses[i]),
                        &displacement, info))
        {
            frames.emplace_back(std::string(info->Name, info->NameLen));
        }
        else
            throw_last_error();
    }

    return frames;
}
