#pragma once
#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"

struct RasterizationData
{
	glm::vec4 position;
	std::vector<glm::vec4> extraData;
};

class Shader
{
public:
	virtual RasterizationData vertexShader(const void* vertexBuffer, const int vertexIdx) = 0;
	virtual glm::vec4 fragmentShader(const RasterizationData& rasterizationData) = 0;
};
