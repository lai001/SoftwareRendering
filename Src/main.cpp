#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <random>
#include <assert.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "assimp/Importer.hpp" 
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "spdlog/spdlog.h"

#include "PPM.hpp"
#include "Util.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "ModelShader.hpp"
#include "ModelShader2.hpp"
#include "RenderPipeLine.hpp"
#include "Texture2D.hpp"
#include "ImageShader.hpp"

struct GlobalResource
{
	const std::string appPath;
	const std::string appFolderPath = getFolder(appPath);
	const std::string modelPath = std::string(appFolderPath).append("\\Resource\\obj\\african_head\\african_head.obj");
	const std::string boxModelPath = std::string(appFolderPath).append("\\Resource\\box.dae");
	const std::string testImagePath = std::string(appFolderPath).append("\\Resource\\test0.jpg");
	const std::string boxWithTextureModelPath = std::string(appFolderPath).append("./Resource/box_with_texutre.dae");
	
	const GLFWwindow* window = nullptr;
	Renderer* renderer = nullptr;

	Assimp::Importer* boxImporter = nullptr;
	const aiScene* boxScene = nullptr;

	Assimp::Importer* modeImporter = nullptr;
	const aiScene* modelScene = nullptr;

	Texture2D* texture = nullptr;

	Assimp::Importer* boxWithTextureModelImporter = nullptr;
	const aiScene* boxWithTextureModelScene = nullptr;

	GlobalResource(int argc, char ** argv)
		:appPath(argv[0])
	{
		boxImporter = new Assimp::Importer();
		boxScene = boxImporter->ReadFile(boxModelPath, (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
		modeImporter = new Assimp::Importer();
		modelScene = modeImporter->ReadFile(modelPath, (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
		texture = new Texture2D(testImagePath);
		boxWithTextureModelImporter = new Assimp::Importer();
		boxWithTextureModelScene = modeImporter->ReadFile(boxWithTextureModelPath, (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices));
	}
};

GlobalResource* globalResource = nullptr;

void drawModel()
{
	const Renderer* renderer = globalResource->renderer;
	std::function<glm::vec2(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		aiVector3D vertex = mesh->mVertices[index];
		return glm::vec2(vertex.x, vertex.y);
	};

	const aiScene* scene = globalResource->modelScene;
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const glm::vec2 a = getVertex(mesh, face.mIndices[0]);
			const glm::vec2 b = getVertex(mesh, face.mIndices[1]);
			const glm::vec2 c = getVertex(mesh, face.mIndices[2]);
			const glm::vec3 color0 = randomColor();
			const glm::vec3 color1 = randomColor();
			const glm::vec3 color2 = randomColor();
			renderer->addTriangle2D(a, b, c, color0, color1, color2);
		}
	}
}

void drawModelPolygon()
{
	const Renderer* renderer = globalResource->renderer;

	std::function<glm::vec2(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		aiVector3D vertex = mesh->mVertices[index];
		return glm::vec2(vertex.x, vertex.y);
	};

	const aiScene* scene = globalResource->modelScene;
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const glm::vec2 a = getVertex(mesh, face.mIndices[0]);
			const glm::vec2 b = getVertex(mesh, face.mIndices[1]);
			const glm::vec2 c = getVertex(mesh, face.mIndices[2]);
			const glm::vec3 color0 = randomColor();
			renderer->addTriangle2D(a, b, c, color0, PolygonModeType::line);
		}
	}
}

void drawModel2()
{
	const Renderer* renderer = globalResource->renderer;

	std::function<glm::vec3(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		const aiVector3D vertex = mesh->mVertices[index];
		double z = rangeMap(vertex.z, -1, 1, 0, 1);
		z = 1 - z;
		return glm::vec3(vertex.x, vertex.y, z);
	};

	const aiScene* scene = globalResource->modelScene;
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const glm::vec3 a = getVertex(mesh, face.mIndices[0]);
			const glm::vec3 b = getVertex(mesh, face.mIndices[1]);
			const glm::vec3 c = getVertex(mesh, face.mIndices[2]);
			const glm::vec3 color0 = randomColor();
			const glm::vec3 color1 = randomColor();
			const glm::vec3 color2 = randomColor();
			renderer->addTriangle3D(a, b, c, color0, color1, color2, DepthFunc::less);
		}
	}
}

void drawModel3(const float time)
{
	const Renderer* renderer = globalResource->renderer;

	FCamera camera = FCamera(renderer->getWidth(), renderer->getHeight());

	camera.MoveUp(3.0);
	camera.MoveLeft(3.0);
	camera.MoveBack(1.0);

	glm::mat4x4 modelMat(1.0f);

	glm::mat4x4 scaleMat = glm::scale(glm::mat4x4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4x4 translateMat = glm::translate(glm::mat4x4(1.0), glm::vec3(0.0f, 0.0, 10.0));
	glm::mat4x4 rotateMat = glm::rotate(glm::mat4x4(1.0), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	modelMat = translateMat * rotateMat * scaleMat;

	glm::mat4x4 viewMat = camera.GetViewMat();
	glm::mat4x4 projectionMat = camera.GetprojectionMat();
	glm::mat4x4 mvpMat = projectionMat * viewMat * modelMat;

	std::function<glm::vec4(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		const aiVector3D vertex = mesh->mVertices[index];
		return glm::vec4(vertex.x, vertex.y, vertex.z, 1.0);
	};

	const aiScene* scene = globalResource->boxScene;

	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		aiMaterial* mMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuseColor;
		aiReturn ret = mMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const glm::vec4 a = getVertex(mesh, face.mIndices[0]);
			const glm::vec4 b = getVertex(mesh, face.mIndices[1]);
			const glm::vec4 c = getVertex(mesh, face.mIndices[2]);

			const glm::vec3 color0 = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
			const glm::vec3 color1 = color0;
			const glm::vec3 color2 = color0;

			const glm::vec4 p0 = mvpMat * a;
			const glm::vec4 p1 = mvpMat * b;
			const glm::vec4 p2 = mvpMat * c;

			renderer->addTriangle3D(p0, p1, p2, color0, color1, color2, DepthFunc::less);
		}
	}
}

void drawImage()
{
	Renderer* renderer = globalResource->renderer;
	ImageShader shader;
	shader.texture = globalResource->texture;
	RenderPipeline pipeline;
	pipeline.depthFunc = DepthFunc::lequal;
	pipeline.shader = &shader;
	std::vector<ImageShaderVertex> vertexBuffer;
	float length = 0.5;
	ImageShaderVertex a = ImageShaderVertex(glm::vec2(-length, length), glm::vec2(0.0f, 0.0f));
	ImageShaderVertex b = ImageShaderVertex(glm::vec2(length, length), glm::vec2(1.0f, 0.0f));
	ImageShaderVertex c = ImageShaderVertex(glm::vec2(-length, -length), glm::vec2(0.0f, 1.0f));

	ImageShaderVertex d = b;
	ImageShaderVertex e = ImageShaderVertex(glm::vec2(length, -length), glm::vec2(1.0f, 1.0f));
	ImageShaderVertex f = c;
	vertexBuffer.insert(vertexBuffer.end(), {a,b,c,d,e,f});

	pipeline.vertexBuffer = static_cast<void*>(vertexBuffer.data());
	pipeline.triangleCount = vertexBuffer.size() / 3;

	renderer->pipeline(pipeline);
}

void testPipeLine(const float time)
{
	Renderer* renderer = globalResource->renderer;

	FCamera camera = FCamera(renderer->getWidth(), renderer->getHeight());

	camera.MoveUp(3.0);
	camera.MoveLeft(2.0);
	camera.MoveBack(5.0);

	glm::mat4x4 modelMat(1.0f);

	glm::mat4x4 scaleMat = glm::scale(glm::mat4x4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4x4 translateMat = glm::translate(glm::mat4x4(1.0), glm::vec3(0.0f, 0.0, 5.0));
	glm::mat4x4 rotateMat = glm::rotate(glm::mat4x4(1.0), glm::radians(time * 5.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	modelMat = translateMat * rotateMat * scaleMat;

	glm::mat4x4 viewMat = camera.GetViewMat();
	glm::mat4x4 projectionMat = camera.GetprojectionMat();
	//glm::mat4x4 mvpMat = projectionMat * viewMat * modelMat;

	std::function<glm::vec3(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		const aiVector3D vertex = mesh->mVertices[index];
		return glm::vec3(vertex.x, vertex.y, vertex.z);
	};

	const aiScene* scene = globalResource->boxScene;
	ModelShader shader;
	shader.modelMat = modelMat;
	shader.viewMat = viewMat;
	shader.projectionMat = projectionMat;
	RenderPipeline pipeline;
	pipeline.shader = &shader;
	std::vector<BaseVertex> vertexBuffer;
	
	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		aiMaterial* mMaterial = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuseColor;
		aiReturn ret = mMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const glm::vec3 a = getVertex(mesh, face.mIndices[0]);
			const glm::vec3 b = getVertex(mesh, face.mIndices[1]);
			const glm::vec3 c = getVertex(mesh, face.mIndices[2]);
			
			const glm::vec3 color0 = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
			const glm::vec3 color1 = color0;
			const glm::vec3 color2 = color0;

			BaseVertex vertex0;
			vertex0.color = color0;
			vertex0.position = a;
			vertexBuffer.push_back(vertex0);

			BaseVertex vertex1;
			vertex1.color = color1;
			vertex1.position = b;
			vertexBuffer.push_back(vertex1);

			BaseVertex vertex2;
			vertex2.color = color2;
			vertex2.position = c;
			vertexBuffer.push_back(vertex2);
		}
	}
	
	pipeline.vertexBuffer = static_cast<void*>(vertexBuffer.data());
	pipeline.triangleCount = vertexBuffer.size() / 3;

	renderer->pipeline(pipeline);
}

void renderCube(const float time)
{
	Renderer* renderer = globalResource->renderer;

	FCamera camera = FCamera(renderer->getWidth(), renderer->getHeight());

	camera.MoveUp(0.0);
	camera.MoveLeft(0.0);
	camera.MoveBack(1.0);

	glm::mat4x4 modelMat(1.0f);

	glm::mat4x4 scaleMat = glm::scale(glm::mat4x4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4x4 translateMat = glm::translate(glm::mat4x4(1.0), glm::vec3(0.0f, 0.0, 5.0));
	glm::mat4x4 rotateMat = glm::rotate(glm::mat4x4(1.0), glm::radians(time * 5.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	modelMat = translateMat * rotateMat * scaleMat;

	glm::mat4x4 viewMat = camera.GetViewMat();
	glm::mat4x4 projectionMat = camera.GetprojectionMat();
	//glm::mat4x4 mvpMat = projectionMat * viewMat * modelMat;

	std::function<BaseVertex2(aiMesh*, unsigned int)> getVertex = [renderer](aiMesh* mesh, unsigned int index) {
		BaseVertex2 baseVertex;
		const aiVector3D vertex = mesh->mVertices[index];
		const aiVector3D coords = mesh->mTextureCoords[0][index];
		baseVertex.position = glm::vec3(vertex.x, vertex.y, vertex.z);
		baseVertex.textureCoords = glm::vec2(coords.x, coords.y);
		return baseVertex;
	};

	const aiScene* scene = globalResource->boxWithTextureModelScene;
	ModelShader2 shader;
	shader.modelMat = modelMat;
	shader.viewMat = viewMat;
	shader.projectionMat = projectionMat;
	shader.texture = globalResource->texture;
	RenderPipeline pipeline;
	pipeline.shader = &shader;
	std::vector<BaseVertex2> vertexBuffer;

	for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];

		for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			const BaseVertex2 vertex0 = getVertex(mesh, face.mIndices[0]);
			const BaseVertex2 vertex1 = getVertex(mesh, face.mIndices[1]);
			const BaseVertex2 vertex2 = getVertex(mesh, face.mIndices[2]);
			vertexBuffer.insert(vertexBuffer.end(), { vertex0, vertex1, vertex2 });
		}
	}

	pipeline.vertexBuffer = static_cast<void*>(vertexBuffer.data());
	pipeline.triangleCount = vertexBuffer.size() / 3;

	renderer->pipeline(pipeline);
}

void glRenderLoop()
{
	drawModel3(glfwGetTime());
	testPipeLine(glfwGetTime());
	drawImage();
	renderCube(glfwGetTime());
}

void initGL()
{
	const Renderer* renderer = globalResource->renderer;

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(renderer->getWidth(), renderer->getHeight(), "SoftwareRendering", NULL, NULL);
	assert(window);
	globalResource->window = window;
	glfwMakeContextCurrent(window);
	//glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	//glfwSetScrollCallback(window, scrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(false);
	}

	while (!glfwWindowShouldClose(window))
	{
		renderer->flush();
		glRenderLoop();
		const void* data = renderer->getFrameBuffer()->getData();
		int width = renderer->getWidth();
		int height = renderer->getHeight();
		int length = width * height;
		void* copyImageData = new unsigned char[length * 3];
		memcpy(copyImageData, renderer->getFrameBuffer()->getData(), length * 3);
		stbi__vertical_flip(copyImageData, width, height, 3);
		glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, copyImageData);
		delete copyImageData;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void write()
{
	Renderer& renderer = *globalResource->renderer;
	{
		renderer.setColor(glm::vec3(-1.0, 1.0, 0), glm::vec3(1, 0, 0), DepthFunc::always);
		renderer.setColor(glm::vec3(1.0, 1.0, 0), glm::vec3(1, 0, 0), DepthFunc::always);
		renderer.setColor(glm::vec3(-1.0, -1.0, 0), glm::vec3(1, 0, 0), DepthFunc::always);
		renderer.setColor(glm::vec3(1.0, -1.0, 0), glm::vec3(1, 0, 0), DepthFunc::always);

		renderer.addLine2D(glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0), Color::white);

		renderer.addTriangle2D(glm::vec2(-0.9, 0.9), glm::vec2(0.0, 0.87), glm::vec2(-0.9, -0.9), Color::red, PolygonModeType::line);
		renderer.addTriangle2D(glm::vec2(-1.0, 0.5), glm::vec2(-1.0, 0.6), glm::vec2(-0.9, -0.8), Color::yellow, PolygonModeType::fill);
		renderer.addTriangle2D(glm::vec2(0.0, 0.0), glm::vec2(1.0, 0.0), glm::vec2(0.0, 1.0),
			Color::gree, Color::red, Color::blue);
		PPM::writePxielsToFile(*renderer.getFrameBuffer(), "Image.ppm");
	}

	{
		renderer.flush();
		drawModel();
		PPM::writePxielsToFile(*renderer.getFrameBuffer(), "Model.ppm");
	}

	{
		renderer.flush();
		drawModelPolygon();
		PPM::writePxielsToFile(*renderer.getFrameBuffer(), "ModelPolygon.ppm");
	}

	{
		renderer.flush();
		drawModel2();
		PPM::writePxielsToFile(*renderer.getFrameBuffer(), "Model2.ppm");
		PPM::writeZBufferToFile(*renderer.getFrameBuffer(), "zBuffer.ppm");
	}

	{
		renderer.flush();
		const glm::vec3 a = glm::vec3(0.0, 0.0, 0.0);
		const glm::vec3 b = glm::vec3(1.0, 0.0, 0.0);
		const glm::vec3 c = glm::vec3(0.0, 1.0, 0.0);
		const glm::vec3 color0 = Color::red;
		const glm::vec3 color1 = Color::gree;
		const glm::vec3 color2 = Color::blue;
		renderer.addTriangle3D(a, b, c, color0, color1, color2, DepthFunc::less);
		PPM::writePxielsToFile(*renderer.getFrameBuffer(), "Triangle.ppm");
	}
}

int main(int argc, char ** argv)
{
	spdlog::set_level(spdlog::level::trace);

	globalResource = new GlobalResource(argc, argv);
	globalResource->renderer = new Renderer(800, 800);

	//write();

	initGL();

	return 0;
}
