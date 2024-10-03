//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include "ui/properties-panel.hxx"
#include "ui/settings-panel.hxx"
#include "ui/viewport.hxx"

#include <QCloseEvent>
#include <QDockWidget>
#include <QMainWindow>

// Forward declarations.
namespace Ui
{
    class MainWindow;
}

namespace com::ui
{
    /// The application's main window.
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param appName The name of the application.
        /// \param appVersion The version of the application.
        /// \param parent The parent widget.
        /// \param flags The window's flags.
        explicit MainWindow(std::string const& appName, uint32_t const appVersion, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        /// Destructor
        ~MainWindow();

    signals:
        /// Emitted when the document is replaced.
        /// \param document The new document.
        void documentReplaced(scene::Document* document);

        /// Emitted when the application is terminating.
        void terminating();

    protected:
        /// See Qt documentation.
        void closeEvent(QCloseEvent* event) final;

    private slots:
        void onDocumentReplaced(scene::Document* document);
        void onFileClose();
        void onFileNew();
        void onFileOpen();
        void onFileSave();
        void onFileSaveAs();
        void onHelpErrorLog();
        void onHelpAbout();

    private:
        void               createDocks();
        void               createViewport(std::string const& appName, uint32_t const appVers);
        void               fileOpen(QString const& path);
        [[nodiscard]] auto fileSave() -> bool;
        void               fileSaveAs(QString const& path);
        void               readSettings();
        void               updateRecentFileActions(QString const& path = {});
        [[nodiscard]] auto save() -> bool;
        void               updateWindowTitle();
        void               writeSettings();

    private:
        std::unique_ptr<scene::Document> m_document;
        PropertiesPanel*                 m_propertiesPanel = nullptr;
        SettingsPanel*                   m_settingsPanel   = nullptr;
        std::unique_ptr<Ui::MainWindow>  m_ui;
        Viewport*                        m_viewport = nullptr;
    };
} // namespace com::ui
