#pragma once
class Vector3
{
	float x;
	float y;
	float z;

	
public:
	Vector3(float x, float y, float z);
	Vector3();
	Vector3(const Vector3& copy);
	float GetX() { return x; }
	float GetY() { return y; }
	float GetZ() { return z; }
	void SetX(float x) { this->x = x; }
	void SetY(float y) { this->y = y; }
	void SetZ(float z) { this->y = z; }
	Vector3 operator+(const Vector3& vector);
	Vector3 operator-(const Vector3& vector);
	template<typename T>
	Vector3 operator*(const T& scalar);
	bool operator ==(const Vector3& other)const ;
	Vector3 Normalize()const;
	Vector3 Project(Vector3& onto);
	Vector3 Reflection(Vector3& normal);
	float GetComponent(int col)const;
	void SetComponent(int index, float value);
	float Magnitude()const;
	bool IsZero();
	static float DotProduct(const Vector3& firstVector, const Vector3& secondVector);
	static float Distance(Vector3& firstVector, Vector3& secondVector);
	static Vector3 CrossProduct(Vector3& firstVector, Vector3& secondVector);
	static float AngleBetweenVectors(Vector3& firstVector, Vector3& secondVector);
	static Vector3 Lerp(Vector3& origin, Vector3& destination, float time);
};

template<typename T>
inline Vector3 Vector3::operator*(const T& scalar)
{

	return Vector3(x * scalar, y * scalar, z* scalar);
}
