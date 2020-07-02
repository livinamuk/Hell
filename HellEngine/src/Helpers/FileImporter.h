#pragma once
#include "header.h"
#include "Animation/SkinnedModel.h"

namespace HellEngine
{
    class FileImporter
    {


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


    public: // static functions
        static SkinnedModel* LoadSkinnedModel(const char* filename);
        static bool InitFromScene(SkinnedModel* skinnedModel, const aiScene* pScene, const string& Filename);

        static void InitMesh(SkinnedModel* skinnedModel, 
            unsigned int MeshIndex,
            const aiMesh* paiMesh,
            vector<glm::vec3>& Positions,
            vector<glm::vec3>& Normals,
            vector<glm::vec2>& TexCoords,
            vector<VertexBoneData>& Bones,
            vector<unsigned int>& Indices);
      
        static void LoadBones(SkinnedModel* skinnedModel, unsigned int MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);

        static void GrabSkeleton(SkinnedModel* skinnedModel, const aiNode* pNode, int parentIndex); // does the same as below, but using my new abstraction stuff
        static void FindBindPoseTransforms(SkinnedModel* skinnedModel, const aiNode* pNode); // for debugging


        static void LoadAnimation(SkinnedModel* skinnedModel, const char* Filename);

    public: // static variables
        static const aiScene* m_pScene;
        static Assimp::Importer m_Importer;


        
    };
}