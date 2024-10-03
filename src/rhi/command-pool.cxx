//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/command-pool.hxx"

namespace com::rhi
{
    CommandPool::CommandPool(Device const* device, uint32_t const queueIndex) : m_device(device->logicalDevice())
    {
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueIndex);
        m_handle = m_device.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo info(m_handle, vk::CommandBufferLevel::ePrimary, 1);
        m_commandBuffer = m_device.allocateCommandBuffers(info)[0];
    }

    CommandPool::~CommandPool()
    {
        m_device.freeCommandBuffers(m_handle, m_commandBuffer);
        m_device.destroyCommandPool(m_handle);
    }
} // namespace com::rhi
