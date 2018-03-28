#include "texture.h"

Texture::Texture() {
	pTexture = NULL;		// ポインタ初期化
}
Texture::~Texture() {}

bool Texture::Load(IDirect3DDevice9* pDevice3D, TCHAR* FileName)
{
	// 画像読み込み
	// DirextXやWindowsAPIの関数はHRESULTを結果に返す関数が多い
	// FAILEDマクロで関数が失敗したかわかる
	// SUCEEDEDマクロで関数が成功したかわかる
	if (FAILED(D3DXCreateTextureFromFileEx(
		pDevice3D,
		FileName,
		D3DX_DEFAULT_NONPOW2,		// 幅(ピクセル)
		D3DX_DEFAULT_NONPOW2,		// 高さ(ピクセル)
		0,							// ミップレベル
		0,							// Usage
		D3DFMT_A8R8G8B8,			// ピクセル フォーマット
		D3DPOOL_MANAGED,			// メモリ クラス
		D3DX_FILTER_LINEAR,			// Filter
		D3DX_FILTER_LINEAR,			// MipFilter
		D3DCOLOR_ARGB(255, 255, 255, 254),		// 透過色
		NULL,						// ソース イメージ ファイル内のデータ
		NULL,						// 256 色パレット
		&pTexture					// ポインター
		))) 
	{
		// 画像読み込み失敗（ファイルがない可能性あり）
		pTexture = NULL;	// ポインタ初期化
		return false;
	}

	// 画像読み込み成功
	return true;
}

void Texture::Delete() {
	// 読み込まれていたら破棄
	if (pTexture != NULL)
		pTexture->Release();
}