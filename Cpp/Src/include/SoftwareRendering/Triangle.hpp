#pragma once
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

struct Triangle
{
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	Triangle()
		:a(glm::identity<glm::vec3>()), b(glm::identity<glm::vec3>()), c(glm::identity<glm::vec3>())
	{

	}
	Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		:a(a), b(b), c(c)
	{

	}
};