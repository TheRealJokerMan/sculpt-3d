//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/mesh.hxx"

namespace com::rhi
{
    /// Make a cursor primitive.
    /// \param context The RHI context.
    /// \param vertexCount The number of vertices in the cursor.
    /// \return A new mesh.
    [[nodiscard]] auto makeCursor(Context* context, uint32_t const vertexCount) -> std::unique_ptr<Mesh>;

    /// Make a sphere primitive.
    /// \param context The RHI context.
    /// \param centre The centre of the sphere.
    /// \param radius The radius of the sphere.
    /// \param minPolygons The minimum amount of polygons
    /// \return A new mesh.
    [[nodiscard]] auto makeSphere(Context* context, glm::vec3 const& centre, float const radius, uint32_t const minPolygons) -> std::unique_ptr<Mesh>;
} // namespace com::rhi
