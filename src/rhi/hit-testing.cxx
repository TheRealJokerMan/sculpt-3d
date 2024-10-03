//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/hit-testing.hxx"

namespace com::rhi
{
    [[nodiscard]] static auto sign(float x)
    {
        return (x < 0.0f) ? -1.0f : 1.0f;
    }

    [[nodiscard]] static auto uncompressNormal(uint32_t const cn)
    {
        uint32_t const ux = cn >> 16;
        uint32_t const uy = cn & 0xffff;

        float const ex = 2.0f * (static_cast<float>(ux) / 65535.0f) - 1.0f;
        float const ey = 2.0f * (static_cast<float>(uy) / 65535.0f) - 1.0f;

        glm::vec3 n;
        float     z = 1.0f - (std::abs(ex) + std::abs(ey));

        if (z >= 0.0f)
            n = glm::vec3(ex, ey, z);
        else
            n = glm::vec3((1.0f - std::abs(ey)) * sign(ex), (1.0f - std::abs(ex)) * sign(ey), z);

        return glm::normalize(n);
    }

    auto createMouseHit(uint32_t const x, uint32_t const y, uint32_t const w, uint32_t const h, Buffer* buffer) -> std::unique_ptr<MouseHit>
    {
        uint8_t const* ptr    = static_cast<uint8_t const*>(buffer->map());
        float          sz     = *reinterpret_cast<float const*>(ptr);
        uint32_t       normal = *reinterpret_cast<uint32_t const*>(ptr + sizeof(float));
        buffer->unmap();

        if (sz > 0.0)
        {
            float const sx = (2.0f * x / static_cast<float>(w)) - 1.0f;
            float const sy = (2.0f * y / static_cast<float>(h)) - 1.0f;

            return std::make_unique<MouseHit>(glm::vec3(sx, sy, sz), uncompressNormal(normal));
        }

        return {};
    }
} // namespace com::rhi
