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
		unsigned int gAlbedo, gNormal, gRMA, rboDepth, gEmissive, gFinalLighting, gGlassBlur, gGlassSurface, gSSR;

		void Configure(int width, int height);
	};
}
