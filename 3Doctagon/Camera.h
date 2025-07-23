#pragma once
#include "./Helpers/MathLib.h"
#include <cstdint>
class Camera
{
public:

	Camera(Vector3& position, Vector3& target, Vector3& up, float fov, float aspect, float nearPlane, float farPlane, 
		uint32_t width, uint32_t height  );
	Vector2 WorldToScreenPixel(Vector3& worldPosition, Matrix4& worldMatrix, float& zDepth);
	Vector2 WorldToScreenPixel(Vector3& worldPosition, Matrix4& worldMatrix);
	Vector3 GetPosition() { return position; }
	void SetPosition(Vector3 position);
	void RebuildMatrices();
/// <summary>
/// If camera controls are added, you must rebuild the view matrix and projection matrix
/// 
/// </summary>
private:

	Matrix4 GetViewMatrix();
	Matrix4 GetPerspectiveMatrix();
	Vector3 position;
	Vector3 target;
	Vector3 up;
	Matrix4 viewMatrix;
	Matrix4 projection;
	float fov;
	float aspect;
	float nearPlane;
	float farPlane;
	uint32_t screenWidth;
	uint32_t screenHeight;

};

