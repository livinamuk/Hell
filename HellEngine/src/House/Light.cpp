#include "hellpch.h"
#include "Light.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	Light::Light(glm::vec3 position)
	{
		m_position = position;
		m_modelTransform.position = position;
		Init();
	}

	Light::Light(glm::vec3 position, glm::vec3 color, float radius, float strength, float magic, Transform modelTransform)
	{
		m_position = position;
		m_modelTransform = modelTransform;
		m_radius = radius;
		m_magic = magic;
		m_strength = strength;
		m_color = color;	
		Init();
	}

	void Light::Init()
	{
		m_shadowMap.Init();
		m_shadowMap.CalculateProjectionTransforms(m_position);
		m_LightProbe.Init();
		std::cout << "INIT LIGHT\n";
	}
}