#pragma once
#include "Header.h"
#include "Core/Camera.h"
#include "Core/AnimatedEntity.h"

namespace HellEngine
{
	class WeaponLogic
	{
	public: // static functions
		static void Init();
		static void Update(float deltatime);
		static void UpdateSkeletalAnimation(float deltatime);
		static void RenderCurrentWeapon(Shader* shader, Camera* camera);
		static glm::mat4 GetCameraMatrix();

	public: // static variables
		static unsigned int s_SelectedWeapon;
		static std::vector<WEAPON> s_WeaponList;
		static glm::mat4 s_AnimatedCameraMatrix;


		static AnimatedEntity m_shotgunAnimatedEntity;
		static AnimatedEntity m_glockAnimatedEntiyty;
	};
}