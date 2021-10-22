#include "Renderer.hpp"

#include "spdlog/spdlog.h"

#include "Util.hpp"
#include "Line2D.hpp"

Renderer::Renderer(int width, int height)
	:frameBuffer(new FrameBuffer(width, height))
{

}

Renderer::~Renderer()
{
	delete frameBuffer;
}

FrameBuffer const * const Renderer::getFrameBuffer() const
{
	return frameBuffer;
}

void Renderer::flush() const
{
	frameBuffer->flush();
}

void Renderer::clear(glm::vec3 color)
{
	frameBuffer->clear(color);
}

int Renderer::getWidth() const
{
	return frameBuffer->getWidth();
}

int Renderer::getHeight() const
{
	return frameBuffer->getHeight();
}

void Renderer::setColor(const glm::vec3 point, const glm::vec3 color, const std::function<bool(double, double)> depthFunc) const
{
	if (isAvailable(point, depthFunc))
	{
		frameBuffer->setPixel(point, color, depthFunc);
	}
}

bool Renderer::isAvailable(const glm::vec3 point, const std::function<bool(double, double)> depthFunc) const
{
	std::function<bool(double)> check = [](double v) {
		return v >= -1.0 && v <= 1.0;
	};

	if (check(point.x) && check(point.y))
	{
		double z = frameBuffer->zValueAtNdcPoint(point);
		bool isPass = depthFunc(point.z, z);
		if (isPass)
		{
			return true;
		}
	}

	return false;
}

void Renderer::addLine2D(const Line2D line2D, const glm::vec3 color) const
{
	addLine2D(line2D.p0, line2D.p1, color);
}

void Renderer::addLine2D(const glm::vec2 p0, const glm::vec2 p1, const glm::vec3 color) const
{
	if (p0.x == p1.x)
	{
		double y = std::min(p0.y, p1.y);
		const double e = std::max(p0.y, p1.y);
		const double step = 1.0 / (double)getHeight();
		while (y < e)
		{
			const double x = p0.x;
			setColor(glm::vec3((double)x, y, 0.0), color, DepthFunc::always);
			y = y + step;
		}
	}
	else if (p0.y == p1.y)
	{
		double x = std::min(p0.x, p1.x);
		const double e = std::max(p0.x, p1.x);
		const double step = 1.0 / (double)getWidth();
		while (x < e)
		{
			const double y = p0.y;
			setColor(glm::vec3((double)x, y, 0.0), color, DepthFunc::always);
			x = x + step;
		}
	}
	else
	{
		const double k = (p1.y - p0.y) / (p1.x - p0.x);
		const double b = p0.y - k * p0.x;
		if (k > 1.0)
		{
			double y = std::min(p0.y, p1.y);
			const double e = std::max(p0.y, p1.y);
			const double step = 1.0 / (double)getHeight();
			while (y < e)
			{
				const double x = (y - b) / k;
				setColor(glm::vec3(x, y, 0.0), color, DepthFunc::always);
				y = y + step;
			}
		}
		else
		{
			double x = std::min(p0.x, p1.x);
			const double e = std::max(p0.x, p1.x);
			const double step = 1.0 / (double)getWidth();
			while (x < e)
			{
				const double y = k * x + b;
				setColor(glm::vec3(x, y, 0.0), color, DepthFunc::always);
				x = x + step;
			}
		}
	}
}

void Renderer::addTriangle2D(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c, 
	const glm::vec3 color,
	const PolygonModeType polygonModeType) const
{
	if (isValidTriangle(a, b, c) == false)
	{
		return;
	}

	switch (polygonModeType)
	{
	case PolygonModeType::line:
		addLine2D(a, b, color);
		addLine2D(b, c, color);
		addLine2D(c, a, color);
		break;

	case PolygonModeType::fill:
		std::vector<glm::vec2> points = { a, b, c };

		std::sort(points.begin(), points.end(), [](glm::vec2 lhs, glm::vec2 rhs) {
			return lhs.y < rhs.y;
		});

		std::function<double(double, glm::vec2, glm::vec2)> calcX = [](double y, glm::vec2 p0, glm::vec2 p1) {
			if (p0.x == p1.x)
			{
				return (double)p0.x;
			}
			else
			{
				const double k = (p1.y - p0.y) / (p1.x - p0.x);
				const double b = p0.y - k * p0.x;
				const double x = (y - b) / k;
				return x;
			}
		};

		std::function<void(Line2D, Line2D, double, double)> scanline = [this, color, calcX](Line2D l1, Line2D l2, double y0, double y1) {
			double step = 1.0 / (double)getHeight();
			double xStep = 1.0 / (double)getWidth();
			double y = y0;
			while (y < y1)
			{
				double x0 = calcX(y, l1.p0, l1.p1);
				double x1 = calcX(y, l2.p0, l2.p1);
				double minX = std::min(x0, x1);
				double maxX = std::max(x0, x1);
				double x = minX;
				while (x < maxX)
				{
					setColor({ (double)x, (double)y, 0.0 }, color, DepthFunc::always);
					x = x + xStep;
				}
				y = y + step;
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
		break;
	}
}

void Renderer::addTriangle2D(const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2,
	const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2) const
{
	if (isValidTriangle(p0, p1, p2) == false)
	{
		return;
	}
	Rect box = Rect::boundingBox(p0, p1, p2);

	for (double y = box.y; y <= box.y + box.height; y += (double)1.0 / (double)getHeight())
	{
		for (double x = box.x; x <= box.x + box.width; x += (double)1.0 / (double)getWidth())
		{
			BarycentricTestResult testResult = BarycentricTestResult::test(p0, p1, p2, x, y);
			if (testResult.isInsideTriangle)
			{
				glm::vec2 interpolationP = interpolation(testResult.weight(), p0, p1, p2);
				glm::vec3 interpolationC = interpolation(testResult.weight(), c0, c1, c2);
				setColor(glm::vec3(interpolationP, 0.0), interpolationC, DepthFunc::always);
			}
		}
	}
}

void Renderer::addTriangle3D(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, 
	const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2, 
	const std::function<bool(double, double)> depthFunc) const
{
	if (isValidTriangle(p0, p1, p2) == false)
	{
		return;
	}
	Rect box = Rect::boundingBox(p0, p1, p2);

	for (double y = box.y; y <= box.y + box.height; y += (double)1.0 / (double)getHeight())
	{
		for (double x = box.x; x <= box.x + box.width; x += (double)1.0 / (double)getWidth())
		{
			BarycentricTestResult testResult = BarycentricTestResult::test(p0, p1, p2, x, y);
			if (testResult.isInsideTriangle)
			{
				glm::vec3 interpolationP = interpolation(testResult.weight(), p0, p1, p2);
				glm::vec3 interpolationC = interpolation(testResult.weight(), c0, c1, c2);

				setColor(interpolationP, interpolationC, depthFunc);
			}
		}
	}
}

void Renderer::addTriangle3D(const glm::vec4 p0, const glm::vec4 p1, const glm::vec4 p2, 
	const glm::vec3 c0, const glm::vec3 c1, const glm::vec3 c2, 
	const std::function<bool(double, double)> depthFunc) const
{
	glm::vec4 a = divideByW(p0);
	glm::vec4 b = divideByW(p1);
	glm::vec4 c = divideByW(p2);
	if (isValidTriangle(a, b, c) == false)
	{
		return;
	}
	Rect box = Rect::boundingBox(a, b, c);

	for (double y = box.y; y <= box.y + box.height; y += (double)1.0 / (double)getHeight())
	{
		for (double x = box.x; x <= box.x + box.width; x += (double)1.0 / (double)getWidth())
		{
			BarycentricTestResult testResult = BarycentricTestResult::test(a, b, c, x, y);
			if (testResult.isInsideTriangle)
			{
				glm::vec3 interpolationP = interpolation(testResult.weight(), glm::vec3(a), glm::vec3(b), glm::vec3(c));
				glm::vec3 interpolationC = interpolation(testResult.weight(), c0, c1, c2);
				
				const glm::vec3 cc = vec3Correction(c0, c1, c2, p0.z, p1.z, p2.z, testResult);
				const glm::vec3 pp = vec3Correction(a, b, c, p0.z, p1.z, p2.z, testResult);

				glm::vec3 point(interpolationP.x, interpolationP.y, pp.z);

				setColor(point, cc, depthFunc);
			}
		}
	}
}

bool Renderer::isValidTriangle(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c) const
{
	double d0 = glm::distance(a, b);
	double d1 = glm::distance(b, c);
	double d2 = glm::distance(c, a);
	if (d0 + d1 > d2 && glm::abs(d0 - d1) < d2)
	{
		return true;
	}
	return false;
}
