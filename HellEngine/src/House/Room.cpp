#include "hellpch.h"
#include "Room.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	Room::Room(glm::vec2 position, glm::vec2 size, int story)
	{
		m_position = position;
		m_size = size;
		m_story = story;
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

		m_doorWaysFrontWall.clear();
		m_doorWaysBackWall.clear();
		m_doorWaysLeftWall.clear();
		m_doorWaysRightWall.clear();

		m_floorTrimTransforms.clear();
		m_ceilingTrimTransforms.clear();

		CalculateWorldSpaceBounds();
		CalculateLightVolume();
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
		for (Transform transform : m_floorTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			AssetManager::BindMaterial(AssetManager::s_MaterialID_Trims);
			AssetManager::DrawModel(AssetManager::s_ModelID_FloorTrim, shader, transform.to_mat4());
		}
		// Ceiling trims
		for (Transform transform : m_ceilingTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			AssetManager::BindMaterial(AssetManager::s_MaterialID_Trims);
			AssetManager::DrawModel(AssetManager::s_ModelID_CeilingTrim, shader, transform.to_mat4());
		}
		// Reset to default
		shader->setVec2("TEXTURE_SCALE", glm::vec2(1.0, 1.0));
	}

	void Room::FindDoors(std::vector<Door>& doors, std::vector<Staircase>& staircases)
	{
		float low_x = std::min(m_position.x - (m_size.x / 2), m_position.x + (m_size.x / 2));
		float high_x = std::max(m_position.x - (m_size.x / 2), m_position.x + (m_size.x / 2));
		float low_z = std::min(m_position.y - (m_size.y / 2), m_position.y + (m_size.y / 2));
		float high_z = std::max(m_position.y - (m_size.y / 2), m_position.y + (m_size.y / 2));

		m_doorWaysBackWall.clear();
		m_doorWaysFrontWall.clear();
		m_doorWaysLeftWall.clear();
		m_doorWaysRightWall.clear();

		std::vector<DoorWay> potentialConnectedDoorWays;

		for (Staircase& staircase : staircases) {
			potentialConnectedDoorWays.push_back(staircase.m_bottomDoorway);
			potentialConnectedDoorWays.push_back(staircase.m_topDoorway);
		}

		for (Door& door : doors) {
			DoorWay doorWay;
			doorWay.position = door.m_rootTransform.position;
			doorWay.type = DoorWayType::DOOR;
			doorWay.story = door.m_story;
			potentialConnectedDoorWays.push_back(doorWay);
		}

		// Do the same for staircase openings
		for (DoorWay& doorWay : potentialConnectedDoorWays)
		{
			if (withinRange(doorWay.position.x, low_x, high_x)) {
				if (withinRange(doorWay.position.z, low_z - 0.2f, low_z + 0.2f))
					m_doorWaysFrontWall.push_back(doorWay);
				else if (withinRange(doorWay.position.z, high_z - 0.2f, high_z + 0.2f))
					m_doorWaysBackWall.push_back(doorWay);
			}

			if (withinRange(doorWay.position.z, low_z, high_z)) {
				if (withinRange(doorWay.position.x, low_x - 0.2f, low_x + 0.2f))
					m_doorWaysLeftWall.push_back(doorWay);
				if (withinRange(doorWay.position.x, high_x - 0.2f, high_x + 0.2f))
					m_doorWaysRightWall.push_back(doorWay);
			}

			// Falls within Z wall range...
			/*if (withinRange(doorWay.position.z, low_z, high_z)) {
				if (sameCoordinate(doorWay.position.x, low_x - 0.05f))
					m_doorWaysLeftWall.push_back(doorWay);
				else if (sameCoordinate(doorWay.position.x, high_x + 0.05f))
					m_doorWaysRightWall.push_back(doorWay);
			}*/
		}

		// Sort em
		std::sort(m_doorWaysFrontWall.begin(), m_doorWaysFrontWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.x < b.position.x; });
		std::sort(m_doorWaysBackWall.begin(), m_doorWaysBackWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.x > b.position.x; });
		std::sort(m_doorWaysLeftWall.begin(), m_doorWaysLeftWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.z > b.position.z; });
		std::sort(m_doorWaysRightWall.begin(), m_doorWaysRightWall.end(), [](const DoorWay a, const DoorWay b) { return a.position.z < b.position.z; });
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
		float ROOM_Y = m_story * STORY_HEIGHT;
		float bias = 0.01; // to hide cracks
		
		// Front walls		
		float cursorX = low_x;
		float cursorZ = low_z;
		for (int i = 0; i < m_doorWaysFrontWall.size(); i++) 
		{
			wallWidth = m_doorWaysFrontWall[i].position.x - cursorX - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX + wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
			cursorX += wallWidth + DOOR_WIDTH;
			//if (m_doorWaysFrontWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX - DOOR_WIDTH, DOOR_HEIGHT + ROOM_Y, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // above door
				AddCeilingTrim(glm::vec3(cursorX - DOOR_WIDTH, ROOM_Y, cursorZ), 0, DOOR_WIDTH);
		//	}
		}
		wallWidth = high_x - cursorX;
		m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX + wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_X); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), 0, wallWidth);

		// Back walls
		cursorX = high_x;
		cursorZ = high_z;
		for (int i = 0; i < m_doorWaysBackWall.size(); i++)	{
			float wallWidth = cursorX - m_doorWaysBackWall[i].position.x - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX - wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
			cursorX -= wallWidth + DOOR_WIDTH;
		//	if (m_doorWaysBackWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX + DOOR_WIDTH, DOOR_HEIGHT + ROOM_Y, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // above door
				AddCeilingTrim(glm::vec3(cursorX + DOOR_WIDTH, ROOM_Y, cursorZ), ROTATE_180, DOOR_WIDTH);
		//	}
		}
		wallWidth = cursorX - low_x;
		m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX - wallWidth, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_X); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_180, wallWidth);

		// Left walls
		cursorZ = high_z;
		cursorX = low_x;
		for (int i = 0; i < m_doorWaysLeftWall.size(); i++) {
			float wallWidth = cursorZ - m_doorWaysLeftWall[i].position.z - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ - wallWidth), POS_Z); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
			cursorZ -= wallWidth + DOOR_WIDTH;
			//if (m_doorWaysLeftWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX, DOOR_HEIGHT + ROOM_Y, cursorZ + DOOR_WIDTH), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), POS_Z); // above door
				AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ + DOOR_WIDTH), ROTATE_90, DOOR_WIDTH);
			//}
		}
		wallWidth = cursorZ - low_z;
		m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ - wallWidth), POS_Z); // wall
		AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);
		AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_90, wallWidth);

		// Right walls
		cursorZ = low_z;
		cursorX = high_x;
		for (int i = 0; i < m_doorWaysRightWall.size(); i++)
		{
			float wallWidth = m_doorWaysRightWall[i].position.z - cursorZ - (DOOR_WIDTH / 2);
			m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ + wallWidth), NEG_Z); // wall
			AddFloorTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);
			AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ), ROTATE_270, wallWidth);
			cursorZ += wallWidth + DOOR_WIDTH;
			//if (m_doorWaysRightWall[i].type == DoorWayType::DOOR) {
				m_wallMesh.AddQuad(glm::vec3(cursorX, DOOR_HEIGHT + ROOM_Y, cursorZ - DOOR_WIDTH), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ), NEG_Z); // above door
				AddCeilingTrim(glm::vec3(cursorX, ROOM_Y, cursorZ - DOOR_WIDTH), ROTATE_270, DOOR_WIDTH);
			//}
		}
		wallWidth = high_z - cursorZ;
		m_wallMesh.AddQuad(glm::vec3(cursorX, FLOOR_TRIM_HEIGHT + ROOM_Y - bias, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + ROOM_Y + bias, cursorZ + wallWidth), NEG_Z); // wall
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
	/*
	void Room::DrawWallTrim(Shader* shader)
	{
		RenderableObject bottomTrim = RenderableObject("WallTrimFloor", AssetManager::GetModelByName("WallTrim.obj"), NULL);
		RenderableObject topTrim = RenderableObject("WallTrimFloor", AssetManager::GetModelByName("WallTrimTop.obj"), NULL);

		for (Transform transform : bottomTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			bottomTrim.transform = transform;
			bottomTrim.Draw(shader, true);
		}
		for (Transform transform : topTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			topTrim.transform = transform;
			topTrim.Draw(shader, true);
		}
	}*/

	void Room::CalculateWorldSpaceBounds()
	{
		m_lowerX = m_position.x - (m_size.x / 2);
		m_lowerZ = m_position.y - (m_size.y / 2);
		m_upperX = m_position.x + (m_size.x / 2);
		m_upperZ = m_position.y + (m_size.y / 2);
	}

	void Room::CalculateLightVolume()
	{
		float bias = 0;
		m_lightVolume.vertices.clear();
		
		// Create vertices
		glm::vec3 A1 = glm::vec3(m_lowerX - bias, -bias, m_lowerZ - bias);
		glm::vec3 B1 = glm::vec3(m_upperX + bias, -bias, m_lowerZ - bias);
		glm::vec3 C1 = glm::vec3(m_lowerX - bias, -bias, m_upperZ + bias);
		glm::vec3 D1 = glm::vec3(m_upperX + bias, -bias, m_upperZ + bias);
		glm::vec3 A2 = glm::vec3(m_lowerX - bias, 2.4f + bias, m_lowerZ - bias);
		glm::vec3 B2 = glm::vec3(m_upperX + bias, 2.4f + bias, m_lowerZ - bias);
		glm::vec3 C2 = glm::vec3(m_lowerX - bias, 2.4f + bias, m_upperZ + bias);
		glm::vec3 D2 = glm::vec3(m_upperX + bias, 2.4f + bias, m_upperZ + bias);

		// Floor
		m_lightVolume.vertices.push_back(A1);
		m_lightVolume.vertices.push_back(B1);
		m_lightVolume.vertices.push_back(C1);
		m_lightVolume.vertices.push_back(D1);
		m_lightVolume.vertices.push_back(C1);
		m_lightVolume.vertices.push_back(B1);
		// Ceiling
		m_lightVolume.vertices.push_back(C2);
		m_lightVolume.vertices.push_back(B2);
		m_lightVolume.vertices.push_back(A2);
		m_lightVolume.vertices.push_back(B2);
		m_lightVolume.vertices.push_back(C2);
		m_lightVolume.vertices.push_back(D2);
		// Side wall
		m_lightVolume.vertices.push_back(C1);
		m_lightVolume.vertices.push_back(A2);
		m_lightVolume.vertices.push_back(A1);
		m_lightVolume.vertices.push_back(C2);
		m_lightVolume.vertices.push_back(A2);
		m_lightVolume.vertices.push_back(C1);
		// Side wall the other one
		m_lightVolume.vertices.push_back(B1);
		m_lightVolume.vertices.push_back(B2);
		m_lightVolume.vertices.push_back(D1);
		m_lightVolume.vertices.push_back(D1);
		m_lightVolume.vertices.push_back(B2);
		m_lightVolume.vertices.push_back(D2);
		// Front wall
		m_lightVolume.vertices.push_back(A1);
		m_lightVolume.vertices.push_back(B2);
		m_lightVolume.vertices.push_back(B1);
		m_lightVolume.vertices.push_back(A1);
		m_lightVolume.vertices.push_back(A2);
		m_lightVolume.vertices.push_back(B2);
		// Back wall
		m_lightVolume.vertices.push_back(D1);
		m_lightVolume.vertices.push_back(D2);
		m_lightVolume.vertices.push_back(C1);
		m_lightVolume.vertices.push_back(D2);
		m_lightVolume.vertices.push_back(C2);
		m_lightVolume.vertices.push_back(C1);
		m_lightVolume.Setup();
	}
}