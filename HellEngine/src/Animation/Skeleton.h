#pragma once
#include "SkeletonJoint.h"

namespace HellEngine
{
    class Skeleton
    {
    public: // methods
        Skeleton();

    public: // fields
        //unsigned int m_jointCount;
        std::vector<SkeletonJoint> m_joints;
    };
}