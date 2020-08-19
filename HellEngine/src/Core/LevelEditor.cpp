#include "hellpch.h"
#include "LevelEditor.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Core/GameData.h"

namespace HellEngine
{
	//int LevelEditor::s_SelectedOjectIndex = -1;
	//void* LevelEditor::s_selectedObject = nullptr;
	PhysicsObjectType LevelEditor::s_SelectedObjectType;
	RaycastResult LevelEditor::s_mouse_ray;
	GizmoState LevelEditor::s_gizmoState;		
	bool LevelEditor::s_snapToGrid;
	float LevelEditor::s_snapAmount[3];
	float* LevelEditor::s_fptr_GizmoMatrix;
	GizmoControlScheme LevelEditor::s_GizmoControlScheme;
	void* LevelEditor::p_selectedObject;
	void* LevelEditor::p_hoveredObject;
	
	void LevelEditor::Update(Game* game)
	{
		/*for (Room& room : GameData::p_house->m_rooms)
		{
			//EntityData* data = (EntityData*)room.m_floor.m_collisionObject->getUserPointer();
			//data->ptr = &room.m_floor;

			room.m_floor.m_parent = &room;
		}*/

		// Find what object the mouse is hovering on
		s_mouse_ray = RaycastResult::CastMouseRay(&game->camera);

		if (Input::s_keyPressed[HELL_KEY_DELETE])
			DeleteSelectedObject(game);

		// Right click
		if (Input::s_rightMousePressed)
		{
			if (s_GizmoControlScheme == GizmoControlScheme::MOVE_RESIZE_SCALE)
				s_gizmoState = (GizmoState)(((int)s_gizmoState + 1) % 3);

			else if (s_GizmoControlScheme == GizmoControlScheme::MOVE_WITH_RIGHT_CLICK_TO_ROTATE)
				RoateSelectedObject90Degrees(game);
		}
		
		// If you aint hovering the gizmo, then set the selected object to whatever you clicked.
		if (Input::s_leftMousePressed && !ImGuizmo::IsOver()) {
			s_SelectedObjectType = s_mouse_ray.m_objectType;
			p_selectedObject = s_mouse_ray.m_objectPtr;
			//s_SelectedOjectIndex = s_mouse_ray.m_elementIndex;
		}
		//else
		//	p_selectedObject = nullptr;

		// Get pointer to selected object
		/*if (s_SelectedObjectType == PhysicsObjectType::DOOR)
			p_selectedObject = &GameData::p_house->m_doors[s_SelectedOjectIndex];
		else if (s_SelectedObjectType == PhysicsObjectType::WINDOW)
			p_selectedObject = &GameData::p_house->m_windows[s_SelectedOjectIndex];
		else if (s_SelectedObjectType == PhysicsObjectType::MISC_MESH)
			p_selectedObject = &GameData::p_house->m_entities[s_SelectedOjectIndex];
		else if (s_SelectedObjectType == PhysicsObjectType::FLOOR)
			p_selectedObject = &GameData::p_house->m_rooms[s_SelectedOjectIndex].m_floor;
		else*/
		//	p_selectedObject = nullptr;

		// Get pointer to hovered object
		if (s_mouse_ray.m_objectType == PhysicsObjectType::DOOR)
			p_hoveredObject = s_mouse_ray.m_objectPtr;// &GameData::p_house->m_doors[s_mouse_ray.m_elementIndex];
		else if (s_mouse_ray.m_objectType == PhysicsObjectType::WINDOW)
			p_hoveredObject = s_mouse_ray.m_objectPtr; //&GameData::p_house->m_windows[s_mouse_ray.m_elementIndex];
		else if (s_mouse_ray.m_objectType == PhysicsObjectType::MISC_MESH)
			p_hoveredObject = s_mouse_ray.m_objectPtr; // &GameData::p_house->m_entities[s_mouse_ray.m_elementIndex];
		else if (s_mouse_ray.m_objectType == PhysicsObjectType::FLOOR)
		{
			p_hoveredObject = s_mouse_ray.m_objectPtr; // &GameData::p_house->m_rooms[s_mouse_ray.m_elementIndex].m_floor;

			Floor* floor = (Floor*)p_hoveredObject;
			std::cout << floor->m_transform.position.x << ", ";
			std::cout << floor->m_transform.position.y << ", ";
			std::cout << floor->m_transform.position.z << "\n";
		}

		else
			p_hoveredObject = nullptr;

		// Update the light volumes cause you are surely fucking with them moving doors around n shit.
		//House::p_house->BuildLightVolumes();
		//House::p_house->RebuildAll();// Clear the room vectors, and reconstruc the floors/ceilings etc.
		
	}

	void LevelEditor::DeleteSelectedObject(Game* game)
	{
		if (s_SelectedObjectType == PhysicsObjectType::DOOR)
			GameData::p_house->DeleteDoor((Door*)p_selectedObject);
		else if (s_SelectedObjectType == PhysicsObjectType::WINDOW)
			GameData::p_house->DeleteWindow((Window*)p_selectedObject);
	}

	void LevelEditor::RenderGizmo(ImGuiIO* io, Game* game)
	{
		// Bail early if nothing valid is selected
		if (s_SelectedObjectType == PhysicsObjectType::UNDEFINED)
			return;

		///////////
		// Setup //

		ImGuizmo::BeginFrame();
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
		static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
		static bool boundSizing = false;
		static bool boundSizingSnap = false;
		float* s_fptr_GizmoMatrix;
		//unsigned int i = s_SelectedOjectIndex;

		// Doors
		if (s_SelectedObjectType == PhysicsObjectType::DOOR)
		{
			Door* door = (Door*)p_selectedObject;
			s_fptr_GizmoMatrix = (float*)glm::value_ptr(door->m_rootTransform.to_mat4());
			EnableSnapToGrid(0.1);
			SetControlSceheme(GizmoControlScheme::MOVE_WITH_RIGHT_CLICK_TO_ROTATE);
		}

		// Windows
		else if (s_SelectedObjectType == PhysicsObjectType::WINDOW)
		{
			Window* window = (Window*)p_selectedObject;
			s_fptr_GizmoMatrix = (float*)glm::value_ptr(window->m_transform.to_mat4());
			EnableSnapToGrid(0.1);
			SetControlSceheme(GizmoControlScheme::MOVE_WITH_RIGHT_CLICK_TO_ROTATE);
		}
		// Room?
		else if (s_SelectedObjectType == PhysicsObjectType::FLOOR)
		{

			Floor* floor = &GameData::p_house->m_rooms[0].m_floor;// 
			floor = (Floor*)p_selectedObject;

			std::cout << floor->m_transform.position.x << ", ";
			std::cout << floor->m_transform.position.y << ", ";
			std::cout << floor->m_transform.position.z << "\n";

			Room* room = (Room*)floor->m_parent;


			Transform worldTransform;
			worldTransform.position = glm::vec3(room->m_position.x, room->m_story * ROOM_HEIGHT, room->m_position.y);
			worldTransform.scale = glm::vec3(room->m_size.x, 0.025f, room->m_size.y);
			s_fptr_GizmoMatrix = (float*)glm::value_ptr(worldTransform.to_mat4());
			EnableSnapToGrid(0.1);
			SetControlSceheme(GizmoControlScheme::MOVE_WITH_RIGHT_CLICK_TO_ROTATE);
		}

		// Misc meshes
		else if (s_SelectedObjectType == PhysicsObjectType::MISC_MESH)
		{
			Entity* entity = (Entity*)p_selectedObject;
			s_fptr_GizmoMatrix = (float*)glm::value_ptr(entity->m_transform.to_mat4());
			DisableSnapToGrid();
			SetControlSceheme(GizmoControlScheme::MOVE_RESIZE_SCALE);
		}

		// Nothing else is implemented yet
		else
			return;


		///////////////////////
		// Manipulate Matrix //

		float deltaptr[16];
		//manipulate gizmo function (returns data in fptr matrix)
		ImGuizmo::SetRect(0, 0, io->DisplaySize.x, io->DisplaySize.y);
		ImGuizmo::Manipulate(
			glm::value_ptr(game->camera.m_viewMatrix),
			glm::value_ptr(game->camera.m_projectionMatrix),
			//mCurrentGizmoOperation,
			(ImGuizmo::OPERATION)s_gizmoState,
			mCurrentGizmoMode,
			s_fptr_GizmoMatrix,
			deltaptr, s_snapToGrid ? &s_snapAmount[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL
		);


		///////////////////////
		// Update Transform  //

		// Doors
		if (s_SelectedObjectType == PhysicsObjectType::DOOR)
		{
			Door* door = (Door*)p_selectedObject;
			SetTranslationFromGizmo(s_fptr_GizmoMatrix, door->m_rootTransform.position);

		//	House::p_house = &game->house;

			door->Reconfigure();
			//game->RebuildMap();
		}
		// Windows
		if (s_SelectedObjectType == PhysicsObjectType::WINDOW)
		{
			Window* window = (Window*)p_selectedObject;
			SetTranslationFromGizmo(s_fptr_GizmoMatrix, window->m_transform.position);
			Physics::SetCollisionObjectWorldTranslation(window->m_collisionObject, window->m_transform.position, glm::vec3(0, WINDOW_HEIGHT_SINGLE * 0.5f, 0));
			Physics::SetCollisionObjectWorldRotation(window->m_collisionObject, window->m_transform.rotation);
		}

		// Misc Mesh
		if (s_SelectedObjectType == PhysicsObjectType::MISC_MESH)
		{
			Entity* entity = (Entity*)p_selectedObject;
			SetTranslationFromGizmo(s_fptr_GizmoMatrix, entity->m_transform.position);
			SetScaleFromGizmo(s_fptr_GizmoMatrix, entity->m_transform.scale);
			SetRotationFromGizmo(deltaptr, entity->m_transform.rotation);
			Physics::SetCollisionObjectWorldTransform(entity->m_collisionObject, entity->m_transform);
		}

		// If the matrices are different, recalculate light volumes and wall meshes etc.
		for (int i = 0; i < 16; i++) {
			if (s_fptr_GizmoMatrix[i] != deltaptr[i]) { 
				GameData::p_house->RebuildAll();
				return;
			}		
		}
	}

	void LevelEditor::RoateSelectedObject90Degrees(Game* game)
	{
		// Doors
		if (s_SelectedObjectType == PhysicsObjectType::DOOR) {
			Door* door = (Door*)p_selectedObject;
			Util::RotateAxisBy90(door->m_axis);
			
		}
		// Windows
		if (s_SelectedObjectType == PhysicsObjectType::WINDOW) {
			Window* window = (Window*)p_selectedObject;
			Util::RotateAxisBy90(window->m_axis);
			window->Reconfigure();
		}
	}

	void LevelEditor::SetTranslationFromGizmo(float* fptr, glm::vec3& sourceTranslation)
	{
		Transform transform;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(fptr, matrixTranslation, matrixRotation, matrixScale);
		sourceTranslation = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
	}

	void LevelEditor::SetScaleFromGizmo(float* fptr, glm::vec3& sourceScale)
	{
		Transform transform;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(fptr, matrixTranslation, matrixRotation, matrixScale);
		sourceScale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
	}

	void LevelEditor::SetRotationFromGizmo(float deltaptr[], glm::vec3& sourceRotation)
	{
		Transform transform;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(deltaptr, matrixTranslation, matrixRotation, matrixScale);
		sourceRotation += glm::vec3(matrixRotation[0] / 180.0, matrixRotation[1] / 180.0, matrixRotation[2] / 180.0);
	}


	void LevelEditor::SetControlSceheme(GizmoControlScheme GizmoControlScheme)
	{
		s_GizmoControlScheme = GizmoControlScheme;

		if (s_GizmoControlScheme == GizmoControlScheme::MOVE_WITH_RIGHT_CLICK_TO_ROTATE)
			s_gizmoState = GizmoState::MOVE;
	}

	bool LevelEditor::IsVisisble()
	{
		return CoreImGui::s_Show;
	}

	void LevelEditor::SetHighlightColorIfSelected(Shader* shader, void* object)
	{
		// If editor is closed bail.
		if (!IsVisisble())
			return;

		if (LevelEditor::p_selectedObject == object)
			shader->setVec3("ColorAdd", glm::vec3(1, 0, 1));

		else if (LevelEditor::p_hoveredObject == object)
			shader->setVec3("ColorAdd", glm::vec3(0.9f));
	}

	
	//void LevelEditor::SetGizmoMatrix(glm::mat4 matrix)
//	{
	//	s_gizmoMatrix
	//}

	void LevelEditor::DisableSnapToGrid()
	{
		s_snapToGrid = false;
		s_snapAmount[0] = 0;
		s_snapAmount[1] = 0;
		s_snapAmount[2] = 0;
	}

	void LevelEditor::EnableSnapToGrid(float snapAmount)
	{
		s_snapToGrid = true;
		s_snapAmount[0] = snapAmount;
		s_snapAmount[1] = snapAmount;
		s_snapAmount[2] = snapAmount;
	}
}

/*
// Doors
if (s_SelectedObjectType == PhysicsObjectType::DOOR)
{
	Door* door = &GameData::p_house->m_doors[s_SelectedOjectIndex];
	SetTranslationFromGizmo(s_gizmoMatrix, door->m_rootTransform.position);
}

// Misc Mesh
if (s_SelectedObjectType == PhysicsObjectType::MISC_MESH)
{
	Entity* entity = &GameData::p_house->m_entities[s_SelectedOjectIndex];
	SetTranslationFromGizmo(s_gizmoMatrix, entity->m_transform.position);
	SetScaleFromGizmo(s_gizmoMatrix, entity->m_transform.scale);
	SetRotationFromGizmo(deltaptr, entity->m_transform.rotation);
	Physics::SetCollisionObjectWorldTransform(entity->m_collisionObject, entity->m_transform);
}
	}





	void LevelEditor::SetGizmoMatrix(glm::mat4& matrix)
	{
		s_gizmoMatrix = (float*)glm::value_ptr(matrix);
	}
*/