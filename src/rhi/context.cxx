//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "rhi/context.hxx"
#include "rhi/per-frame-data.hxx"
#include "rhi/utilities.hxx"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace com::rhi
{
    [[nodiscard]] static auto filterLayers(vk::ArrayProxy<std::string const> const& required) -> std::vector<char const*>
    {
        auto const               available = vk::enumerateInstanceLayerProperties();
        std::vector<char const*> result;

        std::ranges::for_each(required,
                              [available, &result](std::string const& layer)
                              {
                                  auto const it = std::ranges::find_if(available,
                                                                       [layer](vk::LayerProperties const& property)
                                                                       { return std::string(property.layerName.data()).compare(layer) == 0; });

                                  if (it != available.end())
                                  {
                                      result.emplace_back(layer.c_str());
                                  }
                              });

        return result;
    }

    [[nodiscard]] static auto filterExtensions(vk::ArrayProxy<std::string const> const& required) -> std::vector<char const*>
    {
        auto const               available = vk::enumerateInstanceExtensionProperties();
        std::vector<char const*> result;

        std::ranges::for_each(required,
                              [available, &result](std::string const& extension)
                              {
                                  auto const it = std::ranges::find_if(available,
                                                                       [extension](vk::ExtensionProperties const& property)
                                                                       { return std::string(property.extensionName.data()).compare(extension) == 0; });

                                  if (it != available.end())
                                  {
                                      result.emplace_back(extension.c_str());
                                  }
                              });

        return result;
    }

    Context::Context(RhiDescription const& description)
    {
        vk::ApplicationInfo appInfo(description.name.data(), description.version, description.name.data(), description.version, VK_API_VERSION_1_3);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        auto const layers     = filterLayers(description.instanceLayers);
        auto const extensions = filterExtensions(description.instanceExtensions);

        auto const getFlags = []()
        {
#if defined(__APPLE__)
            return vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#else
            return static_cast<vk::InstanceCreateFlags>(0);
#endif
        };

        auto const info = vk::InstanceCreateInfo(getFlags(), &appInfo, layers, extensions);
        m_instance      = vk::createInstance(info);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

        createDebugUtility();
        createSurface(description.windowConnection, description.windowHandle);

        m_physicalDevice     = PhysicalDevice::pick(this, m_surface, description);
        m_computeQueueIndex  = m_physicalDevice->findQueueIndex(vk::QueueFlagBits::eCompute, nullptr);
        m_graphicsQueueIndex = m_physicalDevice->findQueueIndex(vk::QueueFlagBits::eGraphics, nullptr);
        m_presentQueueIndex  = m_physicalDevice->findQueueIndex(vk::QueueFlagBits::eGraphics, &m_surface);
        m_transferQueueIndex = m_physicalDevice->findQueueIndex(vk::QueueFlagBits::eTransfer, nullptr);

        m_device           = std::make_unique<Device>(this, m_physicalDevice.get(), description);
        m_depthFormat      = getDepthFormat(*m_physicalDevice, vk::ImageTiling::eOptimal);
        auto const formats = m_physicalDevice->surfaceFormats(m_surface);
        m_colorFormat      = ((formats.size() == 1) && (formats[0].format == vk::Format::eUndefined)) ? vk::Format::eB8G8R8A8Unorm : formats[0].format;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device->logicalDevice());

        m_computeQueue  = std::make_unique<Queue>(m_device.get(), m_computeQueueIndex);
        m_graphicsQueue = std::make_unique<Queue>(m_device.get(), m_graphicsQueueIndex);
        m_presentQueue  = std::make_unique<Queue>(m_device.get(), m_presentQueueIndex);
        m_transferQueue = std::make_unique<Queue>(m_device.get(), m_transferQueueIndex);

        m_pipelineCache = m_device->logicalDevice().createPipelineCache(vk::PipelineCacheCreateInfo());
    }

    Context::~Context()
    {
        m_device.reset();
        m_instance.destroySurfaceKHR(m_surface);
        m_debugUtil.reset();
        m_instance.destroy();
    }

    void Context::allocatePerFrameData(uint32_t const count)
    {
        if (m_perFrameData.empty())
        {
            auto const qi = queueIndex(QueueIndex::eGraphics);

            for (auto index = 0u; index < count; ++index)
                m_perFrameData.emplace_back(std::make_unique<FrameData>(this, qi));
        }
    }

    void Context::onTerminating()
    {
        auto const& d = device()->logicalDevice();

        d.destroyPipelineCache(m_pipelineCache);

        m_perFrameData.clear();
    }

    auto Context::queueIndices() const -> std::set<uint32_t>
    {
        std::set<uint32_t> queueIndicesSet;

        queueIndicesSet.emplace(m_computeQueueIndex);
        queueIndicesSet.emplace(m_graphicsQueueIndex);
        queueIndicesSet.emplace(m_presentQueueIndex);
        queueIndicesSet.emplace(m_transferQueueIndex);

        return queueIndicesSet;
    }

    void Context::waitForFences(vk::Fence const& fence, vk::Bool32 const waitAll, uint64_t const timeout)
    {
        while (vk::Result::eTimeout == m_device->logicalDevice().waitForFences(fence, waitAll, timeout))
            /* noop*/;
    }

    void Context::waitForIdle()
    {
        auto const& d = device()->logicalDevice();

        queue(QueueIndex::eGraphics)->wait();

        d.waitIdle();
    }

    void Context::createDebugUtility()
    {
#if !defined(NDEBUG)
        m_debugUtil = std::make_unique<DebugUtil>(m_instance);
#endif // #if !defined(NDEBUG)
    }

    void Context::createSurface([[maybe_unused]] void const* connection, void const* display)
    {
#if defined(VK_USE_PLATFORM_METAL_EXT)
        // Metal extension.
        vk::MetalSurfaceCreateInfoEXT surfaceCreateInfo({}, display);
        m_surface = m_instance.createMetalSurfaceEXT(surfaceCreateInfo);

#elif defined(VK_USE_PLATFORM_WIN32_KHR)
        // Win32.
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo({}, ::GetModuleHandleA(nullptr), static_cast<HWND>(const_cast<void*>(display)));
        m_surface = m_instance.createWin32SurfaceKHR(surfaceCreateInfo);

#elif defined(VK_USE_PLATFORM_XCB_KHR)
        // XCB (Linux).
        vk::XcbSurfaceCreateInfoKHR surfaceCreateInfo({}, connection, static_cast<xcb_window_t>(reinterpret_cast<intptr_t>(display)));
        m_surface = m_instance.createXcbSurfaceKHR(surfaceCreateInfo);
#endif
    }

} // namespace com::rhi
