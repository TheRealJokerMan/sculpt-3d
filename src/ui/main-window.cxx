//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/main-window.hxx"
#include "base/preferences.hxx"
#include "ui/about.hxx"
#include "ui/dock-widget.hxx"
#include "ui/error-log.hxx"
#include "ui/ui_main-window.h" // Generated.

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

namespace com::ui
{
    MainWindow::MainWindow(std::string const& appName, uint32_t const appVersion, QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
    {
        m_ui = std::make_unique<Ui::MainWindow>();
        m_ui->setupUi(this);

        createViewport(appName, appVersion);
        createDocks();

        readSettings();

        connect(this, &MainWindow::documentReplaced, this, &MainWindow::onDocumentReplaced);
    }

    MainWindow::~MainWindow()
    {
        delete m_viewport;
        m_viewport = nullptr;
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        if (fileSave())
        {
            m_document.reset();
            emit documentReplaced(m_document.get());
            emit terminating();

            writeSettings();
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }

    void MainWindow::onDocumentReplaced(scene::Document* document)
    {
        auto const enabled = document != nullptr;

        m_ui->m_fileMenuClose->setEnabled(enabled);
        m_ui->m_fileMenuSave->setEnabled(enabled);
        m_ui->m_fileMenuSaveAs->setEnabled(enabled);
    }

    void MainWindow::onFileClose()
    {
        if (fileSave())
        {
            m_document.reset();
            updateWindowTitle();

            emit documentReplaced(m_document.get());
        }
    }

    void MainWindow::onFileNew()
    {
        onFileClose();
        m_document = std::make_unique<scene::Document>(m_viewport->context(), m_viewport->extent());
        updateWindowTitle();

        emit documentReplaced(m_document.get());
    }

    void MainWindow::onFileOpen()
    {
        auto const title       = tr("OpenScene");
        auto const description = tr("FileFilter");
        auto const path        = QFileDialog::getOpenFileName(this,
                                                       title,
                                                       QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), // User documents.
                                                       description);

        if (!path.isEmpty())
        {
            fileOpen(path);
        }
    }

    void MainWindow::onFileSave()
    {
        fileSaveAs(m_document->path());
    }

    void MainWindow::onFileSaveAs()
    {
        auto const title       = tr("SaveScene");
        auto const description = tr("FileFilter");
        auto const path        = QFileDialog::getSaveFileName(this,
                                                       title,
                                                       QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), // User documents.
                                                       description);
        fileSaveAs(path);
    }

    void MainWindow::onHelpErrorLog()
    {
        auto* dialog = new ErrorLog();
        dialog->show();
    }

    void MainWindow::onHelpAbout()
    {
        auto* dialog = new AboutDialog();
        dialog->show();
    }

    void MainWindow::createDocks()
    {
        // Create the first panel.
        auto* propertiesPanelDock = new DockWidget(tr("PropertiesPanel"), this);
        m_propertiesPanel         = new PropertiesPanel(propertiesPanelDock);
        propertiesPanelDock->setWidget(m_propertiesPanel);
        connect(propertiesPanelDock, &DockWidget::closed, m_viewport, &QWindow::requestUpdate);

        // Add the first panel to the main window.
        addDockWidget(Qt::RightDockWidgetArea, propertiesPanelDock);

        // Create the second panel.
        auto* settingsPanelDock = new DockWidget(tr("SettingsPanel"), this);
        m_settingsPanel         = new SettingsPanel(settingsPanelDock);
        settingsPanelDock->setWidget(m_settingsPanel);
        connect(settingsPanelDock, &DockWidget::closed, m_viewport, &QWindow::requestUpdate);

        // Add the second panel behind the first.
        tabifyDockWidget(propertiesPanelDock, settingsPanelDock);
    }

    void MainWindow::createViewport(std::string const& appName, uint32_t const appVersion)
    {
        m_viewport = new Viewport(this, appName, appVersion);

        auto* viewportWidget = QWidget::createWindowContainer(m_viewport, nullptr, Qt::Widget);
        m_ui->m_viewportLayout->addWidget(viewportWidget);

        connect(this, &MainWindow::terminating, m_viewport, &Viewport::onTerminating);
    }

    void MainWindow::fileOpen(QString const& path)
    {
        updateRecentFileActions(path);
        updateWindowTitle();
    }

    auto MainWindow::fileSave() -> bool
    {
        if (m_document)
        {
            auto result = m_document->save();
            updateWindowTitle();
            return result;
        }

        return true;
    }

    void MainWindow::fileSaveAs(QString const& path)
    {
        if (m_document->save(path))
        {
            updateWindowTitle();
            updateRecentFileActions(path);
        }
    }

    void MainWindow::readSettings()
    {
        base::Preferences preferences;

        updateRecentFileActions();
        restoreGeometry(preferences.read("geometry").toByteArray());
        restoreState(preferences.read("windowState").toByteArray());
    }

    void MainWindow::updateRecentFileActions(QString const& path)
    {
        base::Preferences preferences;

        if (!path.isEmpty())
        {
            auto files = preferences.read("recentFileList").toStringList();

            files.removeAll(path);
            files.prepend(path);
            preferences.write("recentFileList", files);
        }

        m_ui->m_fileMenuRecent->clear();

        for (auto const& file : preferences.read("recentFileList").toStringList())
        {
            QFileInfo fileInfo(file);
            if (fileInfo.exists())
            {
                auto* action = new QAction(fileInfo.fileName());
                action->setData(file);
                connect(action, &QAction::triggered, this, [file, this]() { fileOpen(file); });
                m_ui->m_fileMenuRecent->addAction(action);
            }
        }
    }

    void MainWindow::updateWindowTitle()
    {
        QString title = QCoreApplication::applicationName();

        if (m_document)
        {
            title += QString(" - [%1]").arg(m_document->name());

            if (m_document->isModified())
            {
                title += QString("*");
            }
        }

        setWindowTitle(title);
    }

    void MainWindow::writeSettings()
    {
        base::Preferences preferences;

        preferences.write("geometry", saveGeometry());
        preferences.write("windowState", saveState());
    }
} // namespace com::ui
