#pragma once
#include "direct3d.h"

class Texture
{
public:
	// テクスチャデバイス
	IDirect3DTexture9* pTexture;

	Texture();
	~Texture();

	// 画像データ読み込み
	bool Load(IDirect3DDevice9* pDevice3D, TCHAR* FileName);

	// テクスチャデバイス削除
	void Delete();
};