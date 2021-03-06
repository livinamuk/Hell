#include "hellpch.h"
#include "Door.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Audio/Audio.h"
#include "Renderer/Renderer.h"
#include "Core/LevelEditor.h"
#include "Physics/Physics.h"

namespace HellEngine
{
	Door::Door(glm::vec2 position, int story, Axis axis, bool rotateFloorTex)
	{
		m_rootTransform.position.x = position.x;		
		m_rootTransform.position.z = position.y;
		m_axis = axis;
		m_story = story;
		m_floor.m_rotateTexture = rotateFloorTex;
		Reconfigure();
		CreateCollisionObject();
	}

	Door::Door(const Door& cpy)
	{
		m_collisionObject = cpy.m_collisionObject;
		m_rootTransform = cpy.m_rootTransform;
		m_doorTransform = cpy.m_doorTransform;
		m_doorFrameTransform = cpy.m_doorFrameTransform;
		m_axis = cpy.m_axis;
		m_floor = cpy.m_floor;
		m_openStatus = cpy.m_openStatus;
		m_story = cpy.m_story;
		initiallyOpen = cpy.initiallyOpen;
		initiallyLocked = cpy.initiallyLocked;
		locked = cpy.locked;
		openAngle = cpy.openAngle;
		maxOpenAngle = cpy.maxOpenAngle;

		EntityData* entityData = (EntityData*)m_collisionObject->getUserPointer();
		entityData->ptr = this;
		m_floor.m_parent = this;
	}

	Door& Door::operator = (const Door& input) {
		new (this) Door(input);
		return *this;
	}

	void Door::Reconfigure()
	{
		m_rootTransform.position.y = m_story * STORY_HEIGHT;
		m_rootTransform.rotation = Util::SetRotationByAxis(m_axis);
		m_doorTransform.position = glm::vec3(0, 0, 0.03f);

		Transform floorTransform;
		floorTransform.position = m_rootTransform.position;
		floorTransform.rotation = m_rootTransform.rotation;
		floorTransform.scale = glm::vec3(DOOR_WIDTH, FLOOR_THICKNESS, 0.1);

		m_floor = Floor(floorTransform, m_floor.m_rotateTexture, this);

	//	m_floor.m_transform.rotation = Util::SetRotationByAxis(m_axis);
		m_floor.CalculateWorldSpaceCorners();
	}

	void Door::CreateCollisionObject()
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(m_rootTransform.position));
		transform.setRotation(Util::glmVec3_to_btQuat(m_rootTransform.rotation));
		float friction = 0.5f;
		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
		btCollisionShape* collisionShape = Physics::s_doorShape;
		PhysicsObjectType objectType = PhysicsObjectType::DOOR;

		m_collisionObject = Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, this);
	}

	void Door::RemoveCollisionObject()
	{
		Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
	}

	void Door::Draw(Shader* shader)
	{
		GpuProfiler g("Door");
		
		LevelEditor::SetHighlightColorIfSelected(shader, this);
		
		
		//Transform shadowTransform;
		//shadowTransform.scale = glm::vec3(1.1);

		//AssetManager::models[doorModelID].Draw(shader, m_rootTransform.to_mat4() * m_doorTransform.to_mat4());
		//if (Renderer::s_RenderSettings.DrawWeapon == false)
		//	AssetManager::models[AssetManager::s_ModelID_Door].Draw(shader,GetDoorModelMatrixFromPhysicsEngine() * Renderer::s_DebugTransform.to_mat4());
		//else

		int modelID;

		if (Renderer::s_RenderSettings.ShadowMapPass == true)
			modelID = AssetManager::GetModelIDByName("DoorShadowCaster");
		else
			modelID = AssetManager::GetModelIDByName("Door");
	
		modelID = AssetManager::GetModelIDByName("Door");


		shader->setBool("blockoutDecals", true);
		AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("Door"));
		AssetManager::DrawModel(modelID, shader, GetDoorModelMatrixFromPhysicsEngine());
		shader->setBool("blockoutDecalas", false);



	//	AssetManager::DrawModel(AssetManager::GetModelIDByName("DoorVolumeA"), shader, GetDoorModelMatrixFromPhysicsEngine());

		//AssetManager::DrawModel(AssetManager::s_ModelID_Door, shader, GetDoorModelMatrixFromPhysicsEngine());

		AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("DoorFrame")); 
		AssetManager::DrawModel(AssetManager::s_ModelID_DoorFrame, shader, m_rootTransform.to_mat4() * m_doorFrameTransform.to_mat4());

		// Draw Floor
		AssetManager::BindMaterial_0(AssetManager::s_MaterialID_FloorBoards);
		m_floor.Draw(shader);

		shader->setVec3("ColorAdd", glm::vec3(0, 0, 0));
	}

	void Door::Update(float deltaTime)
	{
		
		float openSpeed = 5;

		if (m_openStatus == DOOR_OPENING)
			openAngle += openSpeed * deltaTime;
		if (m_openStatus == DOOR_CLOSING)
			openAngle -= openSpeed * deltaTime;

		if (openAngle >= maxOpenAngle)
			m_openStatus = DOOR_OPEN;
		if (openAngle < 0)
			m_openStatus = DOOR_CLOSED;

		// posibily reduntant
		openAngle = std::max(0.0f, openAngle);
		openAngle = std::min(openAngle, maxOpenAngle);

		if (m_openStatus == DOOR_OPEN)
			openAngle = maxOpenAngle;
		if (m_openStatus == DOOR_CLOSED)
			openAngle = 0;
		if (m_openStatus == LOCKED_FROM_THE_OTHER_SIDE)
			openAngle = 0;
		

//		m_doorTransform.rotation.y += 5 * deltaTime;
//		m_doorTransform.rotation.y = 10;
	//	std::cout << "rot: " << m_doorTransform.rotation.y << "\n";

		UpdateRigidBodyTransfrom();
	}

	void Door::Interact()
	{
		if (!locked)
		{
			if (m_openStatus == DOOR_CLOSED) {
				m_openStatus = DOOR_OPENING;
				Audio::PlayAudio("Door_Open.wav");
			}
			if (m_openStatus == DOOR_OPEN) {
				m_openStatus = DOOR_CLOSING;
				Audio::PlayAudio("Door_Open.wav");
			}
			return;
		}

		// locked from the otherr side
		/*if (playerPosition.x < 1.1f && doorStatus == LOCKED_FROM_THE_OTHER_SIDE) {
			Audio::PlayAudio("Door_Locked.wav");
			Quad2D::TypeText("IT'S LOCKED FROM THE OTHER SIDE.", true);
			locked = false;
			return;
		}

		// you unlocked it
		if (playerPosition.x > 1.1f && doorStatus == LOCKED_FROM_THE_OTHER_SIDE) {
			Audio::PlayAudio("Door_Unlock.wav");
			Quad2D::TypeText("YOU UNLOCKED IT.", true);
			locked = false;
			doorStatus = DOOR_CLOSED;
			return;
		}

		// open
		if (!locked)
		{
			if (doorStatus == DOOR_CLOSED) {
				doorStatus = DOOR_OPENING;
				Audio::PlayAudio("Door_Open.wav");
			}
			if (doorStatus == DOOR_OPEN) {
				doorStatus = DOOR_CLOSING;
				Audio::PlayAudio("Door_Open.wav");
			}
			return;
		}

		// locked
		if (locked && !PlayerHasKey) {

			Audio::PlayAudio("Door_Locked.wav");
			Quad2D::TypeText("IT'S LOCKED WITH A KEY.", true);
			return;
		}

		// unlock it
		if (locked && PlayerHasKey) {
			Audio::PlayAudio("Door_Unlock.wav");
			Quad2D::TypeText("YOU UNLOCKED IT.", true);
			locked = false;
			return;
		}*/
	}


	void Door::UpdateRigidBodyTransfrom()
	{
		btTransform translateXMat;
		translateXMat.setIdentity();
		translateXMat.setOrigin(btVector3(-0.8f / 2, -2 / 2, 0.04f / 2));
		
		btTransform rotateYMat;
		rotateYMat.setIdentity();
		rotateYMat.setRotation(btQuaternion(-openAngle, 0, 0));

		btTransform translateXMat2;
		translateXMat2.setIdentity();
		translateXMat2.setOrigin(btVector3(0.8f / 2, 2 / 2, -0.04f / 2));

		btTransform baseTransform;
		baseTransform.setIdentity();

		glm::vec3 pos = Util::GetTranslationFromMatrix(m_rootTransform.to_mat4() * m_doorTransform.to_mat4());
		float x = pos.x;
		float y = pos.y + 1;
		float z = pos.z;

		baseTransform.setOrigin(btVector3(x, y, z));
		baseTransform.setRotation(btQuaternion(m_rootTransform.rotation.y, 0, 0));

		btTransform newMat = baseTransform * translateXMat * rotateYMat * translateXMat2;
		m_collisionObject->setWorldTransform(newMat);
		//m_rigidBody->setWorldTransform(newMat);
	}
	
	glm::mat4 Door::GetDoorModelMatrixFromPhysicsEngine()
	{
		// Position
		//float xPos = (float)m_rigidBody->getCenterOfMassPosition().x();
		//float yPos = (float)m_rigidBody->getCenterOfMassPosition().y();
		//float zPos = (float)m_rigidBody->getCenterOfMassPosition().z();
		btTransform transform = m_collisionObject->getWorldTransform();
		btVector3 position = transform.getOrigin();
		//float xPos = (float)m_collisionObject->getCenterOfMassPosition().x();
		//float yPos = (float)m_collisionObject->getCenterOfMassPosition().y();
		//float zPos = (float)m_collisionObject->getCenterOfMassPosition().z();
		Transform modelTransform;
		//modelTransform.position = glm::vec3(xPos, yPos - 1, zPos);
		modelTransform.position = glm::vec3(position[0], position[1] - 1, position[2]);

		// Rotation
		btScalar x, y, z;
		//m_rigidBody->getWorldTransform().getRotation().getEulerZYX(x, y, z);
		m_collisionObject->getWorldTransform().getRotation().getEulerZYX(x, y, z);
		modelTransform.rotation = glm::vec3(z, y, x);

		return modelTransform.to_mat4();
	}
}
	// Door transform from center (0, 0, 0.03)
	// Rotate pivot transform (-0.4, 0.0, -0.017)
