#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"

#include "Core/keycodes.h"
#include "fbxsdk.h"

#include "Renderer/Shader.h"
#include "Renderer/Transform.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

#define INVALID_VALUE		0xFFFFFFFF
#define ROTATE_0			(float)0

#define HELL_PI				(float)3.14159265358979323846
#define ROTATE_0			(float)0
#define ROTATE_180			(float)HELL_PI
#define ROTATE_90			(float)HELL_PI * 0.5
#define ROTATE_270			(float)HELL_PI * 1.5

#define GRAVITY				(float)-0.981

#define GROUND_FLOOR		0
#define SECOND_FLOOR		1
#define BASEMENT			-1

#define DEBUG_COLOR_DOOR btVector3(0.4f, 0.5f, 0.5f)
//#define DEBUG_COLOR_WALL btVector3(0.4f, 0.3f, 0.3f)
#define DEBUG_COLOR_WALL btVector3(0.6f, 0.5f, 0.5f)
#define DEBUG_COLOR_RAMP btVector3(0.9f, 0.8f, 0.8f)
#define DEBUG_COLOR_YELLOW btVector3(1.0f, 1.0f, 0.0f)
//#define DEBUG_COLOR_GROUND btVector3(1.0f, 1.0f, 0.0f)
//#define DEBUG_COLOR_YELLOW btVector3(0.2f, 0.2f, 0.2f)
#define DEBUG_COLOR_GROUND btVector3(0.2f, 0.2f, 0.2f)
#define DEBUG_COLOR_STATIC_ENTITY btVector3(1, 1, 0)

#define SMALL_NUMBER		(float)9.99999993922529e-9
#define KINDA_SMALL_NUMBER	(float)0.00001

// from old engine
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ToRadian(x) (float)(((x) * HELL_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / HELL_PI))

// Game constants
#define STORY_HEIGHT	2.6f
#define ROOM_HEIGHT		2.4f
#define DOOR_WIDTH		1.0f
#define DOOR_HEIGHT		2.0f	
#define FLOOR_TRIM_HEIGHT	0.091f
#define CEILING_TRIM_HEIGHT	0.082f

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720
#define NEAR_PLANE 0.01f
#define FAR_PLANE 50.0f

#define CheckGLError() CheckGLError_(__FILE__, __LINE__)

struct Opp {
	int m_initialAmount;
	int m_amount;

	Opp(int initialAmount)
	{
		m_initialAmount = initialAmount;
	}

	void Deduct(int value)
	{
		m_amount -= value;
	}
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	unsigned int BlendingIndex[4];
	glm::vec4 BlendingWeight;

	bool operator==(const Vertex& other) const {
		return Position == other.Position && Normal == other.Normal && TexCoords == other.TexCoords;
	}
};

struct Vertex2D {
	glm::vec2 Position;
	glm::vec2 TexCoords;
};

struct RenderSettings {
	bool DrawWeapon;
	bool BindMaterials;
	bool DrawLightBulbs;
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.TexCoords) << 1);
		}
	};
}

struct Line {
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 color;
};

struct EntityData {
	char* name = "NONE";
	int vectorIndex = -1;
};

struct Animation
{
	fbxsdk::FbxTime  m_startTime;
	fbxsdk::FbxTime  m_endTime;
	fbxsdk::FbxTime  m_currentTime;
	fbxsdk::FbxScene* m_scene = nullptr;
	bool m_finishedPlaying = false;
};

enum FileType {FBX, OBJ, PNG, JPG, UNKNOWN};
enum Axis { POS_X, NEG_X, POS_Z, NEG_Z };
enum PlayerMovementState { STOPPED, WALKING, RUNNING };
enum RunningAnimationState { STARTING, LOOPING, ENDING, NOT_RUNNING};

enum GunState { FIRING, IDLE, RELOADING};
enum ReloadState { FROM_IDLE, SINGLE_RELOAD, DOUBLE_RELOAD, BACK_TO_IDLE, NOT_RELOADING };

enum DoorWayType { DOOR, STAIRCASE_OPENING };

struct DoorWay {
	DoorWayType type;
	glm::vec3 position;
	Axis axis;
	int story;
};

