#pragma once
#include "Header.h"
#include "GL/ShadowMap.h"
#include "GL/LightProbe.h"
#include "GL/LightVolume.h"
#include "GL/LightVolumeDoorWay.h"

namespace HellEngine
{
	class Light
	{
	public: // Methods
		Light(glm::vec3 position);
		Light(glm::vec3 position, glm::vec3 color, float radius, float strength, float magic, Transform modelTransform, int modelType);
		void Init();

	public: // Fields
		glm::vec3 m_position;
		Transform m_modelTransform;
		float m_radius = 6;
		float m_magic = 4;
		float m_strength = 10.00f;
		glm::vec3 m_color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
		int m_roomID = -1;
		int m_modelType = 0;

		ShadowMap m_shadowMap;
		LightProbe m_LightProbe;
		LightVolume m_lightVolume;
		std::vector<LightVolumeDoorWay> m_doorWayLightVolumes;
	};
}