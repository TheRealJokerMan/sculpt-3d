#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

if(BUILD_DOCS)
    # Set Doxygen defaults.
    set(DOXYGEN_EXCLUDE              "${CMAKE_SOURCE_DIR}/src/rhi/shaders/uniforms.hxx")
    set(DOXYGEN_GENERATE_LATEX       NO)
    set(DOXYGEN_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
    set(DOXYGEN_QUIET                YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR    YES)
    set(DOXYGEN_WARN_AS_ERROR        YES)

    # State which files require Doxygen processing.
    file(GLOB_RECURSE DOC_MARKDOWN   "${CMAKE_SOURCE_DIR}/docs/*.md")
    file(GLOB_RECURSE SRC_HEADERS    "${CMAKE_SOURCE_DIR}/src/*.hxx")
    file(GLOB_RECURSE SRC_MARKDOWN   "${CMAKE_SOURCE_DIR}/src/*.md")
    set(DOXYGEN_SOURCES              ${DOC_MARKDOWN} ${SRC_HEADERS} ${SRC_MARKDOWN})

    # Define the Doxygen target.
    doxygen_add_docs(doxygen ${DOXYGEN_SOURCES} ALL USE_STAMP_FILE COMMENT "Generating documentation.")

    # Install a shortcut to the documentation.
    install(CODE [[execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/docs/doxygen/html/index.html ${CMAKE_INSTALL_PREFIX}/Documentation.html)]])
endif(BUILD_DOCS)
