#pragma once

namespace HellEngine
{
	class GBuffer
	{
	public:
		GBuffer(int width, int height);
		GBuffer();
		~GBuffer();

		unsigned int ID;
		unsigned int gAlbedo, gNormal, gRMA, rboDepth, gBuffer4, gFinalLighting;

		void Configure(int width, int height);
	};
}
