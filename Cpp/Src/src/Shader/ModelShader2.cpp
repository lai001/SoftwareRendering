#include "ModelShader2.hpp"

RasterizationData ModelShader2::vertexShader(const void * vertexBuffer, const int vertexIdx)
{
	BaseVertex2 vertex = ((BaseVertex2*)vertexBuffer)[vertexIdx];
	RasterizationData out;
	const glm::mat4x4 mvpMat = projectionMat * viewMat * modelMat;
	out.position = mvpMat * glm::vec4(vertex.position, 1.0f);
	const glm::vec4 coords = glm::vec4(vertex.textureCoords.x, vertex.textureCoords.y, 1.0, 1.0);
	out.extraData.push_back(coords);
	return out;
}

glm::vec4 ModelShader2::fragmentShader(const RasterizationData & rasterizationData)
{
	glm::vec4 uv = rasterizationData.extraData[0];
	glm::vec2 _uv = glm::vec2(uv.x, uv.y);
	if (texture)
	{
		glm::vec4 color = texture->sample(_uv);
		return color;
	}
	else
	{
		return glm::vec4(0.0f);
	}
}
