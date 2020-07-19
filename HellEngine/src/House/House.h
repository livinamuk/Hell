#pragma once
#include "Header.h"
#include "House/Room.h"
#include "House/Door.h"
#include "House/Staircase.h"
#include "House/Window.h"
#include "Core/Entity.h"
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
		void AddWindow(float x, float z, int story, float height, Axis axis);
		//void BuildWallMeshes();
		void RebuildAll();
		void Draw(Shader* shader, bool envMapPass);
		void DetermineWhichLightIsInWhichRoom();
		void BuildLightVolumes();
		

	public:	// fields
		std::vector<Room> m_rooms;
		std::vector<Door> m_doors;
		std::vector<Staircase> m_staircases;
		std::vector<Light> m_lights;
		std::vector<Entity> m_entities;
		std::vector<Window> m_windows;

		static House* p_house;
	};
}