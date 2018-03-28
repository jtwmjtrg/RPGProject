/*
gitクラーケン初使用
初ギットクラーケン
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

	// 3Dデバイス作成
	direct3d.Create(hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	InitRender();				// 透過の設定
	// 幾何オブジェクト初期化
	Geometory::Init(direct3d.pDevice3D);
	// カメラ位置、注視点セット
	camera->SetEyeAtPos(D3DXVECTOR3(0, 0, 200), D3DXVECTOR3(0, 0, 0));
	// 透視射影変換行列作成
	camera->SetProjMatrix((float)lpCreateStruct->cx / (float)lpCreateStruct->cy);
	// 透視射影変換行列セット
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


// ウィンドウプロシージャ、ウィンドウに対するメッセージ処理を行う
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		// ウィンドウが破棄されたとき
	//case WM_DESTROY:
		//PostQuitMessage(0);	// WM_QUITメッセージをメッセージキューに送る
		//return 0;
	}
	// デフォルトのメッセージ処理を行う
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 
// ---------------------------------------------------------------------------------------

Manager* Manager::_instance = NULL;	// シングルトンパターン

// WinMain関数（アプリケーションの開始関数）
// コンソールアプリケーションと違い、コンソールを開かない
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	const TCHAR* WC_BASIC = _T("BASIC_WINDOW");
	// シンプルウィンドウクラス設定
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,WndProc, 0,0,hInstance,
		(HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HCURSOR)LoadImage(NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, WC_BASIC , NULL };

	// シンプルウィンドウクラス作成
	if (!RegisterClassEx(&wcex))
		return false;

	// ウィンドウ幅、高さはディスプレイに依存する。普通は4:3
	// ウィンドウの作成
	HWND hWnd = CreateWindowEx(0, WC_BASIC,
		_T("RPG"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	////////////////////////////////////
	//	Direct3Dの初期化
	////////////////////////////////////
	//direct3d.Create(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT);	// ウィンドウ生成
	//InitRender();										// 透過の設定

	////////////////////////////////////
	//	カメラ
	////////////////////////////////////
	Camera*	camera = Camera::GetInstance();

	////////////////////////////////////
	//	フォント
	////////////////////////////////////
	directxtext.Create(direct3d.pDevice3D, 32);	// フォント生成

	////////////////////////////////
	// DirectSoundデバイス作成
	////////////////////////////////
	directsound.Create(hWnd);	// サウンドデバイス生成

	// ジョイパッド初期化
	Input::JoypadInitialize();

	// キー入力初期化
	Input::UpDate();

	// 乱数シード値初期化
	RandClass::Ini();
	
	// マネージャー作成
	Manager* mgr = Manager::Instance();	// 唯一のManagerを生成してポインタを返す

	Player::playerGraph = 0;

	// debug
	int count		= 0;
	int fps			= 0;
	int presec		= 0;
	time_t now		= time(NULL);
	struct tm *pnow	= localtime(&now);
	presec			= pnow->tm_sec;

	// メッセージループ
	MSG msg = {};
	while (msg.message != WM_QUIT
		&& Input::Get(VK_ESCAPE) == 0
		&& (Input::Get(0, XINPUT_GAMEPAD_START) == 0 || Input::Get(0, XINPUT_GAMEPAD_BACK) == 0)
		&& !Scene::GetEnd()) {
		// アプリケーションに送られてくるメッセージをメッセージキューから取得する
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);	// アプリケーションの各ウィンドウプロシージャにメッセージを転送する
		}
		// メッセージ処理をしていないとき
		else {
			//（ここにDirectXの処理を書く）

			///////////////////////////////////////////////////////////////////////////////////////////////

			// カメラ更新
			camera->Update();
			// ビュー変換行列セット
			Object3D::SetViewMtx(camera->GetViewMatrix());

			// debug
			now		= time(NULL);
			pnow	= localtime(&now);

			// キー入力更新
			Input::UpDate();

			// 描画開始
			if (SUCCEEDED(direct3d.pDevice3D->BeginScene())){
				DWORD ClearColor = 0xff808080;	// 背景クリア色
				// 背景クリア
				direct3d.pDevice3D->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, ClearColor, 1.0f, 0);

				// メイン処理
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

				// 描画終了
				direct3d.pDevice3D->EndScene();
			}
			// 描画反映
			direct3d.pDevice3D->Present(NULL, NULL, NULL, NULL);

			///////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	// マネージャー削除
	Manager::Destroy();		// 終了処理

	// プレイヤー削除
	if (Player::born) {
		Player::DeletePlayer();
	}

	// 終了処理
	AllDeleteGraph();		// 画像をすべて破棄
	AllDeleteSoundMem();	// 音を全て破棄
	Camera::Release();		// カメラ

	return 0;
}