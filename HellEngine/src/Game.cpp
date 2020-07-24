#include "hellpch.h"
#include "Game.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Core/File.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
#include "Audio/Audio.h"
#include "Logic/WeaponLogic.h"
#include "Logic/ShotgunLogic.h"
#include "Logic/GlockLogic.h"
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
		GlockLogic::p_camera = &camera;
		GlockLogic::p_player = &m_player;
		

		Physics::Init();
		WeaponLogic::Init();

		this->house = File::LoadMap("Map.txt");
		this->RebuildMap();


	


		m_testAnimatedEnttity.SetSkinnedModel("Shotgun.fbx");
		m_testAnimatedEnttity.m_currentAnimationIndex = 5;


		AssetManager::PrintSkinnedModelMeshNames("Glock.fbx");
		AssetManager::PrintSkinnedModelBoneNames("Glock.fbx");

		m_NurseGuy.SetSkinnedModel("NurseGuy.fbx");
		m_NurseGuy.m_worldTransform.position.x = 1;
		m_NurseGuy.SetModelScale(0.01f);
		m_NurseGuy.m_currentAnimationIndex = 1;


		m_zombieGuy.SetSkinnedModel("Zombie.fbx");
		m_zombieGuy.SetAnimationToBindPose();
		m_zombieGuy.NewRagdollFromAnimatedTransforms();





		m_zombieGuy.SetMaterial("Body_LOD0", "Zombie_Face");
		m_zombieGuy.SetMaterial("Tshirt_LOD0", "Zombie_Shirt");
		m_zombieGuy.SetMaterial("Hair_LOD0", "Zombie_Hair");
		m_zombieGuy.SetMaterial("BTM_LOD0", "Zombie_Jeans");

		//AssetManager::PrintSkinnedModelMeshNames("Shotgun.fbx");

		/*std::cout << "\n";
		AssetManager::PrintSkinnedModelMeshNames("Zombie.fbx");
		std::cout << "\n";
		AssetManager::PrintSkinnedModelBoneNames("Zombie.fbx");
		std::cout << "\n";

		std::cout << "\n";
		AssetManager::PrintSkinnedModelMeshNames("Shotgun.fbx");
		std::cout << "\n";
		AssetManager::PrintSkinnedModelBoneNames("Shotgun.fbx");*/
		std::cout << "\n";
	}

	void Game::OnUpdate()
	{
		WeaponLogic::Update(m_frameTime);

		//m_NurseGuy.m_worldTransform = Renderer::s_DebugTransform;

		// Keypresses
		Input::HandleKeypresses();
		if (Input::s_keyPressed[HELL_KEY_E])
			OnInteract();




		if (Input::s_keyPressed[HELL_KEY_M])
		{
		
			SkinnedModel* skinnedModel = m_zombieGuy.GetSkinnedModel();
			glm::mat4 m = skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_r"]].ModelSpace_AnimatedTransform;

			//Ragdoll* ragdoll = m_zombieGuy.m_ragdoll;
			//btGeneric6DofConstraint* constraint = ragdoll->m_joints[Ragdoll::JOINT_RIGHT_HIP];
			//btRigidBody* body = ragdoll->m_bodies[Ragdoll::BODYPART_RIGHT_UPPER_LEG];

			//constraint->calculateTransforms();

			//constraint.getr
		}

		m_player.Update(m_frameTime);
		m_player.m_characterController.Update(m_frameTime, &camera);


		Physics::Update(m_frameTime);



	//	if (Input::s_keyPressed[HELL_KEY_P])
	//		m_zombieGuy.SetAnimationToBindPose();

		if (Input::s_keyPressed[HELL_KEY_L])
			m_zombieGuy.m_ragdoll->DisableSkinning = !m_zombieGuy.m_ragdoll->DisableSkinning;


		if (Input::s_keyPressed[HELL_KEY_N])
		{
			for (int i = 0; i < Ragdoll::BODYPART_COUNT; ++i)
				Physics::s_dynamicsWorld->removeRigidBody(m_zombieGuy.m_ragdoll->m_bodies[i]);

			for (int i = 0; i < Ragdoll::JOINT_COUNT; ++i)
				Physics::s_dynamicsWorld->removeConstraint(m_zombieGuy.m_ragdoll->m_joints[i]);

			m_zombieGuy.SetAnimationToBindPose();
			m_zombieGuy.NewRagdollFromAnimatedTransforms();

		}
			

		for (Shell& shell : Shell::s_shotgunShells)
			shell.Update(m_frameTime);

		for (Shell& bulletCasing : Shell::s_bulletCasings)
			bulletCasing.Update(m_frameTime);

		Renderer::s_bloodEffect.Update(m_frameTime);
		Renderer::s_muzzleFlash.Update(m_frameTime);
		Renderer::s_bloodWallSplatter.Update(m_frameTime);

		TextBlitter::UpdateBlitter(m_frameTime);

		
		// Doors
		for (Door& door : house.m_doors)
			door.Update(m_frameTime);

		WeaponLogic::Update(m_frameTime);

		//ShotgunLogic::Update(m_frameTime);
		m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 10.0f, 0); //ShotgunLogic::m_raycast;

		



		
		// Camera
		if (Input::s_rightMouseDown)
			camera.m_zoomFactor += m_frameTime * camera.m_zoomSpeed;
		else
			camera.m_zoomFactor -= m_frameTime * camera.m_zoomSpeed;

		camera.m_zoomFactor = std::min(camera.m_zoomFactor, camera.m_zoomLimit);
		camera.m_zoomFactor = std::max(camera.m_zoomFactor, 0.0f);

		camera.Update(m_frameTime);
		camera.m_weaponCameraMatrix = WeaponLogic::GetCameraMatrix();

		camera.CalculateMatrices(m_player.m_characterController.GetViewPosition());
		camera.CalculateProjectionMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
		camera.CalculateWeaponSwayTransform(m_frameTime);

		if (!Renderer::m_showImGui)
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

		WeaponLogic::UpdateSkeletalAnimation(deltaTime);

		// Update all skeletal meshes
		//m_testAnimatedEnttity.Update(deltaTime);

		m_NurseGuy.Update(deltaTime);
		
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