#include "hellpch.h"
#include "Game.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Importer.h"
#include "Helpers/ImporterNew.h"
#include "Helpers/Util.h"
#include "Core/File.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
#include "Audio/Audio.h"

namespace HellEngine
{

	Game::Game()
	{
		OnLoad();
	}

	void Game::OnLoad()
	{
		Audio::Init();

		Audio::LoadAudio("player_step_1.wav");
		Audio::LoadAudio("player_step_2.wav");
		Audio::LoadAudio("player_step_3.wav");
		Audio::LoadAudio("player_step_4.wav");
		Audio::LoadAudio("Shotgun_Fire_01.wav");
		Audio::LoadAudio("Door_Open.wav");

		// Time
		currentFrame = CoreGL::GetGLTime();
		currentFrame = CoreGL::GetGLTime();
		lastFrame = currentFrame;


		m_assimpModel = AssetManager::LoadFromFile("sphere.obj");

		//	m_assimpModel = AssetManager::LoadFromFile("Shotgun.FBX");
			//m_assimpModel = AssetManager::LoadFromFile("model.dae");
			//m_skinnedMesh.LoadMesh("res/models/AnimatedShotgun.FBX")
			//m_skinnedMesh.LoadMesh("res/models/model.dae");
		//	if (!m_skinnedMesh.LoadMesh("res/models/boblampclean.md5mesh")) {
		if (!m_skinnedMesh.LoadMesh("res/models/boblampclean.md5mesh")) {
			printf("Mesh load failed\n");
			printf("Mesh load failed\n");
			printf("Mesh load failed\n");
			printf("Mesh load failed\n");
			printf("Mesh load failed\n");
		}

		m_shotgunTransform.scale = glm::vec3(0.2);

		Audio::StreamAudio("Music3.mp3");
		
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

		//m_skinnedMesh.Update(deltaTime, &camera);

		// Player
		m_player.Update(deltaTime);
		m_player.m_characterController.Update(deltaTime, &camera);

		Physics::Update(deltaTime);

		// Doors
		for (Door& door : house.m_doors)
			door.Update(deltaTime);

		Model* shotgunModel = AssetManager::GetModelByName("Shotgun");

		// Model IDs
		if (m_HUDshotgun.m_modelID == -1)
			m_HUDshotgun.m_modelID = AssetManager::GetModelIDByName("Shotgun");
		if (m_TestEntity.m_modelID == -1)
			m_TestEntity.m_modelID = AssetManager::GetModelIDByName("Shotgun");

		// Fire shotgun
		if (Input::s_leftMousePressed && m_player.m_gunState != GunState::FIRING)
		{
			m_player.m_gunState = GunState::FIRING;
			Audio::PlayAudio("Shotgun_Fire_01.wav");
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

		m_TestEntity.SetAnimation("Fire", true);

		// Update animations
		m_HUDshotgun.Update(deltaTime);
		//m_TestEntity.Update(deltaTime);


		glm::mat4 shotgunCameraMatrix = shotgunModel->GetSceneCameraMatrix();

		// Camera
		camera.Update(deltaTime);
		camera.CalculateMatrices(m_player.m_characterController.GetWorldPosition(), shotgunModel->GetSceneCameraMatrix());
		camera.CalculateProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);

		if (shotgunModel->m_sceneCamera)
			m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 25);

	}

	void Game::OnRender()
	{


		// Draw house

		Renderer::RenderFrame(this);


		//m_TestEntity.m_transform.position.y = 1.5f;
		//m_TestEntity.Draw(&shader);

		//for (Cube& cube : m_cubes)
		//	cube.Draw(&shader);

		//glm::mat4 identity = glm::mat4(1);
		//Transform transform = Transform();
		//testModel.Draw(&shader, transform.to_mat4());

		//std::cout << testModel.m_meshes[0]->vertices.size() << "\n";

		// Draw shotgun


		/*glDisable(GL_DEPTH_TEST);
		glm::mat4 identity = glm::mat4(1);
		for (int i = 0; i < Physics::m_points.size(); i++)
		{
			Util::DrawPoint(&shader, Physics::m_points[i], identity, glm::vec3(1, 0, 0));
		}*/
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
			spawnPos = glm::vec3(1, 1, 1);
		else
			spawnPos = m_player.m_characterController.GetWorldPosition();			
		m_player.m_characterController = CharacterController(spawnPos);


		FIRST_BUILD = false;
	}

}