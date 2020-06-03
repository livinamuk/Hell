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
	bool Game::s_dontLoadShotgun = true;

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

		if (!m_skinnedMesh.LoadMesh("res/models/boblampclean.md5mesh"))
			printf("Mesh load failed\n");
	
		if (!m_ZombieBoyMesh.LoadMesh("res/models/ZombieBoy.fbx"))
			printf("Mesh load failed\n");
		
		if (!m_srinivasMesh.LoadMesh("res/models/Shotgun.FBX"))
			printf("Shotgun.FBX mesh load failed\n");
		
		///////////////////////////////////


		m_shotgunTransform.scale = glm::vec3(0.2);

		//Audio::StreamAudio("Music3.mp3");
		
		testTrans.rotation.y = HELL_PI * 1.5f;

		m_HUDshotgun = Entity();

		Physics::Init();

		//this->RebuildMap();

		this->house = File::LoadMap("Map.txt");
		this->RebuildMap();


		m_shells.push_back(Shell(Transform(glm::vec3(0, 1, 0))));
		m_shells.push_back(Shell(Transform(glm::vec3(0.3f, 1, 0))));

	//Entity e("Boy");
	//	e.m_modelID = 1;
	//	e.m_materialID = 1;
	//	house.m_entities.push_back(e);

		m_HUDshotgun.m_modelID = AssetManager::GetModelIDByName("Shotgun");
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

		Renderer::s_bloodEffect.Update(deltaTime);
		TextBlitter::UpdateBlitter(deltaTime);

		// Player
		m_player.Update(deltaTime);
		m_player.m_characterController.Update(deltaTime, &camera);

		Physics::Update(deltaTime);

		// Doors
		for (Door& door : house.m_doors)
			door.Update(deltaTime);

		// Fire shotgun
		//if (Input::s_leftMousePressed && m_player.m_gunState != GunState::FIRING)
		if (Input::s_leftMousePressed)
			{
			if (!s_dontLoadShotgun)
				m_player.m_gunState = GunState::FIRING;

			//Physics::m_ragdoll->m_bodies[0]->applyForce(btVector3(10, 10, 10), btVector3(10, 10, 10));
			//Physics::m_ragdoll->m_bodies[6]->applyForce(btVector3(10, 10, 10), btVector3(10, 10, 10));

			Audio::PlayAudio("Shotgun_Fire_01.wav");
			//Audio::PlayAudio("Door_Open.wav");
			TextBlitter::TypeText("TESTING TEXT BLITTER.", true);

			for (int i = 0; i < 12; i++) {
				RaycastResult ray;
				ray.CastRay(camera.m_viewPos, camera.m_Front, 10.0f, 0.125f);
				m_decals.push_back(Decal(ray.m_hitPoint, ray.m_surfaceNormal));

				if (ray.m_name == "NEW MESH") {
					Renderer::s_bloodEffect.m_CurrentTime = 0;
				//	Audio::PlayAudio("Flesh.wav", 0.25f);
				}
			}

		Renderer::s_hitPoint.position = m_cameraRaycast.m_hitPoint;
		Renderer::s_hitPoint.rotation = camera.m_transform.rotation;

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
		
	//	m_HUDshotgun.SetAnimation("Fire", false);
	//	m_HUDshotgun.Update(0);

		glm::mat4 shotgunCameraMatrix;

		if (!s_dontLoadShotgun)
		{
			m_HUDshotgun.Update(deltaTime);
			shotgunCameraMatrix = AssetManager::GetModelByName("Shotgun")->GetSceneCameraMatrix();
		}
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

		m_ZombieBoyMesh.BoneTransform(0, m_ZombieBoyAnimatedTransforms);

		// Animated assimp model
		//m_srinivasMesh.BoneTransform(ANIMATION_TIME, m_srinivasdAnimatedTransforms);
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