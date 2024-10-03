//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "rhi/command-pool.hxx"
#include "rhi/context.hxx"
#include "rhi/per-frame-data.hxx"
#include "rhi/swap-chain.hxx"
#include "scene/camera.hxx"
#include "scene/document.hxx"

#include <QResizeEvent>
#include <QWindow>
#include <chrono>

namespace com::ui
{
    /// The Vulkan viewport.
    class Viewport final : public QWindow
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param mainWindow The main window.
        /// \param appName The name of the application.
        /// \param appVersion The version of the application.
        explicit Viewport(class MainWindow const* mainWindow, std::string const& appName, uint32_t const appVersion);

        /// Get the RHI context.
        /// \return A pointer to a valid object.
        [[nodiscard]] auto context()
        {
            return m_context.get();
        }

        /// Accessor.
        /// \return A valid Vulkan object.
        [[nodiscard]] auto extent()
        {
            return m_swapChain->extent();
        }

    public slots:
        /// React to the application terminating.
        void onTerminating();

    protected:
        /// See Qt documentation.
        [[nodiscard]] auto event(QEvent* event) -> bool final;

        /// See Qt documentation.
        void mouseMoveEvent(QMouseEvent* event) final;

        /// See Qt documentation.
        void mousePressEvent(QMouseEvent* event) final;

        /// See Qt documentation.
        void mouseReleaseEvent(QMouseEvent* event) final;

        /// See Qt documentation.
        void resizeEvent(QResizeEvent* event) final;

        /// See Qt documentation.
        void wheelEvent(QWheelEvent* event) final;

    private slots:
        void onDocumentReplaced(scene::Document* document);

    private:
        void               frameStart();
        void               frameRender();
        void               frameEnd();
        [[nodiscard]] auto makeViewMetalCompatible(uint64_t const handle) -> void*;
        void               render();

    private:
        std::unique_ptr<rhi::Context>   m_context;
        QSize                           m_size;
        std::unique_ptr<rhi::SwapChain> m_swapChain;
        std::unique_ptr<scene::Camera>  m_camera;

        bool             m_convergence = false;
        scene::Document* m_document    = nullptr;
    };
} // namespace com::ui
