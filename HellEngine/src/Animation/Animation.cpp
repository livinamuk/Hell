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

        // need to create an animation clip.
        // need to fill it with animation poses.
        aiAnimation* aiAnim = m_pAnimationScene->mAnimations[0];

        
        std::cout << " numChannels:" << aiAnim->mNumChannels << "\n";

        // so iterate over each channel, and each channel is for each NODE aka joint.

        // Resize the vecotr big enough for each pose
        int nodeCount = aiAnim->mNumChannels;
        int poseCount = aiAnim->mChannels[0]->mNumPositionKeys;
        m_animationFrames.resize(poseCount);

        std::cout << " node (bone) count:  " << nodeCount << '\n';
        std::cout << " frame (pose) count: " << poseCount << '\n';

        // Iterate through each node, and then through it's pos/rot/scl keys and add em to this clip's pose vector


        for (int p = 0; p < poseCount; p++)
        {
            AnimationFrame animationPose;
            animationPose.m_jointPoses.resize(nodeCount);
            //animationPose.m_timeStamp = aiAnim->mChannels[0]->mPositionKeys[p].mTime; // Get the TimeStamp from the first bone (bone 0) position's mTime

            for (int n = 0; n < nodeCount; n++)
            {
                aiVectorKey pos = aiAnim->mChannels[n]->mPositionKeys[p];
                aiQuatKey rot = aiAnim->mChannels[n]->mRotationKeys[p];
                aiVectorKey scale = aiAnim->mChannels[n]->mScalingKeys[p];

                animationPose.m_jointPoses[n].positon = glm::vec3(pos.mValue.x, pos.mValue.y, pos.mValue.z);
                animationPose.m_jointPoses[n].rotation = glm::quat(rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z);
                animationPose.m_jointPoses[n].scale = scale.mValue.x;
                animationPose.m_jointPoses[n].timeStamp = aiAnim->mChannels[n]->mPositionKeys[p].mTime;
                animationPose.m_jointPoses[n].jointName = aiAnim->mChannels[n]->mNodeName.C_Str();
                m_animationFrames[p] = animationPose;
            }
        //    std::cout << p << ": " << animationPose.m_jointPoses.size() << "\n";

          
        }

/*
        for (int i = 0; i < aiAnim->mNumChannels; i++)
        {
            for (int j = 0; j < poseCount; j++)
            {

            }

            const aiNodeAnim* pNodeAnim = aiAnim->mChannels[i];

            std::cout << "  Node name:" << pNodeAnim->mNodeName.C_Str() << "\n";
            std::cout << "  pos keys: " << pNodeAnim->mNumPositionKeys << "\n";
            std::cout << "  rot keys: " << pNodeAnim->mNumPositionKeys << "\n";
            std::cout << "  scl keys: " << pNodeAnim->mNumScalingKeys << "\n";

        }*/


        
	}

    Animation::~Animation()
    {
    }
}
