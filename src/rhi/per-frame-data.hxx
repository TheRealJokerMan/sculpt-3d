//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/buffer.hxx"
#include "rhi/command-pool.hxx"

namespace com::rhi
{
    /// Per-frame data.
    class FrameData final
    {
    public:
        /// Constructor.
        /// \param context The RHI context.
        /// \param queueIndex The queue index to use.
        explicit FrameData(class Context* context, uint32_t const queueIndex);

        /// Destructor.
        ~FrameData();

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto cameraUniformBuffer() const
        {
            return m_cameraUniformBuffer.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto commandBuffer() const -> vk::CommandBuffer const&
        {
            return m_commandPool->commandBuffer();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto commandPool() const
        {
            return m_commandPool.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto fence() const
        {
            return m_fence;
        }

        /// Accessor.
        /// \return A valid integer.
        [[nodiscard]] auto imageIndex() const -> uint32_t const&
        {
            return m_imageIndex;
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto modelUniformBuffer() const
        {
            return m_modelUniformBuffer.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto mouseBuffer() const
        {
            return m_mouseBuffer.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto presentCompleteSemaphore() const -> vk::Semaphore const&
        {
            return m_presentCompleteSemaphore;
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto renderCompleteSemaphore() const -> vk::Semaphore const&
        {
            return m_renderCompleteSemaphore;
        }

        /// Set the image index.
        /// \param index A valid integer.
        void setImageIndex(uint32_t const index)
        {
            m_imageIndex = index;
        }

    private:
        std::unique_ptr<CommandPool> m_commandPool;
        vk::Device                   m_device;
        vk::Fence                    m_fence;
        vk::Semaphore                m_presentCompleteSemaphore;
        vk::Semaphore                m_renderCompleteSemaphore;
        uint32_t                     m_imageIndex = 0;
        std::unique_ptr<Buffer>      m_cameraUniformBuffer;
        std::unique_ptr<Buffer>      m_modelUniformBuffer;
        std::unique_ptr<Buffer>      m_mouseBuffer;
    };
} // namespace com::rhi
