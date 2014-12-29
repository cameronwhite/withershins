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

    // TODO - find file name and line numbers.
    // TODO - demangle symbols.
    std::string file_name;
    int line_number = -1;

    for (int i = 0; i < n_frames; ++i)
        frames.emplace_back(symbols.get()[i], file_name, line_number);

    return frames;
}
