#pragma once
#include "Header.h"
#include "imgui.h"
#include "ImGuizmo/AnimationCurve.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include "Game.h"

namespace HellEngine
{
	class CoreImGui
	{
	public: // functions
		static void InitImGui();
		static void Render(Game* game);

		static void ShowRagdollMenu(Game* game);
		static void ShowMapMenu(Game* game);
		static void ShowOtherMenu(Game* game);
		static void ShowPlayerMenu(Game* game);
		static void ShowShaderMenu(Game* game);
		static void ShowSceneMenu(Game* game);

	public: // static variables
	};
}