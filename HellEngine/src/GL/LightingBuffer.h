#pragma once

namespace HellEngine
{
	class LightingBuffer
	{
	public:
		LightingBuffer(int width, int height);
		LightingBuffer();
		~LightingBuffer();

		unsigned int ID;
		unsigned int TexID;

		void Configure(int width, int height);
	};
}
