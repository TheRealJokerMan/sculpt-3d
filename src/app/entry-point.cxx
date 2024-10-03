//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "app/entry-point.hxx"
#include "app/app.hxx"
#include "app/version.hxx" // Generated.
#include "ui/error-log.hxx"
#include "ui/main-window.hxx"

namespace com::app
{
    auto entryPoint(int32_t argc, char** argv) -> bool
    {
        ui::ErrorLog::installMessageFilter();

        auto app = std::make_shared<app::Application>(argc, argv);

        auto mainWindow = std::make_unique<ui::MainWindow>(Version::name(), Version::asInteger());
        mainWindow->showMaximized();

        auto const result = app->exec() ? false : true;

        ui::ErrorLog::uninstallMessageFilter();
        return result;
    }

} // namespace com::app
