#include "hellpch.h"
#include "BloodEffectVolumetric.h"
#include "Helpers/AssetManager.h"
#include "GL/GpuProfiling.h"
#include "Logic/GlockLogic.h"//TODO access camera world position somehow else 

namespace HellEngine
{

	void BloodEffectVolumetric::Init() {
		bloodModel = std::make_unique<Model>("res/models/blood/blood1/blood_mesh.obj"); 
		bloodModel->ReadFromDisk();
		transform = Transform{ glm::vec3(+0.036f, -0.037f, -0.128f), glm::vec3(glm::radians(0.000f), glm::radians(90.000f), glm::radians(90.000f)), glm::vec3(1.00f, 1.0f, 1.0f) };
		FramesCount = 80;
		OffsetFrames = 0;
		AnimationSeconds = 3;
		m_CurrentTime = AnimationSeconds;
	}

	void BloodEffectVolumetric::Update(float deltaTime)
	{
		m_CurrentTime += deltaTime;
		float currentFrameTime = m_CurrentTime / AnimationSeconds; 
		currentFrameTime = currentFrameTime * FramesCount + OffsetFrames + 1.1f;
		timeInFrames = (ceil(-currentFrameTime) / (FramesCount + 1)) + (1.0f / (FramesCount + 1));
	}

	void BloodEffectVolumetric::Draw(Shader* shader, Transform& global)
	{
		//if (m_CurrentTime >= AnimationSeconds)
		//	return;


		GpuProfiler g("BloodEffectVolumetric");
		glDepthMask(false);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader->use();

		Transform rot;
		rot.rotation.y = HELL_PI * 1.5f;
		rot.rotation.x = -HELL_PI / 2;

		shader->setInt("u_PosTex", 0);
		shader->setInt("u_NormTex", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("blood1_pos"));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, HellEngine::AssetManager::GetTexIDByName("blood1_norm"));

		const glm::mat4 &matrix = global.to_mat4() * rot.to_mat4() * transform.blood_mat4();
		shader->setMat4("u_MatrixWorld", matrix);
		shader->setMat4("u_MatrixInverseWorld", glm::inverse(matrix));

		shader->setFloat("u_BoundingMax", 144);
		shader->setFloat("u_BoundingMin", -86.7f);
		shader->setFloat("u_Speed", 35);
		shader->setInt("u_NumOfFrames", 74);
		shader->setVec4("u_HeightOffset", glm::vec4(-77.4f, -19.48f, 27.6f, 1));

		shader->setFloat("u_TimeInFrames", timeInFrames);
		shader->setVec3("u_WorldSpaceCameraPos", GlockLogic::p_camera->m_transform.position);

		shader->setVec4("u_Color", glm::vec4(0.6f, 0, 0, 1));
		shader->setFloat("u_LightIntensity", 1.6f);
		shader->setVec3("u_SunPos", glm::vec3(0.56f, 1.07f, -0.09f));

		bloodModel->Draw(shader, matrix);

		glDepthMask(true);
		glDisable(GL_BLEND);
	}

}