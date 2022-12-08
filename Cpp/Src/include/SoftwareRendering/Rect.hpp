#pragma once

#include <glm/glm.hpp>

struct Rect
{
	double x;
	double y;
	double width;
	double height;

	static Rect boundingBox(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) noexcept;
};