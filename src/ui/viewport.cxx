//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/viewport.hxx"
#include "rhi/utilities.hxx"
#include "ui/main-window.hxx"

namespace com::ui
{
    static std::array<vk::ClearValue, 2> s_clearValues;

    Viewport::Viewport(MainWindow const* mainWindow, std::string const& appName, uint32_t const appVersion)
    {
        connect(mainWindow, &MainWindow::documentReplaced, this, &Viewport::onDocumentReplaced);

        s_clearValues[0].color        = vk::ClearColorValue(0.2f, 0.2f, 0.2f, 1.0f);
        s_clearValues[1].depthStencil = vk::ClearDepthStencilValue(0.0f, 0);

        rhi::RhiDescription description = { .name = appName, .version = appVersion };

        description.instanceLayers = {
#if !defined(NDEBUG)
            "VK_LAYER_KHRONOS_validation"
#endif // #if !defined(NDEBUG)
        };

        description.instanceExtensions = {
#if !defined(NDEBUG)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // #if !defined(NDEBUG)

#if defined(VK_ENABLE_BETA_EXTENSIONS)
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif // #if defined(VK_ENABLE_BETA_EXTENSIONS)

            VK_KHR_SURFACE_EXTENSION_NAME,
            SURFACE_EXTENSION_NAME,
        };

        description.deviceExtensions = {
#if defined(VK_ENABLE_BETA_EXTENSIONS)
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
#endif // #if defined(VK_ENABLE_BETA_EXTENSIONS)

            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME
        };

        description.deviceFeatures.samplerAnisotropy = VK_TRUE;

#if defined(Q_OS_DARWIN)
        description.windowHandle = makeViewMetalCompatible(winId());
#else
        description.windowHandle = reinterpret_cast<void const*>(winId());
#endif

#if defined(Q_OS_LINUX)
        auto* x11app                 = qApp->nativeInterface<QNativeInterface::QX11Application>();
        description.windowConnection = x11app->connection();
#endif

        m_context = std::make_unique<rhi::Context>(description);
    }

    void Viewport::onTerminating()
    {
        m_context->waitForIdle();

        m_context->onTerminating();
        m_swapChain.reset();

        m_context.reset();
    }

    auto Viewport::event(QEvent* event) -> bool
    {
        switch (event->type())
        {
        case QEvent::UpdateRequest:
            render();
            break;

        default:
            break;
        }

        return QWindow::event(event);
    }

    void Viewport::mouseMoveEvent(QMouseEvent* event)
    {
        m_camera->track(event->position().toPoint() * devicePixelRatio());

        switch (m_camera->mode())
        {
        case scene::CameraMode::None:
            // TODO: rollover highlighting should be performed here.
            break;

        case scene::CameraMode::Dolly:
        case scene::CameraMode::Orbit:
        case scene::CameraMode::Truck:
            m_camera->processMovement();
            break;

        default:
            event->ignore();
            break;
        }

        if (m_document)
        {
            m_document->requestHitUpdate();
        }

        requestUpdate();
    }

    void Viewport::mousePressEvent(QMouseEvent* event)
    {
        auto const point = event->position().toPoint() * devicePixelRatio();

        if (event->modifiers() & (Qt::ControlModifier))
        {
            switch (event->button())
            {
            case Qt::LeftButton:
                m_camera->setMode(scene::CameraMode::Orbit, point);
                break;

            case Qt::MiddleButton:
                m_camera->setMode(scene::CameraMode::Dolly, point);
                break;

            case Qt::RightButton:
                m_camera->setMode(scene::CameraMode::Truck, point);
                break;

            default:
                break;
            }
        }
        else
        {
            m_camera->setMode(scene::CameraMode::Pick, point);
        }

        if (m_document)
        {
            m_document->requestHitUpdate();
        }
    }

    void Viewport::mouseReleaseEvent(QMouseEvent* /*event*/)
    {
        m_camera->setMode(scene::CameraMode::None);
    }

    void Viewport::resizeEvent(QResizeEvent* event)
    {
        auto const size = event->size();
        if (size != m_size)
        {
            m_size   = size;
            m_camera = std::make_unique<scene::Camera>(m_size.width(), m_size.height());

            if (m_context)
            {
                m_swapChain.reset();

                vk::Extent2D const extent(m_size.width(), m_size.height());
                m_swapChain = std::make_unique<rhi::SwapChain>(m_context.get(), m_context->surface(), extent);

                if (m_document)
                {
                    m_document->resize(extent);
                }

                m_context->allocatePerFrameData(m_swapChain->numImages());

                render();
            }
        }
    }

    void Viewport::wheelEvent(QWheelEvent* event)
    {
        auto const delta  = event->angleDelta().y() / 1000.0f;
        auto const factor = std::max(-0.5f, std::min(0.5f, delta));

        m_camera->processMovement(factor);

        requestUpdate();
    }

    void Viewport::onDocumentReplaced(scene::Document* document)
    {
        m_document = document;

        if (m_document)
        {
            m_camera->fitToDocument(m_document);
        }

        m_convergence = false;

        requestUpdate();
    }

    void Viewport::frameStart()
    {
        m_swapChain->acquireNextFrame(m_context->frameData());

        auto*      frameData     = m_context->frameData();
        auto       commandBuffer = frameData->commandBuffer();
        auto const extent        = m_swapChain->extent();

        m_swapChain->image(frameData->imageIndex())->setUsage(rhi::Image::Usage::eUndefined);
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f));
        commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent));

        if (m_document)
        {
            m_document->updateHitTestQuery(m_camera.get(), m_swapChain->rect());
        }

        m_swapChain->image(frameData->imageIndex())->transition(rhi::Image::Usage::eAttachmentReadWrite, commandBuffer);
        m_swapChain->depthStencil()->transition(rhi::Image::Usage::eAttachmentReadWrite, commandBuffer);
    }

    void Viewport::frameRender()
    {
        auto*                                    frameData     = m_context->frameData();
        auto                                     commandBuffer = frameData->commandBuffer();
        std::vector<vk::RenderingAttachmentInfo> attachments;

        // Color attachment.
        auto colourAttachment = m_swapChain->image(frameData->imageIndex())->asRenderingAttachmentInfo();
        colourAttachment.setClearValue(s_clearValues[0]);
        attachments.emplace_back(colourAttachment);

        // Depth attachment.
        auto depthAttachment = m_swapChain->depthStencil()->asRenderingAttachmentInfo();
        depthAttachment.setClearValue(s_clearValues[1]);

        vk::RenderingInfo renderInfo({}, m_swapChain->rect(), 1u, 0, attachments, &depthAttachment);
        commandBuffer.beginRendering(renderInfo);

        if (m_document)
        {
            m_document->render(m_camera.get(), commandBuffer);
        }

        commandBuffer.endRendering();

        m_swapChain->image(frameData->imageIndex())->transition(rhi::Image::Usage::ePresent, commandBuffer);

        commandBuffer.end();

        m_context->queue(rhi::QueueIndex::eGraphics)->submit(commandBuffer, frameData);

        m_context->waitForFences(frameData->fence());
    }

    void Viewport::frameEnd()
    {
        if (m_swapChain->present(m_context->queue(rhi::QueueIndex::eGraphics), m_context->frameData()))
            m_context->advanceNextFrame();
    }

    void Viewport::render()
    {
        if (m_swapChain)
        {
            frameStart();
            frameRender();
            frameEnd();
        }

        if (!m_convergence)
        {
            // When a new documented is opened, the scene renders but doesn't display anything. I
            // have no idea why. To get around that, we render the scene twice when the document
            // is opened.
            requestUpdate();
            m_convergence = true;
        }
    }

} // namespace com::ui
