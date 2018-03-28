#include "sprite.h"

Sprite::Sprite()
{
	pos.x = pos.y = 0.0f;
	width = 0;
	height = 0;
	divU = 1;
	divV = 1;
	numU = 1;
	numV = 1;
}
Sprite::~Sprite() {}

void Sprite::SetPos(float x, float y)
{
	pos.x = x;
	pos.y = y;
}
void Sprite::SetWidth(int Width, int Height)
{
	width = Width;
	height = Height;
}
void Sprite::SetDivide(unsigned int DivU, unsigned int DivV)
{
	if (DivU <= 0 || DivV <= 0)
		return;

	divU = DivU;
	divV = DivV;
}
void Sprite::SetUVNum(unsigned int NumU, unsigned int NumV)
{
	if (NumU >= divU)
		return;
	if (NumV >= divV)
		return;

	numU = NumU;
	numV = NumV;
}

void Sprite::Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture)
{
	// 頂点情報セット
	Vertex vtx[4] = {
		{ pos.x + width / 2, pos.y - height / 2, 0.0f, 1.0f, 1.0f, 0.0f },
		{ pos.x + width / 2, pos.y + height / 2, 0.0f, 1.0f, 1.0f, 1.0f },
		{ pos.x - width / 2, pos.y - height / 2, 0.0f, 1.0f, 0.0f, 0.0f },
		{ pos.x - width / 2, pos.y + height / 2, 0.0f, 1.0f, 0.0f, 1.0f }
	};

	// テクスチャセット
	pDevice3D->SetTexture(0, pTexture);
	// 頂点構造体宣言セット
	pDevice3D->SetFVF(SPRITE_FVF);
	// スプライト描画
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(Vertex));
}

void Sprite::Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture)
{
	// 頂点情報セット
	Vertex vtx[4] = {
		{ x + width	, y			, 0.0f, 1.0f, 1.0f, 0.0f },
		{ x + width	, y + height, 0.0f, 1.0f, 1.0f, 1.0f },
		{ x			, y			, 0.0f, 1.0f, 0.0f, 0.0f },
		{ x			, y + height, 0.0f, 1.0f, 0.0f, 1.0f }
	};

	// テクスチャセット
	pDevice3D->SetTexture(0, pTexture);
	// 頂点構造体宣言セット
	pDevice3D->SetFVF(SPRITE_FVF);
	// スプライト描画
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(Vertex));
}

void Sprite::Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn)
{
	// 頂点情報セット
	Vertex vtx[4] = {
		{ x + width	, y			, 0.0f, 1.0f,(isTurn ? (float)numU / divU : (float)(numU + 1) / divU), (float)numV / divV },
		{ x + width	, y + height, 0.0f, 1.0f,(isTurn ? (float)numU / divU : (float)(numU + 1) / divU), (float)(numV + 1) / divV },
		{ x			, y			, 0.0f, 1.0f,(isTurn ? (float)(numU + 1) / divU : (float)numU / divU), (float)numV / divV },
		{ x			, y + height, 0.0f, 1.0f,(isTurn ? (float)(numU + 1) / divU : (float)numU / divU), (float)(numV + 1) / divV }
	};

	// テクスチャセット
	pDevice3D->SetTexture(0, pTexture);
	// 頂点構造体宣言セット
	pDevice3D->SetFVF(SPRITE_FVF);
	// スプライト描画
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(Vertex));
}

void Sprite::Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	// 頂点情報セット
	Vertex vtx[4] = {
		{ x0	, y0	, 0.0f, 1.0f, 1.0f, 0.0f },
		{ x3	, y3	, 0.0f, 1.0f, 1.0f, 1.0f },
		{ x1	, y1	, 0.0f, 1.0f, 0.0f, 0.0f },
		{ x2	, y2	, 0.0f, 1.0f, 0.0f, 1.0f }
	};

	// テクスチャセット
	pDevice3D->SetTexture(0, pTexture);
	// 頂点構造体宣言セット
	pDevice3D->SetFVF(SPRITE_FVF);
	// スプライト描画
	pDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(Vertex));
}