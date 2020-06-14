#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Transform.h"
#include "header.h"
#include <memory>
#include "Core/Camera.h"
#include "Renderer/Texture.h"

#include "Quad.h"

namespace HellEngine
{
	class MuzzleFlash
	{
	public: // fields
		//struct CreateDesc {

		Transform TransformHead;
		Transform TransformRight;
		Transform TransformMain;
		Transform TransformLeft;
		std::shared_ptr<Texture> TextureHead;
		std::shared_ptr<Texture> TextureRight;
		std::shared_ptr<Texture> TextureMain;
		std::shared_ptr<Texture> TextureLeft;
		uint32_t                 CountRaw;
		uint32_t                 CountColumn;
		float                    AnimationSeconds;

		std::unique_ptr<Quad>   m_pQuad;

		int32_t                 m_FrameIndex = 0;
		float                   m_Interpolate = 0.0f;
		float                   m_CurrentTime = 0.0f;

	public: // methods
		MuzzleFlash();
		void Draw(Shader* shader, Transform& global);
		void Update(float deltaTime);
		void Init();
		void CreateFlash(glm::vec3 worldPosition);
		glm::vec3 m_worldPos;
	};
}