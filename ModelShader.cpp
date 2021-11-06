#include "ModelShader.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "Util.hpp"

RasterizationData ModelShader::vertexShader(const void * vertexBuffer, const int vertexIdx)
{
	BaseVertex vertex = ((BaseVertex*)vertexBuffer)[vertexIdx];
	RasterizationData out;
	out.position = glm::vec4(vertex.position, 1.0f);
	out.extraData.push_back(glm::vec4(vertex.color, 1.0));
	glm::mat4x4 mvpMat = projectionMat * viewMat * modelMat;
	out.position = mvpMat * out.position;
	return out;
}

glm::vec4 ModelShader::fragmentShader(const RasterizationData & rasterizationData)
{
	return rasterizationData.extraData[0];
}
