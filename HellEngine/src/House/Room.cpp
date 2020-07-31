#include "hellpch.h"
#include "Room.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Core/LevelEditor.h"
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
		m_floor = Floor(glm::vec3(m_position.x, 0, m_position.y), m_size, m_story, false, this);
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
		LevelEditor::SetHighlightColorIfSelected(shader, &m_floor);
		m_floor.Draw(shader);
		shader->setVec3("ColorAdd", glm::vec3(0, 0, 0));

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

		std::vector<HoleInWall> potentialConnectedDoorWays;


		for (Window& window : windows) {
			HoleInWall doorWay;
			doorWay.position = window.m_transform.position;
			doorWay.type = HoleInWallType::WINDOW;
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
			HoleInWall doorWay;
			doorWay.position = door.m_rootTransform.position;
			doorWay.type = HoleInWallType::DOOR;
			doorWay.story = door.m_story;
			doorWay.axis = door.m_axis;
			doorWay.parent = &door; 
			doorWay.bottom_Y = (door.m_story * ROOM_HEIGHT);
			doorWay.top_Y = (door.m_story * ROOM_HEIGHT) + DOOR_HEIGHT;
			doorWay.width = DOOR_WIDTH;
			potentialConnectedDoorWays.push_back(doorWay);
		}

		// Do the same for staircase openings
		for (HoleInWall& doorWay : potentialConnectedDoorWays)
		{
			if (withinRange(doorWay.position.x, low_x, high_x)) {
				if (withinRange(doorWay.position.z, low_z - 0.2f, low_z + 0.2f) && (doorWay.axis != Axis::NEG_Z) && (doorWay.axis != Axis::POS_Z))
					m_doorWaysXFrontWall.push_back(doorWay);
				else if (withinRange(doorWay.position.z, high_z - 0.2f, high_z + 0.2f) && (doorWay.axis != Axis::NEG_Z) && (doorWay.axis != Axis::POS_Z))
					m_doorWaysXBackWall.push_back(doorWay);
			}

			if (withinRange(doorWay.position.z, low_z, high_z)) {
				if (withinRange(doorWay.position.x, low_x - 0.2f, low_x + 0.2f) && (doorWay.axis != Axis::NEG_X) && (doorWay.axis != Axis::POS_X))
					m_doorWaysZLeftWall.push_back(doorWay);
				if (withinRange(doorWay.position.x, high_x - 0.2f, high_x + 0.2f) && (doorWay.axis != Axis::NEG_X) && (doorWay.axis != Axis::POS_X))
					m_doorWaysZRightWall.push_back(doorWay);
			}
		}

		// Sort em
		std::sort(m_doorWaysXFrontWall.begin(), m_doorWaysXFrontWall.end(), [](const HoleInWall a, const HoleInWall b) { return a.position.x < b.position.x; });
		std::sort(m_doorWaysXBackWall.begin(), m_doorWaysXBackWall.end(), [](const HoleInWall a, const HoleInWall b) { return a.position.x > b.position.x; });
		std::sort(m_doorWaysZLeftWall.begin(), m_doorWaysZLeftWall.end(), [](const HoleInWall a, const HoleInWall b) { return a.position.z > b.position.z; });
		std::sort(m_doorWaysZRightWall.begin(), m_doorWaysZRightWall.end(), [](const HoleInWall a, const HoleInWall b) { return a.position.z < b.position.z; });
	}

	void Room::BuildWallMesh()
	{
		//m_wallSegments.clear();
		ResetWallSegmentsAndRemoveAssociatedCollisionObjects();

		// Clean up any outdated physics shit
		for (btCollisionObject* collisionObject : m_collisionObjects)
			Physics::s_dynamicsWorld->removeCollisionObject(collisionObject);
		m_collisionObjects.clear();

		// Find door
		float low_x = m_position.x - (m_size.x / 2);
		float high_x = m_position.x + (m_size.x / 2);
		float low_z = m_position.y - (m_size.y / 2);
		float high_z = m_position.y + (m_size.y / 2);
		
		// DO NOT HARDCODE THIS. IT'S ALSO IN WALLMESH.CPP

		float wallWidth;
		float ROOM_Y = (m_story * STORY_HEIGHT);
		float bias = 0.01; // to hide cracks
		float cursorX;
		float cursorZ;
		
		/////////////////
		// Front walls //
		{
			cursorX = low_x;
			cursorZ = low_z;
			for (int i = 0; i < m_doorWaysXFrontWall.size(); i++) {

				HoleInWall* doorway = &m_doorWaysXFrontWall[i];
				float wallWidth = doorway->position.x - cursorX - (doorway->width / 2);
				cursorX += (wallWidth + doorway->width);
				glm::vec3 wallSegmentPosition = glm::vec3(cursorX + (wallWidth / 2) -doorway->width - wallWidth, ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_X, wallWidth, ROOM_HEIGHT, 0));

				// Build upper and lower segment of the break in the wall
				float lowerSegmentHeight = doorway->bottom_Y;
				float upperSegmentHeight = ROOM_HEIGHT - doorway->top_Y;
				wallSegmentPosition = glm::vec3(cursorX + doorway->width / 2 - doorway->width, ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_X, doorway->width, upperSegmentHeight, lowerSegmentHeight));
			}

			// Full height wall (final)
			wallWidth = high_x - cursorX;
			glm::vec3 wallSegmentPosition = glm::vec3(cursorX + wallWidth / 2, ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
			m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_X, wallWidth, ROOM_HEIGHT, 0));
		}

		////////////////
		// Back walls //
		{
			cursorX = high_x;
			cursorZ = high_z;
			for (int i = 0; i < m_doorWaysXBackWall.size(); i++) {

				HoleInWall* doorway = &m_doorWaysXBackWall[i];
				float wallWidth = cursorX - doorway->position.x - (doorway->width / 2);
				cursorX -= (wallWidth + doorway->width);
				// Full height wall
				glm::vec3 wallSegmentPosition = glm::vec3(cursorX - (wallWidth / 2) + (wallWidth + doorway->width), ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_X, wallWidth, ROOM_HEIGHT, 0)); m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_X, wallWidth, ROOM_HEIGHT, 0));

				// Build upper and lower segment of the break in the wall
				float lowerSegmentHeight = doorway->bottom_Y;
				float upperSegmentHeight = ROOM_HEIGHT - doorway->top_Y;
				wallSegmentPosition = glm::vec3(cursorX + doorway->width / 2, ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_X, doorway->width, upperSegmentHeight, lowerSegmentHeight));
			}

			// Full height wall (final)
			wallWidth = cursorX - low_x;
			glm::vec3 wallSegmentPosition = glm::vec3(cursorX - wallWidth / 2, ROOM_Y + ROOM_HEIGHT / 2, cursorZ);
			m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_X, wallWidth, ROOM_HEIGHT, 0));
		}
	
		////////////////
		// Left walls //
		{
			cursorZ = high_z;
			cursorX = low_x;
			for (int i = 0; i < m_doorWaysZLeftWall.size(); i++) {

				HoleInWall* doorway = &m_doorWaysZLeftWall[i];
				float wallWidth = cursorZ - doorway->position.z - (doorway->width / 2);
				cursorZ -= (wallWidth + doorway->width);

				// Full height wall
				glm::vec3 wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, (cursorZ - wallWidth / 2) + (wallWidth + doorway->width));
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_Z, wallWidth, ROOM_HEIGHT, 0));

				// Build upper and lower segment of the break in the wall
				float lowerSegmentHeight = doorway->bottom_Y;
				float upperSegmentHeight = ROOM_HEIGHT - doorway->top_Y;
				wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, cursorZ + doorway->width / 2);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_Z, doorway->width, upperSegmentHeight, lowerSegmentHeight));
			}
			// Full height wall (final)
			wallWidth = cursorZ - low_z;
			glm::vec3 wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, cursorZ - wallWidth / 2);
			m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, POS_Z, wallWidth, ROOM_HEIGHT, 0));
		}

		/////////////////
		// Right walls //
		{
			cursorZ = low_z;
			cursorX = high_x;
			for (int i = 0; i < m_doorWaysZRightWall.size(); i++) {

				HoleInWall* doorway = &m_doorWaysZRightWall[i];
				float wallWidth =  doorway->position.z - cursorZ - (doorway->width / 2);
				cursorZ += (wallWidth + doorway->width);

				// Full height wall
				glm::vec3 wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, (cursorZ - wallWidth / 2) - (doorway->width));
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_Z, wallWidth, ROOM_HEIGHT, 0));

				// Build upper and lower segment of the break in the wall
				float lowerSegmentHeight = doorway->bottom_Y;
				float upperSegmentHeight = ROOM_HEIGHT - doorway->top_Y;
				wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, cursorZ + (doorway->width / 2) - doorway->width);
				m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_Z, doorway->width, upperSegmentHeight, lowerSegmentHeight));
			}
			// Full height wall (final)
			wallWidth = high_z - cursorZ;
			glm::vec3 wallSegmentPosition = glm::vec3(cursorX, ROOM_Y + ROOM_HEIGHT / 2, cursorZ - (wallWidth / 2) + wallWidth);
			m_wallSegments.emplace_back(WallSegment(wallSegmentPosition, NEG_Z, wallWidth, ROOM_HEIGHT, 0));
		}


		/////////////////////////
		// Build the wall Mesh //

		m_wallMesh.ClearMesh();

		for (WallSegment& segment : m_wallSegments)
		{
			glm::vec4 v;
			glm::vec3 bottomLeft, topright;

			Transform trans;
			trans.position = segment.m_position;
			trans.rotation = Util::SetRotationByAxis(segment.m_axis);
			glm::mat4 m = trans.to_mat4();

			// Upper segment
			v = m * glm::vec4(-segment.m_width / 2, (ROOM_HEIGHT / 2) - segment.m_heightOfUpperSegment, 0.0f, 1.0f);
			bottomLeft = glm::vec3(v.x, v.y, v.z);
			v = m * glm::vec4(segment.m_width / 2, ROOM_HEIGHT / 2, 0.0f, 1.0f);
			topright = glm::vec3(v.x, v.y, v.z);
			m_wallMesh.AddQuad(bottomLeft, topright, segment.m_axis);

			// Lower segment
			if (segment.m_heightOfLowerSegment > 0)	{
				v = m * glm::vec4(-segment.m_width / 2, (-ROOM_HEIGHT / 2), 0.0f, 1.0f);
				bottomLeft = glm::vec3(v.x, v.y, v.z);
				v = m * glm::vec4(segment.m_width / 2, (-ROOM_HEIGHT / 2) + segment.m_heightOfLowerSegment, 0.0f, 1.0f);
				topright = glm::vec3(v.x, v.y, v.z);
				m_wallMesh.AddQuad(bottomLeft, topright, segment.m_axis);
			}

			// Floor trim
			if (segment.m_heightOfLowerSegment > 0 || segment.m_heightOfUpperSegment == ROOM_HEIGHT)
			{
				Transform trim;
				trim.position = segment.m_position;
				//trim.position.z += segment.m_width / 2;
				trim.position.y -= ROOM_HEIGHT / 2;
				trim.rotation = Util::SetRotationByAxis(segment.m_axis);
				trim.scale.x = segment.m_width;
				m_floorTrimTransforms.push_back(trim);
			}
			// Ceiling trim
			{
				Transform trim;
				trim.position = segment.m_position;
				//trim.position.z += segment.m_width / 2;
				trim.position.y -= ROOM_HEIGHT / 2;
				trim.rotation = Util::SetRotationByAxis(segment.m_axis);
				trim.scale.x = segment.m_width;
				m_ceilingTrimTransforms.push_back(trim);
			}
		}
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


		for (HoleInWall& doorWay : m_doorWaysXBackWall)
			if (doorWay.type == HoleInWallType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::POS_X)
					m_upperZ += 0.06f;

		for (HoleInWall& doorWay : m_doorWaysXFrontWall)
			if (doorWay.type == HoleInWallType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::NEG_X)
					m_lowerZ -= 0.06f;


		for (HoleInWall& doorWay : m_doorWaysZRightWall)
			if (doorWay.type == HoleInWallType::DOOR)
				if (((Door*)doorWay.parent)->m_axis == Axis::POS_Z)
					m_upperX += 0.06f;

		for (HoleInWall& doorWay : m_doorWaysZLeftWall)
			if (doorWay.type == HoleInWallType::DOOR)
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

	void Room::ResetWallSegmentsAndRemoveAssociatedCollisionObjects()
	{
		for (WallSegment& segment : m_wallSegments)
			segment.RemoveCollisionObjects();

		m_wallSegments.clear();
	}

}