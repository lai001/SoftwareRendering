#include "Camera.hpp"

#include "glm/ext.hpp"
#include "glm/gtx/rotate_vector.hpp"

FCamera::FCamera(const int WindowWidth, const int WindowHeight)
	:ScreenWidth(WindowWidth), ScreenHeight(WindowHeight)
{
	View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
	Projection = glm::perspective(glm::radians(Fov), static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight), Near, Far);
}

FCamera::~FCamera()
{
}

void FCamera::IncreaseCameraSpeed(float Dx)
{
	float X = CameraSpeed + Dx;
	X = glm::max(X, MinCameraSpeed);
	X = glm::min(X, MaxCameraSpeed);
	CameraSpeed = X;
}

void FCamera::SetCameraSpeed(float Speed)
{
	float X = Speed;
	X = glm::max(X, MinCameraSpeed);
	X = glm::min(X, MaxCameraSpeed);
	CameraSpeed = X;
}

glm::vec3 FCamera::GetPosition()
{
	return CameraPos;
}

float FCamera::GetCameraSpeed()
{
	return CameraSpeed;
}

float FCamera::GetFov()
{
	return Fov;
}

void FCamera::SetFov(float Fov)
{
	Fov = Fov * ScrollSensitivity;
	if (Fov >= MinFov && Fov <= MaxFov)

		if (Fov <= MinFov)
			Fov = MinFov;
	if (Fov >= MaxFov)
		Fov = MaxFov;

	this->Fov = Fov;
	Projection = glm::perspective(glm::radians(Fov), (float)ScreenWidth / (float)ScreenHeight, Near, Far);
}

void FCamera::SetNear(float Near)
{
	this->Near = Near;
	Projection = glm::perspective(glm::radians(Fov), static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight), Near, Far);
}

void FCamera::SetFar(float Far)
{
	this->Far = Far;
	Projection = glm::perspective(glm::radians(Fov), static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight), Near, Far);
}

glm::vec3 FCamera::GetCameraFront()
{
	return CameraFront;
}

void FCamera::MoveForward(float Units)
{
	CameraPos += CameraFront * CameraSpeed * Units;
	View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void FCamera::MoveBack(float Units)
{
	MoveForward(-Units);
}

void FCamera::MoveLeft(float Units)
{
	CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed * Units;
	View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void FCamera::MoveRight(float Units)
{
	MoveLeft(-Units);
}

void FCamera::MoveUp(float Units)
{
	CameraPos += glm::normalize(CameraUp) * CameraSpeed * Units;
	View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void FCamera::MoveDown(float Units)
{
	MoveUp(-Units);
}

glm::mat4 FCamera::GetViewMat()
{
	return View;
}

glm::mat4 FCamera::GetprojectionMat()
{
	return Projection;
}

void FCamera::MouseCallback(double Xpos, double Ypos)
{
	//if (bFirstMouse)
	//{
		//LastX = Xpos;
		//LastY = Ypos;
	//	bFirstMouse = false;
	//}

	float xoffset = (float)Xpos - LastX;
	float yoffset = LastY - (float)Ypos;
	LastX = (float)Xpos;
	LastY = (float)Ypos;

	if (!bIsMovable)
	{
		return;
	}

	xoffset *= Sensitivity;
	yoffset *= Sensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.9f)
		Pitch = 89.9f;
	if (Pitch < -89.9f)
		Pitch = -89.9f;


	glm::vec3 Front(0.0f, 0.0f, 1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	Front = glm::rotate(Front, glm::radians(Yaw), glm::vec3(0.0, 1.0, 0.0));
	Front = glm::rotate(Front, glm::radians(Pitch), glm::vec3(1.0, 0.0, 0.0));
	Up = glm::rotate(Up, glm::radians(Pitch), glm::vec3(1.0, 0.0, 0.0));
	Up = glm::rotate(Up, glm::radians(Yaw), glm::vec3(0.0, 0.0, 1.0));

	CameraFront = glm::normalize(Front);
	CameraUp = glm::normalize(Up);

	View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
}

void FCamera::ScrollCallback(double Xoffset, double Yoffset)
{
	float Fov = GetFov();
	Fov -= (float)Yoffset;
	SetFov(Fov);
}