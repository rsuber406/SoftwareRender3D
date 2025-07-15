#include "Camera.h"
#define PI 3.14159f
Camera::Camera(Vector3& position, Vector3& target, Vector3& up, float fov, float aspect, float nearPlane, float farPlane,
	uint32_t width, uint32_t height)
{
	this->position = position;
	this->target = target;
	this->up = Vector3(0, 0, 1);
	this->fov = (fov * PI) / 180;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	screenHeight = height;
	screenWidth = width;
	viewMatrix = GetViewMatrix();
	projection = GetPerspectiveMatrix();
}

Vector2 Camera::WorldToScreenPixel(Vector3& worldPosition, Matrix4& worldMatrix)
{
	//Matrix4 viewMatrix = GetViewMatrix();
	//Matrix4 projection = GetPerspectiveMatrix();
	Matrix4 combined = projection * viewMatrix * worldMatrix;
	Vector4 localPos = Vector4(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ(), 1.0f);
	Vector4 projectedPosition = combined * localPos;
	if (projectedPosition.GetW() != 0.00f) {

		projectedPosition.SetX(projectedPosition.GetX() / projectedPosition.GetW());
		projectedPosition.SetY(projectedPosition.GetY() / projectedPosition.GetW());
		projectedPosition.SetZ(projectedPosition.GetZ() / projectedPosition.GetW());
	}

	float screenX = (projectedPosition.GetX() + 1.0f) * 0.5f * screenWidth;
	float screenY = (projectedPosition.GetY() + 1.0f) * 0.5f * screenHeight;
	Vector2 screenPos(screenX, screenY);

	return screenPos;
}

Matrix4 Camera::GetViewMatrix()
{
	return Matrix4::LookAt(position, target, up);
}

Matrix4 Camera::GetPerspectiveMatrix()
{
	return Matrix4::Perspective(fov, nearPlane, farPlane, aspect);
}
