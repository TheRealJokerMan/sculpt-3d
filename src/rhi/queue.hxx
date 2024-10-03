//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace com::rhi
{
    /// Specifies the queue index.
    enum class QueueIndex
    {
        eCompute,  ///< A compute queue.
        eGraphics, ///< A graphics queue.
        ePresent,  ///< A present queue.
        eTransfer  ///< A transfer queue.
    };

    /// Represents a queue.
    class Queue final
    {
    public:
        /// Constructor.
        /// \param device The device.
        /// \param index The index of this queue.
        explicit Queue(class Device const* device, uint32_t index);

        /// Present the image.
        /// \param swapChain The swap chain.
        /// \param frameData The per-frame data.
        /// \return true on success; false otherwise.
        [[nodiscard]] auto present(vk::SwapchainKHR const& swapChain, class FrameData const* frameData) const -> bool;

        /// Submit the queue.
        /// \param commandBuffer The command buffer.
        /// \param frameData The frame data.
        void submit(vk::CommandBuffer const& commandBuffer, class FrameData const* frameData);

        /// Wait for all operations to finish.
        void wait();

    private:
        vk::Device m_device;
        vk::Queue  m_queue;
    };
} // namespace com::rhi
