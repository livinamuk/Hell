#include "hellpch.h"
#include "LevelEditor.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	int LevelEditor::s_SelectedOjectIndex = -1;
	PhysicsObjectType LevelEditor::s_SelectedObjectType;
	RaycastResult LevelEditor::s_mouse_ray;

	void LevelEditor::DrawOverlay(Shader* shader, Game* game)
	{
		// Find what object the mouse is hovering on
		s_mouse_ray = RaycastResult::CastMouseRay(&game->camera);

		// Yep you are gonna check if mouse clicks here cause fuck it.
		if (Input::s_leftMousePressed) {
			s_SelectedObjectType = s_mouse_ray.m_objectType;
			s_SelectedOjectIndex = s_mouse_ray.m_elementIndex;
		}

		// Draw hover object
		shader->setInt("bufferIndex", 0);
		shader->setVec3("color", glm::vec3(0.9f, 0.9, 0.9f));
		DrawObject(shader, game, s_mouse_ray.m_objectType, s_mouse_ray.m_elementIndex);

		// Draw hover object
		shader->setInt("bufferIndex", 0);
		shader->setVec3("color", glm::vec3(1, 0, 1));
		DrawObject(shader, game, s_SelectedObjectType, s_SelectedOjectIndex);
	}

	void LevelEditor::DrawObject(Shader* shader, Game* game, PhysicsObjectType objectType, unsigned int parentIndex)
	{
		// Is it a door?
		if (objectType == PhysicsObjectType::DOOR)
		{
			Door* door = &game->house.m_doors[parentIndex];
			glm::mat4 worldMatrix = door->GetDoorModelMatrixFromPhysicsEngine();

			glm::vec3 boxScaling = Util::btVec3_to_glmVec3(door->m_rigidBody->getCollisionShape()->getLocalScaling());
			Transform boxTransform = Transform(glm::vec3(0, DOOR_HEIGHT / 2, 0), glm::vec3(0), boxScaling);

			Cube::Draw(shader, worldMatrix * boxTransform.to_mat4());
		}
	}
}