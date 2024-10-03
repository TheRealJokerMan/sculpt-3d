//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/device.hxx"
#include "rhi/context.hxx"

namespace com::rhi
{
    Device::Device(Context const* context, PhysicalDevice const* physicalDevice, RhiDescription const& description)
        : m_context(context), m_physicalDevice(physicalDevice)
    {
        auto const                             queueIndicesSet = m_context->queueIndices();
        std::array<float, 1>                   queueProperties = { 0.0f };
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

        for (auto queueIndex : queueIndicesSet)
            queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, queueIndex, queueProperties));

        std::vector<char const*> extensions;
        std::ranges::for_each(description.deviceExtensions, [&](auto& e) { extensions.emplace_back(e.c_str()); });

        std::vector<char const*> layers;
        std::ranges::for_each(description.deviceLayers, [&](auto& l) { layers.emplace_back(l.c_str()); });

        auto deviceFeatures           = description.deviceFeatures;
        auto dynamicRenderingFeatures = vk::PhysicalDeviceDynamicRenderingFeatures(true);
        auto featureSynchronization2  = vk::PhysicalDeviceSynchronization2Features(true, &dynamicRenderingFeatures);
        auto queryResetFeatures       = vk::PhysicalDeviceHostQueryResetFeatures(true, &featureSynchronization2);

        auto const info = vk::DeviceCreateInfo({}, queueCreateInfos, layers, extensions, &deviceFeatures, &queryResetFeatures);
        m_device        = static_cast<vk::PhysicalDevice>(*m_physicalDevice).createDevice(info);
    }

    Device::~Device()
    {
        m_device.destroy();
    }

    auto findMemoryTypeIndex(vk::PhysicalDeviceMemoryProperties const& memoryProperties,
                             uint32_t                                  typeBits,
                             vk::MemoryPropertyFlags                   requirementsMask) -> std::optional<uint32_t>
    {
        for (auto i = 0u; i < memoryProperties.memoryTypeCount; ++i)
        {
            if ((typeBits & 1) == 1)
            {
                if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
                {
                    return i;
                }
            }

            typeBits >>= 1;
        }

        return std::nullopt;
    }
} // namespace com::rhi
