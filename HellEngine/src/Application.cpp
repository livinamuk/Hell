#include "hellpch.h"
#include "Header.h"
#include "Game.h"
#include "Core/CoreGL.h"
#include "Core/CoreImGui.h"
#include "Helpers/AssetManager.h"
#include "Renderer/Renderer.h"
#include "Audio/Audio.h"
#include "Core/File.h"

using namespace HellEngine;

int main()
{
	CoreGL::InitGL(SCR_WIDTH, SCR_HEIGHT);
	CoreImGui::InitImGui();
	Renderer::Init();
	AssetManager::FindAllFiles();
	AssetManager::ForceLoadTexture("CharSheet");
	Audio::Init();

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

	Game game = Game();

	int frames = 0;
	double frameCounter = 0;
	double lastTime = CoreGL::GetGLTime();
	double unprocessedTime = 0;
	double desiredFrameRate = 60;
	game.m_frameTime = 1.0 / desiredFrameRate;
	double MAX_UPDATE_TIME = game.m_frameTime * 60;

	//CoreGL::SetVSync(true);

	// Main game loop
	while (CoreGL::IsRunning() && !Input::s_keyDown[HELL_KEY_ESCAPE])
	{
		MAX_UPDATE_TIME = Config::TEST_FLOAT;

		bool render = false;
		double startTime = CoreGL::GetGLTime();
		double passedTime = startTime - lastTime;
		lastTime = startTime;

		unprocessedTime += passedTime;
		frameCounter += passedTime;

		while (unprocessedTime > game.m_frameTime)
		{
			unprocessedTime -= game.m_frameTime;

			if (unprocessedTime > MAX_UPDATE_TIME)
				unprocessedTime = 0;

			render = true;

			
			CoreGL::ProcessInput();
			CoreGL::OnUpdate();

			game.OnUpdate();


			if (frameCounter >= 1.0)
			{
				std::cout << "frames: " << frames << "  unprocssed time: " << unprocessedTime << "\n";//System.out.println(frames);
				frames = 0;
				frameCounter = 0;
			}
		}
		if (render)
		{
			game.UpdateSkeletalAnimation();
			game.OnRender(); 
			frames++;

			if (Renderer::m_showImGui)
				CoreImGui::Render(&game);

			CoreGL::SwapBuffersAndPollEvents();
		}



		
	}

	CoreGL::Terminate();
	Audio::Terminate();
	return 0;

	{
		Opp r = Opp(900);

		Opp m = Opp(275);
		m = Opp(275);
		m = Opp(275);
		m = Opp(275);
		m = Opp(50);

		Opp c = Opp(2000);
	}
}