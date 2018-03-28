#include "Shop.h"

vector<Item*>	Shop::item;
ItemBox			Shop::forSale;
string			Shop::greeting;
string			Shop::confirmation;
string			Shop::who;
vector<BattleStatus> Shop::bs;
vector<vector<BattleStatus> > Shop::player_bs;	// プレイヤーの装備のバトルステータス
ItemBox			Shop::charaMenu;
ItemBox			Shop::decideMenu;

Shop::Shop(){
	forSale.Initialize(0, 0, 512, 256);
	decideMenu.itemName.push_back("はい");
	decideMenu.itemName.push_back("いいえ");
	decideMenu.Initialize(1024, 96, 320, 128);
	for (int i = 0, n = Player::character.size(); i < n; ++i) {
		// キャラクターの数だけループ
		charaMenu.itemName.push_back(Player::character[i]->GetName());
	}
	charaMenu.Initialize(1344, 96, 160, 64 + Player::character.size() * 32);
}
Shop::~Shop() {
	for (int i = 0, n = item.size(); i < n; ++i) {
		if (item[i]) {
			delete item[i];
		}
	}
}

void Shop::InitShop(int id) {
	for (int i = 0, n = item.size();i < n;++i) {
		// アイテム一覧を削除
		delete item[i];
	}
	item.clear();			// 配列をクリア
	item.shrink_to_fit();	// サイズを合わせる

	ifstream ifs("shop\\" + to_string(id) + ".csv");	// ファイルオープン
	if (!ifs) {
		// ファイルがない場合
		return;	// エラー
	}

	string s;	// 読み込み用
	for (int i = 0;i <= id;++i) {
		// 指定したデータまで読み進める
		getline(ifs, s);
	}
	stringstream stream(s);	// 
	string token;			// 
	while (getline(stream, token, ',')) {
		// 終点までカンマ区切りで読み込む
		item.push_back(Item::Factory(stoi(token)));	// idからアイテムを取得する
	}
	int itemnum = item.size();
	// あいさつ
	getline(ifs, s);
	greeting = s;
	for (int i = 0;i < itemnum - 1;++i) {
		greeting.pop_back();
	}
	// 確認
	getline(ifs, s);
	confirmation = s;
	for (int i = 0;i < itemnum - 1;++i) {
		confirmation.pop_back();
	}
	// 誰？
	getline(ifs, s);
	who = s;
	for (int i = 0;i < itemnum - 1;++i) {
		who.pop_back();
	}

	// アイテムの戦闘ステータス
	EquipmentItem* e;	// 仮
	bs.clear();
	bs.shrink_to_fit();
	bs.resize(itemnum); // アイテムサイズに合わせる
	for (int i = 0;i < itemnum;++i) {

		switch (item[i]->type) {

		case ItemType::IMPORTANT:
			// 重要アイテム
			// 戦闘ステータスなし
			break;

		case ItemType::RECOVERY:
			// 回復アイテム
			// 戦闘ステータスなし
			break;

		case ItemType::DAMAGE:
			// 攻撃アイテム
			// 戦闘ステータスなし
			break;

		case ItemType::EQUIPMENT:
			// 装備アイテム

			e = new EquipmentItem(item[i]->GetMyData()[0]);
			bs[i] = e->GetCorrection();
			delete e;

			break;

		case ItemType::LIFESTEAL:
			// ライフスティールアイテム
			// 戦闘ステータスなし
			break;

		default:
			break;
		}
	}

	// debug
	forSale.itemName.clear();
	forSale.itemName.shrink_to_fit();
	for (int i = 0, n = item.size(); i < n; ++i) {
		forSale.itemName.push_back(item[i]->GetName());
	}
	forSale.isDraw		= true;
	forSale.isSelect	= true;

	InitPlayerBattleStatus();
}
void Shop::InitPlayerBattleStatus() {
	// 装備のステータスを保存する

	EquipmentItem* e;

	player_bs.resize(Player::character.size());	// リサイズ
	for (int i = 0, n = player_bs.size();i < n;++i) {
		player_bs[i].resize(3);	// リサイズ
		for (int j = 0;j < 3;++j) {
			e = new EquipmentItem(0);
			player_bs[i][j] = e->GetCorrection();
			delete e;
		}
	}

	for (int i = 0, n = player_bs.size();i < n;++i) {
		// キャラクターの数だけループ

		for (int j = 0, m = Player::bag[i].size();j < m;++j) {
			// アイテムの数だけループ

			if (Player::bag[i][j]->isEquipment) {
				// 装備していたら

				e = new EquipmentItem(Player::bag[i][j]->GetMyData()[0]);
				player_bs[i][(int)Player::bag[i][j]->equipmentType - 1] = e->GetCorrection();
				delete e;
			}
		}
	}
}

void Shop::Process() {
	if (forSale.isSelect) {
		// 商品を選んでいるとき
		forSale.Process();

		if ((	Input::Get(VK_Z)				== 1	|| Input::Get(VK_V)					== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1	|| Input::Get(0, XINPUT_GAMEPAD_Y)	== 1)) {
			// 決定
			if (Player::Player::GetMoney() >= item[forSale.icon.Y]->GetPrice()) {
				// お金が足りている
				forSale.isSelect	= false;
				decideMenu.isDraw	= true;
				decideMenu.isSelect = true;
			}
			else {
				// お金が足りていない
				TextBox::AddLine("お金が足りないよ");
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 店を出る
			forSale.isSelect	= false;
			forSale.isDraw		= false;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
			Player::flag_shop	= false;
		}
	}
	else if(decideMenu.isSelect) {
		// 商品を選んだあと
		decideMenu.Process();

		if (	Input::Get(VK_Z)				== 1 || Input::Get(VK_V)				== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定
			if (decideMenu.icon.Y == 0) {
				charaMenu.isDraw	= true;
				charaMenu.isSelect	= true;
				decideMenu.icon.Y	= 0;
				decideMenu.isSelect = false;
			}
			else {
				// いいえ
				forSale.isSelect	= true;
				decideMenu.icon.Y	= 0;
				decideMenu.isSelect = false;
				decideMenu.isDraw	= false;
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// キャンセル
			forSale.isSelect	= true;
			decideMenu.icon.Y	= 0;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}
	}
	else if (charaMenu.isSelect) {
		// キャラクターメニュー
		charaMenu.Process();

		if (	Input::Get(VK_Z)				== 1 || Input::Get(VK_V)				== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定
			if (Player::InsertItem(charaMenu.icon.Y, item[forSale.icon.Y]->GetID())) {
				// 買い物成功
				// 袋にアイテムを入れる
				Player::PayMoney(item[forSale.icon.Y]->GetPrice());		// お金を減らす
				TextBox::AddLine("まいど");								// セリフ
			}
			else {
				// 買い物失敗(アイテムを持てない)
				TextBox::AddLine("これ以上持てない");
			}

			forSale.isSelect	= true;
			decideMenu.icon.Y	= 0;
			charaMenu.isSelect	= false;
			charaMenu.isDraw	= false;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// キャンセル
			forSale.isSelect	= true;

			charaMenu.isSelect	= false;
			charaMenu.isDraw	= false;
			decideMenu.icon.Y	= 0;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}
	}
}
void Shop::Draw() {
	// 所持金
	DrawBox(1000, 0, 300, 64, true);	// 箱
	DrawString(WHITE, 1024, 16, "%d", Player::GetMoney());

	// 上の箱
	forSale.Draw();					
	for (int i = 0, n = item.size(); i < n; ++i) {
		// 値段
		DrawString(WHITE, 512 - (32 * 6), 32 + i * 32, "%d", item[i]->GetPrice());
	}

	// ちょっと上の箱
	DrawBox(0, 256, 1024, 256, true);	// 背景箱
	DrawString(WHITE, 16, 256 + 16, item[forSale.icon.Y]->GetInfo().c_str());	// 説明文

	// アイテムのステータス(右上)
	DrawBox(512, 0, 512, 256, true);
	for (int i = 0, n = item.size(); i < n; ++i) {
		// アイテムの種類によって表示を変化
		switch (item[forSale.icon.Y]->type) {
		case ItemType::IMPORTANT:
			// 重要アイテム
			// これが店に売られてるタイミングある？
			break;

		case ItemType::RECOVERY:
			// 回復アイテム

			DrawString(WHITE, 512 + 16, 0 + 16, ((item[forSale.icon.Y]->GetMyData()[1] == 0 ? "" : "味方全体を ") + to_string(item[forSale.icon.Y]->GetMyData()[0]) + "回復する").c_str());
			break;

		case ItemType::DAMAGE:
			// 攻撃アイテム

			// 未実装

		case ItemType::EQUIPMENT:
			// 装備アイテム

			DrawString(WHITE, 512 + 32, 0 + 16, ("HP : " + to_string(bs[forSale.icon.Y].maxHP)).c_str());
			DrawString(WHITE, 512 + 32, 0 + 48, ("MP : " + to_string(bs[forSale.icon.Y].maxMP)).c_str());
			DrawString(WHITE, 512 + 16, 0 + 80, ("ATK : " + to_string(bs[forSale.icon.Y].STR)).c_str());
			DrawString(WHITE, 512 + 16, 0 +112, ("DEX : " + to_string(bs[forSale.icon.Y].DEX)).c_str());
			DrawString(WHITE, 512 + 16, 0 +144, "AG");DrawString(WHITE, 512 + 64, 0 + 144, ("I : " + to_string(bs[forSale.icon.Y].AGI)).c_str());
			DrawString(WHITE, 512 + 16, 0 +176, ("LUC : " + to_string(bs[forSale.icon.Y].LUC)).c_str());

			DrawString(WHITE, 768 + 16, 0 + 16, ("物防 : " + to_string(bs[forSale.icon.Y].DEF_PHYS)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 48, ("耐熱 : " + to_string(bs[forSale.icon.Y].DEF_HEAT)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 80, ("耐寒 : " + to_string(bs[forSale.icon.Y].DEF_COLD)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 112,("耐電 : " + to_string(bs[forSale.icon.Y].DEF_ELEC)).c_str());

			break;

		case ItemType::LIFESTEAL:
			// ライフスティールアイテム

			// 未実装

			break;

		default:
			// エラー
			break;
		}
	}

	// ちょっと下の箱
	for (int i = 0, n = player_bs.size();i < n;++i) {
		// プレイヤーの数だけループ

		DrawBox(512 * i, 512, 512, 256, true);
	}

	for (int i = 0, n = item.size(); i < n; ++i) {
		// アイテムの種類によって表示を変化
		switch (item[forSale.icon.Y]->type) {
		case ItemType::IMPORTANT:
			// 重要アイテム
			// これが店に売られてるタイミングある？
			break;

		case ItemType::RECOVERY:
			// 回復アイテム

			DrawString(WHITE, 0 + 16, 512 + 16, ((item[forSale.icon.Y]->GetMyData()[1] == 0 ? "" : "味方全体を ") + to_string(item[forSale.icon.Y]->GetMyData()[0]) + "回復する").c_str());
			break;

		case ItemType::DAMAGE:
			// 攻撃アイテム

			// 未実装

		case ItemType::EQUIPMENT:
			// 装備アイテム

			for (int i = 0, n = player_bs.size();i < n;++i) {
				// プレイヤーの数だけループ

				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxHP < bs[forSale.icon.Y].maxHP ? GREEN : WHITE, (512 * i) + 32, 512 + 16, ("HP : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxHP) + " → " + to_string(bs[forSale.icon.Y].maxHP)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxMP < bs[forSale.icon.Y].maxMP ? GREEN : WHITE, (512 * i) + 32, 512 + 48, ("MP : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxMP) + " → " + to_string(bs[forSale.icon.Y].maxMP)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].STR < bs[forSale.icon.Y].STR ? GREEN : WHITE, (512 * i) + 16, 512 + 80, ("ATK : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].STR) + " → " + to_string(bs[forSale.icon.Y].STR)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEX < bs[forSale.icon.Y].DEX ? GREEN : WHITE, (512 * i) + 16, 512 + 112,("DEX : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEX) + " → " + to_string(bs[forSale.icon.Y].DEX)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI < bs[forSale.icon.Y].AGI ? GREEN : WHITE, (512 * i) + 16, 512 + 144, "AG");DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI < bs[forSale.icon.Y].AGI ? GREEN : WHITE, (512 * i) + 64, 512 + 144, ("I : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI) + " → " + to_string(bs[forSale.icon.Y].AGI)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].LUC < bs[forSale.icon.Y].LUC ? GREEN : WHITE, (512 * i) + 16, 512 + 176,("LUC : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].LUC) + " → " + to_string(bs[forSale.icon.Y].LUC)).c_str());

				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_PHYS < bs[forSale.icon.Y].DEF_PHYS ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 16, ("物防 : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_PHYS) + " → " + to_string(bs[forSale.icon.Y].DEF_PHYS)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_HEAT < bs[forSale.icon.Y].DEF_HEAT ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 48, ("耐熱 : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_HEAT) + " → " + to_string(bs[forSale.icon.Y].DEF_HEAT)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_COLD < bs[forSale.icon.Y].DEF_COLD ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 80, ("耐寒 : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_COLD) + " → " + to_string(bs[forSale.icon.Y].DEF_COLD)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_ELEC < bs[forSale.icon.Y].DEF_ELEC ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 112,("耐電 : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_ELEC) + " → " + to_string(bs[forSale.icon.Y].DEF_ELEC)).c_str());
			}
			break;

		case ItemType::LIFESTEAL:
			// ライフスティールアイテム

			// 未実装

			break;

		default:
			// エラー
			break;
		}
	}
	
	// 下の箱
	DrawBox(0, 1080 - 256, 1024, 256, true);	// 背景箱
	if (forSale.isSelect) {
		DrawString(WHITE, 16, 1080 - 256 + 16, greeting.c_str());	// 説明文
	}
	else if (decideMenu.isSelect) {
		DrawString(WHITE, 16, 1080 - 256 + 16, confirmation.c_str());	// 説明文
	}
	else {
		DrawString(WHITE, 16, 1080 - 256 + 16, who.c_str());	// 説明文
	}

	// 右の箱
	if (decideMenu.isDraw) {
		decideMenu.Draw();
	}

	if (charaMenu.isDraw) {
		charaMenu.Draw();
	}

	if (forSale.isSelect) {
		DrawString(WHITE, 0, 32 + forSale.icon.Y * 32 , "●");
	}
	else if (decideMenu.isSelect) {
		DrawString(WHITE, 1024, 32 + 96 + decideMenu.icon.Y * 32, "●");
	}
	else if (charaMenu.isSelect) {
		DrawString(WHITE, 1344, 32 + 96 + charaMenu.icon.Y * 32, "●");
	}
}