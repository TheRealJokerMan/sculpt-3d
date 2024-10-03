//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/description.hxx"
#include "rhi/physical-device.hxx"

namespace com::rhi
{
    /// Represents a logical Vulkan device.
    class Device final
    {
    public:
        /// Constructor.
        /// \param context The context.
        /// \param physicalDevice The physical device.
        /// \param description Parameters.
        explicit Device(class Context const* context, PhysicalDevice const* physicalDevice, RhiDescription const& description);

        /// Destructor.
        ~Device();

        /// Create a semaphore.
        /// \return A valid Vuulkan object.
        [[nodiscard]] auto createSemaphore() const -> vk::Semaphore
        {
            return m_device.createSemaphore(vk::SemaphoreCreateInfo());
        }

        /// Accessor.
        /// \return The logicl device.
        [[nodiscard]] auto logicalDevice() const
        {
            return m_device;
        }

        /// Accessor.
        /// \return The physical device.
        [[nodiscard]] auto physicalDevice() const
        {
            return static_cast<vk::PhysicalDevice>(*m_physicalDevice);
        }

        /// Operator.
        operator vk::Device() const
        {
            return m_device;
        }

    private:
        class Context const*  m_context        = nullptr;
        PhysicalDevice const* m_physicalDevice = nullptr;
        vk::Device            m_device;
    };

    /// Find the memory type index.
    /// \param memoryProperties The memory properties.
    /// \param typeBits The type bits.
    /// \param requirementsMask The requirements mask.
    /// \return A valid type index if found; nothing otherwise.
    [[nodiscard]] auto findMemoryTypeIndex(vk::PhysicalDeviceMemoryProperties const& memoryProperties, //
                                           uint32_t const                            typeBits,
                                           vk::MemoryPropertyFlags                   requirementsMask) -> std::optional<uint32_t>;
} // namespace com::rhi
