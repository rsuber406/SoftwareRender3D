#pragma once
#include "./Helpers/MathLib.h"
#include <vector>
#include <cstdint>
class Shape
{

public:

	std::vector<std::vector<Vector3>> GeneratePoints(uint8_t polygonSides, uint32_t polygonSize, uint32_t polygonHeight, Vector3& center, bool isPlane = false);
};

