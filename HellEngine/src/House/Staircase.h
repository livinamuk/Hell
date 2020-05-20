#pragma once
#include "Header.h"
#include "WallMesh.h"
#include "Floor.h"
#include "Ceiling.h"

namespace HellEngine
{
	class Staircase
	{
	public: // methods
		Staircase(glm::vec2 bottomOpening, Axis axis, int story, int firstSetCount, bool turnsLeft, bool FloorRotateBottom, bool FloorRotateTop, bool FloorRotateLanding);
		void Draw(Shader* shader);
		void Reconfigure();

	public:	// fields
		//glm::vec3 m_bottomOpening;
		WallMesh m_stairMesh;
		WallMesh m_wallMesh;
		WallMesh m_ceilingMesh;
		Floor m_landingFloor;
		Floor m_bottomDoorwayFloor;
		Floor m_topDoorwayFloor;
		Ceiling m_bottomDoorwayCeiling;
		Ceiling m_firstSetCeiling;
		Transform m_rootTransform;
		Transform m_landingTransform;
		float m_stepsInFirstSet;
		float m_stepsInSecondSet;
		bool m_turnsLeft;
		DoorWay m_topDoorway;
		DoorWay m_bottomDoorway;
		int m_story;
		Axis m_axis;

		static float trimScale;
		static float yOffset;

	private: // fields
		float stepHeight = 0.144f;
		float stepDepth = 0.15f;
		float stepWidth = 1.0f;
		float stairIndent = 0.1f;
	};
}