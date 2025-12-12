// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

#include "Common/Phase.h"
#include "Common/Utils/Logging.h"

namespace Mcc
{

    template<typename T, typename M>
    void Scene<T, M>::Register(const flecs::world& world)
    {
        world.pipeline<LoadPipeline>().with(flecs::System).template with<T>().template with<Phase::OnLoad>().build();

        world.pipeline<QuitPipeline>().with(flecs::System).template with<T>().template with<Phase::OnQuit>().build();

        auto builder = world.pipeline<MainPipeline>().with(flecs::System);
        auto lambda  = [&]<typename S> {
            if constexpr (!std::is_same_v<T, S>)
            {
                builder.template without<S>();
            }
        };
        M::SceneList::template Apply<decltype(lambda)>(std::move(lambda));

        builder.template without<Phase::OnLoad>()
            .template without<Phase::OnQuit>()
            .template with<Phase>()
            .cascade(flecs::DependsOn)
            .build();

        world.observer<ActiveScene>().event(flecs::OnAdd).second<T>().run(OnLoad);

        world.observer<ActiveScene>().event(flecs::OnRemove).second<T>().run(OnQuit);
    }

    template<typename T, typename M>
    void Scene<T, M>::OnLoad(flecs::iter& it)
    {
        while (it.next()) {}
        it.world().run_post_frame(
            [](ecs_world_t* world, void*) {
                const auto ecs = flecs::world(world);
                if (ecs.get_info()->frame_count_total == 1)
                    ecs.run_pipeline<Mcc::LoadPipeline>();

                ecs.run_pipeline<LoadPipeline>();
                ecs.set_pipeline<MainPipeline>();
            },
            nullptr
        );
    }

    template<typename T, typename M>
    void Scene<T, M>::OnQuit(flecs::iter& it)
    {
        while (it.next()) {}
        if (auto ecs = it.world(); !ecs.should_quit())
            ecs.run_post_frame(
                [](ecs_world_t* world, void*) {
                    const auto ecs_ = flecs::world(world);
                    ecs_.run_pipeline<QuitPipeline>();

                    const auto root = ecs_.entity<SceneRoot>();
                    ecs_.delete_with(flecs::ChildOf, root);
                    root.clear();
                },
                nullptr
            );
    }

    template<typename... Ts>
    SceneImporter<Ts...>::SceneImporter(flecs::world& world)
    {
        MCC_LOG_DEBUG("Import SceneImporter...");
        world.module<SceneImporter>();

        world.component<ActiveScene>().add(flecs::Exclusive);
        world.component<SceneRoot>();

        world.atfini(
            [](ecs_world_t* worldPtr, void*) {
                const auto world_ = flecs::world(worldPtr);
                auto       cb     = [&]<typename T>() {
                    if (world_.has<ActiveScene, T>())
                    {
                        world_.run_pipeline<typename T::QuitPipeline>();
                        world_.delete_with(flecs::ChildOf, world_.entity<SceneRoot>());
                    }
                };
                SceneList::template Apply<decltype(cb)>(std::move(cb));
                world_.run_pipeline<QuitPipeline>();
            },
            nullptr
        );

        world.component<Phase>();
        world.component<Phase::OnLoad>().add<Phase>();
        world.component<Phase::OnQuit>().add<Phase>();
        world.component<Phase::OnSetup>().add<Phase>();
        world.component<Phase::OnUpdate>().add<Phase>().depends_on<Phase::OnSetup>();
        world.component<Phase::PostUpdate>().add<Phase>().depends_on<Phase::OnUpdate>();
        world.component<Phase::OnDrawGui>().add<Phase>().depends_on<Phase::PostUpdate>();
        world.component<Phase::PostDrawGui>().add<Phase>().depends_on<Phase::OnDrawGui>();
        world.component<Phase::OnClear>().add<Phase>().depends_on<Phase::PostDrawGui>();
        world.component<Phase::OnDraw>().add<Phase>().depends_on<Phase::OnClear>();
        world.component<Phase::PreRender>().add<Phase>().depends_on<Phase::OnDraw>();
        world.component<Phase::OnRender>().add<Phase>().depends_on<Phase::PreRender>();

        auto excludeScene = [](flecs::pipeline_builder<>& builder) {
            return [&]<typename T>() { builder.without<T>(); };
        };

        auto lBuilder = world.pipeline<LoadPipeline>().with(flecs::System);
        SceneList::template Apply<decltype(excludeScene(lBuilder))>(excludeScene(lBuilder));
        lBuilder.with<Phase::OnLoad>().build();

        auto qBuilder = world.pipeline<QuitPipeline>().with(flecs::System);
        SceneList::template Apply<decltype(excludeScene(qBuilder))>(excludeScene(qBuilder));
        qBuilder.with<Phase::OnQuit>().build();

        auto registerScene = [&]<typename T>() { world.import <SceneModule<T>>(); };
        SceneList::template Apply<decltype(registerScene)>(std::move(registerScene));

        world.system().kind(flecs::OnStart).run([](flecs::iter& it) {
            while (it.next()) {};
            it.world().add<ActiveScene, DefaultScene>();
        });
    }

}
