#pragma once

#include <functional>
#include <vector>

#include "DepthFunc.hpp"
#include "Shader.hpp"

class RenderPipeline
{
public:
	DepthFunc::closure depthFunc = DepthFunc::less;
	void* vertexBuffer = nullptr;
	Shader* shader = nullptr;
	int triangleCount = 0;
};
