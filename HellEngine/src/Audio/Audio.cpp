#include "hellpch.h"
#include "Audio.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	std::vector<AudioFile> Audio::s_loadedAudio;
	SoLoud::Soloud Audio::gSoloud;

	void Audio::Init()
	{
		gSoloud.init(); 

		Audio::LoadAudio("Shotgun_Fire_01.wav");
		Audio::LoadAudio("Door_Open.wav");		
		Audio::LoadAudio("player_step_1.wav");
		Audio::LoadAudio("player_step_2.wav");
		Audio::LoadAudio("player_step_3.wav");
		Audio::LoadAudio("player_step_4.wav");
		Audio::LoadAudio("Impact_Flesh.wav");
		Audio::LoadAudio("Empty.wav");
		Audio::LoadAudio("Shotgun_Reload_01.wav");
		Audio::LoadAudio("Shotgun_Reload_05.wav");
		Audio::LoadAudio("Shotgun_ReloadStart.wav");
		Audio::LoadAudio("Shotgun_ReloadEnd.wav");
		Audio::LoadAudio("ShellFloorBounce.wav");
		Audio::LoadAudio("ShellWallBounce.wav");
		Audio::LoadAudio("Glock_Fire_0.wav");
		Audio::LoadAudio("Glock_Fire_1.wav");
		Audio::LoadAudio("Glock_Fire_2.wav");
		Audio::LoadAudio("Glock_DryFire.wav");;
		Audio::LoadAudio("Glock_Reload.wav");
		Audio::LoadAudio("Glock_ReloadFromEmpty.wav");
		Audio::LoadAudio("Glock_DryFire.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_01.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_02.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_03.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_04.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_05.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_06.wav");
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_07.wav"); 
		Audio::LoadAudio("FLY_Bullet_Impact_Flesh_08.wav");
		Audio::LoadAudio("WPNFLY_Glock_MagIn_Full.wav");
		Audio::LoadAudio("BuolletCasingBounce.wav");
		Audio::LoadAudio("FLY_Head_Explode_01.wav");
		Audio::LoadAudio("GlassImpact.wav");
		
		//Audio::LoadAudio("Music.wav");
	}

	void Audio::LoadAudio(const char* name)
	{
		std::string fullpath = "res/audio/";
		fullpath += name;

		AudioFile audioFile;
		audioFile.name = name;
		audioFile.audio = new SoLoud::Wav();
		audioFile.audio->load(fullpath.c_str());
		s_loadedAudio.push_back(audioFile);
	}

	void Audio::PlayAudio(const char* name, float volume)
	{
		for (AudioFile audioFile : s_loadedAudio)
		{
			if (std::strcmp(audioFile.name, name) == 0) {
				gSoloud.play(*audioFile.audio, volume);
				return;
			}
		}
		std::cout << "AUDIO NOT FOUND: " << name << "\n";
	}

	void Audio::LoopAudio(const char* name, float volume)
	{
		for (AudioFile& audioFile : s_loadedAudio)
		{
			if (std::strcmp(audioFile.name, name) == 0) {
				gSoloud.play(*audioFile.audio, volume);
				audioFile.audio->setLooping(true);
				return;
			}
		}
		std::cout << "AUDIO NOT FOUND: " << name << "\n";
	}

	void Audio::Terminate()
	{
		gSoloud.deinit();
	}
}
