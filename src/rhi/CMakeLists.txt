#
# Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
#

# Define the target.
com_library(rhi
    SOURCES
        "buffer.cxx"
        "command-pool.cxx"
        "context.cxx"
        "debug-util.cxx"
        "device.cxx"
        "hit-testing.cxx"
        "image.cxx"
        "mesh.cxx"
        "physical-device.cxx"
        "per-frame-data.cxx"
        "pipeline.cxx"
        "primitive.cxx"
        "queue.cxx"
        "swap-chain.cxx"
        "utilities.cxx"

    PUBLIC_COMPILE_DEFINITIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        $<$<PLATFORM_ID:Darwin>:VK_ENABLE_BETA_EXTENSIONS>
        $<$<PLATFORM_ID:Darwin>:VK_USE_PLATFORM_METAL_EXT>
        $<$<PLATFORM_ID:Darwin>:SURFACE_EXTENSION_NAME=VK_EXT_METAL_SURFACE_EXTENSION_NAME>

        $<$<PLATFORM_ID:Linux>:VK_USE_PLATFORM_XCB_KHR>
        $<$<PLATFORM_ID:Linux>:SURFACE_EXTENSION_NAME=VK_KHR_XCB_SURFACE_EXTENSION_NAME>

        $<$<PLATFORM_ID:Windows>:VK_USE_PLATFORM_WIN32_KHR>
        $<$<PLATFORM_ID:Windows>:SURFACE_EXTENSION_NAME=VK_KHR_WIN32_SURFACE_EXTENSION_NAME>

    PUBLIC_LIBRARIES
        glm::glm
        Vulkan::Vulkan

    PRIVATE_LIBRARIES
        com::base
)

add_subdirectory("shaders")
add_dependencies(com.rhi com.rhi.shaders)
