#include "Util.hpp"

double remapping(const double value, const double fromLower, const double fromUpper, const double toLower, const double toUpper)
{
	return toLower + ((toUpper - toLower) / (fromUpper - fromLower)) * (value - fromLower);
}

glm::vec3 randomColor()
{
	std::function<double()> randomDoubleValue = []() {
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, 255);
		return dist(rd) / 255.0;
	};

	glm::vec3 color;
	color.r = (float)randomDoubleValue();
	color.g = (float)randomDoubleValue();
	color.b = (float)randomDoubleValue();
	return color;
}

std::string getFolder(const std::string filename)
{
	const std::filesystem::path path(filename);
	const std::string directory = path.parent_path().string();
	return directory;
}

double interpolation(const glm::vec3 weight, const glm::vec3 value)
{
	return glm::dot(weight, value);
}

glm::vec4 interpolation(const glm::vec3 weight, const glm::vec4 v0, const glm::vec4 v1, const glm::vec4 v2)
{
	glm::vec4 value;
	value.x = (float)interpolation(weight, glm::vec3(v0.x, v1.x, v2.x));
	value.y = (float)interpolation(weight, glm::vec3(v0.y, v1.y, v2.y));
	value.z = (float)interpolation(weight, glm::vec3(v0.z, v1.z, v2.z));
	value.w = (float)interpolation(weight, glm::vec3(v0.w, v1.w, v2.w));
	return value;
}

glm::vec3 interpolation(const glm::vec3 weight, const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2)
{
	glm::vec3 value;
	value.x = (float)interpolation(weight, glm::vec3(v0.x, v1.x, v2.x));
	value.y = (float)interpolation(weight, glm::vec3(v0.y, v1.y, v2.y));
	value.z = (float)interpolation(weight, glm::vec3(v0.z, v1.z, v2.z));
	return value;
}

glm::vec2 interpolation(const glm::vec3 weight, const glm::vec2 v0, const glm::vec2 v1, const glm::vec2 v2)
{
	glm::vec2 value;
	value.x = (float)interpolation(weight, glm::vec3(v0.x, v1.x, v2.x));
	value.y = (float)interpolation(weight, glm::vec3(v0.y, v1.y, v2.y));
	//value.z = 0;
	return value;
}

glm::vec4 vec4Correction(const glm::vec4 c0, const glm::vec4 c1, const glm::vec4 c2,
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce)
{
	if (z0 == z1 && z1 == z2)
	{
		glm::vec3 weightAtScreenSapce = testResultAtScreenSapce.weight();
		const double x = glm::dot(glm::vec3(c0.x, c1.x, c2.x), weightAtScreenSapce);
		const double y = glm::dot(glm::vec3(c0.y, c1.y, c2.y), weightAtScreenSapce);
		const double z = glm::dot(glm::vec3(c0.z, c1.z, c2.z), weightAtScreenSapce);
		const double w = glm::dot(glm::vec3(c0.w, c1.w, c2.w), weightAtScreenSapce);
		return glm::vec4(x, y, z, w);
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
	const double w = glm::dot(glm::vec3(c0.w, c1.w, c2.w), weightAtWorldSpace);
	return glm::vec4(x, y, z, w);
}

glm::vec3 vec3Correction(const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2,
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

float zCorrection(const float c0, const float c1, const float c2,
	const double z0, const double z1, const double z2,
	const BarycentricTestResult testResultAtScreenSapce)
{
	if (z0 == z1 && z1 == z2)
	{
		glm::vec3 weightAtScreenSapce = testResultAtScreenSapce.weight();
		const double z = c0;
		return (float)z;
	}
	const double zAtWorldSpace = 1.0 / ((testResultAtScreenSapce.w1 / z0) + (testResultAtScreenSapce.w2 / z1) + (testResultAtScreenSapce.w3 / z2));
	BarycentricTestResult testResultAtWorldSpace;
	testResultAtWorldSpace.w1 = zAtWorldSpace * testResultAtScreenSapce.w1 / z0;
	testResultAtWorldSpace.w2 = zAtWorldSpace * testResultAtScreenSapce.w2 / z1;
	testResultAtWorldSpace.w3 = zAtWorldSpace * testResultAtScreenSapce.w3 / z2;
	glm::vec3 weightAtWorldSpace = testResultAtWorldSpace.weight();
	const double z = glm::dot(glm::vec3(c0, c1, c2), weightAtWorldSpace);
	return (float)z;
}

glm::vec4 divideByW(const glm::vec4 vec4)
{
	return glm::vec4(vec4.x / vec4.w, vec4.y / vec4.w, vec4.z / vec4.w, vec4.z);
}

glm::vec4 divideByNegativeW(const glm::vec4 vec4)
{
	return glm::vec4(vec4.x / -vec4.w, vec4.y / -vec4.w, vec4.z / -vec4.w, vec4.z);
}

/*
Copy from stb_image.
*/
void stbi__vertical_flip(void* image, int w, int h, int bytes_per_pixel)
{
	int row;
	size_t bytes_per_row = (size_t)w * bytes_per_pixel;
	stbi_uc temp[2048];
	stbi_uc* bytes = (stbi_uc*)image;

	for (row = 0; row < (h >> 1); row++) {
		stbi_uc* row0 = bytes + row * bytes_per_row;
		stbi_uc* row1 = bytes + (h - row - 1) * bytes_per_row;
		// swap row0 with row1
		size_t bytes_left = bytes_per_row;
		while (bytes_left) {
			size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
			memcpy(temp, row0, bytes_copy);
			memcpy(row0, row1, bytes_copy);
			memcpy(row1, temp, bytes_copy);
			row0 += bytes_copy;
			row1 += bytes_copy;
			bytes_left -= bytes_copy;
		}
	}
}