//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "base/preferences.hxx"

#include <QCoreApplication>
#include <QSettings>
#include <mutex>

namespace com::base
{
    static std::vector<Preference> s_preferences = { { // MinimumPrimitivePolygonCount
                                                       "primitivePolygonCount",
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "primitivePolygonCountLabel"),
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "primitivePolygonCountTooltip"),
                                                       100'000 },

                                                     { // PrimitiveRadius
                                                       "primitiveRadius",
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "primitiveRadiusLabel"),
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "primitiveRadiusTooltip"),
                                                       1.0f },

                                                     { // CursorVertexCount
                                                       "cursorVertexCount",
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "cursorVertexCountLabel"),
                                                       QT_TRANSLATE_NOOP("com::base::Preferences", "cursorVertexCountTooltip"),
                                                       1'000 } };

    Preferences::Preferences(QObject* parent) : QObject(parent)
    {
        populateDefaults();
    }

    auto Preferences::all() -> std::vector<Preference> const&
    {
        populateDefaults();
        return s_preferences;
    }

    void Preferences::populateDefaults()
    {
        static std::once_flag flag;

        std::call_once(flag,
                       []()
                       {
                           QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

                           for (auto& preference : s_preferences)
                           {
                               if (auto const key = QString("settings/%1").arg(preference.name.data()); !settings.contains(key))
                               {
                                   settings.setValue(key, preference.value);
                               }

                               preference.displayLabel   = tr(preference.labelNoOp.data());
                               preference.displayTooltip = tr(preference.tooltipNoOp.data());
                               preference.displayValue   = preference.value.toString();
                           }
                       });
    }

    auto Preferences::read(PreferenceType const type) -> QVariant
    {
        populateDefaults();
        return s_preferences[static_cast<size_t>(type)].value;
    }

    auto Preferences::read(std::string const& name) -> QVariant
    {
        populateDefaults();
        auto const it = std::ranges::find_if(s_preferences, [&](Preference const& preference) { return preference.name == name; });

        if (it != s_preferences.end())
            return it->value;
        else
            return {};
    }

    void Preferences::write(std::string const& name, QVariant const& value)
    {
        populateDefaults();

        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        settings.setValue(name, value);
        settings.sync();

        auto it = std::ranges::find_if(s_preferences, [&](Preference const& preference) { return preference.name == name; });

        if (it != s_preferences.end())
        {
            it->value        = value;
            it->displayValue = it->value.toString();
        }
    }
} // namespace com::base
