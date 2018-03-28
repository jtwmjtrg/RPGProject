#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Item.h"
#include "Player.h"

class Shop {
private:
	static vector<Item*>	item;		// 売り物
	static ItemBox			forSale;	// 売り物
	static vector<BattleStatus>		bs;	// ステータス
	static vector<vector<BattleStatus> >	player_bs;	// プレイヤーの装備のバトルステータス
	static string			greeting;		// あいさつ
	static string			confirmation;	// 確認
	static string			who;			// 誰？

public:
	static ItemBox					decideMenu;	// 確認メニュー
	static ItemBox					charaMenu;	// キャラクターメニュー

	// コンストラクタ＆デストラクタ
	Shop();
	~Shop();

	static void InitShop(int id);	// 商品情報の初期化
	static void InitPlayerBattleStatus();

	void Process();	// 処理系
	void Draw();	// 描写系
};