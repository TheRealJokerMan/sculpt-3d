//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace com::rhi
{
    /// Helper struct for building buffers.
    struct BufferDescription
    {
        vk::BufferUsageFlags flags; ///< The usage flags.
        vk::DeviceSize       size;  ///< The size of the buffer's data, in bytes.
    };

    /// Represents a buffer.
    class Buffer
    {
    public:
        /// Constructor.
        /// \param context The RHI context.
        /// \param size The size of the buffer, in bytes.
        /// \param usageFlags The usage of the buffer.
        /// \param propertyFlags The property flags.
        explicit Buffer(class Context*                context,
                        vk::DeviceSize const          size,
                        vk::BufferUsageFlags const    usageFlags,
                        vk::MemoryPropertyFlags const propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

        /// Destructor.
        ~Buffer();

        /// Accessor.
        /// \return A Vulkan object.
        [[nodiscard]] auto buffer() const -> vk::Buffer const&
        {
            return m_buffer;
        }

        /// Get the number of elements in the buffer.
        /// \return A valid integer.
        [[nodiscard]] auto count() const
        {
            return m_numElements;
        }

        /// Map the memory.
        [[nodiscard]] auto map() -> void*;

        /// Ummap the memory.
        void unmap();

        /// Upload the contents of the buffer to the GPU.
        /// \param data The data.
        /// \param size The size of the data, in bytes.
        void upload(void const* data, size_t const size);

        /// Upload the contents of the buffer to the GPU.
        /// \param data The data.
        template <typename T>
        void upload(std::vector<T> const& data)
        {
            upload(data.data(), data.size() * sizeof(T));
            m_numElements = static_cast<uint32_t>(data.size());
        }

        /// Upload the contents of the buffer to the GPU.
        /// \param data The data.
        template <typename T>
        void upload(T const& data)
        {
            upload(&data, sizeof(T));
            m_numElements = 1;
        }

    private:
        vk::Buffer       m_buffer;
        class Context*   m_context = nullptr;
        vk::DeviceMemory m_deviceMemory;
        uint32_t         m_numElements = 0;
        vk::DeviceSize   m_size;
    };
} // namespace com::rhi
