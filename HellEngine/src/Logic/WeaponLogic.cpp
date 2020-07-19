#include "hellpch.h"
#include "WeaponLogic.h"
#include "ShotgunLogic.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	unsigned int WeaponLogic::s_SelectedWeapon;
	std::vector<WEAPON> WeaponLogic::s_WeaponList;
	glm::mat4 WeaponLogic::s_AnimatedCameraMatrix; 
	AnimatedEntity WeaponLogic::m_shotgunAnimatedEntity;
	AnimatedEntity WeaponLogic::m_glockAnimatedEntiyty;

	void WeaponLogic::Init()
	{
		// Setup Inventory
		s_SelectedWeapon = 0;
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
	}
	
	void WeaponLogic::Update(float deltaTime)
	{
		// Weapon select
		if (Input::s_keyPressed[HELL_KEY_1] && s_WeaponList.size() > 0)
			s_SelectedWeapon = s_WeaponList[0];

		if (Input::s_keyPressed[HELL_KEY_2] && s_WeaponList.size() > 1)
			s_SelectedWeapon = s_WeaponList[1];

		if (Input::s_keyPressed[HELL_KEY_3] && s_WeaponList.size() > 3)
			s_SelectedWeapon = s_WeaponList[2];

		if (Input::s_keyPressed[HELL_KEY_4] && s_WeaponList.size() > 4)
			s_SelectedWeapon = s_WeaponList[3];
	}

	void WeaponLogic::UpdateSkeletalAnimation(float deltatime)
	{
		if (s_SelectedWeapon == WEAPON::GLOCK) {
			m_glockAnimatedEntiyty.m_currentAnimationTime = 2.1f;
			m_glockAnimatedEntiyty.PauseAnimation();
			m_glockAnimatedEntiyty.Update(deltatime);
		}
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
	glm::mat4 WeaponLogic::GetCameraMatrix()
	{
		return s_AnimatedCameraMatrix;
	}
}