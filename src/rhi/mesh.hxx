//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/buffer.hxx"
#include "rhi/utilities.hxx"

#include <unordered_map>
#include <glm/glm-aabb.hpp>
#include <glm/glm.hpp>

namespace com::rhi
{
    /// Parameters to build a mesh.
    struct MeshDescription final
    {
        glm::vec3                              centre;  ///< The centre of the mesh.
        float                                  radius;  ///< The radius of the mesh.
        std::vector<glm::vec3>                 points;  ///< The vertex points.
        std::vector<uint32_t>                  indices; ///< Indices into the points.
        std::unordered_map<uint64_t, uint32_t> edges;   ///< The edges.
    };

    /// A drawable mesh object.
    class Mesh final
    {
    public:
        /// Specifies the type of mesh buffer.
        enum BufferType
        {
            BufferTypeIndex,      ///< The indices.
            BufferTypeBaseVertex, ///< Base vertex positions.
            BufferTypeEditVertex, ///< Edit vertex positions.
            BufferTypeColour,     ///< Per-polygon colour.
            BufferTypeCount       ///< The number of buffers.
        };

    public:
        /// Constructor.
        /// \param context The RHI context.
        /// \param description The description of the mesh.
        explicit Mesh(Context* context, MeshDescription const* description);

        /// Get the bounding box of this mesh.
        /// \return A valid bounding box.
        [[nodiscard]] auto bounds() const
        {
            return m_bounds;
        }

        /// Render the mesh.
        /// \param commandBuffer The command buffer to write instructions to.
        void render(vk::CommandBuffer const& commandBuffer);

        /// Update a uniform.
        /// \param matrix The matrix to upload.
        void updateUniform(glm::mat4 const& matrix);

    private:
        Context*                                             m_context = nullptr;
        std::array<std::unique_ptr<Buffer>, BufferTypeCount> m_buffers;
        AABB                                                 m_bounds;
    };

} // namespace com::rhi
