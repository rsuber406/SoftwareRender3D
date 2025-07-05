#include "Vector3.h"
#include <cmath>
Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(const Vector3& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

Vector3 Vector3::operator+(const Vector3& vector)
{
	return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

Vector3 Vector3::operator-(const Vector3& vector)
{
	return Vector3(x- vector.x, y-vector.y, z - vector.z);
}



float Vector3::DotProduct(const Vector3& firstVector, const Vector3& secondVector)
{
	float result = firstVector.x * secondVector.x + firstVector.y * secondVector.y + firstVector.z * secondVector.z;
	return result;
}

float Vector3::Distance(Vector3& firstVector, Vector3& secondVector)
{
	Vector3 difference = firstVector - secondVector;
	float magnitude = difference.Magnitude();
	return magnitude;
}

Vector3 Vector3::CrossProduct(Vector3& firstVector, Vector3& secondVector)
{
	float xComp = firstVector.y * secondVector.z - firstVector.z * secondVector.y;
	float yComp = firstVector.z * secondVector.x - secondVector.x * firstVector.z;
	float zComp = firstVector.x * secondVector.y - firstVector.y * secondVector.x;
	return Vector3(xComp, yComp, zComp);
}

float Vector3::AngleBetweenVectors(Vector3& firstVector, Vector3& secondVector)
{
	Vector3 normalizedFirst = firstVector.Normalize();
	Vector3 normalizedSecond = secondVector.Normalize();
	float angle = DotProduct(normalizedFirst, normalizedSecond);
	return acos(angle);
}

Vector3 Vector3::Lerp(Vector3& origin, Vector3& destination, float time)
{
	return origin + (destination - origin) * time;
}

bool Vector3::operator==(const Vector3& other)const
{
	float epsilion = 0.001f;
	if (fabs(x - other.x) < epsilion && fabs( y - other.y) < epsilion && fabs( z - other.z) < epsilion) return true;
	else return false;
}

Vector3 Vector3::Normalize() const
{
	float magnitude = Magnitude();
	if (magnitude < 0.00001f) return Vector3(0, 0, 0);
	return Vector3(x/magnitude, y /magnitude, z/magnitude);
}

float Vector3::Magnitude()const
{
	float magnitude = sqrt(x * x + y * y + z * z);
	return magnitude;
}

bool Vector3::IsZero()
{
	if (x == 0 && y == 0 && z == 0) return true;
	else return false;
}

Vector3 Vector3::Project(Vector3& onto)
{
	if (!onto.IsZero()) {
		float scalar = DotProduct(*this, onto) / DotProduct(onto, onto);
		return onto * scalar;
	}
	else
	return onto;
}

Vector3 Vector3::Reflection(Vector3& normal)
{
	return *this - normal * (2 * DotProduct(*this, normal));
}

float Vector3::GetComponent(int col)const
{
	switch (col) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	default: return 0;
	}
}

void Vector3::SetComponent(int index, float value)
{
	switch (index) {
	case 0: x = value; break;
	case 1: y = value; break;
	case 2: z = value; break;
	default: break;
	}
}
