#include "ImageShader.hpp"

RasterizationData ImageShader::vertexShader(const void * vertexBuffer, const int vertexIdx)
{
	ImageShaderVertex vertex = ((ImageShaderVertex*)vertexBuffer)[vertexIdx];
	RasterizationData out;
	out.position = glm::vec4(vertex.position.x, vertex.position.y, 1.0f, 1.0f);
	out.extraData.push_back(glm::vec4(vertex.uv.x, vertex.uv.y, 1.0f, 1.0f));
	return out;
}

glm::vec4 ImageShader::fragmentShader(const RasterizationData & rasterizationData)
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