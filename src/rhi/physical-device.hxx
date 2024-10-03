//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/description.hxx"

#include <memory>

namespace com::rhi
{
    /// Represents a physical Vulkan device, i.e., a GPU.
    class PhysicalDevice final
    {
    public:
        /// Constructor.
        /// \param context The Vulkan context.
        /// \param device The physical Vulkan device.
        explicit PhysicalDevice(class Context const* context, vk::PhysicalDevice const& device);

        /// Find a queue index.
        /// \param bit The queue to look for.
        /// \param surface An optional surface.
        /// \return A queue index if found or zero.
        [[nodiscard]] auto findQueueIndex(vk::QueueFlagBits const bit, vk::SurfaceKHR const* surface) -> uint32_t;

        /// Pick the "best" device.
        /// \param instance The Vulkan instance.
        /// \param surface The Vulkan surface.
        /// \param desc Parameters.
        /// \return The "best" device, if any; nothing if no devices where suitable.
        [[nodiscard]] static auto pick(Context const* instance, vk::SurfaceKHR const& surface, RhiDescription const& desc) -> std::unique_ptr<PhysicalDevice>;

        /// Accessor.
        /// \return The physical device properties.
        [[nodiscard]] auto properties() const
        {
            return m_properties;
        }

        /// Accessor.
        /// \return The queue family properties.
        [[nodiscard]] auto queueFamily() const
        {
            return m_queueFamily;
        }

        /// Determines if this device supports a set of extensions.
        /// \param extensions The extensions to test.
        /// \return true if this device supports a set of extensions; false othewise.
        [[nodiscard]] auto supportsExtensions(vk::ArrayProxy<std::string const> const& extensions) const -> bool;

        /// Determines if this device supports a set of features.
        /// \param features The features to test.
        /// \return true if this device supports a set of features; false othewise.
        [[nodiscard]] auto supportsFeatures(vk::PhysicalDeviceFeatures const& features) const -> bool;

        /// Determines if this device supports presentation.
        /// \param index The queue family index to test.
        /// \param windowHandle The window handle.
        /// \return true if this device supports presentation; false othewise.
        [[nodiscard]] auto supportsPresentation(uint32_t const index, void const* windowHandle) const -> bool;

        /// Determines if this device supports a given surface.
        /// \param index The queue family index to test.
        /// \param surface The surface to test.
        /// \return true if this device supports a given surface; false othewise.
        [[nodiscard]] auto supportsSurface(uint32_t const index, vk::SurfaceKHR const& surface) const -> bool
        {
            return m_device.getSurfaceSupportKHR(index, surface);
        }

        /// Get the available surface formats.
        /// \param surface The presentation surface.
        /// \return A collection of formats.
        [[nodiscard]] auto surfaceFormats(vk::SurfaceKHR const& surface) const -> std::vector<vk::SurfaceFormatKHR>;

        /// Operator.
        operator vk::PhysicalDevice() const
        {
            return m_device;
        }

    private:
        Context const*     m_context = nullptr;
        vk::PhysicalDevice m_device;

        std::vector<vk::ExtensionProperties>   m_extensions;
        vk::PhysicalDeviceFeatures             m_features;
        vk::PhysicalDeviceMemoryProperties     m_memory;
        vk::PhysicalDeviceProperties           m_properties;
        std::vector<vk::QueueFamilyProperties> m_queueFamily;
    };
} // namespace com::rhi
