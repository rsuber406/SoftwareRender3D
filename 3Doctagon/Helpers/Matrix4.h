#pragma once
#include "Vector4.h"

#define DIMENSION_OF_MATRIX 4

class Matrix4
{
	Vector4 rows[DIMENSION_OF_MATRIX];

public:

	Matrix4();
	Matrix4(Vector4& firstRow, Vector4& secondRow, Vector4& thirdRow, Vector4& fourthRow);
	Matrix4(Vector4 firstRow, Vector4 secondRow, Vector4 thirdRow, Vector4 fourthRow);
	Vector4 GetRow(int row)const;
	Vector4 GetColumn(int col)const ;
	void Set(int row, int col, float value);
	float Get(int row, int col)const;
	float Determinant();
	Matrix4 Transpose();
	Matrix4 Inverse();
	Matrix4 CalculateCofactors();
	Matrix4 operator+ (const Matrix4& other);
	Matrix4 operator-(const Matrix4& other);
	Vector4 operator* (const Vector4& other)const ;
	
	Matrix4 operator* (const float scalar);
	Matrix4 operator* (const Matrix4& other);
	bool operator== (const Matrix4& other)const ;
	static float Det3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i);
	static Matrix4 Identity() { return Matrix4(); }
	static Matrix4 Zero();
	static Matrix4 Translation(float x, float y, float z);
	static Matrix4 Translation(Vector3 position);
	static Matrix4 Scaling(float x, float y, float z);
	static Matrix4 RotationX(float angle);
	static Matrix4 RotationY(float angle);
	static Matrix4 RotationZ(float angle);
	static Matrix4 Perspective(float fov, float near, float far, float aspect);
	static Matrix4 OrthographicProjection(float right, float left, float top, float bottom, float near, float far);
	static Matrix4 LookAt(Vector3 eye, Vector3 target, Vector3 up);
};


inline Matrix4 Matrix4::operator*(const float scalar)
{
	return Matrix4(
	rows[0] * scalar,
	rows[1] * scalar,
	rows[2] * scalar,
	rows[3] * scalar
	);
}
