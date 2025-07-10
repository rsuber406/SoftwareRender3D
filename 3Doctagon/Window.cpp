#include "Window.h"
#include <cstdint>
#include <chrono>
#define RENDER_LAB 1// zero is my trial version of all of this
#if RENDER_LAB == 0
#define CAMERA_X_POS 0.00f
#define CAMERA_Y_POS 70.0f
#define CAMERA_Z_POS 5.0f
#elif RENDER_LAB == 1
#define CAMERA_X_POS 0.0f
#define CAMERA_Y_POS 1.13f
#define CAMERA_Z_POS 0.35f
#endif
#define FOV 90.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 10.0f
#define PI 3.14159f
Window::Window()
{

	keepAlive = RS_Initialize("Ryan Suber 3D Oct", WIDTH, HEIGHT);
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
			ClearScreen();
			UpdateActors();
			timeStamp = timeCheck;
		}
		RS_Update(pixels, TOTAL_PIXEL);
	}
}

void Window::KillWindow()
{
	keepAlive = false;
}

Window::~Window()
{
	if (pixels) delete[]pixels;
	if (camera) delete camera;
	RS_Shutdown();
}

void Window::ClearScreen()
{
	for (uint32_t i = 0; i < TOTAL_PIXEL; i++) {
		pixels[i] = 0x00000000;
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
	Vector3 cubePos(0, 0, 0);
	Actor plane;
	Shape createObjects;
	plane.position = planePos;
	plane.rotationModifier = 0.00f;
	plane.vertices = createObjects.GeneratePoints(4, 1, 0, plane.position, true);
	Actor cube;
	cube.position = cubePos;
	cube.rotationModifier = 0.001f;
	cube.vertices = createObjects.GeneratePoints(4, 0.5f, 0.5f, cube.position);
	cube.color = 0xFF00FF00;
	objectsToRender.push_back(plane);
	objectsToRender.push_back(cube);
	Matrix4 moveToOrigin = Matrix4::Translation(objectsToRender[0].position * -1);
	Matrix4 rotation = Matrix4::RotationZ(45.0f * PI / 180.0f);
	Matrix4 moveToPosition = Matrix4::Translation(objectsToRender[0].position);

	objectsToRender[0].worldMatrix = moveToPosition * rotation * moveToOrigin;
	RenderShapes(objectsToRender);
	RS_Update(pixels, TOTAL_PIXEL);
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
	if (actor.vertices.size() == 1) return;
	Face top = actor.vertices[1];
	HandleFace(top, actor.worldMatrix, actor.color);
	Face connected;
	for (int i = 0; i < bottom.size(); i++) {
		DrawLines(bottom[i], top[i], actor.worldMatrix, actor.color);
	}
}

void Window::HandleFace(Face faceToDraw, Matrix4& worldMatrix, uint32_t& color)
{
	for (int i = 0; i < faceToDraw.size() - 1; i++) {

		DrawLines(faceToDraw[i], faceToDraw[i + 1], worldMatrix, color);
	}
	DrawLines(faceToDraw[faceToDraw.size() - 1], faceToDraw[0], worldMatrix, color);
}




void Window::PointToPixel(Vector3& point, Matrix4& worldMatrix, uint32_t & color)
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
