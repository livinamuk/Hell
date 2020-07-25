#pragma once
#include "Header.h"
#include "Game.h"

namespace HellEngine
{
	class LevelEditor
	{
	public: // functions
		static void DrawOverlay(Shader* shader, Game* game);
		static void DrawObject(Shader* shader, Game* game, PhysicsObjectType type, unsigned int parentIndex);
		
	public: // variables
		static int s_SelectedOjectIndex;
		static PhysicsObjectType s_SelectedObjectType; 
		static RaycastResult s_mouse_ray;
	};
}