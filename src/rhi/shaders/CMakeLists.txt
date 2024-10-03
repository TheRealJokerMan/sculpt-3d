#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

# Create a custom target that will depend on the source shaders. This target will become a dependency of the RHI module,
# enabling the shaders to be recompiled when they change.
add_custom_target(com.rhi.shaders)
configure_file("shaders.qrc.in" "${PROJECT_BINARY_DIR}/shaders.qrc")


function(compile_shader fileName)
    # Compute the absolute path of the source file.
    cmake_path(ABSOLUTE_PATH fileName BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}     NORMALIZE OUTPUT_VARIABLE shaderSource)

    # Compute the absolute path of the destination file.
    cmake_path(ABSOLUTE_PATH fileName BASE_DIRECTORY "${PROJECT_BINARY_DIR}/shaders" NORMALIZE OUTPUT_VARIABLE compiledShader)

    # Add a rule to compile the shader.
    add_custom_command(
        OUTPUT
            ${compiledShader}
        COMMAND
            "${CMAKE_COMMAND}" -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND
            Vulkan::glslc ${shaderSource} -o ${compiledShader} -fentry-point=main
        DEPENDS
            ${shaderSource}
        COMMENT
            "Compiling ${fileName} shader."
    )

    # Create a dependency on the RHI's dependency on the compiled shader.
    add_custom_target(shader_${fileName} DEPENDS ${compiledShader})
    add_dependencies(com.rhi.shaders shader_${fileName})
endfunction(compile_shader)


compile_shader("cursor.frag")
compile_shader("cursor.vert")
compile_shader("hit-test.frag")
compile_shader("hit-test.vert")
compile_shader("model.frag")
compile_shader("model.vert")
compile_shader("process.comp")
