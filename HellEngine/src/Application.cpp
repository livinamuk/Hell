#include "hellpch.h"
#include "Header.h"
#include "Game.h"
#include "Core/CoreGL.h"
#include "Core/CoreImGui.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Importer.h"
#include "Renderer/Renderer.h"

using namespace HellEngine;


int main()
{
	CoreGL::InitGL(SCR_WIDTH, SCR_HEIGHT);
	CoreImGui::InitImGui();
	Game game = Game();
	Renderer::Init(&game);
	Importer::Init();
	AssetManager::FindAllFiles();
	AssetManager::ForceLoadTexture("CharSheet");
	
	// Loading loop
	
	while (!AssetManager::s_loadingComplete && !Input::s_keyDown[HELL_KEY_ESCAPE])
	{
		CoreGL::ProcessInput();
		CoreGL::OnUpdate();
		CoreGL::ClearDefaultFrameBufferToBlack();

		if (Input::s_keyPressed[HELL_KEY_F])
			CoreGL::ToggleFullScreen();

		// Limit log output to certain number of lines
		if (Util::CountLines(AssetManager::s_loadLog) > 38)
			Util::EraseFirstLine(&AssetManager::s_loadLog);

		TextBlitter::UpdateBlitter(1);
		TextBlitter::BlitText(AssetManager::s_loadLog, false);
		Renderer::TextBlitPlass(&Renderer::s_quadShader);

		CoreGL::SwapBuffersAndPollEvents();

		static bool RUN_ONCE = true;
		if (RUN_ONCE) {
			AssetManager::LoadAllTextures();
			AssetManager::LoadHardcoded();
			RUN_ONCE = false;
		}

		AssetManager::LoadNextReadyAssetToGL();
	}

	// Main game loop
	while (CoreGL::IsRunning() && !Input::s_keyDown[HELL_KEY_ESCAPE])
	{
		AssetManager::LoadNextReadyAssetToGL();
		CoreGL::ProcessInput();
		CoreGL::OnUpdate();
		
		game.OnUpdate();
		game.OnRender();

		if (Renderer::m_showImGui)
			CoreImGui::Render(&game);
		
		CoreGL::SwapBuffersAndPollEvents();
	}

	CoreGL::Terminate();
	Importer::Terminate();
	return 0;

	Opp r = Opp(2400);
	r.Deduct(1200);

	Opp m = Opp(1200);
	m.Deduct(300);
	m.Deduct(-50);
	m.Deduct(600);

	Opp c = Opp(2400);
	c.Deduct(300);
	c.Deduct(300);
	c.Deduct(300);
}