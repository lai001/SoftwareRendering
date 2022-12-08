#include "FrameBuffer.hpp"
#include <assert.h>

#include "spdlog/spdlog.h"

#include "Util.hpp"

FrameBuffer::FrameBuffer(int width, int height)
	:width(width), height(height)
{
	assert(width >= 0 && height >= 0);
	int length = width * height;
	data = new unsigned char[length * 3];
	zBuffer = new double[length];

	std::fill_n(zBuffer, length, 1.0);
	std::fill_n(data, length * 3, (unsigned char)0);
}

FrameBuffer::~FrameBuffer()
{
	delete[] data;
	delete[] zBuffer;
}

int FrameBuffer::getWidth() const
{
	return width;
}

int FrameBuffer::getHeight() const
{
	return height;
}

glm::ivec2 FrameBuffer::ndcPointToPixelIndex(const glm::vec2 point) const
{
	const double min = -1.0;
	const double max = 1.0;
	const double d = max - min;
	assert(point.x >= min && point.x <= max);
	assert(point.y >= min && point.y <= max);
	int x = (point.x - min) / d * (double)(width - 1);
	int y = (point.y - min) / d * (double)(height - 1);
	y = height - 1 - y;
	return glm::ivec2(x, y);
}

int FrameBuffer::pixelIndexToBufferIndex(const glm::ivec2 index) const
{
	int bufferIndex = index.y * height + index.x;
	assert(bufferIndex >= 0 && bufferIndex < width * height);
	return index.y * height + index.x;
}

int FrameBuffer::ndcPointToBufferIndex(const glm::vec2 point) const
{
	glm::ivec2 index = ndcPointToPixelIndex(point);
	int bufferIndex = pixelIndexToBufferIndex(index);
	return bufferIndex;
}

glm::vec3 FrameBuffer::getPixel(const glm::vec2 point) const
{
	int bufferIndex = ndcPointToBufferIndex(point);
	int start = bufferIndex * 3;
	return glm::vec3((double)data[start] / 255.0, (double)data[start + 1] / 255.0, (double)data[start + 2] / 255.0);
}

void FrameBuffer::setPixel(const glm::vec2 point, const glm::vec3 color)
{
	const glm::ivec2 index = ndcPointToPixelIndex(point);
	const int bufferIndex = pixelIndexToBufferIndex(index);
	int start = bufferIndex * 3;
	data[start] = color.r * 255.0;
	data[start + 1] = color.g * 255.0;
	data[start + 2] = color.b * 255.0;
}

void FrameBuffer::setPixel(const glm::vec3 point, const glm::vec3 color, const std::function<bool(double, double)> depthFunc)
{
	const double zValue = zValueAtNdcPoint(point);
	const bool isPass = depthFunc(point.z, zValue);
	if (isPass)
	{
		zBuffer[ndcPointToBufferIndex(point)] = point.z;
		setPixel(glm::vec2(point.x, point.y), color);
	}
}

double FrameBuffer::zValueAtNdcPoint(const glm::vec3 point) const
{
	return zBuffer[ndcPointToBufferIndex(point)];
}

void FrameBuffer::flush()
{
	const int length = width * height;
	std::fill_n(zBuffer, length, 1.0);
	std::fill_n(data, length * 3, (unsigned char)0);
}

void FrameBuffer::clear(const glm::vec3 color)
{
	const int length = width * height;
	for (int i = 0; i < length; i++)
	{
		data[i * 3] = (unsigned char)(color.r * 255.0);
		data[i * 3 + 1] = (unsigned char)(color.g * 255.0);
		data[i * 3 + 2] = (unsigned char)(color.b * 255.0);
	}
}

double const * const FrameBuffer::getZBuffer() const
{
	return zBuffer;
}

unsigned char const * const FrameBuffer::getData() const
{
	return data;
}

double * FrameBuffer::mutableZBuffer()
{
	return zBuffer;
}

unsigned char * FrameBuffer::mutableData()
{
	return data;
}
