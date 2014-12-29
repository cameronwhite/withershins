#ifndef WITHERSHINS_HPP_INCLUDED
#define WITHERSHINS_HPP_INCLUDED

#include <string>
#include <vector>

namespace withershins
{
    class frame
    {
    public:
        frame(std::string module_name, std::string function_name,
              std::string file_name, int line);

        /// The name of the module that the function belongs to.
        const std::string &module_name() const { return m_module_name; }

        /// The name of the current function.
        const std::string &function_name() const { return m_function_name; }

        /// The source file name. This is an empty string if the source file
        /// information could not be located.
        const std::string &file_name() const { return m_file_name; }

        /// The source line for the stack frame. This is -1 if the source file
        /// information could not be located.
        int line_number() const { return m_line; }

    private:
        std::string m_module_name;
        std::string m_function_name;
        std::string m_file_name;
        int m_line;
    };

    /// Returns a stack trace of at most 'max_frame' previous frames.
    std::vector<frame> trace(int max_frames = 100);
}

#endif
