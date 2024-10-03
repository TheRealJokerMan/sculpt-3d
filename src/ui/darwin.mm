//
// Copyright(c) 2024 Jamie Kenyon. All Rights Reserved.
//

#include "ui/viewport.hxx"

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>

namespace com::ui
{
    auto Viewport::makeViewMetalCompatible(uint64_t const wid) -> void*
    {
        void* handle = reinterpret_cast<void*>(wid);
        NSView* view = (__bridge NSView*)handle;

        // Verify that the handle points to an NSView
        assert([view isKindOfClass:[NSView class]]);

        if (![view.layer isKindOfClass:[CAMetalLayer class]])
        {
            [view setLayer:[CAMetalLayer layer]];
            [view setWantsLayer:YES];
        }

        return view.layer;
    }
} // namespace com::ui
