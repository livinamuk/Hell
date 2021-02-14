#pragma once

namespace HellEngine
{
	class CompositeBuffer
	{
	public:
		CompositeBuffer(int width, int height);
		CompositeBuffer();
		~CompositeBuffer();

		unsigned int ID;
		unsigned int gAlbedoDecalComposite, gRMADecalComposite, gCopy;

		void Configure(int width, int height);
	};
}
