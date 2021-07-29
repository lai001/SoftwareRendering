#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>  
#include <assert.h>

struct Color
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
};

struct Point
{
	float x = 0;
	float y = 0;
	Point(int x, int y)
		:x(x), y(y)
	{

	}
	Point(float x, float y)
		:x(x), y(y)
	{

	}
};

struct Triangle
{
	Point a{ 0, 0 };
	Point b{ 0, 0 };
	Point c{ 0, 0 };
};

struct Line2D
{
	Point p0;
	Point p1;

	float a = 0;
	float b = 0;

	Line2D(Point p0, Point p1)
		:p0(p0), p1(p1)
	{
		a = (p1.y - p0.y) / (p1.x - p0.x);
		b = p0.y - a * p0.x;
	}

	float x(float y) {
		assert(a != 0.0);
		float x = (y - b) / a;
		return x;
	}

	float y(float x) {
		float y = a * x + b;
		return y;
	}

};

class PPM
{

public:
	PPM(int width, int height);
	~PPM();

	void writePPMFileHeader(std::ofstream* f);
	void writeToFile(std::string filename);
	void setColor(Point point, Color p);
	void addLine(Point p0, Point p1, Color color);
	void addTriangle(Triangle triangle, Color color);
	void addTriangleFillWithColor(Triangle triangle, Color color);

public:
	int width = 0;
	int height = 0;

	unsigned char* r = nullptr;
	unsigned char* g = nullptr;
	unsigned char* b = nullptr;
};
