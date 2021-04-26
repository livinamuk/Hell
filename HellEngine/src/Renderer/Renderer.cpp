#include "hellpch.h"
#include "Renderer.h"
#include "Helpers/Util.h"
#include "Helpers/AssetManager.h"
#include "Game.h"
#include "Core/CoreGL.h"
#include "Core/CoreImGui.h"
#include "GL/Quad2D.h"
#include "GL/GpuProfiling.h"
#include "Config.h"
#include "Effects/Decal.h"
#include "Logic/ShotgunLogic.h"
#include "Config.h"
#include "Logic/WeaponLogic.h"
#include "Logic/GlockLogic.h"
#include "Core/LevelEditor.h"
#include "Core/GameData.h"

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
	Shader Renderer::s_BloodVolumetricShader;
	Shader Renderer::s_BloodVolumetricShaderDecals;
	Shader Renderer::s_GunInspectShader;
	Shader Renderer::s_SolidColor3D;
	Shader Renderer::s_TransparentSurfacesShader;
	Shader Renderer::s_GlassBlur;
	Shader Renderer::s_CombineGlassBlurWithFinalLighting;
	Shader Renderer::s_DecalComposite;
	Shader Renderer::s_DecalTexturePaint;
	Shader Renderer::s_ScreenSpaceReflections;
	
	std::string Renderer::s_debugString;

	BloodEffect Renderer::s_bloodEffect;
	MuzzleFlash Renderer::s_muzzleFlash;
	BloodWallSplatter Renderer::s_bloodWallSplatter;

	BloodEffectVolumetric Renderer::s_bloodVolumetricEffect;

	bool Renderer::s_demo = true;

	Transform Renderer::s_hitPoint;
	unsigned int Renderer::s_pointVAO;

	RenderSettings Renderer::s_RenderSettings;
	Transform Renderer::s_DebugTransform;
	Transform Renderer::s_DebugTransform2;
	Transform Renderer::s_DebugTransform3;

	Transform Renderer::s_Inv_GlockTransform;

	float Renderer::s_polygonFactor;
	float Renderer::s_polygonUnits;

	CameraEnvMap Renderer::s_CameraEnvMap;

	GBuffer Renderer::s_gBuffer;
	FinalImageBuffer Renderer::s_FinalImageBuffer;
	LightingBuffer Renderer::s_LightingBuffer;
	DOFBuffer Renderer::s_DOFBuffer;
	FXAABuffer Renderer::s_FXAABuffer;
	ChromaticAbberationBuffer Renderer::s_ChromaticAbberationBuffer;
	CompositeBuffer Renderer::s_compositeBuffer;

	std::vector<BlurBuffer> Renderer::s_BlurBuffers;

	unsigned int Renderer::brdfLUTTexture;

	bool Renderer::m_showBulletDebug = false;
	bool Renderer::m_showDebugTextures = false;
	bool Renderer::b_showCubemap = false;
	bool Renderer::b_renderDoorWayVolumes = true;

	float Renderer::s_animTime = 0;
	

	void Renderer::Init()
	{
		s_forwardShader = Shader("BasicShader", "shader.vert", "shader.frag", "NONE");
		s_solidColorShader = Shader("SolidColor", "solidColor.vert", "solidColor.frag", "NONE");
		s_quadShader = Shader("QuadShader", "quadShader.vert", "quadShader.frag", "NONE");

		s_geometryShader = Shader("GeometryShader", "geometry.vert", "geometry.frag", "NONE");
		
		// Glass
		s_TransparentSurfacesShader = Shader("TransparentSurfaces", "TransparentSurfaces.vert", "TransparentSurfaces.frag", "NONE");
		s_GlassBlur = Shader("GlassBlur", "GlassBlur.vert", "GlassBlur.frag", "NONE"); 
		s_CombineGlassBlurWithFinalLighting = Shader("CombineGlassBlurWithFinalLighting", "CombineGlassBlurWithFinalLighting.vert", "CombineGlassBlurWithFinalLighting.frag", "NONE");

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
		
		s_BloodVolumetricShader = Shader("BloodVolumetric", "bloodVolumetric.vert", "bloodVolumetric.frag", "NONE");
		//s_BloodVolumetricShader = Shader("BloodVolumetric", "bloodVolumetric.vert", "bloodVolumetric.frag", "bloodVolumetric.geom"); 

		s_BloodVolumetricShaderDecals = Shader("BloodVolumetricDecals", "bloodVolumetricDecals.vert", "bloodVolumetricDecals.frag", "NONE");
		s_DecalComposite = Shader("DecalComposite", "DecalComposite.vert", "DecalComposite.frag", "NONE");
		
		s_GunInspectShader = Shader("GunInspect", "GunInspect.vert", "GunInspect.frag", "NONE");
		s_SolidColor3D = Shader("SolidColor3D", "solidColor3D.vert", "solidColor3D.frag", "NONE");

		s_DecalTexturePaint = Shader("DecalTexturePaint", "s_DecalTexturePaint.vert", "s_DecalTexturePaint.frag", "NONE");
		s_ScreenSpaceReflections = Shader("ScreenSpaceReflections", "ScreenSpaceReflections.vert", "ScreenSpaceReflections.frag", "NONE");
		
		SetTextureBindings();

		s_gBuffer = GBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_FinalImageBuffer = FinalImageBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_LightingBuffer = LightingBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_DOFBuffer = DOFBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_ChromaticAbberationBuffer = ChromaticAbberationBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_FXAABuffer = FXAABuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		s_compositeBuffer = CompositeBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);

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
		VolumetricBloodSplatter::Init();

		//s_bloodVolumetricEffect.Init();
	
		// Inventory gun examination transforms
		s_Inv_GlockTransform.position = glm::vec3(0, -1, -10);
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

	void Renderer::DrawTangentDebugAxis(Shader* shader, glm::mat4 modelMatrix, float lineLength)
	{
		static unsigned int VAO = 0;

		if (VAO == 0)
		{
			unsigned int VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glm::vec3 vertices[12];

			vertices[0] = (glm::vec3(0, 0, 0)); // origin
			vertices[1] = (glm::vec3(1, 0, 0)); // red
			vertices[2] = (glm::vec3(lineLength, 0, 0)); // X Axis
			vertices[3] = (glm::vec3(1, 0, 0)); // red

			vertices[4] = (glm::vec3(0, 0, 0)); // origin
			vertices[5] = (glm::vec3(0, 1, 0)); // green
			vertices[6] = (glm::vec3(0, lineLength, 0)); // Y Axis
			vertices[7] = (glm::vec3(0, 1, 0)); // green

			vertices[8] = (glm::vec3(0, 0, 0)); // origin
			vertices[9] = (glm::vec3(0, 0, 1)); // green
			vertices[10] = (glm::vec3(0, 0, lineLength)); // Z Axis
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
		//if (s_demo)
		//	return;

		glm::mat4 worldMatrix = animatedEnitty->m_worldTransform.to_mat4() * animatedEnitty->m_modelTransform.to_mat4();// *animatedEnitty->m_skeletonTransform.to_mat4();

		for (int i = 0; i < animatedEnitty->m_animatedDebugTransforms_Animated.size(); i++) 
		{
			glm::mat4 boneMatrix = animatedEnitty->m_animatedDebugTransforms_Animated[i];
			DrawTangentDebugAxis(shader, worldMatrix * boneMatrix, 0.05f);
		}

		SkinnedModel* skinnedModel = animatedEnitty->GetSkinnedModel();
		/*worldMatrix = glm::mat4(1);
		float modelScale = 1;
		glm::vec3 upperarm_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_l"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 lowerarm_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_l"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 hand_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["hand_l"]].DebugMatrix_AnimatedTransform) * modelScale;

		glm::vec3 upperarm_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["upperarm_r"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 lowerarm_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["lowerarm_r"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 hand_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["hand_r"]].DebugMatrix_AnimatedTransform) * modelScale;

		glm::vec3 thigh_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_l"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 calf_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_l"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 foot_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["foot_l"]].DebugMatrix_AnimatedTransform) * modelScale;

		glm::vec3 thigh_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_r"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 calf_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_r"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 foot_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["foot_r"]].DebugMatrix_AnimatedTransform) * modelScale;

		glm::vec3 pelvis = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["pelvis"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 spine_01 = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["spine_01"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 neck_01 = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["neck_01"]].DebugMatrix_AnimatedTransform) * modelScale;

		glm::vec3 Eye_l = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["Eye_l"]].DebugMatrix_AnimatedTransform) * modelScale;
		glm::vec3 Eye_r = Util::GetTranslationFromMatrix(worldMatrix * skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["Eye_r"]].DebugMatrix_AnimatedTransform) * modelScale;

		DrawPoint(&s_solidColorShader, upperarm_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, lowerarm_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, hand_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, upperarm_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, lowerarm_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, hand_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, thigh_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, calf_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, foot_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, thigh_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, calf_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, foot_r, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, pelvis, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, spine_01, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, neck_01, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, Eye_l, glm::vec3(1, 1, 1));
		DrawPoint(&s_solidColorShader, Eye_r, glm::vec3(1, 1, 1));
		*/


		for (int i = 0; i < skinnedModel->m_NumBones; i++)
		{
			glm::vec3 p = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[i].ModelSpace_AnimatedTransform); 
			DrawPoint(&s_solidColorShader, p, glm::vec3(1, 1, 1));
		}
	}
	
	void Renderer::DrawAnimatedEntityDebugBones_BindPose(Shader* shader, AnimatedEntity* animatedEnitty)
	{
		//if (s_demo)
		//	return;

		glm::mat4 worldMatrix = animatedEnitty->m_worldTransform.to_mat4() * animatedEnitty->m_modelTransform.to_mat4();// *animatedEnitty->m_skeletonTransform.to_mat4();
		worldMatrix = glm::mat4(1);
		SkinnedModel* skinnedModel = animatedEnitty->GetSkinnedModel();

		for (int i = 0; i < skinnedModel->m_BoneInfo.size(); i++)
		{
			glm::mat4 boneMatrix = skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose;
			DrawTangentDebugAxis(shader, worldMatrix * boneMatrix, 0.05f);

			glm::vec3 v = Util::TranslationFromMat4(worldMatrix * boneMatrix);
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
			for (Light& light : GameData::p_house->m_lights)
			{
				light.m_LightProbe.m_needsEnvMapReRender = true;
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = true;
			}
			test = true;
		}
		{
			GpuProfiler g("EnvMapPass");
			EnvMapPass(game, &s_reflection_Map_Shader, &s_SphericalH_Harmonics_Shader);
		}


		/////////////////////////
		// DECAL TEXTURE PAINT //

		DecalTexturePaint(&s_DecalComposite);



		////////////////////////////////////////////
		{
			GpuProfiler g("ShadowmapPass");
			ShadowMapPass(game, &s_ShadowMapShader);
		}
		{
			GpuProfiler g("GeometryPass");
			GeometryPass(game, &s_geometryShader);
		}

		{
			GpuProfiler g("BloodDecalPass");
			VolumetricBloodPassDecals(game, &s_BloodVolumetricShaderDecals);
		}


		//RenderProjectiles(&s_geometryShader);

		//DecalCompositePass(&s_DecalComposite);

		{
			GpuProfiler g("Volumetric Pass");
			VolumetricBloodPass(game, &s_BloodVolumetricShader);
		}


		RenderPlayerWeapon(game, &s_geometryShader);

		//RenderEnemies(game, &s_geometryShader);
		//ReRenderDoors(&s_geometryShader);

		{
			GpuProfiler g("LightingPass");
			LightingPass(game, &s_StencilShader, &s_lightingShader);
		}

		ScreenSpaceReflectionsPass(&s_ScreenSpaceReflections, game);

		{
			GpuProfiler g("GlassPass");
			GlassPass(game, &s_geometryShader);
		}

		{
			GpuProfiler g("DecalPass");
			DecalPass(game, &s_DecalShader);
		}


		{
			GpuProfiler g("EffectsPass");
			EffectsPass(game, &s_BloodShader);
		}

		{
			GpuProfiler g("EditorOverlayPass");
			EditorOverlayPass(game, &s_SolidColor3D);
		}

		{
			GpuProfiler g("BlurPass");
			BlurPass(&s_blurVerticalShader, &s_blurHorizontalShader);
		}

		{
			GpuProfiler g("CompositePass");
			CompositePass(game, &s_compositeShader);
		}

		{
			GpuProfiler g("FXAAPass");
			FXAAPass(&s_FXAAShader);
		}

		{
			GpuProfiler g("DOFPass");
			DOFPass(&s_DOFShader);
		}
		{
			GpuProfiler g("ChromaticAberrationPass");
			ChromaticAberrationPass(&s_ChromaticAberrationShader);
		}

		{

			
			//int floorDecalMap = GameData::p_house->m_rooms[0].m_floor.m_decalMapID;

			GpuProfiler g("RenderFinalImage");
			// Render Final image to screen
			if (!Input::s_keyDown[HELL_KEY_CAPS_LOCK])
				//RenderFinalImage(&s_quadShader, s_FinalImageBuffer.TexID);
				RenderFinalImage(&s_quadShader, s_ChromaticAbberationBuffer.TexID);
			else
				RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gSSR, s_gBuffer.gRMA, s_gBuffer.gFinalLighting);
		//	RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, GameData::p_house->m_entities[2].m_decalMapID, s_gBuffer.gRMA, s_gBuffer.gFinalLighting);
		//	RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, s_gBuffer.gRMA, s_gBuffer.gFinalLighting);
		//		RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, s_gBuffer.gRMA, floorDecalMap);
			//RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_compositeBuffer.gAlbedoDecalComposite, s_gBuffer.gRMA, s_compositeBuffer.gRMADecalComposite);
			//	RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, GameData::p_house->m_lights[0].m_LightProbe.SH_TexID, s_gBuffer.gFinalLighting);
		//		RenderDebugTextures(&s_quadShader, s_gBuffer.gAlbedo, s_gBuffer.gNormal, s_gBuffer.gGlassBlur, s_gBuffer.gFinalLighting);		
			//RenderDebugTextures(&s_quadShader, s_gBuffer.gNormal, s_gBuffer.gGlassSurface, s_gBuffer.gGlassBlur, s_gBuffer.gFinalLighting);
			//	RenderDebugTextures(&s_quadShader, s_BlurBuffers[0].textureA, s_BlurBuffers[1].textureA, s_BlurBuffers[2].textureA, s_BlurBuffers[3].textureA);
		}


		




		static bool white = false;

		if (Input::s_keyPressed[HELL_KEY_Q])
			white = !white;

		if (white) {
			glClearColor(1, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		// Bullet Debug
		if (m_showBulletDebug)
		{
			GpuProfiler g("Bullet debug pass");
			BulletDebugDraw(game, &s_solidColorShader);
		}
	

		// Show a cubemap
		/*if (Input::s_keyDown[HELL_KEY_1]) {
			ViewCubeMap(game, &s_backgroundShader, GameData::p_house->m_lights[0].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_2]) {
			ViewCubeMap(game, &s_backgroundShader, GameData::p_house->m_lights[1].m_LightProbe.CubeMap_TexID);
		}
		if (Input::s_keyDown[HELL_KEY_3]) {
			ViewCubeMap(game, &s_backgroundShader, GameData::p_house->m_lights[2].m_LightProbe.CubeMap_TexID);
		}*/
		//if (Input::s_keyDown[HELL_KEY_4]) {
		//	ViewCubeMap(game, &s_backgroundShader, GameData::p_house->m_lights[3].m_LightProbe.CubeMap_TexID);
		//}
		// Show a cubemap
		if (Input::s_keyDown[HELL_KEY_V])
			ViewCubeMap(game, &s_backgroundShader, GameData::p_house->m_lights[3].m_shadowMap.DepthCubeMapID);

		if (Input::s_keyDown[HELL_KEY_T]) {
			Decal::s_decals.clear();
			Shell::s_bulletCasings.clear();
			Shell::s_shotgunShells.clear();
			VolumetricBloodSplatter::s_volumetricBloodSplatters.clear();

			//		game->m_shotgunAmmo = 4;
		}

		//	}
		if (Input::s_keyDown[HELL_KEY_4]) {
			for (Light& light : GameData::p_house->m_lights)
			{
				light.m_LightProbe.m_needsEnvMapReRender = true;
				light.m_LightProbe.m_needsSphericalHarmonicsReRender = true;
			}
		}

//<<<<<<< HEAD
		TextBlitPlass(&s_quadShader);
		HUDPass(game, &s_quadShader);
		//InventoryPass(game);
//=======
		{
//			GpuProfiler g("TextBlitPlass");
			TextBlitPlass(&s_quadShader);
		}
		{
//			GpuProfiler g("HUDPass");
			HUDPass(game, &s_quadShader);
		}
//>>>>>>> 0c1086a1dabd1dd4b0834f9bb71574f2820126da

		if (Input::s_keyDown[HELL_KEY_L])
		{
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			s_solidColorShader.use();
			s_solidColorShader.setMat4("projection", game->camera.m_projectionMatrix);
			s_solidColorShader.setMat4("view", game->camera.m_viewMatrix);
			s_solidColorShader.setMat4("model", glm::mat4(1));
			s_solidColorShader.setVec3("color", glm::vec3(1, 0, 0));
			//	GameData::p_house->m_lights[0].m_lightVolume.Draw(&s_solidColorShader);

			Light* light = &GameData::p_house->m_lights[0];


			/*	for (Door& door : GameData::p_house->m_doors)
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




		// RENDER ALL BLOOD SPLATTERS
		/*for (VolumetricBloodSplatter& volumetricBloodSplater : game->s_volumetricBloodSplatters) 
		{	
			// debug line
			Line line;
			line.start_pos = volumetricBloodSplater.m_transform.position;
			line.end_pos = volumetricBloodSplater.m_transform.position + volumetricBloodSplater.m_front * glm::vec3(0.2);
			line.start_color = glm::vec3(1);
			line.end_color = glm::vec3(1);

			s_solidColorShader.use();
			s_solidColorShader.setMat4("projection", game->camera.m_projectionMatrix);
			s_solidColorShader.setMat4("view", game->camera.m_viewMatrix);

			glm::vec3 position = volumetricBloodSplater.m_transform.position;
			Transform transform;
			transform.position = position;

			DrawLine(&s_solidColorShader, line, glm::mat4(1));
		}*/

		//	DrawAnimatedEntityDebugBones_Animated(&s_solidColorShader, &game->m_testAnimatedEnttity);

		//	DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_testAnimatedEnttity);

		//	if (!s_demo)
		//	DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_zombieGuy);

		//DrawAnimatedEntityDebugBones_Animated(&s_solidColorShader, &game->m_NurseGuy);
		//DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_NurseGuy);

		//if (!s_demo)
		{
			GpuProfiler g("DrawAnimatedEntityDebugBones_Animated");
			// RAGDOLL TESTING DrawAnimatedEntityDebugBones_Animated(&s_solidColorShader, &game->m_zombieGuy);
		}

		//
	
		for (int i = 0; i < game->m_NurseGuy.m_animatedDebugTransforms_Animated.size(); i++)
		{
			glm::mat4 boneMatrix = game->m_NurseGuy.m_animatedDebugTransforms_Animated[i];
			DrawTangentDebugAxis(&s_solidColorShader, game->m_NurseGuy.m_worldTransform.to_mat4() * boneMatrix, 0.05f);
			DrawPoint(&s_solidColorShader, Util::GetTranslationFromMatrix(game->m_NurseGuy.m_worldTransform.to_mat4() * boneMatrix), glm::vec3(1, 0, 0));
		}



		// draw ragdoll joints
	/*for (int i = 0; i < Ragdoll::JOINT_COUNT; i++)
	{
		glm::vec3 p = game->m_zombieGuy.m_ragdoll->GetJointWorldPosition(i);
	//	glm::vec3 p2 = game->m_zombieGuy.m_ragdoll->GetJointWorldPositionA(i);
	//	DrawPoint(&s_solidColorShader, p, glm::vec3(1, 0, 1));
	//	DrawPoint(&s_solidColorShader, p2, glm::vec3(0, 1, 1));
	}*/



	// volumetric blood point
	//	DrawPoint(&s_solidColorShader, s_DebugTransform.position, glm::vec3(1, 1, 1));
	//	DrawPoint(&s_solidColorShader, Config::BLOOD_MESH_POSITION, glm::vec3(1, 1, 1));
		

	for (int i = 0; i < Ragdoll::BODYPART_COUNT; i++)
	{
		glm::mat4 rot = glm::mat4_cast(Config::TEST_QUAT);
		glm::mat4 rot2 = glm::mat4_cast(Config::TEST_QUAT2);


		/* old ragdoll
		glm::mat4 m = game->m_zombieGuy.m_ragdoll->Get_Top_Joint_World_Matrix(i);
		glm::mat4 m2 = game->m_zombieGuy.m_ragdoll->Get_Bottom_Joint_World_Matrix(i);
		*/
		

	//	WERE USING THIS DrawPoint(&s_solidColorShader, Util::GetTranslationFromMatrix(m), glm::vec3(1, 0, 0));
	// WERE USING THIS	DrawPoint(&s_solidColorShader, Util::GetTranslationFromMatrix(m2), glm::vec3(0, 1, 0));
	}

	/*
	for (int i = 0; i < Ragdoll::BODYPART_COUNT; i++)
	{
		glm::mat4 rot = glm::mat4_cast(Config::TEST_QUAT);
		glm::mat4 rot2 = glm::mat4_cast(Config::TEST_QUAT2);

		glm::mat4 m = game->m_zombieGuy.m_ragdoll->Get_Bottom_Joint_World_Matrix(i);
		DrawPoint(&s_solidColorShader, Util::GetTranslationFromMatrix(m), glm::vec3(0, 0, 1));
		DrawTangentDebugAxis(&s_solidColorShader, m, 0.025f);
	}*/
	

//	btTransform trann;
//	game->m_zombieGuy.m_ragdoll->m_bodies[Ragdoll::BODYPART_RIGHT_LOWER_ARM]->getMotionState()->getWorldTransform(trann);
//	glm::vec3 p = Util::btVec3_to_glmVec3(trann.getOrigin());
//	DrawPoint(&s_solidColorShader, p, glm::vec3(0, 1, 1));

	SkinnedModel* skinnedModel = game->m_zombieGuy.GetSkinnedModel();

	/*for (int i = 0; i < skinnedModel->m_NumBones; i++)
	{
		glm::mat4 m = skinnedModel->m_BoneInfo[i].NodeTransformation;
		DrawTangentDebugAxis(&s_solidColorShader, m, 0.05f);
	}*/
	
	// find magic matrix
	
	/* OLD RAGDOLL Ragdoll* ragdoll = game->m_zombieGuy.m_ragdoll;
	{
		btGeneric6DofConstraint* constraint = ragdoll->m_joints[Ragdoll::JOINT_RIGHT_SHOULDER];
		constraint->calculateTransforms();
		btTransform transformA = constraint->getCalculatedTransformA();
		btTransform transformB = constraint->getCalculatedTransformB();

		btVector3 pos = transformB.getOrigin();
		//btQuaternion rot = transform.getRotation() * btQuaternion(Config::TEST_QUAT2.x, Config::TEST_QUAT2.y, Config::TEST_QUAT2.z, Config::TEST_QUAT2.w) ;
		btQuaternion rot = transformB.getRotation() * btQuaternion(0, 1, 0, 0);

		glm::vec3 posGL = glm::vec3(pos.x(), pos.y(), pos.z());
		glm::quat rotQL = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());

		glm::mat4 m = glm::translate(glm::mat4(1), posGL);
		m *= glm::mat4_cast(rotQL);

		//DrawTangentDebugAxis(&s_solidColorShader, m, 0.025f);
	}
	{
		btGeneric6DofConstraint* constraint = ragdoll->m_joints[Ragdoll::JOINT_RIGHT_ELBOW];
		constraint->calculateTransforms();
		btTransform transformA = constraint->getCalculatedTransformA();
		btTransform transformB = constraint->getCalculatedTransformB();

		btVector3 pos = transformB.getOrigin();
		//btQuaternion rot = transformB.getRotation() * btQuaternion(Config::TEST_QUAT2.x, Config::TEST_QUAT2.y, Config::TEST_QUAT2.z, Config::TEST_QUAT2.w) ;
		btQuaternion rot = transformB.getRotation() * btQuaternion(0.7071, -0.7071, 0, 0);

		glm::vec3 posGL = glm::vec3(pos.x(), pos.y(), pos.z());
		glm::quat rotQL = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());

		glm::mat4 m = glm::translate(glm::mat4(1), posGL);
		m *= glm::mat4_cast(rotQL);

		//DrawTangentDebugAxis(&s_solidColorShader, m, 0.025f);
	}

	

	btTransform trann;
	game->m_zombieGuy.m_ragdoll->m_bodies[Ragdoll::BODYPART_RIGHT_UPPER_LEG]->getMotionState()->getWorldTransform(trann);
	glm::vec3 p = Util::btVec3_to_glmVec3(trann.getOrigin());

	*/

	//glm::vec3 p2 = game->m_zombieGuy.m_ragdoll->GetJointWorldPositionA(i);
	//DrawPoint(&s_solidColorShader, p, glm::vec3(1, 1, 0));
	//	DrawPoint(&s_solidColorShader, p2, glm::vec3(0, 1, 1));

	//SkinnedModel* skinnedModel = AssetManager::skinnedModels[game->m_zombieGuy.m_skinnedModelID];
	glm::vec3 thigh_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["thigh_r"]].ModelSpace_AnimatedTransform);
	glm::vec3 calf_r = Util::GetTranslationFromMatrix(skinnedModel->m_BoneInfo[skinnedModel->m_BoneMapping["calf_r"]].ModelSpace_AnimatedTransform);

	float lA = glm::length(thigh_r - calf_r);
//	std::cout << "lengthA: " << lA << "\n";


	/*glm::vec3 jointTOP = game->m_zombieGuy.m_ragdoll->GetJointWorldPosition(Ragdoll::JOINT_RIGHT_HIP);
	glm::vec3 jointBOTTOM = game->m_zombieGuy.m_ragdoll->GetJointWorldPosition(Ragdoll::JOINT_RIGHT_KNEE);
	lA = glm::length(jointTOP - jointBOTTOM);
	std::cout << "lengthB: " << lA << "\n\n";

	glm::vec3 p3 = Util::btVec3_to_glmVec3(Util::GetRelPosBetween2Vectors(thigh_r, calf_r));
	glm::vec3 p4 = Util::btVec3_to_glmVec3(Util::GetRelPosBetween2Vectors(jointTOP, jointBOTTOM));

	std::cout << "p3: " << Util::Vec3_to_String(p3) << "\n";
	std::cout << "p4: " << Util::Vec3_to_String(p4) << "\n";
	std::cout << "bd: " << Util::Vec3_to_String(p) << "\n\n";

	DrawPoint(&s_solidColorShader, p3, glm::vec3(0, 1, 0));
	DrawPoint(&s_solidColorShader, p4, glm::vec3(0, 0, 1));
	//DrawPoint(&s_solidColorShader, p, glm::vec3(1, 0, 0)); // same as green

	DrawPoint(&s_solidColorShader, jointTOP, glm::vec3(1, 0, 0));
	DrawPoint(&s_solidColorShader, p3, glm::vec3(0, 1, 0));
	*/
	//glm::vec3 pevls = game->m_zombieGuy.m_ragdoll->GetJointWorldPosition(Ragdoll::JOINT_PELVIS_SPINE);
	//DrawPoint(&s_solidColorShader, pevls, glm::vec3(1, 1, 0));

	//glm::vec3 JOINT_RIGHT_KNEE = game->m_zombieGuy.m_ragdoll->GetJointWorldPosition(Ragdoll::JOINT_RIGHT_KNEE);
	//DrawPoint(&s_solidColorShader, JOINT_RIGHT_KNEE, glm::vec3(0, 1, 0));

			//DrawAnimatedEntityDebugBones_BindPose(&s_solidColorShader, &game->m_zombieGuy);

			//DrawSkeleton(&s_solidColorShader, AssetManager::skinnedModels[game->m_testAnimatedEnttity.m_skinnedModelID], &trans3);
	//DrawPoint(&s_solidColorShader, GlockLogic::GetGlockCasingSpawnWorldPosition(), glm::vec3(1, 1, 1));
	
	//		DrawPoint(&s_solidColorShader, ShotgunLogic::GetShotgunBarrelHoleWorldPosition(), glm::vec3(1, 1, 1));
	//		DrawPoint(&s_solidColorShader, ShotgunLogic::GetShotgunShellSpawnWorldPosition(), glm::vec3(1, 1, 1));
	
//	if (WeaponLogic::m_singleHanded)
//		DrawPoint(&s_solidColorShader, GlockLogic::GetGlockCasingSpawnWorldPosition(), glm::vec3(1, 1, 1));

			// try draw ragdoll joints!!!
		


		/*for (Vertex& vertex : GameData::p_house->m_rooms[0].m_wallMesh.vertices)
		{
			DrawPoint(&s_solidColorShader, vertex.Position, glm::vec3(1, 0, 1));
		}
*/
	
		// DRAW POINT AT ZERO
		//DrawPoint(&s_solidColorShader, glm::vec3(0), glm::vec3(1, 1, 1));

		//glm::mat4 camMat = game->m_shotgunAnimatedEntity.GetCameraMatrix();
		std::string text = "";// Util::Mat4ToString(camMat);


		text = "FPS: ";
		text += std::to_string(game->m_fps);

		text += "\nSENSITIVITY: ";
		text += std::to_string(Config::MOUSE_SESNSITIVITY);
		text += "\n";

		//text += "\n CAMERA RAY CAST: ";
		//text += Util::PhysicsObjectEnumToString(game->m_cameraRaycast.m_objectType);
				

	/*	for (VolumetricBloodSplatter splatter : VolumetricBloodSplatter::s_volumetricBloodSplatters)
		{
			text += "\n";
			text += Util::Mat4ToString(splatter.GetDecalModelMatrix());
		}*/

	//	text += "GameData::p_house->m_rooms.size(): ";
	//	text += std::to_string(GameData::p_house->m_rooms.size());
		////text += "GameData::p_house->m_rooms[0].m_wallMesh.vertices.size(): ";
		//text += std::to_string(GameData::p_house->m_rooms[0].m_wallMesh.vertices.size());
		//text += "\n";
		
	/*	text += "Anim index: ";
		text += std::to_string(WeaponLogic::p_currentAnimatedEntity->m_currentAnimationIndex) + "\n";
		text += "Anim time: ";
		text += std::to_string(WeaponLogic::p_currentAnimatedEntity->m_currentAnimationTime) + "\n";
		text += "Anim dur:  ";
		text += std::to_string(WeaponLogic::p_currentAnimatedEntity->m_currentAnimationDuration) + "\n";

		text += "Current Weapon: ";
		text += std::to_string(WeaponLogic::s_SelectedWeapon) + "\n";
		text += "Desired Weapon: ";
		text += std::to_string(WeaponLogic::s_desiredWeapon) + "\n";

		if (WeaponLogic::p_currentAnimatedEntity->IsAnimationComplete())
			text += "Anim commplete: TRUE\n";
		else
			text += "Anim commplete: FALSE\n";

		text += "Fire animation: ";
		text += std::to_string(GlockLogic::s_RandomFireAnimation) + "\n";


		text += "Movement State: ";
		if (game->m_player.m_movementState == PlayerMovementState::STOPPED)
			text += "STOPPED\n";
		if (game->m_player.m_movementState == PlayerMovementState::WALKING)
			text += "WALKING\n";
		if (game->m_player.m_movementState == PlayerMovementState::RUNNING)
			text += "RUNNING\n";

		text += "Gun State: ";
		if (WeaponLogic::p_gunState == GunState::FIRING)
			text += "FIRING\n";
		if (WeaponLogic::p_gunState == GunState::IDLE)
			text += "IDLE\n";
		if (WeaponLogic::p_gunState == GunState::RELOADING)
			text += "RELOADING\n";
		if (WeaponLogic::p_gunState == GunState::EQUIP)
			text += "EQUIP\n";
		if (WeaponLogic::p_gunState == GunState::DEQUIP)
			text += "DEQUIP\n";

		text += "Reload State: ";
		if (WeaponLogic::p_reloadState == ReloadState::FROM_IDLE)
			text += "FROM_IDLE\n";
		if (WeaponLogic::p_reloadState == ReloadState::BACK_TO_IDLE)
			text += "BACK_TO_IDLE\n";
		if (WeaponLogic::p_reloadState == ReloadState::DOUBLE_RELOAD)
			text += "DOUBLE_RELOAD\n";
		if (WeaponLogic::p_reloadState == ReloadState::NOT_RELOADING)
			text += "NOT_RELOADING\n";
		if (WeaponLogic::p_reloadState == ReloadState::SINGLE_RELOAD)
			text += "SINGLE_RELOAD\n";
		if (WeaponLogic::p_reloadState == ReloadState::RELOAD_CLIP)
			text += "RELOAD_CLIP\n";
		if (WeaponLogic::p_reloadState == ReloadState::RELOAD_CLIP_FROM_EMPTY)
			text += "RELOAD_CLIP_FROM_EMPTY\n";

		/*text += "Iron Sight State: ";
		if (ShotgunLogic::m_ironSightState == IronSightState::BEGIN_IRON_SIGHTING)
			text += "BEGIN_IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::END_IRON_SIGHTING)
			text += "END_IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::IRON_SIGHTING)
			text += "IRON_SIGHTING\n";
		if (ShotgunLogic::m_ironSightState == IronSightState::NOT_IRON_SIGHTING)
			text += "NOT_IRON_SIGHTING\n";*/

	


			
		/*
		text += "Weapon: ";
		text += std::to_string(WeaponLogic::s_SelectedWeapon);
		text += "\n";*/

		//s_debugString = Util::Mat4ToString(WeaponLogic::s_AnimatedCameraMatrix);
	//	text += s_debugString;

		TextBlitter::BlitText(text, false);
		{
			GpuProfiler g("TextBlitPlass");
			TextBlitPlass(&s_quadShader);
		}
	}

	void Renderer::ShadowMapPass(Game* game, Shader* shader)
	{
		s_RenderSettings.ShadowMapPass = true;

		glDepthMask(true);
		glDisable(GL_BLEND);

		for (Light light : GameData::p_house->m_lights)
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
		//	glEnable(GL_POLYGON_OFFSET_FILL);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//	glPolygonOffset(s_polygonFactor, s_polygonUnits);


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
		for (Light& light : GameData::p_house->m_lights)
		{
			if (light.m_LightProbe.m_needsEnvMapReRender) {
				RenderEnvMap(game, envMapShader, &light);
				light.m_LightProbe.m_needsEnvMapReRender = false;
			}
		}

		for (Light& light : GameData::p_house->m_lights)
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
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(5, attachments);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_RenderSettings.BindMaterials = true;
		s_RenderSettings.DrawWeapon = true;
		s_RenderSettings.DrawLightBulbs = true;

		shader->use();
		s_geometryShader.setMat4("projection", game->camera.m_projectionMatrix);
		s_geometryShader.setMat4("view", game->camera.m_viewMatrix);
		DrawScene(game, shader, true, false);


	/*	Transform transform;// = Renderer::s_DebugTransform;
		transform.position.y = 1;
		transform.scale = glm::vec3(0.0001f);

		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("White"));

		Model* blood_mesh = AssetManager::GetModelByName("blood_mesh");
		//blood_mesh->Draw(shader, transform.to_mat4());

		int VAO = blood_mesh->m_meshes[0]->VAO;
		int numIndices = blood_mesh->m_meshes[0]->indices.size();
		int numVerts = blood_mesh->m_meshes[0]->vertices.size();

		s_geometryShader.setMat4("model", transform.to_mat4());

		glBindVertexArray(VAO);
		glPointSize(4);
		//glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_POINTS, (GLsizei)numIndices, GL_UNSIGNED_INT, 0);
		*/
		//	glDrawArrays(GL_POINTS, 0, (GLsizei)numIndices);

		//	glDrawArrays(GL_POINTS, 0, 1);

		glDepthMask(GL_FALSE);
	}

	void Renderer::EffectsPass(Game* game, Shader* shader)
	{
		//return;
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
		shader->setVec3("u_ViewPos", game->camera.m_viewPos);

		for (int i = 0; i < GameData::p_house->m_lights.size(); i++)
		{
			Light& light = GameData::p_house->m_lights[i];
			shader->setVec3("lightPosition[" + std::to_string(i) + "]", light.m_position);
			shader->setFloat("lightRadius[" + std::to_string(i) + "]", light.m_radius);
			shader->setFloat("lightMagic[" + std::to_string(i) + "]", light.m_magic);
			shader->setFloat("lightStrength[" + std::to_string(i) + "]", light.m_strength);
			shader->setVec3("lightColor[" + std::to_string(i) + "]", light.m_color);
		}

		SkinnedModel* model = AssetManager::skinnedModels[AssetManager::GetSkinnedModelIDByName("Shotgun.fbx")];
		unsigned int BoneIndex = model->m_BoneMapping["Bolt_bone"];
		//Renderer::s_DebugTransform.position = p_camera->m_Front;
		//Renderer::s_DebugTransform2.position = p_camera->m_Right * glm::vec3(-1);
		Transform trans;
		glm::mat4 worldMatrix = trans.to_mat4() * model->m_BoneInfo[BoneIndex].FinalTransformation;
		glm::vec3 v = Util::TranslationFromMat4(worldMatrix);

		Transform t;
		t.position = s_muzzleFlash.m_worldPos;
		t.position = WeaponLogic::GetBarrelHoleWorldPosition();
		t.rotation = game->camera.m_transform.rotation;


		// Blood
		//s_bloodEffect.Draw(&s_BloodShader, s_hitPoint);
		s_muzzleFlash.Draw(&s_BloodShader, t);

		Transform t2;
		t.position = glm::vec3(0, 1, 0);
		t2 = s_DebugTransform;
		s_bloodWallSplatter.Draw(&s_BloodShader, t2);

		//s_bloodVolumetricEffect.Draw(&s_BloodShader, s_hitPoint);

		// Muzzle flash
	}


	void Renderer::VolumetricBloodPass(Game* game, Shader* shader)
	{
		s_animTime += game->m_frameTime;

		if (Input::s_keyPressed[HELL_KEY_U])
			s_animTime = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glCullFace(GL_BACK);

		AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("White"));

		shader->use();
		shader->setMat4("u_MatrixProjection", game->camera.m_projectionMatrix);
		shader->setMat4("u_MatrixView", game->camera.m_viewMatrix);
		shader->setVec3("u_WorldSpaceCameraPos", game->camera.m_viewPos);

	//	shader->setMat4("debugMatrix", s_DebugTransform.to_mat4());

		// RENDER ALL BLOOD SPLATTERS
		//for (VolumetricBloodSplatter& volumetricBloodSplater : game->s_volumetricBloodSplatters) {
		//	volumetricBloodSplater.Draw(shader);
		//}

		for (int i = 0; i < VolumetricBloodSplatter::s_volumetricBloodSplatters.size(); i++)
		{
			if (i == 0) {
		//		game->s_volumetricBloodSplatters[0].m_transform.position = glm::vec3(0);
		//		game->s_volumetricBloodSplatters[0].m_transform.rotation = glm::vec3(0);

			}
			VolumetricBloodSplatter::s_volumetricBloodSplatters[i].Draw(shader);
		}
	}

	void Renderer::VolumetricBloodPassDecals(Game* game, Shader* shader)
	{
		/*static const float clearcolor[] = { 0, 0, 1, 1 };
		glClearTexImage(s_gBuffer.gBloodDecals, 0, GL_RGBA, GL_FLOAT, clearcolor);
		int scale = 2;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_gBuffer.ID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_gBuffer.ID);
		glDrawBuffer(GL_COLOR_ATTACHMENT7);
		glBlitFramebuffer(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, 0, 0, CoreGL::s_windowWidth / scale, CoreGL::s_windowHeight / scale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			*/

/*		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_gBuffer.ID);
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_gBuffer.ID);
		glDrawBuffer(GL_COLOR_ATTACHMENT7);
		glBlitFramebuffer(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, CoreGL::s_windowWidth / scale, 0, CoreGL::s_windowWidth / scale + CoreGL::s_windowWidth / scale, CoreGL::s_windowHeight / scale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
*/
		float scale = 1;
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth / scale, CoreGL::s_windowHeight / scale);

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
	//	glActiveTexture(GL_TEXTURE6);
	//	glBindTexture(GL_TEXTURE_2D, s_gBuffer.gBloodDecals);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask7"));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask6"));
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask9"));
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask4"));



		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2 };
		//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT7 };
		//unsigned int attachments[1] = {  GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(2, attachments);

		glDepthMask(GL_FALSE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
		glBlendEquation(GL_FUNC_ADD);
		glEnablei(GL_BLEND, 1);
		glBlendFunci(1, GL_DST_COLOR, GL_SRC_COLOR);
		glDisablei(GL_BLEND, 1);

		//glDisable(GL_BLEND);
		//	

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

		glCullFace(GL_FRONT);
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_DEPTH_TEST);

		// RENDER ALL BLOOD SPLATTERS
		for (VolumetricBloodSplatter& volumetricBloodSplater : VolumetricBloodSplatter::s_volumetricBloodSplatters) {
			volumetricBloodSplater.DrawDecal(shader);
		}

		/*if (GameData::p_house->m_rooms.size()) 
		{
			int floorDecalMap = GameData::p_house->m_rooms[0].m_floor.m_decalMapID;
			static const float clearcolor[] = { 0, 0, 1, 1 };
			glClearTexImage(floorDecalMap, 0, GL_RGBA, GL_FLOAT, clearcolor);
		}*/
	}

	void Renderer::RenderPlayerWeapon(Game* game, Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,  GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(5, attachments);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		shader->use();
		shader->setBool("blockoutDecals", true);
		WeaponLogic::RenderCurrentWeapon(shader, &game->camera);
		shader->setBool("blockoutDecals", false);

		glDepthMask(GL_FALSE);
	}

	/*void Renderer::RenderEnemies(Game* game, Shader* shader)
	{
			glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
			glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

			unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,  GL_COLOR_ATTACHMENT7 };
			glDrawBuffers(5, attachments);

			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			shader->use();
			shader->setBool("blockoutDecals", true);
			game->m_zombieGuy.Draw(shader, glm::mat4(1));
			shader->setBool("blockoutDecals", false);

			glDepthMask(GL_FALSE);
	}*/

	void Renderer::DecalCompositePass(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_compositeBuffer.ID);

		glDisable(GL_BLEND);
		unsigned int attachmentss[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, attachmentss);

		glCullFace(GL_BACK);

		glUseProgram(shader->ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_compositeBuffer.gAlbedoDecalComposite);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_compositeBuffer.gCopy);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gSSR);
		glActiveTexture(GL_TEXTURE3);

		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		shader->setMat4("model", glm::mat4(1));
		Quad2D::RenderQuad(shader);



		// ok now blit these back onto the original textures
	/*	glBindFramebuffer(GL_READ_FRAMEBUFFER, s_compositeBuffer.ID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_gBuffer.ID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, 0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_compositeBuffer.ID);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_gBuffer.ID);
		glDrawBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, 0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

*/
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


	}
	/*
	void Renderer::RenderProjectiles(Shader* shader)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		shader->use();
		Shell::DrawInstanced(shader, Shell::s_shotgunShells);
		Shell::DrawInstanced(shader, Shell::s_bulletCasings);
		glDepthMask(GL_FALSE);
	}*/

	/*void Renderer::ReRenderDoors(Shader* shader)
	{
		// Re-render the doors over the top of decals.
		for (Door& door : GameData::p_house->m_doors)
			door.Draw(shader);
	}*/

	void Renderer::DecalPass(Game* game, Shader* shader)
	{
		// First you need to render the glass into the depth map
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		glDepthMask(GL_TRUE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		unsigned int attachmentss[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachmentss);
		s_geometryShader.use();

		for (Window& window : GameData::p_house->m_windows) {
			static int material0 = AssetManager::GetMaterialIDByName("Window");
			static int material1 = AssetManager::GetMaterialIDByName("WindowExterior");
			window.DrawGlass(&s_geometryShader);
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);




		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(1, attachments);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	//	glDisable(GL_CULL_FACE);
		/// Now render the decals
		shader->use(); 
		shader->setMat4("pv", game->camera.m_projectionViewMatrix);
		shader->setMat4("inverseProjectionMatrix", glm::inverse(game->camera.m_projectionMatrix));
		shader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));
		shader->setFloat("screenWidth", CoreGL::s_windowWidth);
		shader->setFloat("screenHeight", CoreGL::s_windowHeight);
		shader->setVec3("u_CameraFront", game->camera.m_Front);
		shader->setVec3("u_ViewPos", game->camera.m_viewPos); 

		// Better pass in that light data too so u can get some nice ligthing ya dick head
		for (int i = 0; i < GameData::p_house->m_lights.size(); i++)
		{
			Light& light = GameData::p_house->m_lights[i];
			shader->setVec3("lightPosition[" + std::to_string(i) + "]", light.m_position);
			shader->setFloat("lightRadius[" + std::to_string(i) + "]", light.m_radius);
			shader->setFloat("lightMagic[" + std::to_string(i) + "]", light.m_magic);
			shader->setFloat("lightStrength[" + std::to_string(i) + "]", light.m_strength);
			shader->setVec3("lightColor[" + std::to_string(i) + "]", light.m_color);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gNormal);


	//	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	//	glDrawBuffers(3, attachments);

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
		
		// Clear the lighting texture
		glClearTexImage(s_gBuffer.gFinalLighting, 0, GL_RGB, GL_FLOAT, 0);

		if (s_demo)
			VP = glm::mat4(1);

		for (Light light : GameData::p_house->m_lights)
		{
			//Light light = GameData::p_house->m_lights[3];

			if (light.m_roomID == -1)
			{
				std::cout << " 'THIS' LIGHT IS NOT IN A ROOM\n";
				break;
			}

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
							if (lightVolumeDoorWay.m_type == HoleInWallType::DOOR)
								if (lightVolumeDoorWay.p_parentDoor->m_openStatus != DoorStatus::DOOR_CLOSED)
									lightVolumeDoorWay.Draw(stencilShader);


							// Staircase opening, well always!
							if (lightVolumeDoorWay.m_type == HoleInWallType::STAIRCASE_OPENING)
								lightVolumeDoorWay.Draw(stencilShader);
						}
					}
				}


			/*	for (Door& door : GameData::p_house->m_doors)
				{
					AssetManager::DrawModel(AssetManager::GetModelIDByName("DoorVolumeA"), stencilShader, door.GetDoorModelMatrixFromPhysicsEngine());
				}*/



			/*enderer::s_RenderSettings.ShadowMapPass = true;
				Door* door = &GameData::p_house->m_doors[0];
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
			lightingShader->setFloat("room_lowerX", GameData::p_house->m_rooms[light.m_roomID].m_lowerX - bias);
			lightingShader->setFloat("room_lowerZ", GameData::p_house->m_rooms[light.m_roomID].m_lowerZ - bias);
			lightingShader->setFloat("room_upperX", GameData::p_house->m_rooms[light.m_roomID].m_upperX + bias);
			lightingShader->setFloat("room_upperZ", GameData::p_house->m_rooms[light.m_roomID].m_upperZ + bias);
	


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
					GameData::p_house->m_rooms[light.m_roomID].m_lightVolume.Draw(lightingShader);

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

	void Renderer::GlassPass(Game* game, Shader* shader)
	{
		// BLUR FIRST
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(1, attachments);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		Shader* blurShader = &s_GlassBlur;
		blurShader->use();
		blurShader->setMat4("projection", game->camera.m_projectionMatrix);
		blurShader->setMat4("viewMatrix", game->camera.m_viewMatrix);
		blurShader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));
		blurShader->setFloat("screenWidth", CoreGL::s_windowWidth);
		blurShader->setFloat("screenHeight", CoreGL::s_windowHeight);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gFinalLighting);

		for (Window& window : GameData::p_house->m_windows) {
			static int material0 = AssetManager::GetMaterialIDByName("Window");
			static int material1 = AssetManager::GetMaterialIDByName("WindowExterior");
			window.DrawGlass(blurShader);
		}


		// NOW DO GLASS				
		glDrawBuffer(GL_COLOR_ATTACHMENT6);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shader = &s_TransparentSurfacesShader;
		shader->use();
		shader->setMat4("projection", game->camera.m_projectionMatrix);
		shader->setMat4("viewMatrix", game->camera.m_viewMatrix);
		shader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));	

		for (int i = 0; i < GameData::p_house->m_lights.size(); i++)
		{
			Light& light = GameData::p_house->m_lights[i];
			shader->setVec3("lightPosition[" + std::to_string(i) + "]", light.m_position);
			shader->setFloat("lightRadius[" + std::to_string(i) + "]", light.m_radius);
			shader->setFloat("lightMagic[" + std::to_string(i) + "]", light.m_magic);
			shader->setFloat("lightStrength[" + std::to_string(i) + "]", light.m_strength);
			shader->setVec3("lightColor[" + std::to_string(i) + "]", light.m_color);
		}
		
		static int dust1 = AssetManager::GetTexIDByName("Dust0_Overlay");
		static int dust2 = AssetManager::GetTexIDByName("Dust0_Overlay2");

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, dust1);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, dust2);
				
		for (Window& window : GameData::p_house->m_windows) {
			static int material0 = AssetManager::GetMaterialIDByName("Window");
			static int material1 = AssetManager::GetMaterialIDByName("WindowExterior");
			AssetManager::BindMaterial_0(material0);
			AssetManager::BindMaterial_1(material1);
			window.DrawGlass(shader);
		}


		// Now combine that the glass blur texture with the final lighting, so that you can draw muzzel flashes ontop of it

		glDrawBuffer(GL_COLOR_ATTACHMENT4);

		glUseProgram(s_CombineGlassBlurWithFinalLighting.ID);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gGlassBlur);

		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		Quad2D::RenderQuad(&s_CombineGlassBlurWithFinalLighting);
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
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gGlassBlur);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gGlassSurface);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gSSR);

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
		// First you need to render the glass into the depth map
		/*glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		glDepthMask(GL_TRUE);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		s_geometryShader.use();
		//s_geometryShader.setMat4("projection", game->camera.m_projectionMatrix);
		//s_geometryShader.setMat4("view", game->camera.m_viewMatrix);

		for (Window& window : GameData::p_house->m_windows) {
			static int material0 = AssetManager::GetMaterialIDByName("Window");
			static int material1 = AssetManager::GetMaterialIDByName("WindowExterior");
			window.DrawGlass(&s_geometryShader);
		}
		
		*/
		// Now you can do your DOF stuff buddy
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

		std::string ammoInGun = std::to_string(WeaponLogic::m_AmmoInGun);
		std::string ammoAvaliable = std::to_string(WeaponLogic::m_AmmoAvaliable);

		char* cstr = new char[ammoInGun.length() + 1];
		strcpy(cstr, ammoInGun.c_str());
		char* cstr2 = new char[ammoAvaliable.length() + 1];
		strcpy(cstr2, ammoAvaliable.c_str());

		glm::vec3 ammoColor = glm::vec3(0.26f, 0.78f, 0.33f);
		if (WeaponLogic::m_AmmoInGun == 0)
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

	void Renderer::InventoryPass(Game* game)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

	//	glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glm::mat4 identity = glm::mat4(1);
		glm::mat4 projection = glm::perspective(Config::INSPECT_FOV, (float)CoreGL::s_windowWidth / (float)CoreGL::s_windowHeight, NEAR_PLANE, FAR_PLANE);

		s_GunInspectShader.use();
		s_GunInspectShader.setMat4("projection", identity);
		s_GunInspectShader.setMat4("view", projection);
	
		s_Inv_GlockTransform.rotation.y += 0.01f;

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, GameData::p_house->m_lights[0].m_LightProbe.SH_TexID);

		AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("Glock"));
		AssetManager::GetModelByName("Glock")->Draw(&s_GunInspectShader, s_Inv_GlockTransform.to_mat4());
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
		GameData::p_house->Draw(shader, envMapPass);
		
		// THIS IS ALL SHIT YOU DON'T WANT DECALS DRAWN OVER. BECAUSE IT MOVES.
		{

			// Enemies
			shader->setBool("blockoutDecals", true);
			game->m_zombieGuy.Draw(shader, glm::mat4(1));
		
			game->m_NurseGuy.Draw(shader, game->m_NurseGuy.m_worldTransform.to_mat4());

			shader->setBool("blockoutDecals", false);

			// Doors
			for (Door& door : GameData::p_house->m_doors)
					door.Draw(shader);

		}

		// If this is NOT the shadowmap pass, then draw projectiles, without decals on them
		if (bindTextures) {
			shader->setBool("blockoutDecals", true);
			Shell::DrawInstanced(shader, Shell::s_shotgunShells);
			Shell::DrawInstanced(shader, Shell::s_bulletCasings);
			shader->setBool("blockoutDecals", false);
		}

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



	//	game->m_zombieGuy.m_worldTransform = s_DebugTransform;
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
		
		SkinnedModel* skinnedModel = game->m_zombieGuy.GetSkinnedModel();

		//for (unsigned int i = 0; i < skinnedModel->m_NumBones; i++)
		//	shader->setMat4("skinningMats[" + std::to_string(i) + "]", skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose);
		
	//	for (unsigned int i = 0; i < skinnedModel->m_NumBones - 5; i++)
	//		shader->setMat4("skinningMats[" + std::to_string(i) + "]", skinnedModel->m_BoneInfo[i].DebugMatrix_BindPose);


		//if (!s_demo)

	

	//	s_debugString += "\nmodelTransform\n";
	//	s_debugString += Util::Mat4ToString(game->m_zombieGuy.m_modelTransform.to_mat4());


		///////////////
		// NURSE GUY //
		///////////////
		
		//game->m_NurseGuy.Draw(shader, glm::mat4(1));


		// First pesron weapon
		if (s_RenderSettings.DrawWeapon)
		{			
		//	WeaponLogic::RenderCurrentWeapon(shader, &game->camera);
		}


		// Animated test entity

		game->m_testAnimatedEnttity.m_worldTransform.position = glm::vec3(1, 1.25f, 0);
		game->m_testAnimatedEnttity.m_worldTransform.rotation = glm::vec3(0, -HELL_PI / 2, 0);
		game->m_testAnimatedEnttity.m_worldTransform.scale = glm::vec3(0.0125f);

		shader->setInt("hasAnimation", true);
	//	game->m_testAnimatedEnttity.Draw(shader, glm::mat4(1));
		shader->setInt("hasAnimation", false);


		//game->m_glockAnimatedEntiyty.m_worldTransform = Renderer::s_DebugTransform2;
//		game->m_testAnimatedEnttity2.m_worldTransform.position = glm::vec3(1, 1.25f, 0);
//		game->m_testAnimatedEnttity2.m_worldTransform.rotation = glm::vec3(0, 0, 0);
		//game->m_glockAnimatedEntiyty.m_worldTransform.scale = glm::vec3(0.0125f);

		AssetManager::BindMaterial_0(AssetManager::GetMaterialIDByName("White"));
		//glDisable(GL_DEPTH_TEST);
		//game->m_glockAnimatedEntiyty.Draw(shader, glm::mat4(1));
		shader->setInt("hasAnimation", false);
		

		// Glass
		/*Transform transform;
		transform = s_DebugTransform;
		AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("White"));
		AssetManager::GetModelByName("Glass")->Draw(shader, transform.to_mat4 ()); */
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

	void Renderer::EditorOverlayPass(Game* game, Shader* shader)
	{
		if (!CoreImGui::s_Show)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(1, attachments);
		glDepthFunc(GL_LEQUAL);
	
		shader->use();
		shader->setMat4("projection", game->camera.m_projectionMatrix);
		shader->setMat4("view", game->camera.m_viewMatrix);

		LevelEditor::Update(game);
	}

	void Renderer::DecalTexturePaint(Shader* shader)
	{


		glDisable(GL_BLEND);
		Entity* couchEntity = &GameData::p_house->m_entities[2];

		float clearcolor[] = { 1, 0, 0, 1 };
		glClearTexImage(couchEntity->m_decalMapID, 0, GL_RGBA, GL_FLOAT, clearcolor);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager::GetTexIDByName("decal_mask7"));


	}

	void Renderer::ScreenSpaceReflectionsPass(Shader* shader, Game* game)
	{
		glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_FXAABuffer.TexID); // Contains the previous renderered frame
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gRMA);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, s_gBuffer.gEmissive);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(1, attachments);

		shader->use();

		shader->setMat4("pv", game->camera.m_projectionViewMatrix);
		shader->setMat4("inverseProjectionMatrix", glm::inverse(game->camera.m_projectionMatrix));
		shader->setMat4("inverseViewMatrix", glm::inverse(game->camera.m_viewMatrix));
		shader->setFloat("screenWidth", CoreGL::s_windowWidth);
		shader->setFloat("screenHeight", CoreGL::s_windowHeight);

		Quad2D::RenderQuad(shader);

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, s_FXAABuffer.TexID);

		// Ok this is wild but you are going to outpuit this pass to the GBUFFER aldebo tex

		
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