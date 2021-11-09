#pragma once

#include <vector>

#include "Shader.hpp"
#include "Texture2D.hpp"

struct ImageShaderVertex
{
	glm::vec2 position;
	glm::vec2 uv;
	ImageShaderVertex(const glm::vec2& position, const glm::vec2& uv)
		:position(position), uv(uv)
	{

	}
};

class ImageShader : public Shader
{
public:
	Texture2D* texture = nullptr;
	virtual RasterizationData vertexShader(const void * vertex, const int vertexIdx) override;
	virtual glm::vec4 fragmentShader(const RasterizationData & rasterizationData) override;
};