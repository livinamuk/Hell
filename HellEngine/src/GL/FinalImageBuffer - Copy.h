#pragma once

namespace HellEngine
{
	class FinalImageBuffer
	{
	public:
		FinalImageBuffer(int width, int height);
		FinalImageBuffer();
		~FinalImageBuffer();

		unsigned int ID;
		unsigned int TexID;
		unsigned int Tex2ID;

		void Configure(int width, int height);
	};
}
