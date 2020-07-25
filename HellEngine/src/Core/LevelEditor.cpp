#include "hellpch.h"
#include "LevelEditor.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	void LevelEditor::Draw(Shader* shader, Game* game)
	{
		// Find what object the mouse is hovering on
		RaycastResult raycastResult = RaycastResult::CastMouseRay(&game->camera);

		// Is it a door?
		if (raycastResult.m_objectType == PhysicsObjectType::DOOR)
		{
			Door* door = &game->house.m_doors[raycastResult.m_elementIndex];
			glm::mat4 worldMatrix = door->GetDoorModelMatrixFromPhysicsEngine();

			glm::vec3 boxScaling = Util::btVec3_to_glmVec3(door->m_rigidBody->getCollisionShape()->getLocalScaling());
			Transform boxTransform = Transform(glm::vec3(0, DOOR_HEIGHT / 2, 0), glm::vec3(0), boxScaling);

			Cube::Draw(shader, worldMatrix * boxTransform.to_mat4());
		}

		/*
		for (Door& door : game->house.m_doors)
		{
			glm::mat4 worldMatrix = door.GetDoorModelMatrixFromPhysicsEngine();

			glm::vec3 boxScaling = Util::btVec3_to_glmVec3(door.m_rigidBody->getCollisionShape()->getLocalScaling());
			Transform boxTransform = Transform(glm::vec3(0, DOOR_HEIGHT / 2, 0), glm::vec3(0), boxScaling);

			Cube::Draw(shader, worldMatrix * boxTransform.to_mat4());
		}*/

	

	}
}