//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "app/entry-point.hxx"

#include <cstdlib>

auto main(int32_t argc, char** argv) -> int32_t
{
    return com::app::entryPoint(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
