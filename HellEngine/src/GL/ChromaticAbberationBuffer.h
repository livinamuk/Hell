#pragma once

namespace HellEngine
{
	class ChromaticAbberationBuffer
	{
	public:
		ChromaticAbberationBuffer(int width, int height);
		ChromaticAbberationBuffer();
		~ChromaticAbberationBuffer();

		unsigned int ID;
		unsigned int TexID;

		void Configure(int width, int height);
	};
}
