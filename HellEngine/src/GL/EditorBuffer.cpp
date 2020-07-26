#include "hellpch.h"
#include "EditorBuffer.h"

namespace HellEngine
{
	EditorBuffer::EditorBuffer(int width, int height)
	{
		glGenFramebuffers(1, &ID);
		glGenTextures(1, &HoverTexture);
		//glGenTextures(1, &SelectedTexture);
		Configure(width, height);
	}

	HellEngine::EditorBuffer::EditorBuffer()
	{
	}

	void EditorBuffer::Configure(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glBindTexture(GL_TEXTURE_2D, HoverTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HoverTexture, 0);

	/*	glBindTexture(GL_TEXTURE_2D, SelectedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SelectedTexture, 0);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);*/

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, attachments);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}