#pragma once
class CubeMapArray
{
public:
	CubeMapArray();
	CubeMapArray(int layers);
	~CubeMapArray();

	const unsigned int SIZE = 64;
	unsigned int ID;
	unsigned int depthCubemap;

	//const static float SHADOW_NEAR_PLANE;
	//const static float SHADOW_FAR_PLANE;
	//const static int SHADOW_SIZE;
};

