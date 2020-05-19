#include "hellpch.h"
#include "House.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Renderer/Renderer.h"

namespace HellEngine
{
	House::House()
	{
	}

	House::~House()
	{
	}

	void House::Draw(Shader* shader, bool envMapPass)
	{
		for (Room& room : m_rooms)
			room.Draw(shader);

		for (Door& door : m_doors)
			door.Draw(shader);

		for (Staircase& staircase : m_staircases)
			staircase.Draw(shader);

		if (Renderer::s_RenderSettings.DrawLightBulbs) {
			for (Light& light : m_lights)
			{
				shader->setBool("hasEmissive", true);
				shader->setVec3("emissiveColor", light.m_color);
				AssetManager::SetModelMaterialIDByModelID(AssetManager::GetModelIDByName("Light"), AssetManager::GetMaterialIDByName("Light"));
				Transform lightPosition(light.m_position);
				AssetManager::GetModelByName("Light")->DrawMesh(shader, 0, lightPosition.to_mat4() * light.m_modelTransform.to_mat4());
			}
			shader->setBool("hasEmissive", false);
		}
	}

	void House::AddRoom(Room room)
	{
		m_rooms.emplace_back(room);
	}

	void House::AddDoor(Door door)
	{
		m_doors.emplace_back(door);
	}

	void House::AddLight(Light light)
	{
		m_lights.emplace_back(light);
	}

	void House::AddStaircase(Staircase staircase)
	{
		m_staircases.emplace_back(staircase);
	}

	void House::BuildWallMeshes()
	{
		for (Room& room : m_rooms) {
			room.FindDoors(m_doors, m_staircases);
			room.BuildWallMesh();
		}			
	}

	void House::RebuildAll()
	{
		// Clear the room vectors, and reconstruc the floors/ceilings etc.
		for (Room& room : m_rooms) {
			room.Rebuild(); 
			room.FindDoors(m_doors, m_staircases);
			room.BuildWallMesh();
			room.m_wallMesh.BufferMeshToGL();
		}
	}
}