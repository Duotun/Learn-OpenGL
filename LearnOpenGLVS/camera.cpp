#include "camera.hpp"

void Camera::UpdateCamWindow(int inWidth, int inHeight, float nearPlane, float farPlane)
{
	width = inWidth;
	height = inHeight;
	near = nearPlane;
	far = farPlane;
}


glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position+FrontDir, UpDir);
	//return Calculate_Lookat_Matrix(Position, Position+FrontDir, UpDir);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return glm::perspective(glm::radians(Fov), (float)width/height, near, far);
}

glm::mat4 Camera::GetViewProjMatrix()
{
	return GetViewMatrix()* GetProjectionMatrix();
}


//update matrix values to the shader
void Camera::UpdateViewMatrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(GetViewMatrix()));
}
void Camera::UpdateProjectionMatrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(GetProjectionMatrix()));
}
void Camera::UpdateViewProjMatrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(GetViewProjMatrix()));
}


//Process Input
void Camera::ProcessKeyboard(CAMERA_MOVEMENT direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == CAMERA_MOVEMENT::Forward)
		Position += FrontDir * velocity;
	if (direction == CAMERA_MOVEMENT::Backward)
		Position -= FrontDir * velocity;
	if (direction == CAMERA_MOVEMENT::Right)
		Position += RightDir * velocity;
	if (direction == CAMERA_MOVEMENT::Left)
		Position -= RightDir * velocity;

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw +=xoffset;
	Pitch +=yoffset;
	//make sure when pitch is not out of bounds
	if (constrainPitch)
	{
		if(Pitch > 89.0f)
		 Pitch = 89.0f;
		if(Pitch <-89.0f)
		 Pitch = -89.0f;
	}

	//update the front, right and up vectors
	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Fov -=(float)yoffset;
	if (Fov < 1.0f)  Fov = 1.0f;
	if (Fov > 45.0f) Fov = 45.0f;
}

