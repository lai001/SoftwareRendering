#pragma once

#include "glm/glm.hpp"

#include "Shader.hpp"
#include "Texture2D.hpp"

struct BaseVertex2
{
	glm::vec3 position;
	glm::vec2 textureCoords;
};

class ModelShader2 : public Shader
{
public:
	glm::mat4x4 modelMat;
	glm::mat4x4 viewMat;
	glm::mat4x4 projectionMat;
	Texture2D *texture = nullptr;

	virtual RasterizationData vertexShader(const void * vertexBuffer, const int vertexIdx) override;
	virtual glm::vec4 fragmentShader(const RasterizationData & rasterizationData) override;
};

