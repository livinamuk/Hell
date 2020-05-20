#include "hellpch.h"
#include "Staircase.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"

namespace HellEngine
{
	float Staircase::trimScale = 0;
	float Staircase::yOffset = 0;

	Staircase::Staircase(glm::vec2 bottomOpening, Axis axis, int story, int firstSetCount, bool turnsLeft, bool FloorRotateBottom, bool FloorRotateTop, bool FloorRotateLanding)
	{
		// Position
		m_rootTransform.position.x = bottomOpening.x;
		m_rootTransform.position.z = bottomOpening.y;
		m_story = story;
		m_axis = axis;
		m_stepsInFirstSet = firstSetCount;
		m_turnsLeft = turnsLeft;

		m_bottomDoorwayFloor.m_rotateTexture = FloorRotateBottom;
		m_topDoorwayFloor.m_rotateTexture = FloorRotateTop;
		m_landingFloor.m_rotateTexture = FloorRotateLanding;

		Reconfigure();
	}

	void Staircase::Reconfigure()
	{
		// Height
		m_rootTransform.position.y = STORY_HEIGHT * m_story;

		// Step count
		m_stepsInSecondSet = 18 - m_stepsInFirstSet;

		// Rotation
		if (m_axis == Axis::POS_X)
			m_rootTransform.rotation.y = ROTATE_0;
		if (m_axis == Axis::NEG_X)
			m_rootTransform.rotation.y = ROTATE_180;
		if (m_axis == Axis::POS_Z)
			m_rootTransform.rotation.y = ROTATE_90;
		if (m_axis == Axis::NEG_Z)
			m_rootTransform.rotation.y = ROTATE_270;


		// Landing
		m_landingTransform.position.x = 0;
		m_landingTransform.position.y = stepHeight * (m_stepsInFirstSet);
		m_landingTransform.position.z = stepDepth * (m_stepsInFirstSet)+0.5f + stairIndent;
		
		if (m_turnsLeft)
			m_landingTransform.rotation.y = ROTATE_270;
		else
			m_landingTransform.rotation.y = ROTATE_90;

		// Doorway: bottom
		Transform bottomDoorWayTransform;
		bottomDoorWayTransform.position.z = 0.05f;

		m_bottomDoorway.position = Util::GetTranslationFromMatrix(m_rootTransform.to_mat4() * bottomDoorWayTransform.to_mat4());
		m_bottomDoorway.type = DoorWayType::STAIRCASE_OPENING;
		m_bottomDoorway.story = m_story;
		m_bottomDoorway.axis = m_axis;

		// Doorway: top
		if (m_stepsInSecondSet == 0) {
			Transform topDoorWayTransform;
			topDoorWayTransform.position.y = 18 * stepHeight;
			topDoorWayTransform.position.z = (18 * stepDepth) + 0.15f;
			m_topDoorway.position = Util::GetTranslationFromMatrix(m_rootTransform.to_mat4() * topDoorWayTransform.to_mat4());
			m_topDoorway.type = DoorWayType::STAIRCASE_OPENING;
			m_topDoorway.story = m_story + 1;
			m_topDoorway.axis = m_axis;
		}
		if (m_stepsInSecondSet > 0) {
			Transform topDoorWayTransform;

			//	topDoorWayTransform.position.x = -0.5f;
			topDoorWayTransform.position.y = m_stepsInSecondSet * stepHeight;
			topDoorWayTransform.position.z = (m_stepsInSecondSet * stepDepth) + 0.05f + 0.5f;
			m_topDoorway.position = Util::GetTranslationFromMatrix(m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * topDoorWayTransform.to_mat4());
			m_topDoorway.type = DoorWayType::STAIRCASE_OPENING;
			m_topDoorway.story = m_story + 1;

			if (m_turnsLeft && m_axis == Axis::POS_X)
				m_topDoorway.axis = Axis::POS_Z;
			if (m_turnsLeft && m_axis == Axis::NEG_X)
				m_topDoorway.axis = Axis::NEG_Z;
			if (m_turnsLeft && m_axis == Axis::POS_Z)
				m_topDoorway.axis = Axis::POS_X;
			if (m_turnsLeft && m_axis == Axis::NEG_Z)
				m_topDoorway.axis = Axis::NEG_X;

			if (!m_turnsLeft && m_axis == Axis::POS_X)
				m_topDoorway.axis = Axis::NEG_Z;
			if (!m_turnsLeft && m_axis == Axis::NEG_X)
				m_topDoorway.axis = Axis::POS_Z;
			if (!m_turnsLeft && m_axis == Axis::POS_Z)
				m_topDoorway.axis = Axis::NEG_X;
			if (!m_turnsLeft && m_axis == Axis::NEG_Z)
				m_topDoorway.axis = Axis::POS_X;
		}



		// gross, but whatever
		if (m_stepsInSecondSet > 0) {
			glm::vec3 landingPosition = glm::vec3(0);
			Util::TranslatePosition(&landingPosition, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_landingFloor = Floor(landingPosition, glm::vec2(1.075f, 1.075f));
		}

		// Floor: bottom door way
		Transform bottomFloorTransform;
		bottomFloorTransform.position.y = 0;
		bottomFloorTransform.position.z = (stairIndent / 2) + 0.025;

		glm::vec2 doorwayFloorSize;
		if (m_axis == Axis::POS_X || m_axis == Axis::NEG_X)
			doorwayFloorSize = glm::vec2(1, stairIndent + 0.05f);
		else
			doorwayFloorSize = glm::vec2(stairIndent + 0.05f, 1);

		m_bottomDoorwayFloor = Floor(Util::Position_From_Mat_4(m_rootTransform.to_mat4() * bottomFloorTransform.to_mat4()), doorwayFloorSize, m_story, m_bottomDoorwayFloor.m_rotateTexture);
		
		// Floor: top door way
		Transform topDoorWayFloorTransform;
		topDoorWayFloorTransform.position = m_topDoorway.position;
		if (m_axis == Axis::POS_Z) topDoorWayFloorTransform.rotation.y = ROTATE_90;
		m_topDoorwayFloor = Floor(Util::Position_From_Mat_4(m_rootTransform.to_mat4() * topDoorWayFloorTransform.to_mat4()), doorwayFloorSize, m_story, m_topDoorwayFloor.m_rotateTexture);




		// Ceiling: Bottom doorway
		Transform bottomCeilingTransform;
		bottomCeilingTransform.position.y = ROOM_HEIGHT + (ROOM_HEIGHT * m_story);
		bottomCeilingTransform.position.z = (stairIndent / 2);
		m_bottomDoorwayCeiling = Ceiling(Util::Position_From_Mat_4(m_rootTransform.to_mat4() * bottomCeilingTransform.to_mat4()), glm::vec2(1, stairIndent));

		// Build first set
		glm::vec3 vertA, vertB, vertC, vertD;
		for (int i = 0; i < m_stepsInFirstSet; i++) {
			vertA = glm::vec3(stepWidth / 2, (i * stepHeight), (i * stepDepth) + stairIndent);
			vertB = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + stairIndent);
			vertC = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + stairIndent);
			vertD = glm::vec3(-stepWidth / 2, (i * stepHeight), (i * stepDepth) + stairIndent);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
			m_stairMesh.AddPlane(vertA, vertB, vertC, vertD);

			vertA = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + stairIndent);
			vertB = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), stepDepth + (i * stepDepth) + stairIndent);
			vertC = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), stepDepth + (i * stepDepth) + stairIndent);
			vertD = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + stairIndent);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
			m_stairMesh.AddPlane(vertA, vertB, vertC, vertD);
		}

		// Build second set
		for (int i = 0; i < m_stepsInSecondSet; i++) {
			vertA = glm::vec3(stepWidth / 2, (i * stepHeight), (i * stepDepth) + 0.5f);
			vertB = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + 0.5f);
			vertC = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + 0.5f);
			vertD = glm::vec3(-stepWidth / 2, (i * stepHeight), (i * stepDepth) + 0.5f);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_stairMesh.AddPlane(vertA, vertB, vertC, vertD);

			vertA = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + 0.5f);
			vertB = glm::vec3(stepWidth / 2, stepHeight + (i * stepHeight), stepDepth + (i * stepDepth) + 0.5f);
			vertC = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), stepDepth + (i * stepDepth) + 0.5f);
			vertD = glm::vec3(-stepWidth / 2, stepHeight + (i * stepHeight), (i * stepDepth) + 0.5f);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_stairMesh.AddPlane(vertA, vertB, vertC, vertD);
		}

		// Walls
		float bias = 0.02f;
		float y_high = ROOM_HEIGHT - CEILING_TRIM_HEIGHT + m_landingTransform.position.y + bias;
		float y_low = ROOM_HEIGHT - CEILING_TRIM_HEIGHT + bias;
		float totalDepth = (stepDepth * m_stepsInFirstSet);

		// left wall bottom
		vertA = glm::vec3(-0.5f, m_landingTransform.position.y, totalDepth + stairIndent);
		vertB = glm::vec3(-0.5f, 0, stairIndent);
		vertC = glm::vec3(-0.5f, y_low, stairIndent);
		vertD = glm::vec3(-0.5f, y_high, totalDepth + stairIndent);
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
		m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

		// left wall bottom straight
		vertA = glm::vec3(-0.5f, 0, stairIndent);
		vertB = glm::vec3(-0.5f, 0, 0);
		vertC = glm::vec3(-0.5f, y_low, 0);
		vertD = glm::vec3(-0.5f, y_low, stairIndent);
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
		m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

		// right wall bottom
		vertA = glm::vec3(0.5f, 0, stairIndent);
		vertB = glm::vec3(0.5f, m_landingTransform.position.y, totalDepth + stairIndent);
		vertC = glm::vec3(0.5f, y_high, totalDepth + stairIndent);
		vertD = glm::vec3(0.5f, y_low, stairIndent);
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
		m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

		// right wall bottom straight
		vertA = glm::vec3(0.5f, 0, 0);
		vertB = glm::vec3(0.5f, 0, stairIndent);
		vertC = glm::vec3(0.5f, y_low, stairIndent);
		vertD = glm::vec3(0.5f, y_low, 0);
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
		m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);


		// Ceiling: first set
		vertA = glm::vec3(-0.5f, ROOM_HEIGHT, stairIndent);
		vertB = glm::vec3(+0.5f, ROOM_HEIGHT, stairIndent);
		vertC = glm::vec3(+0.5f, ROOM_HEIGHT + (stepHeight * m_stepsInFirstSet), stairIndent + (stepDepth * m_stepsInFirstSet));
		vertD = glm::vec3(-0.5f, ROOM_HEIGHT + (stepHeight * m_stepsInFirstSet), stairIndent + (stepDepth * m_stepsInFirstSet));
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
		m_ceilingMesh.AddPlane(vertA, vertB, vertC, vertD);

		if (m_stepsInSecondSet > 0)
		{
			// left and right wall top
			Transform trans;
			trans.position.z = 0.5f;
			y_high = ROOM_HEIGHT - CEILING_TRIM_HEIGHT + (stepHeight * m_stepsInSecondSet);
			y_low = ROOM_HEIGHT - CEILING_TRIM_HEIGHT;
			totalDepth = (stepDepth * m_stepsInSecondSet);

			vertA = glm::vec3(-0.5f, 0, totalDepth);
			vertB = glm::vec3(-0.5f, 0, 0);
			vertC = glm::vec3(-0.5f, y_low, 0);
			vertD = glm::vec3(-0.5f, y_high, totalDepth);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

			vertA = glm::vec3(0.5f, 0, 0);
			vertB = glm::vec3(0.5f, 0, totalDepth);
			vertC = glm::vec3(0.5f, y_high, totalDepth);
			vertD = glm::vec3(0.5f, y_low, 0);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

			// landing back wall A
			trans.position.x = -0.5f;
			trans.position.y = m_stepsInFirstSet * stepHeight;
			trans.position.z = m_stepsInFirstSet * stepDepth + 1 + stairIndent;
			vertA = glm::vec3(1, 0, 0);
			vertB = glm::vec3(0, 0, 0);
			vertC = glm::vec3(0, ROOM_HEIGHT, 0);
			vertD = glm::vec3(1, ROOM_HEIGHT, 0);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * trans.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * trans.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

			// landing back wall B
			vertA = glm::vec3(-0.5f, 0, -0.5f);
			vertB = glm::vec3(0.5f, 0, -0.5f);
			vertC = glm::vec3(0.5f, ROOM_HEIGHT, -0.5f);
			vertD = glm::vec3(-0.5f, ROOM_HEIGHT, -0.5f);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

			// Ceiling: second set
			vertA = glm::vec3(-0.5f, ROOM_HEIGHT, 0.5f);
			vertB = glm::vec3(+0.5f, ROOM_HEIGHT, 0.5f);
			vertC = glm::vec3(+0.5f, ROOM_HEIGHT + (stepHeight * m_stepsInSecondSet), 0.5f + (stepDepth * m_stepsInSecondSet));
			vertD = glm::vec3(-0.5f, ROOM_HEIGHT + (stepHeight * m_stepsInSecondSet), 0.5f + (stepDepth * m_stepsInSecondSet));
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_ceilingMesh.AddPlane(vertA, vertB, vertC, vertD);

			// Ceiling: landing
			vertA = glm::vec3(-0.5f, ROOM_HEIGHT, -0.5f);
			vertB = glm::vec3(+0.5f, ROOM_HEIGHT, -0.5f);
			vertC = glm::vec3(+0.5f, ROOM_HEIGHT, 0.5f);
			vertD = glm::vec3(-0.5f, ROOM_HEIGHT, 0.5f);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_ceilingMesh.AddPlane(vertA, vertB, vertC, vertD);
		}

		if (m_stepsInSecondSet > 0)
		{
			// Above top door frame
			vertA = glm::vec3(0.5f, DOOR_HEIGHT + (m_stepsInSecondSet * stepHeight), 0.5f + (m_stepsInSecondSet * stepDepth));
			vertB = glm::vec3(-0.5f, DOOR_HEIGHT + (m_stepsInSecondSet * stepHeight), 0.5f + (m_stepsInSecondSet * stepDepth));
			vertC = glm::vec3(-0.5f, ROOM_HEIGHT + (m_stepsInSecondSet * stepHeight), 0.5f + (m_stepsInSecondSet * stepDepth));
			vertD = glm::vec3(0.5f, ROOM_HEIGHT + (m_stepsInSecondSet * stepHeight), 0.5f + (m_stepsInSecondSet * stepDepth));
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * m_landingTransform.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);
		}
		else
		{
			// Above top door frame
			vertA = glm::vec3(0.5f, DOOR_HEIGHT + (m_stepsInFirstSet * stepHeight), (m_stepsInFirstSet * stepDepth) + stairIndent);
			vertB = glm::vec3(-0.5f, DOOR_HEIGHT + (m_stepsInFirstSet * stepHeight), (m_stepsInFirstSet * stepDepth) + stairIndent);
			vertC = glm::vec3(-0.5f, ROOM_HEIGHT + (m_stepsInFirstSet * stepHeight), (m_stepsInFirstSet * stepDepth) + stairIndent);
			vertD = glm::vec3(0.5f, ROOM_HEIGHT + (m_stepsInFirstSet * stepHeight), (m_stepsInFirstSet * stepDepth) + stairIndent);
			Util::TranslatePosition(&vertA, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertB, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertC, m_rootTransform.to_mat4());
			Util::TranslatePosition(&vertD, m_rootTransform.to_mat4());
			m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);
		}

		// above bottom door frame		
		Transform trans;
		trans.position.x = -0.5f;
		trans.position.y = 0;
		trans.position.z = 0.1f;
		vertA = glm::vec3(0, DOOR_HEIGHT, 0);
		vertB = glm::vec3(1, DOOR_HEIGHT, 0);
		vertC = glm::vec3(1, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + bias, 0);
		vertD = glm::vec3(0, ROOM_HEIGHT - CEILING_TRIM_HEIGHT + bias, 0);
		Util::TranslatePosition(&vertA, m_rootTransform.to_mat4() * trans.to_mat4());
		Util::TranslatePosition(&vertB, m_rootTransform.to_mat4() * trans.to_mat4());
		Util::TranslatePosition(&vertC, m_rootTransform.to_mat4() * trans.to_mat4());
		Util::TranslatePosition(&vertD, m_rootTransform.to_mat4() * trans.to_mat4());
		m_wallMesh.AddPlane(vertA, vertB, vertC, vertD);

		//glm::vec3 cursorX = glm::vec3(m_rootTransform.position.x, 0, 0);
		//glm::vec3 cursorZ = glm::vec3(0, 0, m_rootTransform.position.z + 0.1f);
		//Util::TranslatePosition(&cursorX, m_rootTransform.to_mat4());
		//Util::TranslatePosition(&cursorZ, m_rootTransform.to_mat4());
		//m_wallMesh.AddQuad(glm::vec3(cursorX - DOOR_WIDTH, DOOR_HEIGHT, cursorZ), glm::vec3(cursorX, ROOM_HEIGHT - CEILING_TRIM_HEIGHT, cursorZ), POS_X); // above door



		// ceiling mesh
	}

	void Staircase::Draw(Shader* shader)
	{
		/*
		Transform transform;
		//AssetManager::GetModelByName("StaircaseNew")->Draw(shader, m_rootTransform.to_mat4());
		AssetManager::GetModelByName("StaircaseNew")->Draw(shader, transform.to_mat4());
		
		return;
		*/

		AssetManager::GetModelByID(AssetManager::s_ModelID_DoorFrame)->DrawMesh(shader, 0, Transform(m_bottomDoorway.position, Util::SetRotationByAxis(m_bottomDoorway.axis)).to_mat4());
		AssetManager::GetModelByID(AssetManager::s_ModelID_DoorFrame)->DrawMesh(shader, 0, Transform(m_topDoorway.position, Util::SetRotationByAxis(m_topDoorway.axis)).to_mat4());
		 
		// Trip: bottom door ceiling
		/*Transform bottomDoorwayCeilingTrimTransform;
		bottomDoorwayCeilingTrimTransform.position.x = -0.5f;
		bottomDoorwayCeilingTrimTransform.position.z = 0.1f;
		bottomDoorwayCeilingTrimTransform.scale.y = trimScale; /////////////////////////////////////
		bottomDoorwayCeilingTrimTransform.position.y = -yOffset; /////////////////////////////////////
		AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * bottomDoorwayCeilingTrimTransform.to_mat4());

		// sides
		bottomDoorwayCeilingTrimTransform;
		bottomDoorwayCeilingTrimTransform.position.x = -0.5;
		bottomDoorwayCeilingTrimTransform.position.z = stairIndent;
		bottomDoorwayCeilingTrimTransform.rotation.y = ROTATE_90;
		bottomDoorwayCeilingTrimTransform.scale.x = stairIndent;
		AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * bottomDoorwayCeilingTrimTransform.to_mat4());

		bottomDoorwayCeilingTrimTransform;
		bottomDoorwayCeilingTrimTransform.position.x = 0.5;
		bottomDoorwayCeilingTrimTransform.position.z = 0;
		bottomDoorwayCeilingTrimTransform.rotation.y = ROTATE_270;
		bottomDoorwayCeilingTrimTransform.scale.x = stairIndent;
		AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * bottomDoorwayCeilingTrimTransform.to_mat4());

		*/

		for (int i = 0; i < m_stepsInFirstSet / 3; i++)
		{
			Transform trans;
			trans.position.y = stepHeight * (3 * i);
			trans.position.z = stepDepth * (3 * i) + stairIndent;
			AssetManager::DrawModel(AssetManager::s_ModelID_Staircase, shader, m_rootTransform.to_mat4() * trans.to_mat4());
		}

		if (m_stepsInSecondSet > 0)
		{
			{
				// Corner piece
				Transform trans, indentTrans;
			//	trans.position.z = stairIndent;
				if (!m_turnsLeft) trans.rotation.y = ROTATE_90;
				AssetManager::GetModelByName("StaircaseLanding")->SetMaterial(AssetManager::GetMaterialIDByName("Trims"));
				AssetManager::GetModelByName("StaircaseLanding")->Draw(shader, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * indentTrans.to_mat4() * trans.to_mat4());
			}

			// Ceiling trims
			Transform trans;
			trans.position.x = 0.5f;
			trans.position.y = stepHeight * m_stepsInFirstSet;
			trans.position.z = stepDepth * m_stepsInFirstSet + 1 + stairIndent;
			trans.rotation.y = ROTATE_180;
	//		AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * trans.to_mat4());


			trans.position.x = 0.5f;
			trans.position.y = stepHeight * m_stepsInFirstSet;
			trans.position.z = stepDepth * m_stepsInFirstSet + stairIndent;
			trans.rotation.y = ROTATE_270;
			
			if (!m_turnsLeft) {
				trans.position.x = -0.5f;
				trans.rotation.y = ROTATE_90;
				trans.position.z = stepDepth * m_stepsInFirstSet + 1 + stairIndent;
			}
	//		AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * trans.to_mat4());

			// Steps
			for (int i = 0; i < m_stepsInSecondSet / 3; i++)
			{
				Transform trans;
				trans.position.z = 0.5f;
				trans.position.z += stepDepth * (3 * i);
				trans.position.y = stepHeight * (3 * i);
				AssetManager::DrawModel(AssetManager::s_ModelID_Staircase, shader, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * trans.to_mat4());
			}
			// Landing
		//	AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("FloorBoards"));
			m_landingFloor.Draw(shader);
		}

		// TOP TRIM
		
		Transform trans;
		trans.position.z += stairIndent;

		//AssetManager::GetModelByName("StaircaseCeilingTrimStraight")->SetMaterial(AssetManager::GetMaterialIDByName("Trims"));
		//AssetManager::GetModelByName("StaircaseCeilingTrimStraight")->Draw(shader, m_rootTransform.to_mat4() * trans.to_mat4());
		
		AssetManager::DrawModel(AssetManager::s_ModelID_StaircaseCeilingTrimStraight, shader, m_rootTransform.to_mat4() * trans.to_mat4());


		// Top door trim
		/*if (m_stepsInSecondSet > 0)
		{
			Transform topDoorTrim;
			topDoorTrim.position.x = 0.5f;
			topDoorTrim.position.y = m_stepsInSecondSet * stepHeight;
			topDoorTrim.position.z = (m_stepsInSecondSet * stepDepth) + 0.5f;
			topDoorTrim.rotation.y = ROTATE_180;
			AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * m_landingTransform.to_mat4() * topDoorTrim.to_mat4());
		}
		else
		{
			Transform topDoorTrim;
			topDoorTrim.position.x = 0.5f;
			topDoorTrim.position.y = 18 * stepHeight;
			topDoorTrim.position.z = (m_stepsInFirstSet * stepDepth) + stairIndent;
			topDoorTrim.rotation.y = ROTATE_180;
			AssetManager::GetModelByName("TrimCeiling")->Draw(shader, m_rootTransform.to_mat4() * topDoorTrim.to_mat4());
		}

		*/

		// Bottom floor
		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("FloorBoards"));
		m_bottomDoorwayFloor.Draw(shader);
		m_topDoorwayFloor.Draw(shader);

		// Ceilings
		m_bottomDoorwayCeiling.Draw(shader);

		m_ceilingMesh.Draw(shader);

		// Walls
		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("WallPaper"));
		m_wallMesh.Draw(shader);
	}
}
