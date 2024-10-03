//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QObject>
#include <QVariant>

namespace com::base
{
    /// Identifies a particular user preference.
    enum class PreferenceType
    {
        MinimumPrimitivePolygonCount, ///< The minimum number of polygons to use when creating a primitive.
        PrimitiveRadius,              ///< The radius of new primitives.
        CursorVertexCount,            ///< The number of vertices in the cursor.
    };

    /// The definition of a single preference.
    struct Preference
    {
        std::string name;           ///< The internal name.
        std::string labelNoOp;      ///< A localised version of the internal name.
        std::string tooltipNoOp;    ///< A localised tooltip.
        QVariant    value;          ///< The value.
        QString     displayLabel;   ///< The label as a translated string for display.
        QString     displayTooltip; ///< The tooltip as a translated string for display.
        QString     displayValue;   ///< The value as a string for display.
    };

    /// Provides access to the user preferences.
    class Preferences final : public QObject
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param parent The parent object.
        explicit Preferences(QObject* parent = nullptr);

        /// Get all the preferences.
        /// \return A container of valid entries.
        [[nodiscard]] static auto all() -> std::vector<Preference> const&;

        /// Read a value from the preferences.
        /// \param type The key.
        /// \return A value.
        [[nodiscard]] static auto read(PreferenceType const type) -> QVariant;

        /// Read a value from the preferences.
        /// \param name The key.
        /// \return A value.
        [[nodiscard]] static auto read(std::string const& name) -> QVariant;

        /// Write a value to the preferences.
        /// \param name The key.
        /// \param value The value.
        static void write(std::string const& name, QVariant const& value);

    private:
        static void populateDefaults();
    };
} // namespace com::base
