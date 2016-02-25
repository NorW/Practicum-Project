#include "StdAfx.h"
#include "Camera.h"

Camera Camera::_camera = Camera();

Camera::Camera()
{

}

void Camera::setPosition(float xPos, float yPos)
{
	*_camera._view = glm::translate(*_camera._view, glm::vec3(xPos - _camera._position.x, yPos - _camera._position.y, 0.0f));
	_camera._position.x = xPos;
	_camera._position.y = yPos;
}

void Camera::shiftCamera(float xPos, float yPos)
{
	*_camera._view = glm::translate(*_camera._view, glm::vec3(xPos, yPos, 0.0f));
	_camera._position.x += xPos;
	_camera._position.y += yPos;
}