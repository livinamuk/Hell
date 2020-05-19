#include "hellpch.h"
#include "DOFBuffer.h"

namespace HellEngine
{
	HellEngine::DOFBuffer::DOFBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &TexID);
		Configure(width, height);
	}

	HellEngine::DOFBuffer::DOFBuffer()
	{
	}

	HellEngine::DOFBuffer::~DOFBuffer()
	{
	}

	void DOFBuffer::Configure(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glBindTexture(GL_TEXTURE_2D, TexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexID, 0);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}