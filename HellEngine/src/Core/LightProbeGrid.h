#pragma once
#include "GL/CubeMapArray.h"

class LightProbeGrid
{

struct Probe {
	glm::vec3 Position;
};

public:
	LightProbeGrid();
	~LightProbeGrid();
	void Init();
	
	static const int GRID_SIZE_X = 8;
	static const int GRID_SIZE_Y = 2;
	static const int GRID_SIZE_Z = 8;
	static constexpr float GRID_SPACING = 1.3f;
	static constexpr float GRID_OFFSET_X = -5;
	static constexpr float GRID_OFFSET_Y = 1.2f;
	static constexpr float GRID_OFFSET_Z = -5;
	float HEIGHTS[4] = {0.5f, 1.9f, 3.1f, 3.5f};

	Probe m_probeArray[GRID_SIZE_X][GRID_SIZE_Y][GRID_SIZE_Z] = {{{}}};
	CubeMapArray m_enviromentMaps;
};



