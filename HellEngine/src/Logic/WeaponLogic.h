#pragma once
#include "Header.h"
#include "Core/Camera.h"
#include "Core/AnimatedEntity.h"
#include "Physics/RaycastResult.h"
#include "Core/CoreImGui.h"

namespace HellEngine
{
	class WeaponLogic
	{
	public: // static functions
		static void Init();
		static void Update(float deltatime);
		static void UpdateSkeletalAnimation(float deltatime);
		static void RenderCurrentWeapon(Shader* shader, Camera* camera);
		static void DequipCurrentWeapon();
		static void SwitchToDesiredWeapon();
		static glm::mat4 GetCameraMatrix();
		static glm::vec3 GetBarrelHoleWorldPosition();

	public: // static variables
		static unsigned int s_SelectedWeapon;
		static unsigned int s_desiredWeapon;
		static std::vector<WEAPON> s_WeaponList;
		static glm::mat4 s_AnimatedCameraMatrix;
		static Transform s_weaponTransform;

		static int m_AmmoInGun;
		static int m_AmmoAvaliable;
		static bool m_singleHanded;

		static GunState p_gunState;
		static ReloadState p_reloadState;
		static AnimatedEntity* p_currentAnimatedEntity;
		static AnimatedEntity m_shotgunAnimatedEntity;
		static AnimatedEntity m_glockAnimatedEntiyty;

		static std::vector<RaycastResult> s_BulletHits; // bullet hits this frame
	};
}