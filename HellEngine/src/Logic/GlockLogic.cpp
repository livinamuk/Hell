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
	bool GlockLogic::s_firstEquip;
	int GlockLogic::s_RandomFireAnimation;

	void GlockLogic::Update(float deltatime)
	{
		if (m_gunState != GunState::DEQUIP && m_gunState != GunState::EQUIP)
		{
			if (Input::s_leftMousePressed)
				Fire();

			if ((Input::s_keyPressed[HELL_KEY_R]))
				Reload();
		}

		//SpawnShellIfRequired();
		Animate(deltatime);

		//if (m_AmmoInGun == 0)
			//AnimateEmptySlide();
	}


	void GlockLogic::Fire()
	{	
		// Fire empty glock
		if (m_AmmoInGun == 0) {
			Audio::PlayAudio("Glock_DryFire.wav");
			return;
		}

		// not if you are reloading mate
		if (m_reloadState == ReloadState::RELOAD_CLIP || m_reloadState == ReloadState::RELOAD_CLIP_FROM_EMPTY)
			return;

		// not if you are empty mate
		if (m_AmmoInGun <= 0)
			return;


		// Fire glock
		if ((m_gunState != GunState::FIRING) ||	(m_gunState == GunState::FIRING && p_model->m_currentAnimationTime > 0.15f))
		{
			s_RandomFireAnimation = rand() % 4;
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
			GetGlockCasingSpawnWorldPosition();
			Renderer::s_muzzleFlash.m_CurrentTime = 0;



			WeaponLogic::FireBullet(p_camera->m_viewPos, p_camera->m_Front, 0.05f, 22, WEAPON::GLOCK);
				

			
			SpawnBulletCasing();
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
		// Equip
		if (m_gunState == GunState::EQUIP)		{
			//p_model->PlayAnimation("Glock_FirstEquip1.fbx", false);
			p_model->PlayAnimation("Glock_Equip2.fbx", false);

			if (p_model->IsAnimationComplete()) {
				p_model->PlayAnimation("Glock_Idle.fbx", true);
				m_gunState = GunState::IDLE;
				m_reloadState = ReloadState::NOT_RELOADING;
			}
			return;
		}
		// Dequiping?
		if (m_gunState == GunState::DEQUIP) {
			p_model->PlayAnimation("Glock_Holster.fbx", false);

			if (p_model->IsAnimationComplete())
				WeaponLogic::SwitchToDesiredWeapon();

			return;
		}


		// Idle 
		if (m_gunState == GunState::IDLE)			
		{
			if (p_player->m_movementState != PlayerMovementState::WALKING)
			{
				//if (!WeaponLogic::m_singleHanded)
					p_model->PlayAnimation("Glock_Idle.fbx", true);
				//else
				//	p_model->PlayAnimation("Glock_SingleHanded_Idle.fbx", true);
			}
			else
				p_model->PlayAnimation("Glock_Walk.fbx", true);
		}

		// Fire
		if (m_gunState == GunState::FIRING)
		{
			//if (!WeaponLogic::m_singleHanded) 
			{
				if (s_RandomFireAnimation == 0)
					p_model->PlayAnimation("Glock_Fire0.fbx", false);
				if (s_RandomFireAnimation == 1)
					p_model->PlayAnimation("Glock_Fire1.fbx", false);
				if (s_RandomFireAnimation == 2)
					p_model->PlayAnimation("Glock_Fire2.fbx", false);
				if (s_RandomFireAnimation == 3)
					p_model->PlayAnimation("Glock_Fire3.fbx", false);
			}
			//else
			//	p_model->PlayAnimation("Glock_SingleHanded_Fire0.fbx", false);

			if (p_model->IsAnimationComplete())
				m_gunState = GunState::IDLE;
		}

		// Reloading
		if (m_gunState == GunState::RELOADING)
		{
			if (m_reloadState == ReloadState::RELOAD_CLIP_FROM_EMPTY)
				//p_model->PlayAnimation("Glock_EmptyReload.fbx", false);
				p_model->PlayAnimation("Glock_Reload.fbx", false);
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
		unsigned int BoneIndex = p_model->GetSkinnedModel()->m_BoneMapping["barrel"];
		glm::mat4 BoneMatrix = p_model->GetSkinnedModel()->m_BoneInfo[BoneIndex].FinalTransformation;

		Transform offsetTransform;
		offsetTransform.position = glm::vec3(0, -15, 10);

		glm::mat4 worldMatrix = WeaponLogic::s_weaponTransform.to_mat4() * p_camera->m_weaponSwayTransform.to_mat4() * BoneMatrix * offsetTransform.to_mat4();
		return Util::TranslationFromMat4(worldMatrix);
	}

	glm::vec3 GlockLogic::GetGlockCasingSpawnWorldPosition()
	{
		unsigned int BoneIndex = p_model->GetSkinnedModel()->m_BoneMapping["barrel"];
		glm::mat4 BoneMatrix = p_model->GetSkinnedModel()->m_BoneInfo[BoneIndex].FinalTransformation;

		Transform offsetTransform = Renderer::s_DebugTransform;
	//offsetTransform.position = glm::vec3(-1, 3, 11);

		glm::mat4 worldMatrix = WeaponLogic::s_weaponTransform.to_mat4() * p_camera->m_weaponSwayTransform.to_mat4() * BoneMatrix * offsetTransform.to_mat4();
		return Util::TranslationFromMat4(worldMatrix);
	}

	void GlockLogic::SpawnBulletCasing()
	{
		Transform t;	
		//t.position = GetGlockCasingSpawnWorldPosition();
		t.position = GetGlockBarrelHoleWorldPosition();// it look sbetter spawning at the barrel lol. too close otherwise
		t.rotation = p_camera->m_transform.rotation;

		glm::vec3 initialVelocity;
		initialVelocity = glm::normalize(p_camera->m_Up + (p_camera->m_Right * glm::vec3(3.0f)));
		initialVelocity *= glm::vec3(1.5f);

		Shell::s_bulletCasings.push_back(Shell(t, initialVelocity, CasingType::BULLET_CASING));
	}

	/*void GlockLogic::AnimateEmptySlide()
	{
		unsigned int BoneIndex = p_model->GetSkinnedModel()->m_BoneMapping["slide"];
		glm::mat4 BoneMatrix = WeaponLogic::m_glockAnimatedEntiyty.m_animatedTransforms[BoneIndex];

		Transform offsetTransform = Renderer::s_DebugTransform;

		WeaponLogic::m_glockAnimatedEntiyty.m_animatedTransforms[BoneIndex] = BoneMatrix * offsetTransform.to_mat4();

	//	Util::PrintMat4(p_model->GetSkinnedModel()->m_BoneInfo[BoneIndex].FinalTransformation);
		//p_model->m_animatedTransforms[]
	}*/
}