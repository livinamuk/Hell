#pragma once
#include "Header.h"
#include "House/Room.h"

namespace HellEngine
{
	class ShotgunLogic
	{
	public: // static functions
		
		static void FireShotgun();

	public:	// static variables
		static GunState s_fireState;
	};
}