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
		// Setup cubemaps
		m_shadowMap.Init();
		m_shadowMap.CalculateProjectionTransforms(m_position);
		m_LightProbe.Init();

		// Build the light volume
		//m_lightVolume.p_parentLight = this;
		m_lightVolume.Rebuild();

		std::cout << "INIT LIGHT\n";
	}
}