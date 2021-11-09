#include "Texture2D.hpp"
#include <assert.h>
#include "spdlog/spdlog.h"
#include "stb_image.h"

Texture2D::Texture2D(const std::string & filePath)
{
	const char* filename = filePath.c_str();
	data = stbi_load(filename, &width, &height, &channels, 0);
	assert(channels == 3 || channels == 4);
}

Texture2D::~Texture2D()
{
	if (data)
	{
		stbi_image_free(data);
	}
}

Texture2D & Texture2D::operator=(const Texture2D & texture)
{
	width = texture.width;
	height = texture.height;
	channels = texture.channels;
	int length = width * height * channels;
	data = new unsigned char[length];
	memcpy(data, texture.data, length);
	return *this;
}

glm::vec4 Texture2D::sample(const glm::vec2& uv) const
{
	glm::vec2 _uv = glm::vec2(glm::clamp(uv.x, 0.0f, 1.0f), glm::clamp(uv.y, 0.0f, 1.0f));
	if (data)
	{
		int x = glm::min(int(_uv.x*width), width-1);
		int y = glm::min(int(_uv.y*height), height - 1);
		int offset = (y * width + x) * channels;
		float r = data[offset + 0];
		float g = data[offset + 1];
		float b = data[offset + 2];
		float a = data[offset + 3];
		glm::vec4 color = glm::vec4(r, g, b, a) / 255.0f;
		return color;
	}
	else
	{
		return glm::vec4(0);
	}
}
