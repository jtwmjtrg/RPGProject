#pragma once
#include "Skill.h"
#include "input.h"

class Enemy;

enum class eModeTerm : __int8 {
	None,		// とくになし
	Hp,			// ＨＰ判定
	Turn,		// ターン数判定
	EnemyNum,	// 敵の数
	BadStatus,	// 状態異常
	Buff,		// バフ
};

// モードの移行条件スーパークラス
class ModeTerm{
protected:
	vector<Skill*> skill;		// 保有スキル
public:
	eModeTerm type;		// 移行条件の種類

	ModeTerm();
	virtual ~ModeTerm() {};

	virtual bool Check() = 0;	// モードの移行条件判定

	void AddSkill(Skill* skillData);	// このモードのスキル追加
	Skill* GetSkill();	// 発動スキルの取得

	void Release();		// 終了処理

};


//////////////////////////////////////
////////////// 【なし】////////////////
//////////////////////////////////////
class ModeTerm_None : public ModeTerm {
private:
	const unsigned int& nowMode;	// 現在のモード
	const unsigned int& modeNum;	// モード番号
public:
	ModeTerm_None(const unsigned int& nowMode, const unsigned int& modeNum);
	~ModeTerm_None();

	bool Check();	// モードの移行条件判定
};

//////////////////////////////////////
//////////// 【ＨＰ判定】//////////////
//////////////////////////////////////
class ModeTerm_HP : public ModeTerm {
private:
	const int& hp;
	const int& max_hp;

	float rate;	// HP（n％未満)

public:
	ModeTerm_HP(string line, const int& hp, const int&  max_hp);
	~ModeTerm_HP();

	bool Check();	// モードの移行条件判定
};

//////////////////////////////////////
////////// 【ターン数判定】////////////
//////////////////////////////////////
class ModeTerm_Turn : public ModeTerm {
private:
	const unsigned int& turnNum;	// 現在のターン数
	unsigned int baseTurn;	// 基準ターン
	__int8 ope;		// 演算子

public:
	ModeTerm_Turn(string line, const unsigned int& turnNum);
	~ModeTerm_Turn();

	bool Check();	// モードの移行条件判定
};

//////////////////////////////////////
///////////// 【敵の数】///////////////
//////////////////////////////////////
class ModeTerm_EnemyNum : public ModeTerm {
private:
	const std::vector<Enemy*>& enemy;	// 敵データ
	unsigned int enemyNum;	// 敵の数
	eAlive AliveType;	// 対象
	__int8 ope;		// 演算子

public:
	ModeTerm_EnemyNum(string line, const std::vector<Enemy*>& enemy);
	~ModeTerm_EnemyNum();

	bool Check();	// モードの移行条件判定
};

//////////////////////////////////////
//////////// 【状態異常】//////////////
//////////////////////////////////////
class ModeTerm_BadStatus : public ModeTerm {
private:
	const std::array<tBadStatus, 9>& badStatus;		// 状態異常の情報
	eBadStatus  bs_Type;	// 判定対象

public:
	ModeTerm_BadStatus(string line, const std::array<tBadStatus, 9>& badStatus);
	~ModeTerm_BadStatus();

	bool Check();	// モードの移行条件判定
};