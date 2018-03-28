#include "texture.h"

Texture::Texture() {
	pTexture = NULL;		// �|�C���^������
}
Texture::~Texture() {}

bool Texture::Load(IDirect3DDevice9* pDevice3D, TCHAR* FileName)
{
	// �摜�ǂݍ���
	// DirextX��WindowsAPI�̊֐���HRESULT�����ʂɕԂ��֐�������
	// FAILED�}�N���Ŋ֐������s�������킩��
	// SUCEEDED�}�N���Ŋ֐��������������킩��
	if (FAILED(D3DXCreateTextureFromFileEx(
		pDevice3D,
		FileName,
		D3DX_DEFAULT_NONPOW2,		// ��(�s�N�Z��)
		D3DX_DEFAULT_NONPOW2,		// ����(�s�N�Z��)
		0,							// �~�b�v���x��
		0,							// Usage
		D3DFMT_A8R8G8B8,			// �s�N�Z�� �t�H�[�}�b�g
		D3DPOOL_MANAGED,			// ������ �N���X
		D3DX_FILTER_LINEAR,			// Filter
		D3DX_FILTER_LINEAR,			// MipFilter
		D3DCOLOR_ARGB(255, 255, 255, 254),		// ���ߐF
		NULL,						// �\�[�X �C���[�W �t�@�C�����̃f�[�^
		NULL,						// 256 �F�p���b�g
		&pTexture					// �|�C���^�[
		))) 
	{
		// �摜�ǂݍ��ݎ��s�i�t�@�C�����Ȃ��\������j
		pTexture = NULL;	// �|�C���^������
		return false;
	}

	// �摜�ǂݍ��ݐ���
	return true;
}

void Texture::Delete() {
	// �ǂݍ��܂�Ă�����j��
	if (pTexture != NULL)
		pTexture->Release();
}