//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QDialog>

// Forward declarations.
namespace Ui
{
    class AboutDialog;
}

namespace com::ui
{
    /// Widget to show information about the application.
    class AboutDialog final : public QDialog
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param parent The parent widget, if any.
        /// \param flags The window flags.
        explicit AboutDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        /// Destructor.
        ~AboutDialog();

    private:
        std::unique_ptr<Ui::AboutDialog> m_ui;
    };
} // namespace com::ui
