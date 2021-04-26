#include "hellpch.h"
#include "Ragdoll2.h"
#include <math.h>

namespace HellEngine
{
	Ragdoll2::Ragdoll2()
	{
	}

	Ragdoll2::Ragdoll2(std::string filename)
	{
		m_skinnedModel = FileImporter::LoadSkinnedModel("NurseGuy.fbx");

		// Load file
		std::string fileName = "res/" + filename;
		FILE* pFile = fopen(fileName.c_str(), "rb");
		char buffer[65536];
		rapidjson::FileReadStream is(pFile, buffer, sizeof(buffer));
		rapidjson::Document document;
		document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

		// Check for errors
		if (document.HasParseError())
			std::cout << "Error  : " << document.GetParseError() << '\n' << "Offset : " << document.GetErrorOffset() << '\n';

		// Hardcode type names
		//static const char* kTypeNames[] =
		//{ "Null", "False", "True", "Object", "Array", "String", "Number" };

		// Loop over entities
		for (auto& entity : document["entities"].GetObject())
		{

			auto components = entity.value["components"].GetObject();
			if (components.HasMember("RigidComponent"))
			{
				RigidComponent rigidComponent;
				rigidComponent.ID = entity.value["id"].GetInt();
				rigidComponent.name = components["NameComponent"].GetObject()["members"].GetObject()["value"].GetString();

				// Rest component
				if (components.HasMember("RestComponent")) {
					auto arr = components["RestComponent"].GetObject()["members"].GetObject()["matrix"].GetObject()["values"].GetArray();
					rigidComponent.restMatrix = Util::Mat4FromJSONArray(arr);
				}
				// Scale
				if (components.HasMember("ScaleComponent")) {
					auto arr2 = components["ScaleComponent"].GetObject()["members"].GetObject()["absolute"].GetObject()["values"].GetArray();
					rigidComponent.scaleAbsoluteVector = Util::Vec3FromJSONArray(arr2);
				}
				// Geometry Description comnponent
				if (components.HasMember("GeometryDescriptionComponent"))
				{
					rigidComponent.capsuleRadius = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["radius"].GetFloat();
					rigidComponent.capsuleLength = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["length"].GetFloat();
					rigidComponent.shapeType = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["type"].GetString();

					auto arr = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["extents"].GetObject()["values"].GetArray();
					rigidComponent.boxExtents = Util::Vec3FromJSONArray(arr);
					auto arr2 = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["offset"].GetObject()["values"].GetArray();
					rigidComponent.offset = Util::Vec3FromJSONArray(arr2);
					auto arr3 = components["GeometryDescriptionComponent"].GetObject()["members"].GetObject()["rotation"].GetObject()["values"].GetArray();
					rigidComponent.rotation = Util::BtQuatFromJSONArray(arr3);
				}

				rigidComponent.mass = components["RigidComponent"].GetObject()["members"].GetObject()["mass"].GetFloat();
				rigidComponent.friction = components["RigidComponent"].GetObject()["members"].GetObject()["friction"].GetFloat();
				rigidComponent.restitution = components["RigidComponent"].GetObject()["members"].GetObject()["restitution"].GetFloat();
				rigidComponent.linearDamping = components["RigidComponent"].GetObject()["members"].GetObject()["linearDamping"].GetFloat();
				rigidComponent.angularDamping = components["RigidComponent"].GetObject()["members"].GetObject()["angularDamping"].GetFloat();
				rigidComponent.sleepThreshold = components["RigidComponent"].GetObject()["members"].GetObject()["sleepThreshold"].GetFloat();


				if (rigidComponent.name == "rRigid")
					rigidComponent.correspondingJointName = "pelvis";
				else if (rigidComponent.name == "rRigid1")
					rigidComponent.correspondingJointName = "spine_01";
				else if (rigidComponent.name == "rRigid2")
					rigidComponent.correspondingJointName = "spine_02";
				else if (rigidComponent.name == "rRigid3")
					rigidComponent.correspondingJointName = "spine_03";
				else if (rigidComponent.name == "rRigid34")
					rigidComponent.correspondingJointName = "thigh_l";
				else if (rigidComponent.name == "rRigid35")
					rigidComponent.correspondingJointName = "calf_l";
				else if (rigidComponent.name == "rRigid36")
					rigidComponent.correspondingJointName = "foot_l";
				else if (rigidComponent.name == "rRigid37")
					rigidComponent.correspondingJointName = "thigh_r";
				else if (rigidComponent.name == "rRigid38")
					rigidComponent.correspondingJointName = "calf_r";
				else if (rigidComponent.name == "rRigid39")
					rigidComponent.correspondingJointName = "foot_r";
				else if (rigidComponent.name == "rRigid4")
					rigidComponent.correspondingJointName = "clavicle_l";
				else if (rigidComponent.name == "rRigid18")
					rigidComponent.correspondingJointName = "clavicle_r";
				else if (rigidComponent.name == "rRigid5")
					rigidComponent.correspondingJointName = "upperarm_l";
				else if (rigidComponent.name == "rRigid19")
					rigidComponent.correspondingJointName = "upperarm_r";
				else if (rigidComponent.name == "rRigid6")
					rigidComponent.correspondingJointName = "lowerarm_l";
				else if (rigidComponent.name == "rRigid20")
					rigidComponent.correspondingJointName = "lowerarm_r";
				else if (rigidComponent.name == "rRigid7")
					rigidComponent.correspondingJointName = "hand_l";
				else if (rigidComponent.name == "rRigid21")
					rigidComponent.correspondingJointName = "hand_r";
				else if (rigidComponent.name == "rRigid32")
					rigidComponent.correspondingJointName = "neck_01";
				else if (rigidComponent.name == "rRigid33")
					rigidComponent.correspondingJointName = "head";
				else
					continue;

				m_rigidComponents.push_back(rigidComponent);
			}


			if (components.HasMember("JointComponent"))
			{
				JointComponent jointComponent;
				jointComponent.name = components["NameComponent"].GetObject()["members"].GetObject()["value"].GetString();
				jointComponent.parentID = components["JointComponent"].GetObject()["members"].GetObject()["parent"].GetInt();
				jointComponent.childID = components["JointComponent"].GetObject()["members"].GetObject()["child"].GetInt();

				auto mat = components["JointComponent"].GetObject()["members"].GetObject()["parentFrame"].GetObject()["values"].GetArray();
				jointComponent.parentFrame = Util::Mat4FromJSONArray(mat);
				mat = components["JointComponent"].GetObject()["members"].GetObject()["childFrame"].GetObject()["values"].GetArray();
				jointComponent.childFrame = Util::Mat4FromJSONArray(mat);

				jointComponent.drive_angularDamping = components["DriveComponent"].GetObject()["members"].GetObject()["angularDamping"].GetFloat();
				jointComponent.drive_angularStiffness = components["DriveComponent"].GetObject()["members"].GetObject()["angularStiffness"].GetFloat();
				jointComponent.drive_linearDampening = components["DriveComponent"].GetObject()["members"].GetObject()["linearDamping"].GetFloat();
				jointComponent.drive_linearStiffness = components["DriveComponent"].GetObject()["members"].GetObject()["linearStiffness"].GetFloat();
				mat = components["DriveComponent"].GetObject()["members"].GetObject()["target"].GetObject()["values"].GetArray();
				jointComponent.target = Util::Mat4FromJSONArray(mat);

				jointComponent.limit_angularDampening = components["LimitComponent"].GetObject()["members"].GetObject()["angularDamping"].GetFloat();
				jointComponent.limit_angularStiffness = components["LimitComponent"].GetObject()["members"].GetObject()["angularStiffness"].GetFloat();
				jointComponent.limit_linearDampening = components["LimitComponent"].GetObject()["members"].GetObject()["linearDamping"].GetFloat();
				jointComponent.limit_linearStiffness = components["LimitComponent"].GetObject()["members"].GetObject()["linearStiffness"].GetFloat();
				jointComponent.twist = components["LimitComponent"].GetObject()["members"].GetObject()["twist"].GetFloat();
				jointComponent.swing1 = components["LimitComponent"].GetObject()["members"].GetObject()["swing1"].GetFloat();
				jointComponent.swing2 = components["LimitComponent"].GetObject()["members"].GetObject()["swing2"].GetFloat();
				jointComponent.limit.x = components["LimitComponent"].GetObject()["members"].GetObject()["x"].GetFloat();
				jointComponent.limit.y = components["LimitComponent"].GetObject()["members"].GetObject()["y"].GetFloat();
				jointComponent.limit.z = components["LimitComponent"].GetObject()["members"].GetObject()["z"].GetFloat();
				jointComponent.enabled = components["LimitComponent"].GetObject()["members"].GetObject()["enabled"].GetBool();

				m_jointComponents.push_back(jointComponent);
			}
		}


		// Set animation to bind pose

		m_animatedTransforms.clear();
		m_animatedDebugTransforms_Animated.clear();

		for (unsigned int i = 0; i < m_skinnedModel->m_NumBones; i++) {
			m_animatedTransforms.push_back(glm::mat4(1));
			m_animatedDebugTransforms_Animated.push_back(glm::mat4(1));
		}

		float m_currentAnimationTime = 0;
		m_skinnedModel->BoneTransform(m_currentAnimationTime, m_animatedTransforms, m_animatedDebugTransforms_Animated);



		//this->SetAnimationToBindPose();
		//this->NewRagdollFromAnimatedTransforms();

		for (SkinnedModel::BoneInfo info : m_skinnedModel->m_BoneInfo)
		{
			std::string name = info.BoneName;
			//std::cout << "NAME: " << name << "\n";
		}


		for (RigidComponent& rigid : m_rigidComponents)
		{
			if (rigid.name == "rSceneShape")
				continue;

			// APPLY SCALE
			if (rigid.shapeType == "capsule") {
				rigid.capsuleRadius *= rigid.scaleAbsoluteVector.x;
				rigid.capsuleLength *= rigid.scaleAbsoluteVector.y;
			}
			else if (rigid.shapeType == "Box") {
				rigid.boxExtents.x *= rigid.scaleAbsoluteVector.x;
				rigid.boxExtents.y *= rigid.scaleAbsoluteVector.y;
				rigid.boxExtents.z *= rigid.scaleAbsoluteVector.z;
			}




			float mass = 1.0f;

			btQuaternion rotate180(1, 0, 0, 0);
			btQuaternion rotate_X_180(1, 0, 0, 0);
			btQuaternion rotate_Y_180(0, 1, 0, 0);
			btQuaternion rotate_Y_NEG_90(0, -0.7070999742, 0, 0.7070999742);
			btQuaternion rotate_X_90(0.7070999742, 0, 0, 0.7070999742);
			btQuaternion rotate_X_NEG_90(-0.7070999742, 0, 0, 0.7070999742);
			btQuaternion rotate_Y_90(0, 0.7070999742, 0, 0.7070999742);
			btQuaternion rotate_Z_90(0, 0, 0.7070999742, 0.7070999742);
			btQuaternion rotate_Z_NEG_90(0, 0, 0.7070999742, -0.7070999742);
			btQuaternion magic(0.5, 0.5, 0.5, -0.5);

			std::cout << "\n" << rigid.correspondingJointName << "\n";

			// Get transform from  current animated skeleton state
			btTransform transform;
			transform.setIdentity();
			//	transform.setRotation(Util::GetRotationFromBoneMatrix(m_skinnedModel->m_BoneInfo[m_skinnedModel->m_BoneMapping[rigid.correspondingJointName]].ModelSpace_AnimatedTransform));
			//	transform.setOrigin(Util::glmVec3_to_btVec3(Util::GetTranslationFromMatrix(m_skinnedModel->m_BoneInfo[m_skinnedModel->m_BoneMapping[rigid.correspondingJointName]].ModelSpace_AnimatedTransform)));



				//std::cout << "\n" << rigid.correspondingJointName << "\n";
				//std::cout << "ShapeType: " << rigid.shapeType << "\n";


			glm::vec3 restPos = Util::GetTranslationFromMatrix((rigid.restMatrix));
			transform.setOrigin(Util::glmVec3_to_btVec3(restPos));
			transform.setRotation(Util::GetRotationFromBoneMatrix(rigid.restMatrix));

			// Rotate all world transforms by neg 90 on Z
			btTransform rotate_neg_90;
			rotate_neg_90.setIdentity();
			rotate_neg_90.setRotation(rotate_Z_NEG_90 * rotate_X_180);

			btTransform rotate_pos_90;
			rotate_pos_90.setIdentity();
			rotate_pos_90.setRotation(rotate_Z_90 * rotate_X_180);

			transform = transform * rotate_neg_90;

			// Get offset from JSON data
			btTransform OffsetTransform;
			OffsetTransform.setIdentity();
			OffsetTransform.setRotation(rigid.rotation);

			OffsetTransform = OffsetTransform * rotate_pos_90;

			glm::vec3 offSetPosition = rigid.offset * rigid.scaleAbsoluteVector;

			OffsetTransform.setOrigin(Util::glmVec3_to_btVec3(offSetPosition));

			// Shape
			btCollisionShape* shape;
			btCompoundShape* compoundShape;
			if (rigid.shapeType == "Box")
			{
				glm::vec3 extents(rigid.boxExtents.y, rigid.boxExtents.x, rigid.boxExtents.z);
				shape = new btBoxShape(Util::glmVec3_to_btVec3(extents) * 0.5);
				compoundShape = new btCompoundShape();
				compoundShape->addChildShape(OffsetTransform, shape);
			}
			else
			{
				shape = new btCapsuleShape(rigid.capsuleRadius, rigid.capsuleLength);
				compoundShape = new btCompoundShape();
				compoundShape->addChildShape(OffsetTransform, shape);
			}




			// Combine?
			//shape.
			//transform;// = transform * OffsetTransform;





			btVector3 localInertia(0, 0, 0);
			shape->calculateLocalInertia(1, localInertia);

			btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
			
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
			

			rbInfo.m_mass = rigid.mass;
			rbInfo.m_friction = rigid.friction;
			rbInfo.m_restitution = rigid.restitution;
			rbInfo.m_linearDamping = rigid.linearDamping;
			rbInfo.m_angularDamping = rigid.angularDamping;
			rbInfo.m_angularSleepingThreshold = Config::TEST_FLOAT;
			rbInfo.m_linearSleepingThreshold = Config::TEST_FLOAT;

			/*
			std::cout << "mass: " << rigid.mass << "\n";
			std::cout << "friction: " << rigid.friction << "\n";
			std::cout << "restitution: " << rigid.restitution << "\n";
			std::cout << "linearDamping: " << rigid.linearDamping << "\n";
			std::cout << "angularDamping: " << rigid.angularDamping << "\n";
			*/

			rigid.rigidBody = new btRigidBody(rbInfo);

			EntityData* entityData = new EntityData();
			entityData->type = PhysicsObjectType::RAGDOLL;
			rigid.rigidBody->setUserPointer(entityData);

			int group = CollisionGroups::ENEMY;
			int mask = CollisionGroups::HOUSE | CollisionGroups::TERRAIN | CollisionGroups::ENEMY;// | CollisionGroups::PLAYER;


			btDynamicsWorld* m_ownerWorld = Physics::s_dynamicsWorld;
			m_ownerWorld->addRigidBody(rigid.rigidBody, group, mask);

			rigid.rigidBody->setActivationState(0);

			rigid.rigidBody->setDamping(0.05f, 0.85f);
			rigid.rigidBody->setDeactivationTime(0.8f);
		//	rigid.rigidBody->setSleepingThresholds(1.6f, 2.5f);
			rigid.rigidBody->setFriction(0.8);//


			//Config::TEST_FLOAT
		}



		for (JointComponent& joint : m_jointComponents)
		{
			std::cout << "\n" << joint.name << "\n";
			std::cout << "parentID: " << joint.parentID << "\n";
			std::cout << "childID: " << joint.childID << "\n";

			btGeneric6DofConstraint* joint6DOF;
			btTransform localA, localB;
			bool useLinearReferenceFrameA = true;

			// Find parent
			RigidComponent* parentRigid;
			RigidComponent* childRigid;

			for (RigidComponent& rigid : m_rigidComponents)
			{
				if (rigid.ID == joint.parentID)
					parentRigid = &rigid;
				if (rigid.ID == joint.childID)
					childRigid = &rigid;
			}

			std::cout << "parent name: " << parentRigid->name << "\n";
			std::cout << "child name: " << childRigid->name << "\n";
			std::cout << "parent capsule: " << parentRigid->correspondingJointName << "\n";
			std::cout << "child capsule: " << childRigid->correspondingJointName << "\n";


			// First joint
			{
				btTransform localA, localB;

				glm::vec3 parentPos = Util::GetTranslationFromMatrix(joint.parentFrame);
				localA.setIdentity();
				localA.setOrigin(Util::glmVec3_to_btVec3(parentPos));
				localA.setBasis(Util::BtMat3FromGlmMatrix(joint.parentFrame));

				glm::vec3 childPos = Util::GetTranslationFromMatrix(joint.childFrame);
				localB.setIdentity();
				localB.setOrigin(Util::glmVec3_to_btVec3(childPos));
				localB.setBasis(Util::BtMat3FromGlmMatrix(joint.childFrame));


				joint.coneTwist = new btConeTwistConstraint(*parentRigid->rigidBody, *childRigid->rigidBody, localA, localB);
				joint.coneTwist->setLimit(joint.swing1, joint.swing2, joint.twist);

				//joint.coneTwist = new btGeneric6DofConstraint(*parentRigid->rigidBody, *childRigid->rigidBody, localA, localB, true);
				//joint.coneTwist->setAngularLowerLimit(btVector3(-joint.twist, -joint.swing1, -joint.swing2));
				//joint.coneTwist->setAngularUpperLimit(btVector3(joint.twist, joint.swing1, joint.swing2));

				//std::cout << "getLimitSoftness: " << joint.coneTwist->getLimitSoftness() << "\n";
				//std::cout << "getDamping: " << joint.coneTwist->getDamping() << "\n";



				if (joint.enabled) {

					//if (joint.limit.x > 0) joint.coneTwist->setAngularLowerLimitsetmopxjoint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

					//if (joint.limit.x > 0) joint.coneTwist->setLimit(  setmopxjoint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
					//if (joint.limit.y > 0) pxjoint->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
					//if (joint.limit.z > 0) pxjoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);

				//	if (joint.limit.x < 0) joint.coneTwist->setLimit(3, 0, 0);
				//	if (joint.limit.y < 0) joint.coneTwist->setLimit(4, 0, 0);
				//		if (joint.limit.z < 0) joint.coneTwist->setLimit(5, 0, 0);

					//if (joint.limit.x < 0) joint.coneTwist->setLimit(pxjoint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
					//if (joint.limit.y < 0) pxjoint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
					//if (joint.limit.z < 0) pxjoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

					//if (joint.twist > 0) pxjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
					//if (joint.swing1 > 0) pxjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
					//if (joint.swing2 > 0) pxjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

					//if (joint.twist < 0) pxjoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
					//if (joint.swing1 < 0) pxjoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLOCKED);
					//if (joint.swing2 < 0) pxjoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);
				}


				Physics::s_dynamicsWorld->addConstraint(joint.coneTwist, true);
			}

			//		break;
		}
	}

	void Ragdoll2::RemovePhysicsObjects()
	{
		for (RigidComponent& rigid : m_rigidComponents) {

			//if (rigidBody->getMotionState())
		//	delete rigid.rigidBody->getMotionState();

			//if (rigidBody->getCollisionShape())
		//	delete rigid.rigidBody->getCollisionShape();

			std::cout << rigid.name << ": " << rigid.rigidBody << "\n";

			//	if (rigid.rigidBody) {
			Physics::s_dynamicsWorld->removeRigidBody(rigid.rigidBody);
			//		delete rigid.rigidBody;
				//	rigid.rigidBody = nullptr;
			//	}
		}

		for (JointComponent& joint : m_jointComponents) {
			Physics::s_dynamicsWorld->removeConstraint(joint.coneTwist);

		}
	}




	Ragdoll2::RigidComponent* Ragdoll2::GetRigidByName(std::string name)
	{
		for (RigidComponent& rigidComponent : m_rigidComponents)
		{
			if (rigidComponent.correspondingJointName == name)
				return &rigidComponent;
		}
		//std::cout << "NOT FOUND: " << name << "\n";
		return nullptr;
	}


	void Ragdoll2::UpdateBoneTransform(SkinnedModel* skinnedModel, std::vector<glm::mat4>& Transforms, std::vector<glm::mat4>& DebugAnimatedTransforms)
	{
		
		//btGeneric6DofConstraint* joint6DOF = (btGeneric6DofConstraint*)m_joints[JOINT_LEFT_HIP];
		//joint6DOF->setAngularLowerLimit(btVector3(-SIMD_PI * Config::TEST_FLOAT, -SIMD_PI * Config::TEST_FLOAT2, -SIMD_PI * Config::TEST_FLOAT3));
		//joint6DOF->setAngularUpperLimit(btVector3(SIMD_PI * Config::TEST_FLOAT4, SIMD_PI * Config::TEST_FLOAT5, SIMD_PI * Config::TEST_FLOAT6));

		// Traverse the tree 
		for (int i = 0; i < skinnedModel->m_skeleton.m_joints.size(); i++)
		{
			// Get the node and its um bind pose transform?
			const char* NodeName = skinnedModel->m_skeleton.m_joints[i].m_name;
			glm::mat4 NodeTransformation = skinnedModel->m_skeleton.m_joints[i].m_inverseBindTransform;


			unsigned int parentIndex = skinnedModel->m_skeleton.m_joints[i].m_parentIndex;

			glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : skinnedModel->m_skeleton.m_joints[parentIndex].m_currentFinalTransform;
			glm::mat4 GlobalTransformation = ParentTransformation * NodeTransformation;

			skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = GlobalTransformation;


			if (skinnedModel->m_BoneMapping.find(NodeName) != skinnedModel->m_BoneMapping.end()) {
				unsigned int BoneIndex = skinnedModel->m_BoneMapping[NodeName];
				skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
				skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = GlobalTransformation;


				if (DisableSkinning)
				{
					glm::quat Rotate_X_180(0, 1, 0, 0);
					glm::quat Rotate_X_90(0.70710, 0.7071, 0, 0);




					btTransform worldTransform;
					Transform localOffsetTransform;

					RigidComponent* rigidComponent = this->GetRigidByName(NodeName);

					if (rigidComponent != NULL) {
						rigidComponent->rigidBody->getMotionState()->getWorldTransform(worldTransform);
					//	localOffsetTransform.position.x = rigidComponent->capsuleLength * -0.5;

						//m_bodies[BODYPART_PELVIS]->getMotionState()->getWorldTransform(worldTransform);
						//localOffsetTransform.position.x = m_capsuleWidths[BODYPART_PELVIS] * -0.5;
						glm::mat4 matrix = Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4();// *glm::mat4_cast(Rotate_X_90);

						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}


					// PELIVS
					if (Util::StrCmp(NodeName, "pelviffs")) {

						//glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_PELVIS) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));


						btTransform worldTransform;
						Transform localOffsetTransform;
						
						RigidComponent* rigidComponent = this->GetRigidByName("pelvis");

						rigidComponent->rigidBody->getMotionState()->getWorldTransform(worldTransform);
						localOffsetTransform.position.x = rigidComponent->capsuleLength * -0.5;
						
						//m_bodies[BODYPART_PELVIS]->getMotionState()->getWorldTransform(worldTransform);
						//localOffsetTransform.position.x = m_capsuleWidths[BODYPART_PELVIS] * -0.5;
						glm::mat4 matrix = Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4() * glm::mat4_cast(Rotate_X_90);

						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;



					}

					/*// SPINE 01
					if (Util::StrCmp(NodeName, "spine_01")) {

						btTransform worldTransform;
						m_bodies[BODYPART_LOWER_TORSO]->getMotionState()->getWorldTransform(worldTransform);
						Transform localOffsetTransform;
						localOffsetTransform.position.x += DIST_FROM_SPINE_01_TO_SPINE_02 * -0.5;
						glm::mat4 matrix = Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4() * glm::mat4_cast(Rotate_X_90);

						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}

					// SPINE 02
					if (Util::StrCmp(NodeName, "spine_02")) {

						btTransform worldTransform;
						m_bodies[BODYPART_UPPER_TORSO]->getMotionState()->getWorldTransform(worldTransform);
						Transform localOffsetTransform;
						localOffsetTransform.position.x += DIST_FROM_SPINE_02_TO_NECK * -0.5;
						glm::mat4 matrix = Util::btTransformToMat4(worldTransform) * localOffsetTransform.to_mat4() * glm::mat4_cast(Rotate_X_90);

						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}



					// Head
					if (Util::StrCmp(NodeName, "neck_01")) {
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_NECK) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT)* glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}

					// LEGS
					if (Util::StrCmp(NodeName, "thigh_r")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_RIGHT_UPPER_LEG) * glm::mat4_cast(glm::quat(-0.5, 0.5, -0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "calf_r")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_RIGHT_LOWER_LEG) * glm::mat4_cast(glm::quat(-0.5, 0.5, -0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "thigh_l")) {
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_LEFT_UPPER_LEG) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "calf_l"))
					{
						glm::mat4 matrix = Get_Top_Joint_World_Matrix(BODYPART_LEFT_LOWER_LEG) * glm::mat4_cast(glm::quat(0.5, 0.5, 0.5, 0.5));
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}


					// ARMS
					if (Util::StrCmp(NodeName, "upperarm_r"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_RIGHT_UPPER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, -0.7071) * Rotate_X_180);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}

					if (Util::StrCmp(NodeName, "lowerarm_r"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_RIGHT_LOWER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, -0.7071) * Rotate_X_180);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}

					if (Util::StrCmp(NodeName, "upperarm_l"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_LEFT_UPPER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT) * glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
						skinnedModel->m_BoneInfo[BoneIndex].ModelSpace_AnimatedTransform = matrix;
					}
					if (Util::StrCmp(NodeName, "lowerarm_l"))
					{
						glm::mat4 matrix = Get_Bottom_Joint_World_Matrix(BODYPART_LEFT_LOWER_ARM) * glm::mat4_cast(glm::quat(0.7071, 0, 0, 0.7071));// glm::mat4_cast(Config::TEST_QUAT)* glm::mat4_cast(Config::TEST_QUAT2);
						skinnedModel->m_BoneInfo[BoneIndex].FinalTransformation = matrix * skinnedModel->m_BoneInfo[BoneIndex].BoneOffset;
						skinnedModel->m_skeleton.m_joints[i].m_currentFinalTransform = matrix;
					}
					*/
				}

			}

			//  * glm::mat4_cast(Config::TEST_QUAT) * glm::mat4_cast(Config::TEST_QUAT2)

		}

		for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++) {
			Transforms[i] = skinnedModel->m_BoneInfo[i].FinalTransformation;
			DebugAnimatedTransforms[i] = skinnedModel->m_BoneInfo[i].ModelSpace_AnimatedTransform;
		}

		/*

		// Climb down the tree and build the transforms. You retreuve them just below see, from m_BoneInfo.
		ReadNodeHeirarchy(skinnedModel, skinnedModel->m_pScene->mRootNode, glm::mat4(1));

		for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++) {
			Transforms[i] = skinnedModel->m_BoneInfo[i].FinalTransformation;
			DebugAnimatedTransforms[i] = skinnedModel->m_BoneInfo[i].DebugMatrix_AnimatedTransform;
		}*/
		
	}

}