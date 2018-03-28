#include "directsound.h"

DirectSound::DirectSound()
{
	pDirectSound8 = NULL;	// DirectSound�f�o�C�X
}
DirectSound::~DirectSound()
{
	// DirectX�̃f�o�C�X�͊J�����K�{
	if (pDirectSound8 != NULL)
		pDirectSound8->Release();
}

bool DirectSound::Create(HWND hWnd)
{
	// �T�E���h�f�o�C�X�쐬
	DirectSoundCreate8(NULL, &pDirectSound8, NULL);
	// �������x���̃Z�b�g�i�����ŉ���炷�E�B���h�E���w�肷��K�v������j
	if (FAILED(pDirectSound8->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		// ���s
		return false;
	}

	return true;
}