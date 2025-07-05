#include "Vector2.h"
#include <cmath>
Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;

}

Vector2::Vector2()
{
	x = 0;
	y = 0;

}

Vector2::Vector2(const Vector2& copy)
{
	x = copy.x;
	y = copy.y;
	
}

Vector2 Vector2::operator+(const Vector2& vector)
{
	return Vector2(x + vector.x, y + vector.y);
}

Vector2 Vector2::operator-(const Vector2& vector)
{
	return Vector2(x - vector.x, y - vector.y);
}



float Vector2::DotProduct(Vector2& firstVector, Vector2& secondVector)
{
	float result = firstVector.x * secondVector.x + firstVector.y * secondVector.y;
	return result;
}

float Vector2::Distance(Vector2& firstVector, Vector2& secondVector)
{
	Vector2 difference = firstVector - secondVector;
	float magnitude = difference.Magnitude();
	return magnitude;
}



float Vector2::AngleBetweenVectors(Vector2& firstVector, Vector2& secondVector)
{
	Vector2 normalizedFirst = firstVector.Normalize();
	Vector2 normalizedSecond = secondVector.Normalize();
	float angle = DotProduct(normalizedFirst, normalizedSecond);
	return acos(angle);
}

Vector2 Vector2::Lerp(Vector2& origin, Vector2& destination, float time)
{
	return origin + (destination - origin) * time;
}

bool Vector2::operator==(const Vector2& other)
{
	float epsilion = 0.001f;
	if (fabs(x - other.x) < epsilion && fabs(y - other.y) < epsilion) return true;
	else return false;
}

Vector2 Vector2::Normalize() const
{
	float magnitude = Magnitude();
	if (magnitude < 0.00001f) return Vector2(0, 0);
	return Vector2(x / magnitude, y / magnitude);
}

float Vector2::Magnitude()const
{
	float magnitude = sqrt(x * x + y * y);
	return magnitude;
}

bool Vector2::IsZero()
{
	if (x == 0 && y == 0) return true;
	else return false;
}

Vector2 Vector2::Project(Vector2& onto)
{
	if (!onto.IsZero()) {
		float scalar = DotProduct(*this, onto) / DotProduct(onto, onto);
		return onto * scalar;
	}
	else
		return onto;
}

Vector2 Vector2::Reflection(Vector2& normal)
{
	return *this - normal * (2 * DotProduct(*this, normal));
}

