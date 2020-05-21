#include "hellpch.h"
#include "Renderer.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Game.h"
#include "Core/CoreGL.h"
#include "GL/Quad2D.h"
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
	
	RenderSettings Renderer::s_RenderSettings; 
	Transform Renderer::s_DebugTransform;

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

	void Renderer::Init(Game* game)
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
		LightingPass(game, &s_StencilShader, &s_lightingShader);
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
			RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, s_gBuffer.gRMA, s_gBuffer.gBuffer4);
		//	RenderDebugTextures(&s_quadShader, s_BlurBuffers[0].textureA, s_BlurBuffers[1].textureA, s_BlurBuffers[2].textureA, s_BlurBuffers[3].textureA);


		// Bullet Debug
		if (m_showBulletDebug)
			BulletDebugDraw(game, &s_solidColorShader);

		// Show a cubemap
		if (Input::s_keyDown[HELL_KEY_1]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[0].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_2]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[1].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_3]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[2].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_4]) {
			ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[3].m_LightProbe.CubeMap_TexID);
		}
		// Show a cubemap
	//	if (Input::s_keyDown[HELL_KEY_V]) {
		//	ViewCubeMap(game, &s_backgroundShader, game->house.m_lights[0].m_shadowMap.FboID);
			//ViewCubeMap(game, &s_backgroundShader, s_CameraEnvMap.CubeMap_TexID);
	//	}
		if (Input::s_keyDown[HELL_KEY_4]) {
			for (Light& light : game->house.m_lights)
			{
				light.m_LightProbe.m_needsEnvMapReRender = true; 
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = true;
			}
		}

		TextBlitPlass(&s_quadShader);


	}

	void Renderer::ShadowMapPass(Game* game, Shader* shader)
	{
		
		for (Light light : game->house.m_lights)
		{
			glViewport(0, 0, ShadowMap::SHADOW_MAP_SIZE, ShadowMap::SHADOW_MAP_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, light.m_shadowMap.FboID);
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT);

			shader->use();
			shader->setFloat("far_plane", FAR_PLANE);
			shader->setVec3("lightPosition", light.m_position);

			for (unsigned int i = 0; i < 6; ++i)
				shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", light.m_shadowMap.m_projectionTransforms[i]);

			s_RenderSettings.BindMaterials = false;
			s_RenderSettings.DrawWeapon = false;
			s_RenderSettings.DrawLightBulbs = false;
			glDisable(GL_CULL_FACE); 
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(s_polygonFactor, s_polygonUnits);
			
			DrawScene(game, shader, false, false);
			glEnable(GL_CULL_FACE);

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glPolygonOffset(0, 0);
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
		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, attachments);
		glDepthMask(GL_TRUE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable (GL_DEPTH_TEST);

		s_RenderSettings.BindMaterials = true;
		s_RenderSettings.DrawWeapon = true;
		s_RenderSettings.DrawLightBulbs = true;

		shader->use();
		s_geometryShader.setMat4("projection", game->camera.m_projectionMatrix);
		s_geometryShader.setMat4("view", game->camera.m_viewMatrix);
		DrawScene(game, shader, true, false);
	}

	void Renderer::LightingPass(Game* game, Shader* stencilShader, Shader* lightingShader)
	{
		// Clear lighting buffer
		glBindFramebuffer(GL_FRAMEBUFFER, s_LightingBuffer.ID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Light light : game->house.m_lights)
		{
			/*
			// Stencil Buffer optimisation
			glm::mat4 MVP = game->camera.m_projectionMatrix * game->camera.m_viewMatrix;
			stencilShader->use();
			stencilShader->setMat4("gWVP", MVP);
			glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
			glDrawBuffer(GL_NONE);
			glEnable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			
			if (light.m_roomID != -1)
				game->house.m_rooms[light.m_roomID].Draw(stencilShader);
			*/

			// Lighting from here on
			glBindFramebuffer(GL_FRAMEBUFFER, s_LightingBuffer.ID);
			unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
			
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
			lightingShader->setFloat("far_plane", FAR_PLANE);

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
			Quad2D::RenderQuad(lightingShader);
		}
		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
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
		glBindTexture(GL_TEXTURE_2D, s_LightingBuffer.TexID);			
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
		game->house.Draw(shader, envMapPass);


		// Animated assimp model (SkinnedMesh)
		{
			Transform trans;
			trans.position = glm::vec3(1.5f, 0, 7);
			trans.rotation = glm::vec3(HELL_PI * 0.5f, HELL_PI, 0);
			trans.scale = glm::vec3(0.03f);
			shader->setMat4("model", trans.to_mat4());

			for (unsigned int i = 0; i < game->m_animatedTransforms.size(); i++)
				shader->setMat4("skinningMats[" + std::to_string(i) + "]", glm::transpose(game->m_animatedTransforms[i]));

			shader->setInt("hasAnimation", true);
			game->m_skinnedMesh.Render();
			shader->setInt("hasAnimation", false);
		}

		/////////////////////////////////////////////////////////////////////
		// Srinivas Mesh
		{
			Transform trans;
			trans.position = glm::vec3(0, 1, 0);
			trans.scale = glm::vec3(0.03f);
			shader->setMat4("model", trans.to_mat4());
						
			for (unsigned int i = 0; i < game->m_srinivasdAnimatedTransforms.size(); i++)
				shader->setMat4("skinningMats[" + std::to_string(i) + "]", glm::transpose(game->m_srinivasdAnimatedTransforms[i]));

			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Shotgun"));
			shader->setInt("hasAnimation", true);
			game->m_srinivasMesh.Render();
			shader->setInt("hasAnimation", false);
		}
		/////////////////////////////////////////////////////////////////////

		if (s_RenderSettings.DrawWeapon)
		{
			Transform weaponTransform;
			weaponTransform.rotation.y = HELL_PI * 1.5f;
			game->m_HUDshotgun.m_transform.position = game->camera.m_viewPos;
			//game->m_HUDshotgun.m_transform.position = glm::vec3(0, 1, 0);
			//game->m_HUDshotgun.m_transform.scale = glm::vec3(2, 2, 2);
			game->m_HUDshotgun.m_transform.rotation = game->camera.m_transform.rotation;
			game->m_HUDshotgun.Draw(shader, game->m_HUDshotgun.m_transform.to_mat4() * game->m_shotgunTransform.to_mat4() * weaponTransform.to_mat4());
		}
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