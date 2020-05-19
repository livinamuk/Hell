#pragma once
#include <vector>

namespace HellEngine
{
	class Material
	{
	public: // methods
		Material(std::string name);
		void FindTextures();

	public: // fields
		std::string name;
		unsigned int ALB = 0;
		unsigned int NRM = 0;
		unsigned int RMA = 0;
		unsigned int RME = 0;
	};
}