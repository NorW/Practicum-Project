#include "StdAfx.h"
#include "Vector.h"

int Vector2::manhattanDistance(Vector2& from, Vector2& to)
{
	return (int)(abs(from.x - to.x) + abs(from.y - to.y));
}

Vector2::Vector2(): x(0), y(0)
{

}

Vector2::Vector2(float xVal, float yVal): x(xVal), y(yVal)
{

}

Vector2& Vector2::operator+(Vector2& other)
{
	std::shared_ptr<Vector2> temp = std::make_shared<Vector2, float, float>(x + other.x, y + other.y);
	return *temp;
}

Vector2& Vector2::operator-(Vector2& other)
{
	return Vector2(x - other.x, y - other.y);
}

Vector2& Vector2::operator*(float scalar)
{
	return Vector2(x * scalar, y * scalar);
}

Vector2& Vector2::operator/(float scalar)
{
	Vector2 result;

	if(scalar)
	{
		result.x = x / scalar;
		result.y = y / scalar;
	}
	
	return result;
}

void Vector2::operator+=(Vector2& other)
{
	x += other.x;
	y += other.y;
}

void Vector2::operator-=(Vector2& other)
{
	x -= other.x;
	y -= other.y;
}

void Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
}

void Vector2::operator/=(float scalar)	  
{
	if(scalar)
	{
		x /= scalar;
		y /= scalar;
	}
}