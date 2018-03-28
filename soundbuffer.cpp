#include "soundbuffer.h"

SoundBuffer::SoundBuffer()
{
	pSecondaryBuffer = NULL;
}
SoundBuffer::~SoundBuffer() {}

bool SoundBuffer::Create(IDirectSound8*	pDirectSound8, WAVEFORMATEX& WaveFormat, byte* WaveData, int DataSize)
{
	DSBUFFERDESC	desc = {};			// セカンダリバッファ作成用設定

	// チャンネル数での分岐、モノラルは1チャンネル、ステレオは2チャンネル
	if (WaveFormat.nChannels == 1) {
		desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_STATIC;
		desc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	}
	else {
		desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLPAN | DSBCAPS_STATIC;
		// | DSBCAPS_CTRLFX;	エフェクトを追加すると Duplicate できない
		desc.guid3DAlgorithm = GUID_NULL;
	}
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwBufferBytes = DataSize;			// 音データサイズ指定
	desc.lpwfxFormat = &WaveFormat;			// フォーマット指定

	IDirectSoundBuffer*	pPrimaryBuffer = NULL;	// プライマリバッファ
												// プライマリバッファ作成
	pDirectSound8->CreateSoundBuffer(&desc, &pPrimaryBuffer, NULL);
	// プライマリバッファからセカンダリバッファ作成
	pPrimaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&pSecondaryBuffer);
	pPrimaryBuffer->Release();	// セカンダリバッファが作成できれば、プライマリバッファは破棄してかまわない

	unsigned char* block1 = NULL;
	unsigned char* block2 = NULL;
	unsigned long blockSize1 = 0;
	unsigned long blockSize2 = 0;
	// セカンダリバッファをロックしてデータ書き込み
	pSecondaryBuffer->Lock(0, DataSize,
		(void**)&block1, &blockSize1, (void**)&block2, &blockSize2, DSBLOCK_ENTIREBUFFER);
	// セカンダリバッファに音データコピー
	memcpy(block1, WaveData, DataSize);
	// セカンダリバッファロック解除
	pSecondaryBuffer->Unlock(block1, blockSize1, block2, 0);

	return true;
}

void SoundBuffer::Play(bool isLoop)
{
	// サウンド再生
	if (pSecondaryBuffer != NULL) {
		DWORD LoopFlag = (isLoop ? DSBPLAY_LOOPING : 0);
		pSecondaryBuffer->Play(0, 0, LoopFlag);
	}
}
void SoundBuffer::Stop()
{
	if (pSecondaryBuffer != NULL)
		pSecondaryBuffer->Stop();
}

void SoundBuffer::Delete()
{
	Stop();	// セカンダリバッファは破棄する前に再生停止すること

	if (pSecondaryBuffer != NULL)
	{
		pSecondaryBuffer->Release();
	}
}