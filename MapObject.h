#pragma once
#include"Player.h"
#include "Shop.h"
#include <cstdlib>
#include <boost/algorithm/string/split.hpp>		// スプリット
#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string.hpp>

enum class GameScene : __int8;
class MapUnit_Mode;

// 挙動の条件
enum class eUnit_If {
	Talk,	// 話しかける
	Touch,	// 触れる
	Message,// メッセージを受信する
	Always,// 常時
	Distance,	// 主人公との距離判定
};

// 挙動
enum class eUnit_Act {
	Warp,		// 転移
	Talk,		// セリフ
	Battle,		// 戦闘
	Damage,		// ダメージ
	Recovery,	// 回復
	Item,		// アイテム取得
	Shop,		// 店
	Existence,	// 消滅-出現
	ChangeMode,	// モード変更
	Message,	// メッセージ発信
	SelectBranch,// 選択分岐
	End,		// 強制終了
	Goto,		// 挙動番号のジャンプ
	GameOver,	// 強制ゲームオーバー
	GameClear,	// ゲームクリア
	PayMoney,	// 金を払う
};

enum class eUnitStatus {
	Apear,		// 出現
	Disapear,	// 消滅
	SetColl,	// 当たり判定を付ける
	DeletColl	// 当たり判定を消す
};

/*-------------------------------------------------ユニットの挙動データ--------------------------------------------*/

enum class eMUAReturn : __int8 {
	Next = 0,	// すぐに次の挙動にうつる
	Stop = 1,	// 一時停止
	End = -1,	// 強制終了
};

// ユニットの挙動
class MapUnit_Action {	// 抽象クラス
protected:
	// アクセス制限されたコンストラクタ
	MapUnit_Action() {}

	int* actionNum;		// 実行中の行動の要素番号

public:
	// デストラクタ
	virtual ~MapUnit_Action() {}

	// 挙動の実行
	virtual eMUAReturn Execution() = 0;
	void SetActionNum(int* actionNum);		// 実行中の行動の要素番号のセッター
};

// 【転移】
class MapUnit_Action_Warp : public MapUnit_Action {
private:
	// ステータス
	int mapId;	// 転移先のマップID
	int x;		// 転移先の座標
	int y;
	int effect;	// 転移エフェクトの種類

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Warp(string line);
	~MapUnit_Action_Warp();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【セリフ】
class MapUnit_Action_Talk : public MapUnit_Action {
private:
	// ステータス
	std::vector<string> str;	// セリフ内容

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Talk(string line);
	~MapUnit_Action_Talk();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【戦闘】
// 戦闘終了の状態
enum class eEndStatus : __int8 {
	Win,	// 敵を全滅させた
	Lose,	// プレイヤーが全滅した
	Escape,	// 逃走成功
};
class MapUnit_Action_Battle : public MapUnit_Action {
private:
	int encID;			// エンカウントID
	int backID;			// 背景ID
	bool escapeFlag;	// 逃走可能フラグ
	bool loseEventFlag;	// 負けイベフラグ
	std::array<int, 3> branch;	// 戦闘終了の状態で分岐（挙動番号の移動数）

	bool startFlag;		// 開始処理フラグ
	bool battleFlag;	// 戦闘中フラグ
	eEndStatus battleEndType;	// 戦闘終了の状態

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Battle(string line);
	~MapUnit_Action_Battle();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【ダメージ】
class MapUnit_Action_Damage : public MapUnit_Action {
private:
	int damage;	// ダメージ量（固定ダメージ）
	int target;	// 対象	0:全員	1:先頭	2:ランダム

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Damage(string line);
	~MapUnit_Action_Damage();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【回復】
class MapUnit_Action_Recovery : public MapUnit_Action {
private:
	int heal_HP;		// HP回復量（固定量）	-1:全快
	int heal_MP;		// MP回復量（固定量）	-1:全快
	bool resurrection;	// 蘇生フラグ
	int target;			// 対象	0:全員	1:先頭	2:ランダム

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Recovery(string line);
	~MapUnit_Action_Recovery();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【アイテム取得】
class MapUnit_Action_Item : public MapUnit_Action {
private:
	int itemID;		// 取得アイテムのID
	int jump_True;	// アイテムを手に入れた場合のジャンプ
	int jump_False;	// 持ち物がいっぱいだった場合のジャンプ

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Item(string line);
	~MapUnit_Action_Item();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【店】
class MapUnit_Action_Shop : public MapUnit_Action {
private:
	string shopID;		// 店ID 

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Shop(string line);
	~MapUnit_Action_Shop();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【消滅-出現】
class MapUnit_Action_Existence : public MapUnit_Action {
private:
	bool flag;		// true::出現	false:消滅
	int staging;	// 演出
	bool& appearFlag;	// 出現フラグ

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Existence(string line, bool& appearFlag);
	~MapUnit_Action_Existence();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【モード変更】
class MapUnit_Action_ChangeMode : public MapUnit_Action {
private:
	int modeNum;	// 変更後のモード数
	int& nowMode;	// 出現フラグ
	const vector<MapUnit_Mode*>& mode;

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_ChangeMode(string line, int& nowMode, const vector<MapUnit_Mode*>& mode);
	~MapUnit_Action_ChangeMode();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【メッセージ発信】
class MapUnit_Action_Message : public MapUnit_Action {
private:
	int messageNum;	// メッセージ番号

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Message(string line);
	~MapUnit_Action_Message();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【選択分岐】
enum class eSelectBranch : __int8 {
	question,	// 質問
	wait,		// 解答待ち
};
class MapUnit_Action_SelectBranch : public MapUnit_Action {
private:
	string question;	// 質問文
	std::vector<tSelectBranch> choices;	// 選択肢
	bool selectFlag;	// 選択フラグ
	int select;			// 選択番号
	eSelectBranch step;	// 段階

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_SelectBranch(string line);
	~MapUnit_Action_SelectBranch();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【強制終了】
class MapUnit_Action_End : public MapUnit_Action {
public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_End();
	~MapUnit_Action_End();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【挙動番号のジャンプ】
class MapUnit_Action_Goto : public MapUnit_Action {
private:
	int moveNum;	// 移動数

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_Goto(string line);
	~MapUnit_Action_Goto();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【強制ゲームオーバー】
class MapUnit_Action_GameOver : public MapUnit_Action {
private:
public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_GameOver();
	~MapUnit_Action_GameOver();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};

// 【ゲームクリア】
class MapUnit_Action_GameClear : public MapUnit_Action {
private:
public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_GameClear();
	~MapUnit_Action_GameClear();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};
// 【金を払う】
class MapUnit_Action_PayMoney : public MapUnit_Action {
private:
	int price;		// 払う金額
	int jump_True;	// 金が足りた場合のジャンプ
	int jump_False;	// 金が足りなかった場合のジャンプ
public:
	// コンストラクタ＆デストラクタ
	MapUnit_Action_PayMoney(string line);
	~MapUnit_Action_PayMoney();

	// オーバーライド
	eMUAReturn Execution();	// 挙動の実行
};
/*-------------------------------------------------ユニットの挙動条件データ--------------------------------------------*/

// ユニットの挙動の条件
class MapUnit_If {	// 抽象クラス

protected:
	// アクセス制限されたコンストラクタ
	MapUnit_If() {}

	// ステータス
	vector<MapUnit_Action*> action;	// 挙動データ
	int actionNum;		// 実行中の行動の要素番号

public:
	// デストラクタ
	virtual ~MapUnit_If() {}

	bool ActionExecution();	// 挙動の実行

	// 制御系
	eUnit_If ifType;	// 条件の種類
	void AddAction(MapUnit_Action* action);	// 挙動追加
	virtual void CheckMessage(int num) {}	// メッセージ判定
	void EndProcess();						// 終了処理

	virtual bool ChackIf() = 0;	// 条件判定
};

// 【主人公がこのユニットに話しかける】
class MapUnit_If_Talk : public MapUnit_If {
private:
	const int& x;
	const int& y;
public:
	// コンストラクタ＆デストラクタ
	MapUnit_If_Talk(string line, const int& x, const int& y);
	~MapUnit_If_Talk();

	// オーバーライド
	bool ChackIf();	// 条件判定
};

// 【主人公がこのユニットに触れる】
class MapUnit_If_Touch : public MapUnit_If {
private:
	const int& x;
	const int& y;
public:
	// コンストラクタ＆デストラクタ
	MapUnit_If_Touch(string line, const int& x, const int& y);
	~MapUnit_If_Touch();

	// オーバーライド
	bool ChackIf();	// 条件判定
};

// 【メッセージ受信】
class MapUnit_If_Message : public MapUnit_If {
private:
	int messageNum;	// メッセージ番号
public:
	// コンストラクタ＆デストラクタ
	MapUnit_If_Message(string line);
	~MapUnit_If_Message();

	// オーバーライド
	bool ChackIf();	// 条件判定
};

// 【常時】
class MapUnit_If_Always : public MapUnit_If {
public:
	// コンストラクタ＆デストラクタ
	MapUnit_If_Always();
	~MapUnit_If_Always();

	// オーバーライド
	bool ChackIf();	// 条件判定
};

// 【プレイヤーとの距離判定】
class MapUnit_If_Distance : public MapUnit_If {
private:
	const int& x;
	const int& y;

	int distance;	// 距離（半径）
	bool In_Out;	// true:近づく	false:離れる
public:
	// コンストラクタ＆デストラクタ
	MapUnit_If_Distance(string line, const int& x, const int& y);
	~MapUnit_If_Distance();

	// オーバーライド
	bool ChackIf();	// 条件判定
};
/*-------------------------------------------------ユニットの移動データ--------------------------------------------*/

// ユニットの移動
class MapUnit_Move {	// 抽象クラス
protected:
	int preX;	// １フレーム前の座標
	int preY;

	int& x;
	int& y;
	int speed;		// 移動速度
	int delay;		// １マス移動するごとにnF止まる
	Direction& dir;	// 移動方向
	int moveRange;	// 移動距離
	bool moveFlag;	// 移動中フラグ

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Move(int& x, int& y, Direction& dir);
	virtual ~MapUnit_Move() {}

	virtual void Move() = 0;	// 移動
	void PosCorrection();		// 座標修正
};

// 【動かない】
class MapUnit_Move_None : public MapUnit_Move {
public:
	// コンストラクタ＆デストラクタ
	MapUnit_Move_None(int& x, int& y, Direction& dir);
	~MapUnit_Move_None();
	void Move();
};

// 【ランダム移動】
class MapUnit_Move_Random: public MapUnit_Move{
private:
	int iniX;		// 初期座標
	int iniY;
	int count;		// フレームカウント
	int moveCount;	// 移動数（３歩ずつ初期座標の方向に動く）

public:
	// コンストラクタ＆デストラクタ
	MapUnit_Move_Random(string line, int& x, int& y, Direction& dir);
	~MapUnit_Move_Random();
	void Move();
};


/*-------------------------------------------------ユニットのモード--------------------------------------------*/

enum class eUnitMode {
	None,		// 条件なし
};

// 【モードの移行条件スーパークラス】
class MapUnit_Mode {
protected:
	vector<MapUnit_If*> terms;	// 挙動の実行条件
	int termNum;				// 実行中の条件の要素番号
	MapUnit_Move* move;			// 移動クラス

	MapUnit_Mode();
public:
	virtual ~MapUnit_Mode() {}
	eUnitMode type;		// 移行条件の種類

	virtual bool ModeCheck() = 0;	// モードの移行条件判定
	bool TermsCheck();				// 挙動の実行条件判定

	bool ActionExecution();		// 行動実行
	void Move();				// 移動
	void PosCorrection();		// 座標修正

	void AddTerm(MapUnit_If* term);	// 挙動の実行条件を追加
	void AddAction(MapUnit_Action* term);	// 挙動追加
	void SetMove(MapUnit_Move* data);		// 移動クラスのセッター

	void EndProcess();	// 終了処理
};

// 【条件なし】
class MapUnit_Mode_None : public MapUnit_Mode {
private:
	const int modeNum;		// 自分のモード番号
	const int& nowMode;		// 現在のモード
public:
	MapUnit_Mode_None(int modeNum, int& nowMode);
	~MapUnit_Mode_None();

	bool ModeCheck();	// モードの移行条件判定
};
// 【ストーリー進行度】
class MapUnit_Mode_Chapter : public MapUnit_Mode {
private:
	int chapter;	// 基準の進行度
	int term;		// 条件（0:のとき	1:以外のとき	2:以前		3:以降）
	
public:
	MapUnit_Mode_Chapter(string line);
	~MapUnit_Mode_Chapter();


	bool ModeCheck();	// モードの移行条件判定
};

/*-------------------------------------------------マップオブジェクトスーパークラス--------------------------------------------*/

// マップ上のオブジェクトスーパークラス
class MapObject {	// 抽象クラス
protected:
	// アクセス制限されたコンストラクタ
	MapObject() {}

	// 配置データ
	int		id;		// ID
	string	name;	// 名前
	int		initX;	// 初期座標
	int		initY;		
	int		imgID;	// 画像ID
	bool	collisionFlag;	// 当たり判定
	bool	appearFlag;		// 出現フラグ
	Direction dir;	// 移動方向

	int		img;	// 画像データ

	int count;
	
public:
	// デストラクタ
	virtual ~MapObject() {}

	int		x;		// 座標
	int		y;

	// 制御系
	bool CheckColl(int x, int y);	// プレイヤーとの当たり判定

	// ゲッター
	int GetImg() { return img; }	// 画像の管理番号を返す
	int GetID() { return id; }		// IDを返す
	bool GetAppearFlag() { return appearFlag; }			// 出現フラグ
	bool GetCollisionFlag(){ return collisionFlag; }	// 当たり判定

	// セッター
	void ChangeStatus(eUnitStatus type);	// 表示状態と当たり判定の有無を決定する
	void SetDir(Direction dir);		// 移動方向)

	// メッセージ関係
	static void MessageUpDate();					// メッセージ更新処理
	static vector<int> messageData;			// メッセージのデータ（1フレーム前に呼ばれたもの）
	static vector<int> messageData_Stack;	// メッセージのデータ（現フレームに呼ばれたもの）

	void SetAppearFlag(bool isAppear) { appearFlag = isAppear; }	// フラグのせったー
};

/*-------------------------------------------------ユニットデータ--------------------------------------------*/

class MapUnit : public MapObject {	// 全ユニットの親クラス
private:
	// ステータス
	int nowMode;	// 現在のモード
	bool execFlag;	// 行動実行中フラグ
	
	vector<MapUnit_Mode*> mode;		// モード

	int areaX;		// 区分けの座標
	int areaY;

	// ファイル入力
	void LoadData();	// 挙動データの読み込み

public:
	// コンストラクタ＆デストラクタ
	MapUnit(string line);
	~MapUnit();

	// メインループ
	bool Process();	// 毎フレームの処理
	void Draw(int cameraX, int cameraY);	// カメラを考慮した描写

	bool ActionExecution();	// 行動実行
	void PosCorrection();	// 座標修正

	void SetMode(int modeNum);	// モードのセッター
	int GetMode();				// モードのゲッター

	void SetArea(int areaX, int areaY);		// 区分け座標のセッター

	void EndProcess();	// 終了処理
};