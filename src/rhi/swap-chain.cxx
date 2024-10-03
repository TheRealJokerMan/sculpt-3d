//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/swap-chain.hxx"
#include "rhi/utilities.hxx"

namespace com::rhi
{
    [[nodiscard]] static auto chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities, vk::Extent2D const& window) -> vk::Extent2D
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            vk::Extent2D actualExtent = window;

            actualExtent.width  = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    [[nodiscard]] static auto getPresentMode(vk::PhysicalDevice const& device, vk::SurfaceKHR const& surface) -> vk::PresentModeKHR
    {
        auto const                            available = device.getSurfacePresentModesKHR(surface);
        std::vector<vk::PresentModeKHR> const preferred = { vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eImmediate };

        auto found = std::ranges::find_first_of(preferred, available);
        return *found;
    }

    SwapChain::SwapChain(Context const* context, vk::SurfaceKHR const& surface, vk::Extent2D const& extent)
        : m_device(context->device()->logicalDevice()), m_surface(surface)
    {
        auto const caps = context->device()->physicalDevice().getSurfaceCapabilitiesKHR(surface);
        m_extent        = chooseSwapExtent(caps, extent);
        m_rect.setExtent(m_extent);

        // Request at least one more image than minImageCount to ensure that double-buffering does work if maxImageCount does allow it.
        uint32_t const backBufferCount = (caps.maxImageCount == 0) ? caps.minImageCount + 1 : std::min(caps.minImageCount + 1, caps.maxImageCount);

        auto const transform =
        (caps.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) ? vk::SurfaceTransformFlagBitsKHR::eIdentity : caps.currentTransform;

        auto const colorFormat         = context->colorFormat();
        auto const swapchainCreateInfo = vk::SwapchainCreateInfoKHR({},
                                                                    surface,
                                                                    backBufferCount,
                                                                    colorFormat,
                                                                    vk::ColorSpaceKHR::eSrgbNonlinear,
                                                                    m_extent,
                                                                    1,
                                                                    vk::ImageUsageFlagBits::eColorAttachment,
                                                                    vk::SharingMode::eExclusive,
                                                                    {},
                                                                    transform,
                                                                    vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                                    getPresentMode(context->device()->physicalDevice(), surface),
                                                                    VK_TRUE,
                                                                    m_swapChain);
        m_swapChain                    = m_device.createSwapchainKHR(swapchainCreateInfo);

        auto const images = m_device.getSwapchainImagesKHR(m_swapChain);
        for (size_t i = 0; i < images.size(); ++i)
        {
            m_colorImages.push_back(std::make_unique<Image>(context->device(), images[i], colorFormat));
        }

        m_depthStencil = std::make_unique<Image>(context->device(), m_extent, context->depthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);
    }

    SwapChain::~SwapChain()
    {
        m_device.destroySwapchainKHR(m_swapChain);
    }

    void SwapChain::acquireNextFrame(FrameData* frameData, uint64_t const timeout)
    {
        vk::ResultValue<uint32_t> result = m_device.acquireNextImageKHR(m_swapChain, timeout, frameData->presentCompleteSemaphore());

        frameData->setImageIndex(result.value);

        m_device.resetFences(frameData->fence());
        frameData->commandPool()->reset();
    }

    auto SwapChain::present(Queue const* queue, FrameData const* frameData) -> bool
    {
        auto const result = queue->present({ m_swapChain }, frameData);

        if (!result)
            return false;

        return true;
    }

} // namespace com::rhi
