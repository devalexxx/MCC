// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace Mcc
{

    template<typename T, typename C>
    flecs::opaque<C, T> VectorReflection(flecs::world& world)
    {
        return flecs::opaque<C, T>(world)
            .as_type(world.vector<T>())
            .serialize([](const flecs::serializer* s, const C* data) {
                for (const auto& el: *data)
                {
                    s->value(el);
                }
                return 0;
            })
            .count([](const C* data) {
                return data->size();
            })
            .resize([](C* data, size_t size) {
                data->resize(size);
            })
            .ensure_element([](C* data, size_t el) {
                if (data->size() <= el)
                {
                    data->resize(el + 1);
                }

                return &data->data()[el];
            });
    }

    template<typename T, typename C>
    flecs::opaque<C, T> DequeReflection(flecs::world& world)
    {
        return flecs::opaque<C, T>(world)
            .as_type(world.vector<T>())
            .serialize([](const flecs::serializer* s, const C* data) {
                for (const auto& el: *data)
                {
                    s->value(el);
                }
                return 0;
            })
            .count([](const C* data) {
                return data->size();
            })
            .resize([](C* data, size_t size) {
                data->resize(size);
            })
            .ensure_element([](C* data, size_t el) {
                if (data->size() <= el)
                {
                    data->resize(el + 1);
                }

                return &data->at(el);
            });
    }

    template<typename T, typename C>
    flecs::opaque<C, T> USetReflection(flecs::world& world)
    {
        return flecs::opaque<C, T>(world)
            .as_type(world.vector<T>())
            .serialize([](const flecs::serializer* s, const C* data) {
                for (const auto& el: *data)
                {
                    s->value(el);
                }
                return 0;
            })
            .count([](const C* data) {
                return data->size();
            });
    }

    template<typename K, typename V, typename T, typename C>
    flecs::opaque<C, T> UMapReflection(flecs::world& world)
    {
        world.component<T>()
            .template member<K>("first")
            .template member<V>("second");

        return flecs::opaque<C, T>(world)
            .as_type(world.vector<T>())
            .serialize([](const flecs::serializer* s, const C* data) {
                for (const auto& el: *data)
                {
                    s->value(el);
                }
                return 0;
            })
            .count([](const C* data) {
                return data->size();
            });
    }


}
