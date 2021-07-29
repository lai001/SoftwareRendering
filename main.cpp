#include <iostream>
#include <vector>
#include <regex>
#include <numeric>

#include "assimp/Importer.hpp" 
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "spdlog/spdlog.h"

#include "PPM.hpp"

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

int main(int argc, char ** argv)
{
	std::string appPath = argv[0];
	std::string appFolderPath = getFolder(appPath);

	int width = 800;
	int height = 800;

	PPM* ppm = new PPM(width, height);
	ppm->addLine({ 80, 40 }, { 13, 20 }, { 254, 254, 254 });
	ppm->addTriangle({ {0, 100},{200, 100},{100,50} }, { 255,0,0 });

	ppm->addTriangleFillWithColor({ {0, 120},{200, 120},{100,150} }, { 255,0,0 });

	Assimp::Importer* importer = new Assimp::Importer();
	const aiScene* scene = importer->ReadFile(appFolderPath.append("\\Resource\\obj\\african_head\\african_head.obj"), (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
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
				aiVector3D v0 = mesh->mVertices[index];
				aiVector3D v1 = mesh->mVertices[nextIndex];
				int x0 = (v0.x + 1.0)*width / 2.0;
				int y0 = (v0.y + 1.0)*height / 2.0;
				int x1 = (v1.x + 1.0)*width / 2.0;
				int y1 = (v1.y + 1.0)*height / 2.0;
				y0 = height - y0;
				y1 = height - y1;
				Color color;
				color.r = 255;
				ppm->addLine({ x0,y0 }, { x1,y1 }, color);
			}
		}
	}

	ppm->writeToFile("image.ppm");
	delete ppm;
	delete importer;
	system("pause");
	return 0;
}
