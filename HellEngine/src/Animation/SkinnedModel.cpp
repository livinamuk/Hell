
#include "hellpch.h"
//#include <assert.h>
#include "SkinnedModel.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION		 1
#define TEX_COORD_LOCATION   2
#define TANGENT_LOCATION     3
#define BITANGENT_LOCATION   4
#define BONE_ID_LOCATION     5
#define BONE_WEIGHT_LOCATION 6

namespace HellEngine
{
    void SkinnedModel::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }

        //   std::cout << "BoneID: " << BoneID << "\n";
       //    std::cout << "Weight: " << Weight << "\n";
        return;

        // should never get here - more bones than we have space for
        assert(0);
    }

    SkinnedModel::SkinnedModel()
    {
        m_VAO = 0;
        ZERO_MEM(m_Buffers);
        m_NumBones = 0;
        m_pScene = NULL;
    }

    SkinnedModel::SkinnedModel(const char* filename)
    {
        m_VAO = 0;
        ZERO_MEM(m_Buffers);
        m_NumBones = 0;
        m_pScene = NULL;
        m_filename = filename;
        LoadMesh(filename);
    }


    SkinnedModel::~SkinnedModel()
    {
        Clear();
    }


    void SkinnedModel::Clear()
    {

        if (m_Buffers[0] != 0) {
            glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
        }

        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }
    }


    bool SkinnedModel::LoadMesh(const string& Filename)
    {
        // Release the previously loaded mesh (if it exists)
        Clear();

        // Create the VAO
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Create the buffers for the vertices attributes
        glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

        bool Ret = false;

        std::string filepath = "res/models/";
        filepath += Filename;
      //   m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); would be great but fucks up rotations on the camera node

        const aiScene* tempScene = m_Importer.ReadFile(filepath.c_str(),  aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
       
        //Getting corrupted later. So deep copying now.
        m_pScene = new aiScene(*tempScene);

        if (m_pScene) {
            m_GlobalInverseTransform = Util::aiMatrix4x4ToGlm(m_pScene->mRootNode->mTransformation);
            //   m_GlobalInverseTransform = Util::SwitchCoordSystem(m_GlobalInverseTransform);
            m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

            Ret = InitFromScene(m_pScene, Filename);
        }
        else {
            printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
        }

        std::cout << "Loaded: " << Filename << "\n";
        std::cout << " " << m_pScene->mNumMeshes << " meshes\n";
        std::cout << " " << this->m_NumBones << " bones\n\n";

        if (m_pScene->mNumCameras > 0)
            aiCamera* m_camera = m_pScene->mCameras[0];
       

        
        //if (m_NumBones == 0)
      //      return;

 //       m_BindPoseTransforms.resize(m_NumBones);
        FindBindPoseTransforms(m_pScene->mRootNode); // only used for debugging at this point


        GrabSkeleton(m_pScene->mRootNode, -1);


        /*for (int i = 0; i < m_skeleton.m_joints.size(); i++)
        {
            std::cout << "\n";
            std::cout << i << ": " << m_skeleton.m_joints[i].m_name << "\n";
            std::cout << " parent: " << m_skeleton.m_joints[i].m_parentIndex<< "\n";
            Util::PrintMat4(m_skeleton.m_joints[i].m_inverseBindTransform);

        }*/



        std::cout << "BONEINFO SIZEEEEE: " << m_BoneInfo.size() << "\n";
        return Ret;

    }

    void SkinnedModel::GrabSkeleton(const aiNode* pNode, int parentIndex)
    {
        // Ok. So this function walks the node tree and makes a direct copy and that becomes your custom skeleton.
        // This includes camera nodes and all that fbx pre rotation/translation bullshit. Hopefully assimp will fix that one day.

        SkeletonJoint joint;
        joint.m_name = pNode->mName.data;
        joint.m_inverseBindTransform = Util::aiMatrix4x4ToGlm(pNode->mTransformation);
        joint.m_parentIndex = parentIndex;
        
        parentIndex = m_skeleton.m_joints.size(); // don't do your head in with why this works, just be thankful it does.

        m_skeleton.m_joints.push_back(joint);


        /*std::string NodeName(pNode->mName.data);
        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].DebugMatrix_BindPose = inverse(m_BoneInfo[BoneIndex].BoneOffset);
        }*/

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
            GrabSkeleton(pNode->mChildren[i], parentIndex);
    }

    void SkinnedModel::FindBindPoseTransforms(const aiNode* pNode)
    {
        std::string NodeName(pNode->mName.data);

        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].DebugMatrix_BindPose = inverse(m_BoneInfo[BoneIndex].BoneOffset);
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
            FindBindPoseTransforms(pNode->mChildren[i]);
    }

    bool SkinnedModel::InitFromScene(const aiScene* pScene, const string& Filename)
    {
        m_meshEntries.resize(pScene->mNumMeshes);

        std::cout << "MESH COUNT: " << pScene->mNumMeshes << "\n";

        vector<glm::vec3> Positions;
        vector<glm::vec3> Normals;
        vector<glm::vec2> TexCoords;
        vector<VertexBoneData> Bones;
        vector<unsigned int> Indices;

        unsigned int NumVertices = 0;
        unsigned int NumIndices = 0;

        // Count the number of vertices and indices
        for (unsigned int i = 0; i < m_meshEntries.size(); i++) 
        {
            m_meshEntries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
            m_meshEntries[i].BaseVertex = NumVertices;
            m_meshEntries[i].BaseIndex = NumIndices;
            m_meshEntries[i].MeshName = pScene->mMeshes[i]->mName.C_Str();

            NumVertices += pScene->mMeshes[i]->mNumVertices;
            NumIndices += m_meshEntries[i].NumIndices;
        }

        // Reserve space in the vectors for the vertex attributes and indices
        Positions.reserve(NumVertices);
        Normals.reserve(NumVertices);
        TexCoords.reserve(NumVertices);
        Bones.resize(NumVertices);
        Indices.reserve(NumIndices);



        // Initialize the meshes in the scene one by one
        for (unsigned int i = 0; i < m_meshEntries.size(); i++) {
            const aiMesh* paiMesh = pScene->mMeshes[i];
            InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
        }

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

        std::cout << "INDICES.size: " << Indices.size() << "\n";

        return true;
    }

    void SkinnedModel::InitMesh(unsigned int MeshIndex,
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


            // this is my shit. my own copy of the data. 
            // umm deal with this later. as in removing all reliance on assimp data structures..
            // Also keep in mind this is only half complete and doesn't have bone shit.
            // you are just using it to add the mesh to bullet for blood lol.

            Vertex v;
            v.Position = Positions[i];
            v.Normal = Normals[i];
            v.TexCoords = TexCoords[i];
            //m_vertices.push_back(v);
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

    bool SkinnedModel::LoadAnimation(const char* Filename)
    {
        m_animations.emplace_back(new Animation(Filename));
        return false;
    }


    void SkinnedModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
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
                m_BoneInfo[BoneIndex].BoneOffset = Util::aiMatrix4x4ToGlm(pMesh->mBones[i]->mOffsetMatrix);
                m_BoneInfo[BoneIndex].BoneName = BoneName;
                m_BoneMapping[BoneName] = BoneIndex;
            }
            else {
                BoneIndex = m_BoneMapping[BoneName];
            }

            for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
                unsigned int VertexID = m_meshEntries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
                float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
                Bones[VertexID].AddBoneData(BoneIndex, Weight);
            }
        }
    }



    void SkinnedModel::Render(Shader* shader, const glm::mat4& modelMatrix)
    {
        glBindVertexArray(m_VAO);
        shader->setMat4("model", modelMatrix);

        for (int i=0; i <m_meshEntries.size(); i++)
        {

            if (Util::StrCmp(m_meshEntries[i].MeshName, "Arms"))
                AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Hands"));
            if (Util::StrCmp(m_meshEntries[i].MeshName, "Shotgun Mesh"))
                AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shotgun"));
            if (Util::StrCmp(m_meshEntries[i].MeshName, "Shell Mesh"))
                AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shell"));

            glDrawElementsBaseVertex(GL_TRIANGLES, m_meshEntries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshEntries[i].BaseIndex), m_meshEntries[i].BaseVertex);
        }
    }

    int SkinnedModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        std::vector<AnimationFrame>& animationFrames = m_animations[currentAnimationIndex]->m_animationFrames;
        
        // This just gets the joint. You wanna make that a function as part of the animation class, that does a search on the first lookup and makes a map.
        const char* jointName = pNodeAnim->mNodeName.C_Str();
        std::vector<SQT> animationFrame = animationFrames[0].m_jointPoses;
        int index = 0;
        for (int i = 0; i < animationFrame.size(); i++) {
            if (Util::StrCmp(animationFrame[i].jointName, jointName)) {
                index = i;
                break;
            }
        }

        // bail if current animation time is earlier than the this nodes first keyframe time
        if (AnimationTime < animationFrames[0].m_jointPoses[index].timeStamp)
            return -1;
        

        //int first_valid_frame;

        if (Util::StrCmp("Bolt_bone", jointName)) {
        //    std::cout << "pNodeAnim->mNumPositionKeys: " << pNodeAnim->mNumPositionKeys << "\n";
        //    std::cout << "pNodeAnim-> animationFrames.size(): " << animationFrames.size() << "\n";
        }

        //for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        for (unsigned int i = 0; i < animationFrames.size() - 1; i++) {
            
            //if (animationFrames[i   + 1].m_jointPoses[index].timeStamp == -1)
            //    return -1;

            //if (Util::StrCmp("Bolt_bone", jointName)) {

                if ((AnimationTime < animationFrames[i + 1].m_jointPoses[index].timeStamp) 
                    && (AnimationTime >= (float)pNodeAnim->mPositionKeys[i + 1].mTime)
                    && (currentAnimationIndex == 5))
                {
                    //std::cout << " \n FOUND IT \n";
                    std::cout << "\ni: " << i << "\n";
                    std::cout << "mTime:      " << (float)pNodeAnim->mPositionKeys[i + 1].mTime << "\n";
                    std::cout << ".timeStamp: " << animationFrames[i + 1].m_jointPoses[index].timeStamp << "\n";
                    std::cout << ".animationFrames.size:        " << animationFrames.size() << "\n";
                    std::cout << ".pNodeAnim->mNumPositionKeys: " << pNodeAnim->mNumPositionKeys << "\n";
                    std::cout << ".AnimationTime:        " << AnimationTime << "\n";
                }
            //}


            if (AnimationTime < animationFrames[i + 1].m_jointPoses[index].timeStamp) {
           // if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
                
               // if (animationFrames[i + 1].m_jointPoses[index].timeStamp != -1)
                    return i;
                //else
                //    return -1;// first_valid_frame;
            }
        }
        return -1;


        // OK INSTEAD OF RETURNING -1. RETURN THE FIRST VALID FRAME. TRY THAT.


        int lastvalidFrame = -1;

        for (unsigned int i = 0; i < animationFrames.size() - 1; i++) 
        {
            if (animationFrames[i].m_jointPoses[index].timeStamp != -1)
                lastvalidFrame = i;
            
            //if (AnimationTime < animationFrames[i + 1].m_timeStamp) {
            if (AnimationTime < animationFrames[i + 1].m_jointPoses[index].timeStamp) 
            {
                if (animationFrames[i + 1].m_jointPoses[index].timeStamp == -1)
                    return -1;
                else
                    return i;
            }
        }
        return -1;
    }


    int SkinnedModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // bail if current animation time is earlier than the this nodes first keyframe time
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[0].mTime)
            return -1;

        for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
            if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
        return -1;
    }


    int SkinnedModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // bail if current animation time is earlier than the this nodes first keyframe time
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[0].mTime)
            return -1;

        assert(pNodeAnim->mNumScalingKeys > 0);

        for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
            if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
                return i;
            }
        }
        return -1;
    }


    void SkinnedModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }

        int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
       // int PositionIndex = GetPositionIndex(AnimationTime, m_animations[currentAnimationIndex]->m_animationFrames);
        int NextPositionIndex = (PositionIndex + 1);

        // Nothing to report
        if (PositionIndex == -1) {
            Out = aiVector3D(0, 0, 0);
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }

        float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

        const char* jointName = pNodeAnim->mNodeName.C_Str();

        std::vector<SQT> jointPosesA = m_animations[currentAnimationIndex]->m_animationFrames[PositionIndex].m_jointPoses;
        std::vector<SQT> jointPosesB = m_animations[currentAnimationIndex]->m_animationFrames[NextPositionIndex].m_jointPoses;
        
        int index = 0;
        for (int i = 0; i < jointPosesA.size(); i++)
        {
            if (Util::StrCmp(jointPosesA[i].jointName, jointName)) {
                index = i;
                break;
            }
        }

        glm::vec3 start = jointPosesA[index].positon;
        glm::vec3 end = jointPosesA[index].positon;

        aiVector3D A;
        A.x = start.x;
        A.y = start.y;
        A.z = start.z;
        aiVector3D B;
        B.x = end.x;
        B.y = end.y;
        B.z = end.z;

        aiVector3D Delta = B - A;
        Out = A + Factor * Delta;
    }


    void SkinnedModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        // we need at least two values to interpolate...
        if (pNodeAnim->mNumRotationKeys == 1) {
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }

        int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
        unsigned int NextRotationIndex = (RotationIndex + 1);

        // Nothing to report
        if (RotationIndex == -1) {
            Out = aiQuaternion(1, 0, 0, 0);
            Out = pNodeAnim->mRotationKeys[0].mValue;
            return;
            RotationIndex = 0;
            NextRotationIndex = 0;
        }

        //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
        float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
        //assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = Out.Normalize();
    }


    int SkinnedModel::GetJointIndex(const char* name)
    {
        return 0;
    }

    int SkinnedModel::GetPositionIndex(float AnimationTime, std::vector<AnimationFrame>& animationFrames)
	{
        // bail if current animation time is earlier than the this nodes first keyframe time
    /*    if (AnimationTime < animationFrames[0].m_timeStamp)
            return -1;
       
        // Otherwuse find the frame we're up to
        for (unsigned int i = 0; i < animationFrames.size() - 1; i++) {
            if (AnimationTime < animationFrames[i + 1].m_timeStamp)
                return i;
        }
        // If somehow you get here, well return -1, it seems to be working thus far
        return -1;*/
        return -1;
	}

	void SkinnedModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumScalingKeys == 1) {
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }

        int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
        unsigned int NextScalingIndex = (ScalingIndex + 1);

        // Nothing to report
        if (ScalingIndex == -1) {
            Out = aiVector3D(1, 1, 1);
            Out = pNodeAnim->mScalingKeys[0].mValue;
            return;
            ScalingIndex = 0;
            NextScalingIndex = 0;
        }

        //assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
        float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
        //assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }


    void SkinnedModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
    {
        // Get the node and its um bind pose transform?
        string NodeName(pNode->mName.data);
        glm::mat4 NodeTransformation(Util::aiMatrix4x4ToGlm(pNode->mTransformation));

        // Calculate any animation
        if (m_animations.size() > 0) 
        {
            aiAnimation* animation = m_animations[currentAnimationIndex]->m_pAnimationScene->mAnimations[0];
            const aiAnimation* pAnimation = animation;
            const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

            if (pNodeAnim) {
                aiVector3D Scaling;
                CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
                glm::mat4 ScalingM;
                ScalingM = Util::Mat4InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
                aiQuaternion RotationQ;
                CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
                glm::mat4 RotationM = Util::aiMatrix3x3ToGlm(RotationQ.GetMatrix());
                
                
                
                aiVector3D Translation;
                //CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
     
                int PositionIndex = GetPositionIndex(AnimationTime, m_animations[currentAnimationIndex]->m_animationFrames);
                int NextPositionIndex = (PositionIndex + 1);



                // Nothing to report: get the pos/rot/scale for frame 0
                if (PositionIndex == -1) {
                    Translation = pNodeAnim->mPositionKeys[0].mValue;
                }
                else
                {
                    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
                    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

                    const char* jointName = pNodeAnim->mNodeName.C_Str();

                    std::vector<SQT> jointPosesA = m_animations[currentAnimationIndex]->m_animationFrames[PositionIndex].m_jointPoses;
                    std::vector<SQT> jointPosesB = m_animations[currentAnimationIndex]->m_animationFrames[NextPositionIndex].m_jointPoses;

                    int index = 0;
                    for (int i = 0; i < jointPosesA.size(); i++)
                    {
                        if (Util::StrCmp(jointPosesA[i].jointName, jointName)) {
                            index = i;
                            break;
                        }
                    }

                    glm::vec3 start = jointPosesA[index].positon;
                    glm::vec3 end = jointPosesA[index].positon;

                    aiVector3D A;
                    A.x = start.x;
                    A.y = start.y;
                    A.z = start.z;
                    aiVector3D B;
                    B.x = end.x;
                    B.y = end.y;
                    B.z = end.z;

                    aiVector3D Delta = B - A;
                    Translation = A + Factor * Delta;
                }

                
                glm::mat4 TranslationM;
                TranslationM = Util::Mat4InitTranslationTransform(Translation.x, Translation.y, Translation.z);
                NodeTransformation = TranslationM * RotationM * ScalingM;
            }
        }

        glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

       /* std::cout << NodeName << "\n";
        Util::PrintMat4(GlobalTransformation);
        std::cout << "\n";*/

        if (NodeName == "Camera001") {
            m_CameraMatrix = GlobalTransformation;
        }

        if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
            unsigned int BoneIndex = m_BoneMapping[NodeName];
            m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            m_BoneInfo[BoneIndex].DebugMatrix_AnimatedTransform = GlobalTransformation;
           // m_BoneInfo[BoneIndex].DebugMatrix_BindPose = inverse(m_BoneInfo[BoneIndex].BoneOffset);


            // If there is no bind pose, then just use bind pose
            if (m_animations.size() == 0)
                m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
            ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }


    void SkinnedModel::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, vector<glm::mat4>& DebugAnimatedTransforms)
    {
        // Get the animation time
        float AnimationTime = 0;
        if (m_animations.size() > 0) {
            aiAnimation* animation = m_animations[currentAnimationIndex]->m_pAnimationScene->mAnimations[0];

            float TicksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
            float TimeInTicks = TimeInSeconds * TicksPerSecond;
            AnimationTime = fmod(TimeInTicks, (float)animation->mDuration);
        }



        // Traverse the tree 
        for (int i = 0; i < m_skeleton.m_joints.size(); i++)
        {
            // Get the node and its um bind pose transform?
            std::string NodeName = m_skeleton.m_joints[i].m_name;
            glm::mat4 NodeTransformation = m_skeleton.m_joints[i].m_inverseBindTransform;

            // Calculate any animation
            if (m_animations.size() > 0)
            {
                aiAnimation* animation = m_animations[currentAnimationIndex]->m_pAnimationScene->mAnimations[0];
                const aiAnimation* pAnimation = animation;
                const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

                if (pNodeAnim) {
                    aiVector3D Scaling;
                    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
                    glm::mat4 ScalingM;
                    ScalingM = Util::Mat4InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
                    aiQuaternion RotationQ;
                    CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
                    glm::mat4 RotationM = Util::aiMatrix3x3ToGlm(RotationQ.GetMatrix());
                    aiVector3D Translation;
                
                    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);



                    glm::mat4 TranslationM;
                    TranslationM = Util::Mat4InitTranslationTransform(Translation.x, Translation.y, Translation.z);
                    NodeTransformation = TranslationM * RotationM * ScalingM;
                }
            }
            unsigned int parentIndex = m_skeleton.m_joints[i].m_parentIndex;
            
            glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : m_skeleton.m_joints[parentIndex].m_currentFinalTransform;
            glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

            m_skeleton.m_joints[i].m_currentFinalTransform = GlobalTransformation;
     

            if (NodeName == "Camera001") {
                m_CameraMatrix = GlobalTransformation;
            }

            if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
                unsigned int BoneIndex = m_BoneMapping[NodeName];
                m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
                m_BoneInfo[BoneIndex].DebugMatrix_AnimatedTransform = GlobalTransformation;

                 // If there is no bind pose, then just use bind pose
                if (m_animations.size() == 0)
                    m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            }
        }
        


        

        // Climb down the tree and build the transforms. You retreuve them just below see, from m_BoneInfo.
    //    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, glm::mat4(1));

        for (unsigned int i = 0; i < m_NumBones; i++) {
            Transforms[i] = m_BoneInfo[i].FinalTransformation;
            DebugAnimatedTransforms[i] = m_BoneInfo[i].DebugMatrix_AnimatedTransform;
        }
    }

    const aiNodeAnim* SkinnedModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
    {
        for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
            const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

            if (string(pNodeAnim->mNodeName.data) == NodeName) {
                return pNodeAnim;
            }
        }
        return NULL;
    }
}