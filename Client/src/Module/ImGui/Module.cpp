// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Client/Module/ImGui/Module.h"

#include "Client/Module/ImGui/System.h"
#include "Client/Module/Renderer/Module.h"
#include "Client/WorldContext.h"

#include "Common/Phase.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Mcc
{

    ImGuiModule::ImGuiModule(flecs::world& world) : BaseModule(world)
    {
        const auto ctx = ClientWorldContext::Get(world);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io     = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;// Enable Gamepad Controls

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(ctx->window.Get(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ImGuiModule::~ImGuiModule()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiModule::RegisterComponent(flecs::world& /* world */) {}

    void ImGuiModule::RegisterPrefab(flecs::world& /* world */) {}

    void ImGuiModule::RegisterSystem(flecs::world& world)
    {
        world.system("ImGuiBeginFrame")
            .kind<Phase::OnSetup>()
            .run(ImGuiBeginFrameSystem);

        world.system("ImGuiRenderFrame")
            .kind<Phase::PostDrawGui>()
            .run(ImGuiRenderFrameSystem);

        world.system("ImGuiDrawFrame")
            .kind<Phase::PreRender>()
            .run(ImGuiDrawFrameSystem);
    }

    void ImGuiModule::RegisterObserver(flecs::world& /* world */) {}
}
