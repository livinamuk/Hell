#pragma once
#include "Renderer/Shader.h"
#include "Header.h"

namespace HellEngine
{
	class NumberBlitter
	{
	public: //methods
		static void DrawTextBlit(Shader* shader, char* text, int xScreenCoord, int yScreenCoord, float scale = 1.0f, glm::vec3 color = glm::vec3(1,1,1), bool leftJustified = true);
		static void UpdateBlitter(float deltaTime);
		static void TypeText(std::string text, bool centered);
		static void BlitText(std::string text, bool centered);
		static void ResetBlitter();

	public: // fields
		static unsigned int VAO, VBO;
		static unsigned int currentCharIndex;
		static std::string s_textToBlit;
		static float s_blitTimer;
		static float s_blitSpeed;
		static float s_waitTimer;
		static float s_timeToWait;
		static std::string s_NumSheet;
		static bool s_centerText;
	};
}