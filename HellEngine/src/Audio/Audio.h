#pragma once
#include "soloud/soloud.h"
#include "soloud/soloud_wav.h"
#include <vector>

namespace HellEngine
{
	struct AudioFile {
		const char* name;
		SoLoud::Wav* audio;
	};

	class Audio
	{
	public: // functions
		static void Init();
		static void LoadAudio(const char* name);
		static void PlayAudio(const char* name, float volume = 1.0f);
		static void LoopAudio(const char* name, float volume = 1.0f);
		static void Terminate();
	
	public: // variables
		static SoLoud::Soloud gSoloud;
		static std::vector<AudioFile> s_loadedAudio;
	};
}