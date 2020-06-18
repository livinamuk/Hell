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


		///////////////////////////////////
		// Working assimp animated model //
		///////////////////////////////////

//		if (!m_skinnedMesh.LoadMesh("res/models/boblampclean.md5mesh"))
//			printf("Mesh load failed\n");
	
//		if (!m_ZombieBoyMesh.LoadMesh("res/models/ZombieBoy.fbx"))
//			printf("Mesh load failed\n");
		
	//	if (!m_srinivasMesh.LoadMesh("res/models/Shotgun.fbx"))
	//		printf("Shotgun.FBX mesh load failed\n");



		//Audio::StreamAudio("Music3.mp3");
		
		testTrans.rotation.y = HELL_PI * 1.5f;


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

	/*	if (Input::s_keyPressed[HELL_KEY_O])
			m_shotgunAnimatedEntity.m_currentAnimationIndex -= 1;
		if (Input::s_keyPressed[HELL_KEY_P])
			m_shotgunAnimatedEntity.m_currentAnimationIndex += 1;
			*/
		// Keypresses
		Input::HandleKeypresses();
		if (Input::s_keyPressed[HELL_KEY_E])
			OnInteract();

		// Time		
		const float MAX_FRAME_DELTA = 2;
		
		currentTime = CoreGL::GetGLTime();
		float deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
			
		// Clamp.
		deltaTime = min(deltaTime, MAX_FRAME_DELTA);
		m_deltaTime = deltaTime;


		for (Shell& shell : Shell::s_shells)
			shell.Update(deltaTime);


		// Update in discrete steps
	//	for (int i = 0; i < deltaTime / 100; i++)
		{

			Renderer::s_bloodEffect.Update(deltaTime);
			Renderer::s_muzzleFlash.Update(deltaTime);
			Renderer::s_bloodWallSplatter.Update(deltaTime);

			TextBlitter::UpdateBlitter(deltaTime);

			// Player
			m_player.Update(deltaTime);
			m_player.m_characterController.Update(deltaTime, &camera);


			// Doors
			for (Door& door : house.m_doors)
				door.Update(deltaTime);

			ShotgunLogic::Update(deltaTime);
			m_cameraRaycast = ShotgunLogic::m_raycast;


			m_testAnimatedEnttity.Update(deltaTime);
			m_shotgunAnimatedEntity.Update(deltaTime);
			//m_zombieGuy.Update(deltaTime);
			//m_zombieGuy.SetAnimationToBindPose();
			m_zombieGuy.AnimatedFromRagdoll();

			m_zombieGuy.m_animatedTransforms[23] = Renderer::s_DebugTransform2.to_mat4();


			// Camera

			//camera.m_zoomLimit = Config::TEST_FLOAT;
			//camera.m_zoomSpeed = Config::TEST_FLOAT2;

			if (Input::s_rightMouseDown)
				camera.m_zoomFactor += deltaTime * camera.m_zoomSpeed;
			else
				camera.m_zoomFactor -= deltaTime * camera.m_zoomSpeed;

			camera.m_zoomFactor = std::min(camera.m_zoomFactor, camera.m_zoomLimit);
			camera.m_zoomFactor = std::max(camera.m_zoomFactor, 0.0f);

			camera.Update(deltaTime);
			camera.m_weaponCameraMatrix = m_shotgunAnimatedEntity.GetCameraMatrix();
			camera.CalculateMatrices(m_player.m_characterController.GetWorldPosition());
			camera.CalculateProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
			camera.CalculateWeaponSwayTransform(deltaTime);

			// Camera ray cast
			m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 25);
		}

		Physics::Update(deltaTime);




			/*for (Shell& shell : m_shells)
			{
				btTransform worldTran = shell.m_rigidBody->getWorldTransform();

				btVector3 position = worldTran.getOrigin();

				// find bolt world pos

				static Transform trans;
				trans.scale = glm::vec3(0.002f);
				trans.position = camera.m_viewPos;
				trans.rotation = camera.m_transform.rotation;


				//SkinnedModel* model = AssetManager::skinnedModels[m_shotgunAnimatedEntity.m_skinnedModelID];
				SkinnedModel* model = AssetManager::skinnedModels[0];
				unsigned int BoneIndex = model->m_BoneMapping["Bolt_bone"];

				Renderer::s_DebugTransform.position = camera.m_Front;
				Renderer::s_DebugTransform2.position = camera.m_Right * glm::vec3(-1);

				glm::mat4 worldMatrix = trans.to_mat4() * model->m_BoneInfo[BoneIndex].FinalTransformation * Renderer::s_DebugTransform2.to_mat4();
				glm::vec4 worldPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) * worldMatrix;



				glm::vec3 v = Util::TranslationFromMat4(worldMatrix);

				v += camera.m_Front * glm::vec3(Renderer::s_DebugTransform.scale.x);
				v += camera.m_Right * glm::vec3(Renderer::s_DebugTransform2.scale.x);

				std::cout << "V: " << Util::Vec3_to_String(v);
				Renderer::s_debugString = Util::Vec3_to_String(v);
				Renderer::s_debugString += "\n\n";
				Renderer::s_debugString += Util::Mat4ToString(worldMatrix);

				glm::quat qt = glm::quat(camera.m_transform.rotation);

				//qt *= glm::quat(0.7071, 0, 0, 0.7071);
				qt *= glm::quat(0.5, 0.5, 0.5, 0.5);

				//	btVector3 newPos = btVector3(worldPos.x, worldPos.y, worldPos.z);
				btVector3 newPos = Util::glmVec3_to_btVec3(v);
				;
				worldTran.setOrigin(newPos);
				worldTran.setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));


		

				shell.m_rigidBody->setWorldTransform(worldTran);
			}*/
			

	
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