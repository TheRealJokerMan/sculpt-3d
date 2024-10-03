#
# Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
#

configure_file("version.hxx.in"                                   "version.hxx")
configure_file("${CMAKE_SOURCE_DIR}/resources/resources.rc.in"    "resources.rc")
configure_file("${CMAKE_SOURCE_DIR}/resources/icons/app-icon.ico" "frame.ico" COPYONLY)

set_source_files_properties("${CMAKE_SOURCE_DIR}/resources/icons/${CMAKE_PROJECT_NAME}.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

qt_add_resources(APP_QT_RESOURCES
    "${CMAKE_SOURCE_DIR}/resources/resources.qrc"
    "${PROJECT_BINARY_DIR}/shaders.qrc"
)

com_executable(${CMAKE_PROJECT_NAME}
    SOURCES
        "app.cxx"
        "app.hxx"
        ${APP_QT_RESOURCES}
        "entry-point.cxx"
        $<$<PLATFORM_ID:Darwin>:main-darwin.mm>
        $<$<PLATFORM_ID:Darwin>:${CMAKE_SOURCE_DIR}/resources/icons/${CMAKE_PROJECT_NAME}.icns>
        $<$<PLATFORM_ID:Linux>:main-linux.cxx>
        $<$<PLATFORM_ID:Windows>:main-win32.cxx>
        $<$<PLATFORM_ID:Windows>:resources.rc>

    TS_FILES
        "${CMAKE_SOURCE_DIR}/resources/translations/en_GB.ts"

    LIBRARIES
        com::ui
        ${FRAMEWORK_COCOA}
        ${FRAMEWORK_QUARTZ}
)

# Deployment.
deploy_app(${CMAKE_PROJECT_NAME}
    LIBRARIES_DARWIN
        Vulkan::Vulkan
)
