#pragma once

#include "directsound.h"

class Wave {
public:
	WAVEFORMATEX		WaveFormat;	// Waveフォーマット
	byte*				WaveData;	// 音の生データ
	int					DataSize;	// データサイズ

	Wave();
	~Wave();

	// Wavファイル読み込み
	bool Load(TCHAR* FileName);

	// wavファイル削除
	void Delete();
};
