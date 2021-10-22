#pragma once
#include <functional>

#include "FrameBuffer.hpp"
#include "Rect.hpp"
#include "BarycentricTestResult.hpp"
#include "Line2D.hpp"

struct DepthFunc
{
	static bool always(const double inputZ, const double z)
	{
		return true;
	}

	static bool never(const double inputZ, const double z)
	{
		return false;
	}

	static bool less(const double inputZ, const double z)
	{
		return inputZ < z;
	}

	static bool equal(const double inputZ, const double z)
	{
		return inputZ == z;
	}

	static bool lequal(const double inputZ, const double z)
	{
		return inputZ <= z;
	}

	static bool greater(const double inputZ, const double z)
	{
		return inputZ > z;
	}

	static bool notequal(const double inputZ, const double z)
	{
		return inputZ != z;
	}


	static bool gequal(const double inputZ, const double z)
	{
		return inputZ >= z;
	}
};


enum PolygonModeType
{
	line,
	fill
};

class Renderer
{
public:
	Renderer(int width, int height);
	~Renderer();

private:
	FrameBuffer* frameBuffer = nullptr;

public:
	FrameBuffer const * const getFrameBuffer() const;
	void flush() const;
	void clear(glm::vec3 color);

	int getWidth() const;
	int getHeight() const;

	void setColor(const glm::vec3 point, const glm::vec3 color, const std::function<bool(double, double)> depthFunc) const;
	bool isAvailable(const glm::vec3 point, const std::function<bool(double, double)> depthFunc) const;

	void addLine2D(const glm::vec2 p0, const glm::vec2 p1, const glm::vec3 color) const;
	void addLine2D(const Line2D line2D, const glm::vec3 color) const;

	void addTriangle2D(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c,
		const glm::vec3 color,
		const PolygonModeType polygonModeType) const;
	void addTriangle2D(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2,
		const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2) const;

	void addTriangle3D(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2,
		const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2,
		const std::function<bool(double, double)> depthFunc) const;
	void addTriangle3D(const glm::vec4 p0, const glm::vec4 p1, const glm::vec4 p2,
		const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2,
		const std::function<bool(double, double)> depthFunc) const;

	bool isValidTriangle(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c) const;
};