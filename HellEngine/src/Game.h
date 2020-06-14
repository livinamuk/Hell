#pragma once
#include "Header.h"
#include "Core/Camera.h"
#include "Core/Player.h"
#include "Core/Entity.h"
#include "Physics/Physics.h"
#include "Physics/RaycastResult.h"
#include "House/House.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Cube.h"
#include "Renderer/Renderer.h"
#include "Renderer/SkinnedModel.h"
#include "Renderer/AssimpModel.h"
#include "Renderer/Effects/Decal.h"
#include "Misc/Shell.h"
#include "Core/AnimatedEntity.h"

namespace HellEngine
{
	class Game
	{
	public:	// Methods
		Game();
		void OnLoad();
		void OnUpdate();
		void OnRender();
		void OnInteract();
;
		void RebuildMap();

	public: // Fields
		double currentTime, lastFrame;
		Camera camera;
		House house;
		//Transform m_shotgunTransform;
		RaycastResult m_cameraRaycast;
		Transform m_doorTrans;
		Player m_player;
		Entity m_HUDshotgun;

		Transform testTrans; 

		SkinnedModel m_skinnedMesh;
		//SkinnedModel m_skinnedShotgunMesh;
		SkinnedModel m_ZombieBoyMesh;

		AnimatedEntity m_shotgunAnimatedEntity;

	//	std::vector<glm::mat4> m_animatedTransforms;
	//	std::vector<glm::mat4> m_ZombieBoyAnimatedTransforms;
//		std::vector<glm::mat4> m_srinivasdAnimatedTransforms; 

		//std::vector<Decal> m_decals;
		float m_deltaTime;

		static bool s_dontLoadShotgun;


	};
}