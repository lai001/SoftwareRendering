#pragma once
#include <array>
#include <functional>

#include "glm/glm.hpp"

enum class BufferType
{
	data,
	z
};

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

public:
	int getWidth() const;
	int getHeight() const;

private:
	int width = 0;
	int height = 0;
	unsigned char* data = nullptr;
	double* zBuffer = nullptr;

public:
	glm::ivec2 ndcPointToPixelIndex(const glm::vec2 point) const;
	int pixelIndexToBufferIndex(const glm::ivec2 index) const;
	int ndcPointToBufferIndex(const glm::vec2 point) const;

	glm::vec3 getPixel(const glm::vec2 point) const;
	void setPixel(const glm::vec2 point, const glm::vec3 color);

	void setPixel(const glm::vec3 point, const glm::vec3 color, const std::function<bool(double, double)> depthFunc);

	double zValueAtNdcPoint(const glm::vec3 point) const;

	void flush();
	void clear(const glm::vec3 color);

	double const * const getZBuffer() const;
	unsigned char const * const getData() const;

	double* mutableZBuffer();
	unsigned char* mutableData();
};
