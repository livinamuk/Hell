#pragma once

namespace HellEngine
{
	class FXAABuffer
	{
	public:
		FXAABuffer(int width, int height);
		FXAABuffer();
		~FXAABuffer();

		unsigned int ID;
		unsigned int TexID;

		void Configure(int width, int height);
	};
}
