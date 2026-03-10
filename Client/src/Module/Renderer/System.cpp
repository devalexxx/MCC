// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/Renderer/System.h"

#include "Client/Graphics/OpenGL/OpenGLCommon.h"
#include "Client/Module/Renderer/Module.h"
#include "Client/WorldContext.h"

#include <glad/glad.h>

namespace Mcc
{

    void SetupRendererSystem(flecs::iter& it)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        IgnoreIter(it);
    }

    void PollWindowEventSystem(flecs::iter& it)
    {
        Window::PollEvents();

        IgnoreIter(it);
    }

    void ClearFrameSystem(flecs::iter& it)
    {
        glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        glCheck(glClearColor(1., 1., 1., 1.));

        IgnoreIter(it);
    }

    void RenderFrameSystem(flecs::iter& it)
    {
        ClientWorldContext::Get(it.world())->window.SwapBuffer();

        IgnoreIter(it);
    }

}