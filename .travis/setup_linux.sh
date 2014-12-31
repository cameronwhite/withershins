#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

sudo add-apt-repository --yes ppa:kalakris/cmake # For cmake 2.8.9+
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test # gcc 4.8
sudo apt-get update -yqq
sudo apt-get install -yqq cmake g++-4.8 binutils-dev
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
