#pragma once

namespace HellEngine
{
	class DOFBuffer
	{
	public:
		DOFBuffer(int width, int height);
		DOFBuffer();
		~DOFBuffer();

		unsigned int ID;
		unsigned int TexID;

		void Configure(int width, int height);
	};
}
