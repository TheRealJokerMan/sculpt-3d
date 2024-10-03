//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/debug-util.hxx"
#include "base/message.hxx"

#include <sstream>

namespace com::rhi
{
    [[nodiscard]] static VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
                                                                VkDebugUtilsMessageTypeFlagsEXT             types,
                                                                VkDebugUtilsMessengerCallbackDataEXT const* data,
                                                                void* /*pUserData*/)
    {
        std::stringstream ss;

        ss << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << ": "
           << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(types)) << ":\n";

        ss << "\tmessageIDName   = <" << data->pMessageIdName << ">\n";
        ss << "\tmessageIdNumber = " << data->messageIdNumber << "\n";
        ss << "\tmessage         = <" << data->pMessage << ">\n";

        if (0 < data->queueLabelCount)
        {
            ss << "\tQueue Labels:\n";

            std::span arr = { data->pQueueLabels, data->queueLabelCount };
            std::ranges::for_each(arr, [&](auto const& label) { ss << "\t\tlabelName = <" << label.pLabelName << ">\n"; });
        }

        if (0 < data->cmdBufLabelCount)
        {
            ss << "\tCommandBuffer Labels:\n";

            std::span arr = { data->pCmdBufLabels, data->cmdBufLabelCount };
            std::ranges::for_each(arr, [&](auto const& label) { ss << "\t\tlabelName = <" << label.pLabelName << ">\n"; });
        }

        if (0 < data->objectCount)
        {
            ss << "\tObjects:\n";

            std::span arr = { data->pObjects, data->objectCount };

            for (auto const& object : arr)
            {
                ss << "\t\tobjectType   = " << vk::to_string(static_cast<vk::ObjectType>(object.objectType)) << "\n";
                ss << "\t\tobjectHandle = " << object.objectHandle << "\n";

                if (object.pObjectName)
                    ss << "\t\tobjectName   = <" << object.pObjectName << ">\n";
            }
        }

        auto const output = ss.str();

        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            base::outputError(output.data());

        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            base::outputWarning(output.data());

        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            base::outputInformation(output.data());

        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            base::outputDebug(output.data());

        return VK_TRUE;
    }

    DebugUtil::DebugUtil(vk::Instance const& instance) : m_instance(instance)
    {
        vk::DebugUtilsMessengerCreateInfoEXT const info({},
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
                                                        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                        validationCallback);

        m_messenger = instance.createDebugUtilsMessengerEXT(info, nullptr, vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr));
    }

    DebugUtil::~DebugUtil()
    {
        m_instance.destroyDebugUtilsMessengerEXT(m_messenger, nullptr, vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr));
    }
} // namespace com::rhi
