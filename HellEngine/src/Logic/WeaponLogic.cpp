#include "hellpch.h"
#include "WeaponLogic.h"
#include "ShotgunLogic.h"
#include "GlockLogic.h"
#include "GL/GpuProfiling.h"
#include "Audio/Audio.h"

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
	std::vector<GunshotReport> WeaponLogic::s_BulletHitsThisFrame;
	Transform WeaponLogic::s_weaponTransform; 
	bool WeaponLogic::m_singleHanded = false;
	//int WeaponLogic::s_numberOfBulletsThatHitEnemiesThisFrame = 0;
	//int WeaponLogic::s_numberOfFollowThroughBulletsThatHitEnemiesThisFrame = 0;
	
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
		GlockLogic::p_model = &m_glockAnimatedEntiyty;
	}
	
	void WeaponLogic::Update(float deltaTime)
	{
		// This is used to stop spawning 12 splatters when you shoot a guy. Capped it at 4 or something...
		//s_numberOfBulletsThatHitEnemiesThisFrame = 0;
		//s_numberOfFollowThroughBulletsThatHitEnemiesThisFrame = 0;

		// Don't do shit if the level editor is open
		if (CoreImGui::s_Show)
			return;

		// not sure how but this seems to be running twice. LOOK INTO IT!!!


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


		// CREATE VOLUMETRIC BLOOD SPLATTER
		int numberOfBulletsThatHitEnemiesThisFrame = 0;
		Camera* camera = GlockLogic::p_camera;

		for (GunshotReport& gunShotReport : s_BulletHitsThisFrame)
		{
			if (numberOfBulletsThatHitEnemiesThisFrame < 5) {
				glm::vec3 position = gunShotReport.hitLocation;
				glm::vec3 rotation = camera->m_transform.rotation;
				Game::s_volumetricBloodSplatters.push_back(VolumetricBloodSplatter(position, rotation, camera->m_Front * glm::vec3(-1))); 
				numberOfBulletsThatHitEnemiesThisFrame++;
			}
		}

		// Follow through blood splatters
		int numberOfFollowThroughBulletsThatHitEnemiesThisFrame = 0;

		for (GunshotReport& gunShotReport : s_BulletHitsThisFrame)
		{
			if (numberOfFollowThroughBulletsThatHitEnemiesThisFrame < 5) {
				if (gunShotReport.hitType == PhysicsObjectType::RAGDOLL)
				{
					RaycastResult followThroughShotResult;
					followThroughShotResult.CastRay(gunShotReport.hitLocation + (gunShotReport.rayDirection * glm::vec3(0.2f)), gunShotReport.rayDirection, 10.0f);

					glm::vec3 position = followThroughShotResult.m_hitPoint;
					glm::vec3 rotation = camera->m_transform.rotation;
					Game::s_volumetricBloodSplatters.push_back(VolumetricBloodSplatter(position, rotation, camera->m_Front * glm::vec3(-1), true));
					numberOfFollowThroughBulletsThatHitEnemiesThisFrame++;
				}
			}
		}





		// clear any bullet hits from last frame
		s_BulletHitsThisFrame.clear();
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

		s_weaponTransform.position = camera->m_viewPos;
		s_weaponTransform.rotation = camera->m_transform.rotation;
		s_weaponTransform.scale = glm::vec3(0.002f);

		glm::mat4 weaponModelMatrix = s_weaponTransform.to_mat4() * camera->m_weaponSwayTransform.to_mat4();

		glDisable(GL_CULL_FACE);

		if (s_SelectedWeapon == WEAPON::GLOCK)
			m_glockAnimatedEntiyty.Draw(shader, weaponModelMatrix);

		if (s_SelectedWeapon == WEAPON::SHOTGUN)
			m_shotgunAnimatedEntity.Draw(shader, weaponModelMatrix);

		glEnable(GL_CULL_FACE);

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

		if (s_SelectedWeapon == WEAPON::GLOCK) {
			Audio::PlayAudio("WPNFLY_Glock_MagIn_Full.wav");
			GlockLogic::m_gunState = GunState::EQUIP;			
		}

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


	GunshotReport WeaponLogic::FireBullet(Camera* camera, float variance, float force)
	{
		glm::vec3 rayOrigin = camera->m_viewPos;
		glm::vec3 rayDirection = camera->m_Front;

		// Variance
		float offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.x += offset;
		offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.y += offset;
		offset = (variance * 0.5f) - Util::RandomFloat(0, variance);
		rayDirection.z += offset;
		rayDirection = glm::normalize(rayDirection);

		RaycastResult raycastResult;
		raycastResult.CastRay(rayOrigin, rayDirection, 10.0f);

		// make bullet hole if it aint the ragdoll
		if (raycastResult.m_objectType != PhysicsObjectType::RAGDOLL)
		{
			// Glass
			if (raycastResult.m_objectType == PhysicsObjectType::GLASS) {
				Decal::s_decals.push_back(Decal(raycastResult.m_hitPoint, raycastResult.m_surfaceNormal, DecalType::GLASS));
			}
			// any other surface
			else
				Decal::s_decals.push_back(Decal(raycastResult.m_hitPoint, raycastResult.m_surfaceNormal, DecalType::PLASTER));
		}


		// Apply force to Ragdoll 
		if (raycastResult.m_objectType == PhysicsObjectType::RAGDOLL)
		{
			float FORCE_SCALING_FACTOR = force;// 5;// Config::TEST_FLOAT;
			raycastResult.m_rigidBody->activate(true);
			btVector3 centerOfMass = raycastResult.m_rigidBody->getCenterOfMassPosition();
			btVector3 hitPoint = Util::glmVec3_to_btVec3(raycastResult.m_hitPoint);
			btVector3 force = Util::glmVec3_to_btVec3(camera->m_Front) * FORCE_SCALING_FACTOR;
			raycastResult.m_rigidBody->applyImpulse(force, hitPoint - centerOfMass);

		}

		// Ragdoll and couch bleeds
	//	if (raycastResult.m_objectType == PhysicsObjectType::RAGDOLL || raycastResult.m_objectType == PhysicsObjectType::MISC_MESH)
		{

			// CREATE VOLUMETRIC BLOOD SPLATTER
			
			//	glm::vec3 position = raycastResult.m_hitPoint;
			//	glm::vec3 rotation = camera->m_transform.rotation;
		//		Game::s_volumetricBloodSplatters.push_back(VolumetricBloodSplatter(position, rotation, camera->m_Front * glm::vec3(-1)));
	
			// If an enemy was hit, make another splatter,decal only, on the wall behind it
			/*if (s_numberOfFollowThroughBulletsThatHitEnemiesThisFrame < 5) {
				if (raycastResult.m_objectType == PhysicsObjectType::RAGDOLL)
				{
					RaycastResult followThroughShotResult;
					followThroughShotResult.CastRay(raycastResult.m_hitPoint + (rayDirection * glm::vec3(0.2f)), rayDirection, 10.0f);

					glm::vec3 position = followThroughShotResult.m_hitPoint;
					glm::vec3 rotation = camera->m_transform.rotation;
					Game::s_volumetricBloodSplatters.push_back(VolumetricBloodSplatter(position, rotation, camera->m_Front * glm::vec3(-1), true));
					s_numberOfFollowThroughBulletsThatHitEnemiesThisFrame++;
				}
			}*/
	}

		GunshotReport gunshotReport;
		gunshotReport.hitType = raycastResult.m_objectType;
		gunshotReport.hitLocation = raycastResult.m_hitPoint;
		gunshotReport.rayDirection = rayDirection;

		WeaponLogic::s_BulletHitsThisFrame.emplace_back(gunshotReport);

		return gunshotReport;
	}
}