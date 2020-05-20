#include "hellpch.h"
#include "Game.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Importer.h"
#include "Helpers/Util.h"
#include "Core/File.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
#include "Audio/Audio.h"

namespace HellEngine
{
	bool Game::s_dontLoadShotgun = false;

	Game::Game()
	{
		OnLoad();
	}

	void Game::OnLoad()
	{
		// Time
		currentFrame = CoreGL::GetGLTime();
		currentFrame = CoreGL::GetGLTime();
		lastFrame = currentFrame;

		///////////////////////////////////
		// Working assimp animated model //
		///////////////////////////////////

		if (!m_skinnedMesh.LoadMesh("res/models/boblampclean.md5mesh")) {
			printf("Mesh load failed\n");
		}
		///////////////////////////////////



		m_shotgunTransform.scale = glm::vec3(0.2);

		//Audio::StreamAudio("Music3.mp3");
		
		testTrans.rotation.y = HELL_PI * 1.5f;

		m_HUDshotgun = Entity();

		house = File::LoadMap("Map.txt");

		Physics::Init();

		this->RebuildMap();
	}

	void Game::OnUpdate()
	{
		// Keypresses
		Input::HandleKeypresses();
		if (Input::s_keyPressed[HELL_KEY_E])
			OnInteract();

		// Time
		currentFrame = CoreGL::GetGLTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		TextBlitter::UpdateBlitter(deltaTime);

		//m_skinnedMesh.Update(deltaTime, &camera);

		// Player
		m_player.Update(deltaTime);
		m_player.m_characterController.Update(deltaTime, &camera);

		Physics::Update(deltaTime);

		// Doors
		for (Door& door : house.m_doors)
			door.Update(deltaTime);

		// Model IDs
		if (m_HUDshotgun.m_modelID == -1)
			m_HUDshotgun.m_modelID = AssetManager::GetModelIDByName("Shotgun");

		// Fire shotgun
		if (Input::s_leftMousePressed && m_player.m_gunState != GunState::FIRING)
		{
			m_player.m_gunState = GunState::FIRING;
			Audio::PlayAudio("Shotgun_Fire_01.wav");
			//Audio::PlayAudio("Door_Open.wav");
			TextBlitter::TypeText("TESTING TEXT BLITTER.", true);
		}
		 
		// HUD shotgun animation
		if (m_player.m_gunState == GunState::FIRING && m_HUDshotgun.IsAnimationComplete())
			m_player.m_gunState = GunState::IDLE;

		if (m_player.m_gunState == GunState::FIRING)
			m_HUDshotgun.SetAnimation("Fire", false);
		else {
			if (m_player.m_movementState == PlayerMovementState::STOPPED)
				m_HUDshotgun.SetAnimation("Idle", true);
			if (m_player.m_movementState == PlayerMovementState::WALKING)
				m_HUDshotgun.SetAnimation("Walk", true);
		}

		// Update animations
		m_HUDshotgun.Update(deltaTime);

		glm::mat4 shotgunCameraMatrix;

		if (AssetManager::GetModelByName("Shotgun") != nullptr)
			shotgunCameraMatrix = AssetManager::GetModelByName("Shotgun")->GetSceneCameraMatrix();
		else
		{
			Transform transform;
			transform.rotation.y = HELL_PI;
			shotgunCameraMatrix = transform.to_mat4();			
		}
			
		// Camera
		camera.Update(deltaTime);
		camera.CalculateMatrices(m_player.m_characterController.GetWorldPosition(), shotgunCameraMatrix);
		camera.CalculateProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
		
		// Camera ray cast
		m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 25);

		// Animated assimp model
		static float ANIMATION_TIME = 0;
		ANIMATION_TIME += deltaTime;
		m_skinnedMesh.BoneTransform(ANIMATION_TIME, m_animatedTransforms);		
	}

	void Game::OnRender()
	{
		Renderer::RenderFrame(this);
	}

	void Game::OnInteract()
	{
		if (m_cameraRaycast.m_name == "DOOR")
			house.m_doors[m_cameraRaycast.m_elementIndex].Interact();
	}

	void Game::RebuildMap()
	{
		static bool FIRST_BUILD = true;
		
		house.RebuildAll();
		Physics::RebuildWorld(&house);

		// Create player
		glm::vec3 spawnPos;
		if (FIRST_BUILD)
			spawnPos = glm::vec3(0.1f, 0.52f, -1.8f);
		else
			spawnPos = m_player.m_characterController.GetWorldPosition();			
		m_player.m_characterController = CharacterController(spawnPos);

		FIRST_BUILD = false;
	}
}