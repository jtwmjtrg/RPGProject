#pragma once
#include "Skill.h"
#include "input.h"

enum class eTermSkill : __int8 {
	Always,		// 毎ターン発動
	Hp,			// ＨＰ判定
	Turn,		// ターン数判定
	EnemyNum,	// 敵の数
	BadStatus,	// 状態異常
	Buff,		// バフ
};

class SkillOrder;
class Character;

// 条件発動スキルスーパークラス
class TermSkill {
protected:
	vector<Skill*> skill;		// 保有スキル
	bool velocity;	// true:ターンの最初に発動	false:ターンの最後
public:
	eTermSkill type;	// 発動条件

	TermSkill();
	virtual ~TermSkill() {};

	virtual void Check(Character* character, SkillOrder *skillOrder) = 0;	// モードの移行条件判定

	void AddSkill(Skill* skillData);	// スキル追加
	void SkillExecution(Character* attacker, SkillOrder *skillOrder);	// スキル発動

	void Release();		// 終了処理
};

//////////////////////////////////////
////////// 【毎ターン発動】////////////
//////////////////////////////////////
class TermSkill_Always : public TermSkill {
public:
	TermSkill_Always(string line);
	TermSkill_Always(bool velocity);
	~TermSkill_Always();

	void Check(Character* attacker, SkillOrder *skillOrder);	// モードの移行条件判定
};