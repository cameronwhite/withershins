#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

pwd

brew update
# Install Qt, gettext, and binutils.
brew install gettext osx/binutils.rb
brew link --force gettext
