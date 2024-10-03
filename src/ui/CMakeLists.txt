#
# Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
#

com_library(ui
    SOURCES
        "about.cxx"
        "about.hxx"
        "about.ui"

        $<$<PLATFORM_ID:Darwin>:darwin.mm>

        "dock-widget.hxx"

        "error-log.cxx"
        "error-log.hxx"
        "error-log.ui"

        "main-window.cxx"
        "main-window.hxx"
        "main-window.ui"

        "properties-panel.cxx"
        "properties-panel.hxx"
        "properties-panel.ui"

        "settings-panel.cxx"
        "settings-panel.hxx"
        "settings-panel.ui"

        "viewport.cxx"
        "viewport.hxx"

    PUBLIC_LIBRARIES
        com::rhi
        com::scene

        Qt::Gui
        Qt::Widgets

    PRIVATE_LIBRARIES
        $<$<PLATFORM_ID:Darwin>:${FRAMEWORK_COCOA}>
        $<$<PLATFORM_ID:Darwin>:${FRAMEWORK_QUARTZ}>
)
