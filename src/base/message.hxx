//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>

namespace com::base
{
    /// Output a message of 'debug' severity.
    /// \param message The message to output.
    void outputDebug(std::string const& message);

    /// Output a message of 'error' severity.
    /// \param message The message to output.
    void outputError(std::string const& message);

    /// Output a message of 'information' severity.
    /// \param message The message to output.
    void outputInformation(std::string const& message);

    /// Output a message of 'warning' severity.
    /// \param message The message to output.
    void outputWarning(std::string const& message);
} // namespace com::base
