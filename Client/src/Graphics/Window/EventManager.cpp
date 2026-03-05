// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Graphics/Window/EventManager.h"

#include "Client/Graphics/Window/Event.h"
#include "Client/Graphics/Window/Window.h"

#include "Common/Utils/Logging.h"

namespace Mcc
{

    void WindowEventManager::BoundEvent(const Window& window)
    {
        glfwSetKeyCallback      (window.Get(), KeyCallback);
        glfwSetCursorPosCallback(window.Get(), CursorPosCallback);
    }

    void WindowEventManager::KeyCallback(
        GLFWwindow* window, const int key, const int scancode, const int action, const int mods
    )
    {
        const auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!w)
        {
            MCC_LOG_WARN("Trying to dispatch an event from a non exiting window (Should not happen)");
            return;
        }

        w->Dispatch<KeyEvent>({ .window=*w, .key=key, .scancode=scancode, .action=action, .mods=mods });
    }

    void WindowEventManager::CursorPosCallback(GLFWwindow* window, const double x, const double y)
    {
        const auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!w)
        {
            MCC_LOG_WARN("Trying to dispatch an event from a non exiting window (Should not happen)");
            return;
        }

        w->Dispatch<CursorPosEvent>({ .window=*w, .x=x, .y=y });
    }

}