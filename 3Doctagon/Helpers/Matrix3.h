#pragma once
#include "Vector4.h"
#include "Vector2.h"
#define DIMENSION_OF_MATRIX3 3
class Matrix3
{
	Vector3 rows[DIMENSION_OF_MATRIX3];

public:

	Matrix3();
	Matrix3(Vector3& firstRow, Vector3& secondRow, Vector3& thirdRow);
	Matrix3(const Vector3 firstRow,const Vector3 secondRow, const Vector3 thirdRow);
	Vector3 GetRow(int row)const;
	Vector3 GetColumn(int col)const;
	void Set(int row, int col, float value);
	float Get(int row, int col)const;
	float Determinant();
	Matrix3 Transpose();
	Matrix3 Inverse();
	Matrix3 CalculateCofactors();
	Matrix3 operator+ (const Matrix3& other);
	Matrix3 operator-(const Matrix3& other);
	Vector3 operator* (const Vector3& other)const;
	Vector2 operator* (const Vector2& other);
	Matrix3 operator* (const float scalar);
	Matrix3 operator* (const Matrix3& other);
	bool operator== (const Matrix3& other)const;
	static float Det3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i);
	static Matrix3 Identity() { return Matrix3(); }
	static Matrix3 Zero();
	static Matrix3 Translation(float x, float y);
	static Matrix3 Scaling(float x, float y);
	static Matrix3 Rotation(float angle);

};

inline Matrix3 Matrix3::operator*(const float scalar)
{
	return Matrix3(
		rows[0] * scalar,
		rows[1] * scalar,
		rows[2] * scalar
	);
}

