#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

# Include the required modules.
include("FetchContent")

# Darwin dependencies.
if(APPLE)
    find_library(FRAMEWORK_APPKIT AppKit     REQUIRED)
    find_library(FRAMEWORK_COCOA  Cocoa      REQUIRED)
    find_library(FRAMEWORK_QUARTZ QuartzCore REQUIRED)
endif(APPLE)

# Doxygen.
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
endif(BUILD_DOCS)

# GLM.
if(NOT TARGET glm::glm)
    FetchContent_Declare(Glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG "1.0.1")
    FetchContent_MakeAvailable(Glm)
    set_property(TARGET glm PROPERTY QT_EXCLUDE_FROM_TRANSLATION ON)

    file(DOWNLOAD
        "https://raw.githubusercontent.com/madevgeny/glm-aabb/refs/heads/master/glm-aabb/AABB.hpp"
        "${glm_SOURCE_DIR}/glm/glm-aabb.hpp"
    )
endif(NOT TARGET glm::glm)

# Qt6.
set(QT_COMPONENTS
    Core
    DBus
    Gui
    OpenGL
    OpenGLWidgets
    Svg
    Test
    Widgets
)

set(QT_EXTRA_COMPONENTS
    LinguistTools
)

if(LINUX)
    list(APPEND QT_COMPONENTS XcbQpaPrivate)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS} ${QT_EXTRA_COMPONENTS})

# Vulkan.
find_package(Vulkan 1.3 REQUIRED COMPONENTS glslc glslangValidator SPIRV-Tools)
