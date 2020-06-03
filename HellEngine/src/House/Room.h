#pragma once
#include "Header.h"
#include "House/WallMesh.h"
#include "House/Door.h"
#include "House/Floor.h"
#include "House/Ceiling.h"
#include "House/Staircase.h"
//#include "GL/LightVolume.h"

namespace HellEngine
{
	class Room
	{
	public: // methods
		Room(glm::vec2 position, glm::vec2 size, int story, void* house);
		~Room();
		void Draw(Shader* shader);
		void Rebuild();

		void FindDoors(std::vector<Door>& doors, std::vector<Staircase>& staircases);
		void BuildWallMesh();
		void AddFloorTrim(glm::vec3 position, float rotation, float scale);
		void AddCeilingTrim(glm::vec3 position, float rotation, float scale);
		void CalculateWorldSpaceBounds();

	public:	// fields
		glm::vec2 m_position;
		glm::vec2 m_size;
		int m_story;
		WallMesh m_wallMesh;
		Floor m_floor;
		Ceiling m_ceiling;
		//LightVolume m_lightVolume;
		void* p_house;

		std::vector<Door*> m_conenctedDoors;
		std::vector<DoorWay> m_doorWaysXFrontWall;
		std::vector<DoorWay> m_doorWaysXBackWall; 
		std::vector<DoorWay> m_doorWaysZLeftWall;
		std::vector<DoorWay> m_doorWaysZRightWall;

		std::vector<Transform> m_floorTrimTransforms;
		std::vector<Transform> m_ceilingTrimTransforms;

		float m_lowerX, m_upperX, m_lowerZ, m_upperZ;
	};
}