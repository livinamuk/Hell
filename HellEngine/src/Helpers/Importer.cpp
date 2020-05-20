#include "hellpch.h"
#include "Importer.h"
#include "AssetManager.h"
#include "Util.h"
#include <assert.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace HellEngine
{
	fbxsdk::FbxManager* Importer::m_fbxMgr{ nullptr };
	
	void Importer::Init()
	{
		m_fbxMgr = fbxsdk::FbxManager::Create();
		fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(m_fbxMgr, IOSROOT);
		m_fbxMgr->SetIOSettings(ios);
	}
	
	void Importer::Terminate()
	{
		if (m_fbxMgr)
		{
			m_fbxMgr->Destroy();
			m_fbxMgr = nullptr;
		}
	}

	void Importer::LoadOBJModel(Model* model)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model->m_filePath.c_str())) {
			//throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes) 
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			for (const auto& index : shape.mesh.indices) {
				Vertex vertex = {};

				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.TexCoords = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				/*vertex.Normal = {
					attrib.vertices[3 * index.normal_index + 0],
					attrib.vertices[3 * index.normal_index + 1],
					attrib.vertices[3 * index.normal_index + 2]
				};*/

				//vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}

			for (int i = 0; i < indices.size(); i += 3) {
				Util::SetNormalsAndTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);

//				Util::SetTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);
			}

		//	std::cout << "verticies.size " << vertices.size() << "\n";
		//	std::cout << "indices.size " << indices.size() << "\n";

			Mesh* mesh = new Mesh(vertices, indices, shape.name.c_str());
			model->m_meshes.push_back(mesh);
			model->m_hasAnimation.push_back(false);
		}
	}

	void Importer::LoadFbxModel(Model* model)
	{
		assert(m_fbxMgr != nullptr);

		fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(m_fbxMgr, "HeImporter");
		bool status = importer->Initialize(model->m_filePath.c_str(), -1, m_fbxMgr->GetIOSettings());
		if (!status)
		{
			importer->Destroy();
			return;
			//return false;
		}

		fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(m_fbxMgr, "HeImportedScene");
		status = importer->Import(scene);

		if (!status)
		{
			importer->Destroy();
			return;
			//return false;
		}

		int fileMajor = 0, fileMinor = 0, fileRevision = 0;
		importer->GetFileVersion(fileMajor, fileMinor, fileRevision);
		importer->Destroy();

		if (scene->GetGlobalSettings().GetSystemUnit() == fbxsdk::FbxSystemUnit::cm)
		{
			const fbxsdk::FbxSystemUnit::ConversionOptions conversionOptions =
			{
			  true, /* mConvertRrsNodes */
			  true, /* mConvertLimits */
			  true, /* mConvertClusters */
			  true, /* mConvertLightIntensity */
			  true, /* mConvertPhotometricLProperties */
			  true  /* mConvertCameraClipPlanes */
			};

			// Convert the scene to meters using the defined options.
			fbxsdk::FbxSystemUnit::m.ConvertScene(scene, conversionOptions);
		}

		fbxsdk::FbxTime time;
		fbxsdk::FbxAnimLayer* animLayer = nullptr;

		fbxsdk::FbxAMatrix lDummyGlobalPosition;
		fbxsdk::FbxPose* pose = nullptr;

		int const nodeCount = scene->GetNodeCount();

		model->m_nodeNameMap.clear();
		model->m_nodeBlendMatrices.resize(nodeCount);
		model->m_clusterInitialMats.clear();
		model->m_clusterRelativeInitMats.clear();
		model->m_clusterLinkNames.clear();

		for (int i = 0; i < nodeCount; ++i)
		{
			fbxsdk::FbxNode* node = scene->GetNode(i);
			model->m_nodeBlendMatrices[i] = node->EvaluateGlobalTransform(time);
			std::string nameOnly(node->GetNameOnly().Buffer());
			model->m_nodeNameMap[nameOnly] = i;
		}

		for (int i = 0; i < nodeCount; ++i)
		{
			fbxsdk::FbxNode* node = scene->GetNode(i);
			fbxsdk::FbxMesh* fbxMesh = node->GetMesh();
			if (!fbxMesh)
			{
				continue;
			}
			int const cpCount = fbxMesh->GetControlPointsCount();

		//	std::cout << "cpCount: " << cpCount << "\n";

			const bool lHasVertexCache = fbxMesh->GetDeformerCount(FbxDeformer::eVertexCache);
			const bool lHasShape = fbxMesh->GetShapeCount() > 0;
			const bool lHasSkin = fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
			const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;

			model->m_hasAnimation.push_back(lHasDeformation);

			std::vector<int>* blendMatIndices = new std::vector<int>[cpCount];
			std::vector<float>* blendMatWeights = new std::vector<float>[cpCount];

			if (lHasDeformation)
			{
				model->m_clusterInitialMats.push_back(std::vector<fbxsdk::FbxAMatrix>());
				model->m_clusterRelativeInitMats.push_back(std::vector<fbxsdk::FbxAMatrix>());
				model->m_clusterLinkNames.push_back(std::vector<std::string>());

				//we need to get the number of clusters
				const int lSkinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
				int lClusterCount = 0;
				for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
				{
					lClusterCount += ((FbxSkin*)(fbxMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
				}
				if (lClusterCount)
				{
					// Deform the vertex array with the skin deformer.
					ComputeLinearDeformation(model, (int)model->m_clusterInitialMats.size() - 1, lDummyGlobalPosition, fbxMesh, time, pose, blendMatIndices, blendMatWeights);
				}
			}

			fbxsdk::FbxGeometryElementNormal* normalElement = fbxMesh->GetElementNormal();
			fbxsdk::FbxGeometryElementTangent* tangentElement = fbxMesh->GetElementTangent();
			fbxsdk::FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV();

			std::vector<glm::vec3> normals, tangents;
			std::vector<glm::vec2> uvs;

			Importer::LoadNormals(normalElement, fbxMesh, normals);
			Importer::LoadTangents(tangentElement, fbxMesh, tangents);
			Importer::LoadUVs(uvElement, fbxMesh, uvs);

			int const indexCount = fbxMesh->GetPolygonCount() * 3;
			bool const hasDataByPolygon = normals.size() == indexCount || tangents.size() == indexCount || uvs.size() == indexCount;

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		
			if (!hasDataByPolygon)
			{
				for (int j = 0; j < cpCount; ++j)
				{
					fbxsdk::FbxVector4 const p = fbxMesh->GetControlPointAt(j);
					Vertex v = {};
					v.Position = glm::vec3(p[0], p[1], p[2]);
					v.Normal = j < normals.size() ? normals[j] : glm::vec3(0, 0, 1);
					v.Tangent = j < tangents.size() ? tangents[j] : glm::vec3(1, 0, 0);
					v.Bitangent = glm::cross(v.Normal, v.Tangent);
					v.TexCoords = j < uvs.size() ? uvs[j] : glm::vec2(0, 0);

					if (lHasDeformation)
					{
						for (int k = 0; k < blendMatIndices[j].size() && k < 4; ++k)
						{
							v.BlendingIndex[k] = (float)blendMatIndices[j][k];
							v.BlendingWeight[k] = blendMatWeights[j][k];
						}
					}
					vertices.push_back(v);
				}

			
				int const* polyIndices = fbxMesh->GetPolygonVertices();
				for (int j = 0; j < indexCount; ++j)
				{
					int const index = polyIndices[j];
					indices.push_back(index);
				}
			}
			else
			{
				std::map<int, int> vertMappings;
				std::map<int, int> blendMatMappings;
				std::vector<glm::vec3> newVertices;
				std::vector<glm::vec3> newNorms;
				std::vector<glm::vec3> newTangents;
				std::vector<glm::vec2> newUvs;

				Importer::RemoveDuplicatedPoints(fbxMesh, normals, tangents, uvs, newVertices, newNorms, newTangents, newUvs, vertMappings, blendMatMappings);

				for (int j = 0; j < newVertices.size(); ++j)
				{
					Vertex v = {};
					v.Position = newVertices[j];
					v.Normal = j < newNorms.size() ? newNorms[j] : glm::vec3(0, 0, 1);
					v.Tangent = j < newTangents.size() ? newTangents[j] : glm::vec3(1, 0, 0);
					v.Bitangent = glm::cross(v.Normal, v.Tangent);
					v.TexCoords = j < uvs.size() ? glm::vec2(newUvs[j].x, 1.0f - newUvs[j].y) : glm::vec2(0, 0);

					if (lHasDeformation)
					{
						int const matIndex = blendMatMappings[j];
						for (int k = 0; k < blendMatIndices[matIndex].size() && k < 4; ++k)
						{
							v.BlendingIndex[k] = (float)blendMatIndices[matIndex][k];
							v.BlendingWeight[k] = blendMatWeights[matIndex][k];
						}
					}
					vertices.push_back(v);
				}
				for (int j = 0; j < indexCount; ++j)
				{
					int mappedIndex = vertMappings[j];
					indices.push_back(mappedIndex);
				}
			}
			delete[] blendMatIndices;
			delete[] blendMatWeights;

			for (int i = 0; i < indices.size(); i += 3)
				Util::SetTangentsFromVertices(&vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);

			const char* name = node->GetName();
			Mesh* mesh = new Mesh(vertices, indices, name);
			model->m_meshes.push_back(mesh);

			//std::cout << model->name << " (" << name << "): " << vertices.size() << " vertices\n";
		}
		if (model->m_sceneCamera)
		{
			model->m_sceneCamPos = model->m_sceneCamera->EvaluatePosition();
			model->m_sceneCamLookAt = model->m_sceneCamera->EvaluateLookAtPosition();
			model->m_sceneCamUpVec = model->m_sceneCamera->EvaluateUpDirection(model->m_sceneCamPos, model->m_sceneCamLookAt);
		}
		//return true;
	}

	void Importer::LoadNormals(fbxsdk::FbxGeometryElementNormal* normalElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec3>& output)
	{
		if (!normalElement)
			return;

		if (normalElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			for (int j = 0; j < mesh->GetControlPointsCount(); ++j)
			{
				int normalIndex = normalElement->GetReferenceMode() == FbxGeometryElement::eDirect ?
					j : normalElement->GetIndexArray().GetAt(j);

				FbxVector4 normal = normalElement->GetDirectArray().GetAt(normalIndex);
				output.push_back(glm::vec3(normal[0], normal[1], normal[2]));
			}
			return;
		}

		int indexByPolygonVertex = 0;
		for (int polyIndex = 0; polyIndex < mesh->GetPolygonCount(); ++polyIndex)
		{
			int const polygonSize = mesh->GetPolygonSize(polyIndex);
			for (int i = 0; i < polygonSize; i++)
			{
				int normalIndex = normalElement->GetReferenceMode() == FbxGeometryElement::eDirect ?
					indexByPolygonVertex : normalElement->GetIndexArray().GetAt(indexByPolygonVertex);

				FbxVector4 normal = normalElement->GetDirectArray().GetAt(normalIndex);
				output.push_back(glm::vec3(normal[0], normal[1], normal[2]));
				++indexByPolygonVertex;
			}
		}
	}

	void Importer::LoadTangents(fbxsdk::FbxGeometryElementTangent* tangentElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec3>& output)
	{
		if (!tangentElement)
		{
			return;
		}

		if (tangentElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			for (int j = 0; j < mesh->GetControlPointsCount(); ++j)
			{
				int tangentIndex = tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect ?
					j : tangentElement->GetIndexArray().GetAt(j);

				FbxVector4 tangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
				output.push_back(glm::vec3(tangent[0], tangent[1], tangent[2]));
			}

			return;
		}

		int indexByPolygonVertex = 0;
		for (int polyIndex = 0; polyIndex < mesh->GetPolygonCount(); ++polyIndex)
		{
			int const polygonSize = mesh->GetPolygonSize(polyIndex);
			for (int i = 0; i < polygonSize; i++)
			{
				int tangentIndex = tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect ?
					indexByPolygonVertex : tangentElement->GetIndexArray().GetAt(indexByPolygonVertex);

				FbxVector4 tangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
				output.push_back(glm::vec3(tangent[0], tangent[1], tangent[2]));
				++indexByPolygonVertex;
			}
		}
	}

	void Importer::LoadUVs(fbxsdk::FbxGeometryElementUV* uvElement, fbxsdk::FbxMesh* mesh, std::vector<glm::vec2>& output)
	{
		if (!uvElement)
		{
			return;
		}

		if (uvElement->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
		{
			for (int j = 0; j < mesh->GetControlPointsCount(); ++j)
			{
				int uvIndex = uvElement->GetReferenceMode() == FbxGeometryElement::eDirect ?
					j : uvElement->GetIndexArray().GetAt(j);

				FbxVector2 uv = uvElement->GetDirectArray().GetAt(uvIndex);
				output.push_back(glm::vec2(uv[0], uv[1]));
			}

			return;
		}

		FbxStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		const char* lUVName = NULL;
		if (lUVNames.GetCount())
		{
			lUVName = lUVNames[0];
		}

		int indexByPolygonVertex = 0;
		for (int polyIndex = 0; polyIndex < mesh->GetPolygonCount(); ++polyIndex)
		{
			int const polygonSize = mesh->GetPolygonSize(polyIndex);
			for (int i = 0; i < polygonSize; i++)
			{
				int uvIndex = 0;
				if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					uvIndex = indexByPolygonVertex;
				}
				else
				{
					uvIndex = uvElement->GetIndexArray().GetAt(indexByPolygonVertex);
				}

				FbxVector2 uv = uvElement->GetDirectArray().GetAt(uvIndex);
				FbxVector2 uv2;
				bool unmapped;
				mesh->GetPolygonVertexUV(polyIndex, i, lUVName, uv2, unmapped);
				output.push_back(glm::vec2(uv[0], uv[1]));
				++indexByPolygonVertex;
			}
		}
	}

	void Importer::RemoveDuplicatedPoints(fbxsdk::FbxMesh* mesh,
		std::vector<glm::vec3> const& normals,
		std::vector<glm::vec3> const& tangents,
		std::vector<glm::vec2> const& uvs,
		std::vector<glm::vec3>& newVertices,
		std::vector<glm::vec3>& newNormals,
		std::vector<glm::vec3>& newTangents,
		std::vector<glm::vec2>& newUvs,
		std::map<int, int>& newFaceMappings,
		std::map<int, int>& blendMatMappings)
	{
		for (int i = 0; i < uvs.size(); ++i)
		{
			int const origIdx = mesh->GetPolygonVertices()[i];
			bool duplicateFound = false;
			for (int v = 0; v < newUvs.size() && !duplicateFound; ++v)
			{
				if ((newUvs[v] == uvs[i]) && (newNormals[v] == normals[i]))
				{
					newFaceMappings[i] = v;
					duplicateFound = true;
				}
			}
			if (duplicateFound)
			{
				continue;
			}

			newFaceMappings[i] = newUvs.size();
			blendMatMappings[newUvs.size()] = origIdx;
			fbxsdk::FbxVector4 const& p = mesh->GetControlPointAt(origIdx);
			newVertices.push_back(glm::vec3(p[0], p[1], p[2]));
			newNormals.push_back(normals[i]);
			newUvs.push_back(uvs[i]);
		}
	}

	void Importer::ComputeLinearDeformation(Model* model, int index, fbxsdk::FbxAMatrix& globalPosition, fbxsdk::FbxMesh* mesh, fbxsdk::FbxTime& time,
		fbxsdk::FbxPose* pose, std::vector<int>* blendMatIndices, std::vector<float>* blendMatWeights)
	{
		// All the links must have the same link mode.
		FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

		int lVertexCount = mesh->GetControlPointsCount();

		FbxAMatrix lVertexTransformMatrix, lClusterTransformMatrix, lClusterRelativeInitMatrix;

		FbxSkin* lSkinDeformer = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
		if (lSkinDeformer != nullptr)
		{
			int lClusterCount = lSkinDeformer->GetClusterCount();
			int matrixIndex = 0;
			for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex)
			{
				FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
				if (!lCluster->GetLink())
				{
					continue;
				}

				ComputeClusterDeformation(globalPosition, mesh, lCluster, lVertexTransformMatrix,
					lClusterTransformMatrix, lClusterRelativeInitMatrix, time, pose);

				model->m_clusterInitialMats[index].push_back(lClusterTransformMatrix);
				model->m_clusterRelativeInitMats[index].push_back(lClusterRelativeInitMatrix);
				model->m_clusterLinkNames[index].push_back(std::string(lCluster->GetLink()->GetNameOnly().Buffer()));

				int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
				for (int k = 0; k < lVertexIndexCount; ++k)
				{
					int lIndex = lCluster->GetControlPointIndices()[k];

					// Sometimes, the mesh can have less points than at the time of the skinning
					// because a smooth operator was active when skinning but has been deactivated during export.
					if (lIndex >= lVertexCount)
						continue;

					double lWeight = lCluster->GetControlPointWeights()[k];

					if (lWeight == 0.0)
					{
						continue;
					}

					blendMatIndices[lIndex].push_back(matrixIndex);
					blendMatWeights[lIndex].push_back((float)lWeight);
				}
				++matrixIndex;
			}
		}
	}

	void Importer::ComputeClusterDeformation(fbxsdk::FbxAMatrix& pGlobalPosition, fbxsdk::FbxMesh* mesh, fbxsdk::FbxCluster* cluster,
		fbxsdk::FbxAMatrix& vertexTransformMatrix, fbxsdk::FbxAMatrix& clusterInitialMatrix,
		fbxsdk::FbxAMatrix& clusterRelativeInitMatrix, fbxsdk::FbxTime time, fbxsdk::FbxPose* pose)
	{
		FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();

		FbxAMatrix lReferenceGlobalInitPosition;
		FbxAMatrix lReferenceGlobalCurrentPosition;
		FbxAMatrix lAssociateGlobalInitPosition;
		FbxAMatrix lAssociateGlobalCurrentPosition;
		FbxAMatrix lClusterGlobalInitPosition;
		FbxAMatrix lClusterGlobalCurrentPosition;

		FbxAMatrix lReferenceGeometry;
		FbxAMatrix lAssociateGeometry;
		FbxAMatrix lClusterGeometry;

		FbxAMatrix lClusterRelativeInitPosition;
		FbxAMatrix lClusterRelativeCurrentPositionInverse;

		if (lClusterMode == FbxCluster::eAdditive && cluster->GetAssociateModel())
		{
			cluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
			// Geometric transform of the model
			lAssociateGeometry = GetGeometry(cluster->GetAssociateModel());
			lAssociateGlobalInitPosition *= lAssociateGeometry;
			lAssociateGlobalCurrentPosition = GetGlobalPosition(cluster->GetAssociateModel(), time, pose);

			cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
			// Multiply lReferenceGlobalInitPosition by Geometric Transformation
			lReferenceGeometry = GetGeometry(mesh->GetNode());
			lReferenceGlobalInitPosition *= lReferenceGeometry;
			lReferenceGlobalCurrentPosition = pGlobalPosition;

			// Get the link initial global position and the link current global position.
			cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
			// Multiply lClusterGlobalInitPosition by Geometric Transformation
			lClusterGeometry = GetGeometry(cluster->GetLink());
			lClusterGlobalInitPosition *= lClusterGeometry;
			lClusterGlobalCurrentPosition = GetGlobalPosition(cluster->GetLink(), time, pose);

			// Compute the shift of the link relative to the reference.
			//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
			clusterInitialMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse();
			vertexTransformMatrix = clusterInitialMatrix *
				lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
		}
		else
		{
			cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
			lReferenceGlobalCurrentPosition = pGlobalPosition;
			// Multiply lReferenceGlobalInitPosition by Geometric Transformation
			lReferenceGeometry = GetGeometry(mesh->GetNode());
			lReferenceGlobalInitPosition *= lReferenceGeometry;

			// Get the link initial global position and the link current global position.
			cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

			lClusterGlobalCurrentPosition = GetGlobalPosition(cluster->GetLink(), time, pose);

			// Compute the initial position of the link relative to the reference.
			lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

			clusterInitialMatrix = lReferenceGlobalCurrentPosition.Inverse();
			clusterRelativeInitMatrix = lClusterRelativeInitPosition;

			// Compute the shift of the link relative to the reference.
			vertexTransformMatrix = clusterInitialMatrix * lClusterGlobalCurrentPosition * clusterRelativeInitMatrix;
		}
	}

	fbxsdk::FbxAMatrix  Importer::GetGlobalPosition(fbxsdk::FbxNode* node, const fbxsdk::FbxTime& time,
		fbxsdk::FbxPose* pose, fbxsdk::FbxAMatrix* parentGlobalPosition)
	{

		FbxAMatrix lGlobalPosition;
		bool        lPositionFound = false;

		if (pose)
		{
			int lNodeIndex = pose->Find(node);

			if (lNodeIndex > -1)
			{
				// The bind pose is always a global matrix.
				// If we have a rest pose, we need to check if it is
				// stored in global or local space.
				if (pose->IsBindPose() || !pose->IsLocalMatrix(lNodeIndex))
				{
					lGlobalPosition = GetPoseMatrix(pose, lNodeIndex);
				}
				else
				{
					// We have a local matrix, we need to convert it to
					// a global space matrix.
					FbxAMatrix lParentGlobalPosition;

					if (parentGlobalPosition)
					{
						lParentGlobalPosition = *parentGlobalPosition;
					}
					else
					{
						if (node->GetParent())
						{
							lParentGlobalPosition = GetGlobalPosition(node->GetParent(), time, pose);
						}
					}

					FbxAMatrix lLocalPosition = GetPoseMatrix(pose, lNodeIndex);
					lGlobalPosition = lParentGlobalPosition * lLocalPosition;
				}

				lPositionFound = true;
			}
		}

		if (!lPositionFound)
		{
			// There is no pose entry for that node, get the current global position instead.

			// Ideally this would use parent global position and local position to compute the global position.
			// Unfortunately the equation 
			//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
			// does not hold when inheritance type is other than "Parent" (RSrs).
			// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
			lGlobalPosition = node->EvaluateGlobalTransform(time);
		}

		return lGlobalPosition;
	}

	fbxsdk::FbxAMatrix Importer::GetGeometry(fbxsdk::FbxNode* node)
	{
		const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxAMatrix(lT, lR, lS);
	}

	fbxsdk::FbxAMatrix Importer::GetPoseMatrix(fbxsdk::FbxPose* pose, int nodeIndex)
	{
		FbxAMatrix lPoseMatrix;
		FbxMatrix lMatrix = pose->GetMatrix(nodeIndex);

		memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

		return lPoseMatrix;
	}

	void Importer::AddAnimation(Model* model, std::string filePath, std::string animName, uint32_t startFrame, uint32_t endFrame)
	{
		if (!model) return;

		fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(m_fbxMgr, "HeImporter");
		bool status = importer->Initialize(filePath.c_str(), -1, m_fbxMgr->GetIOSettings());
		if (!status)
		{
			importer->Destroy();
			return;
		}

		fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(m_fbxMgr, "HeImportedScene");
		status = importer->Import(scene);

		if (!status)
		{
			importer->Destroy();
			return;
		}

		int fileMajor = 0, fileMinor = 0, fileRevision = 0;
		importer->GetFileVersion(fileMajor, fileMinor, fileRevision);
		importer->Destroy();

		if (scene->GetGlobalSettings().GetSystemUnit() == fbxsdk::FbxSystemUnit::cm)
		{
			const fbxsdk::FbxSystemUnit::ConversionOptions conversionOptions =
			{
			  true, /* mConvertRrsNodes */
			  true, /* mConvertLimits */
			  true, /* mConvertClusters */
			  true, /* mConvertLightIntensity */
			  true, /* mConvertPhotometricLProperties */
			  true  /* mConvertCameraClipPlanes */
			};

			// Convert the scene to meters using the defined options.
			fbxsdk::FbxSystemUnit::m.ConvertScene(scene, conversionOptions);
		}

		int numAnimations = scene->GetSrcObjectCount(fbxsdk::FbxCriteria::ObjectType(FbxAnimStack::ClassId));
		if (!numAnimations)
		{
			return;
		}

		int const animationIndex = 0;
		FbxAnimStack* animStack = (FbxAnimStack*)scene->GetSrcObject(fbxsdk::FbxCriteria::ObjectType(FbxAnimStack::ClassId), animationIndex);
		FbxAnimEvaluator* animEvaluator = scene->GetAnimationEvaluator();
		FbxString stackName = animStack->GetName(); // Get the name of the animation if needed

		FbxTakeInfo* takeInfo = scene->GetTakeInfo(stackName);
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

		Animation anim = {};
		anim.m_scene = scene;

		anim.m_startTime = start;
		anim.m_endTime = end;
		anim.m_currentTime = anim.m_startTime;

		anim.m_startTime.SetTime(0, 0, 0, startFrame);
		anim.m_currentTime = anim.m_startTime;
		/*if (endFrame == -1)
		{
			anim.m_endTime = end;
		}
		else
		{
			anim.m_endTime.SetTime(0, 0, 0, endFrame);
		}*/
		model->m_animations[animName] = anim;
		std::cout << "Loaded animation: " << animName << "\n";

		for (int i = 0; i < scene->GetNodeCount(); ++i)
		{
			fbxsdk::FbxNode* node = scene->GetNode(i);
			if (node->GetCamera())
			{
				model->m_sceneCamera = node->GetCamera();
				break;
			}
		}
	}
}