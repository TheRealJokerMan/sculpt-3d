//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace com::rhi
{
    /// Create a descriptor pool
    /// \param device The Vulkan device.
    /// \param poolSizes The pool sizes.
    /// \return A descriptor pool.
    [[nodiscard]] auto createDescriptorPool(vk::Device const& device, std::vector<vk::DescriptorPoolSize> const& poolSizes) -> vk::DescriptorPool;

    /// Helper declaration.
    struct DescriptorSetDescription final
    {
        vk::DescriptorType const   type;  ///< The type.
        uint32_t const             count; ///< How many.
        vk::ShaderStageFlags const flags; ///< Any flags.
    };

    /// Create a descriptor set layout.
    /// \param device The Vulkan device.
    /// \param bindingData The bindings.
    /// \param flags The flags.
    /// \return A descriptor set layout.
    [[nodiscard]] auto createDescriptorSetLayout(vk::Device const&                            device,
                                                 std::vector<DescriptorSetDescription> const& bindingData,
                                                 vk::DescriptorSetLayoutCreateFlags           flags = {}) -> vk::DescriptorSetLayout;

    /// Create a descriptor set layout.
    /// \param device The Vulkan device.
    /// \param fileName The name of the file containing the byte code.
    /// \return A valid shader if successful; nothing otherwise.
    [[nodiscard]] auto createShader(vk::Device const& device, std::string const& fileName) -> vk::ShaderModule;

    /// Find a memory type for allocation.
    /// \param memoryProperties The memory properties.
    /// \param typeBits The type bits.
    /// \param requirementsMask The requirements mask.
    /// \return A valid integer.
    [[nodiscard]] auto findMemoryType(vk::PhysicalDeviceMemoryProperties const& memoryProperties,
                                      uint32_t const                            typeBits,
                                      vk::MemoryPropertyFlags const             requirementsMask) -> uint32_t;

    /// Find a supported depth format.
    /// \param physicalDevice The physical Vulkan device.
    /// \param tiling The preferred tiling method.
    /// \return Any of the desired formats, if supported; vk::Format::eUndefined otherwise.
    [[nodiscard]] auto getDepthFormat(vk::PhysicalDevice const& physicalDevice, vk::ImageTiling const tiling) -> vk::Format;

    /// Determines if a given format is a depth/stencil format.
    /// \param format The format to test.
    /// \return True if a given format is a depth/stencil format; false otherwise.
    [[nodiscard]] auto isDepthFormat(vk::Format const format) -> bool;

    /// Make a 32-bit colour from 8-bit components.
    /// \param r The red value.
    /// \param g The green value.
    /// \param b The blue value.
    /// \param a The alpha value.
    /// \return A 32-bit value in RGBA form.
    [[nodiscard]] static inline auto makeColour(uint8_t const r, uint8_t const g, uint8_t const b, uint8_t const a) -> uint32_t
    {
        return (r << 24) | (g << 16) | (b << 8) | (a << 0);
    }

    /// Helper declaration.
    struct DescriptorUpdate final
    {
        vk::DescriptorType type;   ///< The type.
        vk::Buffer const&  buffer; ///< The buffer.
        vk::DeviceSize     size;   ///< Size.
        vk::BufferView     view;   ///< The view.
    };

    /// Update descriptor sets.
    /// \param device The Vulkan device.
    /// \param descriptorSet The descriptor set.
    /// \param bufferData The buffer data.
    /// \param bindingOffset The binding offset.
    void updateDescriptorSets(vk::Device const&                    device,
                              vk::DescriptorSet const&             descriptorSet,
                              std::vector<DescriptorUpdate> const& bufferData, //
                              uint32_t                             bindingOffset = 0);

} // namespace com::rhi
