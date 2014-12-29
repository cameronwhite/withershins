#include "withershins.hpp"

#include <utility>

withershins::frame::frame(std::string module_name, std::string function_name,
                          std::string file_name, int line)
    : m_module_name(std::move(module_name)),
      m_function_name(std::move(function_name)),
      m_file_name(std::move(file_name)),
      m_line(line)
{
}
