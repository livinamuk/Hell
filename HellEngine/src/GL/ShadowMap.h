#pragma once
#include "Header.h"

namespace HellEngine
{
	class ShadowMap
	{
	public: // Methods
		ShadowMap(); 
		void CalculateProjectionTransforms(glm::vec3 position);
		void Init();

	public: // Fields
		unsigned int FboID;
		unsigned int DepthCubeMapID;
		std::vector<glm::mat4> m_projectionTransforms;
	
	public: // static fields
		static unsigned int SHADOW_MAP_SIZE;
		//static constexpr float SHADOW_MAP_NEAR_PLANE = 0.1f;
		//static constexpr float SHADOW_MAP_FAR_PLANE = 10.0f;
	};
}