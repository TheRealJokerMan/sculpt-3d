//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "base/resource.hxx"

#include <QFile>
#include <QString>

namespace com::base
{
    auto loadBinary(std::string const& fileName) -> std::vector<uint32_t>
    {
        if (QFile file(QString(":/%1").arg(fileName.data())); file.open(QIODevice::ReadOnly))
        {
            QByteArray const contents = file.readAll();
            auto const*      begin    = reinterpret_cast<uint32_t const*>(contents.constData());
            auto const*      end      = begin + (contents.size() >> 2);

            return std::vector<uint32_t>(begin, end);
        }

        return {};
    }
} // namespace com::base
