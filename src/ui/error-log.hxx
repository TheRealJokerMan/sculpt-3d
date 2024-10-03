//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QStandardItemModel>
#include <QWidget>

// Forward declarations.
namespace Ui
{
    class ErrorLog;
}

namespace com::ui
{
    /// A widget to show any messages that have been logged.
    class ErrorLog final : public QWidget
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param parent The parent widget.
        /// \param flags The window's flags.
        explicit ErrorLog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        /// Destructor
        ~ErrorLog();

        /// Intercept Qt logging messages.
        static void installMessageFilter();

        /// Remove custom message handlers.
        static void uninstallMessageFilter();

    private:
        static void messageFilter(QtMsgType const type, QMessageLogContext const& context, QString const& msg);

    private:
        std::unique_ptr<Ui::ErrorLog> m_ui;
        static QtMessageHandler       s_previousMessageHandler;
        static QStandardItemModel     s_model;
    };
} // namespace com::ui
