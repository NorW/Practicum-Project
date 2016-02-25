#pragma once
#include "StdAfx.h"
#include <stdlib.h>
#include <math.h>
#include <memory>

class Vector2
{
public:
	float x, y;
	
	static int manhattanDistance(Vector2& from, Vector2& to);

	Vector2();
	Vector2(float xVal, float yVal);

	Vector2& operator+(Vector2& other);
	Vector2& operator-(Vector2& other);
	Vector2& operator*(float scalar);
	Vector2& operator/(float scalar);

	void operator+=(Vector2& other);
	void operator-=(Vector2& other);
	void operator*=(float scalar);
	void operator/=(float scalar);
};