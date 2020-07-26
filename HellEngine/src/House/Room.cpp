#include "hellpch.h"
#include "Room.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	Room::Room(glm::vec2 position, glm::vec2 size, int story, void* house)
	{
		m_position = position;
		m_size = size;
		m_story = story;
		p_house = house;
		Rebuild();
	}

	Room::~Room()
	{
	}

	void Room::Rebuild()
	{
		m_floor = Floor(glm::vec3(m_position.x, 0, m_position.y), m_size, m_story, false);
		m_ceiling = Ceiling(glm::vec3(m_position.x, 0, m_position.y), m_size, m_story, false);

		m_wallMesh.ClearMesh();

		m_doorWaysXFrontWall.clear();
		m_doorWaysXBackWall.clear();
		m_doorWaysZLeftWall.clear();
		m_doorWaysZRightWall.clear();

		m_floorTrimTransforms.clear();
		m_ceilingTrimTransforms.clear();
	}

	bool sameCoordinate(float a, float b)
	{
		float epsilon = 0.1f;
		return abs(a - b) < epsilon;
	}

	bool withinRange(float x, float lower, float upper)
	{
		if (x < lower)
			return false;
		if (x > upper)
			return false;
		else
			return true;
	}

	void Room::Draw(Shader* shader)
	{
		GpuProfiler g("Room");
		// Draw Floor
		AssetManager::BindMaterial(AssetManager::s_MaterialID_FloorBoards);
		m_floor.Draw(shader);

		AssetManager::BindMaterial(AssetManager::s_MaterialID_PlasterCeiling);
		m_ceiling.Draw(shader);

		// Walls
		AssetManager::BindMaterial(AssetManager::s_MaterialID_WallPaper);
		glm::mat4 identity = glm::mat4(1);
		shader->setMat4("model", identity);
		m_wallMesh.Draw(shader);	

		// Floor trims
		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Trims"));
		for (Transform transform : m_floorTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			AssetManager::DrawModel(AssetManager::s_ModelID_FloorTrim, shader, transform.to_mat4());
		}
		// Ceiling trims
		for (Transform transform : m_ceilingTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			AssetManager::DrawModel(AssetManager::s_ModelID_CeilingTrim, shader, transform.to_mat4());
		}
		// Reset to default
		shader->setVec2("TEXTURE_SCALE", glm::vec2(1.0, 1.0));
	}

	void Room::FindDoors(std::vector<Door>& doors, std::vector<Staircase>& staircases, std::vector<Window>& windows)
	{
		float low_x = std::min(m_position.x - (m_size.x / 2), m_position.x + (m_size.x / 2));
		float high_x = std::max(m_position.x - (m_size.x / 2), m_position.x + (m_size.x / 2));
		float low_z = std::min(m_position.y - (m_size.y / 2), m_position.y + (m_size.y / 2));
		float high_z = std::max(m_position.y - (m_size.y / 2), m_position.y + (m_size.y / 2));

		m_doorWaysXBackWall.clear();
		m_doorWaysXFrontWall.clear();
		m_doorWaysZLeftWall.clear();
		m_doorWaysZRightWall.clear();

		std::vector<DoorWay> potentialConnectedDoorWays;


		for (Window& window : windows) {
			DoorWay doorWay;
			doorWay.position = window.m_transform.position;
			doorWay.type = DoorWayType::WINDOW;
			doorWay.story = window.m_story;
			doorWay.axis = window.m_axis;
			doorWay.parent = &window;
			doorWay.bottom_Y = (window.m_story * ROOM_HEIGHT) + window.m_startHeight - 0.05F;;
			doorWay.top_Y = (window.m_story * ROOM_HEIGHT) + window.m_startHeight + WINDOW_HEIGHT_SINGLE;
			doorWay.width = WINDOW_WIDTH_SINGLE;
			potentialConnectedDoorWays.push_back(doorWay);
		}

		for (Staircase& staircase : staircases) {

			staircase.m_bottomDoorway.width = DOOR_WIDTH;
			staircase.m_topDoorway.width = DOOR_WIDTH;

			staircase.m_bottomDoorway.bottom_Y = (staircase.m_story * ROOM_HEIGHT);
			staircase.m_bottomDoorway.top_Y = (staircase.m_story * ROOM_HEIGHT) + DOOR_HEIGHT;

			staircase.m_topDoorway.bottom_Y = ((staircase.m_story + 1) * ROOM_HEIGHT);
			staircase.m_topDoorway.top_Y = ((staircase.m_story + 1) * ROOM_HEIGHT) + DOOR_HEIGHT;

			potentialConnectedDoorWays.push_back(staircase.m_bottomDoorway);
			potentialConnectedDoorWays.push_back(staircase.m_topDoorway);
		}

		for (Door& door : doors) {
			DoorWay doorWay;
			doorWay.position = door.m_rootTransform.position;
			doorWay.type = DoorWayType::DOOR;
			doorWay.story = door.m_story;
			doorWay.axis = door.m_axis;
			doorWay.parent = &door; 
			doorWay.bottom_Y = (door.m_story * ROOM_HEIGHT);
			doorWay.top_Y = (door.m_story * ROOM_HEIGHT) + DOOR_HEIGHT;
			doorWay.width = DOOR_WIDTH;
			potentialConnectedDoorWays.push_back(doorWay);
		}

		// Do the same for staircase openings
		for (DoorWay& doorWay : potentialConnectedDoorWays)
		{
			if (withinRange(doorWay.position.x, low_x, high_x)) {
				if (withinRange(doorWay.position.z, low_z - 0.2f, low_z + 0.2f))
					m_doorWaysXFrontWall.push_back(doorWay);
				else if (withinRange(doorWay.position.z, high_z - 0.2f, high_z + 0.2f))
					m_doorWaysXBackWall.push_back(doorWay);
			}

			if (withinRange(doorWay.position.z, low_z, high_z)) {
				if (withinRange(doorWay.position.x, low_x - 0.2f, low_x + 0.2f))
					m_doorWaysZLeftWall.push_back(doorWay);
				if (withinRange(doorWay.position.x, high_x - 0.2f, high_x + 0.2f))
					m_doorWaysZRightWall.push_back(doorWay);
			}
		}

		// Sort em
		std::sort(m_doorWaysXFrontWall.begin(), m_doorWaysXFrontWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.x < b.position.x; });
		std::sort(m_doorWaysXBackWall.begin(), m_doorWaysXBackWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.x > b.position.x; });
		std::sort(m_doorWaysZLeftWall.begin(), m_doorWaysZLeftWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.z > b.position.z; });
		std::sort(m_doorWaysZRightWall.begin(), m_doorWaysZRightWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.z < b.position.z; });
	}

	void Room::BuildWallMesh()
	{
		// Find door
		float low_x = m_position.x - (m_size.x / 2);
		float high_x = m_position.x + (m_size.x / 2);
		float low_z = m_position.y - (m_size.y / 2);
		float high_z = m_position.y + (m_size.y / 2);
		
		// DO NOT HARDCODE THIS. IT'S ALSO IN WALLMESH.CPP

		float wallWidth;
		float ROOM_Y = (m_story * STORY_HEIGHT);
		float bias = 0.01; // to hide cracks
		
		// Front walls		
		float cursorX = low_x;
		float cursorZ = low_z;
		for (int i = 0; i < m_doorWaysXFrontWall.size(); i++) 
		{
			wallWidth = m_doorWaysXFrontWall[i].position.x - cursorX - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX + wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // wall
			
			
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
			cursorX += wallWidth + DOOR_WIDTH;
			//if (m_doorWaysFrontWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX - DOOR_WIDTH, DOOR_HEIGHT + ROOM_Y, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // above door
				AddCeilingTrim(glm::vec3(cursorX - DOOR_WIDTH, ROOM_Y, cursorZ), 0, DOOR_WIDTH);
		//	}
		}
		wallWidth = high_x - cursorX;
		m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX + wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);

		// Back walls
		cursorX = high_x;
		cursorZ = high_z;
		for (int i = 0; i < m_doorWaysXBackWall.size(); i++)	{
			float wallWidth = cursorX - m_doorWaysXBackWall[i].position.x - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX - wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
			cursorX -= wallWidth + DOOR_WIDTH;
		//	if (m_doorWaysBackWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX + DOOR_WIDTH, DOOR_HEIGHT + ROOM_Y, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // above door
				AddCeilingTrim(glm::vec3(cursorX + DOOR_WIDTH, ROOM_Y, cursorZ), ROTATE_180, DOOR_WIDTH);
		//	}
		}
		wallWidth = cursorX - low_x;
		m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX - wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);

		// Left walls
		cursorZ = high_z;
		cursorX = low_x;
		for (int i = 0; i < m_doorWaysZLeftWall.size(); i++) {

			DoorWay* doorway = &m_doorWaysZLeftWall[i];

			float wallWidth = cursorZ - m_doorWaysZLeftWall[i].position.z - (doorway->width / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ - wallWidth), POS_Z); // wall
		

			
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
			cursorZ -= wallWidth + doorway->width;
			
			// Window lower wall part
			if (doorway->bottom_Y > 0) {
				m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y, cursorZ + doorway->width), glm::vec3(cursorX, ROOM_Y + doorway->bottom_Y + bias, cursorZ), POS_Z);
				AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ + doorway->width), ROTATE_90, doorway->width);
			}

			m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y + DOOR_HEIGHT, cursorZ + doorway->width), glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT + bias, cursorZ), POS_Z);

			//if (m_doorWaysLeftWall[i].type == DoorWayType::DOOR) {
				AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ + doorway->width), ROTATE_90, doorway->width);
			//}
		}
		wallWidth = cursorZ - low_z;
		m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ - wallWidth), POS_Z); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);

		// Right walls
		cursorZ = low_z;
		cursorX = high_x;
		for (int i = 0; i < m_doorWaysZRightWall.size(); i++)
		{
			float wallWidth = m_doorWaysZRightWall[i].position.z - cursorZ - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ + wallWidth), NEG_Z); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);
			cursorZ += wallWidth + DOOR_WIDTH;
			//if (m_doorWaysRightWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX, DOOR_HEIGHT + ROOM_Y, cursorZ - DOOR_WIDTH), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_Z); // above door
				AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ - DOOR_WIDTH), ROTATE_270, DOOR_WIDTH);
			//}
		}
		wallWidth = high_z - cursorZ;
		m_wallMesh.AddQuad(glm::vec3(cursorX, ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ + wallWidth), NEG_Z); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);

		m_wallMesh.BufferMeshToGL();
	}

	void Room::AddFloorTrim(glm::vec3 position, float rotation, float scale)
	{
		Transform trim;
		trim.position = position;
		trim.rotation.y = rotation;
		trim.scale.x = scale;
		m_floorTrimTransforms.push_back(trim);
	}

	void Room::AddCeilingTrim(glm::vec3 position, float rotation, float scale)
	{
		Transform trim;
		trim.position = position;
		trim.rotation.y = rotation;
		trim.scale.x = scale;
		m_ceilingTrimTransforms.push_back(trim);
	}

	void Room::CalculateWorldSpaceBounds()
	{
		float bias = 0.02;
		m_lowerX = m_position.x - (m_size.x / 2) - bias;
		m_lowerZ = m_position.y - (m_size.y / 2) - bias;
		m_upperX = m_position.x + (m_size.x / 2) + bias;
		m_upperZ = m_position.y + (m_size.y / 2) + bias;


		for (DoorWay& doorWay : m_doorWaysXBackWall)
			if (doorWay.type == DoorWayType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::POS_X)
					m_upperZ += 0.06f;

		for (DoorWay& doorWay : m_doorWaysXFrontWall)
			if (doorWay.type == DoorWayType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::NEG_X)
					m_lowerZ -= 0.06f;


		for (DoorWay& doorWay : m_doorWaysZRightWall)
			if (doorWay.type == DoorWayType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::POS_Z)
					m_upperX += 0.06f;

		for (DoorWay& doorWay : m_doorWaysZLeftWall)
			if (doorWay.type == DoorWayType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::NEG_Z)
					m_lowerX -= 0.06f;
		/*
			light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperZ));

		for (DoorWay& doorWay : room->m_doorWaysXFrontWall)
			light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerZ));

		for (DoorWay& doorWay : room->m_doorWaysZLeftWall)
			light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_lowerX));

		for (DoorWay& doorWay : room->m_doorWaysZRightWall)
			light.m_doorWayLightVolumes.push_back(LightVolumeDoorWay(doorWay, light.m_position, light.m_radius, room->m_upperX));*/
	}
}