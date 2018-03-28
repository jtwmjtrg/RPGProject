#pragma once
#include "direct3d.h"

class Sprite
{
public:
	// 板ポリゴン頂点情報
	struct Vertex {
		float x, y, z;	// 3次元座標
		float rhw;		// 2D変換済みフラグ
		float u, v;		// UV座標
	};
	// FVF（柔軟な頂点構造体宣言）フラグ
	static const DWORD SPRITE_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

	// スプライト位置
	D3DXVECTOR2 pos;
	// スプライトサイズ
	int width;
	int height;
	// UVの分割数
	unsigned int divU;
	unsigned int divV;
	// UVの番号
	unsigned int numU;
	unsigned int numV;

	Sprite();
	~Sprite();

	void SetPos(float x, float y);			// x座標,y座標を変更する
	void SetWidth(int Width, int Height);	// 縦幅と横幅を変更する
	void SetDivide(unsigned int DivU, unsigned int DivV);
	void SetUVNum(unsigned int NumU, unsigned int NumV);
	void Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture);							// 中心座標描写
	void Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture);				// 描写
	void Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn);	// 分割描画
	void Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);	// 頂点指定描写
};
