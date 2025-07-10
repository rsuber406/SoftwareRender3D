#include "Shape.h"
#define PI 3.14159f

std::vector<std::vector<Vector3>> Shape::GeneratePoints(uint8_t polygonSides, float polygonSize, float polygonHeight, Vector3& center, bool isPlane)
{
	Vector3 radius(polygonSize, 0, 0);

	float angle = (2.0f * PI) / polygonSides;
	std::vector<Vector3> bottomPoints(polygonSides);
	Matrix3 offSetRot;
	for (int i = 0; i < polygonSides; i++)
	{
		float angleStep = i * angle;
		Matrix3 rotation = Matrix3::Rotation(angleStep);
		if (isPlane)
			Matrix3 offSetRot = Matrix3::Rotation(90);
		if (!isPlane) {
			Vector3 point = rotation * radius + center;
			bottomPoints[i] = point;
		}
		else {
			Vector3 point = offSetRot * rotation * radius + center;
			bottomPoints[i] = point;
		}
	}
	if (isPlane) {
		std::vector<Vector3> linesForPlane;
		
		std::vector < std::vector<Vector3>> sourcePoints;
		sourcePoints.push_back(bottomPoints);
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
