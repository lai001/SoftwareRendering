#pragma once
#include <random>
#include <functional>

#include "glm/glm.hpp"

#include "BarycentricTestResult.hpp"

namespace Color
{
	static const glm::vec3 red(1.0, 0.0, 0.0);
	static const glm::vec3 blue(0.0, 0.0, 1.0);
	static const glm::vec3 gree(0.0, 1.0, 0.0);
	static const glm::vec3 yellow(1.0, 1.0, 0.0);
	static const glm::vec3 white(1.0);
	static const glm::vec3 black(0.0);
}

static glm::u8vec3 denormalColor(const glm::vec3 color)
{
	return glm::vec3(color.r * 255.0, color.g * 255.0, color.b * 255.0);
}

static double rangeMap(const double v, const double inputLower, const double inputUpper, const double outputLower, const double outputUpper)
{
	return outputLower + ((outputUpper - outputLower) / (inputUpper - inputLower)) * (v - inputLower);
}

static glm::vec3 randomColor()
{
	std::function<double()> randomDoubleValue = []() {
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, 255);
		return dist(rd) / 255.0;
	};

	glm::vec3 color;
	color.r = randomDoubleValue();
	color.g = randomDoubleValue();
	color.b = randomDoubleValue();
	return color;
}

static std::string getFolder(const std::string filename)
{
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx);
	}
	return directory;
}

static unsigned char clamp(const double v)
{
	return (unsigned char)std::max(0.0, std::min(255.0, v * 255.0));
}

template <typename T>
static T clamp(const T v, const T min, const T max)
{
	return (T)std::max(min, std::min(max, v));
}

static double interpolation(const glm::vec3 weight, const glm::vec3 value)
{
	return glm::dot(weight, value);
	return weight.x * value.x + weight.y * value.y + weight.z * value.z;
}

static glm::vec3 interpolation(const glm::vec3 weight, const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2)
{
	glm::vec3 value;
	value.x = interpolation(weight, glm::vec3(v0.x, v1.x, v2.x));
	value.y = interpolation(weight, glm::vec3(v0.y, v1.y, v2.y));
	value.z = interpolation(weight, glm::vec3(v0.z, v1.z, v2.z));
	return value;
}

static glm::vec2 interpolation(const glm::vec3 weight, const glm::vec2 v0, const glm::vec2 v1, const glm::vec2 v2)
{
	glm::vec2 value;
	value.x = interpolation(weight, glm::vec3(v0.x, v1.x, v2.x));
	value.y = interpolation(weight, glm::vec3(v0.y, v1.y, v2.y));
	//value.z = 0;
	return value;
}

static glm::vec3 vec3Correction(const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2, 
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce)
{
	if (z0 == z1 && z1 == z2)
	{
		glm::vec3 weightAtScreenSapce = testResultAtScreenSapce.weight();
		const double x = glm::dot(glm::vec3(c0.x, c1.x, c2.x), weightAtScreenSapce);
		const double y = glm::dot(glm::vec3(c0.y, c1.y, c2.y), weightAtScreenSapce);
		const double z = glm::dot(glm::vec3(c0.z, c1.z, c2.z), weightAtScreenSapce);
		return glm::vec3(x, y, z);
	}
	const double zAtWorldSpace = 1.0 / ((testResultAtScreenSapce.w1 / z0) + (testResultAtScreenSapce.w2 / z1) + (testResultAtScreenSapce.w3 / z2));
	BarycentricTestResult testResultAtWorldSpace;
	testResultAtWorldSpace.w1 = zAtWorldSpace * testResultAtScreenSapce.w1 / z0;
	testResultAtWorldSpace.w2 = zAtWorldSpace * testResultAtScreenSapce.w2 / z1;
	testResultAtWorldSpace.w3 = zAtWorldSpace * testResultAtScreenSapce.w3 / z2;
	glm::vec3 weightAtWorldSpace = testResultAtWorldSpace.weight();
	const double x = glm::dot(glm::vec3(c0.x, c1.x, c2.x), weightAtWorldSpace);
	const double y = glm::dot(glm::vec3(c0.y, c1.y, c2.y), weightAtWorldSpace);
	const double z = glm::dot(glm::vec3(c0.z, c1.z, c2.z), weightAtWorldSpace);
	return glm::vec3(x, y, z);
}

static glm::vec4 divideByW(const glm::vec4 vec4)
{
	return glm::vec4(vec4.x / vec4.w, vec4.y / vec4.w, vec4.z / vec4.w, vec4.z);
}

static glm::vec4 divideByNegativeW(const glm::vec4 vec4)
{
	return glm::vec4(vec4.x / -vec4.w, vec4.y / -vec4.w, vec4.z / -vec4.w, vec4.z);
}