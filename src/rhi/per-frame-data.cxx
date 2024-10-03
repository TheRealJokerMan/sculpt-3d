//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/per-frame-data.hxx"
#include "rhi/context.hxx"
#include "rhi/shaders/uniforms.hxx"

namespace com::rhi
{
    FrameData::FrameData(Context* context, uint32_t const queueIndex) : m_device(context->device()->logicalDevice())
    {
        m_commandPool = std::make_unique<CommandPool>(context->device(), queueIndex);

        m_presentCompleteSemaphore = context->device()->createSemaphore();
        m_renderCompleteSemaphore  = context->device()->createSemaphore();

        m_fence = m_device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));

        BufferDescription bufferDesc;
        bufferDesc.flags = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;

        bufferDesc.size       = sizeof(CameraUniform);
        m_cameraUniformBuffer = std::make_unique<Buffer>(context, bufferDesc.size, bufferDesc.flags);

        bufferDesc.size      = sizeof(glm::mat4x4);
        m_modelUniformBuffer = std::make_unique<Buffer>(context, bufferDesc.size, bufferDesc.flags);

        bufferDesc.size  = 8 * sizeof(float);
        bufferDesc.flags = vk::BufferUsageFlagBits::eTransferDst;
        m_mouseBuffer    = std::make_unique<Buffer>(context, bufferDesc.size, bufferDesc.flags);
    }

    FrameData::~FrameData()
    {
        m_mouseBuffer.reset();
        m_modelUniformBuffer.reset();
        m_cameraUniformBuffer.reset();

        m_device.destroyFence(m_fence);
        m_device.destroySemaphore(m_renderCompleteSemaphore);
        m_device.destroySemaphore(m_presentCompleteSemaphore);

        m_commandPool.reset();
    }

} // namespace com::rhi
