//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/debug-util.hxx"
#include "rhi/description.hxx"
#include "rhi/device.hxx"
#include "rhi/queue.hxx"

#include <set>

namespace com::rhi
{
    /// An instance of Vulkan.
    class Context final
    {
    public:
        /// Constructor.
        /// \param description Parameters to create a new Vulkan instance.
        explicit Context(RhiDescription const& description);

        /// Destructor.
        ~Context();

        /// Advance to the next per-frame data.
        void advanceNextFrame()
        {
            m_currentFrameIndex = (m_currentFrameIndex + 1) % m_perFrameData.size();
        }

        /// Allocate the per-frame data.
        /// \param count The number of entries to allocate.
        void allocatePerFrameData(uint32_t const count);

        /// Get the color format.
        /// \return A format.
        [[nodiscard]] auto colorFormat() const
        {
            return m_colorFormat;
        }

        /// Get the depth format.
        /// \return A format.
        [[nodiscard]] auto depthFormat() const
        {
            return m_depthFormat;
        }

        /// Accessor.
        /// \return The logical device.
        [[nodiscard]] auto device() const
        {
            return m_device.get();
        }

        /// Get the current per-frame data.
        /// \return A valid pointer.
        [[nodiscard]] auto frameData() const
        {
            return m_perFrameData[m_currentFrameIndex].get();
        }

        /// Accessor.
        /// \return The instance.
        [[nodiscard]] auto handle() const
        {
            return m_instance;
        }

        /// React to the application terminating.
        void onTerminating();

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto pipelineCache() const
        {
            return m_pipelineCache;
        }

        /// Accessor.
        /// \return The graphics queue.
        [[nodiscard]] auto queue(QueueIndex const type) const
        {
            switch (type)
            {
            case QueueIndex::eCompute:
                return m_computeQueue.get();
            case QueueIndex::ePresent:
                return m_presentQueue.get();
            case QueueIndex::eTransfer:
                return m_transferQueue.get();
            default:
                return m_graphicsQueue.get();
            }
        }

        /// Accessor.
        /// \return The graphics queue.
        [[nodiscard]] auto queueIndex(QueueIndex const type) const
        {
            switch (type)
            {
            case QueueIndex::eCompute:
                return m_computeQueueIndex;
            case QueueIndex::ePresent:
                return m_presentQueueIndex;
            case QueueIndex::eTransfer:
                return m_transferQueueIndex;
            default:
                return m_graphicsQueueIndex;
            }
        }

        /// Accessor.
        /// \return A set of queue indices.
        [[nodiscard]] auto queueIndices() const -> std::set<uint32_t>;

        /// Accessor.
        /// \return The presentation surface.
        [[nodiscard]] auto surface() const
        {
            return m_surface;
        }

        /// Wait for a set of fences to be signaled.
        /// \param fence The fence to wait on.
        /// \param waitAll true if all should be waited on; false otherwise.
        /// \param timeout A timeout value.
        void waitForFences(vk::Fence const& fence,
                           vk::Bool32 const waitAll = VK_TRUE, //
                           uint64_t const   timeout = std::numeric_limits<uint64_t>::max());

        /// Wait for the device to become idle.
        void waitForIdle();

    private:
        void createDebugUtility();
        void createSurface(void const* connection, void const* display);

    private:
        vk::Instance                    m_instance;
        std::unique_ptr<DebugUtil>      m_debugUtil;
        vk::SurfaceKHR                  m_surface;
        std::unique_ptr<PhysicalDevice> m_physicalDevice;
        vk::Format                      m_colorFormat;
        vk::Format                      m_depthFormat;
        uint32_t                        m_computeQueueIndex  = 0;
        uint32_t                        m_graphicsQueueIndex = 0;
        uint32_t                        m_presentQueueIndex  = 0;
        uint32_t                        m_transferQueueIndex = 0;
        std::unique_ptr<Device>         m_device;
        std::unique_ptr<Queue>          m_computeQueue;
        std::unique_ptr<Queue>          m_graphicsQueue;
        std::unique_ptr<Queue>          m_presentQueue;
        std::unique_ptr<Queue>          m_transferQueue;
        vk::PipelineCache               m_pipelineCache;

        std::vector<std::unique_ptr<FrameData>> m_perFrameData;
        uint32_t                                m_currentFrameIndex = 0;
    };

} // namespace com::rhi
