#pragma once
#include <vector>
#include <map>
#include <random>

#include "wave.h"
#include "soundbuffer.h"

using namespace std;

// define
#define directsound			SoundData::directsound
#define LoadSoundMem		SoundData::LoadSoundMem
#define PlaySoundMem		SoundData::PlaySoundMem
#define StopSoundMem		SoundData::StopSoundMem
#define DeleteSoundMem		SoundData::DeleteSoundMem
#define AllDeleteSoundMem	SoundData::AllDeleteSoundMem

class SoundData
{
private:
	static vector<SoundBuffer>	sb;				// セカンダリバッファ
	static map < int, int >		soundmanager;	// 管理番号

public:
	static DirectSound directsound;		// サウンドデバイス

	SoundData();
	~SoundData();

	// 音を読み込む
	static int LoadSoundMem(TCHAR* filename);
	// 音を再生する
	static void PlaySoundMem(int handle, bool isLoop = false);
	static void PlaySoundMem(int handle, double current_position, bool isLoop = false);
	// 音を停止する
	static void StopSoundMem(int handle);
	// 指定した音を削除する
	static void DeleteSoundMem(int handle);
	// 読み込んだ音を全削除する
	static void AllDeleteSoundMem();
};