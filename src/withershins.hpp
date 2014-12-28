#ifndef WITHERSHINS_HPP_INCLUDED
#define WITHERSHINS_HPP_INCLUDED

#include <string>
#include <vector>

namespace withershins
{
    class frame
    {
    public:
        frame(std::string symbol_name);

        const std::string &symbol_name() const { return m_symbol_name; }

    private:
        std::string m_symbol_name;
    };

    std::vector<frame> trace(int max_frames = 100);
}

#endif
