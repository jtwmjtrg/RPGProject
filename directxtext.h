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
	ID3DXFont*			pFont;	// Direct3Dフォント
	RECT				Rect;	// 描画領域

public:
	std::vector<TCHAR>	Buf;	// 文字列バッファ

	DirectXText();
	virtual ~DirectXText();

	// DirectXフォント作成
	bool Create(IDirect3DDevice9* pD3DDevice, int FontHeight = 16);

	// 文字列の描画
	void Draw(DWORD Color, int x, int y, const TCHAR* Str, ...);
};