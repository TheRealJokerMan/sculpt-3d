//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/utilities.hxx"
#include "base/resource.hxx"

#include <numeric>

namespace com::rhi
{
    static std::vector<vk::Format> s_depthFormats = { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint };

    auto createDescriptorPool(vk::Device const& device, std::vector<vk::DescriptorPoolSize> const& poolSizes) -> vk::DescriptorPool
    {
        auto const functor = [](uint32_t sum, vk::DescriptorPoolSize const& dps) { return sum + dps.descriptorCount; };
        uint32_t   maxSets = std::accumulate(poolSizes.begin(), poolSizes.end(), 0, functor);

        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, maxSets, poolSizes);
        return device.createDescriptorPool(descriptorPoolCreateInfo);
    }

    auto createDescriptorSetLayout(vk::Device const&                            device,
                                   std::vector<DescriptorSetDescription> const& bindingData, //
                                   vk::DescriptorSetLayoutCreateFlags           flags) -> vk::DescriptorSetLayout
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings(bindingData.size());
        for (uint32_t i = 0; i < bindingData.size(); ++i)
            bindings[i] = vk::DescriptorSetLayoutBinding(i, bindingData[i].type, bindingData[i].count, bindingData[i].flags);

        return device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(flags, bindings));
    }

    auto createShader(vk::Device const& device, std::string const& fileName) -> vk::ShaderModule
    {
        auto       byteCode = base::loadBinary(fileName);
        auto const info     = vk::ShaderModuleCreateInfo({}, byteCode);

        return device.createShaderModule(info);
    }

    auto findMemoryType(vk::PhysicalDeviceMemoryProperties const& memoryProperties,
                        uint32_t const                            typeBits_, //
                        vk::MemoryPropertyFlags const             requirementsMask) -> uint32_t
    {
        auto typeBits  = typeBits_;
        auto typeIndex = uint32_t(~0);

        for (auto i = 0u; i < memoryProperties.memoryTypeCount; ++i)
        {
            if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask))
            {
                typeIndex = i;
                break;
            }

            typeBits >>= 1;
        }

        return typeIndex;
    }

    auto getDepthFormat(vk::PhysicalDevice const& physicalDevice, vk::ImageTiling const tiling) -> vk::Format
    {
        for (auto& desiredFormat : s_depthFormats)
        {
            vk::FormatProperties properties = physicalDevice.getFormatProperties(desiredFormat);

            if (tiling == vk::ImageTiling::eLinear && properties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
                return desiredFormat;

            if (tiling == vk::ImageTiling::eOptimal && properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
                return desiredFormat;
        }

        return vk::Format::eUndefined;
    }

    auto isDepthFormat(vk::Format const format) -> bool
    {
        return std::ranges::find(s_depthFormats, format) != s_depthFormats.end();
    }

    void updateDescriptorSets(vk::Device const&                    device,
                              vk::DescriptorSet const&             descriptorSet,
                              std::vector<DescriptorUpdate> const& bufferData,
                              uint32_t                             bindingOffset)
    {
        std::vector<vk::DescriptorBufferInfo> bufferInfos;
        bufferInfos.reserve(bufferData.size());

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
        writeDescriptorSets.reserve(bufferData.size() + 1);

        uint32_t dstBinding = bindingOffset;
        for (auto const& bd : bufferData)
        {
            bufferInfos.emplace_back(bd.buffer, 0, bd.size);
            writeDescriptorSets.emplace_back(descriptorSet, dstBinding++, 0, 1, bd.type, nullptr, &bufferInfos.back(), &bd.view);
        }

        device.updateDescriptorSets(writeDescriptorSets, nullptr);
    }
} // namespace com::rhi
