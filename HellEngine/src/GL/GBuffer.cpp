#include "hellpch.h"
#include "GBuffer.h"

namespace HellEngine
{
	HellEngine::GBuffer::GBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &gAlbedo);
		glGenTextures(1, &gNormal);
		glGenTextures(1, &gRMA);
		glGenTextures(1, &gEmissive);
		glGenTextures(1, &gFinalLighting);
		glGenTextures(1, &gGlassBlur);
		glGenTextures(1, &gGlassSurface);
		glGenTextures(1, &rboDepth);
		Configure(width, height);
	}

	HellEngine::GBuffer::GBuffer()
	{
	}

	HellEngine::GBuffer::~GBuffer()
	{
	}

	void GBuffer::Configure(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		glBindTexture(GL_TEXTURE_2D, gRMA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gRMA, 0);

		glBindTexture(GL_TEXTURE_2D, gEmissive);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmissive, 0);

		glBindTexture(GL_TEXTURE_2D, gFinalLighting);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gFinalLighting, 0);

		glBindTexture(GL_TEXTURE_2D, gGlassBlur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gGlassBlur, 0);

		glBindTexture(GL_TEXTURE_2D, gGlassSurface);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gGlassSurface, 0);

		unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
		glDrawBuffers(7, attachments);

		glBindTexture(GL_TEXTURE_2D, rboDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rboDepth, 0);



		auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (fboStatus == 36053)
			std::cout << "G Buffer: " << "COMPLETE\n";
		if (fboStatus == 36054)
			std::cout << "G Buffer: " << "INCOMPLETE ATTACHMENT\n";
		if (fboStatus == 36057)
			std::cout << "G Buffer: " << "INCOMPLETE DIMENSIONS\n";
		if (fboStatus == 36055)
			std::cout << "G Buffer: " << "MISSING ATTACHMENT\n";
		if (fboStatus == 36061)
			std::cout << "G Buffer: " << "UNSPORTED\n";

		auto glstatus = glGetError();
		if (glstatus != GL_NO_ERROR)
			std::cout << "Error in GL call: " << glstatus << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}