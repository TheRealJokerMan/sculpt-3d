//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/image.hxx"
#include "rhi/buffer.hxx"
#include "rhi/utilities.hxx"

namespace com::rhi
{
    [[nodiscard]] static auto lastUsageToStage(Image::Usage const usage, vk::ImageAspectFlags const aspect)
    {
        vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eNone;

        switch (usage)
        {
        case Image::Usage::eUndefined:
        case Image::Usage::ePresent:
            stage = vk::PipelineStageFlagBits2::eNone;
            break;

        case Image::Usage::eTransferDst:
        case Image::Usage::eTransferSrc:
            stage = vk::PipelineStageFlagBits2::eTransfer;
            break;

        case Image::Usage::eAttachmentReadOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                stage = vk::PipelineStageFlagBits2::eFragmentShader;
            else
                stage = vk::PipelineStageFlagBits2::eLateFragmentTests;
            break;

        case Image::Usage::eAttachmentWriteOnly:
        case Image::Usage::eAttachmentReadWrite:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            else
                stage = vk::PipelineStageFlagBits2::eLateFragmentTests;
            break;
        }

        return stage;
    }

    [[nodiscard]] static auto usageToAccess(Image::Usage const usage, vk::ImageAspectFlags const aspect)
    {
        vk::AccessFlags2 access = vk::AccessFlagBits2::eNone;

        switch (usage)
        {
        case Image::Usage::eUndefined:
        case Image::Usage::ePresent:
            access = vk::AccessFlagBits2::eNone;
            break;

        case Image::Usage::eTransferDst:
            access = vk::AccessFlagBits2::eTransferWrite;
            break;

        case Image::Usage::eTransferSrc:
            access = vk::AccessFlagBits2::eTransferRead;
            break;

        case Image::Usage::eAttachmentReadOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                access = vk::AccessFlagBits2::eColorAttachmentRead;
            else
                access = vk::AccessFlagBits2::eDepthStencilAttachmentRead;
            break;

        case Image::Usage::eAttachmentWriteOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                access = vk::AccessFlagBits2::eColorAttachmentWrite;
            else
                access = vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            break;

        case Image::Usage::eAttachmentReadWrite:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                access = vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite;
            else
                access = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            break;
        }

        return access;
    }

    [[nodiscard]] static auto firstUsageToStage(Image::Usage const usage, vk::ImageAspectFlags const aspect)
    {
        vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eNone;

        switch (usage)
        {
        case Image::Usage::eUndefined:
        case Image::Usage::ePresent:
            stage = vk::PipelineStageFlagBits2::eNone;
            break;

        case Image::Usage::eTransferDst:
        case Image::Usage::eTransferSrc:
            stage = vk::PipelineStageFlagBits2::eTransfer;
            break;

        case Image::Usage::eAttachmentReadOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                stage = vk::PipelineStageFlagBits2::eFragmentShader;
            else
                stage = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
            break;

        case Image::Usage::eAttachmentReadWrite:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                stage = vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            else
                stage = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
            break;

        case Image::Usage::eAttachmentWriteOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            else
                stage = vk::PipelineStageFlagBits2::eLateFragmentTests;
            break;
        }

        return stage;
    }

    [[nodiscard]] static auto usageToLayout(Image::Usage const usage, vk::ImageAspectFlags const aspect)
    {
        vk::ImageLayout layout = vk::ImageLayout::eUndefined;

        switch (usage)
        {
        case Image::Usage::eUndefined:
            layout = vk::ImageLayout::eUndefined;
            break;

        case Image::Usage::ePresent:
            layout = vk::ImageLayout::ePresentSrcKHR;
            break;

        case Image::Usage::eTransferDst:
            layout = vk::ImageLayout::eTransferDstOptimal;
            break;

        case Image::Usage::eTransferSrc:
            layout = vk::ImageLayout::eTransferSrcOptimal;
            break;

        case Image::Usage::eAttachmentReadOnly:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                layout = vk::ImageLayout::eColorAttachmentOptimal;
            else
                layout = vk::ImageLayout::eDepthReadOnlyOptimal;
            break;

        case Image::Usage::eAttachmentWriteOnly:
        case Image::Usage::eAttachmentReadWrite:
            if (aspect == vk::ImageAspectFlagBits::eColor)
                layout = vk::ImageLayout::eColorAttachmentOptimal;
            else
                layout = vk::ImageLayout::eDepthAttachmentOptimal;
            break;
        }

        return layout;
    }

    Image::Image(Device const* device, vk::Extent2D const& extent, vk::Format format, vk::ImageUsageFlags const usage)
        : m_device(device->logicalDevice()), m_isManaged(true), m_format(format), m_currentUsage(Usage::eUndefined)
    {
        vk::ImageCreateInfo info({},
                                 vk::ImageType::e2D,
                                 m_format,
                                 vk::Extent3D(extent, 1),
                                 1,
                                 1,
                                 vk::SampleCountFlagBits::e1,
                                 vk::ImageTiling::eOptimal,
                                 usage,
                                 vk::SharingMode::eExclusive,
                                 0,
                                 nullptr,
                                 vk::ImageLayout::eUndefined);

        m_image = m_device.createImage(info);

        auto const memReqs = getMemoryRequirements();
        auto const index = findMemoryTypeIndex(device->physicalDevice().getMemoryProperties(), memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

        m_memory = m_device.allocateMemory(vk::MemoryAllocateInfo(memReqs.size, *index));
        m_device.bindImageMemory(m_image, m_memory, 0);

        m_aspect            = isDepthFormat(m_format) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        auto const subRange = vk::ImageSubresourceRange(m_aspect, 0, 1, 0, 1);
        m_view              = m_device.createImageView(vk::ImageViewCreateInfo({}, m_image, vk::ImageViewType::e2D, format, {}, subRange));
    }

    Image::Image(Device const* device, vk::Image const& image, vk::Format format)
        : m_device(device->logicalDevice()), m_image(image), m_format(format), m_currentUsage(Usage::eUndefined)
    {
        m_aspect            = vk::ImageAspectFlagBits::eColor;
        auto const subRange = vk::ImageSubresourceRange(m_aspect, 0, 1, 0, 1);
        m_view              = m_device.createImageView(vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, format, {}, subRange));
    }

    Image::~Image()
    {
        m_device.destroyImageView(m_view);

        if (m_isManaged)
        {
            m_device.destroyImage(m_image);
            m_device.freeMemory(m_memory);
        }
    }

    auto Image::asRenderingAttachmentInfo() const -> vk::RenderingAttachmentInfo
    {
        auto attachment = vk::RenderingAttachmentInfo(imageView(), layout());
        attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        attachment.setStoreOp(vk::AttachmentStoreOp::eStore);

        return attachment;
    }

    void Image::copyPixel(uint32_t const x, uint32_t const y, vk::DeviceSize const& offset, vk::CommandBuffer const& commandBuffer, class Buffer* destination) const
    {
        auto const imageSubresource = vk::ImageSubresourceLayers(m_aspect, 0, 0, 1);
        auto const imageOffset      = vk::Offset3D(x, y, 0);
        auto const imageExtent      = vk::Extent3D(1, 1, 1);
        auto const region           = vk::BufferImageCopy(offset, {}, {}, imageSubresource, imageOffset, imageExtent);

        commandBuffer.copyImageToBuffer(m_image, layout(), destination->buffer(), region);
    }

    auto Image::layout() const -> vk::ImageLayout
    {
        return usageToLayout(m_currentUsage, m_aspect);
    }

    void Image::transition(Usage const newUsage, vk::CommandBuffer const& commandBuffer)
    {
        auto const subRange = vk::ImageSubresourceRange(m_aspect, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS);
        auto       barrier  = vk::ImageMemoryBarrier2KHR(lastUsageToStage(m_currentUsage, m_aspect),
                                                  usageToAccess(m_currentUsage, m_aspect),
                                                  firstUsageToStage(newUsage, m_aspect),
                                                  usageToAccess(newUsage, m_aspect),
                                                  usageToLayout(m_currentUsage, m_aspect),
                                                  usageToLayout(newUsage, m_aspect),
                                                         {},
                                                         {},
                                                  m_image,
                                                  subRange);

        m_currentUsage = newUsage;

        commandBuffer.pipelineBarrier2KHR(vk::DependencyInfo({}, {}, {}, barrier));
    }

    auto Image::getMemoryRequirements() const -> vk::MemoryRequirements
    {
        return m_device.getImageMemoryRequirements(m_image);
    }

} // namespace com::rhi
