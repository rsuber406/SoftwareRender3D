#include "Window.h"
#include <cstdint>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <functional>
#define RENDER_LAB 2// zero is my trial version of all of this
#if RENDER_LAB == 0
#define CAMERA_X_POS 0.00f
#define CAMERA_Y_POS 70.0f
#define CAMERA_Z_POS 5.0f
#define WIDTH 1920
#define HEIGHT 1080
#define	TOTAL_PIXEL WIDTH * HEIGHT
#elif RENDER_LAB == 1
#define CAMERA_X_POS 0.0f
#define CAMERA_Y_POS -0.8f
#define CAMERA_Z_POS 0.25f
#define WIDTH 500
#define HEIGHT 500
#define	TOTAL_PIXEL WIDTH * HEIGHT
#elif RENDER_LAB == 2 // optional Assignment in week 2
#define CAMERA_X_POS 0.0f
#define CAMERA_Y_POS -3.0f
#define CAMERA_Z_POS 3.0f
#define WIDTH 500
#define HEIGHT 500
#define	TOTAL_PIXEL WIDTH * HEIGHT
#endif
#define FOV 90.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 10.0f
#define PI 3.14159f
Window::Window()
{
	keepAlive = RS_Initialize("Ryan Suber Programming Assignment 2", WIDTH, HEIGHT);
	CreateRasterThreads();
	Vector3 cameraPos(CAMERA_X_POS, CAMERA_Y_POS, CAMERA_Z_POS);
	Vector3 targetPos(0, 0, 0);
	Vector3 up(0, 0, 1);
	int width = WIDTH;
	int height = HEIGHT;
	float aspect = (float)width / (float)height;
	camera = new Camera(cameraPos, targetPos, up, FOV, aspect, NEAR_PLANE, FAR_PLANE, WIDTH, HEIGHT);
	pixels = new unsigned int[TOTAL_PIXEL];
	ClearScreen();
	BuildScene();
}

bool Window::IsWindowAlive()
{
	return keepAlive;
}

void Window::UpdateLoop()
{
	auto timeStamp = std::chrono::high_resolution_clock::now();
	auto timeCheck = timeStamp;

	while (keepAlive)
	{
		timeCheck = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeCheck - timeStamp);
		if (elapsed > std::chrono::milliseconds(32))
		{
			if (!keepAlive) return;
			ClearScreen();
			UpdateActors();
			RasterScene();
			timeStamp = timeCheck;
		}

		RS_Update(pixels, TOTAL_PIXEL);
	}
	std::cout << "Update loop is exiting\n";
}

void Window::KillWindow()
{
	keepAlive = false;
	threadFlags = UINT64_MAX;
	windowRasterCond.notify_all();
	
	{
		std::unique_lock<std::mutex> lock(windowRasterMutex);
		windowRasterCond.wait(lock, [&] {return threadCount == 0; });
	}
}

Window::~Window()
{

	if (pixels) delete[]pixels;
	if (camera) delete camera;
	if (rasterThreads) delete[] rasterThreads;
	RS_Shutdown();
}

void Window::ClearScreen()
{
	for (uint32_t i = 0; i < TOTAL_PIXEL; i++) {
		pixels[i] = 0x00000000;
	}
}

void Window::CreateRasterThreads()
{
	rasterThreads = new ThreadData[MAX_NUMBER_RASTER_THREADS];
	for (int i = 0; i < MAX_NUMBER_RASTER_THREADS; i++) {
		rasterThreads[i].keepAlive = &keepAlive;
		rasterThreads[i].runThreadFlag = &threadFlags;
		rasterThreads[i].windowRasterCond = &windowRasterCond;
		rasterThreads[i].windowRasterMutex = &windowRasterMutex;
		rasterThreads[i].threadCount = &threadCount;
		rasterThreads[i].threadId = i;
		rasterThreads[i].signalNextFrame = &signalNextFrame;
		auto bound = std::bind(&Window::ThreadEntryPoint, this, &rasterThreads[i]);
		std::thread(bound).detach();
	}
}

void Window::ThreadEntryPoint(ThreadData* threadData)
{
	threadData->windowRasterMutex->lock();
	(*threadData->threadCount)++;
	threadData->windowRasterMutex->unlock();

	RasterThreadLivingPoint(threadData);

	threadData->windowRasterMutex->lock();
	(*threadData->threadCount)--;

	threadData->windowRasterMutex->unlock();
	threadData->signalNextFrame->notify_all();
	threadData->windowRasterCond->notify_all();
}

void Window::RasterThreadLivingPoint(ThreadData* threadData)
{
	while (*threadData->keepAlive) {

		{
			std::unique_lock<std::mutex>lock(*threadData->windowRasterMutex);
			threadData->windowRasterCond->wait(lock, [&] {return *threadData->runThreadFlag & (1 << threadData->threadId); });
		}
		if (!(*threadData->keepAlive)) {
			return;
		}
	
		ThreadRasterObject(threadData->triangle, threadData->worldMatrixRef);
		threadData->triangle.clear();
		threadData->worldMatrixRef.clear();
		//threadData->windowRasterMutex->lock();
		*threadData->runThreadFlag &= ~(1 << threadData->threadId);
		//threadData->windowRasterMutex->unlock();
		threadData->signalNextFrame->notify_all();
	}
}

void Window::UpdateActors()
{
	Vector3 origin(0, 0, 0);
	for (int i = 0; i < objectsToRender.size(); i++) {

		if (objectsToRender[i].rotationModifier) {

			objectsToRender[i].rotationModifier += 1.0f;  // Add rotation speed

			Matrix4 moveToOrigin = Matrix4::Translation(objectsToRender[i].position * -1);
			Matrix4 rotation = Matrix4::RotationZ(objectsToRender[i].rotationModifier * PI / 180.0f);
			Matrix4 moveToPosition = Matrix4::Translation(objectsToRender[i].position);

			objectsToRender[i].worldMatrix = moveToPosition * rotation * moveToOrigin;

		}

		TakeShape(objectsToRender[i]);
	}
}

void Window::BuildScene()
{
	switch (RENDER_LAB) {
	case 1:
		BuildWeekTwoLab();
		break;
	case 2:
		BuildWeekTwoOptional();
		break;
	default:
		RenderOctagon();
		break;
	}
}

void Window::RenderOctagon()
{

	Shape shapeGen;
	Vector3 centerOfObject(10, 0, 0);
	Vector3 centerOfSquare(-20, 0, 0);
	Vector3 origin(0, 0, 0);
	Actor plane;
	plane.position = Vector3(0, 0, 0);
	plane.vertices = shapeGen.GeneratePoints(4, 50, 0, plane.position, true);
	plane.worldMatrix = CreateWorldMatrix(origin);
	plane.position = origin;
	Actor octagon;
	octagon.position = centerOfObject;
	octagon.vertices = shapeGen.GeneratePoints(8, 20, 10, octagon.position);
	octagon.worldMatrix = CreateWorldMatrix(centerOfObject);
	octagon.position = centerOfObject;
	Actor square;
	square.position = centerOfSquare;
	square.vertices = shapeGen.GeneratePoints(4, 10, 10, square.position);
	square.worldMatrix = CreateWorldMatrix(centerOfSquare);
	square.rotationModifier = 0.001f;
	square.position = centerOfSquare;
	square.rotate = true;
	objectsToRender.push_back(plane);
	objectsToRender.push_back(octagon);
	objectsToRender.push_back(square);

	RenderShapes(objectsToRender);
	RS_Update(pixels, TOTAL_PIXEL);
}

void Window::BuildWeekTwoLab()
{
	Vector3 planePos(0, 0, 0);
	Vector3 cubePos(0, 0, 0.0f);
	Actor plane;
	Shape createObjects;
	plane.position = planePos;
	plane.rotationModifier = 0.00f;
	plane.vertices = createObjects.GeneratePoints(4, 0.71f, 0, plane.position, true);
	plane.isPlane = true;
	Actor cube;
	cube.position = cubePos;
	cube.rotationModifier = 0.000f;
	cube.vertices = createObjects.GeneratePoints(4, 0.25f, 0.5f, cube.position);
	cube.color = 0xFF00FF00;
	Actor octagon;
	octagon.position = Vector3(0, 0, -0.75f);
	octagon.rotationModifier = 0.00f;
	octagon.color = 0xFFFF0000;
	octagon.vertices = createObjects.GeneratePoints(8, 0.25f, 0.5f, octagon.position);
	DetermineTriangles(cube);
	objectsToRender.push_back(plane);
	objectsToRender.push_back(cube);
	objectsToRender.push_back(octagon);
	RenderShapes(objectsToRender);
	RasterObject(cube);
	RS_Update(pixels, TOTAL_PIXEL);
}

void Window::BuildWeekTwoOptional()
{
	Actor triangle;
	Shape generator;
	triangle.position = Vector3(-0.0f, 0, -1);
	triangle.vertices = generator.GeneratePoints(3, 1, 1, triangle.position);
	triangle.rotationModifier = 0.000f;
	Actor triangle2;
	triangle2.position = Vector3(0.5f, 3, 0);
	triangle2.vertices = generator.GeneratePoints(3, 1, 1, triangle2.position);
	DetermineTriangles(triangle);
	DetermineTriangles(triangle2);
	objectsToRender.push_back(triangle);
	objectsToRender.push_back(triangle2);
	RenderShapes(objectsToRender);
	RasterScene();
	RS_Update(pixels, TOTAL_PIXEL);
}

void Window::RasterScene()
{
	int threadCounter = 0;
	for (int i = 0; i < objectsToRender.size(); i++) {
		for (int j = 0; j < objectsToRender[i].triangles.size(); j++) {
			if (!keepAlive) return;
			rasterThreads[threadCounter].triangle.push_back(objectsToRender[i].triangles[j]);
			rasterThreads[threadCounter].worldMatrixRef.push_back(objectsToRender[i].worldMatrix);
			threadFlags |= (1 << rasterThreads[threadCounter].threadId);
			threadCounter++;
			if (threadCounter >= MAX_NUMBER_RASTER_THREADS) threadCounter = 0;
		}
	}
	// This will work when I can properly store each item into vectors 
	windowRasterCond.notify_all();
	{
		std::unique_lock<std::mutex>lock(windowRasterMutex);
		signalNextFrame.wait(lock, [&] {return threadFlags == 0 || threadFlags == UINT64_MAX; });
	}
}

void Window::ThreadRasterObject(Triangle& triangle, std::vector<Matrix4>& worldMatrix)
{

	float epsilon = 0.0001f;
	for (int i = 0; i < triangle.size(); i++) {
		Vector3 a = triangle[i][1];
		Vector3 b = triangle[i][0];
		Vector3 c = triangle[i][2];
		Vector3 edge1 = b - a;
		Vector3 edge2 = c - a;
		Vector3 normalTriangle = Vector3::CrossProduct(edge2, edge1);
		normalTriangle = normalTriangle.Normalize();
		Vector3 triangleCenter = Vector3((a.GetX() + b.GetX() + c.GetX()) / 3.0f, (a.GetY() + b.GetY() + c.GetY()) / 3.0f, (a.GetZ() + b.GetZ() + c.GetZ()) / 3.0f);
		Vector3 viewDirection = camera->GetPosition() - triangleCenter;
		viewDirection = viewDirection.Normalize();
		float dotResult = Vector3::DotProduct(normalTriangle, viewDirection);
		if (dotResult <= epsilon) return;
		float deltaY = std::max({ a.GetY(), b.GetY(), c.GetY() }) - std::min({ a.GetY(), b.GetY(), c.GetY() });
		float deltaZ = std::max({ a.GetZ(), b.GetZ(), c.GetZ() }) - std::min({ a.GetZ(), b.GetZ(), c.GetZ() });
		float minX = 0;
		float maxX = 0;
		float deltaMax = 0;
		float deltaMin = 0;
		minX = std::min(std::min(a.GetX(), b.GetX()), c.GetX());
		maxX = std::max(std::max(a.GetX(), b.GetX()), c.GetX());
		if (deltaZ > deltaY) {
			deltaMin = std::min(std::min(a.GetZ(), b.GetZ()), c.GetZ());
			deltaMax = std::max(std::max(a.GetZ(), b.GetZ()), c.GetZ());
		}
		else {
			deltaMin = std::min(std::min(a.GetY(), b.GetY()), c.GetY());
			deltaMax = std::max(std::max(a.GetY(), b.GetY()), c.GetY());
		}


		Vector3 basis0 = c - a;
		Vector3 basis1 = b - a;
		float basis00 = Vector3::DotProduct(basis0, basis0);
		float basis01 = Vector3::DotProduct(basis0, basis1);
		float basis11 = Vector3::DotProduct(basis1, basis1);
		float inverse = 1.0f / ((basis00 * basis11) - (basis01 * basis01)); // Determinant, needed later
		float stepSize = (maxX - minX) / WIDTH;
		float stepSizeZ = (deltaMax - deltaMin) / HEIGHT;

		Vector3 worldPoint;
		for (float x = minX; x <= maxX; x += stepSize) {
			for (float z = deltaMin; z <= deltaMax; z += stepSizeZ) {
				worldPoint = deltaZ > deltaY ? Vector3(x, b.GetY(), z) : Vector3(x, z, b.GetZ());
				Vector3 testPoint = worldPoint - a;
				float testDot02 = Vector3::DotProduct(basis0, testPoint);
				float testDot12 = Vector3::DotProduct(basis1, testPoint);
				float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
				float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
				float w = 1 - u - v;

				if (u >= epsilon && v >= epsilon && w >= epsilon) // Point is in the bounds
				{
					uint8_t red = u * 0xFF;
					uint8_t green = v * 0xFF;
					uint8_t blue = w * 0xFF;
					uint8_t alpha = 0xFF;
					uint32_t color = (alpha << 24) | (red << 16) | (green << 8) | blue;
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, worldMatrix[i]);
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

						int pixelToChange = screenYPos * WIDTH + screenXPos;
						pixels[pixelToChange] = color;

					}
				}
			}
		}
	}

}

void Window::RasterObject(Actor& actor)
{
	float epsilon = 0.0001f;
	for (int i = 0; i < actor.triangles.size(); i++)
	{
		Vector3 a = actor.triangles[i][1];
		Vector3 b = actor.triangles[i][0];
		Vector3 c = actor.triangles[i][2];
		Vector3 edge1 = b - a;
		Vector3 edge2 = c - a;
		Vector3 normalTriangle = Vector3::CrossProduct(edge2, edge1);
		normalTriangle = normalTriangle.Normalize();
		Vector3 triangleCenter = Vector3(a.GetX() + b.GetX() + c.GetX() / 3.0f, a.GetY() + b.GetY() + c.GetY() / 3.0f, a.GetZ() + b.GetZ() + c.GetZ() / 3.0f);
		Vector3 viewDirection = camera->GetPosition() - triangleCenter;
		viewDirection = viewDirection.Normalize();
		float dotResult = Vector3::DotProduct(normalTriangle, viewDirection);
		//if (dotResult >= 0) continue;
		float deltaY = std::max({ a.GetY(), b.GetY(), c.GetY() }) - std::min({ a.GetY(), b.GetY(), c.GetY() });
		float deltaZ = std::max({ a.GetZ(), b.GetZ(), c.GetZ() }) - std::min({ a.GetZ(), b.GetZ(), c.GetZ() });
		float minX = 0;
		float maxX = 0;
		float deltaMax = 0;
		float deltaMin = 0;
		minX = std::min(std::min(a.GetX(), b.GetX()), c.GetX());
		maxX = std::max(std::max(a.GetX(), b.GetX()), c.GetX());
		if (deltaZ > deltaY) {
			deltaMin = std::min(std::min(a.GetZ(), b.GetZ()), c.GetZ());
			deltaMax = std::max(std::max(a.GetZ(), b.GetZ()), c.GetZ());
		}
		else {
			deltaMin = std::min(std::min(a.GetY(), b.GetY()), c.GetY());
			deltaMax = std::max(std::max(a.GetY(), b.GetY()), c.GetY());
		}


		Vector3 basis0 = c - a;
		Vector3 basis1 = b - a;
		float basis00 = Vector3::DotProduct(basis0, basis0);
		float basis01 = Vector3::DotProduct(basis0, basis1);
		float basis11 = Vector3::DotProduct(basis1, basis1);
		float inverse = 1.0f / ((basis00 * basis11) - (basis01 * basis01)); // Determinant, needed later
		float stepSize = 0.01f;
		float stepSizeZ = (deltaMax - deltaMin) / HEIGHT;
		Vector3 worldPoint;
		for (float x = minX; x <= maxX; x += stepSize) {
			for (float z = deltaMin; z <= deltaMax; z += stepSizeZ) {
				worldPoint = deltaZ > deltaY ? Vector3(x, b.GetY(), z) : Vector3(x, z, b.GetZ());
				Vector3 testPoint = worldPoint - a;
				float testDot02 = Vector3::DotProduct(basis0, testPoint);
				float testDot12 = Vector3::DotProduct(basis1, testPoint);
				float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
				float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
				float w = 1 - u - v;

				if (u >= epsilon && v >= epsilon && w >= epsilon) // Point is in the bounds
				{
					uint8_t red = u * 0xFF;
					uint8_t green = v * 0xFF;
					uint8_t blue = w * 0xFF;
					uint8_t alpha = 0xFF;
					uint32_t color = (alpha << 24) | (red << 16) | (green << 8) | blue;
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, actor.worldMatrix);
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

						int pixelToChange = screenYPos * WIDTH + screenXPos;
						pixels[pixelToChange] = color;
						

					}
				}
			}
		}
	}
}

void Window::DetermineTriangles(Actor& actor)
{
	Face bottom = actor.vertices[0];
	if (actor.vertices.size() < 2)
	{
		std::cout << "Missing top face from vertex build \n";
		return;
	}
	Face top = actor.vertices[1];
	for (int i = 0; i < bottom.size(); i++) // This will miss the last one to connect the shape
	{
		std::vector<Vector3> triangle;
		int nextIndex = (i + 1) % bottom.size();
		triangle.push_back(bottom[i]);
		triangle.push_back(top[i]);
		triangle.push_back(top[nextIndex]);
		actor.triangles.push_back(triangle);
	}


	for (int i = 0; i < bottom.size(); i++) {
		std::vector<Vector3> triangle;
		int nextIndex = (i + 1) % bottom.size();
		triangle.push_back(bottom[i]);
		triangle.push_back(bottom[nextIndex]);
		triangle.push_back(top[nextIndex]);
		actor.triangles.push_back(triangle);
	}


	std::vector<Vector3> reversedTop = { top[2], top[1], top[0] };
	std::vector<Vector3> reversedBottom = { top[2], top[1], top[0] };
	switch (bottom.size()) {
	case ShapeSides::Tri:
		actor.triangles.push_back(reversedBottom);
		actor.triangles.push_back(reversedTop);
		break;
	case ShapeSides::Square:
		actor.triangles.push_back(bottom);
		actor.triangles.push_back(top);

	default:

		break;
	}
}

void Window::RenderShapes(Scene sceneToRender)
{
	for (int objectToDisplay = 0; objectToDisplay < sceneToRender.size(); objectToDisplay++) {
		TakeShape(sceneToRender[objectToDisplay]);
	}
}

void Window::DrawLines(Vector3& from, Vector3& to, Matrix4& worldMatrix, uint32_t& color)
{
	Vector2 screenBeginingPoint = camera->WorldToScreenPixel(from, worldMatrix);
	Vector2 screenEndPosition = camera->WorldToScreenPixel(to, worldMatrix);
	float ratio = 1.0f / Vector2::Distance(screenBeginingPoint, screenEndPosition);
	float incrementRatio = ratio;
	while (true)
	{
		Vector3 pointBetween = Vector3::Lerp(from, to, ratio);
		ratio += incrementRatio;
		if (pointBetween == to || ratio >= 1) {

			PointToPixel(pointBetween, worldMatrix, color);

			break;
		}
		else {
			PointToPixel(pointBetween, worldMatrix, color);
		}
	}


}

void Window::TakeShape(Actor& actor)
{
	Face bottom = actor.vertices[0];
	HandleFace(bottom, actor.worldMatrix, actor.color);
	if (actor.isPlane) {

		DrawPlaneLines(actor);
		return;
	}
	Face top = actor.vertices[1];
	HandleFace(top, actor.worldMatrix, actor.color);
	Face connected;
	for (int i = 0; i < bottom.size(); i++) {
		DrawLines(bottom[i], top[i], actor.worldMatrix, actor.color);
	}
}

void Window::DrawPlaneLines(Actor& actor)
{
	Face planarLines = actor.vertices[1];
	int oppositeVertexHopefully = -1;
	for (int i = 0; i < planarLines.size(); i += 2) {

		DrawLines(planarLines[i], planarLines[i + 1], actor.worldMatrix, actor.color);
	}
}

void Window::HandleFace(Face faceToDraw, Matrix4& worldMatrix, uint32_t& color)
{
	for (int i = 0; i < faceToDraw.size() - 1; i++) {

		DrawLines(faceToDraw[i], faceToDraw[i + 1], worldMatrix, color);
	}
	DrawLines(faceToDraw[faceToDraw.size() - 1], faceToDraw[0], worldMatrix, color);
}




void Window::PointToPixel(Vector3& point, Matrix4& worldMatrix, uint32_t& color)
{
	// implement camera logic
	Vector2 screenCoords = camera->WorldToScreenPixel(point, worldMatrix);
	// implement 2d to 1d
	uint32_t xCoord = floor(screenCoords.GetX() + 0.5f);
	uint32_t yCoord = floor(screenCoords.GetY() + 0.5f);
	uint32_t screenPixel = yCoord * WIDTH + xCoord;
	if (xCoord >= 0 && xCoord < WIDTH && yCoord >= 0 && yCoord < HEIGHT) {
		uint32_t screenPixel = yCoord * WIDTH + xCoord;
		pixels[screenPixel] = color;

	}
	else {


	}
}

Matrix4 Window::CreateWorldMatrix(Vector3& desiredPosition)
{
	Matrix4 worldMatrix;
	worldMatrix = Matrix4::Translation(desiredPosition.GetX(), desiredPosition.GetY(), desiredPosition.GetZ());
	return worldMatrix;
}

unsigned int Window::LerpBlend(unsigned int frontColor, unsigned int backColor)
{
	uint32_t frontRed = ((frontColor >> 16) & 0xFF);
	uint32_t frontGreen = ((frontColor >> 8) & 0xFF);
	uint32_t frontBlue = ((frontColor >> 0) & 0xFF);
	float frontAlpha = ((frontColor >> 24) & 0xFF);

	uint32_t backRed = ((backColor >> 16) & 0xFF);
	uint32_t backGreen = ((backColor >> 8) & 0xFF);
	uint32_t backBlue = ((backColor >> 0) & 0xFF);
	uint32_t backAlpha = ((backColor >> 24) & 0xFF);



	frontAlpha /= 255.0f;
	backRed = (uint32_t)(frontRed * frontAlpha + backRed * (1 - frontAlpha));
	backGreen = (uint32_t)(frontGreen * frontAlpha + backGreen * (1 - frontAlpha));
	backBlue = (uint32_t)(frontBlue * frontAlpha + backBlue * (1 - frontAlpha));



	unsigned int color = (backAlpha << 24) | (backRed << 16) | (backGreen << 8) | backBlue;
	return color;
}
