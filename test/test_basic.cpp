#include <withershins.hpp>

#include "catch.hpp"

void baz();

void bar()
{
    baz();
}

static void foo()
{
    bar();
}

static bool ends_with(const std::string &a, const std::string &b)
{
    if (a.length() >= b.length())
        return a.compare(a.length() - b.length(), b.length(), b) == 0;
    else
        return false;
}

void baz()
{
    std::vector<withershins::frame> frames = withershins::trace();

    REQUIRE(frames.size() >= 3);

    REQUIRE(frames[0].symbol_name() == "baz");
    REQUIRE(ends_with(frames[0].file_name(), "test_basic.cpp"));

    REQUIRE(frames[1].symbol_name() == "bar");
    REQUIRE(frames[1].line_number() == 10);

    REQUIRE(frames[2].symbol_name() == "foo");
    REQUIRE(frames[2].line_number() == 15);
}

TEST_CASE("BasicSymbolNames", "")
{
    foo();
}
