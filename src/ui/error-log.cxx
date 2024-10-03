//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/error-log.hxx"
#include "ui/ui_error-log.h" // Generated.

#include <QTime>
#include <mutex>

namespace com::ui
{
    /// Defines the roles of columnns in the tree.
    enum ColumnUsage
    {
        /// The message.
        ColumnUsageMessage,

        /// Time that the log entry was created.
        ColumnUsageTime,

        /// The number of columns.
        ColumnUsageCount
    };

    QtMessageHandler   ErrorLog::s_previousMessageHandler;
    QStandardItemModel ErrorLog::s_model;

    ErrorLog::ErrorLog(QWidget* parent, Qt::WindowFlags flags) : QWidget(parent, flags)
    {
        m_ui = std::make_unique<Ui::ErrorLog>();
        m_ui->setupUi(this);

        // Set the column labels.
        QList<QString> labels({ tr("ColumnLabel01"), tr("ColumnLabel02") });
        s_model.setHorizontalHeaderLabels(labels);

        m_ui->m_treeView->setModel(&s_model);

        // Ensure that the "Message" column is the widest and stretches the full width.
        if (auto* header = m_ui->m_treeView->header(); header)
        {
            header->setStretchLastSection(false);
            header->setSectionResizeMode(QHeaderView::ResizeToContents);
            header->setSectionResizeMode(ColumnUsageMessage, QHeaderView::Stretch);
        }
    }

    ErrorLog::~ErrorLog()
    {
    }

    void ErrorLog::messageFilter(QtMsgType const type, QMessageLogContext const&, QString const& msg)
    {
        std::array<QIcon, 5> icons{
            QIcon(":/debug-icon"),      // QtDebugMsg
            QIcon(":/warning-icon"),    // QtWarningMsg
            QIcon(":/error-icon"),      // QtCriticalMsg
            QIcon(":/error-icon"),      // QtFatalMsg
            QIcon(":/information-icon") // QtInfoMsg
        };

        size_t const index   = (type >= 0 && type <= std::ssize(icons)) ? type : static_cast<size_t>(QtInfoMsg);
        auto*        message = new QStandardItem(icons[index], msg);
        auto*        time    = new QStandardItem(QTime::currentTime().toString(Qt::DateFormat::ISODateWithMs));

        s_model.appendRow({ message, time });
    }

    void ErrorLog::installMessageFilter()
    {
        static std::once_flag flag;

        std::call_once(flag, []() { s_previousMessageHandler = qInstallMessageHandler(messageFilter); });
    }

    void ErrorLog::uninstallMessageFilter()
    {
        qInstallMessageHandler(s_previousMessageHandler);
    }
} // namespace com::ui
