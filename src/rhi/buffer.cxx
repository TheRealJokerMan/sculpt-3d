//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/buffer.hxx"
#include "rhi/context.hxx"
#include "rhi/utilities.hxx"

namespace com::rhi
{
    Buffer::Buffer(Context* context, vk::DeviceSize const size, vk::BufferUsageFlags const usageFlags, vk::MemoryPropertyFlags const propertyFlags)
        : m_context(context), m_size(size)
    {
        auto const& device         = m_context->device()->logicalDevice();
        auto const& physicalDevice = m_context->device()->physicalDevice();
        m_buffer                   = device.createBuffer(vk::BufferCreateInfo({}, size, usageFlags));

        auto const memoryRequirements = device.getBufferMemoryRequirements(m_buffer);
        auto const memoryTypeIndex    = findMemoryType(physicalDevice.getMemoryProperties(), memoryRequirements.memoryTypeBits, propertyFlags);

        m_deviceMemory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, memoryTypeIndex));
        device.bindBufferMemory(m_buffer, m_deviceMemory, 0);
    }

    Buffer::~Buffer()
    {
        auto const& device = m_context->device()->logicalDevice();
        device.destroyBuffer(m_buffer);
        device.freeMemory(m_deviceMemory);
    }

    auto Buffer::map() -> void*
    {
        auto const& device = m_context->device()->logicalDevice();
        return device.mapMemory(m_deviceMemory, 0, m_size);
    }

    void Buffer::unmap()
    {
        auto const& device = m_context->device()->logicalDevice();
        device.unmapMemory(m_deviceMemory);
    }

    void Buffer::upload(void const* data, size_t const size)
    {
        auto const& device     = m_context->device()->logicalDevice();
        auto*       deviceData = static_cast<uint8_t*>(device.mapMemory(m_deviceMemory, 0, size));

        std::memcpy(deviceData, data, size);

        device.unmapMemory(m_deviceMemory);
    }
} // namespace com::rhi
