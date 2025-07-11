#pragma once
#include "./Helpers/MathLib.h"
#include <vector>
#include <cstdint>
class Shape
{
	void GenerateLines(std::vector<Vector3>& lineVector, std::vector<Vector3> &bottomPoints);
public:

	std::vector<std::vector<Vector3>> GeneratePoints(uint8_t polygonSides, float polygonSize, float polygonHeight, Vector3& center, bool isPlane = false);
};

