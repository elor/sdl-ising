#!/bin/bash
#
# This script watches for file changes and rebuilds the project during development
# I'm using it with Neovim to automatically run tests whenever I save any file

set -e -u

watchexec --exts cpp,hpp,txt --restart --ignore 'build/**' './install-build-and-test.sh && echo "Build successful" && build/bin/main'

