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
#include "Renderer/SkinnedMesh.h"
#include "Renderer/AssimpModel.h"
#include "Renderer/Effects/Decal.h"
#include "Misc/Shell.h"

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
		double currentFrame, lastFrame;
		Camera camera;
		House house;
		Transform m_shotgunTransform;
		RaycastResult m_cameraRaycast;
		Transform m_doorTrans;
		Player m_player;
		Entity m_HUDshotgun;
		Transform testTrans; 

		SkinnedMesh m_skinnedMesh;
		SkinnedMesh m_srinivasMesh;
		SkinnedMesh m_ZombieBoyMesh;

		std::vector<glm::mat4> m_animatedTransforms;
		std::vector<glm::mat4> m_ZombieBoyAnimatedTransforms;
		std::vector<glm::mat4> m_srinivasdAnimatedTransforms; 

		std::vector<Decal> m_decals;
		std::vector<Shell> m_shells;

		static bool s_dontLoadShotgun;

	};
}