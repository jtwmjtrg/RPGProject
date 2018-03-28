#pragma once
#include "direct3d.h"

class Sprite
{
public:
	// �|���S�����_���
	struct Vertex {
		float x, y, z;	// 3�������W
		float rhw;		// 2D�ϊ��ς݃t���O
		float u, v;		// UV���W
	};
	// FVF�i�_��Ȓ��_�\���̐錾�j�t���O
	static const DWORD SPRITE_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

	// �X�v���C�g�ʒu
	D3DXVECTOR2 pos;
	// �X�v���C�g�T�C�Y
	int width;
	int height;
	// UV�̕�����
	unsigned int divU;
	unsigned int divV;
	// UV�̔ԍ�
	unsigned int numU;
	unsigned int numV;

	Sprite();
	~Sprite();

	void SetPos(float x, float y);			// x���W,y���W��ύX����
	void SetWidth(int Width, int Height);	// �c���Ɖ�����ύX����
	void SetDivide(unsigned int DivU, unsigned int DivV);
	void SetUVNum(unsigned int NumU, unsigned int NumV);
	void Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture);							// ���S���W�`��
	void Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture);				// �`��
	void Draw(int x, int y, IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn);	// �����`��
	void Draw(IDirect3DDevice9* pDevice3D, IDirect3DTexture9* pTexture, bool isTurn, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);	// ���_�w��`��
};
