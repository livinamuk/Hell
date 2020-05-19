#include "hellpch.h"
#include "ShotgunLogic.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	GunState ShotgunLogic::s_fireState;

	void ShotgunLogic::FireShotgun()
	{
		s_fireState = GunState::FIRING;
	}
}