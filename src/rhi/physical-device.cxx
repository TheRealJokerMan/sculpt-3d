//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/physical-device.hxx"
#include "rhi/context.hxx"

#include <map>

namespace com::rhi
{
    PhysicalDevice::PhysicalDevice(Context const* context, vk::PhysicalDevice const& device) : m_context(context), m_device(device)
    {
        m_extensions  = m_device.enumerateDeviceExtensionProperties();
        m_features    = m_device.getFeatures();
        m_memory      = m_device.getMemoryProperties();
        m_properties  = m_device.getProperties();
        m_queueFamily = m_device.getQueueFamilyProperties();
    }

    auto PhysicalDevice::findQueueIndex(vk::QueueFlagBits bit, vk::SurfaceKHR const* surface) -> uint32_t
    {
        auto const properties = m_device.getQueueFamilyProperties();

        auto functor = [properties, bit](vk::QueueFamilyProperties const& family) { return (family.queueFlags & bit) == bit; };

        if (auto it = std::ranges::find_if(properties, functor); it != properties.end())
        {
            auto const index = static_cast<uint32_t>(std::distance(properties.begin(), it));

            if (surface && !m_device.getSurfaceSupportKHR(index, *surface))
            {
                return 0;
            }

            return index;
        }

        return 0;
    }

    auto PhysicalDevice::pick(Context const* context, vk::SurfaceKHR const& surface, RhiDescription const& description) -> std::unique_ptr<PhysicalDevice>
    {
        using PhysDevicePtr = std::unique_ptr<PhysicalDevice>;
        using RankedMap     = std::multimap<int32_t, PhysDevicePtr, std::greater<int32_t>>;

        enum RankPriority
        {
            High = 10,
            Low  = 1,
        };

        auto const physicalDevices = context->handle().enumeratePhysicalDevices();
        RankedMap  rankedDevices;

        for (auto const physicalDevice : physicalDevices)
        {
            auto device = std::make_unique<PhysicalDevice>(context, physicalDevice);

            if (!device->supportsFeatures(description.deviceFeatures)) // Check if all required features are supported.
                continue;

            if (!device->supportsExtensions(description.deviceExtensions)) // Check if all required extensions are supported.
                continue;

            auto rank = 0;

            // Rank discrete GPUs higher.
            switch (device->properties().deviceType)
            {
            case vk::PhysicalDeviceType::eDiscreteGpu:
                rank += RankPriority::High;
                break;

            case vk::PhysicalDeviceType::eIntegratedGpu:
                rank += RankPriority::Low;
                break;

            default:
                break;
            }

            auto const queueFamilyProperties = device->queueFamily();
            for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyProperties.size(); ++queueFamilyIndex)
            {
                const auto& properties = queueFamilyProperties[queueFamilyIndex];

                // vk::QueueFlagBits::eTransfer is implied for graphics capable queue families.
                if (!(properties.queueFlags & (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)))
                    continue;

                if (!device->supportsPresentation(queueFamilyIndex, description.windowHandle))
                    continue;

                if (!device->supportsSurface(queueFamilyIndex, surface))
                    continue;

                rankedDevices.insert(std::make_pair(rank, std::move(device)));
                break;
            }
        }

        if (rankedDevices.empty())
        {
            throw std::runtime_error("Failed to find suitable Vulkan physical device.");
        }

        return std::move(rankedDevices.begin()->second);
    }

    auto PhysicalDevice::supportsExtensions(vk::ArrayProxy<std::string const> const& extensions) const -> bool
    {
        for (auto const& extension : extensions)
        {
            auto const find = std::ranges::find_if(m_extensions, [extension](auto const& e) { return std::strcmp(e.extensionName, extension.data()) == 0; });

            if (find == m_extensions.end())
                return false;
        }

        return true;
    }

    auto PhysicalDevice::supportsFeatures(vk::PhysicalDeviceFeatures const& features) const -> bool
    {
        for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
        {
            if (reinterpret_cast<VkBool32 const*>(&features)[i] == VK_TRUE && reinterpret_cast<VkBool32 const*>(&m_features)[i] == VK_FALSE)
            {
                return false;
            }
        }

        return true;
    }

    auto PhysicalDevice::supportsPresentation(uint32_t index, [[maybe_unused]] void const* windowHandle) const -> bool
    {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        return m_device.getWin32PresentationSupportKHR(index) == vk::True;
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        auto* x11app = qApp->nativeInterface<QNativeInterface::QX11Application>();
        auto* connection = x11app->connection();

        return m_device.getXcbPresentationSupportKHR(index, connection, static_cast<xcb_visualid_t>(reinterpret_cast<intptr_t>(windowHandle))) == vk::True;
#else
        return true;
#endif
    }

    auto PhysicalDevice::surfaceFormats(vk::SurfaceKHR const& surface) const -> std::vector<vk::SurfaceFormatKHR>
    {
        return m_device.getSurfaceFormatsKHR(surface);
    }

} // namespace com::rhi
