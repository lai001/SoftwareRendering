#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>  
#include <assert.h>

#include "glm/glm.hpp"

static glm::vec3 denormalColor(glm::vec3 color)
{
	return glm::vec3(color.r * 255.0, color.g * 255.0, color.b * 255.0);
}

static double rangeMap(double v, double inputLower, double inputUpper, double outputLower, double outputUpper)
{
	return outputLower + ((outputUpper - outputLower) / (inputUpper - inputLower)) * (v - inputLower);
}

struct BarycentricTestResult
{
	bool isInsideTriangle;
	double v;
	double u;
	double w;
};

struct Vertex
{
	glm::vec3 point = { 0,0,0 };
	glm::vec3 color = { 0,0,0 };

	Vertex()
	{

	}

	Vertex(glm::vec3 point, glm::vec3 color)
		:point(point), color(color)
	{

	}
};

struct Triangle
{
	Vertex a;
	Vertex b;
	Vertex c;
	Triangle()
	{

	}
	Triangle(Vertex a, Vertex b, Vertex c)
		:a(a), b(b), c(c)
	{

	}
};

struct Line2D
{
	glm::vec2 p0;
	glm::vec2 p1;

	double a = 0;
	double b = 0;

	Line2D(glm::vec2 p0, glm::vec2 p1)
		:p0(p0), p1(p1)
	{
		a = (p1.y - p0.y) / (p1.x - p0.x);
		b = p0.y - a * p0.x;
	}

	double x(double y) {
		assert(a != 0.0);
		double x = (y - b) / a;
		return x;
	}

	double y(double x) {
		double y = a * x + b;
		return y;
	}

};

struct Rect
{
	double x;
	double y;
	double width;
	double height;
};

class PPM
{

public:
	PPM(int width, int height);
	~PPM();

	void reset();
	void writePPMFileHeader(std::ofstream* f);
	void writeToFile(std::string filename);
	void writeZBufferToFile(std::string filename);
	int pointToIndex(glm::vec3 point, bool isEnableDepthTest);
	void setColor(glm::vec3 point, glm::vec3 color, bool isEnableDepthTest);
	void addLine(glm::vec2 p0, glm::vec2 p1, glm::vec3 color);
	void addTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec3 color);
	void addTriangleFillWithColor(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec3 color);
	Rect boundingBox(glm::vec2 a, glm::vec2 b, glm::vec2 c);
	BarycentricTestResult barycentricTest(glm::vec2 a, glm::vec2 b, glm::vec2 c, double x, double y);
	Vertex interpolation(BarycentricTestResult testResult, Triangle triangle);
	void addTriangleFillWithColor2(Triangle triangle);
	void addTriangleFillWithColor2(Triangle triangle, glm::vec3 color);

public:
	int width = 0;
	int height = 0;

	unsigned char* r = nullptr;
	unsigned char* g = nullptr;
	unsigned char* b = nullptr;

	double* zBuffer = nullptr;
};
