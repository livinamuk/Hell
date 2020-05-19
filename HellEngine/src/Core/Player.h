#pragma once

#include "Header.h"
#include "Physics/CharacterController.h"

namespace HellEngine
{
	class Player
	{
	public: // methods
		Player();
		void Update(float deltaTime);
		//Transform m_transform;

	public:	// fields
		CharacterController m_characterController;
		PlayerMovementState m_movementState;
		GunState m_gunState;

		bool isMoving;
		float footstepAudioTimer;
		float footstepAudioLoopLength = 0.35f;
	};
}