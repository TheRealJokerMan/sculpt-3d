//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/pipeline.hxx"

namespace com::rhi
{
    [[nodiscard]] auto createGraphicsPipeline(Context const*                         context,
                                              VertexAttributes const&                vertexAttributes,
                                              vk::ShaderModule const&                vertexShader,
                                              vk::ShaderModule const&                fragmentShader,
                                              vk::FrontFace const                    frontFace,
                                              vk::PrimitiveTopology const            topology,
                                              bool const                             depthBuffered,
                                              vk::PipelineRenderingCreateInfo const& renderInfo,
                                              vk::PipelineLayout const&              pipelineLayout) -> vk::Pipeline
    {
        std::array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexShader, "main"),
            vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentShader, "main")
        };

        vk::PipelineVertexInputStateCreateInfo           pipelineVertexInputStateCreateInfo;
        std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
        std::vector<vk::VertexInputBindingDescription>   vertexInputBindingDescription;

        vertexInputAttributeDescriptions.reserve(vertexAttributes.size());
        for (auto i = 0u; i < vertexAttributes.size(); ++i)
        {
            vertexInputAttributeDescriptions.emplace_back(i, i, vertexAttributes[i].first, 0);
            vertexInputBindingDescription.emplace_back(i, static_cast<uint32_t>(vertexAttributes[i].second));
        }
        pipelineVertexInputStateCreateInfo.setVertexBindingDescriptions(vertexInputBindingDescription);
        pipelineVertexInputStateCreateInfo.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);

        vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo({}, topology);

        vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);

        vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
        {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, frontFace, false, 0.0f, 0.0f, 0.0f, 1.0f);

        vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1);

        vk::StencilOpState                      stencilOpState(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
        vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        vk::PipelineDepthStencilStateCreateFlags(), depthBuffered, depthBuffered, vk::CompareOp::eGreater, false, false, stencilOpState, stencilOpState);

        vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(false,
                                                                                vk::BlendFactor::eZero,
                                                                                vk::BlendFactor::eZero,
                                                                                vk::BlendOp::eAdd,
                                                                                vk::BlendFactor::eZero,
                                                                                vk::BlendFactor::eZero,
                                                                                vk::BlendOp::eAdd,
                                                                                vk::FlagTraits<vk::ColorComponentFlagBits>::allFlags);
        vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
        vk::PipelineColorBlendStateCreateFlags(), false, vk::LogicOp::eNoOp, pipelineColorBlendAttachmentState, { { 1.0f, 1.0f, 1.0f, 1.0f } });

        std::array<vk::DynamicState, 2>    dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(vk::PipelineDynamicStateCreateFlags(), dynamicStates);

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(vk::PipelineCreateFlags(),
                                                                  pipelineShaderStageCreateInfos,
                                                                  &pipelineVertexInputStateCreateInfo,
                                                                  &pipelineInputAssemblyStateCreateInfo,
                                                                  nullptr,
                                                                  &pipelineViewportStateCreateInfo,
                                                                  &pipelineRasterizationStateCreateInfo,
                                                                  &pipelineMultisampleStateCreateInfo,
                                                                  &pipelineDepthStencilStateCreateInfo,
                                                                  &pipelineColorBlendStateCreateInfo,
                                                                  &pipelineDynamicStateCreateInfo,
                                                                  pipelineLayout);

        graphicsPipelineCreateInfo.setPNext(&renderInfo);

        auto const result = context->device()->logicalDevice().createGraphicsPipeline(context->pipelineCache(), graphicsPipelineCreateInfo);

        return result.value;
    }
} // namespace com::rhi
