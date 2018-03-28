#pragma once
#include "directsound.h"

class SoundBuffer
{
public:
	IDirectSoundBuffer8* pSecondaryBuffer;	// セカンダリバッファ

	SoundBuffer();
	~SoundBuffer();

	// セカンダリバッファ作成
	bool Create(IDirectSound8* pDirectSound8, WAVEFORMATEX& WaveFormat, byte* WaveData, int DataSize);

	// 音の再生
	void Play(bool isLoop);
	// 音の停止
	void Stop();
	// メモリ開放
	void Delete();
};