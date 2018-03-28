#include "Effect3D.h"

// テクニックハンドル取得
void Effect3D::GetTechniques()
{

	D3DXEFFECT_DESC Effect3DDesc = {};
	D3DXTECHNIQUE_DESC TechniqueDesc = {};
	D3DXHANDLE hTechnique = NULL;

	// エフェクト情報取得
	pEffect->GetDesc(&Effect3DDesc);

	// 現在のテクニック取得
	hCurrentTechnique = pEffect->GetCurrentTechnique();

	// テクニックの取得
	for (DWORD i = 0; i < Effect3DDesc.Techniques; i++)
	{
		// テクニックハンドル取得
		hTechnique = pEffect->GetTechnique(i);
		// テクニック情報取得
		pEffect->GetTechniqueDesc(hTechnique, &TechniqueDesc);

		TCHAR tTechniqueName[MAX_PATH] = {};
		strcpy(tTechniqueName, TechniqueDesc.Name);

		if (TechniqueDesc.Name != NULL)
		{
			hTechniques.insert(std::make_pair(tTechniqueName, hTechnique));
		}

	}
}
// シェーダ変数ハンドル取得
void Effect3D::GetParams()
{

	D3DXEFFECT_DESC Effect3DDesc = {};
	D3DXPARAMETER_DESC ParamDesc = {};
	D3DXHANDLE hParam = NULL;

	// エフェクト情報取得
	pEffect->GetDesc(&Effect3DDesc);

	// シェーダグローバル変数の取得
	for (DWORD i = 0; i < Effect3DDesc.Parameters; i++)
	{
		// シェーダグローバル変数ハンドル取得
		hParam = pEffect->GetParameter(NULL, i);
		// シェーダグローバル変数の情報取得
		pEffect->GetParameterDesc(hParam, &ParamDesc);

		TCHAR tParamName[MAX_PATH] = {};
		strcpy(tParamName, ParamDesc.Name);

		if (ParamDesc.Name != NULL)
		{
			hParams.insert(std::make_pair(tParamName, hParam));
		}

	}
}

// コンストラクタ
Effect3D::Effect3D() :pEffect(NULL), ID(0), hCurrentTechnique(NULL), CurrentPass(0)
{}


// デストラクタ
Effect3D::~Effect3D()
{
	hParams.clear();
	hTechniques.clear();
	if (pEffect) {
		pEffect->Release();
		pEffect = nullptr;
	}
}


// ファイルからエフェクト作成
bool Effect3D::Create(IDirect3DDevice9* pD3DDevice, const TCHAR* FXFileName)
{
	// すでに作成されていたら失敗
	if (pEffect != NULL)
		return false;

	LPD3DXBUFFER pError = NULL;
	// エフェクト作成
	HRESULT hr = D3DXCreateEffectFromFileEx(pD3DDevice, FXFileName, NULL, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &pEffect, &pError);

	// 作成失敗
	if (FAILED(hr))
	{
		::OutputDebugStringA(static_cast<char*>(pError->GetBufferPointer()));
		::OutputDebugString(_T("\n"));
		return false;
	}

	// シェーダ情報取得
	this->GetTechniques();
	this->GetParams();

	// エフェクト名
	Name = FXFileName;

	return true;
}

// リソースからエフェクト作成
bool Effect3D::Create(IDirect3DDevice9* pD3DDevice, int ResourceID)
{
	// すでに作成されていたら失敗
	if (pEffect != NULL)
		return false;

	LPD3DXBUFFER pError = NULL;
	// エフェクト作成
	HRESULT hr = D3DXCreateEffectFromResourceEx(pD3DDevice, NULL, MAKEINTRESOURCE(ResourceID), NULL, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &pEffect, &pError);

	// 作成失敗
	if (FAILED(hr))
	{
		::OutputDebugStringA(static_cast<char*>(pError->GetBufferPointer()));
		::OutputDebugString(_T("\n"));
		return false;
	}

	// シェーダ情報取得
	this->GetTechniques();
	this->GetParams();

	// エフェクトID
	ID = ResourceID;

	return true;
}

// エフェクトポインタ取得
ID3DXEffect* Effect3D::GetEffect() {
	return pEffect;
}

// シェーダグローバル変数名チェック
bool Effect3D::IsParamName(const TCHAR* ParamName) {
	if (hParams.find(ParamName) == hParams.end()) {
		return false;
	}

	return true;
}

// 行列セット
bool Effect3D::SetMatrix(const TCHAR* ParamName, const D3DXMATRIX* pMtx) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetMatrix(hParams[ParamName], pMtx))) {
			return false;
		}
	}

	return true;
}

// 行列パレットセット
bool Effect3D::SetMatrixArray(const TCHAR* ParamName, const D3DXMATRIX* pMtxArray, int Count) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetMatrixArray(hParams[ParamName], pMtxArray, Count))) {
			return false;
		}
	}

	return true;
}

// 4次元ベクトルセット
bool Effect3D::SetVector4(const TCHAR* ParamName, const D3DXVECTOR4* pVec4) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetVector(hParams[ParamName], pVec4))) {
			return false;
		}
	}

	return true;
}

// 3次元ベクトルのセット
bool Effect3D::SetVector3(const TCHAR* ParamName, const D3DXVECTOR3* pVec3)
{

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetFloatArray(hParams[ParamName], &pVec3->x, 3))) {
			return false;
		}
	}

	return true;
}

// 浮動小数セット
bool Effect3D::SetFloat(const TCHAR* ParamName, float Param) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetFloat(hParams[ParamName], Param))) {
			return false;
		}
	}

	return true;
}

// 整数のセット
bool Effect3D::SetInt(const TCHAR* ParamName, int Param)
{

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetInt(hParams[ParamName], Param))) {
			return false;
		}
	}

	return true;
}

// テクスチャセット
bool Effect3D::SetTexture(const TCHAR* TextureName, IDirect3DBaseTexture9* pTexture) {

#ifdef _DEBUG
	if (this->IsParamName(TextureName))
#endif
	{
		if (FAILED(pEffect->SetTexture(hParams[TextureName], pTexture))) {
			return false;
		}
	}

	return true;
}

// テクニックのセット
bool Effect3D::SetTechnique(const TCHAR* TechniqueName, UINT Pass)
{
	// テクニックの変更があった場合
	if (hTechniques[TechniqueName] != hCurrentTechnique) {
		// テクニックのセット
		if (FAILED(pEffect->SetTechnique(hTechniques[TechniqueName]))) {
			return false;
		}
		// 現在のテクニック取得
		hCurrentTechnique = pEffect->GetCurrentTechnique();
	}
	// パスのセット
	CurrentPass = Pass;

	return true;
}

// エフェクト開始
bool Effect3D::Begin() {

#ifdef _DEBUG
	if (pEffect == NULL)
		return false;
#endif


	UINT PassNum;
	pEffect->Begin(&PassNum, 0);
	// パスセット
	if (FAILED(pEffect->BeginPass(CurrentPass))) {
		return false;
	}
	// シェーダ変数の値適応
	pEffect->CommitChanges();

	return true;
}


// エフェクト終了
bool Effect3D::End() {

#ifdef _DEBUG
	if (pEffect == NULL)
		return false;
#endif

	// シェーダプログラムの終了宣言
	pEffect->EndPass();
	pEffect->End();

	return true;
}

//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################

// ビュー変換行列
D3DXMATRIX Object3D::m_ViewMtx;
// 射影変換行列
D3DXMATRIX Object3D::m_ProjMtx;

//-------------------------//
// コンストラクタ
//-------------------------//
Object3D::Object3D()
	:m_Dir(0.0f, 0.0f, -1.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Translate(0, 0, 0)
{
	D3DXMatrixIdentity(&m_RotateMtx);
	D3DXMatrixIdentity(&m_WorldMtx);
}

//-------------------------//
// デストラクタ
//-------------------------//
Object3D::~Object3D()
{}

//-------------------------------------------//
//	ワールド変換行列をセット
//
//	引数：	WorldMtx	ワールド変換行列
//-------------------------------------------//
void Object3D::SetWorldMatrix(const D3DXMATRIX& WorldMtx)
{
	m_WorldMtx = WorldMtx;

	// 計算がScale * Rotate * Translationの順番になっている行列が前提である
	// それ以外の行列からは元の成分を算出できない


	// スケーリング成分取得
	D3DXVECTOR3 Sx(WorldMtx._11, WorldMtx._12, WorldMtx._13);
	D3DXVECTOR3 Sy(WorldMtx._21, WorldMtx._22, WorldMtx._23);
	D3DXVECTOR3 Sz(WorldMtx._31, WorldMtx._32, WorldMtx._33);
	D3DXVECTOR3 Scale(D3DXVec3Length(&Sx), D3DXVec3Length(&Sy), D3DXVec3Length(&Sz));
	m_Scale = Scale;

	// 平行移動成分取得
	m_Translate.x = WorldMtx._41;
	m_Translate.y = WorldMtx._42;
	m_Translate.z = WorldMtx._43;

	// 回転行列取得
	m_RotateMtx._11 = WorldMtx._11 / Scale.x;
	m_RotateMtx._12 = WorldMtx._12 / Scale.x;
	m_RotateMtx._13 = WorldMtx._13 / Scale.x;
	m_RotateMtx._14 = 0.0f;
	m_RotateMtx._21 = WorldMtx._21 / Scale.y;
	m_RotateMtx._22 = WorldMtx._22 / Scale.y;
	m_RotateMtx._23 = WorldMtx._23 / Scale.y;
	m_RotateMtx._24 = 0.0f;
	m_RotateMtx._31 = WorldMtx._31 / Scale.z;
	m_RotateMtx._32 = WorldMtx._32 / Scale.z;
	m_RotateMtx._33 = WorldMtx._33 / Scale.z;
	m_RotateMtx._34 = 0.0f;
	m_RotateMtx._41 = 0.0f;
	m_RotateMtx._42 = 0.0f;
	m_RotateMtx._43 = 0.0f;
	m_RotateMtx._44 = 1.0f;

}

//--------------------------------------------//
//	ワールド変換行列を取得する
//
//	戻り値：ワールド変換行列
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetWorldMatrix() const
{
	return m_WorldMtx;
}

//--------------------------------------------//
//	ワールド変換行列を作成する
//--------------------------------------------//
void Object3D::CreateWorldMatrix() {
	D3DXMATRIX ScaleMtx, RotateMtx, TranslateMtx;
	D3DXMatrixScaling(&ScaleMtx, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixTranslation(&TranslateMtx, m_Translate.x, m_Translate.y, m_Translate.z);
	m_WorldMtx = ScaleMtx * m_RotateMtx * TranslateMtx;
}



//-------------------------------------------//
//	ビュー変換行列をセット
//
//	引数：ViewMtx ビュー変換行列
//-------------------------------------------//
void Object3D::SetViewMtx(const D3DXMATRIX& ViewMtx)
{
	m_ViewMtx = ViewMtx;
}
//--------------------------------------------//
//	ビュー変換行列を取得する
//
//	戻り値：ビュー変換行列
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetViewMatrix()
{
	return m_ViewMtx;
}


//-------------------------------------------//
//	射影変換行列をセット
//
//	引数：ProjMtx 射影変換行列
//-------------------------------------------//
void Object3D::SetProjMtx(const D3DXMATRIX& ProjMtx)
{
	m_ProjMtx = ProjMtx;
}
//--------------------------------------------//
//	射影変換行列を取得する
//
//	戻り値：射影変換行列
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetProjMatrix()
{
	return m_ProjMtx;
}

//////////////////////////////////////////////////////////////////

//-----------------------------------//
//	スケーリングをセットする
//
//	引数：Scale	スケーリング
//-----------------------------------//
void Object3D::SetScale(const D3DXVECTOR3& Scale) {
	m_Scale = Scale;
}
//-----------------------------------//
//	スケーリングをセットする
//
//	引数：x,y,z	各軸に対するスケーリング
//-----------------------------------//
void Object3D::SetScale(float x, float y, float z) {
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

//-----------------------------------//
//	回転をセットする
//
//	引数：Rotate 回転
//-----------------------------------//
void Object3D::SetRotate(const D3DXVECTOR3& Rotate) {
	D3DXMatrixRotationYawPitchRoll(&m_RotateMtx, Rotate.y, Rotate.x, Rotate.z);
}
//-----------------------------------//
//	回転をセットする
//
//	引数：x,y,z	各軸に対する回転
//-----------------------------------//
void Object3D::SetRotate(float x, float y, float z) {
	D3DXMatrixRotationYawPitchRoll(&m_RotateMtx, y, x, z);
}
//-----------------------------------//
//	平行移動をセットする
//
//	引数：Translate 平行移動
//-----------------------------------//
void Object3D::SetTranslate(const D3DXVECTOR3& Translate) {
	m_Translate = Translate;
}
//-----------------------------------//
//	平行移動をセットする
//
//	引数：x,y,z	各軸に対する平行移動
//-----------------------------------//
void Object3D::SetTranslate(float x, float y, float z) {
	m_Translate.x = x;
	m_Translate.y = y;
	m_Translate.z = z;
}

//////////////////////////////////////////////////////////////////


//-----------------------------------//
//	スケーリングを更新する
//
//	引数：Scale	スケーリング
//-----------------------------------//
void Object3D::UpdateScale(const D3DXVECTOR3& Scale)
{
	m_Scale += Scale;
}
//-----------------------------------//
//	スケーリングを更新する
//
//	引数：x,y,z	各軸に対するスケーリング
//-----------------------------------//
void Object3D::UpdateScale(float x, float y, float z)
{
	m_Scale.x += x;
	m_Scale.y += y;
	m_Scale.z += z;
}

//-----------------------------------//
//	回転を更新する
//
//	引数：Rotate 回転
//-----------------------------------//
void Object3D::UpdateRotate(const D3DXVECTOR3& Rotate)
{
	D3DXMATRIX RotateMtx;
	D3DXMatrixRotationYawPitchRoll(&RotateMtx, Rotate.y, Rotate.x, Rotate.z);
	m_RotateMtx = RotateMtx * m_RotateMtx;
}
//-----------------------------------//
//	回転を更新する
//
//	引数：x,y,z	各軸に対する回転
//-----------------------------------//
void Object3D::UpdateRotate(float x, float y, float z)
{
	D3DXMATRIX RotateMtx;
	D3DXMatrixRotationYawPitchRoll(&RotateMtx, y, x, z);
	m_RotateMtx = RotateMtx * m_RotateMtx;
}

//-----------------------------------//
//	平行移動を更新する
//
//	引数：Translate 平行移動
//-----------------------------------//
void Object3D::UpdateTranslate(const D3DXVECTOR3& Translate)
{
	m_Translate += Translate;
}
//-----------------------------------//
//	平行移動を更新する
//
//	引数：x,y,z	各軸に対する平行移動
//-----------------------------------//
void Object3D::UpdateTranslate(float x, float y, float z)
{
	m_Translate.x += x;
	m_Translate.y += y;
	m_Translate.z += z;
}


//////////////////////////////////////////////////////////////////


//-----------------------------------//
//	スケーリングを取得する
//
//	戻り値：スケーリング
//-----------------------------------//
const D3DXVECTOR3& Object3D::GetScale() const {
	return m_Scale;
}
//-----------------------------------//
//	回転を取得する
//
//	戻り値：回転
//-----------------------------------//
const D3DXMATRIX& Object3D::GetRotateMatrix() const {
	return m_RotateMtx;
}
//-----------------------------------//
//	平行移動を取得する
//
//	戻り値：平行移動
//-----------------------------------//
const D3DXVECTOR3& Object3D::GetTranslate() const {
	return m_Translate;
}

//////////////////////////////////////////////////////////////////

//--------------------------------------------//
//	モデルの正面向きセット
//
//	引数：Dir 正面向き
//--------------------------------------------//
void Object3D::SetDir(const D3DXVECTOR3& Dir)
{
	m_Dir = Dir;
}

//--------------------------------------------//
//	モデルの正面向き取得
//
//	戻り値：正面向き
//--------------------------------------------//
D3DXVECTOR3 Object3D::GetDir()
{
	D3DXVECTOR3 Dir;
	D3DXVec3TransformCoord(&Dir, &m_Dir, &m_RotateMtx);
	return Dir;
}

//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################

Effect3D						Geometory::m_Effect;				// 幾何オブジェクトエフェクト
IDirect3DVertexDeclaration9*	Geometory::m_pVertexDecl2D = NULL;	// 2次元幾何オブジェクト頂点構造体宣言
IDirect3DVertexDeclaration9*	Geometory::m_pVertexDecl3D = NULL;	// 3次元幾何オブジェクト頂点構造体宣言

// 初期化
bool Geometory::Init(IDirect3DDevice9* pD3DDevice)
{
	// エフェクト読み込み
	if (!m_Effect.Create(pD3DDevice, _T("geometory.cfx")))
		return false;

	// 2次元幾何オブジェクト頂点構造体定義
	D3DVERTEXELEMENT9 decl2d[] =
	{
		{ 0, 0,	 D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR	 , 0 },
		D3DDECL_END()
	};

	// 2次元幾何オブジェクト頂点構造体宣言作成
	if (FAILED(pD3DDevice->CreateVertexDeclaration(decl2d, &m_pVertexDecl2D)))
	{
		return false;
	}

	// 3次元幾何オブジェクト頂点構造体定義
	D3DVERTEXELEMENT9 decl3d[] =
	{
		{ 0, 0,	 D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION , 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR	 , 0 },
		D3DDECL_END()
	};

	// 3次元幾何オブジェクト頂点構造体宣言作成
	if (FAILED(pD3DDevice->CreateVertexDeclaration(decl3d, &m_pVertexDecl3D)))
	{
		return false;
	}

	atexit(Geometory::Release);
	return true;
}

// 解放
void Geometory::Release()
{
	// 頂点構造体宣言解放
	if (m_pVertexDecl3D) {
		m_pVertexDecl3D->Release();
		m_pVertexDecl3D = NULL;
	}
	if (m_pVertexDecl2D) {
		m_pVertexDecl2D->Release();
		m_pVertexDecl2D = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//								2次元オブジェクト描画										//
//////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------//
//	直線描画
//
//	引数：	Start		始点
//			End			終点
//			Color		色
//
//	戻り値： true:	描画成功
//			false:	描画失敗
//-------------------------------------------//
bool Geometory::DrawLine(const D3DXVECTOR2& Start, const D3DXVECTOR2& End, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// エフェクト開始
	m_Effect.Begin();

	Vtx2D v[2] =
	{
		{ Start.x,Start.y,0.0f,1.0f,Color },
		{ End.x,End.y,0.0f,1.0f,Color },
	};
	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(Vtx2D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-------------------------------------------//
//	丸線描画
//
//	引数：	R		半径
//			Pos		位置
//			Color	色
//			
//	戻り値：	true:	描画成功
//			false:	描画失敗
//-------------------------------------------//
bool Geometory::DrawCircle(float R, const D3DXVECTOR2& Pos, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// エフェクト開始
	m_Effect.Begin();

	const int div = 17;
	const float divide = (D3DX_PI * 2) / (div - 1);
	Vtx2D v[div];

	float phi = 0.0f;
	for (int i = 0; i < div; ++i) {
		v[i].x = R * cosf(phi) + Pos.x;
		v[i].y = R * sinf(phi) + Pos.y;
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
		v[i].color = Color;
		phi += divide;
	}

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div - 1, v, sizeof(Vtx2D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-------------------------------------------//
//	丸描画
//
//	引数：	R		半径
//			Pos		位置
//			Color	色
//			
//	戻り値：	true:	描画成功
//			false:	描画失敗
//-------------------------------------------//
bool Geometory::DrawFillCircle(float R, const D3DXVECTOR2& Pos, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// エフェクト開始
	m_Effect.Begin();

	const int div = 17;
	const float divide = (D3DX_PI * 2) / (div - 1);
	Vtx2D v[div + 1];


	v[0].x = Pos.x;
	v[0].y = Pos.y;
	v[0].z = 0.0f;
	v[0].rhw = 1.0f;
	v[0].color = Color;

	float phi = 0.0f;
	for (int i = 1; i < div + 1; ++i) {
		v[i].x = R * cosf(phi) + Pos.x;
		v[i].y = R * sinf(phi) + Pos.y;
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
		v[i].color = Color;
		phi += divide;
	}

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, div - 1, v, sizeof(Vtx2D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	return true;
}

//-------------------------------------------//
//	矩形線描画
//
//	引数：	Rect		矩形
//			Color		色
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//-------------------------------------------//
bool Geometory::DrawRect(const RECT& Rect, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// エフェクト開始
	m_Effect.Begin();

	RECT rect = Rect;

	Vtx2D v[8] =
	{
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color },

		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color },
		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },

		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },
		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },

		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
	};

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 4, v, sizeof(Vtx2D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-------------------------------------------//
//	矩形描画
//
//	引数：	Rect		矩形
//			Color		色
//
//	戻り値：true:	描画成功
//			false:	描画失敗
//-------------------------------------------//
bool Geometory::DrawFillRect(const RECT& Rect, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// エフェクト開始
	m_Effect.Begin();

	RECT rect = Rect;

	Vtx2D v[4] =
	{
		// 左下
		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },
		// 左上
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
		// 右下
		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },
		// 右上
		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color }

	};

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vtx2D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//								3次元オブジェクト描画										//
//////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------//
//	エフェクトに変換行列セット
//	
//	引数	：	pWorldMtx	ワールド変換行列
//
//	戻り値：	true:	セット成功
//			false:	セット失敗
//------------------------------------------//
bool Geometory::SetEffectParam(const D3DXMATRIX* pWorldMtx)
{

	// ワールド変換行列セット
	if (pWorldMtx != NULL) {
		if (!m_Effect.SetMatrix(_T("g_mWorld"), pWorldMtx))
			return false;
	}
	else {
		D3DXMATRIX	Mtx;
		D3DXMatrixIdentity(&Mtx);
		m_Effect.SetMatrix(_T("g_mWorld"), &Mtx);
	}

	// ビュー変換行列セット
	if (!m_Effect.SetMatrix(_T("g_mView"), &Object3D::GetViewMatrix()))
		return false;
	// 射影変換行列セット
	if (!m_Effect.SetMatrix(_T("g_mProj"), &Object3D::GetProjMatrix()))
		return false;

	return true;
}

//-----------------------------------------------
//	グリッド描画
//
//	引数：	Wide		幅
//			Divide		分割数
//			pWorldMtx	ワールド変換行列
//			Color		色
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawGrid(float Wide, int Divide, const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// 描けない
	if (Divide < 1)
		return false;

	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	// 線の頂点データを作る(時計回りにデータをいれていく)	
	std::vector<Vtx3D> v;
	v.resize(Divide * 4 + 8);

	//縦ライン
	for (unsigned int i = 0; i < v.size() / 2; i += 2)
	{
		v[i].x = -Wide / 2.0f + i / 2 * Wide / (Divide + 1);
		v[i].y = 0.0f;
		v[i].z = Wide / 2.0f;
		v[i].color = Color;

		v[i + 1].x = -Wide / 2.0f + i / 2 * Wide / (Divide + 1);
		v[i + 1].y = 0.0f;
		v[i + 1].z = -Wide / 2.0f;
		v[i + 1].color = Color;
	}

	//横ライン
	for (unsigned int i = v.size() / 2; i < v.size(); i += 2)
	{
		v[i].x = Wide / 2.0f;
		v[i].y = 0.0f;
		v[i].z = -Wide / 2.0f + (i - v.size() / 2) / 2 * Wide / (Divide + 1);
		v[i].color = Color;

		v[i + 1].x = -Wide / 2.0f;
		v[i + 1].y = 0.0f;
		v[i + 1].z = -Wide / 2.0f + (i - v.size() / 2) / 2 * Wide / (Divide + 1);
		v[i + 1].color = Color;
	}

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, v.size() / 2, &v[0], sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}


//-----------------------------------------------
//	3軸描画
//
//	引数：	Length		軸の長さ
//			pWorldMtx	ワールド変換行列
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawAxis(float Length, const D3DXMATRIX* pWorldMtx)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	Vtx3D v[3][2] = {
		{ { -Length / 2.0f,0.0f,0.0f,0xffff0000 },{ Length / 2.0f,0.0f,0.0f,0xffff0000 } },
		{ { 0.0f,-Length / 2.0f,0.0f,0xff00ff00 },{ 0.0f,Length / 2.0f,0.0f,0xff00ff00 } },
		{ { 0.0f,0.0f,-Length / 2.0f,0xff0000ff },{ 0.0f,0.0f,Length / 2.0f,0xff0000ff } },
	};
	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, v, sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-----------------------------------------------
//	直線描画
//
//	引数：	Start		始点
//			End			終点
//			pWorldMtx	ワールド変換行列
//			Color		色
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//-----------------------------------------------
bool Geometory::DrawLine(const D3DXVECTOR3& Start, const D3DXVECTOR3& End, const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	Vtx3D v[2] =
	{
		{ Start.x,Start.y,Start.z,Color },
		{ End.x,End.y,End.z,Color }
	};
	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//--------------------------------------------
//	三角形線を描画
//
//	引数：	Vtx			三角形の頂点
//			pWorldMtx	ワールド変換行列
//			Color		色
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	Vtx3D v[3][2] = {
		{ { Vtx[0].x,Vtx[0].y,Vtx[0].z,Color },{ Vtx[1].x,Vtx[1].y,Vtx[1].z,Color } },
		{ { Vtx[1].x,Vtx[1].y,Vtx[1].z,Color },{ Vtx[2].x,Vtx[2].y,Vtx[2].z,Color } },
		{ { Vtx[2].x,Vtx[2].y,Vtx[2].z,Color },{ Vtx[0].x,Vtx[0].y,Vtx[0].z,Color } }
	};

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, v, sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//--------------------------------------------
//	三角形を描画
//
//	引数：	Vtx			三角形の頂点
//			pWorldMtx	ワールド変換行列
//			Color		色
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawFillTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	Vtx3D v[3] = {
		{ Vtx[0].x,Vtx[0].y,Vtx[0].z,Color },
		{ Vtx[1].x,Vtx[1].y,Vtx[1].z,Color },
		{ Vtx[2].x,Vtx[2].y,Vtx[2].z,Color }
	};

	// 頂点構造体宣言セット	
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, v, sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//--------------------------------------------
//	立方体線を描画
//
//	引数：	pWorldMtx	ワールド変換行列
//			Color		色
//			Min			立方体の対角最小点
//			Max			立方体の対角最大点
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawCube(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	Vtx3D	v[16];

	v[0].x = Min.x;	v[0].y = Min.y;	v[0].z = Min.z;
	v[1].x = Max.x;	v[1].y = Min.y;	v[1].z = Min.z;
	v[2].x = Max.x;	v[2].y = Max.y;	v[2].z = Min.z;
	v[3].x = Min.x;	v[3].y = Max.y;	v[3].z = Min.z;
	v[4].x = Min.x;	v[4].y = Min.y;	v[4].z = Min.z;
	v[5].x = Min.x;	v[5].y = Min.y;	v[5].z = Max.z;
	v[6].x = Min.x;	v[6].y = Max.y;	v[6].z = Max.z;
	v[7].x = Min.x;	v[7].y = Max.y;	v[7].z = Min.z;

	v[8].x = Max.x;	v[8].y = Min.y;	v[8].z = Max.z;
	v[9].x = Min.x;	v[9].y = Min.y;	v[9].z = Max.z;
	v[10].x = Min.x; v[10].y = Max.y; v[10].z = Max.z;
	v[11].x = Max.x; v[11].y = Max.y; v[11].z = Max.z;
	v[12].x = Max.x; v[12].y = Min.y; v[12].z = Max.z;
	v[13].x = Max.x; v[13].y = Min.y; v[13].z = Min.z;
	v[14].x = Max.x; v[14].y = Max.y; v[14].z = Min.z;
	v[15].x = Max.x; v[15].y = Max.y; v[15].z = Max.z;

	for (int i = 0; i < sizeof(v) / sizeof(v[0]); ++i)
		v[i].color = Color;

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// 描画１
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, v, sizeof(Vtx3D))))
		return false;
	// 描画２
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, &v[8], sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-----------------------------------------------
//	球線を描画
//
//	引数：	pWorldMtx	ワールド変換行列
//			Color		色
//			Pos			球の中心座標
//			R			球の半径			
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawSphere(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Pos, float R)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	// 頂点構造体宣言セット
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;

	// x軸回転行列
	D3DXMATRIX RotXMtx;
	D3DXMatrixIdentity(&RotXMtx);
	// y軸回転行列
	D3DXMATRIX RotYMtx;
	D3DXMatrixIdentity(&RotYMtx);

	float divide = D3DX_PI / 8;
	D3DXVECTOR3 vec[17];
	Vtx3D v[17];

	D3DXVECTOR3 Translation = Pos;
	for (float phi = 0; phi < D3DX_PI; phi += divide)
	{
		// 円
		int count = 0;
		for (float theta = 0; theta <= D3DX_PI * 2 + divide; theta += divide)
		{
			vec[count].x = R * cosf(theta);
			vec[count].y = 0.0f;
			vec[count].z = R * sinf(theta);
			count++;
		}

		// 回転
		for (int i = 0; i < ((D3DX_PI * 2) / divide + 1); ++i)
		{
			D3DXVec3TransformCoord(&vec[i], &vec[i], &RotXMtx);
			v[i].x = vec[i].x + Translation.x;
			v[i].y = vec[i].y + Translation.y;
			v[i].z = vec[i].z + Translation.z;
			v[i].color = Color;
		}

		// 描画
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, v, sizeof(Vtx3D))))
			return false;

	}

	for (float phi = 0; phi < D3DX_PI; phi += divide)
	{
		//円
		int count = 0;
		for (float theta = 0; theta <= D3DX_PI * 2 + divide; theta += divide)
		{
			vec[count].x = 0.0f;
			vec[count].y = R * sinf(theta);
			vec[count].z = R * cosf(theta);
			count++;
		}
		D3DXMatrixRotationY(&RotYMtx, phi);

		//回転
		for (int i = 0; i < ((D3DX_PI * 2) / divide + 1); ++i)
		{
			D3DXVec3TransformCoord(&vec[i], &vec[i], &RotYMtx);
			v[i].x = vec[i].x + Translation.x;
			v[i].y = vec[i].y + Translation.y;
			v[i].z = vec[i].z + Translation.z;
			v[i].color = Color;
		}

		//描画
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, v, sizeof(Vtx3D))))
			return false;

	}


	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//---------------------------------------------
//	カプセル描画
//
//	引数：	pWorldMtx	ワールド変換行列
//			Color		色
//			R			球の半径			
//			Height		円柱の高さ
//
//	戻り値：	true:	描画成功
//			false:	描画失敗
//--------------------------------------------
bool Geometory::DrawCapsule(const D3DXMATRIX* pWorldMtx, DWORD Color, float R, float Height)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// デバイス取得
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	// エフェクト開始
	m_Effect.Begin();

	// 頂点構造体宣言セット	
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;


	// y軸回転行列
	D3DXMATRIX RotYMtx;
	D3DXMatrixIdentity(&RotYMtx);

	// 半球2個描画
	const int div = 8;
	float divide = D3DX_PI / div;
	D3DXVECTOR3 vec[div + 1];
	Vtx3D v[div + 1];
	int turn = 1;
	float phi = 0.0f;

	for (int k = 0; k < 2; ++k)
	{
		phi = 0.0f;
		for (int j = 0; j <= div; ++j)
		{
			// 半円
			float theta = 0;
			for (int i = 0; i <= div; ++i)
			{
				vec[i].x = 0.0f;
				vec[i].y = R * sinf(theta * turn);
				vec[i].z = R * cosf(theta * turn);
				theta += divide;
			}
			D3DXMatrixRotationY(&RotYMtx, phi);
			phi += divide;

			// 回転
			for (int i = 0; i <= div; ++i)
			{
				D3DXVec3TransformCoord(&vec[i], &vec[i], &RotYMtx);
				v[i].x = vec[i].x;
				v[i].y = vec[i].y + Height / 2.0f * turn;
				v[i].z = vec[i].z;
				v[i].color = Color;
			}

			// 描画
			if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div, v, sizeof(Vtx3D))))
				return false;

		}
		turn *= -1;
	}


	// 円柱描画

	// 円描画
	Vtx3D circlev[div * 2 + 1];
	turn = 1;
	for (int k = 0; k < 2; ++k)
	{
		phi = 0.0f;
		for (int i = 0; i <= div * 2; ++i)
		{
			circlev[i].x = R * sinf(phi);
			circlev[i].y = Height / 2.0f * turn;
			circlev[i].z = R * cosf(phi);
			circlev[i].color = Color;
			phi += divide;
		}
		// 描画
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div * 2, circlev, sizeof(Vtx3D))))
			return false;

		turn *= -1;
	}

	// 柱描画
	Vtx3D polev[div * 4];
	phi = 0.0f;
	for (int i = 0; i < div * 4; i += 2)
	{
		polev[i].x = R * sinf(phi);
		polev[i].y = Height / 2.0f;
		polev[i].z = R * cosf(phi);
		polev[i].color = Color;

		polev[i + 1].x = R * sinf(phi);
		polev[i + 1].y = -Height / 2.0f;
		polev[i + 1].z = R * cosf(phi);
		polev[i + 1].color = Color;

		phi += divide;
	}
	// 描画
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, div * 2, polev, sizeof(Vtx3D))))
		return false;

	// エフェクト終了
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

