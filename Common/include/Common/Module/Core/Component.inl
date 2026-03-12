// Copyright (c) 2025 devalexxx
// Distributed under the MIT License.
// https://opensource.org/licenses/MIT

namespace glm
{

    template<class Archive>
    void serialize(Archive& ar, vec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    void serialize(Archive& ar, uvec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    void serialize(Archive& ar, ivec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }

    template<class Archive>
    void serialize(Archive& ar, quat& quat)
    {
        ar(quat.x, quat.y, quat.z, quat.w);
    }

}

namespace Mcc
{

    template<class Archive>
    void serialize(Archive& ar, CTransform& transform)
    {
        ar(transform.position, transform.rotation, transform.scale);
    }

}