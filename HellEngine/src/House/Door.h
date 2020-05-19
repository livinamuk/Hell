#pragma once
#include "Header.h"
#include "Floor.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"

namespace HellEngine
{
	enum DoorStatus {
		DOOR_OPEN,
		DOOR_OPENING,
		DOOR_CLOSED,
		DOOR_CLOSING,
		LOCKED_FROM_THE_OTHER_SIDE
	};

	class Door
	{

	public: // methods
		Door(glm::vec2 position, int story, Axis axis, bool rotateFloorTex);
		~Door();
		void Draw(Shader* shader);
		void Update(float deltaTime);
		void Interact();
		void UpdateRigidBodyTransfrom();
		glm::mat4 GetDoorModelMatrixFromPhysicsEngine();
		void Reconfigure();
		
		Transform m_rootTransform;
		Transform m_doorTransform;
		Transform m_doorFrameTransform;
		
	public:	// fields
		Axis m_axis;
		Floor m_floor;
		btRigidBody* m_rigidBody;
		int m_openStatus = DOOR_CLOSED;
		int m_story;
		bool initiallyOpen = false;
		bool initiallyLocked = false;
		bool locked = false;
		float openAngle = 0;
		float maxOpenAngle = 2;
	};
}