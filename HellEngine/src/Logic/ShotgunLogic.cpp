#include "hellpch.h"
#include "ShotgunLogic.h"
#include "Helpers/Util.h"
#include "Core/Input.h"
#include "Audio/audio.h"
#include "Renderer/renderer.h"
#include "Renderer/Effects/Decal.h"
#include "Misc/Shell.h"

namespace HellEngine
{
	GunState ShotgunLogic::m_gunState = GunState::IDLE;
	ReloadState ShotgunLogic::m_reloadState = ReloadState::NOT_RELOADING;
	IronSightState ShotgunLogic::m_ironSightState = IronSightState::NOT_IRON_SIGHTING;
	RaycastResult ShotgunLogic::m_raycast;
	int ShotgunLogic::m_AmmoInGun = 8;
	int ShotgunLogic::m_AmmoAvaliable = 1000;
	Camera* ShotgunLogic::p_camera;
	Player* ShotgunLogic::p_player;
	AnimatedEntity* ShotgunLogic::p_model;
	bool ShotgunLogic::s_awaitingShell;

	void ShotgunLogic::Update(float deltatime)
	{
		if (Input::s_leftMousePressed)
			FireShotgun();

		//if (Input::s_rightMousePressed)
		//	BeginIronSighting();

		if (Input::s_rightMouseDown)
			BeginIronSighting();

		if (!Input::s_rightMouseDown)
			EndIronSighting();

		if ((Input::s_keyPressed[HELL_KEY_R]))
			ReloadShotgun();

		SpawnShellIfRequired();
		Animate(deltatime);

	}


	void ShotgunLogic::FireShotgun()
	{
		// Fire empty shotgun
		if (m_AmmoInGun == 0) {
			Audio::PlayAudio("Empty.wav");
			return;
		}

		// Fire shotgun
		if (m_gunState != GunState::FIRING && m_AmmoInGun > 0)
		{
			m_reloadState = ReloadState::NOT_RELOADING;
			m_gunState = GunState::FIRING;
			m_AmmoInGun -= 1;
			Audio::PlayAudio("Shotgun_Fire_01.wav");
			s_awaitingShell = true;
			p_model->m_currentAnimationTime = 0;
			Renderer::s_muzzleFlash.CreateFlash(GetShotgunBarrelHoleWorldPosition());

		//	Renderer::s_muzzleFlash.m_CurrentTime = 0;
			bool playFleshSound = false;

			for (int i = 0; i < 12; i++)
			{
				RaycastResult raycastResult;
				raycastResult.CastRay(p_camera->m_viewPos, p_camera->m_Front, 10.0f, 0.125f);
				
				// make bullet hole if it aint the ragdoll
				if (raycastResult.m_name != "RAGDOLL")
					Decal::s_decals.push_back(Decal(raycastResult.m_hitPoint, raycastResult.m_surfaceNormal));

				// make blood on couches and ragdolls
				if ((raycastResult.m_name == "NEW MESH") || (raycastResult.m_name == "RAGDOLL")) {
					Renderer::s_bloodEffect.m_CurrentTime = 0;
					playFleshSound = true;
					Renderer::s_hitPoint.position = m_raycast.m_hitPoint;
					Renderer::s_hitPoint.rotation = p_camera->m_transform.rotation;
				}
				m_raycast = raycastResult;


				if (raycastResult.m_name == "RAGDOLL") 
				{
					float FORCE_SCALING_FACTOR = 2;// 5;// Config::TEST_FLOAT;
					raycastResult.m_rigidBody->activate(true);
					btVector3 centerOfMass = raycastResult.m_rigidBody->getCenterOfMassPosition();
					btVector3 hitPoint = Util::glmVec3_to_btVec3(raycastResult.m_hitPoint);
					btVector3 force = Util::glmVec3_to_btVec3(p_camera->m_Front) * FORCE_SCALING_FACTOR;
					raycastResult.m_rigidBody->applyImpulse(force, hitPoint - centerOfMass);
				}
			}

			if (playFleshSound)
				Audio::PlayAudio("Impact_Flesh.wav", 1.0f);
		}
	}

	
	void ShotgunLogic::ReloadShotgun()
	{
		// Can only reload if your gun aint busy
		if (m_gunState != GunState::IDLE && m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
			return;
		
		// But if you aint busy, and have ammo, then go for it
		if (m_AmmoInGun < 8 && m_AmmoAvaliable > 0) {
			m_gunState = GunState::RELOADING;
			m_reloadState = ReloadState::FROM_IDLE;
			m_ironSightState = IronSightState::NOT_IRON_SIGHTING;
			Audio::PlayAudio("Shotgun_ReloadStart.wav");
		}
	}


	void ShotgunLogic::BeginIronSighting()
	{
		if (m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
		{
			m_ironSightState = IronSightState::BEGIN_IRON_SIGHTING;
			m_reloadState = ReloadState::NOT_RELOADING;
			m_gunState = GunState::IDLE;
		}
	}


	void ShotgunLogic::EndIronSighting()
	{
		if (m_gunState != GunState::FIRING)
			if (m_ironSightState == IronSightState::IRON_SIGHTING)
				m_ironSightState = IronSightState::END_IRON_SIGHTING;
	}


	void ShotgunLogic::Animate(float deltatime)
	{
		static bool firstShellLoaded = false;
		static bool secondShellLoaded = false;

		// Being iron sight?
		if (m_ironSightState == IronSightState::BEGIN_IRON_SIGHTING)
		{
			p_model->PlayAnimation("Shotgun_SightingIn.fbx", false);

			if (p_model->IsAnimationComplete())
				m_ironSightState = IronSightState::IRON_SIGHTING;

			if (m_gunState == GunState::FIRING)
				m_ironSightState = IronSightState::IRON_SIGHTING;
		}	
		
		// End iron sight?
		if (m_ironSightState == IronSightState::END_IRON_SIGHTING)
		{
			p_model->PlayAnimation("Shotgun_SightingOut.fbx", false);

			if (p_model->IsAnimationComplete())
				m_ironSightState = IronSightState::NOT_IRON_SIGHTING;
		}

		// Idle 
		if (m_gunState == GunState::IDLE && p_player->m_movementState == PlayerMovementState::STOPPED)
		{
			if (m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
					p_model->PlayAnimation("Shotgun_Idle.fbx", true);

			if (m_ironSightState == IronSightState::IRON_SIGHTING) {
				p_model->PlayAnimation("Shotgun_SightingIn.fbx", false);
				p_model->PauseOnFinalFrame();
			}
		}

		// Walking
		if (p_player->m_movementState == PlayerMovementState::WALKING)
		{
			if (m_gunState == GunState::IDLE)
			{
				if (m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
					p_model->PlayAnimation("Shotgun_Walk.fbx", true);

				if (m_ironSightState == IronSightState::IRON_SIGHTING)
					p_model->PlayAnimation("Shotgun_SightingWalk.fbx", true);
			}
		}

		// Fire
		if (m_gunState == GunState::FIRING)
		{
			if (m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
				p_model->PlayAnimation("Shotgun_Fire.fbx", false);

			if (m_ironSightState == IronSightState::IRON_SIGHTING)
				p_model->PlayAnimation("Shotgun_SightingFire.fbx", false);

			if (p_model->IsAnimationComplete())
				m_gunState = GunState::IDLE;
		}


		// Reloading
		if (m_gunState == GunState::RELOADING)
		{
			// Have you finished?
			if (m_reloadState == ReloadState::BACK_TO_IDLE)
			{
				if (p_model->IsAnimationComplete())
				{
					m_gunState = GunState::IDLE;
					m_reloadState = ReloadState::NOT_RELOADING;
				}
			}

			// Or are you just starting
			if (m_reloadState == ReloadState::FROM_IDLE) {
				p_model->PlayAnimation("Shotgun_ReloadWetStart.fbx", false);
			}

			// Or have you finished just starting, then you better begin actual reload
			if (m_reloadState == ReloadState::FROM_IDLE && p_model->IsAnimationComplete())
			{

				// How many shots
				if (m_AmmoInGun < 7)
					m_reloadState = ReloadState::DOUBLE_RELOAD;
				else
					m_reloadState = ReloadState::SINGLE_RELOAD;


				if (m_reloadState == ReloadState::SINGLE_RELOAD) {
					p_model->PlayAnimation("Shotgun_Reload1Shell.fbx", false);
				}
				else if (m_reloadState == ReloadState::DOUBLE_RELOAD) {
					p_model->PlayAnimation("Shotgun_Reload2Shells.fbx", false);
				}
			}

			// So the reloading animation is playing
			if (m_reloadState == ReloadState::SINGLE_RELOAD || m_reloadState == ReloadState::DOUBLE_RELOAD)
			{
				// Is it finished
				if (p_model->IsAnimationComplete())
				{
					p_model->ResetAnimationTimer();
					p_model->m_currentAnimationTime = 0;

					// How many shots
					if (m_AmmoInGun < 7 && m_AmmoAvaliable >= 2)
					{
						m_reloadState = ReloadState::DOUBLE_RELOAD;
						p_model->PlayAnimation("Shotgun_Reload2Shells.fbx", false);
					}
					else if (m_AmmoInGun < 8 && m_AmmoAvaliable >= 1)
					{
						m_reloadState = ReloadState::SINGLE_RELOAD;
						p_model->PlayAnimation("Shotgun_Reload1Shell.fbx", false);
					}
					else if (m_AmmoInGun == 8 || m_AmmoAvaliable == 0)
					{
						m_reloadState = ReloadState::BACK_TO_IDLE;
						p_model->PlayAnimation("Shotgun_ReloadEnd.fbx", false);
						Audio::PlayAudio("Shotgun_ReloadEnd.wav");
					}

					firstShellLoaded = false;
					secondShellLoaded = false;
				}
			}

			// play noise and increase counter
			if (m_reloadState == ReloadState::DOUBLE_RELOAD)
			{
				if (!firstShellLoaded && p_model->m_currentAnimationTime > 0.56f) {
					firstShellLoaded = true;
					Audio::PlayAudio("Shotgun_Reload_01.wav");
					m_AmmoInGun++;
					m_AmmoAvaliable--;
				}
				if (!secondShellLoaded && p_model->m_currentAnimationTime > 1.03f) {
					secondShellLoaded = true;
					Audio::PlayAudio("Shotgun_Reload_05.wav");
					m_AmmoInGun++;
					m_AmmoAvaliable--;
				}
			}
			// play noise and increase counter
			if (m_reloadState == ReloadState::SINGLE_RELOAD)
			{
				if (!firstShellLoaded && p_model->m_currentAnimationTime > 0.45f) {
					firstShellLoaded = true;
					Audio::PlayAudio("Shotgun_Reload_01.wav");
					m_AmmoInGun++;
					m_AmmoAvaliable--;
				}
			}
		}
	}

	void ShotgunLogic::SpawnShellIfRequired()
	{
		if (!s_awaitingShell)
			return;


		if (m_gunState == GunState::FIRING && p_model->m_currentAnimationTime > 0.53f)
		{
			s_awaitingShell = false;
		

			Transform t;
			t.position = GetShotgunShellSpawnWorldPosition();
			t.rotation = p_camera->m_transform.rotation;

			glm::vec3 initialVelocity;
			initialVelocity = glm::normalize(p_camera->m_Up + (p_camera->m_Right * glm::vec3(3.0f)));
			initialVelocity *= glm::vec3(1.5f);

			Shell::s_shells.push_back(Shell(t, initialVelocity));
		}	
	}
	glm::vec3 ShotgunLogic::GetShotgunBarrelHoleWorldPosition()
	{
		static Transform camTrans;
		camTrans.scale = glm::vec3(0.002f);
		camTrans.position = p_camera->m_viewPos;
		camTrans.rotation = p_camera->m_transform.rotation;

		SkinnedModel* model = AssetManager::skinnedModels[AssetManager::GetSkinnedModelIDByName("Shotgun.fbx")];
		unsigned int BoneIndex = model->m_BoneMapping["ShotgunMain_bone"];

		Transform hardcodedLocatorTransform;
		hardcodedLocatorTransform.position = glm::vec3(0, -73, 6);

		glm::mat4 worldMatrix = camTrans.to_mat4() * p_camera->m_weaponSwayTransform.to_mat4() * model->m_BoneInfo[BoneIndex].FinalTransformation * hardcodedLocatorTransform.to_mat4();
		return Util::TranslationFromMat4(worldMatrix);
	}

	glm::vec3 ShotgunLogic::GetShotgunShellSpawnWorldPosition()
	{
		Transform trans;
		trans.scale = glm::vec3(0.002f);
		trans.position = p_camera->m_viewPos;
		trans.rotation = p_camera->m_transform.rotation;

		trans.position += p_camera->m_Up * glm::vec3(-0.005f);
		trans.position += p_camera->m_Right * glm::vec3(0.02f);
		trans.position += p_camera->m_Front * glm::vec3(0.16f);

		SkinnedModel* model = AssetManager::skinnedModels[AssetManager::GetSkinnedModelIDByName("Shotgun.fbx")];
		unsigned int BoneIndex = model->m_BoneMapping["Bolt_bone"];

		glm::mat4 worldMatrix = trans.to_mat4() * p_camera->m_weaponSwayTransform.to_mat4() * model->m_BoneInfo[BoneIndex].FinalTransformation;
		return Util::TranslationFromMat4(worldMatrix);
	}
}