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

		//m_doors.reserve(30);
	}

	House::~House()
	{
	}

	void House::Draw(Shader* shader, bool envMapPass)
	{
		GpuProfiler g("House");
		for (Window& window : m_windows) {
			static int material0 = AssetManager::GetMaterialIDByName("Window");
			static int material1 = AssetManager::GetMaterialIDByName("WindowExterior");
			AssetManager::BindMaterial_0(material0);
			AssetManager::BindMaterial_1(material1);
			window.DrawFrameAndSashes(shader);
		//	window.DrawGlass(shader);
		}

		// Draw glass
		static Model* model = AssetManager::GetModelByName("Window");
		//	glDisable(GL_CULL_FACE);
		Transform trans = m_windows[0].m_transform;
		trans.position.x += 1;
		model->DrawMesh(shader, 2, trans.to_mat4());
		model->DrawMesh(shader, 4, trans.to_mat4());
		//	glEnable(GL_CULL_FACE);

		for (Room& room : m_rooms)
			room.Draw(shader);

		for (Door& door : m_doors)
			door.Draw(shader);

		for (Staircase& staircase : m_staircases)
			staircase.Draw(shader);

		for (Entity& entity : m_entities)
			entity.DrawEntity(shader);



		// RENDER LIGHTS
		if (Renderer::s_RenderSettings.DrawLightBulbs) {

			static unsigned int light_model_01 = AssetManager::GetModelIDByName("Light_01");
			static unsigned int light_model_02 = AssetManager::GetModelIDByName("Light_02");
			static unsigned int light_model_03 = AssetManager::GetModelIDByName("Light_03");
			static unsigned int light_model_04 = AssetManager::GetModelIDByName("Light_04");
			static unsigned int emmissiveTexture = AssetManager::GetTexIDByName("Light_E");

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, emmissiveTexture);

			AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("Light")); 
			for (Light& light : m_lights)
			{
				shader->setBool("hasEmissive", true);
				shader->setVec3("emissiveColor", light.m_color);
				Transform lightPosition(light.m_position);

				unsigned int modelID = 0;
				if (light.m_modelType == 0)
					modelID = light_model_01;

				if (light.m_modelType == 1)
					modelID = light_model_02;

				if (light.m_modelType == 2)
					modelID = light_model_03;

				if (light.m_modelType == 3)
					modelID = light_model_04;

				AssetManager::models[modelID].DrawMesh(shader, 0, lightPosition.to_mat4() * light.m_modelTransform.to_mat4());
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

	void House::AddWindow(glm::vec3 position, Axis axis)
	{
		m_windows.push_back(Window(position, axis));
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
		for (Room& room : m_rooms) {
			room.Rebuild(); 
			room.FindDoors(m_doors, m_staircases, m_windows);
			room.BuildWallMesh();
			room.m_wallMesh.BufferMeshToGL();
			room.CalculateWorldSpaceBounds();

			// Fix broken pointer
			EntityData* data = (EntityData*)room.m_floor.m_collisionObject->getUserPointer();
			data->ptr = &room.m_floor;
			room.m_floor.m_parent = &room;
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
			//light.m_roomID = 0;
			// To stop a crash during a test where I disabled all rooms but room 0.
			//if (light.m_roomID > m_rooms.size())
			//	continue;


			/// THIS IS DEFINITELY NOT HOW YOU WANT TO BE DOING THIS LONG TERM. BUT FOR NOW...
			float bias = 0.02f;


			Room* room = &m_rooms[light.m_roomID];
			light.m_lightVolume.BuildFromRoom(room);

			light.m_doorWayLightVolumes.clear();
			for (HoleInWall& doorWay : room->m_doorWaysXBackWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperZ + bias));
		
			for (HoleInWall& doorWay : room->m_doorWaysXFrontWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerZ - bias));

			for (HoleInWall& doorWay : room->m_doorWaysZLeftWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerX - bias));
			
			for (HoleInWall& doorWay : room->m_doorWaysZRightWall)
				light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperX + bias));
		}

		//Light* light = &m_lights[0];
	//	Door* door = &m_doors[0];
	//	LightVolumeDoorWay test(door, light->m_position, light->m_radius);
	//	light->m_doorWayLightVolumes.push_back(test);
	}
	
	void House::DeleteDoor(Door* door)
	{
		for (int i = 0; i < m_doors.size(); i++)
		{
			if (door == &m_doors[i]) {
				m_doors[i].RemoveCollisionObject();
				m_doors.erase(m_doors.begin() + i);
				return;
			}
		}
	}	
	
	void House::DeleteWindow(Window* window)
	{
		for (int i = 0; i < m_windows.size(); i++)
		{
			if (window == &m_windows[i]) {
				m_windows[i].RemoveCollisionObject();
				m_windows.erase(m_windows.begin() + i);
				return;
			}
		}
	}
}