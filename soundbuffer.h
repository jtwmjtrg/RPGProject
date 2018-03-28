#pragma once
#include "directsound.h"

class SoundBuffer
{
public:
	IDirectSoundBuffer8* pSecondaryBuffer;	// �Z�J���_���o�b�t�@

	SoundBuffer();
	~SoundBuffer();

	// �Z�J���_���o�b�t�@�쐬
	bool Create(IDirectSound8* pDirectSound8, WAVEFORMATEX& WaveFormat, byte* WaveData, int DataSize);

	// ���̍Đ�
	void Play(bool isLoop);
	// ���̒�~
	void Stop();
	// �������J��
	void Delete();
};