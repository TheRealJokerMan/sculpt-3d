//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace com::rhi
{
    /// Debug utility.
    class DebugUtil final
    {
    public:
        /// Constructor.
        /// \param instance The Vulkan instance.
        explicit DebugUtil(vk::Instance const& instance);

        /// Destructor.
        ~DebugUtil();

    private:
        vk::Instance               m_instance;
        vk::DebugUtilsMessengerEXT m_messenger;
    };
} // namespace com::rhi
