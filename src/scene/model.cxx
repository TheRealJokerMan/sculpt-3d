//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "scene/model.hxx"

namespace com::scene
{
    Model::Model(std::unique_ptr<rhi::Mesh> mesh) : m_mesh(std::move(mesh)), m_transform(1.0f)
    {
    }

    void Model::render(vk::CommandBuffer const& commandBuffer) const
    {
        m_mesh->render(commandBuffer);
    }
} // namespace com::scene
