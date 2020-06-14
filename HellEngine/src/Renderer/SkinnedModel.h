#pragma once

#include <map>
#include <vector>
#include <assert.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "header.h"
#include "Animation/Animation.h"

using namespace std;

namespace HellEngine
{
    class SkinnedModel
    {
    public:
        SkinnedModel();
        SkinnedModel(const char* filename);
        ~SkinnedModel();

        bool LoadMesh(const string& Filename);
        bool LoadAnimation(const char* Filename);
        void Render(Shader* shader, const glm::mat4& modelMatrix);
        void SetCamereaMatrix(const aiNode* pNode);

        glm::mat4 m_CameraMatrix = glm::mat4(1);
        std::vector<glm::mat4> m_animatedTransforms;

        const char* m_filename;

       //std::vector<Animation> m_animations;
        std::vector<Animation*> m_animations;

        void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);

    private:

        struct BoneInfo
        {
            glm::mat4 BoneOffset;
            glm::mat4 FinalTransformation;
            glm::mat4 TangentSpaceDebugMatrix;
            std::string BoneName;

            BoneInfo()
            {
                BoneOffset = glm::mat4(0);
                FinalTransformation = glm::mat4(0);
            }
        };

        struct VertexBoneData
        {
            unsigned int IDs[4];
            float Weights[4];

            VertexBoneData()
            {
                Reset();
            };

            void Reset()
            {
                ZERO_MEM(IDs);
                ZERO_MEM(Weights);
            }

            void AddBoneData(unsigned int BoneID, float Weight);
        };

        void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
        int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
        const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
        void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
        bool InitFromScene(const aiScene* pScene, const string& Filename);
        void InitMesh(unsigned int MeshIndex,
            const aiMesh* paiMesh,
            vector<glm::vec3>& Positions,
            vector<glm::vec3>& Normals,
            vector<glm::vec2>& TexCoords,
            vector<VertexBoneData>& Bones,
            vector<unsigned int>& Indices);
        void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
        bool InitMaterials(const aiScene* pScene, const string& Filename);
        void Clear();

        enum VB_TYPES {
            INDEX_BUFFER,
            POS_VB,
            NORMAL_VB,
            TEXCOORD_VB,
            TANGENT_VB,
            BITANGENT_VB,
            BONE_VB,
            NUM_VBs
        };

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
            unsigned int MaterialIndex;
            std::string MeshName;
        };

        vector<MeshEntry> m_Entries;

    public:
        map<string, unsigned int> m_BoneMapping; // maps a bone name to its index

    public:
        unsigned int m_NumBones;
        vector<BoneInfo> m_BoneInfo;
        glm::mat4 m_GlobalInverseTransform;

        const aiScene* m_pScene;
   //     const aiScene* m_pAnimationScene;
        Assimp::Importer m_Importer;
   //     Assimp::Importer m_AnimationImporter;


       // std::vector<const aiScene*> m_animatedScenes;

      //  std::vector<aiAnimation*> m_animations;
        unsigned int currentAnimationIndex = 0;
    };
}