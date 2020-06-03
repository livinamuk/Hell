
#include "hellpch.h"
//#include <assert.h>
#include "SkinnedMesh.h"
#include "Helpers/Util.h"
//#include "HellEngine//Audio/Audio.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION		 1
#define TEX_COORD_LOCATION   2
#define TANGENT_LOCATION     3
#define BITANGENT_LOCATION   4
#define BONE_ID_LOCATION     5
#define BONE_WEIGHT_LOCATION 6

namespace HellEngine
{
    void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        return;

        // should never get here - more bones than we have space for
        assert(0);
    }

    SkinnedMesh::SkinnedMesh()
    {
        m_VAO = 0;
        ZERO_MEM(m_Buffers);
        m_NumBones = 0;
        m_pScene = NULL;
    }


    SkinnedMesh::~SkinnedMesh()
    {
        Clear();
    }


    void SkinnedMesh::Clear()
    {

        if (m_Buffers[0] != 0) {
            glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
        }

        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }
    }


    bool SkinnedMesh::LoadMesh(const string& Filename)
    {
        // Release the previously loaded mesh (if it exists)
        Clear();

        // Create the VAO
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Create the buffers for the vertices attributes
        glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

        bool Ret = false;

        m_pScene = m_Importer.ReadFile(Filename.c_str(),
            aiProcess_Triangulate |
            aiProcess_LimitBoneWeights |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            //	aiProcess_OptimizeGraph | WHATS THIS?
            aiProcess_CalcTangentSpace);
        
        //aiProcess_Triangulate |
         //   aiProcess_GenSmoothNormals |
        //    aiProcess_FlipUVs);

          //  aiProcess_PreTransformVertices); NEVER WORKED

        if (m_pScene) {
            m_GlobalInverseTransform = Util::aiMatrix4x4ToGlm(m_pScene->mRootNode->mTransformation);
            m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
            Ret = InitFromScene(m_pScene, Filename);
        }
        else {
            printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
        }

        // Make sure the VAO is not changed from the outside
        glBindVertexArray(0);


        std::cout << "Loaded: " << Filename << "\n";
        std::cout << " " << m_pScene->mNumMeshes << " meshes\n";
        std::cout << " " << this->m_NumBones << " bones\n\n";

        return Ret;
    }

    bool SkinnedMesh::LoadAnimation(const string& Filename)
    {
        // Release the previously loaded mesh (if it exists)
        Clear();

        // Create the VAO
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Create the buffers for the vertices attributes
        glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

        bool Ret = false;

        m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (m_pScene) {
            m_GlobalInverseTransform = Util::aiMatrix4x4ToGlm(m_pScene->mRootNode->mTransformation);
            m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
        //    Ret = InitFromScene(m_pScene, Filename);
            Ret = true;
            std::cout << "NO SCENE\n";
        }
        else {
            printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
        }

        // Make sure the VAO is not changed from the outside
        glBindVertexArray(0);

        return Ret;
    }


    bool SkinnedMesh::InitFromScene(const aiScene* pScene, const string& Filename)
    {
        m_Entries.resize(pScene->mNumMeshes);

        vector<glm::vec3> Positions;
        vector<glm::vec3> Normals;
        vector<glm::vec2> TexCoords;
        vector<VertexBoneData> Bones;
        vector<unsigned int> Indices;

        unsigned int NumVertices = 0;
        unsigned int NumIndices = 0;

        // Count the number of vertices and indices
        for (unsigned int i = 0; i < m_Entries.size(); i++) {
            m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
            m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
            m_Entries[i].BaseVertex = NumVertices;
            m_Entries[i].BaseIndex = NumIndices;
            m_Entries[i].MeshName = pScene->mMeshes[i]->mName.C_Str();

            NumVertices += pScene->mMeshes[i]->mNumVertices;
            NumIndices += m_Entries[i].NumIndices;
        }

        // Reserve space in the vectors for the vertex attributes and indices
        Positions.reserve(NumVertices);
        Normals.reserve(NumVertices);
        TexCoords.reserve(NumVertices);
        Bones.resize(NumVertices);
        Indices.reserve(NumIndices);

        // Initialize the meshes in the scene one by one
        for (unsigned int i = 0; i < m_Entries.size(); i++) {
            const aiMesh* paiMesh = pScene->mMeshes[i];
            InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
        }

        // CHRIS you added this. It's to find those fucking tranforms.
        std::cout << "\nLOADING MESH TRANFORMS\n";
        LoadMeshTransforms(m_pScene->mRootNode, glm::mat4(1));
        std::cout << "\n";

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(POSITION_LOCATION);
        glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(NORMAL_LOCATION);
        glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(TEX_COORD_LOCATION);
        glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(TANGENT_LOCATION);
        glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BITANGENT_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(BITANGENT_LOCATION);
        glVertexAttribPointer(BITANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(BONE_ID_LOCATION);
        glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
        glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
        glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);


        return true;
    }


    void SkinnedMesh::InitMesh(unsigned int MeshIndex,
        const aiMesh* paiMesh,
        vector<glm::vec3>& Positions,
        vector<glm::vec3>& Normals,
        vector<glm::vec2>& TexCoords,
        vector<VertexBoneData>& Bones,
        vector<unsigned int>& Indices)
    {
        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

        // Populate the vertex attribute vectors
        for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
            const aiVector3D* pPos = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

            Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
            Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
            TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
        }

        LoadBones(MeshIndex, paiMesh, Bones);

        // Populate the index buffer
        for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
            const aiFace& Face = paiMesh->mFaces[i];
            assert(Face.mNumIndices == 3);
            Indices.push_back(Face.mIndices[0]);
            Indices.push_back(Face.mIndices[1]);
            Indices.push_back(Face.mIndices[2]);
        }
    }


    void SkinnedMesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
    {
        for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
            unsigned int BoneIndex = 0;
            string BoneName(pMesh->mBones[i]->mName.data);

            if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
                // Allocate an index for a new bone
                BoneIndex = m_NumBones;
                m_NumBones++;
                BoneInfo bi;
                m_BoneInfo.push_back(bi);
                //m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;  CHECK HERE FOR ERROS MAYBE?
                m_BoneInfo[BoneIndex].BoneOffset = glm::transpose(Util::aiMatrix4x4ToGlm(pMesh->mBones[i]->mOffsetMatrix));
                m_BoneMapping[BoneName] = BoneIndex;
            }
            else {
                BoneIndex = m_BoneMapping[BoneName];
            }

            for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
                unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
                float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
                Bones[VertexID].AddBoneData(BoneIndex, Weight);
            }
        }
    }


    bool SkinnedMesh::InitMaterials(const aiScene* pScene, const string& Filename)
    {
        // Extract the directory part from the file name
        string::size_type SlashIndex = Filename.find_last_of("/");
        string Dir;

        if (SlashIndex == string::npos) {
            Dir = ".";
        }
        else if (SlashIndex == 0) {
            Dir = "/";
        }
        else {
            Dir = Filename.substr(0, SlashIndex);
        }

        bool Ret = true;
        return Ret;
    }


    void SkinnedMesh::Render(Shader* shader, const glm::mat4& modelMatrix)
    {
        glBindVertexArray(m_VAO);
        
        for (MeshEntry& mesh : m_Entries) {

            // If a model space mesh transformation exist, apply it.
            if (m_transforms.find(mesh.MeshName) == m_transforms.end())
                shader->setMat4("model", modelMatrix);
            else
                shader->setMat4("model", modelMatrix * m_transforms[mesh.MeshName]);

            glDrawElementsBaseVertex(GL_TRIANGLES, mesh.NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh.BaseIndex), mesh.BaseVertex);
        }
    }


    unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
            if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumRotationKeys > 0);

        for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
            if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumScalingKeys > 0);

        for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
            if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }

        unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
        unsigned int NextPositionIndex = (PositionIndex + 1);
        //assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
        float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }


    void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }

        unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        unsigned int NextRotationIndex = (RotationIndex + 1);
        //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
    }


    void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumScalingKeys == 1) {
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }

        unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
        unsigned int NextScalingIndex = (ScalingIndex + 1);
        //assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
        float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }


    // void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
    glm::mat4 SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
    {
        // Get this nodes transformation, convert it from aiMatrix4x4 to glm::mat4, and transpose for GL.
        glm::mat4 NodeTransformation(Util::aiMatrix4x4ToGlm(pNode->mTransformation));
        NodeTransformation = glm::transpose(NodeTransformation);

        // Get node name
        string NodeName(pNode->mName.data);

        // Is there an animation then modifty the NodeTransformation matrix.
        if (m_pScene->mAnimations != nullptr)
        {
            const aiAnimation* pAnimation = m_pScene->mAnimations[0];
            const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
            if (pNodeAnim) {
                // Interpolate scaling and generate scaling transformation matrix
                aiVector3D Scaling;
                CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
                glm::mat4 ScalingM;
                ScalingM = Util::Mat4InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
                // Interpolate rotation and generate rotation transformation matrix
                aiQuaternion RotationQ;
                CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
                glm::mat4 RotationM = Util::aiMatrix3x3ToGlm(RotationQ.GetMatrix());
                // Interpolate translation and generate translation transformation matrix
                aiVector3D Translation;
                CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
                glm::mat4 TranslationM;
                TranslationM = Util::Mat4InitTranslationTransform(Translation.x, Translation.y, Translation.z);
                // Combine the above transformations
                NodeTransformation = ScalingM * RotationM * TranslationM;
            }
        }



     

        glm::mat4 GlobalTransformation = NodeTransformation * ParentTransform;

        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) 
        {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; i++) {

            glm::mat4 childTransformation = ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);

            // ok
            // u need to look into the boneinfo array, 
            // by the name of this child node you will get the bone index, 
            // and retrieve its final transformation

            /*std::string childName = pNode->mChildren[i]->mName.C_Str();

            if (m_BoneMapping.find(childName) != m_BoneMapping.end())
            {
                unsigned int BoneIndex = m_BoneMapping[childName];
                childTransformation = m_BoneInfo[BoneIndex].FinalTransformation;
            }*/       

            Line line;
            line.start_pos = Util::GetTranslationFromMatrix(glm::transpose(GlobalTransformation * this->m_GlobalInverseTransform));
            line.end_pos = Util::GetTranslationFromMatrix(glm::transpose(childTransformation * this->m_GlobalInverseTransform));
            line.start_color = glm::vec3(1, 0, 1);
            line.end_color = glm::vec3(1, 1, 1);

            // if its a bone name, then add
            if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
                m_lines.push_back(line);        
        }

        // try and find a way to not have to return this mat4. 
        // its only so u can draw the lines. 
        // you can probably do it from the child name.

        return GlobalTransformation;
    }


    void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
    {
        if (m_pScene == nullptr)
            return;

        this->m_lines.clear();
        Transforms.clear();
        Transforms.resize(m_NumBones);

        // If there is an animation, figure out the frame time
        float AnimationTime = 0;
        if (m_pScene->mNumAnimations > 0) {
            float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
            float TimeInTicks = TimeInSeconds * TicksPerSecond;
            AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
        }
        // If not just continue with time = 0
        ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, glm::mat4(1));



        for (unsigned int i = 0; i < m_NumBones; i++) {
            Transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
    }

	void SkinnedMesh::SetBindPose(std::vector<glm::mat4>& Transforms)
	{
        Transforms.clear();
        Transforms.resize(m_NumBones);
        BoneTransform(0, Transforms);
	}


    const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
    {
        for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
            const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

            if (string(pNodeAnim->mNodeName.data) == NodeName) {
                return pNodeAnim;
            }
        }

        return NULL;
    }

    void SkinnedMesh::LoadMeshTransforms(const aiNode* pNode, const glm::mat4& ParentTransform)
    {
        std::string nodeName = pNode->mName.C_Str();
        glm::mat4 transformation = Util::aiMatrix4x4ToGlm(pNode->mTransformation);

        m_transforms[nodeName] = transformation * ParentTransform;

        // Climb down the tree
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
            LoadMeshTransforms(pNode->mChildren[i], transformation);
    }
}