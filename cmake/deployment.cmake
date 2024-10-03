#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEPLOY_BUILD_TYPE "DEBUG")
else()
    set(DEPLOY_BUILD_TYPE "RELEASE")
endif()

# Deploy an application and its dependencies.
function(deploy_app appName)
    set(optionKeywords)

    set(oneValueKeywords)

    set(multiValueKeywords
        DIRECTORIES         # List of directories.
        FRAMEWORKS          # List of OSX frameworks.
        LIBRARIES           # List of shared libraries.
        LIBRARIES_DARWIN    # List of shared libraries only for OSX.
        LIBRARIES_LINUX     # List of shared libraries only for Linux.
        LIBRARIES_WIN32     # List of shared libraries only for Windows.
        PLUGINS             # List of plugin libraries.
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

    set(_deployDest "bin")
    if(APPLE)
        set(_deployDest "${appName}.app/Contents/MacOS")
    endif()

    install(TARGETS ${appName} RUNTIME DESTINATION ${_deployDest} BUNDLE DESTINATION ".")

    foreach(dir ${args_DIRECTORIES})
        install(DIRECTORY ${dir} DESTINATION ${_deployDest} USE_SOURCE_PERMISSIONS)
    endforeach()

    if(APPLE)
        foreach(fw ${args_FRAMEWORKS})
            get_target_property(FW_SOURCE_DIR ${fw} IMPORTED_LOCATION)
            while(NOT ${FW_SOURCE_DIR} MATCHES ".*framework$")
                get_filename_component(FW_SOURCE_DIR "${FW_SOURCE_DIR}" PATH)
            endwhile()

            install(DIRECTORY ${FW_SOURCE_DIR} DESTINATION "${_deployDest}/../Frameworks" USE_SOURCE_PERMISSIONS)
        endforeach()
    endif(APPLE)

    foreach(lib ${args_LIBRARIES})
        __install_library(${lib} ${_deployDest})
    endforeach()

    if(APPLE)
        foreach(lib ${args_LIBRARIES_DARWIN})
            __install_library(${lib} ${_deployDest})
        endforeach()
    endif(APPLE)

    if(LINUX)
        foreach(lib ${args_LIBRARIES_LINUX})
            __install_library(${lib} ${_deployDest})
        endforeach()
    endif(LINUX)

    if(WIN32)
        foreach(lib ${args_LIBRARIES_WIN32})
            __install_library(${lib} ${_deployDest})
        endforeach()
    endif(WIN32)

    foreach(lib ${args_PLUGINS})
        install(FILES $<TARGET_FILE:${lib}> DESTINATION "${_deployDest}/plugins")
    endforeach()

    # Compute the base path of Qt.
    get_target_property(_qtDir Qt6::Core IMPORTED_LOCATION_${DEPLOY_BUILD_TYPE})
    if(NOT _qtDir)
        get_target_property(_qtDir Qt6::Core IMPORTED_LOCATION)
    endif()

    # Install the Qt components.
    foreach(lib ${QT_COMPONENTS})
        if(APPLE)
            get_target_property(_qtLoc Qt6::${lib} IMPORTED_LOCATION)
            get_filename_component(_qtLoc ${_qtLoc} DIRECTORY)
            get_filename_component(_qtLoc ${_qtLoc}/../../ ABSOLUTE)
            __install_framework(${_qtLoc} ${_deployDest})
        elseif(WIN32)
            __install_library("Qt6::${lib}" ${_deployDest})
        else()
        endif()
    endforeach()

    # Install the Qt plugins.
    get_filename_component(_qtDir ${_qtDir} DIRECTORY)
    if(APPLE)
        get_filename_component(_qtDir ${_qtDir}/../../../../ ABSOLUTE)
        install(DIRECTORY "${_qtDir}/plugins/" DESTINATION "${_deployDest}/../PlugIns" USE_SOURCE_PERMISSIONS)
    else()
        install(DIRECTORY "${_qtDir}/../plugins/" DESTINATION "${_deployDest}/../plugins" USE_SOURCE_PERMISSIONS)
        get_filename_component(_qtDir ${_qtDir}/../ ABSOLUTE)
    endif()

    # Install the Qt resources.
    if(APPLE)
        install(FILES "${CMAKE_SOURCE_DIR}/resources/qt.conf" DESTINATION "${_deployDest}/../Resources")
    else()
        install(FILES "${CMAKE_SOURCE_DIR}/resources/qt.conf" DESTINATION ${_deployDest})
    endif()
endfunction(deploy_app)


function(__install_framework frameWork dest)
    if(APPLE)
        set(_destination "${dest}/../Frameworks")
        install(DIRECTORY "${frameWork}" DESTINATION ${_destination} USE_SOURCE_PERMISSIONS)
    endif(APPLE)
endfunction(__install_framework)


function(__install_library lib dest)
    if(TARGET ${lib})
        get_target_property(libLoc ${lib} IMPORTED_LOCATION_${DEPLOY_BUILD_TYPE})
        if(NOT libLoc)
            get_target_property(libLoc ${lib} LOCATION)
            if(NOT libLoc)
                get_target_property(libLoc ${lib} IMPORTED_LOCATION)
                if(NOT libLoc)
                    message(FATAL_ERROR "Can't find ${libLoc}")
                endif()
            endif()
        endif()

        # Sometimes the location is incorrectly set to ".lib" instead of ".dll"
        if(WIN32)
            string(REGEX REPLACE "\\.lib$" ".dll" libLoc "${libLoc}")

             if(${lib} MATCHES "^GLEW::")
                string(REGEX REPLACE "/lib" "/bin"  libLoc "${libLoc}")
            endif()
        endif()

        # If this is a symlink on Mac/Linux make sure we also install the target
        if(UNIX)
            get_filename_component(_shared_lib_dir  ${libLoc} DIRECTORY)
            get_filename_component(_shared_lib_name ${libLoc} NAME_WE)

            file(GLOB _files "${_shared_lib_dir}/${_shared_lib_name}*")

            install(FILES ${_files} DESTINATION ${dest})
        else()
            install(FILES ${libLoc} DESTINATION ${dest})
        endif()
    else()
        install(FILES $<TARGET_FILE:${lib}> DESTINATION ${dest})
    endif()
endfunction(__install_library)
