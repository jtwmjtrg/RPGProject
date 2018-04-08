#include "Scene.h"

class Scene;

#pragma once

#include "Scene.h"

#include "Item.h"
#include "Character.h"
#include "Emote.h"

// 「話しかける」
typedef struct tTalkFlag {
	bool flag;	// 話すフラグ（1フレームのみ有効）
	Position2D pos_;	// 話しかけた座標（マップ座標）
}TalkFlag;

// プレイヤーの向きの名前付列挙
typedef enum ePlayerDirection{
	UP, DOWN, LEFT, RIGHT
}Direction;

// プレイヤーの選択状態
typedef enum ePlayerMode {
	FREE, MENU
}Mode;

// メニューの項目
typedef struct _MenuItem {
	int	   id;		// その項目のID
	string name;	// その項目の名前
}MenuItem;
// メニュー
typedef struct _Menu {
	Position2D		 pos;	// メニューの箱の座標
	Position2D		 info;	// 選択しているメニューの説明
	Position2D		 icon;	// 選択アイコン
	vector<MenuItem> item;	// 項目
}Menu;
// メニューごとの保存項目
typedef struct _MenuVar {
	bool		flag;
	Position2D	pos;
}MenuVar;

typedef struct _ItemBox{
	// 選択項目を持つ箱の描画関係をまとめた構造体
	bool			isDraw;		// 描画するかどうか
	bool			isSelect;	// 今操作可能かどうか
	Position2D		pos;		// 箱の座標
	int				width;		// 箱の幅
	int				height;		// 箱の高さ
	Position2D		icon;		// アイコンの座標
	vector<string>	itemName;	// 項目の名前

	void Initialize() {
		// 初期化
		isDraw = false;
		isSelect = false;
		icon.Y = 0;
		icon.X = 0;
	}
	void Initialize(int x, int y, int w, int h) {
		// 初期化
		isDraw		= false;
		isSelect	= false;
		pos.X		= x;
		pos.Y		= y;
		width		= w;
		height		= h;
		icon.Y		= 0;
		icon.X		= 0;
	}

	void Process() {
		// 処理

		if (!itemName.empty()) {
			// 項目があれば

			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
				// アイコンを上に移動
				if (icon.Y == 0) {
					// 一番上なら
					icon.Y = itemName.size() - 1;	// 一番下に移動
				}
				else {
					// それ以外なら
					--icon.Y;	// 一つ上へ
				}
			}
			else if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
				// アイコンを下に移動
				if (icon.Y == itemName.size() - 1) {
					// 一番下なら
					icon.Y = 0;	// 一番下に移動
				}
				else {
					// それ以外なら
					++icon.Y;	// 一つ下へ
				}
			}

			if ((unsigned)icon.Y > itemName.size() - 1) {
				// 範囲外を戻す
				icon.Y = 0;
			}
		}
	}

	void Draw() {
		// 描画
		DrawBox(pos.X, pos.Y, width, height, true);	// 箱
		for (int i = 0, n = itemName.size(); i < n; ++i) {
			// 項目の数だけループ
			DrawString(WHITE, pos.X + 16 + 32, pos.Y + 32 + (i * 32), itemName[i].c_str());	// 項目名を描画
		}
		DrawString(WHITE, pos.X + 16, pos.Y + 32 + (icon.Y * 32), "・");	// アイコン
	}
}ItemBox;


// メニューの項目
typedef enum ePlayerMenu {
	START, ITEM, SKILL, SKILLTREE, REARRANGE, STATUS, SAVE, BACK_TITLE, EMOTE, BACK
}PlayerMenu;

class Item;
class MapUnit;

class Player	// 静的クラス
{
private:
	// 画像
	static int animCount;	// アニメーションの判定
	static int animSpeed;	// アニメーションの速度。小さいほうが早い

	// 制御系
	static Position2D pre_pos_;		// 直前の座標
	static Mode mode;				// メニュー画面を開いているかどうか
	static int	swapCharacterY;		// ならべかえ

	// ステータス
	static int speed;	// 移動スピード※２の冪乗にすること
	static int money;	// 所持金
	
	// 一時セーブデータ
	static map<int, map<int, int>> tmpSave_unitMode;	// ユニットのモード	<マップID <ユニットID , モード>>

	// メインループ
	static void MenuProcess();	// メニュー画面の処理

	// 条件等で呼ばれる関数
	static void UseItem(Item* item);	// アイテ使用時に呼ばれる

	static int		menuY;

	static int cover_handle_;	// 半透明な黒
	
	static void Process_Start();
	static void Process_Item();
	static void Process_Skill();
	static void Process_SkillTree();
	static void Process_Rearrange();
	static void Process_Status();
	static void Process_Save();
	static void Process_Back_Title();	// タイトルに戻る
	static void Process_Emote();		// エモート
	static void Process_Back();

	static void Draw_Start();
	static void Draw_Item();
	static void Draw_Skill();
	static void Draw_SkillTree();
	static void Draw_Rearrange();
	static void Draw_Status();
	static void Draw_Save();
	static void Draw_Back_Title();
	static void Draw_Emote();			// エモート
	static void Draw_Back();

public:
	// メニューボックスたち
	static ItemBox	mainMenu;	// メインメニュー
	static ItemBox	charaMenu;	// キャラクターメニュー
	static ItemBox	itemMenu;	// アイテムメニュー
	static ItemBox	doMenu;		// Ｄｏメニュー
	static ItemBox	targetMenu;	// ターゲットメニュー
	static ItemBox	decideMenu;	// 確認メニュー

	static int playerGraph;	// 本体の画像
	
	// 生成チェック
	static bool born;

	// セーブフラグ（セーブしたフレームにのみtrueになる）
	static bool saveFlag;

	// 初期化関数
	static void InitPlayer(bool isContinue);	// 初期化
	static void DeletePlayer();					// 削除

	// 制御系
	static int chapter;			// ストーリー進行度
	static bool animation;		// アニメーション中かどうか
	static bool flag_shop;		// 買い物中かどうか

	// ステータス
	static int stage;				// 今いるステージ
	static Position2D pos_;			// 座標
	static Direction direction;		// 向き
	static TalkFlag talkFlag;		// 話しかけた座標

	// パーティー
	static vector<PlayerChara*> character;	// パーティーメンバ
	static vector<vector<Item*> > bag;		// アイテムバッグ

	// debug
	static int emote_lock_;		// エモートの連投禁止

	// メインループ
	static bool Process();	// 処理		座標を移動したらtrueを返す（エンカウント処理用）
	static void Draw(bool isFollow, int cameraX, int cameraY);		// 描写

	// 条件等で呼び出される関数
	static void Save();			// セーブデータの作成
	static void PosCorrection();	// プレイヤーの座標修正
	static bool InsertItem(int item_id);			// アイテムをバッグに追加する。アイテムの格納先は先頭から空きを見つけ次第そこに追加する
	static bool InsertItem(int order, int item_id);	// アイテムをバッグに追加する

	static void AddMoney(int value);	// 金を手に入れる
	static void PayMoney(int value);	// 金を払う
	static int GetMoney();				// 金のゲッター

	// 一時セーブデータ
	static void LoadTmpSave_UnitMode(string line);								// ユニットのモードのロード
	static void TmpSave_UnitMode(int saveStage, const std::map<int, MapUnit*> unitData);		// ユニットのモードの一時セーブ
	static bool SetUnitMode_TmpSave(std::map<int, MapUnit*>& unitData);	// ユニットのモードのセット

	static vector<vector<bool> > isFinish;
};