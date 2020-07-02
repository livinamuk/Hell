#include "hellpch.h"
#include "MuzzleFlash.h"
#include "Helpers/AssetManager.h"
#include "Config.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	MuzzleFlash::MuzzleFlash()
	{
	}

	void MuzzleFlash::Init()
	{
		CountRaw = 5;
		CountColumn = 4;
		AnimationSeconds = 1.0f;
		m_pQuad = std::make_unique<Quad>();
	}

	void MuzzleFlash::CreateFlash(glm::vec3 worldPosition)
	{
		m_worldPos = worldPosition;
		m_CurrentTime = 0;
	}

	void MuzzleFlash::Update(float deltaTime)
	{
		//if (m_CurrentTime >= AnimationSeconds)
		//		m_CurrentTime = 0.0f;
			//m_CurrentTime = AnimationSeconds;

		auto dt = AnimationSeconds / static_cast<float>(CountRaw * CountColumn - 1);

		m_FrameIndex = std::floorf(m_CurrentTime / dt);
		m_Interpolate = (m_CurrentTime - m_FrameIndex * dt) / dt;
		m_CurrentTime += deltaTime * 7.5f;// Config::TEST_FLOAT;

	}

	void MuzzleFlash::Draw(Shader* shader, Transform& global)
	{
		if (m_CurrentTime >= AnimationSeconds)
			return;
		GpuProfiler g("MuzzleFlash");
		glDepthMask(false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader->use();
		shader->setInt("u_FrameIndex", m_FrameIndex);
		shader->setInt("u_CountRaw", CountRaw);
		shader->setInt("u_CountColumn", CountColumn);
		shader->setFloat("u_TimeLerp", m_Interpolate);

		Transform rot;
		rot.rotation.y = HELL_PI / 2;
		rot.rotation.x = -HELL_PI / 2;

		Transform scale;
		//scale.scale = glm::vec3(Config::TEST_FLOAT, Config::TEST_FLOAT2, Config::TEST_FLOAT3);
		scale.scale = glm::vec3(0.25f, 0.125f, 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("Muzzle1"));
		shader->setMat4("u_MatrixWorld", global.to_mat4() * scale.to_mat4());
		m_pQuad->Draw();

		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}