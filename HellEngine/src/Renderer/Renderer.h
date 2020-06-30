#pragma once
#include "Header.h"
#include "Core/LightProbeGrid.h"
#include "GL/GBuffer.h"
#include "GL/LightProbe.h"
#include "GL/FinalImageBuffer.h"
#include "GL/LightingBuffer.h"
#include "GL/BlurBuffer.h"
#include "GL/DOFBuffer.h"
#include "GL/FXAABuffer.h"
#include "GL/ChromaticAbberationBuffer.h"
#include "GL/CameraEnvMap.h"
#include "TextBlitter.h"
#include "House/Light.h"

#include "Effects/BloodEffect.h"
#include "Effects/MuzzleFlash.h"
#include "Effects/BloodWallSplatter.h"

#include "Animation/SkinnedModel.h"
#include "NumberBlitter.h"
#include "Core/AnimatedEntity.h"

namespace HellEngine
{
	class Game;

	class Renderer
	{
	public: // Functions
		static void Init();
		static void RenderFrame(Game* game);
		static void SetTextureBindings();

	private: // Functions
		static void DrawScene(Game* game, Shader* shader, bool bindTextures, bool envMapPass);
		static void GeometryPass(Game* game, Shader* shader);
		static void LightingPass(Game* game, Shader* stencilShader, Shader* lightingShader);
		static void BlurPass(Shader* blurVerticalShader, Shader* blurHorizontalShader);
		static void CompositePass(Game* game, Shader* shader);
		static void ChromaticAberrationPass(Shader* shader);
		static void FXAAPass(Shader* shader);
		static void DOFPass(Shader* shader);
		static void EffectsPass(Game* game, Shader* shader);
		static void DecalPass(Game* game, Shader* shader);
		static void HUDPass(Game* game, Shader* shader);
		static void InventoryPass(Game* game);

		static void ShadowMapPass(Game* game, Shader* shader);

		static void EnvMapPass(Game* game, Shader* envmapShader, Shader* sphericalHarmonicsShader);
		static void RenderEnvMap(Game* game, Shader* shader, Light* light);
		static void RenderSphericalHarmonicsTexture(Game* game, Shader* shader, Light* light);

		static void RenderFinalImage(Shader* shader, unsigned int textureID);
		static void RenderDebugTextures(Shader* shader, unsigned int texture0_ID, unsigned int texture1_ID, unsigned int texture2_ID, unsigned int texture3_ID);
		static void BulletDebugDraw(Game* game, Shader* shader);
		static void ViewCubeMap(Game* , Shader* shader, unsigned int CubeMapID);
		static void CreateBRDFLut();

		static void DrawPoint(Shader* shader, glm::vec3 position, glm::vec3 color);
		static void DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix);
		//static void DrawSkeleton(Shader* shader, SkinnedModel* skinnedModel, Transform* transform);
		static void DrawTangentDebugAxis(Shader* shader, glm::mat4 modelMatrix);

		static void DrawAnimatedEntityDebugBones_Animated(Shader* shader, AnimatedEntity* animatedEnitty);
		static void DrawAnimatedEntityDebugBones_BindPose(Shader* shader, AnimatedEntity* animatedEnitty);

	public: // functions
		static void TextBlitPlass(Shader* shader);

	public: // Fields
		static Shader s_forwardShader;
		static Shader s_solidColorShader;
		static Shader s_quadShader;
		static Shader s_geometryShader;
		static Shader s_StencilShader;
		static Shader s_lightingShader;
		static Shader s_compositeShader;
		static Shader s_ShadowMapShader;
		
		static Shader s_skyBoxShader;
		static Shader s_backgroundShader;
		static Shader s_BRDF_Shader;
		static Shader s_reflection_Map_Shader;
		static Shader s_SphericalH_Harmonics_Shader;

		static Shader s_blurVerticalShader;
		static Shader s_blurHorizontalShader;

		static Shader s_DOFShader;
		static Shader s_FXAAShader;
		static Shader s_ChromaticAberrationShader;

		static Shader s_BloodShader;
		static Shader s_DecalShader;

		static Shader s_GunInspectShader;

		static std::vector<BlurBuffer> s_BlurBuffers;

		static bool s_demo;

		// FBOs
		static GBuffer s_gBuffer;
		static LightingBuffer s_LightingBuffer;
		static FinalImageBuffer s_FinalImageBuffer;
		static DOFBuffer s_DOFBuffer;
		static FXAABuffer s_FXAABuffer;
		static ChromaticAbberationBuffer s_ChromaticAbberationBuffer;

		// Env maps
		//static LightProbe s_LightProbe;
		static CameraEnvMap s_CameraEnvMap;


		//static LightProbeGrid s_LightProbeGrid;

		static unsigned int brdfLUTTexture;
		static bool m_showBulletDebug;
		static bool m_showDebugTextures;
		static bool m_showImGui;
		static bool b_showCubemap;
		static bool b_renderDoorWayVolumes;

		static RenderSettings s_RenderSettings;
		static Transform s_DebugTransform;
		static Transform s_DebugTransform2;
		static Transform s_DebugTransform3;

		static Transform s_Inv_GlockTransform;

		static float s_polygonFactor;
		static float s_polygonUnits;

		static Transform s_hitPoint;
		static unsigned int s_pointVAO;

		static BloodEffect s_bloodEffect;
		static MuzzleFlash s_muzzleFlash;
		static BloodWallSplatter s_bloodWallSplatter;

		static std::string s_debugString;

		static AnimatedEntity s_zombieAnimatedEntity;
	};
}