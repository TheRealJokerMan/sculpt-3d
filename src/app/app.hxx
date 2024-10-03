//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QApplication>
#include <QTranslator>

namespace com::app
{
    /// The application.
    class Application final : public QApplication
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param argc The numnber of values in argv.
        /// \param argv A null-terminated list of null-terminated strings.
        explicit Application(int32_t argc, char** argv);

        /// Destructor.
        ~Application();

    private:
        void installTranslators();

    private:
        std::unique_ptr<QTranslator> m_localeTranslator;
        std::unique_ptr<QTranslator> m_defaultTranslator;
    };
} // namespace com::app
