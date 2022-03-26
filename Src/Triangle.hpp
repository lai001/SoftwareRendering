#pragma once
#include "glm/glm.hpp"

struct Triangle
{
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	Triangle()
	{

	}
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		:a(a), b(b), c(c)
	{

	}
};