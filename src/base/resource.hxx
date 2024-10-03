//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#pragma once

#include <string>
#include <vector>

namespace com::base
{
    /// Load the contents of a binary file from the embedded resources.
    /// \param fileName The name of the file to load.
    /// \return The contents of the file.
    [[nodiscard]] auto loadBinary(std::string const& fileName) -> std::vector<uint32_t>;
} // namespace com::base
