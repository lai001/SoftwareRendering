#include "Rect.hpp"
#include <algorithm>

Rect Rect::boundingBox(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) noexcept
{
	Rect rect;
	rect.x = std::min({ a.x, b.x, c.x });
	rect.y = std::min({ a.y, b.y, c.y });
	rect.width = std::max({ a.x, b.x, c.x }) - rect.x;
	rect.height = std::max({ a.y, b.y, c.y }) - rect.y;
	return rect;
}