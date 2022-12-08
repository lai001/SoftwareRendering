#pragma once
#include <random>
#include <functional>
#include <filesystem>

#include "glm/glm.hpp"
#include "stb_image.h"

#include "BarycentricTestResult.hpp"

namespace Color
{
	static constexpr glm::vec3 red(1.0, 0.0, 0.0);
	static constexpr glm::vec3 blue(0.0, 0.0, 1.0);
	static constexpr glm::vec3 gree(0.0, 1.0, 0.0);
	static constexpr glm::vec3 yellow(1.0, 1.0, 0.0);
	static constexpr glm::vec3 white(1.0);
	static constexpr glm::vec3 black(0.0);
}

double remapping(const double value, const double fromLower, const double fromUpper, const double toLower, const double toUpper);

glm::vec3 randomColor();

std::string getFolder(const std::string filename);

double interpolation(const glm::vec3 weight, const glm::vec3 value);

glm::vec4 interpolation(const glm::vec3 weight, const glm::vec4 v0, const glm::vec4 v1, const glm::vec4 v2);

glm::vec3 interpolation(const glm::vec3 weight, const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2);

glm::vec2 interpolation(const glm::vec3 weight, const glm::vec2 v0, const glm::vec2 v1, const glm::vec2 v2);

glm::vec4 vec4Correction(const glm::vec4 c0, const glm::vec4 c1, const glm::vec4 c2,
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce);

glm::vec3 vec3Correction(const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2,
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce);

float zCorrection(const float c0, const float c1, const float c2,
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce);

glm::vec4 divideByW(const glm::vec4 vec4);

glm::vec4 divideByNegativeW(const glm::vec4 vec4);

/*
Copy from stb_image.
*/
void stbi__vertical_flip(void* image, int w, int h, int bytes_per_pixel);