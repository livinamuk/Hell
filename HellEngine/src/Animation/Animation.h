#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include<assimp/postprocess.h>     // Post processing flags

#include "AnimationFrame.h"

namespace HellEngine
{
    class Animation
    {
    public: // methods
        Animation(const char* Filename);
        ~Animation();

    public: // fields
        double m_duration;
        const char* m_filename;
        aiScene* m_pAnimationScene;
        Assimp::Importer m_AnimationImporter;


        // replace these later
        //const char* m_name;
        //float m_duration;
        std::vector<AnimationFrame> m_animationFrames;

        
    };
}