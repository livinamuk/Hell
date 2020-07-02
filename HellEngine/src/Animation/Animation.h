#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include<assimp/postprocess.h>     // Post processing flags

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

        //aiScene* m_pAnimationScene;
        //Assimp::Importer m_AnimationImporter;


        std::vector<AnimatedNode> m_animatedNodes;
        std::map<const char*, unsigned int> m_NodeMapping; // maps a node name to its index
     
    };
}