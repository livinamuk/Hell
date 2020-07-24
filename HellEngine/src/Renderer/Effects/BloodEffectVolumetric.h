#pragma once

#include "Renderer/Transform.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"

namespace HellEngine
{
	class BloodEffectVolumetric
	{
	public:
		std::unique_ptr<Model> bloodModel; 
		Transform transform;
		float	AnimationSeconds; 
		float	m_CurrentTime = 0.0f;
		float   FramesCount;
		float   OffsetFrames;
		float   timeInFrames;
	public:
		void Draw(Shader* shader, Transform& global);
		void Update(float deltaTime);
		void Init();
	};
}

