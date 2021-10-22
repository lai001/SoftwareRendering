#pragma once
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"

class FCamera
{
public:
	FCamera(const int WindowWidth, const int WindowHeight);
	~FCamera();

private:
	int ScreenWidth = 800;
	int ScreenHeight = 600;
	float Sensitivity = 1.0f;
	float ScrollSensitivity = 1.0f;
	float MaxFov = 90.0f;
	float MinFov = 1.0f;
	float MaxCameraSpeed = 10.0f;
	float MinCameraSpeed = 0.01f;
	float CameraSpeed = 1.0f;
	float Near = 0.1f;
	float Far = 1000.0f;
	bool bFirstMouse = true;

	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::mat4(1.0f);

	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float LastX = 0.0f;
	float LastY = 0.0f;
	float Fov = 90.0f;

	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:
	bool bIsMovable = true;

public:
	void IncreaseCameraSpeed(float Dx);
	void SetCameraSpeed(float Speed);
	float GetCameraSpeed();
	float GetFov();
	void SetFov(float Fov);
	void SetNear(float Near);
	void SetFar(float Far);
	glm::vec3 GetCameraFront();

	void MoveForward(float Units = 1.0f);
	void MoveBack(float Units = 1.0f);
	void MoveLeft(float Units = 1.0f);
	void MoveRight(float Units = 1.0f);
	void MoveUp(float Units = 1.0f);
	void MoveDown(float Units = 1.0f);

	glm::mat4 GetViewMat();
	glm::mat4 GetprojectionMat();
	glm::vec3 GetPosition();

	virtual void ScrollCallback(double Xoffset, double Yoffset);
	virtual void MouseCallback(double Xpos, double Ypos);
};
