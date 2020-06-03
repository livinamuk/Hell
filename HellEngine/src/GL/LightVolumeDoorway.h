#pragma once
#include "Header.h"
#include "House/Door.h"

namespace HellEngine
{
	class LightVolumeDoorWay
	{
	public: // methods
		LightVolumeDoorWay(DoorWay& doorWay, glm::vec3 lightPosition, float lightRadius, float roomEdge);
		//void Rebuild();
	
	//	void BuildFromRoom(Room* room);
	//	void AddCuboidToLightVolume(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
	//	void AddCubeToLightVolume(float lowerX, float upperX, float lowerY, float upperY, float lowerZ, float upperZ, float bias);

		//	LightVolume(DoorHole* doorHole, Light light);
		//	~LightVolume();
		void Draw(Shader* shader);

		void Setup();

	public: // fields
		unsigned int VAO;
		std::vector<glm::vec3> vertices;
		Door* p_parentDoor = nullptr;
		DoorWayType m_type;
		//Light* p_parentLight;

	public: // static
		//static LightVolume BuildLightVolumeFromDoorHole(DoorHole doorHole, Light light, float floorHeight, float bias);
	};
}