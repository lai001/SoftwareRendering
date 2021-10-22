#pragma once
#include "glm/glm.hpp"
struct Line2D
{
	const glm::vec2 p0;
	const glm::vec2 p1;

	Line2D(glm::vec2 p0, glm::vec2 p1)
		:p0(p0), p1(p1)
	{

	}
};