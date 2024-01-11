#include "DepthFrameBuffer.h"

DepthFrameBuffer::DepthFrameBuffer(int width, int height) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &FBOindex);

	// We create the Texture to save the data
	glGenTextures(1, &textureIndex);
	glBindTexture(GL_TEXTURE_2D, textureIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// we attach the Texture to the generated shadow frame buffer
	// we set the draw and read buffers to none because we will not save colors
	glBindFramebuffer(GL_FRAMEBUFFER, FBOindex);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureIndex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// go back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthFrameBuffer::DepthFrameBuffer(const DepthFrameBuffer& other) {
	width = other.width;
	height = other.height;
	FBOindex = other.FBOindex;
	textureIndex = other.textureIndex;
}

DepthFrameBuffer& DepthFrameBuffer::operator=(const DepthFrameBuffer& other) {
	if (this == &other)
		return *this;
	width = other.width;
	height = other.height;
	FBOindex = other.FBOindex;
	textureIndex = other.textureIndex;
	return *this;
}