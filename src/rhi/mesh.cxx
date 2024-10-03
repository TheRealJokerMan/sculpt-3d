//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/mesh.hxx"
#include "rhi/utilities.hxx"

namespace com::rhi
{
    Mesh::Mesh(Context* context, MeshDescription const* description) : m_context(context)
    {
        auto const        numVertices = description->points.size();
        auto const        numPolygons = description->indices.size() / 3;
        BufferDescription desc;

        // Index buffer.
        desc.flags = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        desc.size  = sizeof(glm::uvec3) * numPolygons;
        if (m_buffers[BufferTypeIndex] = std::make_unique<Buffer>(m_context, desc.size, desc.flags); m_buffers[BufferTypeIndex])
        {
            m_buffers[BufferTypeIndex]->upload(description->indices);
        }

        // Base vertices buffer.
        auto const transferFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        desc.flags               = vk::BufferUsageFlagBits::eVertexBuffer | transferFlags | vk::BufferUsageFlagBits::eStorageBuffer;
        desc.size                = sizeof(glm::vec3) * numVertices;
        if (m_buffers[BufferTypeBaseVertex] = std::make_unique<Buffer>(m_context, desc.size, desc.flags); m_buffers[BufferTypeBaseVertex])
        {
            m_buffers[BufferTypeBaseVertex]->upload(description->points);

            for (auto const& point : description->points)
                m_bounds.extend(point);

            // Edit vertices buffer.
            if (m_buffers[BufferTypeEditVertex] = std::make_unique<Buffer>(m_context, desc.size, desc.flags); m_buffers[BufferTypeEditVertex])
            {
                m_buffers[BufferTypeEditVertex]->upload(description->points);
            }
        }

        // Colour buffer.
        desc.size = sizeof(uint32_t) * numVertices;
        if (m_buffers[BufferTypeColour] = std::make_unique<Buffer>(m_context, desc.size, desc.flags); m_buffers[BufferTypeColour])
        {
            std::vector<uint32_t> colours(numVertices);
            std::ranges::fill(colours, makeColour(0xFF, 0, 0, 0xFF));

            m_buffers[BufferTypeColour]->upload(colours);
        }
    }

    void Mesh::render(vk::CommandBuffer const& commandBuffer)
    {
        commandBuffer.bindVertexBuffers(0, { m_buffers[BufferTypeBaseVertex]->buffer() }, { 0 });
        commandBuffer.bindVertexBuffers(1, { m_buffers[BufferTypeColour]->buffer() }, { 0 });
        commandBuffer.bindIndexBuffer(m_buffers[BufferTypeIndex]->buffer(), 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexed(m_buffers[BufferTypeIndex]->count(), 1, 0, 0, 0);
    }
} // namespace com::rhi
