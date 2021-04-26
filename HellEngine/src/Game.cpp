#include "hellpch.h"
#include "Game.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Core/File.h"
#include "Core/CoreGL.h"
#include "Core/CoreImGui.h"
#include "GL/Quad2D.h"
#include "Audio/Audio.h"
#include "Logic/WeaponLogic.h"
#include "Logic/ShotgunLogic.h"
#include "Logic/GlockLogic.h"
#include "Config.h"
#include "Core/EnemyCharacter.h"
#include "Core/Core.h"

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

		//this->house = File::LoadMap("Map.txt");

		GameData::LoadHouse("Map.txt");
	

		this->RebuildMap();


	
	/*	Model* windowModel = AssetManager::GetModelByName("Window");
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01a", AssetManager::GetMaterialIDByName("Window"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01a", AssetManager::GetMaterialIDByName("WindowExterior"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01b", AssetManager::GetMaterialIDByName("Window"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01b", AssetManager::GetMaterialIDByName("WindowExterior"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01c", AssetManager::GetMaterialIDByName("Window"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01c", AssetManager::GetMaterialIDByName("WindowExterior"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01d", AssetManager::GetMaterialIDByName("Window"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Window_01d", AssetManager::GetMaterialIDByName("WindowExterior"));
		windowModel->Set_Mesh_MaterialID_S et0("SM_Glass_Top", AssetManager::GetMaterialIDByName("White"));
		windowModel->Set_Mesh_MaterialID_Set0("SM_Glass_Down", AssetManager::GetMaterialIDByName("White"));
		*/
		m_testAnimatedEnttity.SetSkinnedModel("Shotgun.fbx");
		m_testAnimatedEnttity.m_currentAnimationIndex = 5;

		//AssetManager::PrintSkinnedModelMeshNames("Glock.fbx");
		//AssetManager::PrintSkinnedModelBoneNames("Glock.fbx");

		m_NurseGuy.SetSkinnedModel("NurseGuy.fbx");
		m_NurseGuy.m_worldTransform.position.x = 1;
	//	m_NurseGuy.SetModelScale(0.01025f);
		m_NurseGuy.m_currentAnimationIndex = 0;





		m_NurseGuy.SetMaterial("Ch16_Body1", "Nurse");
		m_NurseGuy.SetMaterial("Ch16_Cap", "Nurse2");
		m_NurseGuy.SetMaterial("Ch16_Eyelashes", "Nurse");
		m_NurseGuy.SetMaterial("Ch16_Mask", "Nurse2");
		m_NurseGuy.SetMaterial("Ch16_Pants", "Nurse");
		m_NurseGuy.SetMaterial("Ch16_Shirt", "Nurse");
		m_NurseGuy.SetMaterial("Ch16_Shoes", "Nurse2");


		m_zombieGuy.SetSkinnedModel("Zombie.fbx");
		m_zombieGuy.SetAnimationToBindPose();
		m_zombieGuy.NewRagdollFromAnimatedTransforms();

		m_NurseGuy.GetSkinnedModel()->PrintBonesList();



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
		AssetManager::PrintSkinnedModelBoneNames("Shotgun.fbx");
		std::cout << "\n";*/

	}

	void Game::OnUpdate()
	{
		WeaponLogic::Update(m_frameTime);

		//m_NurseGuy.m_worldTransform = Renderer::s_DebugTransform;

		// Keypresses
		Input::HandleKeypresses();
		if (Input::s_keyPressed[HELL_KEY_E])
			OnInteract();



		m_player.Update(m_frameTime);
		m_player.m_characterController.Update(m_frameTime, &camera);


		Physics::Update(m_frameTime);


		for (VolumetricBloodSplatter& volumetricBloodSplater : VolumetricBloodSplatter::s_volumetricBloodSplatters) {
			volumetricBloodSplater.Update(m_frameTime);
		}

	//	if (Input::s_keyPressed[HELL_KEY_P])
	//		m_zombieGuy.SetAnimationToBindPose();

		if (Input::s_keyPressed[HELL_KEY_L])
			m_zombieGuy.m_ragdoll->DisableSkinning = !m_zombieGuy.m_ragdoll->DisableSkinning;


		if (Input::s_keyPressed[HELL_KEY_N])
		{
			/* old ragdoll 
			for (int i = 0; i < Ragdoll::BODYPART_COUNT; ++i)
				Physics::s_dynamicsWorld->removeRigidBody(m_zombieGuy.m_ragdoll->m_bodies[i]);

			for (int i = 0; i < Ragdoll::JOINT_COUNT; ++i)
				Physics::s_dynamicsWorld->removeConstraint(m_zombieGuy.m_ragdoll->m_joints[i]);
				*/
			m_zombieGuy.m_ragdoll->RemovePhysicsObjects();

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

		//Renderer::s_bloodVolumetricEffect.Update(m_frameTime);

		TextBlitter::UpdateBlitter(m_frameTime);

		
		// Doors
		for (Door& door : GameData::p_house->m_doors)
			door.Update(m_frameTime);

		WeaponLogic::Update(m_frameTime);

		//ShotgunLogic::Update(m_frameTime);
		m_cameraRaycast.CastRay(camera.m_viewPos, camera.m_Front, 10.0f); //ShotgunLogic::m_raycast;

		



		
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

		if (!CoreImGui::s_Show)
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

	//	WeaponLogic::UpdateSkeletalAnimation(deltaTime);

		// Update all skeletal meshes
		//m_testAnimatedEnttity.Update(deltaTime);

		m_NurseGuy.Update(deltaTime);
/*
		std::cout << "\n";
		std::cout << "anim cout: " << m_NurseGuy.GetSkinnedModel()->m_animations.size() << "\n";
		std::cout << "anim index: " << m_NurseGuy.GetSkinnedModel()->currentAnimationIndex << "\n";
		*/
		
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
		if (m_cameraRaycast.m_objectType == PhysicsObjectType::DOOR)
		{
			Door* door = (Door*)m_cameraRaycast.m_objectPtr;
			door->Interact();
		}
	}

	void Game::RebuildMap()
	{
		static bool FIRST_BUILD = true;
		

		//house.RebuildAll();
		//Physics::RebuildWorld(&house);



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