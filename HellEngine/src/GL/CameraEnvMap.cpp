#include "hellpch.h"
#include "CameraEnvMap.h"
#include "Renderer/Renderer.h"
#include "GL/Quad2D.h"

namespace HellEngine
{
	CameraEnvMap::CameraEnvMap()
	{
	}

	void CameraEnvMap::Init()
	{
		glGenFramebuffers(1, &CubeMap_FBO);
		glGenTextures(1, &depthCubemap);
		glGenTextures(1, &CubeMap_TexID);

		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_2D); // Allocate the mipmaps
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, CubeMap_FBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap_TexID);
		//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB16F, width, height);

		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			glGenerateMipmap(GL_TEXTURE_2D); // Allocate the mipmaps
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);



		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CubeMap_TexID, 0);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
/*
		if (fboStatus == 36053)
			std::cout << "CUBE_MAP FrameBuffer: " << "COMPLETE\n";
		else if (fboStatus == 36054)
			std::cout << "CUBEMAP FrameBuffer: " << "INCOMPLETE ATTACHMENT\n";
		else if (fboStatus == 36057)
			std::cout << "CUBEMAP FrameBuffer: " << "INCOMPLETE DIMENSIONS\n";
		else if (fboStatus == 36055)
			std::cout << "CUBEMAP FrameBuffer: " << "MISSING ATTACHMENT\n";
		else if (fboStatus == 36061)
			std::cout << "CUBEMAP FrameBuffer: " << "UNSPORTED\n";
		else
			std::cout << "unknown fbostatus code\n";
		*/
		auto glstatus = glGetError();
		if (glstatus != GL_NO_ERROR)
			std::cout << "Error in GL call: " << glstatus << std::endl;

		//CheckFramebuffer(CubeMap_FBO, GL_FRAMEBUFFER);

		/*glGenFramebuffers(1, &CubeMap_FBO);
		glGenRenderbuffers(1, &CubeMap_RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, CubeMap_FBO);
		glBindRenderbuffer(GL_RENDERBUFFER, CubeMap_RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, CubeMap_RBO);

		glGenTextures(1, &CubeMap_TexID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap_TexID);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		*/
		// Sphereical Harmonics shit (FBO with single color 3x3 RGB_16F attachment)
		glGenFramebuffers(1, &SH_FboID);
		glGenTextures(1, &SH_TexID);
		glBindFramebuffer(GL_FRAMEBUFFER, SH_FboID);
		glBindTexture(GL_TEXTURE_2D, SH_TexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 3, 3, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SH_TexID, 0);
	}

	CameraEnvMap::~CameraEnvMap()
	{
	}
}