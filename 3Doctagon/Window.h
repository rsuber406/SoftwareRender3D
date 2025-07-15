#pragma once
#include "./Helpers/RasterSurface.h"
#include <vector>
#include "Shape.h"
#include "Camera.h"
#include <thread>
#include <mutex>
#include "Helpers/greendragon.h"
#include <condition_variable>

#define MAX_NUMBER_RASTER_THREADS 15
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

struct ThreadData {
	bool* keepAlive = nullptr;
	uint8_t threadId;
	uint64_t* runThreadFlag = nullptr;
	uint16_t* threadCount = nullptr;
	std::vector<Matrix4> worldMatrixRef;
	std::condition_variable* windowRasterCond = nullptr;
	std::condition_variable* signalNextFrame = nullptr;
	std::mutex* windowRasterMutex = nullptr;
	Triangle triangle;
};

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
	void CreateRasterThreads();
	void ThreadEntryPoint(ThreadData* threadData);
	void RasterThreadLivingPoint(ThreadData* threadData);
	void UpdateActors();
	void BuildScene();
	void RenderOctagon();
	void BuildWeekTwoLab();
	void BuildWeekTwoOptional();
	void RasterScene();
	void ThreadRasterObject(Triangle& triangle, std::vector<Matrix4>& worldMatrix);
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
	float* depthBuffer = nullptr;
	uint64_t threadFlags;
	Camera* camera = nullptr;
	Scene objectsToRender;
	ThreadData* rasterThreads = nullptr;
	std::condition_variable windowRasterCond;
	std::condition_variable signalNextFrame;
	std::mutex windowRasterMutex;
	uint16_t threadCount = 0;
};

