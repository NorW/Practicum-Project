#pragma once
#include "StdAfx.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
private:
	
	static Camera _camera;

	std::shared_ptr<glm::mat4> _view, _projection;

	Vector2 _position;

	Camera();

public:
	
	inline static auto getProjection()
	{
		return _camera._projection;
	}

	inline static auto getView()
	{
		return _camera._view;
	}

	inline static void setProjection(std::shared_ptr<glm::mat4> proj)
	{
		_camera._projection = proj;
	}

	inline static void setView(std::shared_ptr<glm::mat4> view)
	{
		_camera._view = view;
	}

	static void setPosition(float xPos, float yPos);

	static void shiftCamera(float xOffset, float yOffset);
};