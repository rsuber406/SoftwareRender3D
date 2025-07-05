#pragma once
#include "./Helpers/RasterSurface.h"
#include <vector>
#include "Shape.h"
#include "Camera.h"
#define WIDTH 1920
#define HEIGHT 1080
#define	TOTAL_PIXEL WIDTH * HEIGHT
typedef std::vector<std::vector<Vector3>> StoredShape;

struct Actor 
{
	StoredShape vertices;
	Matrix4 worldMatrix;
	Vector3 position;
	bool rotate = false;
	float rotationModifier = 0.00f;
};
typedef std::vector<Vector3> Face;
typedef std::vector<Actor> Scene;


class Window
{


public:
	Window();
	bool IsWindowAlive();
	void UpdateLoop();
	void KillWindow();
	~Window();

private:

	void ClearScreen();
	void UpdateActors();
	void BuildScene();
	void RenderOctagon();
	void RenderShapes(Scene sceneToRender);
	void DrawLines(Vector3& from, Vector3& to, Matrix4& worldMatrix);
	void TakeShape(Actor& actor);
	void HandleFace(Face faceToDraw, Matrix4& worldMatrix);
	void PointToPixel(Vector3& point, Matrix4& worldMatrix);
	Matrix4 CreateWorldMatrix(Vector3& desiredPosition);
	unsigned int LerpBlend(unsigned int frontColor, unsigned int backColor);
	unsigned int* pixels = nullptr;
	bool keepAlive = false;
	Camera* camera = nullptr;
	Scene objectsToRender;
};

