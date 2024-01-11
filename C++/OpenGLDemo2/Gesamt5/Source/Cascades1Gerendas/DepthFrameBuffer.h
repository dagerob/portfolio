#pragma once
#include "commom_header.h"

class DepthFrameBuffer {
	unsigned int FBOindex, textureIndex;
	int width, height;
public:
	DepthFrameBuffer(int width, int length);
	DepthFrameBuffer(const DepthFrameBuffer& other);
	DepthFrameBuffer& operator=(const DepthFrameBuffer& other);
	int getWidth() const { return width; };
	int getHeight() const { return height; };
	int getFBIndex() const { return FBOindex; };
	int getTextureIndex() const { return textureIndex; };
};