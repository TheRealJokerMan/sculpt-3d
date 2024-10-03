//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/device.hxx"

namespace com::rhi
{
    /// Represents an image.
    class Image
    {
    public:
        /// Defines the usage of the image.
        enum class Usage
        {
            eUndefined,           ///< Not specified.
            ePresent,             ///< Presentation.
            eTransferDst,         ///< A destination.
            eTransferSrc,         ///< A source.
            eAttachmentReadOnly,  ///< A read-only attachment.
            eAttachmentWriteOnly, ///< A write-only attachment.
            eAttachmentReadWrite, ///< An attachment.
        };

    public:
        /// Constructor - managed.
        /// \param device The Vulkan device.
        /// \param extent A image extent.
        /// \param format The format of the image.
        /// \param usage The usage.
        explicit Image(Device const* device, vk::Extent2D const& extent, vk::Format format, vk::ImageUsageFlags const usage);

        /// Constructor - unmanaged.
        /// \param device The Vulkan device.
        /// \param image An image.
        /// \param format The format of the image.
        explicit Image(Device const* device, vk::Image const& image, vk::Format format);

        /// Destructor.
        ~Image();

        /// Get the image as a Vulkan object.
        /// \return A Vulkan object.
        [[nodiscard]] auto asRenderingAttachmentInfo() const -> vk::RenderingAttachmentInfo;

        /// Copy a pixel.
        /// \param x The horizontal offset.
        /// \param y The vertical offset.
        /// \param offset The offset into the target buffer.
        /// \param commandBuffer The command buffer.
        /// \param destination The target memory.
        void copyPixel(uint32_t const x, uint32_t const y, vk::DeviceSize const& offset, vk::CommandBuffer const& commandBuffer, class Buffer* destination) const;

        /// Accessor.
        /// \return The image view.
        [[nodiscard]] auto imageView() const
        {
            return m_view;
        }

        /// Accessor.
        /// \return The image' view's lyaout.
        [[nodiscard]] auto layout() const -> vk::ImageLayout;

        /// Set the image to a given state.
        /// \param newUsage The new usage.
        void setUsage(Usage const newUsage)
        {
            m_currentUsage = newUsage;
        }

        /// Transition the image to a new state.
        /// \param newUsage The new usage.
        /// \param commandBuffer The command buffer.
        void transition(Usage const newUsage, vk::CommandBuffer const& commandBuffer);

    private:
        [[nodiscard]] auto getMemoryRequirements() const -> vk::MemoryRequirements;

    protected:
        vk::Device           m_device;            ///< The Vulkan device.
        bool                 m_isManaged = false; ///< Determines if the resources should be freed upon destruction.
        vk::Image            m_image;             ///< The image.
        vk::Format           m_format;            ///< The format.
        vk::ImageAspectFlags m_aspect;            ///< The aspect.
        Usage                m_currentUsage;      ///< The current usage.
        vk::ImageView        m_view;              ///< The view.
        vk::DeviceMemory     m_memory;            ///< The memory backing the buffer.s
    };

} // namespace com::rhi
