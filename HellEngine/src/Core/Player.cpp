#include "hellpch.h"
#include "Player.h"
#include "Helpers/Util.h"
#include "Core/Input.h"
#include "Audio/Audio.h"

namespace HellEngine
{
	Player::Player()
	{
	}

	void Player::Update(float deltaTime)
	{
		// Stopped
		if ((!Input::s_keyDown[HELL_KEY_W]) && (!Input::s_keyDown[HELL_KEY_S]) && (!Input::s_keyDown[HELL_KEY_A]) && (!Input::s_keyDown[HELL_KEY_D]))
			m_movementState = PlayerMovementState::STOPPED;

		// Walking
		if (Input::s_keyDown[HELL_KEY_W])
			m_movementState = PlayerMovementState::WALKING;
		if (Input::s_keyDown[HELL_KEY_S])
			m_movementState = PlayerMovementState::WALKING;
		if (Input::s_keyDown[HELL_KEY_D])
			m_movementState = PlayerMovementState::WALKING;
		if (Input::s_keyDown[HELL_KEY_A])
			m_movementState = PlayerMovementState::WALKING;

		//if (Input::s_leftMousePressed)
		//	m_gunState == GunState::FIRING;

		if (m_movementState == PlayerMovementState::WALKING)
			isMoving = true;
		else
			isMoving = false;		

			if (!isMoving)
				footstepAudioTimer = 0;
			else {
				if (isMoving && footstepAudioTimer == 0) {
					int random_number = std::rand() % 4 + 1;
					std::string file = "player_step_" + std::to_string(random_number) + ".wav";
					//char* name = ;
					Audio::PlayAudio(file.c_str(), 0.5f);
				}
				footstepAudioTimer += deltaTime;

				///if (!isRunning)
				//	footstepAudioLoopLength = 0.35f;
				//else
					footstepAudioLoopLength = 0.25f;

				if (footstepAudioTimer > footstepAudioLoopLength)
					footstepAudioTimer = 0;
			}
	}
}