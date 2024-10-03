//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QDockWidget>

namespace com::ui
{
    /// A custom dock widget.
    class DockWidget final : public QDockWidget
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param title The dock's title.
        /// \param parent The parent widget.
        /// \param flags The window's flags.
        explicit DockWidget(QString const& title, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags()) : QDockWidget(title, parent, flags)
        {
            setObjectName(title);
        }

    signals:
        /// Emitted when a dock is closed.
        void closed();

    protected:
        /// See Qt documentation.
        void closeEvent(QCloseEvent* event) final
        {
            emit closed();

            QDockWidget::closeEvent(event);
        }
    };
} // namespace com::ui
