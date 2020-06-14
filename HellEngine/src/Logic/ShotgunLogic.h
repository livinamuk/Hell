#pragma once
#include "Header.h"
#include "House/Room.h"
#include "Core/Camera.h"
#include "Core/PLayer.h"
#include "Core/AnimatedEntity.h"
#include "Physics/RaycastResult.h"

namespace HellEngine
{
	class ShotgunLogic
	{
	public: // static functions
		
		static void Update(float deltatime);
		static void Animate(float deltatime);

		static void FireShotgun();
		static void ReloadShotgun();
		static void BeginIronSighting();
		static void EndIronSighting();
		static void SpawnShellIfRequired();
		static glm::vec3 GetShotgunBarrelHoleWorldPosition();

	public:	// static variables
		
		static GunState m_gunState;
		static ReloadState m_reloadState;
		static IronSightState m_ironSightState;

		static int m_AmmoInGun;
		static int m_AmmoAvaliable;

		static RaycastResult m_raycast;
		static Camera* p_camera;
		static Player* p_player;
		static AnimatedEntity* p_model;
		static bool s_awaitingShell;
	};
}