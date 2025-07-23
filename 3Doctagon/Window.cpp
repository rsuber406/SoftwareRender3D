#include "Window.h"
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <functional>
#include "Helpers/StoneHenge_Texture.h"
#include "Helpers/StoneHenge.h"
#define NOMINMAX

#include <windows.h>

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
#elif RENDER_LAB == 3 // week 3 assignment
#define CAMERA_X_POS 0.0f
#define CAMERA_Y_POS 0.5f
#define CAMERA_Z_POS -1.0f
#define WIDTH 500
#define HEIGHT 600
#define	TOTAL_PIXEL WIDTH * HEIGHT
#define TEXTURE_WIDTH greendragon_width
#define TEXTURE_HEIGHT greendragon_height
#define TEXTURE_PIXELS greendragon_pixels
#elif RENDER_LAB == 4
#define CAMERA_X_POS 0.0f
#define CAMERA_Y_POS 1.0f
#define CAMERA_Z_POS -4.0f
#define WIDTH 800
#define HEIGHT 600
#define	TOTAL_PIXEL WIDTH * HEIGHT
#define TEXTURE_WIDTH StoneHenge_width
#define TEXTURE_HEIGHT StoneHenge_height
#define TEXTURE_PIXELS StoneHenge_pixels
#endif
#define FOV 90.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 10.0f
#define PI 3.14159f
Window::Window()
{
	keepAlive = RS_Initialize("Ryan Suber Programming Assignment 4", WIDTH, HEIGHT);
	//CreateRasterThreads();
	Vector3 cameraPos(CAMERA_X_POS, CAMERA_Y_POS, CAMERA_Z_POS);
	Vector3 targetPos(0, 0, 0);
	Vector3 up(0, 1, 0);
	int width = WIDTH;
	int height = HEIGHT;
	float aspect = (float)width / (float)height;
	camera = new Camera(cameraPos, targetPos, up, FOV, aspect, NEAR_PLANE, FAR_PLANE, WIDTH, HEIGHT);
	pixels = new unsigned int[TOTAL_PIXEL];
	depthBuffer = new float[TOTAL_PIXEL];
	directionLight.direction = Vector3(-0.577f, -0.577f, 0.577f);
	directionLight.color = 0xFFC0C0F0;
	pointLight.color = 0xFFFFFF00;
	pointLight.position = Vector3(-1, 0.5, 1);
	pointLight.radius = 1.0f;
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
	auto prevFrame = timeStamp;
	while (keepAlive)
	{
		timeCheck = std::chrono::high_resolution_clock::now();
		auto currentFrame = timeCheck;

		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeCheck - timeStamp);
		auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - prevFrame);
		if (elapsed > std::chrono::milliseconds(32))
		{
			if (!keepAlive) return;
			if (RENDER_LAB != 4) {

				ClearScreen();

				UpdateActors();
				//RasterScene();
				BetterRaster(objectsToRender[1]);
			}
			else {
				HandleInputControls(deltaTime);
				ClearScreen();
				AdjustPointLightRadius();
				UpdateStoneHengeScene();
			}
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
	if (depthBuffer) delete[] depthBuffer;
	RS_Shutdown();
}

void Window::HandleInputControls(std::chrono::milliseconds deltaTime)
{
	float cameraMovementSpeed = 0.01f;
	if (GetAsyncKeyState('E')) // rotate ccw
	{
		float moveAmount = cameraMovementSpeed;
		Vector3 position = camera->GetPosition();
		camera->SetPosition(position * -1);
		Matrix4 rotation = Matrix4::RotationY(moveAmount);
		Vector4 newPosition = rotation * position;
		camera->SetPosition(newPosition.ToVector3());
		camera->RebuildMatrices();
	}
	else if (GetAsyncKeyState('Q')) // rotate cw
	{
		float moveAmount = cameraMovementSpeed;
		Vector3 position = camera->GetPosition();
		camera->SetPosition(position * -1);
		Matrix4 rotation = Matrix4::RotationY(-moveAmount);
		Vector4 newPosition = rotation * position;
		camera->SetPosition(newPosition.ToVector3());
		camera->RebuildMatrices();
	}
	if (GetAsyncKeyState('S')) // pitch up
	{
		cameraPitch += cameraMovementSpeed;
		if (cameraPitch >= 0.52) {
			cameraPitch = 0.52;
			return;
		}
		float moveAmount = cameraMovementSpeed;
		Vector3 position = camera->GetPosition();
		camera->SetPosition(position * -1);
		Matrix4 rotation = Matrix4::RotationX(moveAmount);
		Vector4 newPosition = rotation * position;
		camera->SetPosition(newPosition.ToVector3());
		camera->RebuildMatrices();
	}
	else if (GetAsyncKeyState('W')) // pitch down
	{
		cameraPitch -= cameraMovementSpeed;
		if (cameraPitch <= -0.52f) {
			cameraPitch = -0.52f;
			return;
		}
		float moveAmount = cameraMovementSpeed;
		Vector3 position = camera->GetPosition();
		camera->SetPosition(position * -1);
		Matrix4 rotation = Matrix4::RotationX(-moveAmount);
		Vector4 newPosition = rotation * position;
		camera->SetPosition(newPosition.ToVector3());
		camera->RebuildMatrices();
	}
	else if (GetAsyncKeyState(VK_UP)) {
		float moveAmount = cameraMovementSpeed;
		Vector3 position = camera->GetPosition();
		camera->SetPosition(position * -1);
		Matrix4 translation = Matrix4::Translation(Vector3(0, 1, 0));
		camera->SetPosition((translation * position).ToVector3());


	}
}

void Window::ClearScreen()
{
	if (RENDER_LAB != 4) {

		for (uint32_t i = 0; i < TOTAL_PIXEL; i++) {
			pixels[i] = 0x0000000;
		}
	}
	else {
		for (uint32_t i = 0; i < TOTAL_PIXEL; i++) {
			pixels[i] = 0x000008B;
		}
	}
	for (uint32_t i = 0; i < TOTAL_PIXEL; i++) {
		depthBuffer[i] = 1000.0f;
	}
}

void Window::BuildLightColor()
{
	for (int i = 0; i < scene.size(); i++) {

		for (int j = 0; j < scene[i].normals.size(); j++) {
			float lightRatio = Saturate(0, 1, Vector3::DotProduct(directionLight.direction * -1, scene[i].normals[j]));
			lightRatio += 0.2f; // hardcoded ambient
			lightRatio = Saturate(0, 1, lightRatio);
			scene[i].lightColor.push_back(DetermineLightColor(lightRatio, directionLight.color));
		}
		if (scene[i].lightColor.size() > 0) {

			for (int j = 0; j < 2532; ) {
				std::vector<uint32_t> lightColor{ scene[i].lightColor[StoneHenge_indicies[j]],
					scene[i].lightColor[StoneHenge_indicies[j + 1]]
					,scene[i].lightColor[StoneHenge_indicies[j + 2]] };
				scene[i].triangleLightColor.push_back(lightColor);
				j += 3;
			}
		}

	}

}

uint32_t Window::DetermineLightColor(float ratio, uint32_t color)
{
	uint8_t alpha = (color >> 24) & 0xFF;
	uint8_t red = (color >> 16) & 0xFF;
	uint8_t green = (color >> 8) & 0xFF;
	uint8_t blue = color & 0xFF;
	red *= ratio;
	green *= ratio;
	blue *= ratio;
	alpha = 0xFF;
	uint32_t transformedColor = (alpha << 24) | (red << 16) | (green << 8) | blue;
	return transformedColor;
}

uint32_t Window::DetermineSceneColor(uint32_t lightColor, uint32_t textureColor)
{
	uint8_t alpha = (lightColor >> 24) & 0xFF;
	uint8_t red = (lightColor >> 16) & 0xFF;
	uint8_t green = (lightColor >> 8) & 0xFF;
	uint8_t blue = lightColor & 0xFF;
	float ratioRed = red / 255.0f;
	float ratioGreen = green / 255.0f;
	float ratioBlue = blue / 255.0f;
	uint8_t textAlpha = (textureColor >> 24) & 0xFF;
	uint8_t textRed = (textureColor >> 16) & 0xFF;
	uint8_t textGreen = (textureColor >> 8) & 0xFF;
	uint8_t textBlue = textureColor & 0xFF;
	textRed *= ratioRed;
	textGreen *= ratioGreen;
	textBlue *= ratioBlue;
	textAlpha = 0xFF;
	uint32_t finalColor = (textAlpha << 24) | (textRed << 16) | (textGreen << 8) | textBlue;

	return finalColor;
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

		ThreadRasterObject(threadData->triangle, threadData->worldMatrixRef, threadData->triangleIdx);
		threadData->triangle.clear();
		threadData->worldMatrixRef.clear();
		//threadData->windowRasterMutex->lock();
		*threadData->runThreadFlag &= ~(1 << threadData->threadId);
		//threadData->windowRasterMutex->unlock();
		threadData->signalNextFrame->notify_all();
	}
}

uint32_t Window::InterpolateLight(uint32_t color1, uint32_t color2, uint32_t color3, float u, float v, float w)
{
	uint8_t red1 = (color1 >> 16) & 0xFF;
	uint8_t red2 = (color2 >> 16) & 0xFF;
	uint8_t red3 = (color3 >> 16) & 0xFF;
	uint8_t green1 = (color1 >> 8) & 0xFF;
	uint8_t green2 = (color2 >> 8) & 0xFF;
	uint8_t green3 = (color3 >> 8) & 0xFF;
	uint8_t blue1 = color1 & 0xFF;
	uint8_t blue2 = color2 & 0xFF;
	uint8_t blue3 = color3 & 0xFF;
	uint8_t redFinal = red1 * u + red2 * v + red3 * w;
	uint8_t greenFinal = green1 * u + green2 * v + green3 * w;
	uint8_t blueFinal = blue1 * u + blue2 * v + blue3 * w;
	uint32_t interpolated = (0xFF << 24) | (redFinal << 16) | (greenFinal << 8) | blueFinal;

	return interpolated;
}

float Window::CalculatePointLight(Vector3& vertexPos, Vector3& vertexNorm, Vector3& lightPos)
{
	Vector3 vertexToLight = lightPos - vertexPos;
	float distance = vertexToLight.Magnitude();
	vertexToLight = vertexToLight.Normalize(); // pure direction with no distance
	float diffuse = Saturate(0, 1, Vector3::DotProduct(vertexToLight, vertexNorm));
	float radius = 0.2f;
	float attenuation = 1.0f / (1.0 + distance * distance / pointLight.radius);
	float pointLightValue = diffuse * attenuation;
	return pointLightValue;
}

void Window::AdjustPointLightRadius()
{
	if (shrinkPointLightRadius) {
		pointLight.radius -= 0.025f;
		if (pointLight.radius <= 0.01) {
			shrinkPointLightRadius = false;
		}
		pointLight.radius = Saturate(0.01, 1, pointLight.radius);
	}
	else {
		pointLight.radius += 0.025f;
		if (pointLight.radius > 1) shrinkPointLightRadius = true;
		pointLight.radius = Saturate(0.01, 1, pointLight.radius);
	}
}

uint32_t Window::CombineDirectionAndPointLight(uint32_t color1, uint32_t color2)
{
	uint16_t red1 = (color1 >> 16) & 0xFF;
	uint16_t red2 = (color2 >> 16) & 0xFF;
	uint16_t green1 = (color1 >> 8) & 0xFF;
	uint16_t green2 = (color2 >> 8) & 0xFF;
	uint16_t blue1 = color1 & 0xFF;
	uint16_t blue2 = color2 & 0xFF;
	uint8_t finalRed = Saturate(0, 255, red1 + red2);
	uint8_t finalGreen = Saturate(0, 255, green1 + green2);
	uint8_t finalBlue = Saturate(0, 255, blue1 + blue2);
	uint32_t finalColor = (0xFF << 24) | (finalRed << 16) | (finalGreen << 8) | finalBlue;
	return finalColor;
}

void Window::UpdateActors()
{
	Vector3 origin(0, 0, 0);
	for (int i = 0; i < objectsToRender.size(); i++) {

		if (objectsToRender[i].rotationModifier) {

			objectsToRender[i].rotationModifier += 1.0f;  // Add rotation speed

			Matrix4 moveToOrigin = Matrix4::Translation(objectsToRender[i].position * -1);
			Matrix4 rotation = Matrix4::RotationY(objectsToRender[i].rotationModifier * PI / 180.0f);
			Matrix4 moveToPosition = Matrix4::Translation(objectsToRender[i].position);

			objectsToRender[i].worldMatrix = moveToPosition * rotation * moveToOrigin;

			//objectsToRender[i].triangles.clear();

		}

	}
	if (objectsToRender.size())
		TakeShape(objectsToRender[0]);
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
	case 3:
		BuildWeekTwoLab();
		break;
	case 4:
		BuildWeekFourLab();
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
	cube.rotationModifier = 0.01f;
	//cube.vertices = createObjects.GeneratePoints(4, 0.25f, 0.5f, cube.position);
	cube.vertices = createObjects.GenerateSquare();
	cube.color = 0xFF00FF00;
	Actor octagon;
	octagon.position = Vector3(0, 0, -0.75f);
	octagon.rotationModifier = 0.00f;
	octagon.color = 0xFFFF0000;
	octagon.vertices = createObjects.GeneratePoints(8, 0.25f, 0.5f, octagon.position);
	Matrix4 moveToOrigin = Matrix4::Translation(cube.position * -1);
	//Matrix4 rotation = Matrix4::RotationZ(45 * PI / 180.0f);
	Matrix4 moveToPosition = Matrix4::Translation(cube.position);

	//cube.worldMatrix = moveToPosition * rotation * moveToOrigin;
	DetermineTriangles(cube);
	objectsToRender.push_back(plane);
	objectsToRender.push_back(cube);
	//objectsToRender.push_back(octagon);
	RenderShapes(objectsToRender);
	//RasterScene();
	BetterRaster(cube);
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

void Window::BuildWeekFourLab()
{
	SceneObject starfield;
	starfield.position = Vector3(0, 0, 0);
	starfield.positions = BuildStars();
	scene.push_back(starfield);

	for (int i = 0; i < starfield.positions.size(); i++) {
		PointToPixel(starfield.positions[i], starfield.worldMatrix, starfield.color);
	}
	SceneObject stoneHenge = BuildStoneHengeData();

	scene.push_back(starfield);
	scene.push_back(stoneHenge);
	BuildLightColor();
}

void Window::UpdateStoneHengeScene()
{
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i].triangles.size() == 0) {
			PaintStarfield(scene[i].positions, scene[i].worldMatrix, scene[i].color);
		}
		else BuildStoneHenge(scene[i]);
	}
}

void Window::BuildStoneHenge(SceneObject& sceneObj)
{

	BetterRaster(sceneObj);
}

void Window::PaintStarfield(Verticies verticies, Matrix4& worldMatrix, uint32_t color)
{
	for (int i = 0; i < verticies.size(); i++) {
		PointToPixel(verticies[i], worldMatrix, color);
	}
}

Verticies Window::BuildStars()
{
	// This sets up the creation of 3000 stars at a radius of 50
	float angle = (2 * PI) / 60;
	float sphereAngle = PI / 50;
	Verticies stars;
	float sphereRadius = 50;
	Vector3 position(0, 0, 0);
	for (int i = 0; i < 60; i++)
	{
		for (int j = 0; j < 50; j++) {
			float theta = i * angle;
			float phi = j * sphereAngle;
			float x = sin(phi) * cos(theta);
			float y = sin(theta) * sin(phi);
			float z = cos(phi);
			x += (rand() % 5);
			y += (rand() % 5);
			z += (rand() % 5);
			Vector3 star(x * sphereRadius, y * sphereRadius, z * sphereRadius);
			//star = star + radius + position;
			stars.push_back(star);
		}

	}
	return stars;
}

SceneObject Window::BuildStoneHengeData()
{
	SceneObject stoneHenge;

	// cannot build at the moment until I figure out how to translate their OBJ data to work with my types. Because saving data as 
	// floats would have been to easy
	for (int i = 0; i < 1457; i++) {
		Vector3 pointInfo(StoneHenge_data[i].pos[0], StoneHenge_data[i].pos[1], StoneHenge_data[i].pos[2]);
		pointInfo = pointInfo * 0.1f;
		stoneHenge.positions.push_back(pointInfo);

		Vector3 normalInfo(StoneHenge_data[i].nrm[0], StoneHenge_data[i].nrm[1], StoneHenge_data[i].nrm[2]);
		stoneHenge.normals.push_back(normalInfo);
	}

	// map the triangles

	for (int i = 0; i < 2532; ) {
		std::vector<Vector3> set = { stoneHenge.positions[StoneHenge_indicies[i]],
			stoneHenge.positions[StoneHenge_indicies[i + 1]],
			stoneHenge.positions[StoneHenge_indicies[i + 2]] };
		stoneHenge.triangles.push_back(set);
		Vector2 uvCoordA(StoneHenge_data[StoneHenge_indicies[i]].uvw[0], StoneHenge_data[StoneHenge_indicies[i]].uvw[1]);
		Vector2 uvCoordB(StoneHenge_data[StoneHenge_indicies[i + 1]].uvw[0], StoneHenge_data[StoneHenge_indicies[i + 1]].uvw[1]);
		Vector2 uvCoordC(StoneHenge_data[StoneHenge_indicies[i + 2]].uvw[0], StoneHenge_data[StoneHenge_indicies[i + 2]].uvw[1]);
		std::vector<Vector2> uvSet = { uvCoordA, uvCoordB, uvCoordC };
		stoneHenge.uvCoords.push_back(uvSet);
		Vector3 normalA(StoneHenge_data[StoneHenge_indicies[i]].nrm[0],
			StoneHenge_data[StoneHenge_indicies[i]].nrm[1],
			StoneHenge_data[StoneHenge_indicies[i]].nrm[2]);
		Vector3 normalB(StoneHenge_data[StoneHenge_indicies[i + 1]].nrm[0],
			StoneHenge_data[StoneHenge_indicies[i + 1]].nrm[1],
			StoneHenge_data[StoneHenge_indicies[i + 1]].nrm[2]);
		Vector3 normalC(StoneHenge_data[StoneHenge_indicies[i + 2]].nrm[0],
			StoneHenge_data[StoneHenge_indicies[i + 2]].nrm[1],
			StoneHenge_data[StoneHenge_indicies[i + 2]].nrm[2]);
		std::vector<Vector3> normalSet{ normalA, normalB, normalC };
		stoneHenge.triangleNormal.push_back(normalSet);
		i += 3;
	}
	return stoneHenge;
}

void Window::RasterScene()
{
	int threadCounter = 0;
	//for (int i = 0; i < objectsToRender.size(); i++) {
	//	for (int j = 0; j < objectsToRender[i].triangles.size(); j++) {
	//		if (!keepAlive) return;
	//		rasterThreads[threadCounter].triangle.push_back(objectsToRender[i].triangles[j]);
	//		rasterThreads[threadCounter].worldMatrixRef.push_back(objectsToRender[i].worldMatrix);
	//		rasterThreads[threadCounter].triangleIdx = threadCounter;
	//		threadFlags |= (1 << rasterThreads[threadCounter].threadId);
	//		threadCounter++;
	//		if (threadCounter >= MAX_NUMBER_RASTER_THREADS) threadCounter = 0;
	//	}
	//}
	//// This will work when I can properly store each item into vectors 
	//windowRasterCond.notify_all();
	//{
	//	std::unique_lock<std::mutex>lock(windowRasterMutex);
	//	signalNextFrame.wait(lock, [&] {return threadFlags == 0 || threadFlags == UINT64_MAX; });
	//}
	for (int i = 0; i < objectsToRender.size(); i++) {
		RasterObject(objectsToRender[i]);
	}
}

void Window::ThreadRasterObject(Triangle& triangle, std::vector<Matrix4>& worldMatrix, int triangleNum)
{

	float epsilon = 0.0001f;

	//float uCoordA = (triangleNum % 3) * 0.5;
	//float vCoordA = (triangleNum % 3) * 0.5;
	//float uCoordB = uCoordA + 0.5;
	//float vCoordB = vCoordA;
	//float uCoordC = uCoordA;
	//float vCoordC = vCoordA + 0.5;
	float uCoordA = 1.0;
	float vCoordA = 1.0;
	float uCoordB = 0.0;
	float vCoordB = 1.0;
	float uCoordC = 0.0;
	float vCoordC = 0.0;
	for (int i = 0; i < triangle.size(); i++) {
		Vector3 a = triangle[i][1];
		Vector3 b = triangle[i][0];
		Vector3 c = triangle[i][2];
		float deltaX = std::max({ a.GetX(), b.GetX(), c.GetX() }) - std::min({ a.GetX(), b.GetX(), c.GetX() });
		float deltaY = std::max({ a.GetY(), b.GetY(), c.GetY() }) - std::min({ a.GetY(), b.GetY(), c.GetY() });
		float deltaZ = std::max({ a.GetZ(), b.GetZ(), c.GetZ() }) - std::min({ a.GetZ(), b.GetZ(), c.GetZ() });
		float minX = 0;
		float maxX = 0;
		float deltaMax = 0;
		float deltaMin = 0;
		minX = std::min(std::min(a.GetX(), b.GetX()), c.GetX());
		maxX = std::max(std::max(a.GetX(), b.GetX()), c.GetX());
		float minY = std::min(std::min(a.GetY(), b.GetY()), c.GetY());
		float maxY = std::max(std::max(a.GetY(), b.GetY()), c.GetY());
		float minZ = std::min(std::min(a.GetZ(), b.GetZ()), c.GetZ());
		float maxZ = std::max(std::max(a.GetZ(), b.GetZ()), c.GetZ());
		if (deltaZ > deltaY && deltaX > epsilon) {
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
		float stepSize = 0.001f;
		float stepSizeZ = (deltaMax - deltaMin) / HEIGHT;
		Vector3 worldPoint;
		if (deltaX > epsilon) {

			for (float x = minX; x <= maxX; x += stepSize) {
				for (float z = deltaMin; z <= deltaMax; z += stepSizeZ) {
					worldPoint = deltaZ > deltaY ? Vector3(x, b.GetY(), z) : Vector3(x, z, b.GetZ());
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, worldMatrix[i]);
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					int pixelToChange = screenYPos * WIDTH + screenXPos;
					Vector4 transformedPoint = worldMatrix[i] * worldPoint;
					if (transformedPoint.GetY() > depthBuffer[pixelToChange]) continue;

					Vector3 testPoint = worldPoint - a;
					float testDot02 = Vector3::DotProduct(basis0, testPoint);
					float testDot12 = Vector3::DotProduct(basis1, testPoint);
					float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
					float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
					float w = 1 - u - v;

					if (u >= epsilon && v >= epsilon && w >= epsilon && u < 1 + epsilon && v < 1 + epsilon && w < 1 + epsilon) // Point is in the bounds
					{

						float finalU = u * uCoordA + v * uCoordB + w * uCoordC;
						float finalV = u * vCoordA + v * vCoordB + w * vCoordC;
						int uLocation = (int)((finalU)*greendragon_width);
						int vLocation = (int)((finalV)*greendragon_height);
						uLocation = std::max(0, std::min(uLocation, 511));
						vLocation = std::max(0, std::min(vLocation, 511));
						int textureIdx = vLocation * greendragon_width + uLocation;

						uint32_t color = ConvertColorType(greendragon_pixels[textureIdx]);
						if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

							if (pixelToChange > TOTAL_PIXEL) continue;



							if (transformedPoint.GetY() < depthBuffer[pixelToChange]) {
								depthBuffer[pixelToChange] = transformedPoint.GetY();
								pixels[pixelToChange] = color;
							}



						}
					}
				}
			}
		}
		else {
			for (float y = minY; y <= maxY; y += stepSize) {
				for (float z = minZ; z <= maxZ; z += stepSizeZ) {
					worldPoint = Vector3(b.GetX(), y, z);
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, worldMatrix[i]);
					Vector4 transformedPoint = worldMatrix[i] * worldPoint;
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					int pixelToChange = screenYPos * WIDTH + screenXPos;

					Vector3 testPoint = worldPoint - a;
					float testDot02 = Vector3::DotProduct(basis0, testPoint);
					float testDot12 = Vector3::DotProduct(basis1, testPoint);
					float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
					float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
					float w = 1 - u - v;

					if (u >= epsilon && v >= epsilon && w >= epsilon) // Point is in the bounds
					{
						float finalU = u * uCoordA + v * uCoordB + w * uCoordC;
						float finalV = u * vCoordA + v * vCoordB + w * vCoordC;
						int uLocation = (int)((finalU)*greendragon_width);
						int vLocation = (int)((finalV)*greendragon_height);
						uLocation = std::max(0, std::min(uLocation, 511));
						vLocation = std::max(0, std::min(vLocation, 511));
						int textureIdx = uLocation * greendragon_width + vLocation;
						//if (textureIdx > greendragon_height * greendragon_width) continue;
						uint32_t color = ConvertColorType(greendragon_pixels[textureIdx]);
						if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

							if (pixelToChange > TOTAL_PIXEL) continue;



							if (transformedPoint.GetY() < depthBuffer[pixelToChange]) {
								depthBuffer[pixelToChange] = transformedPoint.GetY();
								pixels[pixelToChange] = color;
							}



						}
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
		float uCoordA = 1.0;
		float vCoordA = 1.0;
		float uCoordB = 0.0;
		float vCoordB = 1.0;
		float uCoordC = 0.0;
		float vCoordC = 0.0;
		Vector3 a = actor.triangles[i][1];
		Vector3 b = actor.triangles[i][0];
		Vector3 c = actor.triangles[i][2];
		float deltaX = std::max({ a.GetX(), b.GetX(), c.GetX() }) - std::min({ a.GetX(), b.GetX(), c.GetX() });
		float deltaY = std::max({ a.GetY(), b.GetY(), c.GetY() }) - std::min({ a.GetY(), b.GetY(), c.GetY() });
		float deltaZ = std::max({ a.GetZ(), b.GetZ(), c.GetZ() }) - std::min({ a.GetZ(), b.GetZ(), c.GetZ() });
		float minX = 0;
		float maxX = 0;
		float deltaMax = 0;
		float deltaMin = 0;
		minX = std::min(std::min(a.GetX(), b.GetX()), c.GetX());
		maxX = std::max(std::max(a.GetX(), b.GetX()), c.GetX());
		float minY = std::min(std::min(a.GetY(), b.GetY()), c.GetY());
		float maxY = std::max(std::max(a.GetY(), b.GetY()), c.GetY());
		float minZ = std::min(std::min(a.GetZ(), b.GetZ()), c.GetZ());
		float maxZ = std::max(std::max(a.GetZ(), b.GetZ()), c.GetZ());
		if (deltaZ > deltaY && deltaX > epsilon) {
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
		float stepSize = 0.001f;
		float stepSizeZ = (deltaMax - deltaMin) / HEIGHT;
		Vector3 worldPoint;
		if (deltaX > epsilon) {


			for (float x = minX; x <= maxX; x += stepSize) {
				for (float z = deltaMin; z <= deltaMax; z += stepSizeZ) {
					worldPoint = deltaZ > deltaY ? Vector3(x, b.GetY(), z) : Vector3(x, z, b.GetZ()); // will only be one dependant on face
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, actor.worldMatrix);
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					int pixelToChange = screenYPos * WIDTH + screenXPos;
					Vector4 transformedPoint = actor.worldMatrix * worldPoint;
					if (transformedPoint.GetY() > depthBuffer[pixelToChange]) continue;

					Vector3 testPoint = worldPoint - a;
					float testDot02 = Vector3::DotProduct(basis0, testPoint);
					float testDot12 = Vector3::DotProduct(basis1, testPoint);
					float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
					float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
					float w = 1 - u - v;

					if (u >= epsilon && v >= epsilon && w >= epsilon && u < 1 + epsilon && v < 1 + epsilon && w < 1 + epsilon) // Point is in the bounds
					{

						float finalU = w * uCoordA + v * uCoordB + u * uCoordC;
						float finalV = w * vCoordA + v * vCoordB + u * vCoordC;
						int uLocation = (int)((finalU)*greendragon_width);
						int vLocation = (int)((finalV)*greendragon_height);
						uLocation = std::max(0, std::min(uLocation, 511));
						vLocation = std::max(0, std::min(vLocation, 511));
						int textureIdx = vLocation * greendragon_width + uLocation;

						uint32_t color = ConvertColorType(greendragon_pixels[textureIdx]);
						if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

							if (pixelToChange > TOTAL_PIXEL) continue;



							if (transformedPoint.GetY() < depthBuffer[pixelToChange]) {
								depthBuffer[pixelToChange] = transformedPoint.GetY();
								pixels[pixelToChange] = color;
							}



						}
					}
				}
			}
		}
		else {

			for (float y = minY; y <= maxY; y += stepSize) {
				for (float z = minZ; z <= maxZ; z += stepSizeZ) {
					worldPoint = Vector3(b.GetX(), y, z);
					Vector2 screenPoint = camera->WorldToScreenPixel(worldPoint, actor.worldMatrix);
					Vector4 transformedPoint = actor.worldMatrix * worldPoint;
					int screenXPos = floor(screenPoint.GetX() + 0.5f);
					int screenYPos = floor(screenPoint.GetY() + 0.5f);
					int pixelToChange = screenYPos * WIDTH + screenXPos;

					Vector3 testPoint = worldPoint - a;
					float testDot02 = Vector3::DotProduct(basis0, testPoint);
					float testDot12 = Vector3::DotProduct(basis1, testPoint);
					float u = (basis11 * testDot02 - basis01 * testDot12) * inverse;
					float v = (basis00 * testDot12 - basis01 * testDot02) * inverse;
					float w = 1 - u - v;

					if (u >= epsilon && v >= epsilon && w >= epsilon) // Point is in the bounds
					{
						float finalU = w * uCoordA + v * uCoordB + u * uCoordC;
						float finalV = w * vCoordA + v * vCoordB + u * vCoordC;
						int uLocation = (int)((finalU)*greendragon_width);
						int vLocation = (int)((finalV)*greendragon_height);
						uLocation = std::max(0, std::min(uLocation, 511));
						vLocation = std::max(0, std::min(vLocation, 511));
						int textureIdx = vLocation * greendragon_width + uLocation;
						if (textureIdx > greendragon_height * greendragon_width) continue;
						uint32_t color = ConvertColorType(greendragon_pixels[textureIdx]);
						if (screenXPos >= 0 && screenXPos < WIDTH && screenYPos >= 0 && screenYPos < HEIGHT) {

							if (pixelToChange > TOTAL_PIXEL) continue;



							if (transformedPoint.GetY() < depthBuffer[pixelToChange]) {
								depthBuffer[pixelToChange] = transformedPoint.GetY();
								pixels[pixelToChange] = color;
							}



						}
					}
				}
			}
		}
	}
}

void Window::BetterRaster(Actor& actor)
{

	for (int i = 0; i < actor.triangles.size(); i++) {
		Vector3 a = (actor.worldMatrix * actor.triangles[i][1]).ToVector3();
		Vector3 b = (actor.worldMatrix * actor.triangles[i][0]).ToVector3();
		Vector3 c = (actor.worldMatrix * actor.triangles[i][2]).ToVector3();
		Vector2 uvCoordA = actor.uvCoords[i][1];

		Vector2 uvCoordB = actor.uvCoords[i][0];

		Vector2 uvCoordC = actor.uvCoords[i][2];
		Matrix4 ident = Matrix4::Identity();
		Vector2 screenPointA = camera->WorldToScreenPixel(a, ident);
		Vector2 screenPointB = camera->WorldToScreenPixel(b, ident);
		Vector2 screenPointC = camera->WorldToScreenPixel(c, ident);
		int minX = (int)std::min(screenPointA.GetX(), std::min(screenPointB.GetX(), screenPointC.GetX()));
		int maxX = (int)std::max(screenPointA.GetX(), std::max(screenPointB.GetX(), screenPointC.GetX()));
		int minY = (int)std::min(screenPointA.GetY(), std::min(screenPointB.GetY(), screenPointC.GetY()));
		int maxY = (int)std::max(screenPointA.GetY(), std::max(screenPointB.GetY(), screenPointC.GetY()));

		for (int y = minY; y <= maxY; y++) {
			for (int x = minX; x <= maxX; x++) {

				float edge0 = (screenPointB.GetX() - screenPointA.GetX()) * (y - screenPointA.GetY()) - (screenPointB.GetY() - screenPointA.GetY()) * (x - screenPointA.GetX());
				float edge1 = (screenPointC.GetX() - screenPointB.GetX()) * (y - screenPointB.GetY()) - (screenPointC.GetY() - screenPointB.GetY()) * (x - screenPointB.GetX());
				float edge2 = (screenPointA.GetX() - screenPointC.GetX()) * (y - screenPointC.GetY()) - (screenPointA.GetY() - screenPointC.GetY()) * (x - screenPointC.GetX());
				if ((edge0 >= 0 && edge1 >= 0 && edge2 >= 0) || (edge0 <= 0 && edge1 <= 0 && edge2 <= 0)) {

					float area = edge0 + edge1 + edge2;
					float u = edge1 / area;
					float v = edge2 / area;
					float w = edge0 / area;


					float zDepth = u * a.GetZ() + v * b.GetZ() + w * c.GetZ();  // Y is my depth not Z

					float finalU = (u * uvCoordA.GetX() + v * uvCoordB.GetX() + w * uvCoordC.GetX());  // Keep U the same
					float finalV = (u * uvCoordA.GetY() + v * uvCoordB.GetY() + w * uvCoordC.GetY());
					int uLocation = (int)((finalU)*TEXTURE_WIDTH);
					int vLocation = (int)((finalV)*TEXTURE_HEIGHT);
					uLocation = std::max(0, std::min(uLocation, 511));
					vLocation = std::max(0, std::min(vLocation, 511));
					int textureLocation = vLocation * TEXTURE_WIDTH + uLocation;
					int pixelLocation = y * WIDTH + x;
					if (pixelLocation > TOTAL_PIXEL) continue;
					if (pixelLocation < 0) continue;
					if (zDepth < depthBuffer[pixelLocation]) {
						depthBuffer[pixelLocation] = zDepth;
						pixels[pixelLocation] = ConvertColorType(TEXTURE_PIXELS[textureLocation]);
					}

				}
			}
		}
	}
}

void Window::BetterRaster(SceneObject& sceneObj)
{
	for (int i = 0; i < sceneObj.triangles.size(); i++) {
		Vector3 a = (sceneObj.worldMatrix * sceneObj.triangles[i][1]).ToVector3();
		Vector3 b = (sceneObj.worldMatrix * sceneObj.triangles[i][0]).ToVector3();
		Vector3 c = (sceneObj.worldMatrix * sceneObj.triangles[i][2]).ToVector3();
		Vector2 uvCoordA = sceneObj.uvCoords[i][1];

		Vector2 uvCoordB = sceneObj.uvCoords[i][0];

		Vector2 uvCoordC = sceneObj.uvCoords[i][2];
		Matrix4 ident = Matrix4::Identity();
		float depthA = 0.0f;
		float depthB = 0.0f;
		float depthC = 0.0f;
		Vector2 screenPointA = camera->WorldToScreenPixel(a, ident, depthA);
		Vector2 screenPointB = camera->WorldToScreenPixel(b, ident, depthB);
		Vector2 screenPointC = camera->WorldToScreenPixel(c, ident, depthC);
		uint32_t lightColorA = sceneObj.triangleLightColor[i][1];
		uint32_t lightColorB = sceneObj.triangleLightColor[i][0];
		uint32_t lightColorC = sceneObj.triangleLightColor[i][2];
		Vector3 normA = sceneObj.triangleNormal[i][1];
		Vector3 normB = sceneObj.triangleNormal[i][0];
		Vector3 normC = sceneObj.triangleNormal[i][2];

		int minX = (int)std::min(screenPointA.GetX(), std::min(screenPointB.GetX(), screenPointC.GetX()));
		int maxX = (int)std::max(screenPointA.GetX(), std::max(screenPointB.GetX(), screenPointC.GetX()));
		int minY = (int)std::min(screenPointA.GetY(), std::min(screenPointB.GetY(), screenPointC.GetY()));
		int maxY = (int)std::max(screenPointA.GetY(), std::max(screenPointB.GetY(), screenPointC.GetY()));

		for (int y = minY; y <= maxY; y++) {
			for (int x = minX; x <= maxX; x++) {

				float edge0 = (screenPointB.GetX() - screenPointA.GetX()) * (y - screenPointA.GetY()) - (screenPointB.GetY() - screenPointA.GetY()) * (x - screenPointA.GetX());
				float edge1 = (screenPointC.GetX() - screenPointB.GetX()) * (y - screenPointB.GetY()) - (screenPointC.GetY() - screenPointB.GetY()) * (x - screenPointB.GetX());
				float edge2 = (screenPointA.GetX() - screenPointC.GetX()) * (y - screenPointC.GetY()) - (screenPointA.GetY() - screenPointC.GetY()) * (x - screenPointC.GetX());
				if ((edge0 >= 0 && edge1 >= 0 && edge2 >= 0) || (edge0 <= 0 && edge1 <= 0 && edge2 <= 0)) {

					float area = edge0 + edge1 + edge2;
					float u = edge1 / area;
					float v = edge2 / area;
					float w = edge0 / area;


					float zDepth = u * depthA + v * depthB + w * depthC;  // Z is now depth

					float finalU = (u * uvCoordA.GetX() + v * uvCoordB.GetX() + w * uvCoordC.GetX());  // Keep U the same
					float finalV = (u * uvCoordA.GetY() + v * uvCoordB.GetY() + w * uvCoordC.GetY());
					int uLocation = (int)((finalU)*TEXTURE_WIDTH);
					int vLocation = (int)((finalV)*TEXTURE_HEIGHT);
					uLocation = std::max(0, std::min(uLocation, (int)TEXTURE_WIDTH));
					vLocation = std::max(0, std::min(vLocation, (int)TEXTURE_HEIGHT));
					int textureLocation = vLocation * TEXTURE_WIDTH + uLocation;
					int pixelLocation = y * WIDTH + x;
					if (pixelLocation > TOTAL_PIXEL) continue;
					if (pixelLocation < 0) continue;
					if (zDepth < depthBuffer[pixelLocation])
					{
						depthBuffer[pixelLocation] = zDepth;
						uint32_t interpolatedColor = InterpolateLight(lightColorA, lightColorB, lightColorC, u, v, w);
						//pixels[pixelLocation] = ConvertColorType(TEXTURE_PIXELS[textureLocation]);
						uint32_t textureColor = ConvertColorType(TEXTURE_PIXELS[textureLocation]);
						float pointLight1 = CalculatePointLight(a, normA, pointLight.position);
						float pointLight2 = CalculatePointLight(b, normB, pointLight.position);
						float pointLight3 = CalculatePointLight(c, normC, pointLight.position);
						float interpolatePointLight = Saturate(0, 1, pointLight1 * u + pointLight2 * v + pointLight3 * w);
						uint32_t pointLightColor = DetermineLightColor(interpolatePointLight, pointLight.color);
						uint32_t combinedLightingColor = CombineDirectionAndPointLight(interpolatedColor, pointLightColor);
						textureColor = DetermineSceneColor(combinedLightingColor, textureColor);
						pixels[pixelLocation] = textureColor;
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
		if (i % 2 == 0) {

			float uCoordA = 1.0; // correct change otherside
			float vCoordA = 1.0;
			float uCoordB = 1.0;
			float vCoordB = 0.0;
			float uCoordC = 0.0;
			float vCoordC = 0.0;
			//float uCoordA = 1.0; // works but inverted
			//float vCoordA = 1.0;
			//float uCoordB = 1.0;
			//float vCoordB = 0.0;
			//float uCoordC = 0.0;
			//float vCoordC = 0.0;
			Vector2 coordA(uCoordA, vCoordA);
			Vector2 coordB(uCoordB, vCoordB);
			Vector2 coordC(uCoordC, vCoordC);
			actor.uvCoords.push_back(std::vector<Vector2>({ coordA, coordB, coordC }));
		}
		else {

			//float uCoordA = 0.0; // works but inverted
			//float vCoordA = 0.0;
			//float uCoordB = 0.0;
			//float vCoordB = 1.0;
			//float uCoordC = 1.0;
			//float vCoordC = 1.0;
			float uCoordA = 1.0; //good no more adjustment
			float vCoordA = 1.0;
			float uCoordB = 1.0;
			float vCoordB = 0.0;
			float uCoordC = 0.0;
			float vCoordC = 0.0;
			Vector2 coordA(uCoordA, vCoordA);
			Vector2 coordB(uCoordB, vCoordB);
			Vector2 coordC(uCoordC, vCoordC);
			actor.uvCoords.push_back(std::vector<Vector2>({ coordA, coordB, coordC }));
		}
	}


	for (int i = 0; i < bottom.size(); i++) {
		std::vector<Vector3> triangle;
		int nextIndex = (i + 1) % bottom.size();
		triangle.push_back(bottom[i]);
		triangle.push_back(top[nextIndex]);
		triangle.push_back(bottom[nextIndex]);
		actor.triangles.push_back(triangle);
		if (i % 2 == 0) {
			float uCoordA = 1.0;
			float vCoordA = 1.0;
			float uCoordB = 0.0;
			float vCoordB = 0.0;
			float uCoordC = 0.0;
			float vCoordC = 1.0;
			Vector2 coordA(uCoordA, vCoordA);
			Vector2 coordB(uCoordB, vCoordB);
			Vector2 coordC(uCoordC, vCoordC);
			actor.uvCoords.push_back(std::vector<Vector2>({ coordA, coordB, coordC }));
		}
		else {

			float uCoordA = 1.0; //good do not adjust anymore
			float vCoordA = 1.0;
			float uCoordB = 0.0;
			float vCoordB = 0.0;
			float uCoordC = 0.0;
			float vCoordC = 1.0;
			Vector2 coordA(uCoordA, vCoordA);
			Vector2 coordB(uCoordB, vCoordB);
			Vector2 coordC(uCoordC, vCoordC);
			actor.uvCoords.push_back(std::vector<Vector2>({ coordA, coordB, coordC }));
		}
	}


	std::vector<Vector3> reversedTop = { top[2], top[1], top[0] };
	std::vector<Vector3> reversedBottom = { bottom[2], bottom[1], bottom[0] };
	actor.triangles.push_back(reversedBottom);
	actor.triangles.push_back(reversedTop);
	Vector2 bottomA(0.0, 0.0), bottomB(1.0, 0.0), bottomC(0.0, 1.0);
	actor.uvCoords.push_back(std::vector<Vector2>({ bottomA, bottomB, bottomC }));

	Vector2 bottom2A(1.0, 0.0), bottom2B(1.0, 1.0), bottom2C(0.0, 1.0);
	actor.uvCoords.push_back(std::vector<Vector2>({ bottom2A, bottom2B, bottom2C }));
	std::vector<Vector3> secondBottom = { bottom[3], bottom[0], bottom[2] };
	std::vector<Vector3> secondTop;
	Vector2 topA(0.0, 0.0), topB(1.0, 0.0), topC(0.0, 1.0);
	actor.uvCoords.push_back(std::vector<Vector2>({ topA, topB, topC }));

	Vector2 top2A(1.0, 0.0), top2B(1.0, 1.0), top2C(0.0, 1.0);
	actor.uvCoords.push_back(std::vector<Vector2>({ top2A, top2B, top2C }));
	switch (bottom.size()) {
	case ShapeSides::Tri:
		actor.triangles.push_back(reversedBottom);
		actor.triangles.push_back(reversedTop);
		break;
	case ShapeSides::Square:
		secondTop = std::vector<Vector3>({ top[3], top[0], top[2] });
		secondBottom = std::vector<Vector3>({ bottom[3], bottom[0], bottom[2] });
		actor.triangles.push_back(secondBottom);
		actor.triangles.push_back(secondTop);

		break;
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
unsigned int Window::ConvertColorType(unsigned int color)
{
	unsigned int a = (color >> 24) & 0xFF;
	unsigned int b = (color >> 16) & 0xFF;
	unsigned int g = (color >> 8) & 0xFF;
	unsigned int r = (color >> 0) & 0xFF;

	unsigned int converted = (r << 24) | (g << 16) | (b << 8) | a;


	return converted;
}

float Window::Saturate(float min, float max, float input)
{
	if (input < min) input = min;
	if (input > max) input = max;
	return input;
}
