#pragma once


class Vector2
{
	float x;
	float y;



public:
	Vector2(float x, float y);
	Vector2();
	Vector2(const Vector2& copy);
	float GetX()const { return x; }
	float GetY()const { return y; }
	
	void SetX(float x) { this->x = x; }
	void SetY(float y) { this->y = y; }
	
	Vector2 operator+(const Vector2& vector);
	Vector2 operator-(const Vector2& vector);
	template<typename T>
	Vector2 operator*(const T& scalar);
	bool operator ==(const Vector2& other);
	Vector2 Normalize()const;
	Vector2 Project(Vector2& onto);
	Vector2 Reflection(Vector2& normal);
	float Magnitude()const;
	bool IsZero();
	static float DotProduct(Vector2& firstVector, Vector2& secondVector);
	static float Distance(Vector2& firstVector, Vector2& secondVector);
	static float AngleBetweenVectors(Vector2& firstVector, Vector2& secondVector);
	static Vector2 Lerp(Vector2& origin, Vector2& destination, float time);
};

template<typename T>
inline Vector2 Vector2::operator*(const T& scalar)
{

	return Vector2(x * scalar, y * scalar);
}


