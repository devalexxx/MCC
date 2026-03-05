// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/ImGui/System.h"

#include "Common/Utils/FlecsUtils.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Mcc
{

    void ImGuiBeginFrameSystem(flecs::iter& it)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        IgnoreIter(it);
    }

    void ImGuiRenderFrameSystem(flecs::iter& it)
    {
        ImGui::Render();

        IgnoreIter(it);
    }

    void ImGuiDrawFrameSystem(flecs::iter& it)
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        IgnoreIter(it);
    }

}
