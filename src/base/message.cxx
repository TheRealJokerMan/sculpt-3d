//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "base/message.hxx"

#include <QDebug>
#include <print>

namespace com::base
{
    void outputDebug(std::string const& message)
    {
        qDebug() << message;
        std::println("{}", message);
    }

    void outputError(std::string const& message)
    {
        qCritical() << message;
        std::println("{}", message);
    }

    void outputInformation(std::string const& message)
    {
        qInfo() << message;
        std::println("{}", message);
    }

    void outputWarning(std::string const& message)
    {
        qWarning() << message;
        std::println("{}", message);
    }
} // namespace com::base
