#pragma once
#include "Header.h"
#include "House/Room.h"

namespace HellEngine
{
	class LightVolume
	{
	public: // methods
		LightVolume();
		void Rebuild();
		//LightVolume(float lowerX, float lowerZ, float upperX, float upperZ);

		void BuildFromRoom(Room* room);
		void AddCuboidToLightVolume(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
		void AddCubeToLightVolume(float lowerX, float upperX, float lowerY, float upperY, float lowerZ, float upperZ, float bias);

	//	LightVolume(DoorHole* doorHole, Light light);
	//	~LightVolume();
		void Draw(Shader* shader);

		void Setup();

	public: // fields
		unsigned int VAO;
		std::vector<glm::vec3> vertices;
		//Light* p_parentLight;

	public: // static
		//static LightVolume BuildLightVolumeFromDoorHole(DoorHole doorHole, Light light, float floorHeight, float bias);
	};
}