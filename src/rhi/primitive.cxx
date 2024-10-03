//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/primitive.hxx"

#include <numbers>

namespace com::rhi
{
    auto makeCursor(Context* context, uint32_t const cursorVertexCount) -> std::unique_ptr<Mesh>
    {
        auto           meshDescription   = std::make_shared<MeshDescription>();
        uint32_t const cursorCircleCount = cursorVertexCount - 2;

        for (auto i = 0u; i < cursorCircleCount; ++i)
        {
            auto const t      = static_cast<float>(i) / static_cast<float>(cursorCircleCount);
            auto const amount = static_cast<float>(std::numbers::pi) * 2 * t;

            meshDescription->points.emplace_back(std::sin(amount));
            meshDescription->points.emplace_back(0.0f);
            meshDescription->points.emplace_back(std::cos(amount));
        }

        meshDescription->points.emplace_back(0.0f);
        meshDescription->points.emplace_back(0.0f);
        meshDescription->points.emplace_back(0.0f);

        meshDescription->points.emplace_back(0.0f);
        meshDescription->points.emplace_back(0.5f);
        meshDescription->points.emplace_back(0.0f);

        int32_t last = cursorCircleCount - 1;
        for (uint32_t i = 0; i < cursorCircleCount; ++i)
        {
            meshDescription->indices.emplace_back(last);
            meshDescription->indices.emplace_back(i);

            last = i;
        }

        meshDescription->indices.emplace_back(cursorVertexCount - 2);
        meshDescription->indices.emplace_back(cursorVertexCount - 1);

        return std::make_unique<Mesh>(context, meshDescription.get());
    }

    [[nodiscard]] static auto findOrAddIndex(uint32_t const i0, uint32_t const i1, MeshDescription* meshDescription)
    {
        auto const edge = (static_cast<uint64_t>(std::min(i0, i1)) << 32) | static_cast<uint64_t>(std::max(i0, i1));

        if (auto i = meshDescription->edges.find(edge); i != meshDescription->edges.end())
        {
            auto const index = i->second;
            meshDescription->edges.erase(i);
            return index;
        }
        else
        {
            auto const point0   = meshDescription->points[i0];
            auto const point1   = meshDescription->points[i1];
            auto const midPoint = 0.5f * (point0 + point1);
            auto const normal   = midPoint - meshDescription->centre;
            auto const newPoint = meshDescription->centre + meshDescription->radius * glm::normalize(normal);

            auto const index = static_cast<uint32_t>(meshDescription->points.size());

            meshDescription->points.emplace_back(newPoint);
            meshDescription->edges.insert({ edge, index });
            return index;
        }
    }

    static void subdivide(uint32_t const   i0,
                          uint32_t const   i1,
                          uint32_t const   i2,
                          uint32_t const   targetPolygonCount,
                          uint32_t const   currentPolygonCount,
                          MeshDescription* meshDescription)
    {
        if (targetPolygonCount <= currentPolygonCount)
        {
            meshDescription->indices.emplace_back(i0);
            meshDescription->indices.emplace_back(i1);
            meshDescription->indices.emplace_back(i2);
        }
        else
        {
            auto const i01 = findOrAddIndex(i0, i1, meshDescription);
            auto const i12 = findOrAddIndex(i1, i2, meshDescription);
            auto const i20 = findOrAddIndex(i2, i0, meshDescription);

            subdivide(i0, i01, i20, targetPolygonCount, 4 * currentPolygonCount, meshDescription);
            subdivide(i01, i1, i12, targetPolygonCount, 4 * currentPolygonCount, meshDescription);
            subdivide(i20, i12, i2, targetPolygonCount, 4 * currentPolygonCount, meshDescription);
            subdivide(i01, i12, i20, targetPolygonCount, 4 * currentPolygonCount, meshDescription);
        }
    }

    auto makeSphere(Context* context, glm::vec3 const& centre, float const radius, uint32_t const minPolygons) -> std::unique_ptr<Mesh>
    {
        auto meshDescription = std::make_shared<MeshDescription>(centre, radius);

        meshDescription->points.reserve(minPolygons >> 1);
        meshDescription->indices.reserve(3 * minPolygons);
        meshDescription->edges.reserve(3 * (minPolygons >> 1));

        meshDescription->points.emplace_back(centre - glm::vec3(radius, 0.0f, 0.0f));
        meshDescription->points.emplace_back(centre + glm::vec3(radius, 0.0f, 0.0f));
        meshDescription->points.emplace_back(centre - glm::vec3(0.0f, radius, 0.0f));
        meshDescription->points.emplace_back(centre + glm::vec3(0.0f, radius, 0.0f));
        meshDescription->points.emplace_back(centre - glm::vec3(0.0f, 0.0f, radius));
        meshDescription->points.emplace_back(centre + glm::vec3(0.0f, 0.0f, radius));

        auto const numPolysPerFace = minPolygons / 8;
        subdivide(0, 5, 3, numPolysPerFace, 1, meshDescription.get());
        subdivide(5, 1, 3, numPolysPerFace, 1, meshDescription.get());
        subdivide(1, 4, 3, numPolysPerFace, 1, meshDescription.get());
        subdivide(4, 0, 3, numPolysPerFace, 1, meshDescription.get());
        subdivide(5, 0, 2, numPolysPerFace, 1, meshDescription.get());
        subdivide(1, 5, 2, numPolysPerFace, 1, meshDescription.get());
        subdivide(4, 1, 2, numPolysPerFace, 1, meshDescription.get());
        subdivide(0, 4, 2, numPolysPerFace, 1, meshDescription.get());

        return std::make_unique<Mesh>(context, meshDescription.get());
    }
} // namespace com::rhi
