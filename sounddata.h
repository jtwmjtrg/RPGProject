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
	static vector<SoundBuffer>	sb;				// �Z�J���_���o�b�t�@
	static map < int, int >		soundmanager;	// �Ǘ��ԍ�

public:
	static DirectSound directsound;		// �T�E���h�f�o�C�X

	SoundData();
	~SoundData();

	// ����ǂݍ���
	static int LoadSoundMem(TCHAR* filename);
	// �����Đ�����
	static void PlaySoundMem(int handle, bool isLoop = false);
	static void PlaySoundMem(int handle, double current_position, bool isLoop = false);
	// �����~����
	static void StopSoundMem(int handle);
	// �w�肵�������폜����
	static void DeleteSoundMem(int handle);
	// �ǂݍ��񂾉���S�폜����
	static void AllDeleteSoundMem();
};