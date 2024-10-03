#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

set(CMAKE_INSTALL_MESSAGE LAZY)

# Set the minimum MacOS version.
set(CMAKE_OSX_DEPLOYMENT_TARGET "15" CACHE STRING "Minimum OS X deployment version")

# Clang Format.
find_program(CLANG_FORMAT_EXECUTABLE NAMES "clang-format" REQUIRED
    HINTS
        ENV "VCINSTALLDIR"
    PATH_SUFFIXES
         "Tools/Llvm/x64/bin"
)
