#include "BarycentricTestResult.hpp"


glm::vec3 BarycentricTestResult::weight() const noexcept
{
	return glm::vec3(w1, w2, w3);
}

BarycentricTestResult BarycentricTestResult::test(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c, const double x, const double y) noexcept
{
	const std::function<bool(double)> check = [](double value) {
		return (value >= 0) && (value <= 1.0);
	};

	BarycentricTestResult result;

	//double w1 = a.x * (c.y - a.y) + (y - a.y) * (c.x - a.x) - x * (c.y - a.y);
	//w1 = w1 / ((b.y - a.y) * (c.x - a.x) - (b.x - a.x) * (c.y - a.y));

	//double w2 = y - a.y - w1 * (b.y - a.y);
	//w2 = w2 / (c.y - a.y);

	//double w3 = 1 - w1 - w2;

	const glm::vec3 u = glm::cross(glm::vec3(c.x - a.x, b.x - a.x, a.x - x), glm::vec3(c.y - a.y, b.y - a.y, a.y - y));
	const double w1 = 1.0 - (u.x + u.y) / u.z;
	const double w2 = u.y / u.z;
	const double w3 = u.x / u.z;

	result.w1 = w1;
	result.w2 = w2;
	result.w3 = w3;

	if (check(result.w1) && check(result.w2) && check(result.w3))
	{
		result.isInsideTriangle = true;
	}
	else
	{
		result.isInsideTriangle = false;
	}

	return result;
}
