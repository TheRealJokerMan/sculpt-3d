//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <cstdint>

namespace com::app
{
    /// Application entry point.
    /// \param argc The number of arguments.
    /// \param argv An array of null-terminated strings.
    /// \return true on success; false otherwise.
    [[nodiscard]] auto entryPoint(int32_t argc, char** argv) -> bool;

} // namespace com::app
