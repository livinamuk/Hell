#include "hellpch.h"
#include "BloodEffect.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"

namespace HellEngine
{
	BloodEffect::BloodEffect()
	{
	}

	void BloodEffect::Init()
	{
		TransformHead = Transform{ glm::vec3(+0.036f, -0.037f, -0.128f), glm::vec3(glm::radians(0.000f), glm::radians(90.000f), glm::radians(90.000f)), glm::vec3(1.00f, 1.0f, 1.0f) };
		TransformRight = Transform{ glm::vec3(+0.674f, +0.258f, +0.008f), glm::vec3(glm::radians(41.23f), glm::radians(20.878f), glm::radians(18.627f)), glm::vec3(1.50f, 1.0f, 1.0f) };
		TransformMain = Transform{ glm::vec3(+0.710f, +0.000f, +0.044f), glm::vec3(glm::radians(90.00f), glm::radians(0.0000f), glm::radians(0.0000f)), glm::vec3(2.00f, 1.0f, 1.0f) };
		TransformLeft = Transform{ glm::vec3(+0.612f, -0.117f, -0.117f), glm::vec3(glm::radians(137.4f), glm::radians(22.758f), glm::radians(12.567f)), glm::vec3(2.00f, 1.0f, 1.0f) };
		CountRaw = 8;
		CountColumn = 4;
		AnimationSeconds = 1.0f;
		m_pQuad = std::make_unique<Quad>();
	}

	void BloodEffect::Update(float deltaTime)
	{
		//if (m_CurrentTime >= AnimationSeconds)
		//		m_CurrentTime = 0.0f;
			//m_CurrentTime = AnimationSeconds;

		auto dt = AnimationSeconds / static_cast<float>(CountRaw * CountColumn - 1);

		m_FrameIndex = std::floorf(m_CurrentTime / dt);
		m_Interpolate = (m_CurrentTime - m_FrameIndex * dt) / dt;
		m_CurrentTime += deltaTime;

	}

	void BloodEffect::Draw(Shader* shader, Transform& global)
	{
		if (m_CurrentTime >= AnimationSeconds)
			return;

		GpuProfiler g("BloodEffect");
		glDepthMask(false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader->use();
		shader->setInt("u_FrameIndex", m_FrameIndex);
		shader->setInt("u_CountRaw", CountRaw);
		shader->setInt("u_CountColumn", CountColumn);
		shader->setFloat("u_TimeLerp", m_Interpolate);
		shader->setBool("u_isBlood", true);

		Transform rot;
		rot.rotation.y = HELL_PI * 1.5f;
		rot.rotation.x = -HELL_PI / 2;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("Blood10"));
		shader->setMat4("u_MatrixWorld", global.to_mat4() * rot.to_mat4() * TransformHead.blood_mat4());
		m_pQuad->Draw();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("Blood3"));
		shader->setMat4("u_MatrixWorld", global.to_mat4() * rot.to_mat4() * TransformRight.blood_mat4());
		m_pQuad->Draw();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("Blood1"));
		shader->setMat4("u_MatrixWorld", global.to_mat4() * rot.to_mat4() * TransformMain.blood_mat4());
		m_pQuad->Draw();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("Blood2"));
		shader->setMat4("u_MatrixWorld", global.to_mat4() * rot.to_mat4() * TransformLeft.blood_mat4());
		m_pQuad->Draw();
		shader->setBool("u_isBlood", false);

		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}
