#pragma once
#define DIRECTSOUND_VERSION 0x800	// DirectSound�̃o�[�W����
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#include <comdef.h>
#include <dsound.h>
#include <tchar.h>

class DirectSound
{
public:
	IDirectSound8*	pDirectSound8; // DirectSound�f�o�C�X

	DirectSound();
	~DirectSound();

	// �T�E���h�f�o�C�X�쐬
	bool Create(HWND hWnd);
};