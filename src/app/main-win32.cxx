//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "app/entry-point.hxx"

#include <cstdlib>
#include <windef.h>

auto WinMain(HINSTANCE, HINSTANCE, char*, int32_t) -> int32_t
{
    return com::app::entryPoint(__argc, __argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
