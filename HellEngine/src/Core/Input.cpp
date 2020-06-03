#include "hellpch.h"
#include "Input.h"
#include "keycodes.h"
#include "CoreGL.h"
#include "Game.h"

namespace HellEngine
{
	int Input::s_mouseX;
	int Input::s_mouseY;
	int Input::s_storedMouseX;
	int Input::s_storedMouseY;
	int Input::s_mouseX_Editor;
	int Input::s_mouseY_Editor;
	bool Input::s_showCursor = false;
	bool Input::s_keyPressed[350] = { false };
	bool Input::s_keyDown[350] = { false };
	bool Input::s_keyDownLastFrame[350] = { false };
	bool Input::s_leftMousePressed = false;
	bool Input::s_rightMousePressed = false;
	bool Input::s_leftMouseDown = false;
	bool Input::s_rightMouseDown = false;
	bool Input::s_leftMouseDownLastFrame = false;
	bool Input::s_rightMouseDownLastFrame = false;
	
	void Input::MouseUpdate(double xpos, double ypos)
	{
		s_mouseX = xpos;
		s_mouseY = ypos;

		if (s_showCursor) {
			s_mouseX_Editor = xpos;
			s_mouseY_Editor = ypos;
		}
	}

	void Input::HandleKeypresses()
	{
		// Toggle cursor
		if (s_keyPressed[HELL_KEY_M])
			s_showCursor = !s_showCursor;

		// Toggle fullscreen
		if (s_keyPressed[HELL_KEY_F]) {
			CoreGL::ToggleFullScreen();
		}

		// Hotload Shaders
		if (s_keyPressed[HELL_KEY_H])
		{
			Renderer::s_lightingShader.ReloadShader();
			Renderer::s_SphericalH_Harmonics_Shader.ReloadShader();
			Renderer::s_geometryShader.ReloadShader();
			Renderer::s_reflection_Map_Shader.ReloadShader();
			Renderer::s_backgroundShader.ReloadShader();
			Renderer::s_compositeShader.ReloadShader();
			Renderer::s_ChromaticAberrationShader.ReloadShader();
			Renderer::s_DOFShader.ReloadShader();
			Renderer::s_ShadowMapShader.ReloadShader();
			Renderer::s_FXAAShader.ReloadShader();
			Renderer::s_BloodShader.ReloadShader();
			Renderer::s_DecalShader.ReloadShader();
			Renderer::s_solidColorShader.ReloadShader();
			Renderer::SetTextureBindings();
		}

		// Toggle Bullet Debug Drawer
		if (s_keyPressed[HELL_KEY_B])
			Renderer::m_showBulletDebug = !Renderer::m_showBulletDebug;

		// Toggle ImGui
		if (s_keyPressed[HELL_KEY_I])
			Renderer::m_showImGui = !Renderer::m_showImGui;
	}
}