//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/device.hxx"

namespace com::rhi
{
    /// Represents a command pool.
    class CommandPool final
    {
    public:
        /// Constructor.
        /// \param device The Vulkan device.
        /// \param queueIndex The queue index to use.
        explicit CommandPool(Device const* device, uint32_t const queueIndex);

        /// Destructor.
        ~CommandPool();

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto commandBuffer() const -> vk::CommandBuffer const&
        {
            return m_commandBuffer;
        }

        /// Reset the command pool.
        void reset()
        {
            m_device.resetCommandPool(m_handle);
        }

    private:
        vk::Device        m_device;
        vk::CommandPool   m_handle;
        vk::CommandBuffer m_commandBuffer;
    };
} // namespace com::rhi
