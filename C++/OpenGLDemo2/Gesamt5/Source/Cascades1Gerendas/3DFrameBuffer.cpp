#include "3DFrameBuffer.h"

FrameBuffer3D::FrameBuffer3D(int width, int length, int height) {
	this->width = width;
	this->length = length;
	this->height = height;

	// we create the texture to use as framebuffer
	glGenFramebuffers(1, &FBOindex);
	glGenTextures(1, &textureIndex);
	glBindTexture(GL_TEXTURE_3D, textureIndex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED,
		width, length, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	// we configure the buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBOindex);
	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, textureIndex, 0,0);
	glDrawBuffer(GL_FRONT_AND_BACK);
	glReadBuffer(GL_NONE);

	// go back to the default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer3D::FrameBuffer3D(const FrameBuffer3D& other) {
	width = other.width;
	length = other.length;
	height = other.height;
	FBOindex = other.FBOindex;
	textureIndex = other.textureIndex;
	currentLevel = other.currentLevel;
}

FrameBuffer3D& FrameBuffer3D::operator=(const FrameBuffer3D& other) {
	if (this == &other)
		return *this;
	width = other.width;
	length = other.length;
	height = other.height;
	FBOindex = other.FBOindex;
	textureIndex = other.textureIndex;
	currentLevel = other.currentLevel;
	return *this;
}

bool FrameBuffer3D::moveToNextLevel() {
	currentLevel++;
	bool finished = false;
	if (currentLevel >= height) {
		currentLevel = 0;
		finished = true;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBOindex);
	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, textureIndex, 0, currentLevel);
	return finished;
}