#pragma once

#include "directsound.h"

class Wave {
public:
	WAVEFORMATEX		WaveFormat;	// Wave�t�H�[�}�b�g
	byte*				WaveData;	// ���̐��f�[�^
	int					DataSize;	// �f�[�^�T�C�Y

	Wave();
	~Wave();

	// Wav�t�@�C���ǂݍ���
	bool Load(TCHAR* FileName);

	// wav�t�@�C���폜
	void Delete();
};
