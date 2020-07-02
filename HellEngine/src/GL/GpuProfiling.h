#pragma once
#include "Header.h"

namespace HellEngine
{
	class GpuProfiler
	{
	public: // Methods
		GpuProfiler(const char* m);
		~GpuProfiler();
	};
}

#define GPU_PROFILE 1