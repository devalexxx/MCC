// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MCC_CLIENT_SCENE_GAME_SCENE_SYSTEM_H
#define MCC_CLIENT_SCENE_GAME_SCENE_SYSTEM_H

#include <flecs.h>

namespace Mcc
{

    void OnEnterGameStateInGame(flecs::iter& it);
    void OnExitGameStateInGame(flecs::iter& it);
    void OnEnterGameStateShutdown(flecs::iter& it);

    void SetupStateSystem(flecs::iter& it);
    void ConnectToServerSystem(flecs::iter& it);
    void ClearGameInfoSystem(flecs::iter& it);
    void DisconnectFromServerSystem(flecs::iter& it);

    void DisplayEscapeMenuSystem(const flecs::iter& it, size_t);
    void DisplayLoadScreenSystem(const flecs::iter& it, size_t);
    void JoinPendingMeshTaskSystem(const flecs::iter& it, size_t);
    void DisplayDebugMenuSystem(flecs::iter& it);

}

#endif
