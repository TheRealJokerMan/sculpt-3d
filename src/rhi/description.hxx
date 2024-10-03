//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string_view>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace com::rhi
{
    /// Parameters to create a new Vulkan instance.
    struct RhiDescription
    {
        std::string_view           name;                       ///< The application name.
        uint32_t                   version;                    ///< The application version.
        std::vector<std::string>   instanceLayers;             ///< Required layers.
        std::vector<std::string>   instanceExtensions;         ///< Required extensions.
        std::vector<std::string>   deviceLayers;               ///< Required layers.
        std::vector<std::string>   deviceExtensions;           ///< Required extensions.
        vk::PhysicalDeviceFeatures deviceFeatures;             ///< Device features.
        void const*                windowHandle     = nullptr; ///< A handle to the underlying window.
        void const*                windowConnection = nullptr; ///< The X connection of the application, for use with XCB.
    };
} // namespace com::rhi
