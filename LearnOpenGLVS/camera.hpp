#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "shader.hpp"

enum CAMERA_MOVEMENT {
	Forward,
	Backward,
	Left,
	Right
};

//Default camera values
const float YAW_INIT = -90.0f;
const float PITCH_INIT = 0.0f;
const float SPEED_INIT = 2.5F;
const float SENSITIVITY_INIT = 0.1F;
const float FOV_INIT = 45.0f;  



class Camera {

public:
	//camera attributes
	glm::vec3 Position;
	glm::vec3 FrontDir;
	glm::vec3 UpDir;
	glm::vec3 RightDir;
	glm::vec3 WorldUp;  //for trick to compute the view matrix
	
	//euler angles for rotating
	float Yaw;
	float Pitch;

	//constructors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float inYaw=YAW_INIT,
	float inPitch = PITCH_INIT)
	:FrontDir(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED_INIT), MouseSensitivity(SENSITIVITY_INIT), Fov(FOV_INIT)
	{
		Position = position;
		WorldUp = up;
		Yaw = inYaw;
		Pitch = inPitch;
		UpdateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float inYaw, float inPitch)
	:FrontDir(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED_INIT), MouseSensitivity(SENSITIVITY_INIT), Fov(FOV_INIT)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = inYaw;
		Pitch = inPitch;
		UpdateCameraVectors();

	}

	//movement
	float MovementSpeed;
	float MouseSensitivity;
	float Fov;

	//Window - Aspect Ratio
	int width;
	int height;

	//for Projection
	float near;
	float far;

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewProjMatrix();
	void UpdateCamWindow(int inWidth, int inHeight, float nearPlane, float farPlane);

	//Process Input
	void ProcessKeyboard(CAMERA_MOVEMENT direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch=true);
	void ProcessMouseScroll(float yoffset);

	//update matrix
	void UpdateViewMatrix(Shader& shader, const char *uniform);
	void UpdateProjectionMatrix(Shader& shader, const char* uniform);
	void UpdateViewProjMatrix(Shader& shader, const char* uniform);


	//update the camera vector to build the view matrix
	void UpdateCameraVectors()
	{
		//calculate the new Front Vector
		glm::vec3 front;   //toward the scene -> negative z
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		FrontDir = glm::normalize(front);
		//Right is from the trick of World-Up
		RightDir = glm::normalize(glm::cross(FrontDir, WorldUp));
		//Up is from the cross-product of Front and Up
		UpDir = glm::normalize(glm::cross(RightDir, FrontDir));
	}

private:


	//manual lookup function
	glm::mat4 Calculate_Lookat_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
	{
		//calculate the three vectors - perpendicular
		glm::vec3 zaxis = glm::normalize(position - target);
		glm::vec3 xaxis = glm::normalize(glm::cross(worldUp, zaxis));
		glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));  //right-hand rule

		//create translation and rotation for the 4x4 matrix
		glm::mat4 translation = glm::mat4(1.0f);
		translation[3][0] = -position.x;   //camera reverse movement
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;
		glm::mat4 rotation = glm::mat4(1.0f);

		rotation[0][0] = xaxis.x;
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;

		rotation[0][1] = yaxis.x;
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;

		rotation[0][2] = zaxis.x;
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		return translation * rotation;   //return the manual calculated view matrix

	}

};


#endif // !CAMERA_H
