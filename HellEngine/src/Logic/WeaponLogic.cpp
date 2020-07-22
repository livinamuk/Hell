#include "hellpch.h"
#include "WeaponLogic.h"
#include "ShotgunLogic.h"
#include "GlockLogic.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	unsigned int WeaponLogic::s_SelectedWeapon;
	unsigned int WeaponLogic::s_desiredWeapon;
	std::vector<WEAPON> WeaponLogic::s_WeaponList;
	glm::mat4 WeaponLogic::s_AnimatedCameraMatrix; 
	AnimatedEntity WeaponLogic::m_shotgunAnimatedEntity;
	AnimatedEntity WeaponLogic::m_glockAnimatedEntiyty; 
	AnimatedEntity* WeaponLogic::p_currentAnimatedEntity;
	GunState WeaponLogic::p_gunState; 
	ReloadState WeaponLogic::p_reloadState;
	int WeaponLogic::m_AmmoInGun = 8;
	int WeaponLogic::m_AmmoAvaliable = 1000;
	std::vector<RaycastResult> WeaponLogic::s_BulletHits;

	void WeaponLogic::Init()
	{
		// Setup Inventory
		s_SelectedWeapon = 1;
		s_WeaponList.clear();
		s_WeaponList.push_back(WEAPON::GLOCK);
		s_WeaponList.push_back(WEAPON::SHOTGUN);

		// Weapon models
		m_shotgunAnimatedEntity.SetSkinnedModel("Shotgun.fbx");
		m_glockAnimatedEntiyty.SetSkinnedModel("Glock.fbx");
		m_glockAnimatedEntiyty.m_currentAnimationIndex = 1;

		// Weapon Materials
		m_shotgunAnimatedEntity.SetMaterial("Arms", "Hands");
		m_shotgunAnimatedEntity.SetMaterial("Shotgun Mesh", "Shotgun");
		m_shotgunAnimatedEntity.SetMaterial("shotgunshells", "Shell");

		m_glockAnimatedEntiyty.SetMaterial("manniquen1_2", "Hands");
		m_glockAnimatedEntiyty.SetMaterial("Glock", "Glock");

		ShotgunLogic::p_model = &m_shotgunAnimatedEntity;
		GlockLogic::p_model = &m_glockAnimatedEntiyty;
	}
	
	void WeaponLogic::Update(float deltaTime)
	{
		// clear any bullet hits from last frame
		s_BulletHits.clear();

		// Weapon select
		if (Input::s_keyPressed[HELL_KEY_1] && s_WeaponList.size() > 0) {
			s_desiredWeapon = s_WeaponList[0];
			DequipCurrentWeapon();
		}

		if (Input::s_keyPressed[HELL_KEY_2] && s_WeaponList.size() > 1) {
			s_desiredWeapon = s_WeaponList[1];
			DequipCurrentWeapon();
		}

		if (Input::s_keyPressed[HELL_KEY_3] && s_WeaponList.size() > 3) {
			s_desiredWeapon = s_WeaponList[2];
			DequipCurrentWeapon();
		}

		if (Input::s_keyPressed[HELL_KEY_4] && s_WeaponList.size() > 4) {
			s_desiredWeapon = s_WeaponList[3];
			DequipCurrentWeapon();
		}

		// Updates
		if (s_SelectedWeapon == WEAPON::GLOCK) {
			GlockLogic::Update(deltaTime);
			m_AmmoInGun = GlockLogic::m_AmmoInGun;
			m_AmmoAvaliable = GlockLogic::m_AmmoAvaliable;
			p_gunState = GlockLogic::m_gunState;
			p_reloadState = GlockLogic::m_reloadState;
			p_currentAnimatedEntity = &m_glockAnimatedEntiyty;
		}
		if (s_SelectedWeapon == WEAPON::SHOTGUN) {
			ShotgunLogic::Update(deltaTime);
			m_AmmoInGun = ShotgunLogic::m_AmmoInGun;
			m_AmmoAvaliable = ShotgunLogic::m_AmmoAvaliable;
			p_gunState = ShotgunLogic::m_gunState;
			p_reloadState = ShotgunLogic::m_reloadState;
			p_currentAnimatedEntity = &m_shotgunAnimatedEntity;
		}
	}

	void WeaponLogic::UpdateSkeletalAnimation(float deltatime)
	{
		if (s_SelectedWeapon == WEAPON::GLOCK)
			m_glockAnimatedEntiyty.Update(deltatime);
		
		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			m_shotgunAnimatedEntity.Update(deltatime);
	}

	void WeaponLogic::RenderCurrentWeapon(Shader* shader, Camera* camera)
	{
		GpuProfiler g("Weapon");

		static Transform trans;
		trans.position = camera->m_viewPos;
		trans.rotation = camera->m_transform.rotation;
		trans.scale = glm::vec3(0.002f);

		glm::mat4 weaponModelMatrix = trans.to_mat4() * camera->m_weaponSwayTransform.to_mat4();

		if (s_SelectedWeapon == WEAPON::GLOCK)
			m_glockAnimatedEntiyty.Draw(shader, weaponModelMatrix);

		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			m_shotgunAnimatedEntity.Draw(shader, weaponModelMatrix);

	}
	void WeaponLogic::DequipCurrentWeapon()
	{
		if (s_SelectedWeapon == WEAPON::GLOCK)
			GlockLogic::m_gunState = GunState::DEQUIP;

		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			ShotgunLogic::m_gunState = GunState::DEQUIP;
	}

	void WeaponLogic::SwitchToDesiredWeapon()
	{
		s_SelectedWeapon = s_desiredWeapon;

		if (s_SelectedWeapon == WEAPON::GLOCK)
			GlockLogic::m_gunState = GunState::EQUIP;

		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			ShotgunLogic::m_gunState = GunState::EQUIP;
	}

	glm::mat4 WeaponLogic::GetCameraMatrix()
	{
		return s_AnimatedCameraMatrix;
	}

	glm::vec3 WeaponLogic::GetBarrelHoleWorldPosition()
	{
		if (s_SelectedWeapon == WEAPON::GLOCK)
			return GlockLogic::GetGlockBarrelHoleWorldPosition();

		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			return ShotgunLogic::GetShotgunBarrelHoleWorldPosition();
	}
}