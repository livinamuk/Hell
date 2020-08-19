#pragma once
#include "Header.h"
#include "House/House.h"

namespace HellEngine
{
	class GameData
	{
	public: // functions
		static void LoadHouse(const char* filename);

	public: // variables
		static House* p_house;

	private:
		static House s_house;
	};
}