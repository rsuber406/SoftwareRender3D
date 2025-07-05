#include "Matrix4.h"
#include <cmath>

Matrix4::Matrix4()
{
	Vector4 first(1, 0, 0, 0);
	Vector4 second(0, 1, 0, 0);
	Vector4 third(0, 0, 1, 0);
	Vector4 fourth(0, 0, 0, 1);
	rows[0] = first;
	rows[1] = second;
	rows[2] = third;
	rows[3] = fourth;
}

Matrix4::Matrix4(Vector4& firstRow, Vector4& secondRow, Vector4& thirdRow, Vector4& fourthRow)
{
	rows[0] = firstRow;
	rows[1] = secondRow;
	rows[2] = thirdRow;
	rows[3] = fourthRow;
}

Matrix4::Matrix4(Vector4 firstRow, Vector4 secondRow, Vector4 thirdRow, Vector4 fourthRow)
{
	rows[0] = firstRow;
	rows[1] = secondRow;
	rows[2] = thirdRow;
	rows[3] = fourthRow;
}

Vector4 Matrix4::GetRow(int row)const
{
	if (row > DIMENSION_OF_MATRIX) return Vector4(0, 0, 0, 0);
	else
		return rows[row];
}

Vector4 Matrix4::GetColumn(int col)const
{
	if (col >= DIMENSION_OF_MATRIX) return Vector4(0, 0, 0, 0);

	return Vector4(
		rows[0].GetComponent(col),
		rows[1].GetComponent(col),
		rows[2].GetComponent(col),
		rows[3].GetComponent(col)
	);
}

void Matrix4::Set(int row, int col, float value)
{
	rows[row].SetComponent(col, value);
}

float Matrix4::Get(int row, int col) const
{
	return rows[row].GetComponent(col);
}

float Matrix4::Determinant()
{
	float a = Get(0, 0);
	float b = Get(0, 1);
	float c = Get(0, 2);
	float d = Get(0, 3);

	float det0 = Det3x3(Get(1, 1), Get(1, 2), Get(1, 3),
					    Get(2, 1), Get(2, 2), Get(2, 3),
						Get(3, 1), Get(3, 2), Get(3, 3));
	float det1 = Det3x3(Get(1, 0), Get(1, 2), Get(1, 3),
						Get(2, 0), Get(2, 2), Get(2, 3),
						Get(3, 0), Get(3, 2), Get(3, 3)
	);
	float det2 = Det3x3(Get(1, 0), Get(1, 1), Get(1, 3),
		Get(2, 0), Get(2, 1), Get(2, 3),
		Get(3, 0), Get(3, 1), Get(3, 3)
	);
	float det3 = Det3x3(Get(1, 0), Get(1, 1), Get(1, 2),
		Get(2, 0), Get(2, 1), Get(2, 2),
		Get(3, 0), Get(3, 1), Get(3, 2)
	);



	return a * det0 - b * det1 + c * det2 - d * det3;
}

Matrix4 Matrix4::Transpose()
{
	Matrix4 result(

		GetColumn(0),
		GetColumn(1),
		GetColumn(2),
		GetColumn(3)
		);

	

	return result;
}

Matrix4 Matrix4::Inverse()
{
	float det = this->Determinant();
	if (det == 0) return Matrix4();

	Matrix4 cofactor = CalculateCofactors();
	Matrix4 abjugate = cofactor.Transpose();
	return abjugate * (1.0f / det);

}

Matrix4 Matrix4::CalculateCofactors()
{
	// Row 0
	float cofactor0_0 = +1 * Det3x3(Get(1, 1), Get(1, 2), Get(1, 3), Get(2, 1), Get(2, 2), Get(2, 3), Get(3, 1), Get(3, 2), Get(3, 3));
	float cofactor0_1 = -1 * Det3x3(Get(1, 0), Get(1, 2), Get(1, 3), Get(2, 0), Get(2, 2), Get(2, 3), Get(3, 0), Get(3, 2), Get(3, 3));
	float cofactor0_2 = +1 * Det3x3(Get(1, 0), Get(1, 1), Get(1, 3), Get(2, 0), Get(2, 1), Get(2, 3), Get(3, 0), Get(3, 1), Get(3, 3));
	float cofactor0_3 = -1 * Det3x3(Get(1, 0), Get(1, 1), Get(1, 2), Get(2, 0), Get(2, 1), Get(2, 2), Get(3, 0), Get(3, 1), Get(3, 2));

	// Row 1  
	float cofactor1_0 = -1 * Det3x3(Get(0, 1), Get(0, 2), Get(0, 3), Get(2, 1), Get(2, 2), Get(2, 3), Get(3, 1), Get(3, 2), Get(3, 3));
	float cofactor1_1 = +1 * Det3x3(Get(0, 0), Get(0, 2), Get(0, 3), Get(2, 0), Get(2, 2), Get(2, 3), Get(3, 0), Get(3, 2), Get(3, 3));
	float cofactor1_2 = -1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 3), Get(2, 0), Get(2, 1), Get(2, 3), Get(3, 0), Get(3, 1), Get(3, 3));
	float cofactor1_3 = +1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 2), Get(2, 0), Get(2, 1), Get(2, 2), Get(3, 0), Get(3, 1), Get(3, 2));

	// Row 2
	float cofactor2_0 = +1 * Det3x3(Get(0, 1), Get(0, 2), Get(0, 3), Get(1, 1), Get(1, 2), Get(1, 3), Get(3, 1), Get(3, 2), Get(3, 3));
	float cofactor2_1 = -1 * Det3x3(Get(0, 0), Get(0, 2), Get(0, 3), Get(1, 0), Get(1, 2), Get(1, 3), Get(3, 0), Get(3, 2), Get(3, 3));
	float cofactor2_2 = +1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 3), Get(1, 0), Get(1, 1), Get(1, 3), Get(3, 0), Get(3, 1), Get(3, 3));
	float cofactor2_3 = -1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 2), Get(1, 0), Get(1, 1), Get(1, 2), Get(3, 0), Get(3, 1), Get(3, 2));

	// Row 3
	float cofactor3_0 = -1 * Det3x3(Get(0, 1), Get(0, 2), Get(0, 3), Get(1, 1), Get(1, 2), Get(1, 3), Get(2, 1), Get(2, 2), Get(2, 3));
	float cofactor3_1 = +1 * Det3x3(Get(0, 0), Get(0, 2), Get(0, 3), Get(1, 0), Get(1, 2), Get(1, 3), Get(2, 0), Get(2, 2), Get(2, 3));
	float cofactor3_2 = -1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 3), Get(1, 0), Get(1, 1), Get(1, 3), Get(2, 0), Get(2, 1), Get(2, 3));
	float cofactor3_3 = +1 * Det3x3(Get(0, 0), Get(0, 1), Get(0, 2), Get(1, 0), Get(1, 1), Get(1, 2), Get(2, 0), Get(2, 1), Get(2, 2));
	
	Matrix4 cofactor(Vector4(cofactor0_0, cofactor0_1, cofactor0_2, cofactor0_3),
		Vector4(cofactor1_0, cofactor1_1, cofactor1_2, cofactor1_3),
		Vector4(cofactor2_0, cofactor2_1, cofactor2_2, cofactor2_3),
		Vector4(cofactor3_0, cofactor3_1, cofactor3_2, cofactor3_3));
		return cofactor;
}

Matrix4 Matrix4::operator+(const Matrix4& other)
{
	return Matrix4(
		rows[0] + other.rows[0],
		rows[1] + other.rows[1],
		rows[2] + other.rows[2],
		rows[3] + other.rows[3]
	);
}

Matrix4 Matrix4::operator-(const Matrix4& other)
{
	return Matrix4(
		rows[0] - other.rows[0],
		rows[1] - other.rows[1],
		rows[2] - other.rows[2],
		rows[3] - other.rows[3]
	);
}

Vector4 Matrix4::operator*(const Vector4& other)const
{
	return Vector4(
		Vector4::DotProduct(rows[0], other),
		Vector4::DotProduct(rows[1], other),
		Vector4::DotProduct(rows[2], other),
		Vector4::DotProduct(rows[3], other)
	);
}

Matrix4 Matrix4::operator*(const Matrix4& other)
{
	Matrix4 resultMatrix;
	for (int row = 0; row < DIMENSION_OF_MATRIX; row++) {
		for (int col = 0; col < DIMENSION_OF_MATRIX; col++) {

			Vector4 leftRow = rows[row];
			Vector4 rightCol = other.GetColumn(col);
			float result = Vector4::DotProduct(leftRow, rightCol);
			resultMatrix.Set(row, col, result);
		}
	}
	return resultMatrix;
}

bool Matrix4::operator==(const Matrix4& other)const
{
	if (
		rows[0] == other.GetRow(0) &&
		rows[1] == other.GetRow(1) &&
		rows[2] == other.GetRow(2) &&
		rows[3] == other.GetRow(3)
		) return true;
	else return false;
}

float Matrix4::Det3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{


	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - g * e);
}

Matrix4 Matrix4::Zero()
{
	
	return Matrix4(Vector4(0,0,0,0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0) );
}

Matrix4 Matrix4::Translation(float x, float y, float z)
{
	return Matrix4(
		Vector4(1, 0, 0, x),
		Vector4(0, 1, 0, y),
		Vector4(0, 0, 1, z),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::Translation(Vector3 position)
{
	float x = position.GetX();
	float y = position.GetY();
	float z = position.GetZ();
	return Matrix4(
		Vector4(1, 0, 0, x),
		Vector4(0, 1, 0, y),
		Vector4(0, 0, 1, z),
		Vector4(0, 0, 0, 1));
}

Matrix4 Matrix4::Scaling(float x, float y, float z)
{
	return Matrix4(
		Vector4(x, 0, 0, 0),
		Vector4(0, y, 0, 0),
		Vector4(0, 0, z, 0),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::RotationX(float angle)
{
	float c = cos(angle);
	float s = sin(angle);

	return Matrix4(
		Vector4(1, 0, 0, 0),
		Vector4(0, c, s, 0),
		Vector4(0, -s, c, 0),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::RotationY(float angle)
{
	float c = cos(angle);
	float s = sin(angle);

	return Matrix4(
		Vector4(c, 0, -s, 0),
		Vector4(0, 1, 0, 0),
		Vector4(s, 0, c, 0),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::RotationZ(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return Matrix4(
		Vector4(c, s, 0, 0),
		Vector4(-s, c, 0, 0),
		Vector4(0, 0, 1, 0),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::Perspective(float fov, float near, float far, float aspect)
{
	float focalLength = 1.0f / tan(fov * 0.5f);
	float rangeInverse = 1.0f / (near - far);

	return Matrix4(
		Vector4(focalLength / aspect, 0, 0, 0),
		Vector4(0, focalLength, 0, 0),
		Vector4(0, 0, (far + near) * rangeInverse, 2 * far * near * rangeInverse),
		Vector4(0, 0, -1, 0)
	);
}

Matrix4 Matrix4::OrthographicProjection(float right, float left, float top, float bottom, float near, float far)
{
	return Matrix4(
		Vector4(2 / (right - left), 0, 0, -(right + left) / (right - left)),
		Vector4(0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom)),
		Vector4(0, 0, -2 / (far - near), -(far + near) / (far - near)),
		Vector4(0, 0, 0, 1)
	);
}

Matrix4 Matrix4::LookAt(Vector3 eye, Vector3 target, Vector3 up)
{
	Vector3 forward = target - eye;
	forward = forward.Normalize();
	Vector3 right = Vector3::CrossProduct(forward, up);
	right = right.Normalize();
	Vector3 newUp = Vector3::CrossProduct(right, forward);

	return Matrix4(
		Vector4(right.GetX(), right.GetY(), right.GetZ(), -Vector3::DotProduct(right, eye)),
		Vector4(newUp.GetX(), newUp.GetY(), newUp.GetZ(), -Vector3::DotProduct(newUp, eye)),
		Vector4(-forward.GetX(), -forward.GetY(), -forward.GetZ(), Vector3::DotProduct(forward, eye)),
		Vector4(0, 0, 0, 1)
	);
}
