#pragma once
#include "commom_header.h"

class Texture {
	unsigned int textureIndex;
public:
	Texture() : textureIndex(0) {};
	Texture(const std::string& name);
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
	int getTextureIndex() const { return textureIndex; };
};