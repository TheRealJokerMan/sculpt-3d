//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/buffer.hxx"

#include <glm/glm.hpp>

namespace com::rhi
{
    /// Represents a mouse hit.
    struct MouseHit
    {
        glm::vec3 point;  ///< The world-space position.
        glm::vec3 normal; ///< The normal.
    };

    /// Create a mouse-hit from a buffer.
    /// \param x The horizontal offset.
    /// \param y The vertical offset.
    /// \param w The viewport width.
    /// \param h The viewport height.
    /// \param buffer.
    /// \return A valid object on success; nothing otherwise.
    [[nodiscard]] auto createMouseHit(uint32_t const x, uint32_t const y, uint32_t const w, uint32_t const h, Buffer* buffer) -> std::unique_ptr<MouseHit>;
} // namespace com::rhi
