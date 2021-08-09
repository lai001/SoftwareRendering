#include "PPM.hpp"
#include <algorithm>
#include <vector>
#include <functional>

#include "spdlog/spdlog.h"

static unsigned char clamp(double v)
{
	return (unsigned char)std::max(0.0, std::min(255.0, v * 255.0));
}

PPM::PPM(int width, int height)
	:width(width), height(height)
{
	int l = width * height * sizeof(unsigned char);
	r = new unsigned char[l];
	g = new unsigned char[l];
	b = new unsigned char[l];
	zBuffer = new double[l];

	std::fill_n(zBuffer, l, 1.0);
	std::fill_n(r, l, 0);
	std::fill_n(g, l, 0);
	std::fill_n(b, l, 0);
}

PPM::~PPM()
{
	if (r)
	{
		delete[] r;
	}
	if (g)
	{
		delete[] g;
	}
	if (b)
	{
		delete[] b;
	}
	if (zBuffer)
	{
		delete[] zBuffer;
	}
}

void PPM::reset()
{
	int l = width * height * sizeof(unsigned char);
	std::fill_n(zBuffer, l, 1.0);
	std::fill_n(r, l, 0);
	std::fill_n(g, l, 0);
	std::fill_n(b, l, 0);
}

void PPM::writePPMFileHeader(std::ofstream * f)
{
	if (f)
	{
		*f << "P3" << std::endl;
		*f << std::to_string(width) << " " << std::to_string(height) << std::endl;
		*f << "255" << std::endl;
	}
}

void PPM::writeToFile(std::string filename)
{
	if (r, g, b)
	{
		std::ofstream f(filename);
		writePPMFileHeader(&f);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = i * width + j;
				const unsigned char _r = r[idx];
				const unsigned char _g = g[idx];
				const unsigned char _b = b[idx];

				f << std::to_string(_r) << " ";
				f << std::to_string(_g) << " ";
				f << std::to_string(_b) << " ";
			}
			f << std::endl;
		}
	}
}

void PPM::writeZBufferToFile(std::string filename)
{
	if (zBuffer)
	{
		std::ofstream f(filename);
		writePPMFileHeader(&f);
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = i * width + j;
				const unsigned char z = zBuffer[idx] * 255.0;
				const unsigned char _r = z;
				const unsigned char _g = z;
				const unsigned char _b = z;

				f << std::to_string(_r) << " ";
				f << std::to_string(_g) << " ";
				f << std::to_string(_b) << " ";
			}
			f << std::endl;
		}
	}
}

int PPM::pointToIndex(glm::vec3 point, bool isEnableDepthTest)
{
	int y = height / 2 - (int)point.y;
	int x = (int)point.x + width / 2;
	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		int idx = y * width + x;

		if (isEnableDepthTest)
		{
			if (point.z < zBuffer[idx])
			{
				zBuffer[idx] = point.z;
				return idx;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return idx;
		}
	}
	else
	{
		return -1;
	}
}

void PPM::setColor(glm::vec3 point, glm::vec3 color, bool isEnableDepthTest)
{
	int idx = pointToIndex(point, isEnableDepthTest);
	if (idx != -1)
	{
		r[idx] = clamp(color.r);
		g[idx] = clamp(color.g);
		b[idx] = clamp(color.b);
	}
}

void PPM::addLine(glm::vec2 p0, glm::vec2 p1, glm::vec3 color)
{
	double a = (p1.y - p0.y) / (p1.x - p0.x);
	double b = p0.y - a * p0.x;
	int s = (int)std::min(p0.x, p1.x);
	int e = (int)std::max(p0.x, p1.x);
	for (int x = s; x <= e; x++) {
		double y = a * x + b;
		setColor({ (double)x, y, 0.0 }, color, false);
	}
}

void PPM::addTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec3 color)
{
	addLine(a, b, color);
	addLine(b, c, color);
	addLine(c, a, color);
}

void PPM::addTriangleFillWithColor(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec3 color)
{
	std::vector<glm::vec2> points = { a, b, c };

	std::sort(points.begin(), points.end(), [](glm::vec2 lhs, glm::vec2 rhs) {
		return lhs.y < rhs.y;
	});

	std::function<void(Line2D, Line2D, int, int)> scanline = [this, color](Line2D l1, Line2D l2, int y0, int y1) {
		for (int y = y0; y < y1; y++)
		{
			double x0 = l1.x(y);
			double x1 = l2.x(y);
			for (int x = std::min(x0, x1); x < std::max(x0, x1); x++)
			{
				setColor({ (double)x, (double)y, 0.0 }, color, false);
			}
		}
	};

	scanline(Line2D(points[0], points[1]),
		Line2D(points[0], points[2]),
		points[0].y,
		points[1].y);

	scanline(Line2D(points[1], points[2]),
		Line2D(points[0], points[2]),
		points[1].y,
		points[2].y);
}

Rect PPM::boundingBox(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
	Rect rect;
	rect.x = std::min({ a.x, b.x, c.x });
	rect.y = std::min({ a.y, b.y, c.y });
	rect.width = std::max({ a.x, b.x, c.x }) - rect.x;
	rect.height = std::max({ a.y, b.y, c.y }) - rect.y;
	return rect;
}

BarycentricTestResult PPM::barycentricTest(glm::vec2 a, glm::vec2 b, glm::vec2 c, double x, double y)
{
	std::function<bool(double)> check = [](double value) {
		return (value >= 0) && (value <= 1.0);
	};

	BarycentricTestResult result;

	double cx = x - c.x;
	double ax = b.x - c.x;
	double bx = a.x - c.x;

	double cy = y - c.y;
	double ay = b.y - c.y;
	double by = a.y - c.y;

	double u = (cy * ax - cx * ay) / (by * ax - bx * ay);
	double v = (cx - u * bx) / ax;
	double w = 1 - u - v;

	if (check(u) && check(v) && check(w))
	{
		result.isInsideTriangle = true;
	}
	else
	{
		result.isInsideTriangle = false;
	}
	result.u = u;
	result.v = v;
	result.w = w;
	return result;
}

Vertex PPM::interpolation(BarycentricTestResult testResult, Triangle triangle)
{
	assert(testResult.isInsideTriangle);
	Vertex v;
	v.point.x = triangle.a.point.x * testResult.u +
		triangle.b.point.x * testResult.v +
		triangle.c.point.x * testResult.w;

	v.point.y = triangle.a.point.y * testResult.u +
		triangle.b.point.y * testResult.v +
		triangle.c.point.y * testResult.w;

	v.point.z = triangle.a.point.z * testResult.u +
		triangle.b.point.z * testResult.v +
		triangle.c.point.z * testResult.w;

	v.color.r = triangle.a.color.r * testResult.u +
		triangle.b.color.r * testResult.v +
		triangle.c.color.r * testResult.w;

	v.color.g = triangle.a.color.g * testResult.u +
		triangle.b.color.g * testResult.v +
		triangle.c.color.g * testResult.w;

	v.color.b = triangle.a.color.b * testResult.u +
		triangle.b.color.b * testResult.v +
		triangle.c.color.b * testResult.w;

	return v;
}

void PPM::addTriangleFillWithColor2(Triangle triangle)
{
	glm::vec2 a(triangle.a.point.x, triangle.a.point.y);
	glm::vec2 b(triangle.b.point.x, triangle.b.point.y);
	glm::vec2 c(triangle.c.point.x, triangle.c.point.y);
	Rect box = boundingBox(a, b, c);

	for (int y = box.y; y <= box.y + box.height; y++)
	{
		for (int x = box.x; x <= box.x + box.width; x++)
		{
			BarycentricTestResult testResult = barycentricTest(a, b, c, x, y);
			if (testResult.isInsideTriangle)
			{
				Vertex interpolationVertex = interpolation(testResult, triangle);
				setColor({ (double)x, (double)y, interpolationVertex.point.z }, interpolationVertex.color, true);
			}
		}
	}
}

void PPM::addTriangleFillWithColor2(Triangle triangle, glm::vec3 color)
{
	glm::vec2 a(triangle.a.point.x, triangle.a.point.y);
	glm::vec2 b(triangle.b.point.x, triangle.b.point.y);
	glm::vec2 c(triangle.c.point.x, triangle.c.point.y);
	Rect box = boundingBox(a, b, c);

	for (int y = box.y; y <= box.y + box.height; y++)
	{
		for (int x = box.x; x <= box.x + box.width; x++)
		{
			BarycentricTestResult testResult = barycentricTest(a, b, c, x, y);
			if (testResult.isInsideTriangle)
			{
				Vertex interpolationVertex = interpolation(testResult, triangle);
				setColor({ (double)x, (double)y, interpolationVertex.point.z }, color, true);
			}
		}
	}
}