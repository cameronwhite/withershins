#include <withershins.hpp>

#include "catch.hpp"

static void baz()
{
    std::vector<withershins::frame> frames = withershins::trace();

    REQUIRE(frames.size() >= 3);
    REQUIRE(frames[0].symbol_name() == "baz");
    REQUIRE(frames[1].symbol_name() == "bar");
    REQUIRE(frames[2].symbol_name() == "foo");
}

static void bar()
{
    baz();
}

static void foo()
{
    bar();
}

TEST_CASE("BasicSymbolNames", "")
{
    foo();
}