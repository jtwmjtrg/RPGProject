#include "direct3d.h"

Direct3D::Direct3D() {
	pD3D9 = NULL;			// Direct3Dデバイス生成用オブジェクト
	pDevice3D = NULL;		// Direct3Dのデバイス
}
Direct3D::~Direct3D() {
	// DirectXの変数は必ず後処理をすること
	if (pDevice3D != NULL)
		pDevice3D->Release();
	if (pD3D9 != NULL)
		pD3D9->Release();
}

bool Direct3D::Create(HWND hWnd, int Width, int Height) {

	// Direct3D9オブジェクトの作成	
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	// ディスプレイ情報取得
	D3DDISPLAYMODE Display;
	pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display);

	// スワップチェイン設定
	// 第９引数　TRUE:ウィンドウモード　FALSE:フルスクリーン
	D3DPRESENT_PARAMETERS	D3DParam = {
		(UINT)Width,(UINT)Height,Display.Format,(UINT)1,D3DMULTISAMPLE_NONE,0,
		D3DSWAPEFFECT_DISCARD,hWnd,/*ここFALSEでフルスク*/true,TRUE,D3DFMT_D24S8,0,(UINT)0,(UINT)D3DPRESENT_INTERVAL_DEFAULT
	};

	// HALモードで3Dデバイス作成
	if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
		if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DParam.hDeviceWindow,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
			// REFモードで3Dデバイス作成
			if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
				if (FAILED(pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, D3DParam.hDeviceWindow,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DParam, &pDevice3D)))
				{
					// 3Dデバイス作成失敗(このグラフィックボードではDirectXが使えない)
					pD3D9->Release();
					return false;
				}

	return true;
}