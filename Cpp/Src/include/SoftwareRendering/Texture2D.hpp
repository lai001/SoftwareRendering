#pragma once
#include <string>
#include "glm/glm.hpp"

enum class TextureWrappingType
{
	repeat,
	mirroredRepeat,
	clampToEdge,
	clampToBorder
};

class Texture2D
{
public:
	Texture2D(const std::string& filePath);
	~Texture2D();
	Texture2D & operator=(const Texture2D & t);

public:
	glm::vec4 sample(const glm::vec2& uv) const;
private:
	unsigned char* data = nullptr;
	int width;
	int height;
	int channels;

};

