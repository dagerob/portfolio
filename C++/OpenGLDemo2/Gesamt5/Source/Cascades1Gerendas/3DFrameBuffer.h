#pragma once
#include "commom_header.h"

class FrameBuffer3D {
	unsigned int FBOindex, textureIndex;
	int width, length, height;
	int currentLevel = 0;
public:
	FrameBuffer3D(int width, int length, int height);
	FrameBuffer3D(const FrameBuffer3D& other);
	FrameBuffer3D& operator=(const FrameBuffer3D& other);
	int getWidth() const { return width; };
	int getLength() const { return length; };
	int getHeight() const { return height; };
	int getFBIndex() const { return FBOindex; };
	int getTextureIndex() const { return textureIndex; };
	bool moveToNextLevel();
	int getCurrentLevel() const { return currentLevel; };
};