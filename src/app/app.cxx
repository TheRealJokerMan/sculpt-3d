//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "app/app.hxx"
#include "app/version.hxx" // Generated.

#include <QIcon>

namespace com::app
{
    Application::Application(int32_t argc, char** argv) : QApplication(argc, argv)
    {
        setApplicationName(QString::fromStdString(app::Version::name()));
        setApplicationVersion(QString::fromStdString(app::Version::asString()));
        setOrganizationName("jamie.kenyon");
        setWindowIcon(QIcon(":/app-icon"));

        installTranslators();
    }

    Application::~Application()
    {
    }

    void Application::installTranslators()
    {
        // Find a locale-specific qm file, for example: /i18n/en_GB.qm
        m_localeTranslator = std::make_unique<QTranslator>();
        if (m_localeTranslator->load(QLocale(), "", "", ":/i18n"))
            QCoreApplication::installTranslator(m_localeTranslator.get());

        // Additionally install an English default in case there's no locale-specific variant.
        m_defaultTranslator = std::make_unique<QTranslator>();
        if (m_defaultTranslator->load("en_GB", ":/i18n"))
            QCoreApplication::installTranslator(m_defaultTranslator.get());
    }
} // namespace com::app
