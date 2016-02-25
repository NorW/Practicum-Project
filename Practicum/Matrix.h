#pragma once
#include "StdAfx.h"
#include <array>

class Matrix4x4
{
private:
	std::array<std::array<float, 4>, 4> _values;

public:
	static std::shared_ptr<Matrix4x4> makeOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal);

	Matrix4x4();
	explicit Matrix4x4(float** values);

	inline float get(unsigned const int x, unsigned const int y)
	{
		if (x > 3 || y > 3)
		{
			return 0;
		}
		return _values[x][y];
	}

	inline void set(unsigned const int x, unsigned const int y, float value)
	{
		if (x < 4 && y < 4)
		{
			_values[x][y] = value;
		}
	}

	float* getAll();
};
