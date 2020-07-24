#include "hellpch.h"
#include "GlockLogic.h"
#include "WeaponLogic.h"
#include "Helpers/Util.h"
#include "Core/Input.h"
#include "Audio/audio.h"
#include "Renderer/renderer.h"
#include "Renderer/Effects/Decal.h"
#include "Misc/Shell.h"

namespace HellEngine
{
	GunState GlockLogic::m_gunState = GunState::IDLE;
	ReloadState GlockLogic::m_reloadState = ReloadState::NOT_RELOADING;
	//IronSightState GlockLogic::m_ironSightState = IronSightState::NOT_IRON_SIGHTING;
	//RaycastResult GlockLogic::m_raycast;
	int GlockLogic::m_AmmoInGun = 15;
	int GlockLogic::m_AmmoAvaliable = 1000;
	Camera* GlockLogic::p_camera;
	Player* GlockLogic::p_player;
	AnimatedEntity* GlockLogic::p_model;
	bool GlockLogic::s_awaitingShell;
	int GlockLogic::s_RandomFireAnimation;

	void GlockLogic::Update(float deltatime)
	{
		if (Input::s_leftMousePressed)
			Fire();

		if ((Input::s_keyPressed[HELL_KEY_R]))
			Reload();

		//SpawnShellIfRequired();
		Animate(deltatime);
	}


	void GlockLogic::Fire()
	{
		// Fire empty glock
		if (m_AmmoInGun == 0) {
			Audio::PlayAudio("Glock_DryFire.wav");
			return;
		}

		// not if you are reloading mate
		if (m_reloadState == ReloadState::RELOAD_CLIP)
			return;

		// not if you are empty mate
		if (m_AmmoInGun <= 0)
			return;


		// Fire glock
		if ((m_gunState != GunState::FIRING) ||	(m_gunState == GunState::FIRING && p_model->m_currentAnimationTime > 0.15f))
		{
			s_RandomFireAnimation = rand() % 3;
			m_reloadState = ReloadState::NOT_RELOADING;
			m_gunState = GunState::FIRING;
			m_AmmoInGun -= 1;
			
			// Audio
			int RandomAudio = rand() % 3;

			if (RandomAudio == 0)
				Audio::PlayAudio("Glock_Fire_0.wav");
			if (RandomAudio == 1)
				Audio::PlayAudio("Glock_Fire_1.wav");
			if (RandomAudio == 2)
				Audio::PlayAudio("Glock_Fire_2.wav");

			//s_awaitingShell = true;
			p_model->m_currentAnimationTime = 0;
			Renderer::s_muzzleFlash.CreateFlash(GetGlockBarrelHoleWorldPosition());
			Renderer::s_muzzleFlash.m_CurrentTime = 0;

			bool playFleshSound = false;

			for (int i = 0; i < 1; i++)
			{
				RaycastResult raycastResult;
				raycastResult.CastRay(p_camera->m_viewPos, p_camera->m_Front, 10.0f, 0.05f);

				// make bullet hole if it aint the ragdoll
				if (raycastResult.m_name != "RAGDOLL")
					Decal::s_decals.push_back(Decal(raycastResult.m_hitPoint, raycastResult.m_surfaceNormal));

				// make blood on couches and ragdolls
				if ((raycastResult.m_name == "NEW MESH") || (raycastResult.m_name == "RAGDOLL")) {
					Renderer::s_bloodEffect.m_CurrentTime = 0;
					Renderer::s_bloodVolumetricEffect.m_CurrentTime = 0;
					playFleshSound = true;
					Renderer::s_hitPoint.position = raycastResult.m_hitPoint;
					Renderer::s_hitPoint.rotation = p_camera->m_transform.rotation;
				}
				WeaponLogic::s_BulletHits.emplace_back(raycastResult);


				if (raycastResult.m_name == "RAGDOLL")
				{
					float FORCE_SCALING_FACTOR = 22;// Config::TEST_FLOAT;
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


	void GlockLogic::Reload()
	{
		// Can only reload if your gun aint busy
		//if (m_gunState != GunState::IDLE && m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
		//	return;

		// But if you aint busy, and have ammo, then go for it
		if (m_AmmoInGun < 15 && m_AmmoAvaliable > 0) {
			m_gunState = GunState::RELOADING;
			//m_ironSightState = IronSightState::NOT_IRON_SIGHTING;

			if (m_AmmoInGun == 0) {
				m_reloadState = ReloadState::RELOAD_CLIP_FROM_EMPTY;
				Audio::PlayAudio("Glock_ReloadFromEmpty.wav");
			}
			else {
				m_reloadState = ReloadState::RELOAD_CLIP;
				Audio::PlayAudio("Glock_Reload.wav");
			}

			m_AmmoInGun = 15;
			m_AmmoAvaliable -= 15;
		}
	}

	void GlockLogic::Animate(float deltatime)
	{
		// Idle 
		if (m_gunState == GunState::IDLE)
		{			
			p_model->PlayAnimation("Glock_Idle.fbx", true);
		}

		// Fire
		if (m_gunState == GunState::FIRING)
		{			
			if (s_RandomFireAnimation == 0)
				p_model->PlayAnimation("Glock_Fire0.fbx", false);
			if (s_RandomFireAnimation == 1)
				p_model->PlayAnimation("Glock_Fire1.fbx", false);
			if (s_RandomFireAnimation == 2)
				p_model->PlayAnimation("Glock_Fire2.fbx", false);

			if (p_model->IsAnimationComplete())
				m_gunState = GunState::IDLE;
		}

		// Reloading
		if (m_gunState == GunState::RELOADING)
		{
			if (m_reloadState == ReloadState::RELOAD_CLIP_FROM_EMPTY)
				p_model->PlayAnimation("Glock_EmptyReload.fbx", false);
			else
				p_model->PlayAnimation("Glock_Reload.fbx", false);
			//p_model->PlayAnimation("Glock_EmptyReload.fbx", false);

			// Have you finished?
			if ((p_model->m_currentAnimationTime > 2.1f && ReloadState::RELOAD_CLIP_FROM_EMPTY)
				|| (p_model->IsAnimationComplete()))
			{
				m_gunState = GunState::IDLE;
				m_reloadState = ReloadState::NOT_RELOADING;
			}
		
			/*
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
			}*/
		}
	}

	/*void ShotgunLogic::SpawnShellIfRequired()
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
	}*/
	
	glm::vec3 GlockLogic::GetGlockBarrelHoleWorldPosition()
	{
		static Transform camTrans;
		camTrans.scale = glm::vec3(0.002f);
		camTrans.position = p_camera->m_viewPos;
		camTrans.rotation = p_camera->m_transform.rotation;

		unsigned int BoneIndex = p_model->GetSkinnedModel()->m_BoneMapping["barrel"];

		Transform hardcodedLocatorTransform;
		hardcodedLocatorTransform.position = glm::vec3(0, -15, 10);

		glm::mat4 worldMatrix = camTrans.to_mat4() * p_camera->m_weaponSwayTransform.to_mat4() * p_model->GetSkinnedModel()->m_BoneInfo[BoneIndex].FinalTransformation * hardcodedLocatorTransform.to_mat4();
		return Util::TranslationFromMat4(worldMatrix);
	}
	/*
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
	}*/
}