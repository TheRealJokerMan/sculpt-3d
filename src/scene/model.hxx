//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/mesh.hxx"
#include "rhi/pipeline.hxx"
#include "scene/camera.hxx"

namespace com::scene
{
    /// A model is a 3D mesh that can be rendered and drawn upon.
    class Model final
    {
    public:
        /// Constructor.
        /// \param mesh The model's mesh,
        explicit Model(std::unique_ptr<rhi::Mesh> mesh);

        /// Get the bounding box of this mesh.
        /// \return A valid bounding box.
        [[nodiscard]] auto bounds() const -> AABB
        {
            return m_mesh ? m_mesh->bounds() : AABB();
        }

        /// Render the model.
        /// \param commandBuffer The command buffer to write instructions to.
        void render(vk::CommandBuffer const& commandBuffer) const;

        /// Accessor.
        /// \return A valid matrix.
        [[nodiscard]] auto transform() const
        {
            return m_transform;
        }

    private:
        std::unique_ptr<rhi::Mesh> m_mesh;
        glm::mat4                  m_transform;
    };
} // namespace com::scene
