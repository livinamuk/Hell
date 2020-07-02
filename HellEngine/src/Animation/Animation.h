#pragma once
#include "AnimatedNode.h"

namespace HellEngine
{
    class Animation
    {
    public: // methods
        Animation(const char* Filename);
        ~Animation();

    public: // fields                
        float m_duration;
        float m_ticksPerSecond;
        const char* m_filename;

        std::vector<AnimatedNode> m_animatedNodes;
        std::map<const char*, unsigned int> m_NodeMapping; // maps a node name to its index
    };
}