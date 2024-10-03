//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/context.hxx"

namespace com::rhi
{
    /// Vertex attributes.
    using VertexAttributes = std::vector<std::pair<vk::Format, size_t>>;

    /// Create a graphics pipeline.
    /// \param context The RHI context.
    /// \param vertexAttributes The vertex attributes.
    /// \param vertexShader The vertex shader.
    /// \param fragmentShader The fragment shader.
    /// \param frontFace The front face.
    /// \param topology The topology.
    /// \param depthBuffered Is depth buffered.
    /// \param pipelineLayout The pipeline layout.
    /// \param renderInfo The render information.
    /// \return A valid pipeline on success; nothing otherwise.
    [[nodiscard]] auto createGraphicsPipeline(Context const*                         context,
                                              VertexAttributes const&                vertexAttributes,
                                              vk::ShaderModule const&                vertexShader,
                                              vk::ShaderModule const&                fragmentShader,
                                              vk::FrontFace const                    frontFace,
                                              vk::PrimitiveTopology const            topology,
                                              bool const                             depthBuffered,
                                              vk::PipelineRenderingCreateInfo const& renderInfo,
                                              vk::PipelineLayout const&              pipelineLayout) -> vk::Pipeline;
} // namespace com::rhi
