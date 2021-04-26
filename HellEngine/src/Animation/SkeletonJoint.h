#pragma once

namespace HellEngine
{
    class SkeletonJoint
    {
    public: // methods
        SkeletonJoint();

    public: // fields
        const char* m_name;
        int m_parentIndex = -1;
        glm::mat4 m_inverseBindTransform;
        glm::mat4 m_currentFinalTransform;
    };
}