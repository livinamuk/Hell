#pragma once

namespace HellEngine
{
	class CameraEnvMap
	{
	public: // methods
		CameraEnvMap();
		~CameraEnvMap();
		void Init();

	public: // fields
		glm::vec3 m_position;
		const int width = 256;
		const int height = 256;
		unsigned int CubeMap_FBO = 0;
		unsigned int CubeMap_RBO = 0;
		unsigned int CubeMap_TexID = 0;
		unsigned int SH_FboID = 0;
		unsigned int SH_TexID = 0; 
		unsigned int depthCubemap;
	};
}
