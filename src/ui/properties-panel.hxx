//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <QWidget>

// Forward declarations.
namespace Ui
{
    class PropertiesPanel;
}

namespace com::ui
{
    /// TODO.
    class PropertiesPanel final : public QWidget
    {
        Q_OBJECT

    public:
        /// Constructor.
        /// \param parent The parent widget.
        /// \param flags The window's flags.
        explicit PropertiesPanel(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        /// Destructor
        ~PropertiesPanel();

    private:
        std::unique_ptr<Ui::PropertiesPanel> m_ui;
    };
} // namespace com::ui
