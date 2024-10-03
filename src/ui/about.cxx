//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/about.hxx"
#include "ui/ui_about.h" // Generated

namespace com::ui
{
    AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags flags) : QDialog(parent, flags)
    {
        m_ui = std::make_unique<Ui::AboutDialog>();
        m_ui->setupUi(this);

        auto const text = tr("Details").arg(qApp->applicationName()).arg(qApp->applicationVersion()).arg(qApp->organizationName());
        m_ui->m_details->setText(text);
    }

    AboutDialog::~AboutDialog()
    {
    }
} // namespace com::ui
