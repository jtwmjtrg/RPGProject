#pragma once
#include <array>
#include <fstream>
#include <sstream>
#include "graphicdata.h"
#include "input.h"
#include "BattleStatus.h"
#include "Equipment.h"
#include "Item.h"
#include "Skill.h"
#include "TermSkill.h"

#include <boost/algorithm/string/split.hpp>		// スプリット
#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string.hpp>

enum class BuffEffect : __int8 {
	Negate, // 打ち消す
	Buff,	// バフ
	deBuff,	// デバフ
};

class Character;
class EquipmentItem;
class Battle;

// 行動順
struct tSkillOrder {
	Character* attacker;	// スキルの使用者
	Skill* skill;	// 使用スキル
	double speed;	// 速度
	int totalUseEnergy;	// スキルの発動に必要なMUE値
};

class SkillOrder {
private:
	double maxSpeed;	// 最速行動の速度s

public:
	SkillOrder();
	~SkillOrder();
	
	vector<tSkillOrder> order;	// オーダー

	void Add(Character* attacker, Skill* skill, int totalUseEnergy);	// スキルをスキルオーダーに追加
	void Add_Velocity(Character* attacker, Skill* skill, int totalUseEnergy, bool velocity);	// 最速/最遅スキル
	void Pop_Back();	// 最後尾をポップする

	Character* GetAttacker();
	Skill* GetSkill();
	tSkillOrder GetOrder();
};

class Character	// 抽象クラス
{
protected:
	// 【画像】
	static array<int, 7> gr_BadStatus;		// 状態異常

	// 【音】
	static int se_Damage;		// ダメージse

	// 不変ステータス
	string					name;			// 名前

	BattleStatus			battlestatus;	// 戦闘ステータス
	array<int,9>			DEF_badStatus;	// 状態異常耐性
	array<int,9>			EBB_badStatus;	// 状態異常の蓄積値
	array<tBadStatus,9>		badStatus;		// 状態異常の情報
	array<tBuff,22>			buffData;		// バフ,デバフ
	eAlive					aliveState;		// 生存状態
	bool					defenceFlag;	// 防御状態フラグ
	array<array<float,9>,2> efficiency;		// エネルギー変換効率
	array<tCharge, 8> chargeData;			// チャージ

	vector<vector<__int8>>	collision;	// 当たり判定

	SkillOrder *skillOrder;		// スキルオーダー
	Battle* battle;				// 戦闘クラス

	std::vector<TermSkill*> termSkill;		// 条件発動スキル

	// 【特殊行動】
	Skill* nomalAttack;		// 通常攻撃
	Skill* confusionSkill;	// 混乱時の攻撃用スキル

	// 【スキルの対象になった時に保持するもの】
	std::array<bool, 6> receiveSkill;	// スキルの対象になったらtrue
	unsigned int skillDamage;		// スキルで受けたダメージ
	bool skillDeath;				// スキルで死んだらtrue
	std::array<bool, 10> skillHealType;	// うけた回復のタイプ
	int skillHeal_HP;			// スキルで回復した量
	int skillHeal_MP;
	std::array<bool, 9> skillBadStatus;	// うけた状態異常の種類
	std::array<bool, 22> skillBuff;		// バフ
	std::array<BuffEffect, 22> skillBuffType;// バフの効果タイプ
	bool skillResurrection;			// 蘇生
	

	unsigned __int8 damageDelay;	// ダメージエフェクトのカウント
	unsigned __int8 deathDelay;		// 死亡エフェクトのカウント


	// アクセス制限されたコンストラクタ
	Character();

	EquipmentItem* weapon;	// 武器
	EquipmentItem* armor;	// 防具
	EquipmentItem* amulet;	// その他

public:
	// デストラクタ
	virtual ~Character();
	
	static void Init();		// 初期化
	static void Fina();		// 終了処理
	void SetPointer(SkillOrder *skillOrder, Battle* battle);	// ポインタセット
	static void CommonInitBattle();	// 戦闘共通初期化処理
	static void CommonFinaBattle();	// 終了処理

	virtual void SetColision(vector<vector<Character*>>& collField) = 0;	// 当たり判定の作成
	virtual void DeleteColision(vector<vector<Character*>>& collField) = 0;	// 当たり判定の削除

	// エフェクト
	int Recovery(double value);		// HP回復
	int Recovery_MP(double value);	// MP回復
	int FullRecovery();				// HP全回復
	int FullRecovery_MP();			// MP全回復
	int		Damage(double value);	// HPダメージ
	int PhysDamage(double value);	// 物理ダメージ
	int HeatDamage(double value);	// 熱ダメージ
	int ColdDamage(double value);	// 冷気ダメージ
	int ElecDamage(double value);	// 電気ダメージ
	int Resurrection(double value);	// 蘇生
	int FullResurrection();			// 完全蘇生

	void IncreaseMP(int value);	// MP増加
	void DecreaseMP(int value);	// MP減少

	void GiveBadStatus(eBadStatus type, unsigned int power);	// 状態異常を与える
	void GiveEddDEF_BadStatus(eBadStatus type, int value);	// 状態異常の蓄積値
	void HealBadStatus();	// 全状態異常回復
	void HealBadStatus(eBadStatus type);	// 状態異常回復（１つのみ）
	void GiveBuff(eBuff type, float rate, int turn);	// バフを与える
	void Defence();		// 防御状態にする
	void SetCharge(eCharge type, float rate, int turn);	// チャージのセット
	virtual void ModeChange(int modeNum) {}	// モード変更

	void StateReset();	// 状態初期化
	void TurnStartProcess();	// ターン開始処理
	void TurnEndProcess();		// ターン終了時の処理
	
	// 【スキルの対象になった時に保持するもの】
	void ReceiveSkillIni();	// スキル対象フラグ初期化
	void SetReceiveSkill(eSkillEffect type);	// スキル対象フラグを立てる
	bool GetReceiveSkill(eSkillEffect type);	// スキル対象フラグ取得
	int GetSkillDamage();			// スキルで受けたダメージ取得
	bool GetSkillDeath();			// スキルで死んだらtrue
	virtual void GetEffectText(vector<string>& message) = 0;	// スキルのメッセージ取得
	
	// ゲッター
	string GetName();
	int GetMaxHP();
	int GetMaxMP();
	BattleStatus GetBattleStatus();
	int GetMaxUseEnergy();
	virtual int GetStatus(eStatus status);	// ステータス取得
	Skill* GetConfusionSkill();	// 混乱時の攻撃用スキル取得
	eAlive GetAlive();	// 生存状態を返す	0:生存	1:死亡	2:消滅
	double GetSpeed();	// 行動速度
	tBadStatus GetBadStatus(eBadStatus type);	// 状態異常
	int GetDEF_BadStatus(eBadStatus type);		// 状態異常耐性の取得
	double GetEfficiency(eEnergy energy, bool direction);	// エネルギー変換効率
	int GetHit();	// 命中力
	int GetAvoid();	// 回避力
	virtual bool GetCharType() = 0;	// キャラクターのタイプ取得	プレイヤー：true	敵：false
	virtual int GetPlayerOrder() { return 0; }	// プレイヤーの並び順
	virtual int GetX() = 0;	// バトルフィールド座標
	virtual int GetY() = 0;
	float GetCharge(eCharge type);	// チャージ倍率

	// 装備
	void EquipmentWeapon(EquipmentItem* weapon);	// 武器の装備
	int GetWeaponID();		// 武器のIDを取得
	string GetWeaponName();	// 武器の名前を取得
	void EquipmentArmor(EquipmentItem* armor);		// 防具の装備
	int GetArmorID();		// 防具のIDを取得
	string GetArmorName();	// 防具の名前を取得
	void			EquipmentAmulet(EquipmentItem* armor);	// その他の装備
	int				GetAmuletID();							// その他のIDを取得
	string			GetAmuletName();						// その他の名前を取得

	void ChangeWeapon();			// 武器の変更時に通常攻撃を変える処理

	// debug
	int				GetATK();
	BattleStatus	GetFixed();	// 修正込みのステータス
};

class PlayerChara :public Character {
protected:
	// 【画像】
	static int gr_PlayerStatus;	// プレイヤーのステータスの枠（正常時）
	static int gr_PlayerStatus_Death;	// プレイヤーのステータスの枠（死亡時）

	// 経験値
	unsigned int EXP;	// 所持している経験値
	short LV;			// 現在のレベル
	unsigned int nextEXP;	// 次のレベルアップに必要な経験値

	unsigned int skillPoint;	// 保持スキルポイント

	vector<Skill*> skillList;	// 保持スキル
	
public:
	PlayerChara(string name, bool isContinue);
	virtual ~PlayerChara();

	static void InitBattle();	// 戦闘初期化処理
	static void FinaBattle();	// 終了処理

	void LoadSkillTree();	// 取得スキルのロード
	void InitSkillTree();	// スキルツリーの初期化

	virtual void Lv1_Init() {};	// キャラクタごとのLv.1状態への初期化

	void Draw();	// 描画

	int order;	// 並び順

	void SetColision(vector<vector<Character*>>& collField);	// 当たり判定の作成
	void DeleteColision(vector<vector<Character*>>& collField);	// 当たり判定の削除

	virtual void UniqueProcess() = 0;		// 固有処理（刻印とか）
	vector<Skill*> GetSkillList();			// 保持スキルの取得
	vector<Skill*> GetSkillList_Battle();	// 戦闘スキルの取得
	vector<Skill*> GetSkillList_Field();	// フィールドスキルの取得
	bool CheckAbleAct();		// キャラクタが行動可能か返す ＆ 状態に応じた行動をする
	void GetEffectText(vector<string>& message);	// スキルのメッセージ取得
	void AddEXP(int exp);		// 経験値取得

	int GetX();	// バトルフィールド座標
	int GetY();

	void AutoIncMP_Field();	// フィールドでの自動MP回復
	bool DecSkillPoint(int point);	// スキルポイントの消費

	// セーブ
	void Save();

	// ゲッター
	int GetEXP();
	int GetLV();
	int GetStatus(eStatus status);		// ステータス取得
	bool GetCharType() { return true; }	// キャラクターのタイプ取得	プレイヤー：true	敵：false
	Skill* GetNomalAttack();			// 通常攻撃取得
	int GetPlayerOrder();				// プレイヤーの並び順
	unsigned int GetSkillPoint();		// スキルポイント
};

class Kata :public PlayerChara {
private: 
	SkillEffect_Buff* curse;	// 力の刻印
public: 
	Kata(string name, bool isContinue);
	~Kata();

	void Lv1_Init();	// Lv.1状態への初期化

	void UniqueProcess();		// 固有処理（刻印とか）
};

class Tee :public PlayerChara {
public:
	Tee(string name, bool isContinue);
	~Tee();

	void Lv1_Init();	// Lv.1状態への初期化

	void UniqueProcess();		// 固有処理（刻印とか）
};

// debug
class Aoi :public PlayerChara {
public:
	Aoi(string name, bool isContinue);
	~Aoi();

	void Lv1_Init();	// Lv.1状態への初期化

	void UniqueProcess() {}		// 固有処理（刻印とか）
};