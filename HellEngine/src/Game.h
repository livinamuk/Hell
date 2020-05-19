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
		Entity m_TestEntity;

		Transform testTrans; 


		AssimpModel m_assimpModel;
		SkinnedMesh m_skinnedMesh;
		SkinnedMesh m_skinnedAnimation;
	};
}