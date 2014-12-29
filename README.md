withershins
===========

[![Build Status](https://travis-ci.org/cameronwhite/withershins.svg?branch=master)](https://travis-ci.org/cameronwhite/withershins)
[![Build status](https://ci.appveyor.com/api/projects/status/ka47n02r8ja2adu2?svg=true)](https://ci.appveyor.com/project/cameronwhite/withershins)
[![License](http://img.shields.io/:license-mit-blue.svg)](https://github.com/cameronwhite/withershins/blob/master/LICENSE)

A simple cross-platform C++11 library for generating stack traces.

### Supported Platforms
This library is currently supported on Linux and Windows, and requires a compiler that is fairly compliant with C++11 - the library has been compiled under gcc 4.8, clang 3.4, and MSVC 2013 (older versions of those compilers may work, but have not yet been tested).

### Dependencies
* Linux: `libbfd`
  * For Ubuntu and similar distributions, this can be installed through `apt-get install binutils-dev`.
* Windows: `DbgHelp.dll` (included with Windows)

### Example
```cpp
#include <withershins.hpp>

void foo()
{
  std::vector<withershins::frame> frames = withershins::trace();
  assert(frames[0].function_name() == "foo");
  assert(frames[1].function_name() == "main");
}

int main(int argc, char *argv[])
{
  foo();
  return 0;
}
```
