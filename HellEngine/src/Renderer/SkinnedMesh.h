#pragma once
#include <map>
#include <vector>
#include <assert.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include "header.h"
#include <map>

namespace HellEngine
{
    using namespace std;

    class SkinnedMesh
    {
    public: // methods
        bool m_applyMeshTransforms;
        SkinnedMesh();
        ~SkinnedMesh();
        bool LoadMesh(const std::string& Filename);
        bool LoadAnimation(const std::string& Filename);
        void Render(Shader* shader, const glm::mat4& modelMatrix);
        unsigned int GetBoneCount() const {return m_NumBones;}
        void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);
        
        void SetBindPose(std::vector<glm::mat4>& Transforms);


        void LoadMeshTransforms(const aiNode* pNode, const glm::mat4& ParentTransform);
        std::map<std::string, glm::mat4> m_transforms;

    private:
#define NUM_BONES_PER_VEREX 4

        struct BoneInfo
        {
            glm::mat4 BoneOffset;
            glm::mat4 FinalTransformation;

            BoneInfo()
            {
                BoneOffset = glm::mat4(0);
                FinalTransformation = glm::mat4(0);
            }
        };

        struct VertexBoneData
        {
            unsigned int IDs[NUM_BONES_PER_VEREX];
            float Weights[NUM_BONES_PER_VEREX];

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

    private:
        void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
        const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
       // void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
        glm::mat4 ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
        bool InitFromScene(const aiScene* pScene, const std::string& Filename);
        void InitMesh(unsigned int MeshIndex,
            const aiMesh* paiMesh,
            std::vector<glm::vec3>& Positions,
            std::vector<glm::vec3>& Normals,
            std::vector<glm::vec2>& TexCoords,
            std::vector<VertexBoneData>& Bones,
            std::vector<unsigned int>& Indices);
        void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
        bool InitMaterials(const aiScene* pScene, const std::string& Filename);
        void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

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
                MaterialIndex = INVALID_MATERIAL;
                MeshName = "No name";
            }

            unsigned int NumIndices;
            unsigned int BaseVertex;
            unsigned int BaseIndex;
            unsigned int MaterialIndex;
            std::string MeshName;
        };

        std::vector<MeshEntry> m_Entries;
        //vector<Texture*> m_Textures;

        std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
        unsigned int m_NumBones;
        std::vector<BoneInfo> m_BoneInfo;
        glm::mat4 m_GlobalInverseTransform;

        const aiScene* m_pScene;
        Assimp::Importer m_Importer;

    public:
        std::vector<Line> m_lines;
    };
}