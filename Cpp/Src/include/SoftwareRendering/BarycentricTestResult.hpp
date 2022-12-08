#pragma once
#include <functional>
#include <glm/glm.hpp>

struct BarycentricTestResult
{
	bool isInsideTriangle;
	double w1;
	double w2;
	double w3;

	glm::vec3 weight() const noexcept;

	static BarycentricTestResult test(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c, const double x, const double y) noexcept;
};