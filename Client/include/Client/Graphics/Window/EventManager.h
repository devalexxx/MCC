// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_GRAPHICS_WINDOW_EVENT_MANAGER_H
#define MCC_CLIENT_GRAPHICS_WINDOW_EVENT_MANAGER_H

#include "Common/Utils/EventManager.h"

struct GLFWwindow;

namespace Mcc
{

    class Window;

    struct WindowEventTag
    {};

    class WindowEventManager : public EventManager<WindowEventTag>
    {
      public:
        static void BoundEvent(const Window& window);

      private:
        static void KeyCallback        (GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback  (GLFWwindow* window, double x, double y);
    };

}

#endif