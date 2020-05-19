#include "hellpch.h"
#include "LightProbeGrid.h"
#include "Helpers/Util.h"

LightProbeGrid::LightProbeGrid()
{

}

LightProbeGrid::~LightProbeGrid()
{
}

void LightProbeGrid::Init()
{
	for (size_t x = 0; x < GRID_SIZE_X; x++)
		for (size_t y = 0; y < GRID_SIZE_Y; y++)
			for (size_t z = 0;z < GRID_SIZE_Z; z++)
			{
				float xpos = GRID_OFFSET_X + (x * GRID_SPACING);
				float ypos = HEIGHTS[y];
				float zpos = GRID_OFFSET_Z + (z * GRID_SPACING);
				m_probeArray[x][y][z].Position = glm::vec3(xpos, ypos, zpos);

				//std::cout << HellEngine::Util::Vec3_to_String(m_probeArray[x][y][z].Position) << "\n";
			}
}
