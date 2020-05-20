#pragma once
#include "Renderer/Shader.h"
#include "Header.h"

namespace HellEngine
{
	class TextBlitter
	{
	public: //methods
		static void DrawTextBlit(Shader* shader);
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
		static std::string s_CharSheet;
		static bool s_centerText;
	};
}