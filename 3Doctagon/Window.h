#pragma once
#include "./Helpers/RasterSurface.h"
#include <vector>
#include "Shape.h"
#include "Camera.h"
#include <thread>
#include <mutex>
#include "Helpers/greendragon.h"
#include <condition_variable>
#include "Helpers/greendragon.h"
#include <chrono>

#define RENDER_LAB 4 // zero is my trial version of all of this
#define MAX_NUMBER_RASTER_THREADS 15
typedef std::vector<std::vector<Vector3>> StoredShape;
typedef std::vector<std::vector<Vector3>> Triangle;
typedef std::vector <std::vector<Vector2>> UVCoords;
typedef std::vector<Vector3> Verticies;
typedef std::vector<Vector3> SurfaceNormals;
typedef std::vector<uint32_t> LightColors;
typedef std::vector<std::vector<Vector3>> TriangleNormals;
typedef std::vector<std::vector<uint32_t>> TriangleLightColor;

/// <summary>
/// This works well for labs 2 - 3 does not work for lab 4. Need altered approach
/// </summary>
struct Actor 
{
	StoredShape vertices;
	Triangle triangles;
	UVCoords uvCoords;
	Matrix4 worldMatrix;
	Vector3 position;
	bool rotate = false;
	bool isPlane = false;
	float rotationModifier = 0.00f;
	uint32_t color = 0xFFFFFFFF;
};
typedef std::vector<Vector3> Face;
typedef std::vector<Actor> Scene;

struct UVMap {
	float u;
	float v;
	float w;
};
typedef std::vector<UVMap> MappedUV;

struct SceneObject {
	Verticies positions;
	uint32_t color = 0xFFFFFFFF;
	Vector3 position;
	Matrix4 worldMatrix;
	UVCoords uvCoords;
	Triangle triangles;
	SurfaceNormals normals;
	LightColors lightColor;
	LightColors triangleColor;
	TriangleNormals triangleNormal;
	TriangleLightColor triangleLightColor;
};
typedef std::vector<SceneObject>StoneHengeScene;

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
	int triangleIdx;
};
struct PointLight {
	Vector3 position;
	uint32_t color;
};
struct DirectionalLight {
	Vector3 direction;
	uint32_t color;
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
	void HandleInputControls(std::chrono::milliseconds deltaTime);
	void ClearScreen();
	void BuildLightColor();
	uint32_t DetermineLightColor(float ratio, uint32_t color);
	uint32_t DetermineSceneColor(uint32_t lightColor, uint32_t textureColor);
	void CreateRasterThreads();
	void ThreadEntryPoint(ThreadData* threadData);
	void RasterThreadLivingPoint(ThreadData* threadData);
	uint32_t InterpolateLight(uint32_t color1, uint32_t color2, uint32_t color3, float u, float v, float w);
	uint32_t CalculatePointLight(Vector3& vertexPos, Vector3& vertexNorm, Vector3& lightPos);
	void UpdateActors();
	void BuildScene();
	void RenderOctagon();
	void BuildWeekTwoLab();
	void BuildWeekTwoOptional();
	void BuildWeekFourLab();
	void UpdateStoneHengeScene();
	void BuildStoneHenge(SceneObject& sceneObj);
	void PaintStarfield(Verticies verticies, Matrix4& worldMatrix, uint32_t color);
	Verticies BuildStars();
	SceneObject BuildStoneHengeData();
	void RasterScene();
	unsigned int ConvertColorType(unsigned int color);
	float Saturate(float min, float max, float input);
	/// <summary>
	/// This also needs deleted - only keeping so you can see the pain of going the wrong path
	/// </summary>
	/// <param name="triangle"></param>
	/// <param name="worldMatrix"></param>
	/// <param name="triangleNum"></param>
	void ThreadRasterObject(Triangle& triangle, std::vector<Matrix4>& worldMatrix, int triangleNum);
	/// <summary>
	/// This also needs deleted - only keeping so you can see the pain of going the wrong path
	/// </summary>
	/// <param name="actor"></param>
	void RasterObject(Actor& actor);
	// Good raster method
	void BetterRaster(Actor& actor);
	void BetterRaster(SceneObject& sceneObj);
	/// <summary>
	/// This could be made better but I dont have the time to rebuild this
	/// </summary>
	/// <param name="actor"></param>
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
	StoneHengeScene scene;
	std::chrono::milliseconds deltaTime;
	DirectionalLight directionLight;
	PointLight pointLight;
};

