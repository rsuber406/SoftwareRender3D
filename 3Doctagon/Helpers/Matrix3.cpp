#include "Matrix3.h"
#include <cmath>
#include "Matrix4.h"
#include <cmath>

Matrix3::Matrix3()
{
	Vector3 first(1, 0, 0);
	Vector3 second(0, 1, 0);
	Vector3 third(0, 0, 1);
	rows[0] = first;
	rows[1] = second;
	rows[2] = third;
}

Matrix3::Matrix3(Vector3& firstRow, Vector3& secondRow, Vector3& thirdRow)
{
	rows[0] = firstRow;
	rows[1] = secondRow;
	rows[2] = thirdRow;
}

Matrix3::Matrix3(const Vector3 firstRow, const Vector3 secondRow, const Vector3 thirdRow)
{
	rows[0] = firstRow;
	rows[1] = secondRow;
	rows[2] = thirdRow;

}
Vector2 Matrix3::operator*(const Vector2& other)
{
	Vector3 homogenous(other.GetX(), other.GetY(), 1.0f);
	Vector3 result = *this * homogenous;
	return Vector2(result.GetX(), result.GetY());
}

Vector3 Matrix3::GetRow(int row)const
{
	if (row >= DIMENSION_OF_MATRIX3) return Vector3(0, 0, 0);
	else
		return rows[row];
}

Vector3 Matrix3::GetColumn(int col)const
{
	if (col >= DIMENSION_OF_MATRIX3) return Vector3(0, 0, 0);

	return Vector3(
		rows[0].GetComponent(col),
		rows[1].GetComponent(col),
		rows[2].GetComponent(col)
	);
}

void Matrix3::Set(int row, int col, float value)
{
	rows[row].SetComponent(col, value);
}

float Matrix3::Get(int row, int col) const
{
	return rows[row].GetComponent(col);
}

float Matrix3::Determinant()
{
	
	float a = Get(0, 0), b = Get(0, 1), c = Get(0, 2);
	float d = Get(1, 0), e = Get(1, 1), f = Get(1, 2);
	float g = Get(2, 0), h = Get(2, 1), i = Get(2, 2);

	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

Matrix3 Matrix3::Transpose()
{
	Matrix3 result(

		GetColumn(0),
		GetColumn(1),
		GetColumn(2)
	);



	return result;
}

Matrix3 Matrix3::Inverse()
{
	// needs totally remade
	float det = this->Determinant();
	if (det < 0.0001f) return Matrix3();

	Matrix3 cofactor = CalculateCofactors();
	Matrix3 abjugate = cofactor.Transpose();
	return abjugate * (1.0f / det);

}

Matrix3 Matrix3::CalculateCofactors()
{

	float cofactor0_0 = +1 * (Get(1, 1) * Get(2, 2) - Get(1, 2) * Get(2, 1));
	float cofactor0_1 = -1 * (Get(1, 0) * Get(2, 2) - Get(1, 2) * Get(2, 0));
	float cofactor0_2 = +1 * (Get(1, 0) * Get(2, 1) - Get(1, 1) * Get(2, 0));

	float cofactor1_0 = -1 * (Get(0, 1) * Get(2, 2) - Get(0, 2) * Get(2, 1));
	float cofactor1_1 = +1 * (Get(0, 0) * Get(2, 2) - Get(0, 2) * Get(2, 0));
	float cofactor1_2 = -1 * (Get(0, 0) * Get(2, 1) - Get(0, 1) * Get(2, 0));

	float cofactor2_0 = +1 * (Get(0, 1) * Get(1, 2) - Get(0, 2) * Get(1, 1));
	float cofactor2_1 = -1 * (Get(0, 0) * Get(1, 2) - Get(0, 2) * Get(1, 0));
	float cofactor2_2 = +1 * (Get(0, 0) * Get(1, 1) - Get(0, 1) * Get(1, 0));

	Matrix3 cofactor(Vector3(cofactor0_0, cofactor0_1, cofactor0_2),
		Vector3(cofactor1_0, cofactor1_1, cofactor1_2),
		Vector3(cofactor2_0, cofactor2_1, cofactor2_2));
	return cofactor;
}

Matrix3 Matrix3::operator+(const Matrix3& other)
{
	return Matrix3(
		rows[0] + other.rows[0],
		rows[1] + other.rows[1],
		rows[2] + other.rows[2]
	);
}

Matrix3 Matrix3::operator-(const Matrix3& other)
{
	return Matrix3(
		rows[0] - other.rows[0],
		rows[1] - other.rows[1],
		rows[2] - other.rows[2]
	);
}

Vector3 Matrix3::operator*(const Vector3& other)const
{
	return Vector3(
		Vector3::DotProduct(rows[0], other),
		Vector3::DotProduct(rows[1], other),
		Vector3::DotProduct(rows[2], other)
	);
}



Matrix3 Matrix3::operator*(const Matrix3& other)
{
	Matrix3 resultMatrix;
	for (int row = 0; row < DIMENSION_OF_MATRIX3; row++) {
		for (int col = 0; col < DIMENSION_OF_MATRIX3; col++) {

			Vector3 leftRow = rows[row];
			Vector3 rightCol = other.GetColumn(col);
			float result = Vector3::DotProduct(leftRow, rightCol);
			resultMatrix.Set(row, col, result);
		}
	}
	return resultMatrix;
}

bool Matrix3::operator==(const Matrix3& other)const
{
	if (
		rows[0] == other.GetRow(0) &&
		rows[1] == other.GetRow(1) &&
		rows[2] == other.GetRow(2)
		) return true;
	else return false;
}

float Matrix3::Det3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{


	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - g * e);
}

Matrix3 Matrix3::Zero()
{

	return Matrix3(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
}

Matrix3 Matrix3::Translation(float x, float y)
{
	return Matrix3(
		Vector3(1, 0, x),  
		Vector3(0, 1, y),  
		Vector3(0, 0, 1)   
	);
}

Matrix3 Matrix3::Scaling(float x, float y)
{
	return Matrix3(
		Vector3(x, 0, 0), 
		Vector3(0, y, 0), 
		Vector3(0, 0, 1)   
	);
}

Matrix3 Matrix3::Rotation(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	return Matrix3(
		Vector3(c, s, 0), 
		Vector3(-s, c, 0),  
		Vector3(0, 0, 1)   
	);
}


