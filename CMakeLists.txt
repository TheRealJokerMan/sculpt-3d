#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

# Set the minimum CMake version.
cmake_minimum_required(VERSION 3.29.5 FATAL_ERROR)

# Include pre-project definitions.
include("toolchain-defaults")

# Define the project.
project(Sculpt3D
    VERSION         0.0.1.0
    DESCRIPTION     "Sculpt3D"
    HOMEPAGE_URL    "https://www.jamiekenyon.com/"
    LANGUAGES       CXX
)

# Include the dependent modules.
include("deployment")
include("options")
include("packages")
include("targets")

# Enable testing.
if(BUILD_TESTING)
    enable_testing()
endif()

# Setup project-wide defaults to a standard arrangement.
qt_standard_project_setup(I18N_SOURCE_LANGUAGE en)

# Add the sources.
add_subdirectory("src")

# Add the documentation.
add_subdirectory("docs")
