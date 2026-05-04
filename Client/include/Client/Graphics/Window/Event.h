// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_WINDOW_EVENT_H
#define MCC_CLIENT_GRAPHICS_WINDOW_EVENT_H

namespace Mcc
{

    class Window;

    struct KeyEvent
    {
        Window& window;

        int key;
        int scancode;
        int action;
        int mods;
    };

    struct CursorPosEvent
    {
        Window& window;

        double x;
        double y;
    };

    struct MouseButtonEvent
    {
        Window& window;

        int button;
        int action;
        int mods;
    };

}

#endif