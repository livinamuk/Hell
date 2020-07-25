#pragma once
#include "Header.h"
#include "Game.h"

namespace HellEngine
{
	class LevelEditor
	{
	public: // functions
		static void Draw(Shader* shader, Game* game);

	public: // variables
		static unsigned int s_hover_index;
		static PhysicsObjectType s_hover_object_type;
	};
}