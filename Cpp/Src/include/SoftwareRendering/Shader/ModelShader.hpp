#pragma once

#include <vector>

#include "Shader.hpp"

struct BaseVertex
{
	glm::vec3 position;
	glm::vec3 color;
};

class ModelShader : public Shader
{
public:
	glm::mat4x4 modelMat;
	glm::mat4x4 viewMat;
	glm::mat4x4 projectionMat;

	virtual RasterizationData vertexShader(const void * vertexBuffer, const int vertexIdx) override;
	virtual glm::vec4 fragmentShader(const RasterizationData & rasterizationData) override;
};
