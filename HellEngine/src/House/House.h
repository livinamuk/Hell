#pragma once
#include "Header.h"
#include "House/Room.h"
#include "House/Door.h"
#include "House/Staircase.h"
#include "House/Light.h"

namespace HellEngine
{
	class House
	{
	public: // methods
		House();
		~House(); 

		void AddRoom(Room room);
		void AddDoor(Door door);
		void AddLight(Light light);
		void AddStaircase(Staircase staircase);
		void BuildWallMeshes();
		void RebuildAll();
		void Draw(Shader* shader, bool envMapPass);
		void DetermineWhichLightIsInWhichRoom();
		

	public:	// fields
		std::vector<Room> m_rooms;
		std::vector<Door> m_doors;
		std::vector<Staircase> m_staircases;
		std::vector<Light> m_lights;
	};
}