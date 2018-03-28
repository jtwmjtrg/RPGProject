#pragma once
#define DIRECTSOUND_VERSION 0x800	// DirectSoundのバージョン
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#include <comdef.h>
#include <dsound.h>
#include <tchar.h>

class DirectSound
{
public:
	IDirectSound8*	pDirectSound8; // DirectSoundデバイス

	DirectSound();
	~DirectSound();

	// サウンドデバイス作成
	bool Create(HWND hWnd);
};