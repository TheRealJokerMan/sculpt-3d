//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/context.hxx"
#include "rhi/device.hxx"
#include "rhi/image.hxx"
#include "rhi/per-frame-data.hxx"
#include "rhi/queue.hxx"

namespace com::rhi
{
    /// A collection of framebuffers for rendering into.
    class SwapChain final
    {
    public:
        /// Constructor.
        /// \param context The RHI context.
        /// \param surface The presentation surface.
        /// \param extent The window's extent.
        explicit SwapChain(Context const* context, vk::SurfaceKHR const& surface, vk::Extent2D const& extent);

        /// Destructor.
        ~SwapChain();

        /// Get the index of the next available swapchain image.
        /// \param frameData The per-frame data.
        /// \param timeout The timeout value.
        void acquireNextFrame(FrameData* frameData, uint64_t const timeout = std::numeric_limits<uint64_t>::max());

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto depthStencil()
        {
            return m_depthStencil.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto extent()
        {
            return m_extent;
        }

        /// Accessor.
        /// \param index A valid integer.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto image(uint32_t const index)
        {
            return m_colorImages[index].get();
        }

        /// Get the number of back buffers.
        /// \return A valid integer.
        [[nodiscard]] auto numImages() const
        {
            return static_cast<uint32_t>(m_colorImages.size());
        }

        /// Present the image.
        /// \param queue The queue to use.
        /// \param frameData Per-frame data.
        /// \return true on success; false otherwise.
        [[nodiscard]] auto present(Queue const* queue, FrameData const* frameData) -> bool;

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto rect()
        {
            return m_rect;
        }

    private:
        vk::Device       m_device;
        vk::SurfaceKHR   m_surface;
        vk::Extent2D     m_extent;
        vk::Rect2D       m_rect;
        vk::SwapchainKHR m_swapChain;

        std::vector<std::unique_ptr<Image>> m_colorImages;
        std::unique_ptr<Image>              m_depthStencil;
    };
} // namespace com::rhi
