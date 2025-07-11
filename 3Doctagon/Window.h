#pragma once
#include "./Helpers/RasterSurface.h"
#include <vector>
#include "Shape.h"
#include "Camera.h"

typedef std::vector<std::vector<Vector3>> StoredShape;
typedef std::vector<std::vector<Vector3>> Triangle;
struct Actor 
{
	StoredShape vertices;
	Triangle triangles;
	Matrix4 worldMatrix;
	Vector3 position;
	bool rotate = false;
	bool isPlane = false;
	float rotationModifier = 0.00f;
	uint32_t color = 0xFFFFFFFF;
};
typedef std::vector<Vector3> Face;
typedef std::vector<Actor> Scene;

enum ShapeSides {
	Tri = 3,
	Square,
	Pentagon,
	Hexagon,
	Octagon
};

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
	void BuildWeekTwoLab();
	void BuildWeekTwoOptional();
	void RasterScene();
	void RasterObject(Actor& actor);
	void DetermineTriangles(Actor& actor);
	void RenderShapes(Scene sceneToRender);
	void DrawLines(Vector3& from, Vector3& to, Matrix4& worldMatrix, uint32_t& color);
	void TakeShape(Actor& actor);
	void DrawPlaneLines(Actor& actor);
	void HandleFace(Face faceToDraw, Matrix4& worldMatrix, uint32_t& color);
	void PointToPixel(Vector3& point, Matrix4& worldMatrix, uint32_t& color);
	Matrix4 CreateWorldMatrix(Vector3& desiredPosition);
	unsigned int LerpBlend(unsigned int frontColor, unsigned int backColor);
	unsigned int* pixels = nullptr;
	bool keepAlive = false;
	Camera* camera = nullptr;
	Scene objectsToRender;
};

