#pragma once
#include "SQT.h"

namespace HellEngine
{
    class AnimationFrame
    {
    public: // methods
        AnimationFrame();

    public: // fields
        std::vector<SQT> m_jointPoses;
        //float m_timeStamp;
    };
}