#include "Shape.h"
#include <iostream>
#define PI 3.14159f

void Shape::GenerateLines(std::vector<Vector3>& lineVector,  std::vector<Vector3>& bottomPoints)
{
	float radius = 0.5f;
	float step = 0.1f;

	
	for (float y = -radius; y <= radius; y += step) {
		lineVector.push_back(Vector3(-radius, y, 0)); 
		lineVector.push_back(Vector3(radius, y, 0));
	}

	
	for (float x = -radius; x <= radius; x += step) {
		lineVector.push_back(Vector3(x, -radius, 0));
		lineVector.push_back(Vector3(x, radius, 0));
	}
}

std::vector<std::vector<Vector3>> Shape::GeneratePoints(uint8_t polygonSides, float polygonSize, float polygonHeight, Vector3& center, bool isPlane)
{
	Vector3 radius(polygonSize, 0, 0);
	float degreeOffset = polygonSides > 3 ? 45.0f : 30.4f;
	float angle = (2.0f * PI) / polygonSides;
	std::vector<Vector3> bottomPoints(polygonSides);
	Matrix3 offSetRot;
	for (int i = 0; i < polygonSides; i++)
	{
		float angleStep = i * angle;
		Matrix3 rotation = Matrix3::Rotation(angleStep);
		Matrix3 rotationToNormal = Matrix3::Rotation((degreeOffset * PI / 180.0f));
		//if (!isPlane)
		{
			Vector3 point = rotationToNormal * rotation * radius + center;
			bottomPoints[i] = point;
		}
		//else
		{
			//offSetRot = Matrix3::Rotation(90);
			//Vector3 point = offSetRot * rotation * radius + center;
			//bottomPoints[i] = point;
		}
	}
	if (isPlane) {
		std::vector<Vector3> linesForPlane;
		/// Calculate Plane lines
		GenerateLines(linesForPlane, bottomPoints);

		std::vector < std::vector<Vector3>> sourcePoints;
		sourcePoints.push_back(bottomPoints);
		sourcePoints.push_back(linesForPlane);
		return sourcePoints;
	}

	Vector3 zHeight(0, 0, polygonHeight);
	std::vector<Vector3> topPoints(polygonSides);
	for (int i = 0; i < bottomPoints.size(); i++) {
		topPoints[i] = bottomPoints[i] + zHeight;
	}

	std::vector<std::vector<Vector3>> allPoints;
	allPoints.push_back(bottomPoints);
	allPoints.push_back(topPoints);
	return allPoints;

}

std::vector<std::vector<Vector3>> Shape::GenerateSquare()
{
	Vector3 firstVertex(-0.25f, 0.25, 0);
	Vector3 secondVertex(-0.25f, -0.25, 0);
	Vector3 thirdVertex(0.25f, -0.25, 0);
	Vector3 fourthVertex(0.25f, 0.25, 0);

	std::vector<Vector3> base;
	base.push_back(firstVertex);
	base.push_back(secondVertex);
	base.push_back(thirdVertex);
	base.push_back(fourthVertex);
	std::vector<Vector3> top;
	for (int i = 0; i < base.size(); i++) {
		Vector3 topPoints(base[i].GetX(), base[i].GetY(), 0.5f);
		top.push_back(topPoints);
	}
	std::vector<std::vector<Vector3>> vertices;
	vertices.push_back(base);
	vertices.push_back(top);
	return vertices;
}
