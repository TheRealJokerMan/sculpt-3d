#
# Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
#

com_library(base
    SOURCES
        "message.cxx"
        "message.hxx"
        "preferences.cxx"
        "preferences.hxx"
        "resource.cxx"
        "resource.hxx"

    PRIVATE_LIBRARIES
        Qt::Core
)
