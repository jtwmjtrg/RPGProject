#include "sounddata.h"

DirectSound				SoundData::directsound;
vector<SoundBuffer>		SoundData::sb;
map < int, int >		SoundData::soundmanager;

SoundData::SoundData() {}
SoundData::~SoundData() {}

int SoundData::LoadSoundMem(TCHAR* filename)
{
	int num = 0;	// 同期用

	Wave wave;

	wave.Load(filename);

	// データ保存
	if (sb.empty())
	{
		// 初めてサウンドが読み込まれるとき
		sb.emplace_back();	// メモリ確保
		if (!(sb.back().Create(directsound.pDirectSound8, wave.WaveFormat, wave.WaveData, wave.DataSize)))
		{
			// ファイルがない
			sb.erase(sb.begin());
			return 0;
		}

		num = 0;
	}
	else
	{
		
		sb.emplace_back();	// メモリ確保
		if (!(sb.back().Create(directsound.pDirectSound8, wave.WaveFormat, wave.WaveData, wave.DataSize)))
		{
			// ファイルがない
			sb.erase(sb.begin()- (sb.size() - 1));
			return 0;
		}

		num = sb.size() - 1;
	}

	random_device				rnd;							// 非決定的な乱数生成器を生成
	mt19937						mt(rnd());						//  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<>	rand_integer(0, 1000000000);	// 乱数範囲指定
	int							r		= rand_integer(mt);		// 保存
	auto						itr		= soundmanager.find(r);	// イテレータ生成

	// かぶり検索
	while (itr != soundmanager.end())
	{
		r	= rand_integer(mt);	// 乱数生成
		itr = soundmanager.find(r);	// イテレータ
	}

	// 管理番号確保
	soundmanager.insert(make_pair(r, num));

	// 管理番号を返す
	return r;

	// 管理番号を返す
	return (soundmanager.size() - 1);
}

void SoundData::PlaySoundMem(int handle, bool isLoop)
{
	// 再生
	sb[soundmanager[handle]].Play(isLoop);
}
void SoundData::PlaySoundMem(int handle, double current_position, bool isLoop)
{
	// 再生
	DSBCAPS dsbcaps;

	dsbcaps.dwSize = sizeof(DSBCAPS);

	sb[soundmanager[handle]].pSecondaryBuffer->GetCaps(&dsbcaps);

	sb[soundmanager[handle]].pSecondaryBuffer->SetCurrentPosition((DWORD)((double)dsbcaps.dwBufferBytes*current_position) - 1);

	sb[soundmanager[handle]].Play(isLoop);
}

void SoundData::StopSoundMem(int handle)
{
	// 停止
	sb[soundmanager[handle]].Stop();
}

void SoundData::DeleteSoundMem(int handle)
{
	sb[soundmanager[handle]].Delete();
	sb[soundmanager[handle]].pSecondaryBuffer = NULL;

	// 探索
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

