#include "PPM.hpp"
#include <algorithm>
#include <vector>
#include <functional>

PPM::PPM(int width, int height)
	:width(width), height(height)
{
	int l = width * height * sizeof(unsigned char);
	r = new unsigned char[l];
	g = new unsigned char[l];
	b = new unsigned char[l];

	memset(r, 0, l);
	memset(g, 0, l);
	memset(b, 0, l);
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

void PPM::setColor(Point point, Color p)
{
	int idx = (int)point.y * width + (int)point.x;
	if (idx < width * height && idx >= 0)
	{
		r[idx] = p.r;
		g[idx] = p.g;
		b[idx] = p.b;
	}
}

void PPM::addLine(Point p0, Point p1, Color color)
{
	float a = (p1.y - p0.y) / (p1.x - p0.x);
	float b = p0.y - a * p0.x;
	int s = (int)std::min(p0.x, p1.x);
	int e = (int)std::max(p0.x, p1.x);
	for (int x = s; x <= e; x++) {
		float y = a * x + b;
		setColor({ (float)x, y }, color);
	}
}

void PPM::addTriangle(Triangle triangle, Color color)
{
	addLine(triangle.a, triangle.b, color);
	addLine(triangle.b, triangle.c, color);
	addLine(triangle.c, triangle.a, color);
}

void PPM::addTriangleFillWithColor(Triangle triangle, Color color)
{
	std::vector<Point> points = { triangle.a, triangle.b, triangle.c };

	std::sort(points.begin(), points.end(), [](Point lhs, Point rhs) {
		return lhs.y < rhs.y;
	});

	std::function<void(Line2D, Line2D, int, int)> scanline = [this, color](Line2D l1, Line2D l2, int y0, int y1) {
		for (int y = y0; y < y1; y++)
		{
			float x0 = l1.x(y);
			float x1 = l2.x(y);
			for (int x = std::min(x0, x1); x < std::max(x0, x1); x++)
			{
				setColor({ x,y }, color);
			}
		}
	};

	scanline(Line2D(points[0], points[1]), Line2D(points[0], points[2]), points[0].y, points[1].y);
	scanline(Line2D(points[1], points[2]), Line2D(points[0], points[2]), points[1].y, points[2].y);
}
