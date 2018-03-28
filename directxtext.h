#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <vector>
#include <Windows.h>
#include <tchar.h>

#include "direct3d.h"

class DirectXText
{
private:
	ID3DXFont*			pFont;	// Direct3D�t�H���g
	RECT				Rect;	// �`��̈�

public:
	std::vector<TCHAR>	Buf;	// ������o�b�t�@

	DirectXText();
	virtual ~DirectXText();

	// DirectX�t�H���g�쐬
	bool Create(IDirect3DDevice9* pD3DDevice, int FontHeight = 16);

	// ������̕`��
	void Draw(DWORD Color, int x, int y, const TCHAR* Str, ...);
};