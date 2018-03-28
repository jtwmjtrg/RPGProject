/*
git�N���[�P�����g�p
���M�b�g�N���[�P��
*/

#include <Windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include <time.h>

#include "graphicdata.h"
#include "sounddata.h"
#include "input.h"

#include "Manager.h"

#include "Camera.h"




BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) {

	Camera*	camera = Camera::GetInstance();

	// 3D�f�o�C�X�쐬
	direct3d.Create(hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	InitRender();				// ���߂̐ݒ�
	// �􉽃I�u�W�F�N�g������
	Geometory::Init(direct3d.pDevice3D);
	// �J�����ʒu�A�����_�Z�b�g
	camera->SetEyeAtPos(D3DXVECTOR3(0, 0, 200), D3DXVECTOR3(0, 0, 0));
	// �����ˉe�ϊ��s��쐬
	camera->SetProjMatrix((float)lpCreateStruct->cx / (float)lpCreateStruct->cy);
	// �����ˉe�ϊ��s��Z�b�g
	Object3D::SetProjMtx(camera->GetProjMatrix());

	return TRUE;
}

void OnClose(HWND hWnd) {
	DestroyWindow(hWnd);
}
void ApplicationEnd() {
	PostQuitMessage(0);
}
void OnDestroy(HWND hWnd) {
	ApplicationEnd();
}


// �E�B���h�E�v���V�[�W���A�E�B���h�E�ɑ΂��郁�b�Z�[�W�������s��
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		// �E�B���h�E���j�����ꂽ�Ƃ�
	//case WM_DESTROY:
		//PostQuitMessage(0);	// WM_QUIT���b�Z�[�W�����b�Z�[�W�L���[�ɑ���
		//return 0;
	}
	// �f�t�H���g�̃��b�Z�[�W�������s��
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 
// ---------------------------------------------------------------------------------------

Manager* Manager::_instance = NULL;	// �V���O���g���p�^�[��

// WinMain�֐��i�A�v���P�[�V�����̊J�n�֐��j
// �R���\�[���A�v���P�[�V�����ƈႢ�A�R���\�[�����J���Ȃ�
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	const TCHAR* WC_BASIC = _T("BASIC_WINDOW");
	// �V���v���E�B���h�E�N���X�ݒ�
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,WndProc, 0,0,hInstance,
		(HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HCURSOR)LoadImage(NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, WC_BASIC , NULL };

	// �V���v���E�B���h�E�N���X�쐬
	if (!RegisterClassEx(&wcex))
		return false;

	// �E�B���h�E���A�����̓f�B�X�v���C�Ɉˑ�����B���ʂ�4:3
	// �E�B���h�E�̍쐬
	HWND hWnd = CreateWindowEx(0, WC_BASIC,
		_T("RPG"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	////////////////////////////////////
	//	Direct3D�̏�����
	////////////////////////////////////
	//direct3d.Create(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT);	// �E�B���h�E����
	//InitRender();										// ���߂̐ݒ�

	////////////////////////////////////
	//	�J����
	////////////////////////////////////
	Camera*	camera = Camera::GetInstance();

	////////////////////////////////////
	//	�t�H���g
	////////////////////////////////////
	directxtext.Create(direct3d.pDevice3D, 32);	// �t�H���g����

	////////////////////////////////
	// DirectSound�f�o�C�X�쐬
	////////////////////////////////
	directsound.Create(hWnd);	// �T�E���h�f�o�C�X����

	// �W���C�p�b�h������
	Input::JoypadInitialize();

	// �L�[���͏�����
	Input::UpDate();

	// �����V�[�h�l������
	RandClass::Ini();
	
	// �}�l�[�W���[�쐬
	Manager* mgr = Manager::Instance();	// �B���Manager�𐶐����ă|�C���^��Ԃ�

	Player::playerGraph = 0;

	// debug
	int count		= 0;
	int fps			= 0;
	int presec		= 0;
	time_t now		= time(NULL);
	struct tm *pnow	= localtime(&now);
	presec			= pnow->tm_sec;

	// ���b�Z�[�W���[�v
	MSG msg = {};
	while (msg.message != WM_QUIT
		&& Input::Get(VK_ESCAPE) == 0
		&& (Input::Get(0, XINPUT_GAMEPAD_START) == 0 || Input::Get(0, XINPUT_GAMEPAD_BACK) == 0)
		&& !Scene::GetEnd()) {
		// �A�v���P�[�V�����ɑ����Ă��郁�b�Z�[�W�����b�Z�[�W�L���[����擾����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);	// �A�v���P�[�V�����̊e�E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W��]������
		}
		// ���b�Z�[�W���������Ă��Ȃ��Ƃ�
		else {
			//�i������DirectX�̏����������j

			///////////////////////////////////////////////////////////////////////////////////////////////

			// �J�����X�V
			camera->Update();
			// �r���[�ϊ��s��Z�b�g
			Object3D::SetViewMtx(camera->GetViewMatrix());

			// debug
			now		= time(NULL);
			pnow	= localtime(&now);

			// �L�[���͍X�V
			Input::UpDate();

			// �`��J�n
			if (SUCCEEDED(direct3d.pDevice3D->BeginScene())){
				DWORD ClearColor = 0xff808080;	// �w�i�N���A�F
				// �w�i�N���A
				direct3d.pDevice3D->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, ClearColor, 1.0f, 0);

				// ���C������
				mgr->UpDate();

				// debug
				if (pnow->tm_sec == presec) {
					++count;
				}
				else {
					fps		= count;
					count	= 0;

					presec = pnow->tm_sec;
				}
				DrawBox(1920 - 160, 1080 - 48, 159, 47, true);
				DrawString(WHITE, 1920 - 160 + 16, 1080 - 48 + 8, "fps: %d", fps);
				/*DrawString(WHITE, 1500, 32, "presec: %d", presec);
				DrawString(WHITE, 1500, 64, "tm_sec: %d", pnow->tm_sec);
				DrawString(WHITE, 1500, 96, "count: %d", count);*/

				// �`��I��
				direct3d.pDevice3D->EndScene();
			}
			// �`�攽�f
			direct3d.pDevice3D->Present(NULL, NULL, NULL, NULL);

			///////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	// �}�l�[�W���[�폜
	Manager::Destroy();		// �I������

	// �v���C���[�폜
	if (Player::born) {
		Player::DeletePlayer();
	}

	// �I������
	AllDeleteGraph();		// �摜�����ׂĔj��
	AllDeleteSoundMem();	// ����S�Ĕj��
	Camera::Release();		// �J����

	return 0;
}