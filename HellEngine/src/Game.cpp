#include "hellpch.h"
#include "Game.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Core/File.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
#include "Audio/Audio.h"
#include "Logic/ShotgunLogic.h"
#include "Config.h"
#include "Core/EnemyCharacter.h"

namespace HellEngine
{
	bool Game::s_dontLoadShotgun = true;

	Game::Game()
	{
		OnLoad();
	}

	void Game::OnLoad()
	{
		currentTime = CoreGL::GetGLTime();
		lastFrame = 0;

		// Set up sketchy ass pointers
		ShotgunLogic::p_camera = &camera;
		ShotgunLogic::p_player = &m_player;
		ShotgunLogic::p_model = &m_shotgunAnimatedEntity;
		
		Physics::Init();

		this->house = File::LoadMap("Map.txt");
		this->RebuildMap();

		m_shotgunAnimatedEntity.SetSkinnedModel("Shotgun.fbx");


		m_testAnimatedEnttity.SetSkinnedModel("Shotgun.fbx");
		m_testAnimatedEnttity.m_currentAnimationIndex = 3;


		m_zombieGuy.SetSkinnedModel("Zombie.fbx");
		m_zombieGuy.SetModelScale(0.00925f);
		m_zombieGuy.FlipModelUpAxis(true);
		m_zombieGuy.FlipSkeletonUpAxis(true);
		m_zombieGuy.m_worldTransform.rotation.y = HELL_PI / 2;

		m_zombieGuy.SetAnimationToBindPose();
		m_zombieGuy.NewRagdollFromAnimatedTransforms();

		std::cout << "\n";
		AssetManager::PrintSkinnedModelMeshNames("Zombie.fbx");
		std::cout << "\n";
		AssetManager::PrintSkinnedModelBoneNames("Zombie.fbx");
		std::cout << "\n";
	}

	void Game::OnUpdate()
	{
		// Keypresses
		Input::HandleKeypresses();
		if (Input::s_keyPressed[HELL_KEY_E])
			OnInteract();

		m_player.Update(m_frameTime);
		m_player.m_characterController.Update(m_frameTime, &camera);

		
		Physics::Update(m_frameTime);




		for (Shell& shell : Shell::s_shells)
			shell.Update(m_frameTime);


		Renderer::s_bloodEffect.Update(m_frameTime);
		Renderer::s_muzzleFlash.Update(m_frameTime);
		Renderer::s_bloodWallSplatter.Update(m_frameTime);

		TextBlitter::UpdateBlitter(m_frameTime);

		
		// Doors
		for (Door& door : house.m_doors)
			door.Update(m_frameTime);

		ShotgunLogic::Update(m_frameTime);
		m_cameraRaycast = ShotgunLogic::m_raycast;

		



		
		// Camera
		if (Input::s_rightMouseDown)
			camera.m_zoomFactor += m_frameTime * camera.m_zoomSpeed;
		else
			camera.m_zoomFactor -= m_frameTime * camera.m_zoomSpeed;

		camera.m_zoomFactor = std::min(camera.m_zoomFactor, camera.m_zoomLimit);
		camera.m_zoomFactor = std::max(camera.m_zoomFactor, 0.0f);

		camera.Update(m_frameTime);
		camera.m_weaponCameraMatrix = m_shotgunAnimatedEntity.GetCameraMatrix();
		camera.CalculateMatrices(m_player.m_characterController.GetViewPosition());
		camera.CalculateProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
		camera.CalculateWeaponSwayTransform(m_frameTime);

		m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 25);

	}

	void Game::UpdateSkeletalAnimation()
	{
		// Init time
		static float currentFrame = CoreGL::GetGLTime();
		static float lastFrame = currentFrame;	

		// Calculate deltaTime
		currentFrame = CoreGL::GetGLTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update all skeletal meshes
		m_testAnimatedEnttity.Update(deltaTime);
		m_shotgunAnimatedEntity.Update(deltaTime);
		//m_zombieGuy.Update(deltaTime);
		//m_zombieGuy.SetAnimationToBindPose();
		m_zombieGuy.AnimatedFromRagdoll();
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
		//	spawnPos = glm::vec3(1.4f, 0.52f, 3.8f);
		else
			spawnPos = m_player.m_characterController.GetWorldPosition();			
		m_player.m_characterController = CharacterController(spawnPos);

		FIRST_BUILD = false;
	}
}