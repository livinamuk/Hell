#include "hellpch.h"
#include "House.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Renderer/Renderer.h"

namespace HellEngine
{
	House* House::p_house;

	House::House()
	{
		p_house = this;
	}

	House::~House()
	{
	}

	void House::Draw(Shader* shader, bool envMapPass)
	{
		GpuProfiler g("House");
		for (Window& window : m_windows) {
			static int floorMaterialID = AssetManager::GetMaterialIDByName("FloorBoards");
			AssetManager::BindMaterial(floorMaterialID);
			window.Draw(shader); 
		}

		for (Room& room : m_rooms)
			room.Draw(shader);

		for (Door& door : m_doors)
			door.Draw(shader);

		for (Staircase& staircase : m_staircases)
			staircase.Draw(shader);

		for (Entity& entity : m_entities)
			entity.DrawEntity(shader);

		if (Renderer::s_RenderSettings.DrawLightBulbs) {

			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Light")); 
			for (Light& light : m_lights)
			{
				shader->setBool("hasEmissive", true);
				shader->setVec3("emissiveColor", light.m_color);
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

	void House::AddWindow(float x, float z, int story, float height, Axis axis)
	{
		m_windows.push_back(Window(x, z, story, height, axis));
	}

/*	void House::BuildWallMeshes()
	{
		for (Room& room : m_rooms) {
			room.FindDoors(m_doors, m_staircases);
			room.BuildWallMesh();
		}			
	}*/

	void House::RebuildAll()
	{
		// Clear the room vectors, and reconstruc the floors/ceilings etc.
		for (Room& room : m_rooms) {
			room.Rebuild(); 
			room.FindDoors(m_doors, m_staircases, m_windows);
			room.BuildWallMesh();
			room.m_wallMesh.BufferMeshToGL();
			room.CalculateWorldSpaceBounds();
			//room.CalculateLightVolume();
		}

		DetermineWhichLightIsInWhichRoom();
		BuildLightVolumes();
	}

	void House::DetermineWhichLightIsInWhichRoom() // note this does not consider rooms above rooms yet
	{
		// Reset IDs
		for (Light& light : m_lights)
			light.m_roomID = -1;

		// Itereate through each room and check which lights are in.
		// Store the ID of the room in that light, to be used for rendering light volumes in the stencil optimsiation pass.
		for (int i = 0; i < m_rooms.size(); i++)
		{
			for (Light& light : m_lights)
			{		
				if (light.m_position.x > m_rooms[i].m_lowerX)
					if (light.m_position.x < m_rooms[i].m_upperX)
						if (light.m_position.z > m_rooms[i].m_lowerZ)
							if (light.m_position.z < m_rooms[i].m_upperZ)
								light.m_roomID = i;
			}
		}
	}

	void House::BuildLightVolumes()
	{
		for (Light& light : m_lights)
		{
			Room* room = &m_rooms[light.m_roomID];
			light.m_lightVolume.BuildFromRoom(room);

			for (DoorWay& doorWay : room->m_doorWaysXBackWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperZ));
		
			for (DoorWay& doorWay : room->m_doorWaysXFrontWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerZ));

			for (DoorWay& doorWay : room->m_doorWaysZLeftWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerX));
			
			for (DoorWay& doorWay : room->m_doorWaysZRightWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperX));
		}

		//Light* light = &m_lights[0];
	//	Door* door = &m_doors[0];
	//	LightVolumeDoorWay test(door, light->m_position, light->m_radius);
	//	light->m_doorWayLightVolumes.push_back(test);
	}
}