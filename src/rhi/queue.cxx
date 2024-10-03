//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/queue.hxx"
#include "rhi/device.hxx"
#include "rhi/per-frame-data.hxx"

namespace com::rhi
{
    Queue::Queue(Device const* device, uint32_t index) : m_device(device->logicalDevice())
    {
        m_queue = m_device.getQueue(index, 0);
    }

    auto Queue::present(vk::SwapchainKHR const& swapChain, FrameData const* frameData) const -> bool
    {
        vk::PresentInfoKHR info(frameData->renderCompleteSemaphore(), swapChain, frameData->imageIndex());

        auto const result = m_queue.presentKHR(info);

        return result == vk::Result::eSuccess;
    }

    void Queue::submit(vk::CommandBuffer const& commandBuffer, FrameData const* frameData)
    {
        vk::PipelineStageFlags const waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        vk::SubmitInfo const info(frameData->presentCompleteSemaphore(), waitDstStageMask, commandBuffer, frameData->renderCompleteSemaphore());

        m_queue.submit(info, frameData->fence());
    }

    void Queue::wait()
    {
        m_queue.waitIdle();
    }
} // namespace com::rhi
