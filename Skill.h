#pragma once
#include<memory>
#include <string>
#include <vector>
#include <array>
#define _USE_MATH_DEFINES	// M_PIが使えるように
#include <math.h>		// 三角比
#include <boost/lexical_cast.hpp>
#include "sounddata.h"

// ファイル読み込み
#include <fstream>
#include <sstream>

#include "TextBox.h"
#include "Equipment.h"

using namespace std;

enum class eAlive : __int8 {
	alive,		// 生存
	dead,		// 死亡
	extinction,	// 消滅
};

// スキルの種類
enum class eSkillType : __int8{
	Nomal,		// 普通のスキル
	Confusion,	// 混乱時の攻撃用スキル
	None,		//「なにもしない」スキル
	Defence,	// 防御
	Escape,		// 逃げる
};

// スキルエフェクトの種類
enum class eSkillEffect : __int8 {
	Attack,		// 攻撃
	Heal,		// 回復
	BadStatus,	// 状態異常
	Buff,		// バフ、デバフ
	AnotherSkill,	// 他スキル発動
	Resurrection,	// 蘇生
	Hate,			// ヘイト増減
	Charge,			// チャージ
	Mode,			// モード変更
	MoveMP,			// 魔素の移動
};

// エネルギーの種類
enum eEnergy {
	None = -1, // 変換なし
	Heat,	// 熱
	Elec,	// 電気
	Shine,	// 光
	Sound,	// 音
	Motion,	// 運動
	Gravity,// 重力
	Life,	// 生命
	Static,	// 静止
	Nucleus,// 核
};

// エネルギー変換の際に使用する魔素、エネルギーの対象
enum class eConvType : __int8 {
	Self,	// 自分の保有魔素、エネルギー
	Field,	// フィールドの魔素、エネルギー
	Target,	// 相手
	None,	// エネルギー変換以外の現象（発電器官とか）
};

enum class eSkillTarget : __int8 {
	Self,	// 自分
	Enemy,	// 敵全体
	Ally,	// 味方全体
	Overall,// 敵味方全体
	Select,	// 選択範囲
	Previous,// 引き継ぎ
};

// エネルギー変換
struct tEnergyConv {
	eEnergy type;	// エネルギーの種類
	eConvType convType;	// 変換する魔素の対象
	bool direction;	// 変換方向（1:+　0:-）
	double rate;	// 変換割合（必要なMUE値のn%、変換対象が対象なしの場合は変換量）
	__int8 correction;	// 補正
};

// ステータス
enum class eStatus : __int8 {
	HP,
	maxHP,
	MP,
	maxMP,
	STR,
	DEX,
	AGI,
	LUC,
	DEF_PHYS,
	DEF_HEAT,
	DEF_COLD,
	DEF_ELEC,
	MUE
};

// 回復タイプ
enum class eHealType : __int8 {
	HP,		// HP
	MP,		// MP
	BadStatus,	// 全状態異常
	Poison,		// 毒
	Paralysis,	// 麻痺
	Sleep,		// 睡眠
	Confusion,	// 混乱
	Blind,		// 暗闇
	Weakness,	// 衰弱
	Debuff	// 全デバフ
};

// バフ,デバフ用
enum class eBuff :__int8 {
	One_any = -1,	// どれか一つ
	// 【ステータス】
	HP,		// 体力
	maxHP,	// 最大体力
	MP,		// マジックパワー
	maxMP,	// 最大MP
	STR,
	DEX,
	AGI,
	LUC,
	DEF_PHYS,
	DEF_HEAT,
	DEF_COLD,
	DEF_ELEC,
	MUE,
	// 【状態異常】
	Poison,		// 毒
	Paralysis,	// 麻痺
	Sleep,		// 睡眠
	confusion,	// 混乱
	Blind,		// 暗闇
	Weakness,	// 衰弱
	Stun,		// スタン
	Death,		// 即死
	Extinction, // 消滅
};

// バフデータ
struct tBuff {
	float rate;	// 倍率
	int trun;	// 持続ターン数
};
// 状態異常
enum class eBadStatus : __int8 {
	One_any = -1,	// どれか一つ
	Poison,		// 毒
	Paralysis,	// 麻痺
	Sleep,		// 睡眠
	Confusion,	// 混乱
	Blind,		// 暗闇
	Weakness,	// 衰弱
	Stun,		// スタン
	Death,		// 即死
	Extinction, // 消滅
};
struct tBadStatus {
	bool get;	// 状態異常ならtrue
	int turn;	// 状態異常になってから経過したターン
	unsigned int power;	// 状態異常の強さ
};

enum class eCharge : __int8 {
	Damage,		// ダメージ倍率
	Defence,	// 防御倍率
	MUE,		// MUE
	Avoid,		// 回避率
	Hit,		// 命中率
	Range,		// 範囲拡大
	Speed,		// 行動速度
	Continuing,	// 連続行動
};
class SkillEffect;
class Skill;
class Character;
class PlayerChara;

// 構造体
// 【攻撃】
struct tSAttack {
	float damageRate;	// ダメージ倍率
	float hit;			// 命中補正
	unsigned __int8 times_min;	// 最小行動回数
	unsigned __int8 times_max;	// 最大行動回数
};
// 【回復】
struct tHeal {
	eHealType type;	// 回復タイプ
	float rate;	// 倍率
};
// 【状態異常】
struct tBadStatus_Skill {
	eBadStatus type;	// 種類
	int hit;			// 命中力
	unsigned int power;	// 威力
};
// 【バフ】
struct tBuff_Skill {
	eBuff type;		// バフの種類
	float hit;	// 命中
	bool dir;	// true:バフ	false:デバフ
	unsigned __int8 trun;	// 持続ターン数
};
// 【他のスキル発動】
struct tAnotherSkill {
	string skillID;	// スキルID
	Skill* skill;	// 発動するスキルデータ
	__int8 invocation;	// 発動率
	bool fastFlag;	// 速攻フラグ
};
// 蘇生
struct tResurrection {
	__int8 success;	// 成功率
	float rate;		// 倍率
};
// ヘイト増減
struct tHate {
	int value;	// 増減値
	float rate;	// 増減倍率
};
// チャージ
struct tCharge {
	eCharge type;	// 種類
	float rate;		// 倍率
	int turn;		// 発動ターン数
};
// モード変更
struct tModeChange {
	int modeNum;	// モード番号
};

// 【効果の変換量を得る関数】
double GetConvValue(const tEnergyConv energy, int totalUseEnergy);

// 【スキルの効果スーパークラス】
class SkillEffect {
protected:
	int totalUseEnergy;
public:
	eSkillEffect	type;
	tEnergyConv		energyConv;					// エネルギー変換
	eSkillTarget	targetType;					// 攻撃対象
	std::vector<std::vector<__int8>> collision;	// 当たり判定
	
	SkillEffect() {};
	SkillEffect(int totalUseEnergy) : totalUseEnergy(totalUseEnergy){}
	virtual ~SkillEffect() {}

	virtual void Execution_Field(Character* attacker, Character* target) {}	// 効果発動（フィールド用）

	virtual const tSAttack*				GetAttack()			{ return NULL; }	// 攻撃
	virtual const tHeal*				GetHeal()			{ return NULL; }	// 回復
	virtual const tBadStatus_Skill*		GetBadStatus()		{ return NULL; }	// 状態異常
	virtual const tBuff_Skill*			GetBuff()			{ return NULL; }	// バフ-デバフ
	virtual const tAnotherSkill*		GetAnotherSkill()	{ return NULL; }	// 他のスキル発動
	virtual const tResurrection*		GetResurrection()	{ return NULL; }	// 蘇生
	virtual const tHate*				GetHate()			{ return NULL; }	// ヘイト増減
	virtual const tCharge*				GetCharge()			{ return NULL; }	// チャージ
	virtual const tModeChange*			GetMode()			{ return NULL; }	// モード変更
};

//////////////////////////////////////
////////////// 【攻撃】////////////////
//////////////////////////////////////
class SkillEffect_Attack : public SkillEffect {
private:
	tSAttack attackData;
public:
	SkillEffect_Attack();	// 混乱時の攻撃用初期化
	SkillEffect_Attack(string line, int totalUseEnergy);
	~SkillEffect_Attack();

	//void Execution_Field(Character* attacker, Character* target);	// 効果発動（フィールド用）

	// debug
	void TestIni();
	const tSAttack* GetAttack();
};


//////////////////////////////////////
////////////// 【回復】////////////////
//////////////////////////////////////
class SkillEffect_Heal : public SkillEffect {
private:
	tHeal healData;
public:
	SkillEffect_Heal();
	SkillEffect_Heal(string line, int totalUseEnergy);
	~SkillEffect_Heal();

	void Execution_Field(Character* attacker, Character* target);	// 効果発動（フィールド用）

	const tHeal* GetHeal();
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// 【状態異常】//////////////
//////////////////////////////////////
class SkillEffect_BadStatus : public SkillEffect {
private:
	tBadStatus_Skill badStatusData;
public:
	SkillEffect_BadStatus();
	SkillEffect_BadStatus(string line, int totalUseEnergy);
	SkillEffect_BadStatus(tBadStatus_Skill badStatusData);
	~SkillEffect_BadStatus();

	const tBadStatus_Skill* GetBadStatus();
};

//////////////////////////////////////
/////////// 【バフ-デバフ】////////////
//////////////////////////////////////
class SkillEffect_Buff : public SkillEffect {
private:
	tBuff_Skill buffData;
public:
	SkillEffect_Buff();
	SkillEffect_Buff(string line, int totalUseEnergy);
	SkillEffect_Buff(tBuff_Skill buffData, tEnergyConv energyConv);
	~SkillEffect_Buff();

	const tBuff_Skill* GetBuff();
};

//////////////////////////////////////
////////// 【他のスキル発動】//////////
//////////////////////////////////////
class SkillEffect_AnotherSkill : public SkillEffect {
private:
	tAnotherSkill anotherSkillData;
public:
	SkillEffect_AnotherSkill();
	SkillEffect_AnotherSkill(string line, int totalUseEnergy);
	~SkillEffect_AnotherSkill();

	const tAnotherSkill* GetAnotherSkill();
	// debug
	void TestIni();
};

//////////////////////////////////////
/////////////// 【蘇生】///////////////
//////////////////////////////////////
class SkillEffect_Resurrection : public SkillEffect {
private:
	tResurrection resurrectionData;
public:
	SkillEffect_Resurrection();
	SkillEffect_Resurrection(string line, int totalUseEnergy);
	~SkillEffect_Resurrection();

	const tResurrection* GetResurrection();
	void Execution_Field(Character* attacker, Character* target);	// 効果発動（フィールド用）
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// 【ヘイト増減】////////////
//////////////////////////////////////
class SkillEffect_Hate : public SkillEffect {
private:
	tHate hateData;
public:
	SkillEffect_Hate();
	SkillEffect_Hate(string line, int totalUseEnergy);
	~SkillEffect_Hate();

	const tHate* GetHate();
	// debug
	void TestIni();
};

//////////////////////////////////////
///////////// 【チャージ】/////////////
//////////////////////////////////////
class SkillEffect_Charge : public SkillEffect {
private:
	tCharge chargeData;
public:
	SkillEffect_Charge();
	SkillEffect_Charge(string line, int totalUseEnergy);
	~SkillEffect_Charge();

	const tCharge* GetCharge();
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// 【モード変更】////////////
//////////////////////////////////////
class SkillEffect_Mode : public SkillEffect {
private:
	tModeChange modeData;
public:
	SkillEffect_Mode();
	SkillEffect_Mode(string line, int totalUseEnergy);
	~SkillEffect_Mode();

	const tModeChange* GetMode();
	// debug
	void TestIni();
};

// スキルの画像エフェクト
struct tSkillGrEffect {
	bool flag;		// エフェクトの有無
	int gr;
	unsigned __int8 divX;		// 分割数X
	unsigned __int8 divY;		// 　　　Y
	unsigned short divNum;		// 総分割数
};

// 【スキルのメインクラス】
class Skill {
private:

public:
	Skill();
	Skill(int id);
	Skill(string path);
	~Skill();

	void LoadData(string path);		// スキルデータ読み込み

	eSkillType type;	// スキルの種類
	unsigned int id;	// スキルのID
	string name;		// スキル名
	float speed;		// 速度補正
	bool selectFlag;	// 攻撃範囲選択フラグ
	bool selectTarget;	// 攻撃範囲選択対象		F:敵	T:味方
	unsigned __int8 width;			// 攻撃範囲 横幅
	unsigned __int8 height;			//          縦幅
	std::array< unsigned int, 3> useMP;	// 使用MP
	int totalUseEnergy;	// 必要なMUE値
	bool battleFlag;	// 戦闘中使用フラグ
	bool fieldFlag;		// フィールド使用フラグ
	eWeaponType weaponRestriction;		// 武器制限
	
	std::vector<std::string> explanatoryText;	// 説明文
	std::vector<std::string> utilizationText;	// スキル使用のテキスト

	tSkillGrEffect grEffect;	// 画像エフェクト
	int so_SE;		// SE

	void PlayerSE();	// SEをならす

	__int8 x, y;	// 座標（攻撃時に決定する）
	vector<SkillEffect*> effect;		// スキル効果
	vector<vector<__int8>> collision;	// 当たり判定

	void Execution_Field(Character* attacker, std::vector<PlayerChara*> playerList, Character* target = nullptr);	// スキル発動（フィールド用）
	void Release();		// 終了処理

	// debug
	void TestIni();
};

// 【混乱時の行動用スキル】
class Skill_Confusion : public Skill{
private:

public:
	Skill_Confusion(int arWidth, int arHeight, vector<vector<__int8>> arCollision);
	~Skill_Confusion();
};

// 【なにもしない】
class Skill_None : public Skill {
private:

public:
	Skill_None();
	~Skill_None();
};

// 【防御】
class Skill_Defence : public Skill {
private:

public:
	Skill_Defence();
	~Skill_Defence();
};

// 【逃走】
class Skill_Escape : public Skill {
private:

public:
	Skill_Escape();
	~Skill_Escape();
};


