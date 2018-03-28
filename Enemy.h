#pragma once
#include "Character.h"
#include "ModeTerm.h"
#include "sounddata.h"

// 死亡した敵のIDと座標
struct tEnemyGrave {
	int id;
	int x;
	int y;
};

class Enemy : public Character {
private:
	int gr;		// 画像
	int id;		// 敵ID
	int width;	// 横幅
	int height;	// 縦幅
	int x, y;	// 座標

	static int gr_HP;	// HPバー

	vector<ModeTerm*> modeTerm;		// 保有スキル
	unsigned int nowMode;			// 現在のモード


	// 【戦利品】
	unsigned int exp;	// 経験値
	unsigned int money;	// 金
	int dropItem;	// ドロップアイテムID
	__int8 dropRate;	// ドロップ率

public:
	Enemy(int x, int y, string name);
	Enemy(int x, int y, int id, unsigned int& turnNum);	// コンストラクタ
	~Enemy();
	
	static void InitBattle();	// 初期化処理
	static void FinaBattle();	// 終了処理

	void LoadData(const unsigned int& turnNum, const std::vector<Enemy*>& enemy);

	static vector<unsigned int> deathList;	// 敵死亡リスト

	void Draw(int fieldX, int fieldY, int cellsize);	// 描画

	void DecisionSkill(vector<int> playerHate, vector<Enemy*> enemy);	// 発動スキルの決定

	void ModeChange(int modeNum);	// モード変更

	void SetColision(vector<vector<Character*>>& collField);	// 当たり判定の作成
	void DeleteColision(vector<vector<Character*>>& collField);	// 死亡処理
	void Release();		// 終了処理

	bool GetCharType() { return false; }	// キャラクターのタイプ取得	プレイヤー：true	敵：false
	void GetEffectText(vector<string>& message);	// スキルのメッセージ取得
	int GetID();	// ID取得
	int GetWidth();		// 横幅
	int GetHeight();	// 縦幅
	int GetX();		// X座標
	int GetY();		// Y座標
	

	// 戦利品
	unsigned int GetEXP();		// 経験値
	unsigned int GetMoney();	// 金
	int GetDropItem();			// ドロップアイテム
	__int8 GetDropRate();		// ドロップ率

	void TestIni(string name);
};