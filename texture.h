#pragma once
#include "direct3d.h"

class Texture
{
public:
	// �e�N�X�`���f�o�C�X
	IDirect3DTexture9* pTexture;

	Texture();
	~Texture();

	// �摜�f�[�^�ǂݍ���
	bool Load(IDirect3DDevice9* pDevice3D, TCHAR* FileName);

	// �e�N�X�`���f�o�C�X�폜
	void Delete();
};