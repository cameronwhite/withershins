#include <withershins.hpp>

#include "catch.hpp"

void baz();

void bar()
{
    baz();
}

void foo()
{
    bar();
}

#if defined(_WIN32) || defined(WITHERSHINS_ENABLE_LIBBFD)
#define ENABLE_FILE_INFO
#endif

#ifdef ENABLE_FILE_INFO
static bool ends_with(const std::string &a, const std::string &b)
{
    if (a.length() >= b.length())
        return a.compare(a.length() - b.length(), b.length(), b) == 0;
    else
        return false;
}
#endif

void baz()
{
    std::vector<withershins::frame> frames = withershins::trace();

    REQUIRE(frames.size() >= 3);

    REQUIRE(frames[0].function_name() == "baz");
    REQUIRE(frames[0].module_name().find("tests") != std::string::npos);
    REQUIRE(frames[1].function_name() == "bar");
    REQUIRE(frames[2].function_name() == "foo");

#ifdef ENABLE_FILE_INFO
    REQUIRE(ends_with(frames[0].file_name(), "test_basic.cpp"));
    REQUIRE(frames[1].line_number() == 10);
    REQUIRE(frames[2].line_number() == 15);
#endif
}

TEST_CASE("BasicSymbolNames", "")
{
    foo();
}
