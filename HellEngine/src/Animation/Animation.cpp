#include "hellpch.h"
#include "Animation.h"

namespace HellEngine
{

    Animation::Animation(const char* Filename)
	{
        m_filename = Filename;

        std::string filepath = "res/models/";
        filepath += Filename;
        
        // Try and load the animation
        const aiScene* tempAnimScene = m_AnimationImporter.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        // Failed
        if (!tempAnimScene) {
            std::cout << "Could not load: " << Filename << "\n";
            assert(0);
        }

        // Success
        m_pAnimationScene = new aiScene(*tempAnimScene);
        if (m_pAnimationScene) {
            m_duration = m_pAnimationScene->mAnimations[0]->mDuration;
            std::cout << "Loaded animation: " << Filename << "\n";
        }

        // Some other error possibilty
        else {
            printf("Error parsing '%s': '%s'\n", Filename, m_AnimationImporter.GetErrorString());
            assert(0);
        }
	}

    Animation::~Animation()
    {
    }
}
