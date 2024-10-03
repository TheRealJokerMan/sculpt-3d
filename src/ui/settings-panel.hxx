//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QStandardItemModel>
#include <QWidget>

// Forward declarations.
namespace Ui
{
    class SettingsPanel;
}

namespace com::ui
{
    /// A widget for interacting with the application's settings.
    class SettingsPanel final : public QWidget
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param parent The parent widget.
        /// \param flags The window's flags.
        explicit SettingsPanel(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        /// Destructor
        ~SettingsPanel();

    private:
        std::unique_ptr<Ui::SettingsPanel> m_ui;
        QStandardItemModel*                m_model = nullptr;
    };
} // namespace com::ui
