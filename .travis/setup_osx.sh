#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
# Install gettext and binutils.
brew install gettext osx/binutils.rb
brew link --force gettext
