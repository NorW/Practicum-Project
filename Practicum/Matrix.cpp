#include "StdAfx.h"
#include "Matrix.h"

std::shared_ptr<Matrix4x4> Matrix4x4::makeOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
{
	std::shared_ptr<Matrix4x4> val = std::make_shared<Matrix4x4>();

	val->set(0, 0, 2.0f / (right - left));
	val->set(0, 3, -(right + left) / (right - left));
	val->set(1, 1, 2.0f / (top - bottom));
	val->set(1, 3, -(top + bottom) / (top - bottom));
	val->set(2, 2, -2.0f / (farVal - nearVal));
	val->set(2, 3, -(farVal + nearVal) / (farVal - nearVal));
	val->set(3, 3, 1.0f);
	return val;
}

Matrix4x4::Matrix4x4()
{
	_values.fill(std::array<float, 4>());
	for (auto i : _values)
	{
		i.fill(0);
	}
}

Matrix4x4::Matrix4x4(float** values)
{
	_values.fill(std::array<float, 4>());
	for (unsigned int x = 0; x < _values.size(); x++)
	{
		for (unsigned int y = 0; y < _values[x].size(); y++)
		{
			_values[x][y] = values[x][y];
		}
	}
}

float* Matrix4x4::getAll()
{
	float val[16];

	for (unsigned int x = 0; x < _values.size(); x++)
	{
		for (unsigned int y = 0; y < _values[x].size(); y++)
		{
			val[x * 4 + y] = _values[x][y];
		}
	}

	return val;
}