#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "fbxsdk.h"

namespace HellEngine
{
	class Importer
	{
	public: // Fucntions
		static void Init();
		static void Terminate();
		
		static void LoadNormals(fbxsdk::FbxGeometryElementNormal* normalElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec3>& output);
		static void LoadTangents(fbxsdk::FbxGeometryElementTangent* tangentElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec3>& output);
		static void LoadUVs(fbxsdk::FbxGeometryElementUV* uvElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec2>& output);
		static void AddAnimation(Model* model, std::string filePath, std::string animName, uint32_t startFrame, uint32_t endFrame);

		static void RemoveDuplicatedPoints(fbxsdk::FbxMesh* mesh,
			std::vector<glm::vec3> const& normals,
			std::vector<glm::vec3> const& tangents,
			std::vector<glm::vec2> const& uvs,
			std::vector<glm::vec3>& newVertices,
			std::vector<glm::vec3>& newNormals,
			std::vector<glm::vec3>& newTangents,
			std::vector<glm::vec2>& newUvs,
			std::map<int, int>& newFaceMappings,
			std::map<int, int>& blendMatMappings);

		static void ComputeLinearDeformation(Model *model, int index, fbxsdk::FbxAMatrix& globalPosition, fbxsdk::FbxMesh* mesh, fbxsdk::FbxTime& time,
			fbxsdk::FbxPose* pose, std::vector<int>* blendMatIndices, std::vector<float>* blendMatWeights);

		static void ComputeClusterDeformation(fbxsdk::FbxAMatrix& pGlobalPosition, fbxsdk::FbxMesh* mesh, fbxsdk::FbxCluster* cluster,
			fbxsdk::FbxAMatrix& vertexTransformMatrix, fbxsdk::FbxAMatrix& clusterInitialMatrix,
			fbxsdk::FbxAMatrix& clusterRelativeInitMatrix, fbxsdk::FbxTime time, fbxsdk::FbxPose* pose);

		static fbxsdk::FbxAMatrix  GetGlobalPosition(fbxsdk::FbxNode* node, const fbxsdk::FbxTime& time,
			fbxsdk::FbxPose* pose, fbxsdk::FbxAMatrix* parentGlobalPosition = nullptr);

		static fbxsdk::FbxAMatrix GetGeometry(fbxsdk::FbxNode* node);
		static fbxsdk::FbxAMatrix GetPoseMatrix(fbxsdk::FbxPose* pose, int nodeIndex);

		static void LoadFbxModel(Model* model);
		static void LoadOBJModel(Model* model);

	public: // Variables
		static fbxsdk::FbxManager* m_fbxMgr;
	};
}