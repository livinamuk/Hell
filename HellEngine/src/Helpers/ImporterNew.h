#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "fbxsdk.h"

namespace HellEngine
{
	class ImporterNew
	{
	public: // Fucntions
	//	static void Init();
	//	static void Terminate();

		static void LoadFbxModel(Model* model);
	};
}