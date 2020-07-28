#pragma once
#include "Header.h"
#include "House/WallMesh.h"
#include "House/WallSegment.h"
#include "House/Door.h"
#include "House/Floor.h"
#include "House/Ceiling.h"
#include "House/Window.h"
#include "House/Staircase.h"
#include "bullet/src/btBulletCollisionCommon.h"
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

		void FindDoors(std::vector<Door>& doors, std::vector<Staircase>& staircases, std::vector<Window>& windows);
		void BuildWallMesh();
		void AddFloorTrim(glm::vec3 position, float rotation, float scale);
		void AddCeilingTrim(glm::vec3 position, float rotation, float scale);
		void CalculateWorldSpaceBounds();

		void ResetWallSegmentsAndRemoveAssociatedCollisionObjects();

		std::vector<btCollisionObject*> m_collisionObjects; // Thee are all the cubes that make up the walls.

	public:	// fields
		glm::vec2 m_position;
		glm::vec2 m_size;
		int m_story;
		WallMesh m_wallMesh;
		Floor m_floor;
		Ceiling m_ceiling;
		void* p_house;

		std::vector<WallSegment> m_wallSegments;

		std::vector<Door*> m_conenctedDoors;
		std::vector<HoleInWall> m_doorWaysXFrontWall;
		std::vector<HoleInWall> m_doorWaysXBackWall; 
		std::vector<HoleInWall> m_doorWaysZLeftWall;
		std::vector<HoleInWall> m_doorWaysZRightWall;

		std::vector<Transform> m_floorTrimTransforms;
		std::vector<Transform> m_ceilingTrimTransforms;

		float m_lowerX, m_upperX, m_lowerZ, m_upperZ;
	};
}