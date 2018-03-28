#include "direct3d.h"

Direct3D::Direct3D() {
	pD3D9 = NULL;			// Direct3D�f�o�C�X�����p�I�u�W�F�N�g
	pDevice3D = NULL;		// Direct3D�̃f�o�C�X
}
Direct3D::~Direct3D() {
	// DirectX�̕ϐ��͕K���㏈�������邱��
	if (pDevice3D != NULL)
		pDevice3D->Release();
	if (pD3D9 != NULL)
		pD3D9->Release();
}

bool Direct3D::Create(HWND hWnd, int Width, int Height) {

	// Direct3D9�I�u�W�F�N�g�̍쐬	
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	// �f�B�X�v���C���擾
	D3DDISPLAYMODE Display;
	pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display);

	// �X���b�v�`�F�C���ݒ�
	// ��X�����@TRUE:�E�B���h�E���[�h�@FALSE:�t���X�N���[��
	D3DPRESENT_PARAMETERS	D3DParam = {
		(UINT)Width,(UINT)Height,Display.Format,(UINT)1,D3DMULTISAMPLE_NONE,0,
		D3DSWAPEFFECT_DISCARD,hWnd,/*����FALSE�Ńt���X�N*/true,TRUE,D3DFMT_D24S8,0,(UINT)0,(UINT)D3DPRESENT_INTERVAL_DEFAULT
	};

	// HAL���[�h��3D�f�o�C�X�쐬
	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
		if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
			// REF���[�h��3D�f�o�C�X�쐬
			if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
				if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
				{
					// 3D�f�o�C�X�쐬���s(���̃O���t�B�b�N�{�[�h�ł�DirectX���g���Ȃ�)
					pD3D9->Release();
					return false;
				}

	return true;
}