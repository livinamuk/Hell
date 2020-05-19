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
	AssetManager::LoadAllTextures();
	AssetManager::LoadHardcoded();

	// Main loop
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

	Opp c = Opp(2400);
	c.Deduct(300);
	c.Deduct(300);

	Opp b = Opp(100);
}