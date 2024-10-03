#
# Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
#

com_library(scene
    SOURCES
        "camera.cxx"
        "camera.hxx"
        "document.cxx"
        "document.hxx"
        "model.cxx"
        "model.hxx"

    PUBLIC_LIBRARIES
        com::rhi

        Qt::Core
)
