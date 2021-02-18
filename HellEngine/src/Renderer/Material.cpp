#include "hellpch.h"
#include "Material.h"
#include "Renderer/Texture.h"
#include <Helpers/Util.h> 
#include <Helpers/AssetManager.h> 

namespace HellEngine
{
	Material::Material(std::string name)
	{
		this->name = name;
		this->FindTextures();
	}

	void Material::FindTextures()
	{
		this->ALB = AssetManager::GetTexIDByName("White");
		this->NRM = AssetManager::GetTexIDByName("Normal");
		this->RMA = AssetManager::GetTexIDByName("Black");

		unsigned int ALB = AssetManager::GetTexIDByName(name + "_ALB");
		unsigned int NRM = AssetManager::GetTexIDByName(name + "_NRM");
		unsigned int RMA = AssetManager::GetTexIDByName(name + "_RMA");
		unsigned int RME = AssetManager::GetTexIDByName(name + "_RME");		

		if (ALB != 0)	this->ALB = ALB;
		if (NRM != 0)	this->NRM = NRM;
		if (RMA != 0)	this->RMA = RMA;
		if (RME != 0)	this->RME = RME;

		if (RME != 0) {
			std::cout << "OMGGG " << name << "HAS EMISSVE " << RME << "\n";
		}
	}
}