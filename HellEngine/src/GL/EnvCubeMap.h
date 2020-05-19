#pragma once

namespace HellEngine
{
	class EnvCubeMap
	{
	public:
		EnvCubeMap();
		~EnvCubeMap();
		void Init();

		const int width = 256;
		const int height = 256;

		unsigned int FBO;
		unsigned int RBO;
		unsigned int CubeMap_TexID;

		unsigned int SH_FboID;
		unsigned int SH_TexID;
	};
}
	