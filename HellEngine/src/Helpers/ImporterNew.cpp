#include "hellpch.h"
#include "ImporterNew.h"
#include "AssetManager.h"
#include "Importer.h"
#include "Util.h"
#include <assert.h>

namespace HellEngine
{
	void ImporterNew::LoadFbxModel(Model* model)
	{
		// Create the FBX SDK manager
		fbxsdk::FbxManager* lSdkManager = FbxManager::Create();

		// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		// Import options determine what kind of data is to be imported.
		// True is the default, but here we’ll set some to true explicitly, and others to false.
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, false);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, true);
		(*(lSdkManager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

		// Initialize the importer.
		bool lImportStatus = lImporter->Initialize(model->m_filePath.c_str(), -1, lSdkManager->GetIOSettings());

		if (!lImportStatus) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
			exit(-1);
		}
		else
			std::cout << "I think we're loading something...\n";

		// Create a new scene so it can be populated by the imported file.
		FbxScene* scene = FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(scene);

		// The file has been imported; we can get rid of the importer.
		lImporter->Destroy();

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

		std::cout << "NodeCount: " << nodeCount << "\n";

		FbxNode* pFbxRootNode = scene->GetRootNode();

		if (pFbxRootNode)
		{
			for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
			{
				FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
		
				// Early bail out
				if (pFbxChildNode->GetNodeAttribute() == NULL)
					continue;
				if (pFbxChildNode->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eMesh)
					continue;

				FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();		
				FbxVector4* vertexElement = pMesh->GetControlPoints();
				FbxGeometryElementNormal* normalElement = pMesh->GetElementNormal();
				FbxGeometryElementTangent* tangentElement = pMesh->GetElementTangent();
				FbxGeometryElementUV* uvElement = pMesh->GetElementUV();
				
				const bool lHasVertexCache = pMesh->GetDeformerCount(FbxDeformer::eVertexCache);
				const bool lHasShape = pMesh->GetShapeCount() > 0;
				const bool lHasSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
				const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;

				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;
				std::vector<glm::vec3> normals, tangents;
				std::vector<glm::vec2> uvs;
				const char* name = pFbxChildNode->GetName();

				Importer::LoadNormals(normalElement, pMesh, normals);
				Importer::LoadTangents(tangentElement, pMesh, tangents);
				Importer::LoadUVs(uvElement, pMesh, uvs);

				for (int j = 0; j < pMesh->GetPolygonCount(); j++)
				{
					int iNumVertices = pMesh->GetPolygonSize(j);
					assert(iNumVertices == 3);

					for (int k = 0; k < iNumVertices; k++) {
						int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

						Vertex vertex;
						vertex.Position.x = (float)vertexElement[iControlPointIndex].mData[0];
						vertex.Position.y = (float)vertexElement[iControlPointIndex].mData[1];
						vertex.Position.z = (float)vertexElement[iControlPointIndex].mData[2];
						vertex.Normal = j < normals.size() ? normals[j] : glm::vec3(0, 0, 1);
						vertex.Tangent = j < tangents.size() ? tangents[j] : glm::vec3(1, 0, 0);
						vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
						vertex.TexCoords = j < uvs.size() ? uvs[j] : glm::vec2(0, 0);

						vertices.push_back(vertex);
						indices.push_back(indices.size());
					}
				}

				std::cout << " Vertices: " << vertices.size() << "\n";

				Mesh* mesh = new Mesh(vertices, indices, name);
				model->m_meshes.push_back(mesh);
				model->m_hasAnimation.push_back(false);
			}
		}


	}
}