#pragma once
#include "Game.h"
#include "Skill.h"
#include "Effect3D.h"
#include "Camera.h"

class PlayerChara;

struct tSkillCell {
	int x;
	int y;
	Skill* skill;		// 取得スキル
	int cost;			// 取得コスト
	bool getFlag;		// 取得フラグ
	std::vector<string> premiseSkillID;	// 前提スキルID
};

class Effect_Cube : Object3D {
public:
	// 3次元頂点構造体
	struct Vtx3D {
		float	x, y, z;
		DWORD	color;
	};

	// 描画パス
	enum {
		PASS_DRAW2D,
		PASS_DRAW3D
	};

	D3DVERTEXELEMENT9 decl3d[3];
	IDirect3DDevice9* pD3DDevice;
	Vtx3D	v[9][16];
	Vtx3D	v_ini[9][16];
	float thetaX[9];
	float thetaY[9];
	float thetaZ[9];
	float scale;
	int count;

	D3DXVECTOR3 core;	// 中心座標

	static Effect3D						m_Effect;		// 幾何オブジェクトエフェクト
	static IDirect3DVertexDeclaration9*	m_pVertexDecl3D;// 3次元幾何オブジェクト頂点構造体宣言

	Effect_Cube(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max);
	~Effect_Cube();

	static void Release();
	static bool SetEffectParam(const D3DXMATRIX* pWorldMtx);

	void Draw();
	void InitStatus();
};

class Effect_SkillTree {
private:
	int count;

	Effect_Cube* effect;

public:
	bool flag;

	Effect_SkillTree();
	~Effect_SkillTree();

	void Draw();
	void Set();
};

// 【スキルツリークラス】
class SkillTree : public Game {
private:
	// 【画像】
	int gr_back;		// 背景
	int gr_select;		// 選択カーソル
	int gr_get;			// 取得済みスキル

	// 【音】
	int so_BGM;		// BGM
	int so_get;		// スキル取得

	static PlayerChara *chara;		// キャラクタ
	std::map<std::string, tSkillCell> skills;
	std::vector<std::vector<std::string>> cell;

	int selectX;	// 選択座標
	int selectY;

	int scrollX;	// スクロール
	int scrollY;

	// 【エフェクト】
	Effect_SkillTree* effect;
	
public:
	SkillTree();
	~SkillTree();

	void Process();
	void Draw();

	static void SetData(PlayerChara *chara);
	bool CheckPremiseSkill(tSkillCell* skill);		// 前提スキルのチェック
	
	bool LoadData();	// スキルツリーのロード
	void SaveData();	// スキルツリーのセーブ
};