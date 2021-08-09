#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <random>

#include "assimp/Importer.hpp" 
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "spdlog/spdlog.h"

#include "PPM.hpp"

glm::vec3 randomColor()
{
	std::function<double()> randomDoubleValue = []() {
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, 255);
		return dist(rd) / 255.0;
	};

	glm::vec3 color;
	color.r = randomDoubleValue();
	color.g = randomDoubleValue();
	color.b = randomDoubleValue();
	return color;
}

std::string getFolder(std::string filename)
{
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx);
	}
	return directory;
}

void drawModelPolygon(PPM* ppm, std::string modelPath)
{
	std::function<glm::vec2(aiMesh*, unsigned int)> getPoint = [ppm](aiMesh* mesh, unsigned int index) {
		aiVector3D vertex = mesh->mVertices[index];
		return glm::vec2(vertex.x*ppm->width / 2.0, vertex.y*ppm->height / 2.0);
	};

	float width = ppm->width;
	float height = ppm->height;

	Assimp::Importer* importer = new Assimp::Importer();
	const aiScene* scene = importer->ReadFile(modelPath, (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			for (int vertexIndex = 0; vertexIndex < face.mNumIndices; vertexIndex++)
			{
				unsigned int index = face.mIndices[vertexIndex];
				unsigned int nextIndex = face.mIndices[(vertexIndex + 1) % face.mNumIndices];
				glm::vec2 p0 = getPoint(mesh, index);
				glm::vec2 p1 = getPoint(mesh, nextIndex);;
				ppm->addLine(p0, p1, {1, 0, 0});
			}
		}
	}
	delete importer;
}

void drawModel(PPM* ppm, std::string modelPath)
{
	std::function<Vertex(aiMesh*, unsigned int)> getVertex = [ppm](aiMesh* mesh, unsigned int index) {
		aiVector3D vertex = mesh->mVertices[index];
		double z = rangeMap(vertex.z, -1, 1, 0, 1);
		z = 1 - z;
		Vertex v = Vertex({ vertex.x*ppm->width / 2.0, vertex.y*ppm->height / 2.0, z }, { 1, 0, 0 });
		return v;
	};

	Assimp::Importer* importer = new Assimp::Importer();
	const aiScene* scene = importer->ReadFile(modelPath, (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			Triangle triangle;
			triangle.a = getVertex(mesh, face.mIndices[0]);
			triangle.b = getVertex(mesh, face.mIndices[1]);
			triangle.c = getVertex(mesh, face.mIndices[2]);
			ppm->addTriangleFillWithColor2(triangle, randomColor());
		}
	}
	delete importer;
}

int main(int argc, char ** argv)
{
	spdlog::set_level(spdlog::level::trace);

	const std::string appPath = argv[0];
	const std::string appFolderPath = getFolder(appPath);
	const std::string modelPath = std::string(appFolderPath).append("\\Resource\\obj\\african_head\\african_head.obj");

	int width = 800;
	int height = 800;

	PPM ppm = PPM(width, height);

	ppm.addLine({ -400, -400 }, { 0, 0 }, { 1, 1, 1 });
	ppm.addTriangle( {0, 0}, {400, 0}, {400, 400} , { 1, 0, 0 });
	ppm.addTriangleFillWithColor2(Triangle({ {-100,-100,1}, {1, 0, 0} }, { {0,-100,1}, {0, 1, 0} }, { {400,200,0}, {0, 0, 1} }));
	ppm.addTriangleFillWithColor2(Triangle({ {-400,0,0.0}, {1, 0, 0} }, { {400,0,0.5}, {0, 1, 0} }, { {0,400,1}, {0, 0, 1} }));
	drawModelPolygon(&ppm, modelPath);
	drawModel(&ppm, modelPath);

	ppm.writeToFile("image.ppm");
	ppm.writeZBufferToFile("zbuffer.ppm");
	return 0;
}
