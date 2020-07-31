#pragma once
#include "Header.h"
#include "Game.h"
#include "CoreImGui.h"
#include "ImGuizmo/ImGuizmo.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

namespace HellEngine
{
	enum class GizmoControlScheme { MOVE_RESIZE_SCALE, MOVE_WITH_RIGHT_CLICK_TO_ROTATE };

	class LevelEditor
	{
	public: // functions
		static void Update(Game* game);
		static void DrawOverlay(Shader* shader, Game* game);
		//static void DrawObject(Shader* shader, Game* game, PhysicsObjectType type, unsigned int parentIndex);
		static void RenderGizmo(ImGuiIO* io, Game* game);
		static void SetTranslationFromGizmo(float* fptr, glm::vec3& sourceTranslation);
		static void SetScaleFromGizmo(float* fptr, glm::vec3& sourceScale);
		static void SetRotationFromGizmo(float deltaptr[], glm::vec3& sourceRotation); 
		static void RoateSelectedObject90Degrees(Game* game);
		static void SetControlSceheme(GizmoControlScheme GizmoControlScheme);
		static bool IsVisisble();
		static void SetHighlightColorIfSelected(Shader* shader, void* object);

		//static void SetGizmoMatrix(glm::mat4& matrix);
		static void DisableSnapToGrid();
		static void EnableSnapToGrid(float snapAmount);

	public: // variables
		static void* p_selectedObject;
		static void* p_hoveredObject;

		//static int s_SelectedOjectIndex;
		//static void* s_selectedObject;
		static PhysicsObjectType s_SelectedObjectType; 
		static RaycastResult s_mouse_ray;
		static GizmoState s_gizmoState;


		static bool s_snapToGrid;
		static float s_snapAmount[3];
		static float* s_fptr_GizmoMatrix;;
		static GizmoControlScheme s_GizmoControlScheme;
	};
}