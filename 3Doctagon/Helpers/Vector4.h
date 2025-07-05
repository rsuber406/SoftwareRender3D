#pragma once
#include "Vector3.h"
class Vector4
{
	float x;
	float y;
	float z;
	float w;

public:
	Vector4(float x, float y, float z, float w);
	Vector4();
	Vector4(const Vector4& copy);
	Vector4(Vector3& other);
	float GetX() { return x; }
	float GetY() { return y; }
	float GetZ() { return z; }
	float GetW() { return w; }
	void SetX(float x) { this->x = x; }
	void SetY(float y) { this->y = y; }
	void SetZ(float z) { this->z = z; }
	Vector4 operator+(const Vector4& vector);
	Vector4 operator-(const Vector4& vector);
	template<typename T>
	Vector4 operator*(const T& scalar);
	bool operator ==(const Vector4& other)const;
	Vector4 Normalize()const;
	Vector4 Project(Vector4& onto);
	Vector4 Reflection(Vector4& normal);
	Vector3 ToVector3();
	Vector3 ToHomogenousVector3()const ;
	bool IsPoint()const;
	bool IsDirection()const;
	float GetComponent(int index) const;
	void SetComponent(int index, float value);
	float Magnitude()const;
	bool IsZero();
	static float DotProduct(const Vector4& firstVector, const Vector4& secondVector);
	static float Distance(Vector4& firstVector, Vector4& secondVector);
	static float AngleBetweenVectors(Vector4& firstVector, Vector4& secondVector);
	static Vector4 Lerp(Vector4& origin, Vector4& destination, float time);
	static Vector3 NormalizeXYZ(const Vector4& refVector);
};

template<typename T>
inline Vector4 Vector4::operator*(const T& scalar)
{

	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

