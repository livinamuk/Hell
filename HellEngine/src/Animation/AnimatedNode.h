#pragma once
#include "Header.h"
#include "SQT.h"

namespace HellEngine
{
    class AnimatedNode
    {
    public: // methods
        AnimatedNode(const char* name);
        //bool IsAnimated();

    public: // fields
        std::vector<SQT> m_nodeKeys;
        const char* m_nodeName;
    };
}