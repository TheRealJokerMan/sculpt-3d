#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#
# This module defines a set of functions for creating CMake and CTest targets. Defining CMake/CTest
# targets can be verbose and these functions help reduce that verbosity.


# Create a new named CMake target. Use this function to create an application.
function(com_executable targetName)
    set(multiValueKeywords
        SOURCES                     # List of source files.
        TS_FILES                    # List of Qt Linguist files.
        LIBRARIES                   # List of dependent libraries to link that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    if(args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    # Define the target.
    qt_add_executable(${targetName} MACOSX_BUNDLE WIN32 ${args_SOURCES})
    _set_target_defaults(${targetName})

    # See https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
    set_target_properties(${targetName}
        PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME           "${CMAKE_PROJECT_NAME}"
            MACOSX_BUNDLE_GUI_IDENTIFIER        "com.jamie-kenyon.${CMAKE_PROJECT_NAME}"
            MACOSX_BUNDLE_ICON_FILE             "${CMAKE_PROJECT_NAME}.icns"
            MACOSX_BUNDLE_COPYRIGHT             "2024 Jamie Kenyon. All Rights Reserved."
            MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}"
            MACOSX_BUNDLE_INFO_STRING           "${CMAKE_PROJECT_NAME} ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} by Jamie Kenyon."
            MACOSX_BUNDLE_LONG_VERSION_STRING   "${CMAKE_PROJECT_NAME} ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} by Jamie Kenyon."
    )

    if(args_TS_FILES)
        qt_add_translations(${targetName}
            TS_FILES
                ${args_TS_FILES}
            LUPDATE_OPTIONS
                -no-ui-lines -locations none
        )
    endif()

     # Set the target's dependencies.
    target_link_libraries(${targetName} PRIVATE ${args_LIBRARIES})
endfunction(com_executable)


# Create a new named CMake target. Use this function to create a static libary.
function(com_library targetName)
    set(multiValueKeywords
        SOURCES                      # List of source files.
        PRIVATE_LIBRARIES            # List of dependent libraries to link that upstream consumers don't require.
        PUBLIC_LIBRARIES             # List of dependent libraries to link that upstream consumers require.
        PUBLIC_COMPILE_DEFINITIONS   # List of compile definitions that upstream consumers require.
        PRIVATE_COMPILE_DEFINITIONS  # List of compile definitions that upstream consumers don't require.
    )

    cmake_parse_arguments(args
        "${optionKeywords}"
        "${oneValueKeywords}"
        "${multiValueKeywords}"
        ${ARGN}
    )

    if(args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    # Generate the target names.
    set(targetName "com.${targetName}")
    string(REPLACE "." "::" namespacedtargetName ${targetName})

    # Define the target.
    add_library(${targetName} STATIC ${args_SOURCES})
    _set_target_defaults(${targetName})

    # Set the compile definitions
    target_compile_definitions(${targetName}
        PUBLIC
            ${args_PUBLIC_COMPILE_DEFINITIONS}
        PRIVATE
            ${args_PRIVATE_COMPILE_DEFINITIONS}
    )

     # Set the target's dependencies.
    target_link_libraries(${targetName}
        PUBLIC
            ${args_PUBLIC_LIBRARIES}
        PRIVATE
            ${args_PRIVATE_LIBRARIES}
    )

    # Define an alias for downstream consumption.
    add_library(${namespacedtargetName} ALIAS ${targetName})
endfunction(com_library)


# Set default target options.
function(_set_target_defaults targetName)
    # Set properties.
    set_target_properties(${targetName}
        PROPERTIES
            BUILD_WITH_INSTALL_RPATH  ON
            COMPILE_WARNING_AS_ERROR  ON
            C_VISIBILITY_PRESET       default
            CXX_VISIBILITY_PRESET     default
            MACOSX_RPATH              ON
            POSITION_INDEPENDENT_CODE ON
            VISIBILITY_INLINES_HIDDEN OFF
    )

    if(APPLE)
        set_target_properties(${targetName} PROPERTIES INSTALL_RPATH "@executable_path;@loader_path;@executable_path/../Frameworks")
    elseif(LINUX)
        set_target_properties(${targetName} PROPERTIES INSTALL_RPATH "$ORIGIN;$ORIGIN/..")
    endif(APPLE)

    # Set the target's compile definitions.
    target_compile_definitions(${targetName}
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:_GLIBCXX_USE_CXX11_ABI=1>
            $<$<CXX_COMPILER_ID:MSVC>:_AMD64_>
            $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
            $<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>
    )

    # Set the C++ standard version.
    target_compile_features(${targetName} PRIVATE cxx_std_23)

    # Set C++ flags.
    target_compile_options(${targetName}
        PRIVATE
            $<$<CXX_COMPILER_ID:AppleClang,GNU>:-Wall>                                        # Highest warning-level.
            $<$<CXX_COMPILER_ID:MSVC>:/W4>                                                    # Highest warning-level.
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/analyze:WX- /analyze:external-> # MSVC Code Analysis.
            $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Zi>                           # Symbols in Release.
            $<$<AND:$<CXX_COMPILER_ID:AppleClang,GNU>,$<CONFIG:Release>>:-g -g2>              # Symbols in Release.
    )

    # Set the target's include directories.
    target_include_directories(${targetName}
        PRIVATE
            "${CMAKE_SOURCE_DIR}/src"
            "${CMAKE_BINARY_DIR}/src"
            "${CMAKE_SOURCE_DIR}"
    )
endfunction(_set_target_defaults)
