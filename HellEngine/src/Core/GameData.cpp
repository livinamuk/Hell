#include "hellpch.h"
#include "GameData.h"
#include "Core/File.h"

namespace HellEngine
{
	House* GameData::p_house;
	House GameData::s_house;

	void GameData::LoadHouse(const char* filename)
	{
		s_house = File::LoadMap("Map.txt");
		p_house = &s_house;
		p_house->RebuildAll();

		// You are storing a pointer to the current map because you want to be able to 
		// have many maps stored in memory at once and this will allow for easy switching.
	}
}	
