#include "Vector4.h"
#include <cmath>
Vector4::Vector4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vector4::Vector4(const Vector4& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
}

Vector4::Vector4(Vector3& other)
{
	x = other.GetX();
	y = other.GetY();
	z = other.GetZ();
	w = 1.0f;
}

Vector4 Vector4::operator+(const Vector4& vector)
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
}

Vector4 Vector4::operator-(const Vector4& vector)
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
}



float Vector4::DotProduct(const Vector4& firstVector, const Vector4& secondVector)
{
	float result = firstVector.x * secondVector.x + firstVector.y * secondVector.y + firstVector.z * secondVector.z + firstVector.w * secondVector.w;
	return result;
}

float Vector4::Distance(Vector4& firstVector, Vector4& secondVector)
{
	Vector4 difference = firstVector - secondVector;
	float magnitude = difference.Magnitude();
	return magnitude;
}



float Vector4::AngleBetweenVectors(Vector4& firstVector, Vector4& secondVector)
{
	Vector4 normalizedFirst = firstVector.Normalize();
	Vector4 normalizedSecond = secondVector.Normalize();
	float angle = DotProduct(normalizedFirst, normalizedSecond);
	return acos(angle);
}

Vector4 Vector4::Lerp(Vector4& origin, Vector4& destination, float time)
{
	return origin + (destination - origin) * time;
}

Vector3 Vector4::NormalizeXYZ(const Vector4& refVector)
{
	Vector3 xyzVector(refVector.x, refVector.y, refVector.z);
	xyzVector = xyzVector.Normalize();
	return xyzVector;
}

bool Vector4::operator==(const Vector4& other)const
{
	float epsilion = 0.001f;
	if (fabs(x - other.x) < epsilion && fabs(y - other.y) < epsilion && fabs(z - other.z) < epsilion) return true;
	else return false;
}

Vector4 Vector4::Normalize() const
{
	float magnitude = Magnitude();
	if (magnitude < 0.00001f) return Vector4(0, 0, 0, 0);
	return Vector4(x / magnitude, y / magnitude, z / magnitude, w/magnitude);
}

float Vector4::Magnitude()const
{
	float magnitude = sqrt(x * x + y * y + z * z);
	return magnitude;
}

bool Vector4::IsZero()
{
	if (x == 0 && y == 0 && z == 0) return true;
	else return false;
}

Vector4 Vector4::Project(Vector4& onto)
{
	if (!onto.IsZero()) {
		float scalar = DotProduct(*this, onto) / DotProduct(onto, onto);
		return onto * scalar;
	}
	else
		return onto;
}

Vector4 Vector4::Reflection(Vector4& normal)
{
	return *this - normal * (2 * DotProduct(*this, normal));
}

Vector3 Vector4::ToVector3()
{
	return Vector3(x,y,z);
}

Vector3 Vector4::ToHomogenousVector3() const
{
	if (w != 0.00f) {
		return Vector3(x / w, y / w, z / w);
	}
	return Vector3(x,y,z);
}

bool Vector4::IsPoint()const
{
	return (fabs(w - 1.0f) < 0.0001f);
	
}

bool Vector4::IsDirection()const
{
	return (fabs(w) < 0.0001f);
}

float Vector4::GetComponent(int index)const
{

	switch (index) {
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	default: return 0;
	}
	
}

void Vector4::SetComponent(int index, float value)
{
	switch (index) {
	case 0: x = value; break;
	case 1: y = value; break;
	case 2: z = value; break;
	case 3: w = value; break;
	default: break;
	}
}
