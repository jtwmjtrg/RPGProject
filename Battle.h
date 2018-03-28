#pragma once
#include "Game.h"
#include "Enemy.h"

// バトルシーンの段階
enum class eBattleStep : __int8 {
	START,	// 戦闘開始
	COMMAND,// プレイヤーコマンド処理
	BATTLE,	// バトルシーン
	END,	// 戦闘終了
};
// コマンド画面の段階
enum class eCommand : __int8 {
	CharaSelect,	// キャラ選択
	ActionSelect,	// 行動選択
	SkillSelect,	// 使用スキル選択
	ItemSelect,		// 使用アイテム選択
	TargetSelect,	// スキル等の対象範囲選択
};

// 前ターンに選択したカーソルの位置を記憶しておく
struct tCommandPreSelect {
	int action;		// 行動選択
	int skill;		// 使用スキル選択
	int item;		// 使用アイテム選択
	int targetX;	// スキル等の対象範囲選択	X座標	敵選択用
	int targetY;	//							Y座標
	int selectTargetAlly;	// 味方選択用
	bool fullPowerFlag;		// 全力スキルフラグ
};

enum class eActionSelect : __int8 {
	Action,		// 攻撃
	Skill,		// スキル
	Item,		// アイテム
	Defence,	// 防御
	Escape,		// 逃げる
};

// ダメージエフェクト
class DamageEffect {
private:
	static std::array<int, 10> gr_RedNum;	// 数字画像（赤）

	int x, y;	// 座標
	int value;	// 表示する数値
	int frame;	// 表示するフレーム数
	int count;	// 表示カウント
	int delay;	// 表示までのdelay
	int color;	// 色
public:
	DamageEffect(int x, int y, int value, int frame, int color = WHITE, int delay = 0);
	~DamageEffect();

	static void Init();	// 初期化
	static void Fina();	// 終了処理

	bool Process();
	void Draw();
};

// 画像を表示するエフェクト
class GraphicEffect {
protected:
	const int gr;			// 描画する画像
	const short x, y;		// 座標
	short frame;	// 表示するフレーム数
	short count;	// 表示カウント
	short delay;	// 表示までのdelay
public:
	GraphicEffect(short x, short y, int gr, short frame, short delay = 0);
	~GraphicEffect();

	virtual bool Process();
	virtual void Draw(int cellSize);
};
// 画像を表示するエフェクト（分割）
class DivGraphicEffect : public GraphicEffect {
private:
	const unsigned __int8 divX;	// 分割数X
	const unsigned __int8 divY;	// 分割数Y
	const int divNum;			// 総分割数

	const unsigned __int8 width;	// 横幅
	const unsigned __int8 height;	// 縦幅
public:
	DivGraphicEffect(short x, short y, int gr, unsigned __int8 divX, unsigned __int8 divY, short divNum, unsigned __int8 width, unsigned __int8 height, short delay = 0);
	~DivGraphicEffect();

	virtual bool Process();
	virtual void Draw(int cellSize);
};

/*
// スキルオーダー用
struct tSkillOrder {
	Character* attacker;	// スキルの使用者
	Skill* skill;	// スキルデータ
};
*/

// debug
// アイテムを使用する際の情報
typedef struct _UsedItem {
	bool		isUse;		// 処理を行うか
	int			player;		// 並び順で何番目か
	Position2D	item;		// バッグのどの位置のアイテムか
}UsedItem;

class Battle :public Game{
private:
	eEndStatus battleEndType;	// 戦闘終了の状態
	eBattleStep step;			// バトルシーンの段階
	unsigned int turnNum;		// ターン数
	bool EscapeFlag;			// 逃走可能フラグ
	bool loseEventFlag;			// 負けイベフラグ

	// 【画像】
	int gr_Back;			// 背景
	int gr_Operation;		// 戦闘画面での操作説明
	int gr_SkillSelect;		// スキル選択ウィンドウ
	int gr_scroll;			// スクロールバー

	vector<PlayerChara*> player;	// プレイヤーのデータ
	vector<Enemy*> enemy;	// 敵のデータ

	vector<vector<Character*>> enemyField;	// 敵フィールド（マスが敵のポインタを持っている）
	vector<vector<Character*>> playerField;	// プレイヤーフィールド（マスがプレイヤーのポインタを持っている）
	int enemyFieldWidth;	// 敵フィールドの横幅
	int enemyFieldHeight;	// 敵フィールドの縦幅
	int cellSize;	// 1マスのサイズ
	int enemyFieldX;
	int enemyFieldY;

	int playerFieldWidth;	// プレイヤーフィールドの横幅
	int playerFieldHeight;	// プレイヤーフィールドの縦幅

	int fieldMP;	// フィールドのMP
	int fieldMaxMP;	// フィールドの最大MP
	
	vector<Character*> targetList;		// 攻撃対象リスト
	vector<Character*> targetSave;		// 攻撃対象リストの保存（複合攻撃とかに使う）

	vector<Character*>	order;			// 敵味方の行動順（遅い順に入れる）
	SkillOrder			*skillOrder;	// スキルの発動順

	vector<int>			playerHate;		// プレイヤーのヘイト
	int					maxHate;		// ヘイト値上限

	vector<vector<bool>>	enemyField_Save;	// スキル対象保存用（敵）
	vector<vector<bool>>	playerField_Save;	// スキル対象保存用（味方）
	vector<Character*>		target_Save;		// スキル対象保存用（キャラクターごと）

	// 特殊スキル
	Skill_Defence*	skill_Defence;	// 防御
	Skill_None*		skill_None;		// 「なにもしない」スキル
	Skill_Escape*	skill_Escape;	// 逃走

	// 【コマンド画面】
	eCommand					commandStep;		// コマンド画面の段階
	vector<tCommandPreSelect>	commandPreSelect;	// 前ターンに選択したカーソルの位置を記憶しておく
	int							selectChara;
	int							selectAction;		// 行動選択
	int							ActionNum;			// 行動選択数

	int							selectSkill;		// スキル選択
	int							skillNum;			// スキル選択数
	int							selectSkillCursor;	// スキル選択画面のカーソルの位置
	int							selectSkillArea;
	vector<Skill*>				selectSkillList;	// スキルリスト
	//vector<Skill*> skillOrder_C;	// コマンド画面中に追加したスキル
	//vector<Character*>	attackerOrder_C;	// スキルの使用者
	vector<tSkillOrder>			commandOrder;		// コマンドでのスキルオーダー

	Skill*	selectSkillData;	// 選択したスキル
	int		selectTargetX;		// 選択中の座標（敵選択用）
	int		selectTargetY;
	int		selectTargetAlly;	// 味方選択用



	// 戦利品
	int				exp;		// 戦闘で得た経験値の合計
	int				money;		// 戦闘で得た金の合計

	// debug
	deque<DamageEffect>	damageEffect;	// ダメージエフェクト（ダメージ量の表示とか）
	deque<GraphicEffect*>	graphicEffect;	// 画像を表示するエフェクト
	Skill*					testSkill;		// スキル
	int						gr_SkillHit1;	// スキルの当たり判定
	int						gr_SkillHit2;	// スキルの当たり判定
	int						gr_SkillHit3;	// スキルの当たり判定
	int						gr_SkillHit4;	// スキルの当たり判定
	int						gr_target;		// 敵の当たり判定
	int						gr_NA;
	bool					skillStep;		// true:スキルエフェクト表示	false:スキル発動

	// suzuki_debug
	MenuVar					selectItem;		// アイテム選択時の要素
	vector<UsedItem>		usedItem;		// そのターンに使用する予定のアイテムのポインタを保存

public:
	Battle();
	~Battle();

	void Process();
	void Draw();

	void Process_START();	// 戦闘開始
	//void Draw_START();
	void Process_END();		// 戦闘終了
	//void Draw_END();
	void Process_COMMAND();	// プレイヤーコマンド画面
	void Draw_COMMAND();
	void Process_BATTLE();	// バトルシーン
	//void Draw_BATTLE();

	void StepChange_START();	// 開始画面への移行処理
	void StepChange_COMMAND();	// コマンド画面への移行処理
	void StepChange_BATTLE();	// バトルシーンへの移行処理
	//void StepChange_END();		// 終了画面への移行処理
	//void StepChange_TARGET();	// 

	int EnergyConvDmage(Character* attacker, Character* target, tEnergyConv energy, float hit, float damageRate, int totalUseEnergy);	// エネルギーごとのダメージ補正計算
	bool CheckAbleSkill(tSkillOrder order);				// スキルが発動可能か調べる
	bool SkillExec(Character* attacker, Skill* skill);	// スキル発動
	bool SkillExec(tSkillOrder order);	// スキル発動
	void SkillEffectExec(int FieldX, int FieldY, Character* attacker, Character* target, SkillEffect* effect, int totalUseEnergy, int effectNum);	// 対象にスキルの効果を与える
	void DeleteDeadEnemy();	// 死んだ敵のデータの削除

	int GetFieldMP();					// フィールドMPのゲッター
	void IncreaseFieldMP(int value);	// フィールドMP増加
	void DecreaseFieldMP(int value);	// フィールドMP減少
	
	void EndText();	// エンカウント情報の表示

	bool CheckEnemysDead();		// 敵全滅判定
	bool CheckPlayersDead();	// プレイヤー全滅判定

	void LoadEncounter();	// エンカウントデータの読み込み

	// debug
	void TestIni();	// テスト用初期化
	
};