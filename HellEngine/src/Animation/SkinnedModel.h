#pragma once

#include <map>
#include <vector>
#include <assert.h>
//#include <assimp/Importer.hpp>      // C++ importer interface
//#include <assimp/scene.h>       // Output data structure
//#include <assimp/postprocess.h> // Post processing flags

#include "header.h"

#include "Animation/Animation.h"
#include "Renderer/Mesh.h"
#include "Animation/Skeleton.h"

using namespace std;

namespace HellEngine
{
    class SkinnedModel
    {
    public:
        SkinnedModel();
       // SkinnedModel(const char* filename);
        ~SkinnedModel();

       // bool LoadMesh(const string& Filename);
   //     bool LoadAnimation(const char* Filename);
        void Render(Shader* shader, const glm::mat4& modelMatrix);

        glm::mat4 m_CameraMatrix = glm::mat4(1);

        Skeleton m_skeleton;

        const char* m_filename;



        std::vector<Animation*> m_animations;

        void BoneTransform(float Time, vector<glm::mat4>& Transforms, vector<glm::mat4>& DebugAnimatedTransforms);
   


    public:

        struct BoneInfo
        {
            glm::mat4 BoneOffset;
            glm::mat4 FinalTransformation;
            glm::mat4 DebugMatrix_AnimatedTransform;
            glm::mat4 DebugMatrix_BindPose;
            std::string BoneName;

            BoneInfo()
            {
                BoneOffset = glm::mat4(0);
                FinalTransformation = glm::mat4(0);
                DebugMatrix_BindPose = glm::mat4(1);
                DebugMatrix_AnimatedTransform = glm::mat4(1);
            }
        };



        void CalcInterpolatedScaling(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
        void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const AnimatedNode* animatedNode);
        void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
        int FindAnimatedNodeIndex(float AnimationTime, const AnimatedNode* animatedNode);
        
        const AnimatedNode* FindAnimatedNode(Animation* animation, const char* NodeName);
     
        
    public:

     

    public:
        GLuint m_VAO;
        GLuint m_Buffers[NUM_VBs];

        struct MeshEntry {
            MeshEntry()
            {
                NumIndices = 0;
                BaseVertex = 0;
                BaseIndex = 0;
                MeshName = "No name";
            }

            unsigned int NumIndices;
            unsigned int BaseVertex;
            unsigned int BaseIndex;
            std::string MeshName;
        };

    public:
        vector<MeshEntry> m_meshEntries;
        map<string, unsigned int> m_BoneMapping; // maps a bone name to its index

    public:
        unsigned int m_NumBones;
        vector<BoneInfo> m_BoneInfo;
        glm::mat4 m_GlobalInverseTransform;

        unsigned int currentAnimationIndex = 0;
    };
}