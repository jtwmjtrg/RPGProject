#include "sounddata.h"

DirectSound				SoundData::directsound;
vector<SoundBuffer>		SoundData::sb;
map < int, int >		SoundData::soundmanager;

SoundData::SoundData() {}
SoundData::~SoundData() {}

int SoundData::LoadSoundMem(TCHAR* filename)
{
	int num = 0;	// �����p

	Wave wave;

	wave.Load(filename);

	// �f�[�^�ۑ�
	if (sb.empty())
	{
		// ���߂ăT�E���h���ǂݍ��܂��Ƃ�
		sb.emplace_back();	// �������m��
		if (!(sb.back().Create(directsound.pDirectSound8, wave.WaveFormat, wave.WaveData, wave.DataSize)))
		{
			// �t�@�C�����Ȃ�
			sb.erase(sb.begin());
			return 0;
		}

		num = 0;
	}
	else
	{
		
		sb.emplace_back();	// �������m��
		if (!(sb.back().Create(directsound.pDirectSound8, wave.WaveFormat, wave.WaveData, wave.DataSize)))
		{
			// �t�@�C�����Ȃ�
			sb.erase(sb.begin()- (sb.size() - 1));
			return 0;
		}

		num = sb.size() - 1;
	}

	random_device				rnd;							// �񌈒�I�ȗ���������𐶐�
	mt19937						mt(rnd());						//  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	uniform_int_distribution<>	rand_integer(0, 1000000000);	// �����͈͎w��
	int							r		= rand_integer(mt);		// �ۑ�
	auto						itr		= soundmanager.find(r);	// �C�e���[�^����

	// ���Ԃ茟��
	while (itr != soundmanager.end())
	{
		r	= rand_integer(mt);	// ��������
		itr = soundmanager.find(r);	// �C�e���[�^
	}

	// �Ǘ��ԍ��m��
	soundmanager.insert(make_pair(r, num));

	// �Ǘ��ԍ���Ԃ�
	return r;

	// �Ǘ��ԍ���Ԃ�
	return (soundmanager.size() - 1);
}

void SoundData::PlaySoundMem(int handle, bool isLoop)
{
	// �Đ�
	sb[soundmanager[handle]].Play(isLoop);
}
void SoundData::PlaySoundMem(int handle, double current_position, bool isLoop)
{
	// �Đ�
	DSBCAPS dsbcaps;

	dsbcaps.dwSize = sizeof(DSBCAPS);

	sb[soundmanager[handle]].pSecondaryBuffer->GetCaps(&dsbcaps);

	sb[soundmanager[handle]].pSecondaryBuffer->SetCurrentPosition((DWORD)((double)dsbcaps.dwBufferBytes*current_position) - 1);

	sb[soundmanager[handle]].Play(isLoop);
}

void SoundData::StopSoundMem(int handle)
{
	// ��~
	sb[soundmanager[handle]].Stop();
}

void SoundData::DeleteSoundMem(int handle)
{
	sb[soundmanager[handle]].Delete();
	sb[soundmanager[handle]].pSecondaryBuffer = NULL;

	// �T��
	auto itr = soundmanager.find(handle);
	if (itr != soundmanager.end())
	{
		soundmanager.erase(itr);
	}
}

void SoundData::AllDeleteSoundMem()
{
	for (auto itr = soundmanager.begin(); itr != soundmanager.end(); ++itr) {
		DeleteSoundMem(itr->second);
	}
}

