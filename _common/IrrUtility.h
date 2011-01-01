#pragma once

#include <irrlicht.h>

using namespace irr;

inline bool fakeEquals(const core::vector3df& v1, const core::vector3df& v2)
{
    //! hack to solve the problem caused by
    //  .\source\game\CSceneNodeAnimatorFlyStraightOnTerrain

    return core::equals(v1.X, v2.X) && core::equals(v1.Z, v2.Z);
}

inline float fakeDistance(const core::vector3df& v1, const core::vector3df& v2)
{
    //! hack to solve the problem caused by
    f32 dx = v1.X-v2.X;
    f32 dz = v1.Z-v2.Z;

    return sqrt(dx*dx+dz*dz);
}

inline core::vector3df transform_vector(const vector3df& rotation, core::vector3df& vec)
{
    irr::core::matrix4 m;
    m.setRotationDegrees(rotation);
    m.transformVect(vec);

	return vec;
}
 