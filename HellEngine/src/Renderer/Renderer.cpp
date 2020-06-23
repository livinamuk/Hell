#include "hellpch.h"
#include "Renderer.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Game.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
#include "Config.h"
#include "Effects/Decal.h"
#include "Logic/ShotgunLogic.h"
#include "Config.h"

namespace HellEngine
{
	Shader Renderer::s_forwardShader;
	Shader Renderer::s_solidColorShader;
	Shader Renderer::s_quadShader;
	Shader Renderer::s_geometryShader;
	Shader Renderer::s_lightingShader;
	Shader Renderer::s_compositeShader;
	Shader Renderer::s_backgroundShader;
	Shader Renderer::s_skyBoxShader;
	Shader Renderer::s_BRDF_Shader;
	Shader Renderer::s_reflection_Map_Shader;
	Shader Renderer::s_SphericalH_Harmonics_Shader;
	Shader Renderer::s_blurVerticalShader;
	Shader Renderer::s_blurHorizontalShader;
	Shader Renderer::s_DOFShader;
	Shader Renderer::s_ChromaticAberrationShader;
	Shader Renderer::s_ShadowMapShader;
	Shader Renderer::s_FXAAShader;
	Shader Renderer::s_StencilShader;
	Shader Renderer::s_BloodShader;
	Shader Renderer::s_DecalShader;

	std::string Renderer::s_debugString;

	BloodEffect Renderer::s_bloodEffect;
	MuzzleFlash Renderer::s_muzzleFlash;
	BloodWallSplatter Renderer::s_bloodWallSplatter;

	bool Renderer::s_demo = false;

	Transform Renderer::s_hitPoint;
	unsigned int Renderer::s_pointVAO;

	RenderSettings Renderer::s_RenderSettings;
	Transform Renderer::s_DebugTransform;
	Transform Renderer::s_DebugTransform2;

	float Renderer::s_polygonFactor;
	float Renderer::s_polygonUnits;

	//LightProbeGrid Renderer::s_LightProbeGrid;

	//LightProbe Renderer::s_LightProbe;
	CameraEnvMap Renderer::s_CameraEnvMap;

	GBuffer Renderer::s_gBuffer;
	FinalImageBuffer Renderer::s_FinalImageBuffer;
	LightingBuffer Renderer::s_LightingBuffer;
	DOFBuffer Renderer::s_DOFBuffer;
	FXAABuffer Renderer::s_FXAABuffer;
	ChromaticAbberationBuffer Renderer::s_ChromaticAbberationBuffer;

	std::vector<BlurBuffer> Renderer::s_BlurBuffers;

	unsigned int Renderer::brdfLUTTexture;

	bool Renderer::m_showBulletDebug = false;
	bool Renderer::m_showDebugTextures = false;
	bool Renderer::m_showImGui = false;
	bool Renderer::b_showCubemap = false;
	bool Renderer::b_renderDoorWayVolumes = true;




	void Renderer::Init()
	{
		s_forwardShader = Shader("BasicShader", "shader.vert", "shader.frag", "NONE");
		s_solidColorShader = Shader("SolidColor", "solidColor.vert", "solidColor.frag", "NONE");
		s_quadShader = Shader("QuadShader", "quadShader.vert", "quadShader.frag", "NONE");

		s_geometryShader = Shader("GeometryShader", "geometry.vert", "geometry.frag", "NONE");
		s_lightingShader = Shader("LightingShader", "lighting.vert", "lighting.frag", "NONE");
		s_compositeShader = Shader("CompositeShader", "composite.vert", "composite.frag", "NONE");
		s_ShadowMapShader = Shader("ShadowMapShader", "ShadowMapDepth.vert", "ShadowMapDepth.frag", "ShadowMapDepth.geom");
		s_DOFShader = Shader("DOFShader", "DOF.vert", "DOF.frag", "NONE");
		s_ChromaticAberrationShader = Shader("ChromaticAberration", "ChromaticAberration.vert", "ChromaticAberration.frag", "NONE");
		s_FXAAShader = Shader("FXAAShader", "FXAA.vert", "FXAA.frag", "NONE");
		s_StencilShader = Shader("StencilShader", "stencil.vert", "stencil.frag", "NONE");

		// IBL
		s_backgroundShader = Shader("Background", "background.vert", "background.frag", "NONE");
		s_BRDF_Shader = Shader("BRDF", "brdf.vert", "brdf.frag", "NONE");
		s_reflection_Map_Shader = Shader("ReflectionMapShader", "reflectionMap.vert", "reflectionMap.frag", "reflectionMap.geom");
		s_SphericalH_Harmonics_Shader = Shader("SH Shader", "SH.vert", "SH.frag", "NONE");

		// Blur
		s_blurVerticalShader = Shader("Blur V", "blurVertical.vert", "blur.frag", "NONE");
		s_blurHorizontalShader = Shader("Blur H", "blurHorizontal.vert", "blur.frag", "NONE");

		s_BloodShader = Shader("Blood", "blood.vert", "blood.frag", "NONE");
		s_DecalShader = Shader("Decal", "decals.vert", "decals.frag", "NONE");

		SetTextureBindings();

		s_gBuffer = GBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_FinalImageBuffer = FinalImageBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_LightingBuffer = LightingBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_DOFBuffer = DOFBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_ChromaticAbberationBuffer = ChromaticAbberationBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_FXAABuffer = FXAABuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2));
		s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 4, SCR_HEIGHT / 4));
		s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 8, SCR_HEIGHT / 8));
		s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 16, SCR_HEIGHT / 16));

		//s_LightProbe.Init();
		s_CameraEnvMap.Init();
		//s_LightProbeGrid.Init();

		CreateBRDFLut();

		s_bloodEffect.Init();
		s_muzzleFlash.Init();
		s_bloodWallSplatter.Init();
		
	}

	void Renderer::CreateBRDFLut()
	{
		// pbr: generate a 2D LUT from the BRDF equations used.
		unsigned int BRDFLut_Fbo;
		unsigned int BRDFLut_Rbo;
		glGenFramebuffers(1, &BRDFLut_Fbo);
		glGenRenderbuffers(1, &BRDFLut_Rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, BRDFLut_Fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, BRDFLut_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, BRDFLut_Rbo);
		glGenTextures(1, &brdfLUTTexture);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, BRDFLut_Fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, BRDFLut_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
		glViewport(0, 0, 512, 512);
		Renderer::s_BRDF_Shader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Quad2D::RenderQuad(&Renderer::s_BRDF_Shader);
		glDeleteBuffers(1, &BRDFLut_Fbo);
		glDeleteRenderbuffers(1, &BRDFLut_Rbo);
	}

	void Renderer::DrawPoint(Shader* shader, glm::vec3 position, glm::vec3 color)
	{
		static unsigned int VAO = 0;
		static unsigned int VBO;

		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
		}

		float vertices[] = {
				0,0,0, color.r, color.g, color.b

		};
		glPointSize(4);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		shader->setVec3("color", glm::vec3(1, 0, 0));
		shader->setMat4("model", Transform(position).to_mat4());
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	void Renderer::DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix = glm::mat4(1))
	{
		static unsigned int VAO = 0;
		static unsigned int VBO;

		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
		}

		float vertices[] = {
			line.start_pos.r,  line.start_pos.g,  line.start_pos.b,  line.start_color.r,  line.start_color.g,  line.start_color.b,
			line.end_pos.r,  line.end_pos.g,  line.end_pos.b,  line.end_color.r,  line.end_color.g,  line.end_color.b,
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 6);


		/*
		// REMEMBBER!!! The normal of the vertex is the colour of the line.

		static unsigned int VAO = 0;
		static unsigned int VBO;

		if (VAO == 0)
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
		}
		std::vector<glm::vec3> vertexData;
		vertexData.push_back(line.start_pos);
		vertexData.push_back(line.start_color);
		//vertexData.push_back(line.end_pos);
		vertexData.push_back(glm::vec3(1, 1, 1));
		vertexData.push_back(glm::vec3(1,1,1));
	//	vertexData.push_back(line.end_color);

		//float vertices[] = { origin.x, origin.y, origin.z, end.x, end.y, end.z};
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * 6, &vertexData[0], GL_STATIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData[0], GL_STATIC_DRAW);
		glBindVertexArray(VAO);

		glEnableVertexAttribArray(0);f
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));


		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 6);*/

	}

/*	void Renderer::DrawSkeleton(Shader* shader, SkinnedModel* skinnedModel, Transform* transform)
	{
		shader->setVec3("color", glm::vec3(1, 0, 1));

		for (Line line : skinnedMesh->m_lines)	{
			DrawLine(shader, line, transform->to_mat4());
		}

		Line x, y, z;

		float lineScale = 5;

		x.start_pos = glm::vec3(0, 0, 0);
		x.end_pos = glm::vec3(lineScale, 0, 0);
		x.start_color = glm::vec3(1, 0, 0);
		x.end_color = glm::vec3(1, 0, 0);


		y.start_pos = glm::vec3(0, 0, 0);
		y.end_pos = glm::vec3(0, lineScale, 0);
		y.start_color = glm::vec3(0, 1, 0);
		y.end_color = glm::vec3(0, 1, 0);

		z.start_pos = glm::vec3(0, 0, 0);
		z.end_pos = glm::vec3(0, 0, lineScale);
		z.start_color = glm::vec3(0, 0, 1);
		z.end_color = glm::vec3(0, 0, 1);


		for (int i = 0; i < skinnedModel->m_NumBones; i++)
		{
			glm::mat4 matrix = skinnedModel->m_BoneInfo[i].DebugMatrix_AnimatedTransform;
			DrawTangentDebugAxis(shader, transform->to_mat4() * matrix);
				
		//		DrawLine(shader, x, transform->to_mat4() * matrix);
		//	DrawLine(shader, y, transform->to_mat4() * matrix);
		//	DrawLine(shader, z, transform->to_mat4() * matrix);
		}
	}*/

	void Renderer::DrawTangentDebugAxis(Shader* shader, glm::mat4 modelMatrix)
	{
		static unsigned int VAO = 0;

		if (VAO == 0)
		{
			unsigned int VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glm::vec3 vertices[12];
			float lineScale = 5;

			vertices[0] = (glm::vec3(0, 0, 0)); // origin
			vertices[1] = (glm::vec3(1, 0, 0)); // red
			vertices[2] = (glm::vec3(lineScale, 0, 0)); // X Axis
			vertices[3] = (glm::vec3(1, 0, 0)); // red

			vertices[4] = (glm::vec3(0, 0, 0)); // origin
			vertices[5] = (glm::vec3(0, 1, 0)); // green
			vertices[6] = (glm::vec3(0, lineScale, 0)); // Y Axis
			vertices[7] = (glm::vec3(0, 1, 0)); // green

			vertices[8] = (glm::vec3(0, 0, 0)); // origin
			vertices[9] = (glm::vec3(0, 0, 1)); // green
			vertices[10] = (glm::vec3(0, 0, lineScale)); // Z Axis
			vertices[11] = (glm::vec3(0, 0, 1)); // green

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glBindVertexArray(VAO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}

		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 6);
	}

	void Renderer::DrawAnimatedEntityDebugBones_Animated(Shader* shader, AnimatedEntity* animatedEnitty)
	{
		if (s_demo)
			return;

		glm::mat4 worldMatrix = animatedEnitty->m_worldTransform.to_mat4() * animatedEnitty->m_modelTransform.to_mat4() * animatedEnitty->m_skeletonTransform.to_mat4() ;

		for (int i = 0; i < animatedEnitty->m_animatedDebugTransforms_Animated.size(); i++) 
		{
			glm::mat4 boneMatrix = animatedEnitty->m_animatedDebugTransforms_Animated[i];
		//	glm::mat4 boneMatrix = animatedEnitty->m_animatedTransforms[i];
			DrawTangentDebugAxis(shader, worldMatrix * boneMatrix);
		}
	}
	
	void Renderer::DrawAnimatedEntityDebugBones_BindPose(Shader* shader, AnimatedEntity* animatedEnitty)
	{
		if (s_demo)
			return;

		glm::mat4 worldMatrix = animatedEnitty->m_worldTransform.to_mat4() * animatedEnitty->m_modelTransform.to_mat4() * animatedEnitty->m_skeletonTransform.to_mat4();

		SkinnedModel* skinnedModel = AssetManager::skinnedModels[animatedEnitty->m_skinnedModelID];

		for (int i = 0; i < skinnedModel->m_BoneInfo.size(); i++)
		{
			glm::mat4 boneMatrix = skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose;
			DrawTangentDebugAxis(shader, worldMatrix * boneMatrix);

			glm::vec3 v = Util::TranslationFromMat4(worldMatrix * boneMatrix);
			//DrawPoint(&s_solidColorShader, v, glm::vec3(1, 1, 1));
		}
	}


	void Renderer::SetTextureBindings()
	{
		glUseProgram(s_geometryShader.ID);
		glUniform1i(glGetUniformLocation(s_geometryShader.ID, "ALB_Texture"), 0);
		glUniform1i(glGetUniformLocation(s_geometryShader.ID, "NRM_Texture"), 1);
		glUniform1i(glGetUniformLocation(s_geometryShader.ID, "RMA_Texture"), 2);

		glUseProgram(s_reflection_Map_Shader.ID);
		glUniform1i(glGetUniformLocation(s_reflection_Map_Shader.ID, "ALB_Texture"), 0);
		glUniform1i(glGetUniformLocation(s_reflection_Map_Shader.ID, "NRM_Texture"), 1);
		glUniform1i(glGetUniformLocation(s_reflection_Map_Shader.ID, "RMA_Texture"), 2);

		glUseProgram(s_lightingShader.ID);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "ALB_Texture"), 0);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "NRM_Texture"), 1);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "RMA_Texture"), 2);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "Depth_Texture"), 3);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "ShadowMap"), 4);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "Env_LUT"), 5);
		glUniform1i(glGetUniformLocation(s_lightingShader.ID, "BRDF_LUT"), 6);

		glUseProgram(s_compositeShader.ID);
		glUniform1i(glGetUniformLocation(s_compositeShader.ID, "FinalLighting_Texture"), 0);
		glUniform1i(glGetUniformLocation(s_compositeShader.ID, "Emissive_Texture"), 1);

		glUseProgram(s_backgroundShader.ID);
		glUniform1i(glGetUniformLocation(s_backgroundShader.ID, "environmentMap"), 0);
	}

	void Renderer::RenderFrame(Game* game)
	{
		EnvMapPass(game, &s_reflection_Map_Shader, &s_SphericalH_Harmonics_Shader);

		/////////////////////////////////////////////
		/// FIND OUT WHY IT'S NECCESSARY FOR EnvMapPass() TO HAVE TO RUN TWICE TO WORK.
		static bool test = false;
		if (test == false)
		{
			for (Light& light : game->house.m_lights)
			{
				light.m_LightProbe.m_needsEnvMapReRender = true;
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = true;
			}
			test = true;
		}
		EnvMapPass(game, &s_reflection_Map_Shader, &s_SphericalH_Harmonics_Shader);
		////////////////////////////////////////////

		ShadowMapPass(game, &s_ShadowMapShader);
		GeometryPass(game, &s_geometryShader);
		DecalPass(game, &s_DecalShader);
		LightingPass(game, &s_StencilShader, &s_lightingShader);
		EffectsPass(game, &s_BloodShader);
		BlurPass(&s_blurVerticalShader, &s_blurHorizontalShader);
		CompositePass(game, &s_compositeShader);
		FXAAPass(&s_FXAAShader);
		DOFPass(&s_DOFShader);
		ChromaticAberrationPass(&s_ChromaticAberrationShader);

		// Render Final image to screen
		if (!Input::s_keyDown[HELL_KEY_CAPS_LOCK])
			//RenderFinalImage(&s_quadShader, s_FinalImageBuffer.TexID);
			RenderFinalImage(&s_quadShader, s_ChromaticAbberationBuffer.TexID);
		else
			//	RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, s_gBuffer.gRMA, s_gBuffer.gFinalLighting);
			RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, game->house.m_lights[0].m_LightProbe.SH_TexID, s_gBuffer.gFinalLighting);
		//	RenderDebugTextures(&s_quadShader, s_BlurBuffers[0].textureA, s_BlurBuffers[1].textureA, s_BlurBuffers[2].textureA, s_BlurBuffers[3].textureA);


		// Bullet Debug
		if (m_showBulletDebug)
			BulletDebugDraw(game, &s_solidColorShader);

		// Show a cubemap
		/*if (Input::s_keyDown[HELL_KEY_1]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[0].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_2]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[1].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_3]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[2].m_LightProbe.CubeMap_TexID);
		}*/
		//if (Input::s_keyDown[HELL_KEY_4]) {
		//	ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[3].m_LightProbe.CubeMap_TexID);
		//}
		// Show a cubemap
		//if (Input::s_keyDown[HELL_KEY_V])
		//	ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[0].m_shadowMap.FboID);

		if (Input::s_keyDown[HELL_KEY_T]) {
			Decal::s_decals.clear();

			//		game->m_shotgunAmmo = 4;
		}

		//	}
		if (Input::s_keyDown[HELL_KEY_4]) {
			for (Light& light : game->house.m_lights)
			{
				light.m_LightProbe.m_needsEnvMapReRender = true;
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = true;
			}
		}

		TextBlitPlass(&s_quadShader);
		HUDPass(game, &s_quadShader);

		if (Input::s_keyDown[HELL_KEY_L])
		{
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			s_solidColorShader.use();
			s_solidColorShader.setMat4("projection", game->camera.m_projectionMatrix);
			s_solidColorShader.setMat4("view", game->camera.m_viewMatrix);
			s_solidColorShader.setMat4("model", glm::mat4(1));
			s_solidColorShader.setVec3("color", glm::vec3(1, 0, 0));
			//	game->house.m_lights[0].m_lightVolume.Draw(&s_solidColorShader);

			Light* light = &game->house.m_lights[0];


			/*	for (Door& door : game->house.m_doors)
				{
					AssetManager::DrawModel(AssetManager::GetModelIDByName("DoorVolumeA"), &s_solidColorShader, door.GetDoorModelMatrixFromPhysicsEngine());
				}*/

				//	for (LightVolumeDoorWay& lightVolumeDoorWay : light->m_doorWayLightVolumes)
			//		lightVolumeDoorWay.Draw(&s_solidColorShader);
		}

		//glDisable(GL_DEPTH_TEST);

		glDisable(GL_CULL_FACE);



		s_solidColorShader.use();
		s_solidColorShader.setMat4("projection", game->camera.m_projectionMatrix);
		s_solidColorShader.setMat4("view", game->camera.m_viewMatrix);

		// Render lantern guy skeleton
		Transform trans;
		trans.position = glm::vec3(1.5f, 0, 7);
		trans.rotation = glm::vec3(HELL_PI * 1.5f, HELL_PI, 0);
		trans.scale = glm::vec3(0.03f);
		//	DrawSkeleton(&s_solidColorShader, &game->m_skinnedMesh, &trans);

			// Render zombie boy skeleton
		Transform trans2;
		trans2.rotation = glm::vec3(HELL_PI * 0.5f, HELL_PI / 2, HELL_PI);
		trans2.scale = glm::vec3(0.01f);
		//DrawSkeleton(&s_solidColorShader, &game->m_ZombieBoyMesh, &trans2);

		Transform trans3;
		trans3.position = glm::vec3(1, 1, 0);
		trans3.scale = glm::vec3(0.02f);
		//	DrawSkeleton(&s_solidColorShader, &game->m_srinivasMesh, &trans3);
			//std::cout << game->m_srinivasMesh.m_lines.size() << "\n";


		glDisable(GL_DEPTH_TEST);

		//	DrawAnimatedEntityDebugBones_Animated(&s_solidColorShader, &game->m_testAnimatedEnttity);

		//	DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_testAnimatedEnttity);

		//	if (!s_demo)
		//	DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_zombieGuy);

		if (!s_demo)
			DrawAnimatedEntityDebugBones_Animated(&s_solidColorShader, &game->m_zombieGuy);

		//	DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_zombieGuy);

			//DrawSkeleton(&s_solidColorShader, AssetManager::skinnedModels[game->m_testAnimatedEnttity.m_skinnedModelID], &trans3);


	//		DrawPoint(&s_solidColorShader, ShotgunLogic::GetShotgunBarrelHoleWorldPosition(), glm::vec3(1, 1, 1));
	//		DrawPoint(&s_solidColorShader, ShotgunLogic::GetShotgunShellSpawnWorldPosition(), glm::vec3(1, 1, 1));



			// try draw ragdoll joints!!!
		if (!s_demo)
		{
			Ragdoll* ragdoll = game->m_zombieGuy.m_ragdoll;

			for (int i = 0; i < ragdoll->JOINT_COUNT; i++)
			{
				//	glm::vec3 jointWorldPos = ragdoll->GetJointWorldPosition(i);
				glm::vec3 jointWorldPos = Util::GetTranslationFromMatrix(ragdoll->GetJointWorldMatrix(i));
				DrawPoint(&s_solidColorShader, jointWorldPos, glm::vec3(1, 0, 1));
			}
		}


		glm::mat4 camMat = game->m_shotgunAnimatedEntity.GetCameraMatrix();
		std::string text = Util::Mat4ToString(camMat);

		text += "Anim index: ";
		text += std::to_string(game->m_shotgunAnimatedEntity.m_currentAnimationIndex) + "\n";
		text += "Anim time: ";
		text += std::to_string(game->m_shotgunAnimatedEntity.m_currentAnimationTime) + "\n";
		text += "Anim dur:  ";
		text += std::to_string(game->m_shotgunAnimatedEntity.m_currentAnimationDuration) + "\n";

		if (game->m_shotgunAnimatedEntity.IsAnimationComplete())
			text += "Anim commplete: TRUE\n";
		else
			text += "Anim commplete: FALSE\n";

		text += "Movement State: ";
		if (game->m_player.m_movementState == PlayerMovementState::STOPPED)
			text += "STOPPED\n";
		if (game->m_player.m_movementState == PlayerMovementState::WALKING)
			text += "WALKING\n";
		if (game->m_player.m_movementState == PlayerMovementState::RUNNING)
			text += "RUNNING\n";

		text += "Gun State: ";
		if (ShotgunLogic::m_gunState == GunState::FIRING)
			text += "FIRING\n";
		if (ShotgunLogic::m_gunState == GunState::IDLE)
			text += "IDLE\n";
		if (ShotgunLogic::m_gunState == GunState::RELOADING)
			text += "RELOADING\n";

		text += "Reload State: ";
		if (ShotgunLogic::m_reloadState == ReloadState::FROM_IDLE)
			text += "FROM_IDLE\n";
		if (ShotgunLogic::m_reloadState == ReloadState::BACK_TO_IDLE)
			text += "BACK_TO_IDLE\n";
		if (ShotgunLogic::m_reloadState == ReloadState::DOUBLE_RELOAD)
			text += "DOUBLE_RELOAD\n";
		if (ShotgunLogic::m_reloadState == ReloadState::NOT_RELOADING)
			text += "NOT_RELOADING\n";
		if (ShotgunLogic::m_reloadState == ReloadState::SINGLE_RELOAD)
			text += "SINGLE_RELOAD\n";

		text += "Iron Sight State: ";
		if (ShotgunLogic::m_ironSightState == IronSightState::BEGIN_IRON_SIGHTING)
			text += "BEGIN_IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::END_IRON_SIGHTING)
			text += "END_IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::IRON_SIGHTING)
			text += "IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
			text += "NOT_IRON_SIGHTING\n";

		text += s_debugString;

		if (Shell::s_shells.size() > 0)
		{
			/*int index = Shell::s_shells.size() - 1;
			btVector3 vel = Shell::s_shells[index].m_rigidBody->getAngularVelocity();
			text += "\n";
			text += Util::Vec3_to_String(Util::btVec3_to_glmVec3(vel));
			text += "\n";
			text += std::to_string(vel.length());*/
		}

		TextBlitter::BlitText(text, false);
		TextBlitPlass(&s_quadShader);
	}

	void Renderer::ShadowMapPass(Game* game, Shader* shader)
	{
		s_RenderSettings.ShadowMapPass = true;

		glDepthMask(true);
		glDisable(GL_BLEND);

		for (Light light : game->house.m_lights)
		{
			glViewport(0, 0, ShadowMap::SHADOW_MAP_SIZE, ShadowMap::SHADOW_MAP_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, light.m_shadowMap.FboID);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);

			shader->use();
			shader->setFloat("far_plane", SHAADOW_FAR_PLANE);
			shader->setVec3("lightPosition", light.m_position);

			for (unsigned int i = 0; i < 6; ++i)
				shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", light.m_shadowMap.m_projectionTransforms[i]);

			s_RenderSettings.BindMaterials = false;
			s_RenderSettings.DrawWeapon = false;
			s_RenderSettings.DrawLightBulbs = false;
			//glDisable(GL_CULL_FACE);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPolygonOffset(s_polygonFactor, s_polygonUnits);


			// glDepthRange(0.0, 0.9);	
			glEnable(GL_CULL_FACE);

			glCullFace(GL_FRONT);
			DrawScene(game, shader, false, false);
			glEnable(GL_CULL_FACE);

			glCullFace(GL_BACK);
			// glDepthRange(0.1, 1.0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glPolygonOffset(0, 0);

		s_RenderSettings.ShadowMapPass = false;

	}

	void Renderer::EnvMapPass(Game* game, Shader* envMapShader, Shader* sphericalHarmonicsShader)
	{
		for (Light& light : game->house.m_lights)
		{
			if (light.m_LightProbe.m_needsEnvMapReRender) {
				RenderEnvMap(game, envMapShader, &light);
				light.m_LightProbe.m_needsEnvMapReRender = false;
			}
		}

		for (Light& light : game->house.m_lights)
		{
			if (light.m_LightProbe.m_needsSphericalHarmonicsReRender) {
				RenderSphericalHarmonicsTexture(game, sphericalHarmonicsShader, &light);
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = false;
			}
		}
	}

	void Renderer::GeometryPass(Game* game, Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		glDepthMask(GL_TRUE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_RenderSettings.BindMaterials = true;
		s_RenderSettings.DrawWeapon = true;
		s_RenderSettings.DrawLightBulbs = true;

		shader->use();
		s_geometryShader.setMat4("projection", game->camera.m_projectionMatrix);
		s_geometryShader.setMat4("view", game->camera.m_viewMatrix);
		DrawScene(game, shader, true, false);

		glDepthMask(GL_FALSE);
	}

	void Renderer::EffectsPass(Game* game, Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[1] = {GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(1, attachments);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		shader->use();
		shader->setMat4("u_MatrixProjection", game->camera.m_projectionMatrix);
		shader->setMat4("u_MatrixView", game->camera.m_viewMatrix);

		SkinnedModel* model = AssetManager::skinnedModels[0];
		unsigned int BoneIndex = model->m_BoneMapping["Bolt_bone"];
		//Renderer::s_DebugTransform.position = p_camera->m_Front;
		//Renderer::s_DebugTransform2.position = p_camera->m_Right * glm::vec3(-1);
		Transform trans;
		glm::mat4 worldMatrix = trans.to_mat4() * model->m_BoneInfo[BoneIndex].FinalTransformation;
		glm::vec3 v = Util::TranslationFromMat4(worldMatrix);

		Transform t;
		t.position = s_muzzleFlash.m_worldPos;;;
		t.position = ShotgunLogic::GetShotgunBarrelHoleWorldPosition();
		t.rotation = game->camera.m_transform.rotation;

		// Blood
		s_bloodEffect.Draw(&s_BloodShader, s_hitPoint);
		s_muzzleFlash.Draw(&s_BloodShader, t);

		Transform t2;
		t.position = glm::vec3(0, 1, 0);
		t2 = s_DebugTransform;
		s_bloodWallSplatter.Draw(&s_BloodShader, t2);

		// Muzzle flash
	}

	void Renderer::DecalPass(Game* game, Shader* shader)
	{
		shader->use(); 
		shader->setMat4("pv", game->camera.m_projectionViewMatrix);
		shader->setMat4("inverseProjectionMatrix", glm::inverse(game->camera.m_projectionMatrix));
		shader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));
		shader->setFloat("screenWidth", CoreGL::s_windowWidth);
		shader->setFloat("screenHeight", CoreGL::s_windowHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gNormal);


		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		//glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (Decal decal : Decal::s_decals) {
			shader->setVec3("targetPlaneSurfaceNormal", decal.normal);
			decal.Draw(shader, true);
		}

		return;

		


		shader->setInt("writeRoughnessMetallic", 0);
		for (Decal decal : Decal::s_decals) {
			decal.Draw(shader, false);
		}
		shader->setInt("writeRoughnessMetallic", 1);
		for (Decal decal : Decal::s_decals) {
			shader->setVec3("targetPlaneSurfaceNormal", decal.normal);
			decal.Draw(shader, true);
		}

		//if (showRaycastPlane)
		//	cube.DecalDraw(shader, normal);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDepthMask(GL_FALSE);
	}

	void Renderer::LightingPass(Game* game, Shader* stencilShader, Shader* lightingShader)
	{
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		glm::mat4 VP;
		
		if (s_demo)
			VP = glm::mat4(1);

		for (Light light : game->house.m_lights)
		{
			

			// Stencil Buffer optimisation
			VP = game->camera.m_projectionMatrix * game->camera.m_viewMatrix;

			//if (!s_demo) {
				stencilShader->use();
				stencilShader->setMat4("VP", VP);
				glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
				glDrawBuffer(GL_NONE);

				glEnable(GL_STENCIL_TEST);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);


				stencilShader->setMat4("model", glm::mat4(1));

				if (light.m_roomID != -1)
				{
					// Draw main light volume (the room the light is in)
					light.m_lightVolume.Draw(stencilShader);
				
					// Draw doorway volumes into the stencil buffer
					if (b_renderDoorWayVolumes)	{
						for (LightVolumeDoorWay& lightVolumeDoorWay : light.m_doorWayLightVolumes)
						{
							// Doors, only if they aren't closed
							if (lightVolumeDoorWay.m_type == DoorWayType::DOOR)
								if (lightVolumeDoorWay.p_parentDoor->m_openStatus != DoorStatus::DOOR_CLOSED)
									lightVolumeDoorWay.Draw(stencilShader);


							// Staircase opening, well always!
							if (lightVolumeDoorWay.m_type == DoorWayType::STAIRCASE_OPENING)
								lightVolumeDoorWay.Draw(stencilShader);
						}
					}
				}


			/*	for (Door& door : game->house.m_doors)
				{
					AssetManager::DrawModel(AssetManager::GetModelIDByName("DoorVolumeA"), stencilShader, door.GetDoorModelMatrixFromPhysicsEngine());
				}*/



			/*enderer::s_RenderSettings.ShadowMapPass = true;
				Door* door = &game->house.m_doors[0];
				stencilShader->setMat4("gWVP", door->GetDoorModelMatrixFromPhysicsEngine() * MVP);
				door->Draw(stencilShader);
				Renderer::s_RenderSettings.ShadowMapPass = false;*/
				
			// Lighting from here on
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);				// FIX TO STOP JUNK DATA GLITCHES. 
			glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
			glDrawBuffer(GL_COLOR_ATTACHMENT4);


			glUseProgram(lightingShader->ID);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_gBuffer.gAlbedo);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, s_gBuffer.gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, s_gBuffer.gRMA);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);


			lightingShader->setMat4("inverseProjectionMatrix", glm::inverse(game->camera.m_projectionMatrix));
			lightingShader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));
			lightingShader->setMat4("viewMatrix", game->camera.m_viewMatrix);
			lightingShader->setFloat("screenWidth", CoreGL::s_windowWidth);
			lightingShader->setFloat("screenHeight", CoreGL::s_windowHeight);
			lightingShader->setFloat("far_plane", SHAADOW_FAR_PLANE);

			glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE); // additive blending for multiple lights. you always forget how to do this.

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, light.m_shadowMap.DepthCubeMapID);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, light.m_LightProbe.SH_TexID);

			lightingShader->setVec3("lightPosition", light.m_position);
			lightingShader->setFloat("lightAttenuationConstant", light.m_radius);
			lightingShader->setFloat("lightAttenuationExp", light.m_magic);
			lightingShader->setFloat("lightStrength", light.m_strength);
			lightingShader->setVec3("lightColor", light.m_color);

			// This is a dirty hack. To apply the shadow factor to indirect lighting outside of room bounds.
			float bias = 0;// 0.07f;
			lightingShader->setFloat("room_lowerX", game->house.m_rooms[light.m_roomID].m_lowerX - bias);
			lightingShader->setFloat("room_lowerZ", game->house.m_rooms[light.m_roomID].m_lowerZ - bias);
			lightingShader->setFloat("room_upperX", game->house.m_rooms[light.m_roomID].m_upperX + bias);
			lightingShader->setFloat("room_upperZ", game->house.m_rooms[light.m_roomID].m_upperZ + bias);
	


			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
			
			glEnable(GL_CULL_FACE);

		/*	lightingShader->setMat4("gWVP", MVP);

			if (!s_demo) {
				glCullFace(GL_FRONT);

				if (light.m_roomID != -1)
					game->house.m_rooms[light.m_roomID].m_lightVolume.Draw(lightingShader);

			}		
			else
			{
				glDisable(GL_STENCIL_TEST);
				Quad2D::RenderQuad(lightingShader);
			}
			*/
			//MVP = glm::mat4(1);

			
			Quad2D::RenderQuad(lightingShader);

			glCullFace(GL_BACK);
			glDisable(GL_BLEND);
			glDisable(GL_STENCIL_TEST);
		}
		// HOLY SHIT. THIS CAUSED THAT ANNOYING AS FUCK PROBLEM.
		// YOU CAN'T CREATE CUBEMAPS WITHOUT THIS SET BACK TO TRUE.
		glDepthMask(GL_TRUE);
	}


	void Renderer::BlurPass(Shader* blurVerticalShader, Shader* blurHorizontalShader)
	{
		// clear blur fuffer
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[0].ID);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[1].ID);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[2].ID);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[3].ID);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);

		// Source
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_gBuffer.ID);
		glReadBuffer(GL_COLOR_ATTACHMENT3);
		int factor = 2;

		// Destination
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_BlurBuffers[0].ID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		// Blit
		glBlitFramebuffer(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, 0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Blur horizontal
		glViewport(0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor);
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[0].ID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[0].textureA);
		blurHorizontalShader->use();
		blurHorizontalShader->setFloat("targetWidth", CoreGL::s_windowWidth / factor);
		Quad2D::RenderQuad(blurHorizontalShader);

		// Blur vertical
		glViewport(0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor);
		glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[0].ID);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[0].textureB);
		blurVerticalShader->use();
		blurVerticalShader->setFloat("targetHeight", CoreGL::s_windowHeight / factor);
		Quad2D::RenderQuad(blurVerticalShader);


		// second downscale //
		for (int i = 1; i < 4; i++)
		{
			factor *= 2;
			// Source
			glBindFramebuffer(GL_READ_FRAMEBUFFER, s_BlurBuffers[i - 1].ID);
			glReadBuffer(GL_COLOR_ATTACHMENT0);

			// Destination
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_BlurBuffers[i].ID);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			// Blit
			glBlitFramebuffer(0, 0, CoreGL::s_windowWidth / (factor / 2), CoreGL::s_windowHeight / (factor / 2), 0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// Blur horizontal
			glViewport(0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor);
			glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[i].ID);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[i].textureA);
			blurHorizontalShader->use();
			blurHorizontalShader->setFloat("targetWidth", CoreGL::s_windowWidth / factor);
			Quad2D::RenderQuad(blurHorizontalShader);

			// Blur vertical
			glViewport(0, 0, CoreGL::s_windowWidth / factor, CoreGL::s_windowHeight / factor);
			glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[i].ID);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[i].textureB);
			blurVerticalShader->use();
			blurVerticalShader->setFloat("targetHeight", CoreGL::s_windowWidth / factor);
			Quad2D::RenderQuad(blurVerticalShader);
		}
	}

	void Renderer::CompositePass(Game* game, Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_FinalImageBuffer.ID);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gFinalLighting);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[0].textureA);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[1].textureA);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[2].textureA);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[3].textureA);

		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		Quad2D::RenderQuad(shader);
	}

	void Renderer::FXAAPass(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_FXAABuffer.ID);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_FinalImageBuffer.TexID);

		shader->setFloat("rt_w", CoreGL::s_windowWidth);
		shader->setFloat("rt_h", CoreGL::s_windowHeight);

		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		Quad2D::RenderQuad(shader);
	}

	void Renderer::DOFPass(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_DOFBuffer.ID);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->ID);
		shader->setFloat("screenWidth", CoreGL::s_windowWidth);
		shader->setFloat("screenHeight", CoreGL::s_windowHeight);
		shader->setBool("showFocus", Config::DOF_showFocus);
		shader->setBool("vignetting", Config::DOF_vignetting);
		shader->setFloat("vignout", Config::DOF_vignout);
		shader->setFloat("vignin", Config::DOF_vignin);
		shader->setFloat("vignfade", Config::DOF_vignfade);
		shader->setFloat("CoC", Config::DOF_CoC);
		shader->setFloat("maxblur", Config::DOF_maxblur);
		shader->setInt("samples", Config::DOF_samples);
		shader->setInt("samples", Config::DOF_samples);
		shader->setInt("rings", Config::DOF_rings);
		shader->setFloat("threshold", Config::DOF_threshold);
		shader->setFloat("gain", Config::DOF_gain);
		shader->setFloat("bias", Config::DOF_bias);
		shader->setFloat("fringe", Config::DOF_fringe);

		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_FXAABuffer.TexID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		Quad2D::RenderQuad(shader);
	}

	void Renderer::ChromaticAberrationPass(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_ChromaticAbberationBuffer.ID);
		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_DOFBuffer.TexID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		shader->setFloat("time", CoreGL::GetGLTime());
		Quad2D::RenderQuad(shader);
	}

	void Renderer::TextBlitPlass(Shader* shader)
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("CharSheet"));

		TextBlitter::DrawTextBlit(shader);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}	
	
	void Renderer::HUDPass(Game* game, Shader* shader)
	{
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glColorMask(false, false, false, false);

		//glDepthMask(false);

	//	glClearColor(1, 0, 0, 1);


		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glBlendEquation(GL_FUNC_ADD);
		glActiveTexture(GL_TEXTURE0);

		shader->use();

		// Ammo
		Quad2D::RenderQuad(shader, AssetManager::GetTextureByName("HUDGradient"), 1604, 934);

		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("NumSheet"));

		std::string ammoInGun = std::to_string(ShotgunLogic::m_AmmoInGun);
		std::string ammoAvaliable = std::to_string(ShotgunLogic::m_AmmoAvaliable);

		char* cstr = new char[ammoInGun.length() + 1];
		strcpy(cstr, ammoInGun.c_str());
		char* cstr2 = new char[ammoAvaliable.length() + 1];
		strcpy(cstr2, ammoAvaliable.c_str());

		glm::vec3 ammoColor = glm::vec3(0.26f, 0.78f, 0.33f);
		if (ShotgunLogic::m_AmmoInGun == 0)
			ammoColor = glm::vec3(0.78125f, 0.3f, 0.3f);

		NumberBlitter::DrawTextBlit(shader, "/", 1700, 943);
		NumberBlitter::DrawTextBlit(shader, cstr2, 1715, 943, 0.8);
		NumberBlitter::DrawTextBlit(shader, cstr, 1695, 943, 1.0f, ammoColor, false);
	
		// Crosshair
		//int texID = AssetManager::GetTexIDByName("CrosshairCross");
		int texID = AssetManager::GetTexIDByName("CrosshairDefault");

		/*if (cameraRaycastData.distance < interactDistance)
		{
			if (cameraRaycastData.name == "DOOR"
				|| (!Door::PlayerHasKey && cameraRaycastData.name == "KEY")
				|| (cameraRaycastData.name == "NOTE"))
				texID = Texture::GetIDByName("CrosshairInteract");
		}*/


		shader->setVec3("colorTint", glm::vec3(1, 1, 1));
		int crosshairSize = 16;

		shader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		//glViewport(SCR_WIDTH / 2 - (crosshairSize / 2), SCR_HEIGHT / 2 - (crosshairSize / 2), crosshairSize, crosshairSize);
	//	Quad2D::RenderCrosshair(shader, SCR_WIDTH, SCR_HEIGHT, crosshairSize);

	//	Quad2D::RenderQuad(shader, AssetManager::GetTextureByName("NumSheet"), 1604, 934);
	//	Quad2D::RenderQuad(shader, AssetManager::GetTextureByName("NumSheet"), 960, 540);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);



		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// Reset color tint bruvva
	}

	void Renderer::RenderEnvMap(Game* game, Shader* shader, Light* light)
	{
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		Transform viewPosTransform;


		shader->use();
		shader->setInt("equirectangularMap", 0);
		shader->setMat4("projection", captureProjection);
		shader->setVec3("lightPosition", light->m_position);
		shader->setFloat("lightAttenuationConstant", light->m_radius);
		shader->setFloat("lightAttenuationExp", light->m_magic);
		shader->setFloat("lightStrength", light->m_strength);
		shader->setVec3("lightColor", light->m_color);
		shader->setVec3("viewPos", game->camera.m_viewPos);

		glViewport(0, 0, light->m_LightProbe.width, light->m_LightProbe.height);
		glBindFramebuffer(GL_FRAMEBUFFER, light->m_LightProbe.CubeMap_FBO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, light->m_LightProbe.CubeMap_TexID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		for (unsigned int i = 0; i < 6; ++i)
			shader->setMat4("captureViewMatrix[" + std::to_string(i) + "]", captureProjection * captureViews[i] * glm::inverse(Transform(light->m_position).to_mat4()));

		s_RenderSettings.BindMaterials = true;
		s_RenderSettings.DrawWeapon = false;
		s_RenderSettings.DrawLightBulbs = false;

		DrawScene(game, shader, true, true);


		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void Renderer::RenderDebugTextures(Shader* shader, unsigned int texture0_ID, unsigned int texture1_ID, unsigned int texture2_ID, unsigned int texture3_ID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0_ID);
		glViewport(0, CoreGL::s_windowHeight / 2, CoreGL::s_windowWidth / 2, CoreGL::s_windowHeight / 2);
		Quad2D::RenderQuad(shader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1_ID);
		glViewport(CoreGL::s_windowWidth / 2, CoreGL::s_windowHeight / 2, CoreGL::s_windowWidth / 2, CoreGL::s_windowHeight / 2);
		Quad2D::RenderQuad(shader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2_ID);
		glViewport(0, 0, CoreGL::s_windowWidth / 2, CoreGL::s_windowHeight / 2);
		Quad2D::RenderQuad(shader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3_ID);
		glViewport(CoreGL::s_windowWidth / 2, 0, CoreGL::s_windowWidth / 2, CoreGL::s_windowHeight / 2);
		Quad2D::RenderQuad(shader);
	}

	void Renderer::RenderFinalImage(Shader* shader, unsigned int textureID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		Quad2D::RenderQuad(shader);
	}

	void Renderer::DrawScene(Game* game, Shader* shader, bool bindTextures, bool envMapPass)
	{
		// DRAW THE HOUSE
		// walls, floors, ceilings, doors, staircases.
		game->house.Draw(shader, envMapPass);
		
		// Projectiles
		for (Shell& shell : Shell::s_shells)
			shell.Draw(shader);


		////////////////
		// ZOMBIE BOY //
		////////////////

		/*AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("ZombieBoy_Top"));
		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shell"));

		Transform trans;
		trans.rotation = glm::vec3(HELL_PI * 0.5f, HELL_PI / 2, HELL_PI);
		trans.scale = glm::vec3(0.01f);

	//	for (unsigned int i = 0; i < game->m_ZombieBoyMesh.m_animatedTransforms.size(); i++)
	//		shader->setMat4("skinningMats[" + std::to_string(i) + "]", glm::mat4(1));

		shader->setInt("hasAnimation", true);
		//game->m_ZombieBoyMesh.Render(shader, trans.to_mat4());
		shader->setInt("hasAnimation", false);

		*/
		// Animated assimp model (SkinnedMesh)
		/*{
			Transform trans;
			trans.position = glm::vec3(1.5f, 0, 7);
			trans.rotation = glm::vec3(HELL_PI * 1.5f, HELL_PI, 0);
			trans.scale = glm::vec3(0.03f);

			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Light"));

		//	for (unsigned int i = 0; i < game->m_skinnedMesh.m_animatedTransforms.size(); i++)
		//		//shader->setMat4("skinningMats[" + std::to_string(i) + "]", glm::transpose(game->m_skinnedMesh.m_animatedTransforms[i]));
		//		shader->setMat4("skinningMats[" + std::to_string(i) + "]", game->m_skinnedMesh.m_animatedTransforms[i]);

		//	shader->setInt("hasAnimation", true);
		//	game->m_skinnedMesh.Render(shader, trans.to_mat4());
		//	shader->setInt("hasAnimation", false);
		}*/

		/////////////////////////////////////////////////////////////////////
		// Srinivas Mesh
		//if (!s_demo) {	
		{
		//	Transform trans;
		//	trans.position = glm::vec3(1, 1, 0);
		//	trans.scale = glm::vec3(0.02f);
			
			
		}
		/////////////////////////////////////////////////////////////////////



		//zombie

	

		//Transform transform = s_DebugTransform;
		//glm::mat4 matrix = s_DebugTransform.to_mat4();



		game->m_zombieGuy.m_worldTransform = s_DebugTransform;
		//Util::PrintMat4(trans5.to_mat4());
	//	std::cout << "\n";
	//	Util::PrintMat4(game->m_zombieGuy.m_worldTransform.to_mat4());
	//	std::cout << "\n";*/

	//	game->m_zombieGuy.m_worldTransform.position.x = 100;
	//	std::cout << game->m_zombieGuy.m_worldTransform.position.x << "\n";

		/*s_debugString == "";
		s_debugString += "\BONES\n";

		SkinnedModel* model = AssetManager::skinnedModels[game->m_zombieGuy.m_skinnedModelID];
		glm::mat4 mat = model->m_BoneInfo[0].DebugMatrix_BindPose;

		s_debugString += Util::Mat4ToString(mat);
		
		*/

	//	game->m_zombieAnimatedEntity.SetModelScale(Config::TEST_FLOAT);
		
		SkinnedModel* skinnedModel = AssetManager::skinnedModels[game->m_zombieGuy.m_skinnedModelID];

		//for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++)
		//	shader->setMat4("skinningMats[" + std::to_string(i) + "]", skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose);
		
	//	for (unsigned int i = 0; i < skinnedModel->m_NumBones - 5; i++)
	//		shader->setMat4("skinningMats[" + std::to_string(i) + "]", skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose);


		if (!s_demo)
			game->m_zombieGuy.Draw(shader, glm::mat4(1));

	//	s_debugString += "\nmodelTransform\n";
	//	s_debugString += Util::Mat4ToString(game->m_zombieGuy.m_modelTransform.to_mat4());



		// First pesron weapon
		if (s_RenderSettings.DrawWeapon)
		{
		

			static Transform trans;
			trans.position = game->camera.m_viewPos;
			trans.rotation = game->camera.m_transform.rotation;
			trans.scale = glm::vec3(0.002f);
			glm::mat4 HUD_SHOTGUN_MATRIX = trans.to_mat4() * game->camera.m_weaponSwayTransform.to_mat4();

			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shotgun"));
			game->m_shotgunAnimatedEntity.Draw(shader, HUD_SHOTGUN_MATRIX);
		}


		// Animated test entity

		game->m_testAnimatedEnttity.m_worldTransform.position = glm::vec3(1, 1.25f, 0);
		game->m_testAnimatedEnttity.m_worldTransform.rotation = glm::vec3(0, -HELL_PI / 2, 0);
		game->m_testAnimatedEnttity.m_worldTransform.scale = glm::vec3(0.0125f);

		shader->setInt("hasAnimation", true);
		//game->m_testAnimatedEnttity.Draw(shader, glm::mat4(1));
		shader->setInt("hasAnimation", false);
	}

	void Renderer::BulletDebugDraw(Game* game, Shader* shader)
	{
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		shader->use();
		shader->setMat4("projection", game->camera.m_projectionMatrix);
		shader->setMat4("view", game->camera.m_viewMatrix);
		Physics::DebugDraw(&s_solidColorShader);
	}

	void Renderer::ViewCubeMap(Game* game, Shader* shader, unsigned int CubeMapID)
	{
		// Forward
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		shader->use();
		shader->setMat4("projection", game->camera.m_projectionMatrix);

		//glm::mat4 indentity = glm::mat4(1);
		Transform cameraTransform;
		Transform modelTransform;
		modelTransform.scale = glm::vec3(10);
		cameraTransform = game->camera.m_transform;
		cameraTransform.position = glm::vec3(0);

		glm::mat4 viewMatrix = game->camera.m_viewMatrix;
		viewMatrix[3][0] = 0;
		viewMatrix[3][1] = 0;
		viewMatrix[3][2] = 0;
		viewMatrix[0][3] = 0;
		viewMatrix[1][3] = 0;
		viewMatrix[2][3] = 0;

		shader->setMat4("model", modelTransform.to_mat4());
		shader->setMat4("view", viewMatrix);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);

		glCullFace(GL_FRONT);
		Cube cube = Cube(glm::vec3(0));
		cube.Draw(shader);
		glCullFace(GL_BACK);
	}
	void Renderer::RenderSphericalHarmonicsTexture(Game* game, Shader* shader, Light* light)
	{
		glViewport(0, 0, 3, 3);
		glBindFramebuffer(GL_FRAMEBUFFER, light->m_LightProbe.SH_FboID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader->ID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, light->m_LightProbe.CubeMap_TexID);
		Quad2D::RenderQuad(shader);
	}
}