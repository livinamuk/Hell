#include "hellpch.h"
#include "CompositeBuffer.h"

namespace HellEngine
{
	HellEngine::CompositeBuffer::CompositeBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &gAlbedoDecalComposite);
		glGenTextures(1, &gRMADecalComposite);
		glGenTextures(1, &gCopy);
		//glGenTextures(1, &gPreviousFrame);
		Configure(width, height);
	}

	HellEngine::CompositeBuffer::CompositeBuffer()
	{
	}

	HellEngine::CompositeBuffer::~CompositeBuffer()
	{
	}

	void CompositeBuffer::Configure(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glBindTexture(GL_TEXTURE_2D, gAlbedoDecalComposite);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoDecalComposite, 0);

		glBindTexture(GL_TEXTURE_2D, gRMADecalComposite);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gRMADecalComposite, 0);

		glBindTexture(GL_TEXTURE_2D, gCopy);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gCopy, 0);

/*
		glBindTexture(GL_TEXTURE_2D, gPreviousFrame);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPreviousFrame, 0);
		*/
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return;

/*		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glBindTexture(GL_TEXTURE_2D, gAlbedoDecalComposite);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoDecalComposite, 0);

		glBindTexture(GL_TEXTURE_2D, gRMADecalComposite);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gRMADecalComposite, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		//glBindTexture(GL_TEXTURE_2D, rboDepth);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rboDepth, 0);

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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
	}
}