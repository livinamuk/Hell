
#include <map>
#include <vector>
#include <assert.h>
//#include <GL/glew.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "header.h"
//#include "math_3d.h"
//#include "texture.h"

using namespace std;

namespace HellEngine
{
    class SkinnedMesh
    {
    public:
        SkinnedMesh();

        ~SkinnedMesh();

        bool LoadMesh(const string& Filename);
        bool LoadAnimation(const string& Filename);

        void Render();

        unsigned int NumBones() const
        {
            return m_NumBones;
        }

        void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);

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

        void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
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
            }

            unsigned int NumIndices;
            unsigned int BaseVertex;
            unsigned int BaseIndex;
            unsigned int MaterialIndex;
        };

        vector<MeshEntry> m_Entries;
        //vector<Texture*> m_Textures;

        map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
        unsigned int m_NumBones;
        vector<BoneInfo> m_BoneInfo;
        glm::mat4 m_GlobalInverseTransform;

        const aiScene* m_pScene;
        const aiScene* m_pAnimationScene;
        Assimp::Importer m_Importer;
        Assimp::Importer m_AnimationImporter;
    };
}