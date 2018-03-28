#pragma once
#include "direct3d.h"
#include "d3dx9effect.h"
#include <map>
#include <vector>

// 3Dエフェクト
class Effect3D{
private:
	ID3DXEffect*											pEffect;			// エフェクトポインタ
	std::map<std::basic_string<TCHAR>, D3DXHANDLE>			hTechniques;		// テクニックハンドル
	std::map<std::basic_string<TCHAR>, D3DXHANDLE>			hParams;			// シェーダグローバル変数ハンドル
	std::map<std::basic_string<TCHAR>, D3DXTECHNIQUE_DESC>	Techniques;		// テクニック
	std::map<std::basic_string<TCHAR>, D3DXPARAMETER_DESC>	Params;			// シェーダグローバル変数
	std::basic_string<TCHAR>								Name;				// エフェクト名
	int														ID;				// エフェクトリソースID
	D3DXHANDLE												hCurrentTechnique;// 現在のテクニック
	UINT													CurrentPass;		// 現在のパス
	
	// テクニック情報取得
	void GetTechniques(); 
	// シェーダ変数情報取得
	void GetParams();
public:
	Effect3D();
	~Effect3D();
	
	// ファイルからエフェクト作成
	bool Create(IDirect3DDevice9* pD3DDevice, const TCHAR* FXFileName);
	// リソースからエフェクト作成
	bool Create(IDirect3DDevice9* pD3DDevice, int ResourceID);

	// エフェクトポインタ取得
	ID3DXEffect* GetEffect();

	// シェーダグローバル変数名チェック
	bool IsParamName(const TCHAR* ParamName);
	// 行列セット
	bool SetMatrix(const TCHAR* ParamName, const D3DXMATRIX* pMtx);
	// 行列パレットセット
	bool SetMatrixArray(const TCHAR* ParamName, const D3DXMATRIX* pMtxArray, int Count);
	// 4次元ベクトルセット
	bool SetVector4(const TCHAR* ParamName, const D3DXVECTOR4* pVec4);
	// 3次元ベクトルセット
	bool SetVector3(const TCHAR* ParamName, const D3DXVECTOR3* pVec3);
	// 浮動小数セット
	bool SetFloat(const TCHAR* ParamName, float Param);
	// 整数のセット
	bool SetInt(const TCHAR* ParamName, int Param);
	// テクスチャセット
	bool SetTexture(const TCHAR* TextureName, IDirect3DBaseTexture9* pTexture);

	// テクニックのセット
	bool SetTechnique(const TCHAR* TechniqueName, UINT Pass = 0);
	// エフェクト開始
	bool Begin();
	// エフェクト終了
	bool End();
};


// ３次元オブジェクトクラス
class Object3D
{
protected:
	// オブジェクトの正面方向（向いてる向き）
	D3DXVECTOR3 m_Dir;
	// スケール、平行移動
	D3DXVECTOR3 m_Scale, m_Translate;
	// 回転行列
	D3DXMATRIX	m_RotateMtx;
	// ワールド変換行列
	D3DXMATRIX	m_WorldMtx;
	// ビュー変換行列
	static D3DXMATRIX m_ViewMtx;
	// 射影変換行列
	static D3DXMATRIX m_ProjMtx;
public:

	//-------------------------//
	// コンストラクタ
	//-------------------------//
	Object3D();
	//-------------------------//
	// デストラクタ
	//-------------------------//
	virtual ~Object3D();

	//-------------------------------------------//
	//	ワールド変換行列をセット
	//
	//	引数：	WorldMtx	ワールド変換行列
	//-------------------------------------------//
	void SetWorldMatrix(const D3DXMATRIX& WorldMtx);
	//--------------------------------------------//
	//	ワールド変換行列を取得する
	//
	//	戻り値：ワールド変換行列
	//--------------------------------------------//
	const D3DXMATRIX& GetWorldMatrix() const;
	//--------------------------------------------//
	//	ワールド変換行列を作成する
	//--------------------------------------------//
	virtual void CreateWorldMatrix();

	//-------------------------------------------//
	//	ビュー変換行列をセット
	//
	//	引数：ViewMtx ビュー変換行列
	//-------------------------------------------//
	static void SetViewMtx(const D3DXMATRIX& ViewMtx);
	//--------------------------------------------//
	//	ビュー変換行列を取得する
	//
	//	戻り値：ビュー変換行列
	//--------------------------------------------//
	static const D3DXMATRIX& GetViewMatrix();

	//-------------------------------------------//
	//	射影変換行列をセット
	//
	//	引数：ProjMtx 射影変換行列
	//-------------------------------------------//
	static void SetProjMtx(const D3DXMATRIX& ProjMtx);
	//--------------------------------------------//
	//	射影変換行列を取得する
	//
	//	戻り値：射影変換行列
	//--------------------------------------------//
	static const D3DXMATRIX& GetProjMatrix();

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	スケーリングをセットする
	//
	//	引数：Scale	スケーリング
	//-----------------------------------//
	void SetScale(const D3DXVECTOR3& Scale);
	//-----------------------------------//
	//	スケーリングをセットする
	//
	//	引数：x,y,z	各軸に対するスケーリング
	//-----------------------------------//
	void SetScale(float x, float y, float z);
	//-----------------------------------//
	//	回転をセットする
	//
	//	引数：Rotate 回転
	//-----------------------------------//
	void SetRotate(const D3DXVECTOR3& Rotate);
	//-----------------------------------//
	//	回転をセットする
	//
	//	引数：x,y,z	各軸に対する回転
	//-----------------------------------//
	void SetRotate(float x, float y, float z);
	//-----------------------------------//
	//	平行移動をセットする
	//
	//	引数：Translate 平行移動
	//-----------------------------------//
	void SetTranslate(const D3DXVECTOR3& Translate);
	//-----------------------------------//
	//	平行移動をセットする
	//
	//	引数：x,y,z	各軸に対する平行移動
	//-----------------------------------//
	void SetTranslate(float x, float y, float z);

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	スケーリングを更新する
	//
	//	引数：Scale	スケーリング
	//-----------------------------------//
	void UpdateScale(const D3DXVECTOR3& Scale);
	//-----------------------------------//
	//	スケーリングを更新する
	//
	//	引数：x,y,z	各軸に対するスケーリング
	//-----------------------------------//
	void UpdateScale(float x, float y, float z);
	//-----------------------------------//
	//	回転を更新する
	//
	//	引数：Rotate 回転
	//-----------------------------------//
	void UpdateRotate(const D3DXVECTOR3& Rotate);
	//-----------------------------------//
	//	回転を更新する
	//
	//	引数：x,y,z	各軸に対する回転
	//-----------------------------------//
	void UpdateRotate(float x, float y, float z);
	//-----------------------------------//
	//	平行移動を更新する
	//
	//	引数：Translate 平行移動
	//-----------------------------------//
	void UpdateTranslate(const D3DXVECTOR3& Translate);
	//-----------------------------------//
	//	平行移動を更新する
	//
	//	引数：x,y,z	各軸に対する平行移動
	//-----------------------------------//
	void UpdateTranslate(float x, float y, float z);

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	スケーリングを取得する
	//
	//	戻り値：スケーリング
	//-----------------------------------//
	const D3DXVECTOR3& GetScale() const;
	//-----------------------------------//
	//	回転を取得する
	//
	//	戻り値：回転行列
	//-----------------------------------//
	const D3DXMATRIX& GetRotateMatrix() const;
	//-----------------------------------//
	//	平行移動を取得する
	//
	//	戻り値：平行移動
	//-----------------------------------//
	const D3DXVECTOR3& GetTranslate() const;

	//////////////////////////////////////////////////////////////////

	//--------------------------------------------//
	//	モデルの正面向きセット
	//
	//	引数：Dir 正面向き
	//--------------------------------------------//
	void SetDir(const D3DXVECTOR3& Dir);

	//--------------------------------------------//
	//	モデルの正面向き取得
	//
	//	戻り値：正面向き
	//--------------------------------------------//
	D3DXVECTOR3 GetDir();

};



// 幾何オブジェクトクラス
class Geometory
{
private:
	// 2次元頂点構造体
	struct Vtx2D {
		float	x, y, z;
		float	rhw;
		int		color;
	};
	// 3次元頂点構造体
	struct Vtx3D {
		float	x, y, z;
		int		color;
	};

	// 描画パス
	enum {
		PASS_DRAW2D,
		PASS_DRAW3D
	};


	static Effect3D						m_Effect;		// 幾何オブジェクトエフェクト
	static IDirect3DVertexDeclaration9*	m_pVertexDecl2D;// 2次元幾何オブジェクト頂点構造体宣言
	static IDirect3DVertexDeclaration9*	m_pVertexDecl3D;// 3次元幾何オブジェクト頂点構造体宣言

														// コンストラクタ
	Geometory() {}
	// コピーコンストラクタ
	Geometory(const Geometory& obj) {}
	// 代入演算子
	Geometory& operator=(const Geometory& obj) {}

public:
	// 初期化
	static bool Init(IDirect3DDevice9* pD3DDevice);
	// 解放
	static void Release();

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
	//	戻り値：	true:	描画成功
	//			false:	描画失敗
	//-------------------------------------------//
	static bool DrawLine(const D3DXVECTOR2& Start, const D3DXVECTOR2& End, DWORD Color = 0xffff0000);

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
	static bool DrawCircle(float R, const D3DXVECTOR2& Pos, DWORD Color = 0xffff0000);

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
	static bool DrawFillCircle(float R, const D3DXVECTOR2& Pos, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	矩形線描画
	//
	//	引数：	Rect		矩形
	//			Color		色
	//
	//	戻り値：true:	描画成功
	//			false:	描画失敗
	//-------------------------------------------//
	static bool DrawRect(const RECT& Rect, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	矩形描画
	//
	//	引数：	Rect		矩形
	//			Color		色
	//
	//	戻り値：true:	描画成功
	//			false:	描画失敗
	//-------------------------------------------//
	static bool DrawFillRect(const RECT& Rect, DWORD Color = 0xffff0000);

	//////////////////////////////////////////////////////////////////////////////////////////
	//								3次元オブジェクト描画										//
	//////////////////////////////////////////////////////////////////////////////////////////

private:
	//------------------------------------------//
	//	エフェクトに変換行列セット
	//	
	//	引数	：	pWorldMtx	ワールド変換行列
	//
	//	戻り値：	true:	セット成功
	//			false:	セット失敗
	//------------------------------------------//
	static bool SetEffectParam(const D3DXMATRIX* pWorldMtx);

public:

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
	static bool DrawGrid(float Wide, int Divide = 5, const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffffffff);

	//-----------------------------------------------
	//	3軸描画
	//
	//	引数：	Length		軸の長さ
	//			pWorldMtx	ワールド変換行列
	//
	//	戻り値：	true:	描画成功
	//			false:	描画失敗
	//--------------------------------------------
	static bool DrawAxis(float Length, const D3DXMATRIX* pWorldMtx = NULL);

	//-----------------------------------------------
	//	直線描画
	//
	//	引数：	Start		始点
	//			End			終点
	//			pWorldMtx	ワールド変換行列
	//			Color		色
	//
	//	戻り値：true:	描画成功
	//			false:	描画失敗
	//-----------------------------------------------
	static bool DrawLine(const D3DXVECTOR3& Start, const D3DXVECTOR3& End, const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);

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
	static bool DrawTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);

	//--------------------------------------------
	//	三角形を描画
	//
	//	引数：	Vtx			三角形の頂点
	//			pWorldMtx	ワールド変換行列
	//			Color		色
	//
	//	戻り値：true:	描画成功
	//			false:	描画失敗
	//--------------------------------------------
	static bool DrawFillTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);


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
	static bool DrawCube(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, const D3DXVECTOR3 &Min = D3DXVECTOR3(-0.5f, -0.5f, -0.5f), const D3DXVECTOR3 &Max = D3DXVECTOR3(0.5f, 0.5f, 0.5f));

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
	static bool DrawSphere(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, const D3DXVECTOR3 &Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f), float R = 1.0f);

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
	static bool DrawCapsule(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, float R = 1.0f, float Height = 2.0f);
};


