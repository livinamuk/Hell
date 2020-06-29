#include "hellpch.h"
#include "GpuProfiling.h"

namespace HellEngine
{
	GpuProfiler::GpuProfiler(const char* m)
	{
		#if GPU_PROFILE
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m);
		#endif
	}

	GpuProfiler::~GpuProfiler()
	{
		#if GPU_PROFILE
		glPopDebugGroup();
		#endif
	}
}