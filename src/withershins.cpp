#include "withershins.hpp"

#include <utility>

withershins::frame::frame(std::string symbol_name, std::string file_name,
                          int line)
    : m_symbol_name(std::move(symbol_name)),
      m_file_name(std::move(file_name)),
      m_line(line)
{
}
