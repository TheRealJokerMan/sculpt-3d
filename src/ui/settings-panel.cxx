//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/settings-panel.hxx"
#include "base/preferences.hxx"
#include "ui/ui_settings-panel.h" // Generated.

namespace com::ui
{
    /// Defines the roles of columnns in the tree.
    enum ColumnUsage
    {
        /// The name of the setting.
        ColumnUsageName,

        /// The setting's value.
        ColumnUsageValue,

        /// The number of columns.
        ColumnUsageCount
    };

    SettingsPanel::SettingsPanel(QWidget* parent, Qt::WindowFlags flags) : QWidget(parent, flags)
    {
        m_ui = std::make_unique<Ui::SettingsPanel>();
        m_ui->setupUi(this);

        // Set the column labels.
        QList<QString> labels({ tr("ColumnLabel01"), tr("ColumnLabel02") });
        m_model = new QStandardItemModel(this);
        m_model->setHorizontalHeaderLabels(labels);
        m_ui->m_tableView->setModel(m_model);

        for (auto const& preference : base::Preferences::all())
        {
            auto* nameItem = new QStandardItem(preference.displayLabel);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            nameItem->setToolTip(preference.displayTooltip);

            auto* valueItem = new QStandardItem(preference.displayValue);
            valueItem->setToolTip(preference.displayTooltip);

            m_model->appendRow({ nameItem, valueItem });
        }

        // Ensure that the "Name" column is the widest and stretches the full width.
        if (auto* header = m_ui->m_tableView->horizontalHeader(); header)
        {
            header->setStretchLastSection(false);
            header->setSectionResizeMode(QHeaderView::ResizeToContents);
            header->setSectionResizeMode(ColumnUsageName, QHeaderView::Stretch);
        }
    }

    SettingsPanel::~SettingsPanel()
    {
    }

} // namespace com::ui
