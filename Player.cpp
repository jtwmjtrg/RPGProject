#include "Player.h"
#include "SkillTree.h"
#include "MapObject.h"
#include "Game.h"

bool					Player::saveFlag	= false;// セーブフラグ（セーブしたフレームにのみtrueになる）
bool					Player::born		= false;// 生成は一回のみ
int						Player::playerGraph;		// 本体の画像
int						Player::animCount;			// アニメーションの判定
int						Player::animSpeed;			// アニメーションの速度。小さいほうが早い
Position2D				Player::pos_;				// 座標
int						Player::speed;				// 移動スピード※２の冪乗にすること
Position2D				Player::pre_pos_;			// 直前座標
int						Player::chapter;			// ストーリーの進行度
int						Player::stage;				// 今いるステージ
Direction				Player::direction;			// 向き
TalkFlag				Player::talkFlag;			// 話しかけた座標
bool					Player::animation;			// アニメーション中かどうか
Mode					Player::mode;				// メニューを開いているかどうか
int						Player::swapCharacterY;		// ならべかえ
bool					Player::flag_shop;			// 買い物中かどうか
vector<PlayerChara*>	Player::character;			// パーティーメンバー
vector<vector<Item*> >	Player::bag;				// アイテムバッグ
vector<vector<bool> >	Player::isFinish;			// イベントフラグ
int						Player::money;				// 金
ItemBox					Player::mainMenu;			// メインメニューボックス
ItemBox					Player::charaMenu;			// キャラクターメニューボックス
ItemBox					Player::itemMenu;			// アイテムメニューボックス
ItemBox					Player::doMenu;				// Ｄｏメニュー
ItemBox					Player::targetMenu;			// ターゲットメニュー
ItemBox					Player::decideMenu;			// 確認メニュー
int						Player::menuY;				// 大事な座標
int						Player::cover_handle_;		// 半透明な黒

// 一時セーブデータ
map<int, map<int, int>> Player::tmpSave_unitMode;	// ユニットのモード	<マップID <ユニットID , モード>>

// debug
int						Player::emote_lock_;		// エモート連投禁止

void Player::InitPlayer(bool isContinue) {
	if (!character.empty()) {
		DeletePlayer();
	}

	// 生成チェック
	born = true;

	// 画像とアニメーション
	if (playerGraph == 0){
		playerGraph = LoadGraph("img\\player\\kata.png", 4, 4);		// 本体
	}
	animCount = 0;	// アニメーションの判定
	animSpeed = 10;	// アニメーションの速度。小さいほうが早い

	// 続きから
	if (isContinue) {
		// -------------------------------------------------------
		// ファイルオープン
		ifstream ifs("savedata\\savedata.txt");
		// セーブデータがないとき
		if (!ifs) {
			chapter = 0;
			stage = 0;
			pos_.set_pos(CHIPSIZE * 25, CHIPSIZE * 50);	// 開始時の座標

			// debug
			// パーティー
			for (int i = 0, n = character.size(); i < n; ++i) {
				for (int j = 0, m = bag[i].size(); j < m; ++j) {
					if (bag[i][j] != NULL) {
						delete bag[i][j];
					}
				}
				bag[i].clear();
				bag[i].shrink_to_fit();
			}
			bag.clear();
			bag.shrink_to_fit();
			for (int i = 0, n = character.size(); i < n; ++i) {
				delete character[i];
			}
			character.clear();
			character.shrink_to_fit();
			int back = 0;
			character.push_back(new Kata("kata", false));
			bag.resize(character.size());
			// アイテムを持たせる
			Player::InsertItem(back, 11);	// ボロボロの剣
			Player::InsertItem(back, 12);	// 傷んだマント
			// それらを装備
			for (int i = 0;i < 2;++i) {
				Player::bag[back][i]->Effect(Player::character[back]);	// 装備
				Player::bag[back][i]->isEquipment = true;
			}
			for (int i = 0, n = (int)character.size(); i < n; ++i) {
				character[i]->Lv1_Init();		// Lv.1状態への初期化
			}
			money = 10000;

			// debug
			InsertItem(0, 0);
			isFinish.resize(10);
			for (int i = 0;i < 10;++i) {
				isFinish[i].resize(100);
			}
			for (int i = 0;i < 10;++i) {
				for (int j = 0;j < 100;++j) {
					isFinish[i][j] = false;
				}
			}
		}
		// セーブデータがあるとき
		else {
			// 制御系
			int loop = 0;	// ループ回数
			string line;		// 列格納用

			// 列によって処理を分ける
			while (getline(ifs, line)) {
				istringstream stream(line);
				string token;
				vector<string> vStr;
				for (int i = 0;getline(stream, token, ','); i++) {
					vStr.push_back(token);
				}

				// ステータス
				if (loop == 0) {
					chapter = boost::lexical_cast<int>(vStr[0]);
					stage = boost::lexical_cast<int>(vStr[1]);
					pos_.set_pos(boost::lexical_cast<int>(vStr[2]), boost::lexical_cast<int>(vStr[3]));	// セーブされた座標
				}
				// キャラクター
				else if (loop == 1) {
					for (int i = 0, n = vStr.size();i < n;i++) {
						// 名前で呼ぶキャラクターを分ける
						if (vStr[i] == "kata") {
							// ケータ
							character.push_back(new Kata("kata", isContinue));
						}
						else if (vStr[i] == "tee") {
							// ティー
							character.push_back(new Tee("tee", isContinue));
						}

						// 共通処理
						character[i]->order = i;		// 順番
					}

					bag.resize(character.size());
				}
				// アイテム
				else if (loop == 2) {
					for (int i = 0, n = vStr.size();i < n;++i) {
						InsertItem((int)(boost::lexical_cast<int>(vStr[i]) / 10000), (int)((boost::lexical_cast<int>(vStr[i]) % 10000)));
						++i;
						if ((int)(boost::lexical_cast<int>(vStr[i])) == 1) {
							// 装備している
							bag[(int)(boost::lexical_cast<int>(vStr[i - 1]) / 10000)].back()->isEquipment = true;
						}
						else {
							// 装備していない
							bag[(int)(boost::lexical_cast<int>(vStr[i - 1]) / 10000)].back()->isEquipment = false;
						}
					}
				}
				// フラグ
				else if (loop == 3) {
					isFinish.resize(10);
					for (int j = 0;j < 1000;++j) {
						isFinish[(int)(j/100)].push_back(vStr[j] == "1" ? true : false);
					}
				}
				// 所持金
				else if (loop == 4) {
					money = boost::lexical_cast<int>(vStr[0]);
				}
				// ユニットのモードデータ
				else if (loop >= 5) {
					Player::LoadTmpSave_UnitMode(line);
				}

				// カウントアップ
				loop++;
			}
		}

		ifs.close();
		// ファイルクローズ
		// -----------------------------------------------------
	}
	// 初めから
	else {
		chapter = 0;
		stage = 0;
		pos_.set_pos(CHIPSIZE * 25, CHIPSIZE * 50);	// 開始時の座標

		// debug
		// パーティー
		for (int i = 0, n = character.size(); i < n; ++i) {
			for (int j = 0, m = bag[i].size(); j < m; ++j) {
				if (bag[i][j] != NULL) {
					delete bag[i][j];
				}
			}
			bag[i].clear();
			bag[i].shrink_to_fit();
		}
		bag.clear();
		bag.shrink_to_fit();
		for (int i = 0, n = character.size(); i < n; ++i) {
			delete character[i];
		}
		character.clear();
		character.shrink_to_fit();
		int back = 0;
		character.push_back(new Kata("kata", false));
		bag.resize(character.size());
		// アイテムを持たせる
		Player::InsertItem(back, 11);	// ボロボロの剣
		Player::InsertItem(back, 12);	// 傷んだマント
		// それらを装備
		for (int i = 0;i < 2;++i) {
			Player::bag[back][i]->Effect(Player::character[back]);	// 装備
			Player::bag[back][i]->isEquipment = true;
		}
		for (int i = 0, n = (int)character.size(); i < n; ++i) {
			character[i]->Lv1_Init();		// Lv.1状態への初期化
		}
		money = 10000;

		// debug
		InsertItem(0, 0);
		isFinish.resize(10);
		for (int i = 0;i < 10;++i) {
			isFinish[i].resize(100);
		}
		for (int i = 0;i < 10;++i) {
			for (int j = 0;j < 100;++j) {
				isFinish[i][j] = false;
			}
		}
	}

	// 制御系
	direction			= DOWN;		// 一番最初の向き
	pre_pos_ = pos_;				// 直前座標
	animation			= false;	// 入力を受け付けるかどうか
	mode				= FREE;		// プレイヤーの選択状態
	swapCharacterY		= -1;		// ならべかえ
	
	// ステータス
	speed = 8;	// 移動の速さ(アニメーション)

	flag_shop = false;	// ショップ画面かどうか

	mainMenu.itemName.clear();
	mainMenu.itemName.shrink_to_fit();
	charaMenu.itemName.clear();
	charaMenu.itemName.shrink_to_fit();
	itemMenu.itemName.clear();
	itemMenu.itemName.shrink_to_fit();
	doMenu.itemName.clear();
	doMenu.itemName.shrink_to_fit();
	targetMenu.itemName.clear();
	targetMenu.itemName.shrink_to_fit();
	decideMenu.itemName.clear();
	decideMenu.itemName.shrink_to_fit();

	mainMenu.itemName.resize(9);
	mainMenu.itemName[0] = "アイテム";
	mainMenu.itemName[1] = "スキル";
	mainMenu.itemName[2] = "スキルツリー";
	mainMenu.itemName[3] = "ならびかえ";
	mainMenu.itemName[4] = "ステータス";
	mainMenu.itemName[5] = "セーブ";
	mainMenu.itemName[6] = "タイトルにもどる";
	mainMenu.itemName[7] = "エモート";
	mainMenu.itemName[8] = "もどる";
	mainMenu.Initialize(0, 0, 320, 64 + 9 * 32);
	for (int i = 0, n = character.size(); i < n; ++i) {
		// キャラクターの数だけループ
		charaMenu.itemName.push_back(character[i]->GetName());
	}
	charaMenu.Initialize(320, 0, 160, 64 + 9 * 32);
	itemMenu.Initialize(480, 0, 320, 64 + 9 * 32);
	doMenu.itemName.resize(5);
	doMenu.itemName[0] = "使う";
	doMenu.itemName[1] = "渡す";
	doMenu.itemName[2] = "装備";
	doMenu.itemName[3] = "捨てる";
	doMenu.itemName[4] = "戻る";
	doMenu.Initialize(800, 0, 160, 64 + doMenu.itemName.size() * 32);
	for (int i = 0, n = character.size(); i < n; ++i) {
		// キャラクターの数だけループ
		targetMenu.itemName.push_back(character[i]->GetName());
	}
	targetMenu.Initialize(960, 0, 160, 64 + character.size() * 32);
	decideMenu.itemName.push_back("はい");
	decideMenu.itemName.push_back("いいえ");
	decideMenu.Initialize(960, 0, 160, 128);
	menuY = 0;

	cover_handle_ = LoadGraph("img\\cover.png");	// 半透明な黒

	// debug
	emote_lock_ = 0;
}
void Player::DeletePlayer() {
	born = false;	// 終了処理を行った

	// メモリ解放
	for (int i = 0, n = character.size();i < n;i++) {
		delete character[i];
	}
	character.clear();
	character.shrink_to_fit();
	
	// アイテム削除
	for (unsigned int i = 0, n = bag.size();i < n;++i) {
		for (unsigned int j = 0;j < bag[i].size();++j) {
			delete bag[i][j];
		}
		bag[i].clear();
		bag[i].shrink_to_fit();
	}
	bag.clear();
	bag.shrink_to_fit();

	DeleteGraph(cover_handle_);	// 半透明な黒
}

bool Player::Process(){
	// 直前座標を保存
	pre_pos_ = pos_;

	// セーブフラグを切る
	saveFlag = false;

	// 会話フラグを切る
	if (talkFlag.flag) {
		talkFlag.flag = false;
	}

	// 入力を受け付けている状態の場合
	if (!animation){
		// 通常状態
		switch (mode) {
		case FREE:
			/////////////////////////////////////////////////
			// 移動
			/////////////////////////////////////////////////
			if (Input::Get(VK_UP) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 0) {
				// 向きを変えて移動モーション(入力を受け付けない状態)へ
				direction = UP;
				animation = true;
			}
			else if (Input::Get(VK_DOWN) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 0) {
				direction = DOWN;
				animation = true;
			}
			else if (Input::Get(VK_LEFT) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 0) {
				direction = LEFT;
				animation = true;
			}
			else if (Input::Get(VK_RIGHT) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 0) {
				direction = RIGHT;
				animation = true;
			}
			////////////////////////////////////////////////
			// メニューを開く
			/////////////////////////////////////////////////
			if (Input::Get(VK_C) == 1 || Input::Get(0, XINPUT_GAMEPAD_X) == 1) {
				mode		= MENU;		// メニュー画面へ
				// debug
				mainMenu.icon.Y		= 0;
				mainMenu.isSelect	= true;
				mainMenu.isDraw		= true;
				charaMenu.isSelect	= false;
				charaMenu.isDraw	= false;
				itemMenu.isSelect	= false;
				itemMenu.isDraw		= false;
				doMenu.isSelect		= false;
				doMenu.isDraw		= false;
				targetMenu.isSelect = false;
				targetMenu.isDraw	= false;
			}
			////////////////////////////////////////////////
			// ショートカット（話す）
			/////////////////////////////////////////////////
			if (Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				talkFlag.flag = true;
				switch (direction)
				{
				case UP:
					talkFlag.pos_.set_pos(pos_.X, pos_.Y - 64);
					break;
				case DOWN:
					talkFlag.pos_.set_pos(pos_.X, pos_.Y + 64);
					break;
				case LEFT:
					talkFlag.pos_.set_pos(pos_.X - 64, pos_.Y);
					break;
				case RIGHT:
					talkFlag.pos_.set_pos(pos_.X + 64, pos_.Y);
					break;
				default:
					break;
				}
			}
			////////////////////////////////////////////////
			// ショートカット（エモート）
			////////////////////////////////////////////////
			if (emote_lock_ <= 0) {
				if (Input::Get(VK_E) == 1) {
					Emote::SetEmote(GetRand(9), WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 96);	// エモートセット

					emote_lock_ = 30;	// 連投禁止
				}
			}
			else {
				emote_lock_--;
			}
			break;

		case MENU:
			MenuProcess();	// メニュー画面のプロセス
			break;

		default:
			// エラー
			break;
		}
	}
	////////////////////////////////////////////////
	// アニメーション
	/////////////////////////////////////////////////
	else {
		// ちょっとずつ移動
		switch (direction)
		{
		case UP:
			pos_.Y -= speed;
			break;
		case DOWN:
			pos_.Y += speed;
			break;
		case LEFT:
			pos_.X -= speed;
			break;
		case RIGHT:
			pos_.X += speed;
			break;
		default:
			break;
		}

		// マス目ぴったりで移動モーション終了
		if (Player::pos_.X%CHIPSIZE == 0 && Player::pos_.Y%CHIPSIZE == 0) {
			animation = false;
			return true;	//座標を移動したらtrueを返す（エンカウント処理用）
		}
	}
	return false;
}
void Player::MenuProcess() {
	// メニュー画面の処理
	
	switch (menuY) {
	case START:
		Process_Start();
		break;

	case ITEM:
		Process_Item();
		break;

	case SKILL:
		Process_Skill();
		break;

	case SKILLTREE:
		Process_SkillTree();
		break;

	case REARRANGE:
		Process_Rearrange();
		break;

	case STATUS:
		Process_Status();
		break;

	case SAVE:
		Process_Save();
		break;

	case BACK_TITLE:
		Process_Back_Title();
		break;

	case EMOTE:
		Process_Emote();
		break;

	case BACK:
		Process_Back();
		break;

	default:
		// エラー
		break;
	}

	// メニュー終了
	if (Input::Get(VK_C) == 1 || Input::Get(0, XINPUT_GAMEPAD_X) == 1) {
		// 初期化
		mode				= FREE;

		itemMenu.itemName.clear();
		itemMenu.itemName.shrink_to_fit();

		// debug
		menuY				= START;
		mainMenu.isSelect	= false;
		mainMenu.isDraw		= false;
		charaMenu.isSelect	= false;
		charaMenu.isDraw	= false;
		itemMenu.isSelect	= false;
		itemMenu.isDraw		= false;
		doMenu.isSelect		= false;
		doMenu.isDraw		= false;
		targetMenu.isSelect = false;
		targetMenu.isDraw	= false;
		decideMenu.isSelect = false;
		decideMenu.isDraw	= false;
	}
}

void Player::Process_Start() {
	if (mainMenu.isSelect) {
		// メインメニューの処理
		mainMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定

			switch (mainMenu.icon.Y + 1) {
				// 何を選択したか

			case START:
				// スタートメニュー
				// エラー
				break;

			case ITEM:
				// アイテムメニュー
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				itemMenu.isDraw = true;
				itemMenu.isSelect = true;
				menuY = ITEM;
				break;

			case SKILL:
				// スキルメニュー
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				itemMenu.isDraw = true;
				itemMenu.isSelect = true;
				menuY = SKILL;
				break;

			case SKILLTREE:
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				menuY = SKILLTREE;
				break;

			case REARRANGE:
				// 並べ替えメニュー
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				menuY = REARRANGE;	// ならべかえ画面へ
				break;

			case STATUS:
				// ステータスメニュー
				menuY = STATUS;		// ステータス画面へ
				break;

			case SAVE:
				// セーブメニュー
				menuY = SAVE;
				break;

			case BACK_TITLE:
				// タイトルに戻る
				mainMenu.isSelect	= false;
				decideMenu.icon.Y	= 1;
				decideMenu.isSelect = true;
				decideMenu.isDraw	= true;		// 確認メッセージ
				TextBox::AddLine("セーブされていない進行は失われてしまいます");	// 警告
				menuY = BACK_TITLE;
				break;

			case EMOTE:
				// エモート
				mainMenu.isSelect = false;
				menuY = EMOTE;
				break;

			case BACK:
				// 戻る
				mainMenu.icon.Y = 0;
				mainMenu.isSelect = true;
				mainMenu.isDraw = true;
				charaMenu.isSelect = false;
				charaMenu.isDraw = false;
				itemMenu.isSelect = false;
				itemMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				mode = FREE;	// メニュー画面を閉じる
				break;

			default:
				// 例外
				// エラー
				break;
			}

			mainMenu.isSelect = false;		// 操作受付を切る
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			menuY = 0;
			mainMenu.isSelect = false;
			mainMenu.isDraw = false;
			mode = FREE;		// メニュー画面を閉じる
		}
	}
}
void Player::Process_Item() {
	if (itemMenu.isSelect) {
		// アイテムメニューの処理
		if (charaMenu.isSelect) {
			// キャラクターメニューの処理
			charaMenu.Process();

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// 現在選んでいるバッグのアイテムをメニュー画面にコピー
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// 決定

				if (Player::bag[charaMenu.icon.Y].empty()) {
					// バッグが空
					TextBox::AddLine("アイテムを持っていません・・・");
					return;
				}
				itemMenu.icon.Y = 0;
				charaMenu.isSelect = false;
			}

			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// 戻る
				itemMenu.itemName.clear();
				itemMenu.itemName.shrink_to_fit();
				mainMenu.isSelect	= true;
				charaMenu.isDraw	= false;
				charaMenu.isSelect	= false;
				itemMenu.isDraw		= false;
				itemMenu.isSelect	= false;
				menuY				= START;
			}
		}
		else {
			// アイテムメニューの処理
			itemMenu.Process();

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// 決定
				doMenu.isDraw = true;
				doMenu.isSelect = true;
				itemMenu.isSelect = false;
			}

			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// 戻る
				charaMenu.isSelect = true;
			}
		}
	}
	else if (doMenu.isSelect) {
		// Ｄｏメニューの処理
		doMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定

			Item* selectedItem = bag[charaMenu.icon.Y][itemMenu.icon.Y];	// エイリアス

			switch (doMenu.icon.Y) {
			case 0:
				// 使う

				if (selectedItem->type == ItemType::EQUIPMENT
					|| selectedItem->type == ItemType::IMPORTANT) {
					// 装備だった場合
					TextBox::AddLine("いまはつかえない");	// 実行失敗メッセージ
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					itemMenu.isSelect = true;
					return;
				}

				// 使用可能アイテムの処理
				if (selectedItem->GetIsArea()) {
					// 全体効果
					UseItem(selectedItem);	// 効果発動

					doMenu.isSelect = false;
					doMenu.isDraw = false;
					itemMenu.isSelect = true;

					if (bag[charaMenu.icon.Y].empty()) {
						// アイテムがなくなった
						doMenu.icon.Y = 0;
						itemMenu.isSelect = false;
						itemMenu.isDraw = false;
					}
				}
				else {
					// 単体効果
					targetMenu.isDraw = true;
					targetMenu.isSelect = true;
					doMenu.isSelect = false;
				}
				break;

			case 1:
				// 渡す
				if (selectedItem->isEquipment) {
					// 装備中のアイテムは渡せない
					TextBox::AddLine("装備中です");		// 実行失敗メッセージ
					return;
				}
				doMenu.isSelect = false;
				targetMenu.isDraw = true;
				targetMenu.isSelect = true;
				break;

			case 2:
				// 装備
				if (selectedItem->type != ItemType::EQUIPMENT) {
					// 装備品でない
					TextBox::AddLine("装備できません");	// 選んだ行動を保存
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				for (int i = 0, n = bag[charaMenu.icon.Y].size(); i < n; ++i) {
					// 同じタイプの装備を既に付けていないかチェックする
					if (i == itemMenu.icon.Y)continue;	// 自分はスキップする

					if (selectedItem->equipmentType == bag[charaMenu.icon.Y][i]->equipmentType) {
						// 見つけた場合
						if (bag[charaMenu.icon.Y][i]->isEquipment == true) {
							// 装備を外す
							bag[charaMenu.icon.Y][i]->isEquipment = false;	// 装備を外す
							break;
						}
					}
				}
				selectedItem->Effect(character[charaMenu.icon.Y]);	// 装備
				TextBox::AddLine("装備");	// ログを流す
				itemMenu.isSelect = true;	// アイテムメニューに操作を戻す
				doMenu.isSelect = false;	// フラグを折る
				doMenu.isDraw = false;
				break;

			case 3:
				// 捨てる
				if (selectedItem->isEquipment) {
					// 装備中のアイテムは捨てられない
					TextBox::AddLine("装備中です");		// 実行失敗メッセージ
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				if (selectedItem->type == ItemType::IMPORTANT) {
					// それを捨てるなんてとんでもない！
					TextBox::AddLine("それを捨てるなんてとんでもない！");	// 実行失敗メッセージ
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				decideMenu.icon.Y	= 1;		// いいえをデフォルトに
				decideMenu.isDraw	= true;		// 最後の選択画面へ
				decideMenu.isSelect = true;
				doMenu.isSelect		= false;
				break;

			case 4:
				// 戻る
				itemMenu.isSelect = true;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
				break;

			default:
				// エラー
				break;
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// 現在選んでいるバッグのアイテムをメニュー画面にコピー
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			itemMenu.isSelect = true;
			doMenu.isSelect = false;
			doMenu.isDraw = false;
		}
	}
	else if (targetMenu.isSelect) {
		// ターゲットメニューの処理
		targetMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定

			Item* selectedItem = bag[charaMenu.icon.Y][itemMenu.icon.Y];	// エイリアス

			switch (doMenu.icon.Y) {
			case 0:
				// 使用
				UseItem(selectedItem);	// 効果発動

				itemMenu.isSelect = true;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;

				if (bag[charaMenu.icon.Y].empty()) {
					// アイテムがなくなった
					charaMenu.isSelect = true;
					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
				}
				break;

			case 1:
				// 渡す

				if (InsertItem(targetMenu.icon.Y, selectedItem->GetID())) {
					// 渡した
					TextBox::AddLine("渡した");

					bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);	// 配列の再整列

					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;

					if (bag[charaMenu.icon.Y].empty()) {
						// アイテムがなくなった
						charaMenu.isSelect = true;
						itemMenu.isSelect = true;
						targetMenu.isSelect = false;
						targetMenu.isDraw = false;
						doMenu.isSelect = false;
						doMenu.isDraw = false;
					}
				}
				else {
					TextBox::AddLine("持ち物がいっぱいです");

					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
				}

				break;

			default:
				// エラー
				break;
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// 現在選んでいるバッグのアイテムをメニュー画面にコピー
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
			doMenu.isSelect = false;
			doMenu.isDraw = false;
		}
	}
	else if (decideMenu.isSelect) {
		// 確認メニューの処理
		decideMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定

			if (decideMenu.icon.Y == 0) {
				// はい
				delete bag[charaMenu.icon.Y][itemMenu.icon.Y];
				bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);
			}
			else {
				// いいえ
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// 現在選んでいるバッグのアイテムをメニュー画面にコピー
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}

			itemMenu.icon.Y = 0;
			itemMenu.isSelect = true;
			decideMenu.isDraw = false;
			decideMenu.isSelect = false;
			doMenu.isDraw = false;
			doMenu.isSelect = false;

			if (bag[charaMenu.icon.Y].empty()) {
				// アイテムがなくなった
				charaMenu.isSelect = true;
				itemMenu.isSelect = true;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			decideMenu.isDraw	= false;		// 最後の選択画面へ
			decideMenu.isSelect = false;
			doMenu.isSelect		= true;
		}
	}
}
void Player::Process_Skill() {
	if (itemMenu.isSelect) {
		// キャラクターを選ぶ前の処理

		if (charaMenu.isSelect) {
			// キャラ選択中
			charaMenu.Process();

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::character[charaMenu.icon.Y]->GetSkillList().size(); i < n; ++i) {
				// 現在選んでいるバッグのアイテムをメニュー画面にコピー
				if (Player::character[charaMenu.icon.Y]->GetSkillList()[i]->fieldFlag) {
					itemMenu.itemName.push_back(Player::character[charaMenu.icon.Y]->GetSkillList()[i]->name);
				}
			}

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// 決定
				if (itemMenu.itemName.empty()) {
					// スキルを持っていない
					TextBox::AddLine("スキルを所持していません…");	// 警告
					return;
				}
				else {
					// 持っている
					itemMenu.icon.Y = 0;
					itemMenu.isDraw = true;
					itemMenu.isSelect	= true;
					charaMenu.isSelect	= false;
				}
			}
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// 戻る
				menuY = START;
				mainMenu.isSelect = true;
				charaMenu.isDraw = false;
				charaMenu.isSelect = false;
				itemMenu.isDraw = false;
				itemMenu.isSelect = false;
			}
		}
		else {
			itemMenu.Process();

			if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
				// 選んだアイテム座標を保存
				targetMenu.isDraw = true;
				targetMenu.isSelect = true;
				itemMenu.isSelect = false;
			}
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// 戻る
				charaMenu.isSelect = true;
				itemMenu.isDraw = false;
			}
		}
	}
	else if (targetMenu.isSelect) {
		// キャラクターを選んだ後の処理
		targetMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定
			for (int i = 0, n = Player::character[charaMenu.icon.Y]->GetSkillList().size(); i < n; ++i) {
				// 名前の完全一致を検索
				if (Player::character[charaMenu.icon.Y]->GetSkillList()[i]->name == itemMenu.itemName[itemMenu.icon.Y]) {
					// 名前が一致したスキルの効果をすべて発動させる
					//for (int j = 0, m = character[charaMenu.icon.Y]->GetSkillList()[i]->effect.size(); j < m; ++j) {
					//	character[charaMenu.icon.Y]->GetSkillList()[i]->effect[j]->Execution_Field(character[charaMenu.icon.Y], character[targetMenu.icon.Y]);
					//}
					character[charaMenu.icon.Y]->GetSkillList()[i]->Execution_Field(character[charaMenu.icon.Y], character, character[targetMenu.icon.Y]);

					break;
				}
			}

			vector<string> a;
			character[targetMenu.icon.Y]->GetEffectText(a);
			if (a.empty()) {
				// メッセージがない
			}
			else {
				// メッセージ有り
				TextBox::AddLine(a);
			}
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
		}
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
		}
	}
}
void Player::Process_SkillTree() {

	// キャラ選択中
	charaMenu.Process();

	if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
		|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
		// 決定
		SkillTree::SetData(Player::character[charaMenu.icon.Y]);
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
	}
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		// 戻る
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
	}

}
void Player::Process_Rearrange() {
	// 並べ替え
	if (charaMenu.isSelect) {
		charaMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定
			if (swapCharacterY == -1) {
				// 移動元
				swapCharacterY = charaMenu.icon.Y;	// 座標を保存
			}
			else {
				// 移動先
				PlayerChara* pc = character[swapCharacterY];				// 一時避難	
				character[swapCharacterY] = character[charaMenu.icon.Y];	// 移動先を移動
				character[charaMenu.icon.Y] = pc;							// 移動元を移動
				bag[swapCharacterY].swap(bag[charaMenu.icon.Y]);			// 荷物も入れ替え
																			// 並び順
				for (int i = 0, n = character.size(); i < n; i++) {
					character[i]->order = i;								// 並び順を更新
				}
				swapCharacterY = -1;										// 初期化

				charaMenu.itemName.clear();
				charaMenu.itemName.shrink_to_fit();
				for (int i = 0, n = character.size(); i < n; ++i) {
					// キャラクターの数だけループ
					charaMenu.itemName.push_back(character[i]->GetName());
				}
				targetMenu.itemName.clear();
				targetMenu.itemName.shrink_to_fit();
				for (int i = 0, n = character.size(); i < n; ++i) {
					// キャラクターの数だけループ
					targetMenu.itemName.push_back(character[i]->GetName());
				}
				targetMenu.Initialize(960, 0, 160, 64 + character.size() * 32);
			}
		}
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// 戻る
			menuY = START;
			mainMenu.isSelect = true;
			charaMenu.isDraw = false;
			charaMenu.isSelect = false;
			itemMenu.isDraw = false;
			itemMenu.isSelect = false;
		}
	}
}
void Player::Process_Status() {
	// ステータス表示
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		// 戻る
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
		itemMenu.isDraw = false;
		itemMenu.isSelect = false;
	}
}
void Player::Process_Save() {
	Save();
	TextBox::AddLine("セーブしました。");
	mainMenu.isSelect = true;
	menuY = START;
}
void Player::Process_Back_Title() {
	// タイトルに戻る

	if (decideMenu.isSelect) {
		// 本当にもどるかの確認
		
		decideMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// 決定キー入力

			if (decideMenu.icon.Y == 0) {
				// はい

				Scene::nowScene = GameScene::TITLE;
			}
			else {
				// いいえ

				decideMenu.Initialize(960, 0, 160, 128);	// 初期化
				mainMenu.isSelect = true;
				menuY = 0;
			}
		}
		else if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// キャンセル

			if (decideMenu.icon.Y == 0) {
				// はいを選んでいるとき

				decideMenu.icon.Y = 1;	// いいえに移動
			}
			else {
				// いいえを選んでいるとき

				decideMenu.Initialize(960, 0, 160, 128);	// 初期化
				mainMenu.isSelect = true;
				menuY = 0;
			}
		}
	}
}
void Player::Process_Emote() {
	// エモート

	Emote::SetEmote(GetRand(9), WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 96);	// エモートセット

	mainMenu.isSelect = false;
	mainMenu.isDraw = false;
	menuY = START;
	mode = FREE;
}
void Player::Process_Back() {

}

void Player::Draw(bool isFollow, int cameraX, int cameraY){
	// アニメーション処理
	if (animCount < animSpeed * 4){
		animCount++;
	}
	else{
		animCount = 0;
	}

	// プレイヤーの画像
	if (isFollow) {
		DrawGraph(CHIPSIZE * 15, CHIPSIZE * 8, playerGraph, (int)(animCount / animSpeed) % 4, direction);
	}
	else {
		DrawGraph(CHIPSIZE * (pos_.X / 64) - cameraX, CHIPSIZE * (pos_.Y / 64) -cameraY, playerGraph, (int)(animCount / animSpeed) % 4, direction);
	}

	// メニュー
	switch (mode) {
	case FREE:
		// 何もなし
		break;

	case MENU:
		// メインメニュー

		// 後ろを半透明な黒でカバー
		DrawGraph(0, 0, cover_handle_);
		
		if (mainMenu.isDraw) {
			// メインメニュー描写
			mainMenu.Draw();	// 描写
			DrawBox(320, 0, 160, 64, true);	// お金の箱
			DrawString(WHITE, 320 + 16, 16, to_string(money).c_str());
			DrawString(WHITE, 320 + 160 -32, 16, "G");
		}

		if (charaMenu.isDraw) {
			// キャラクターメニュー描写
			charaMenu.Draw();
		}

		if (itemMenu.isDraw) {
			// アイテムメニュー描写
			itemMenu.Draw();
		}

		if (doMenu.isDraw) {
			// どぅメニュー描写
			doMenu.Draw();
		}

		if (targetMenu.isDraw) {
			// ターゲットメニュー描写
			targetMenu.Draw();
		}

		if (decideMenu.isDraw) {
			// 確認メニュー描写
			decideMenu.Draw();
		}

		if (mainMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, mainMenu.pos.X + 16, mainMenu.pos.Y + 32 + (mainMenu.icon.Y * 32), "●");
		}
		else if (charaMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, charaMenu.pos.X + 16, charaMenu.pos.Y + 32 + (charaMenu.icon.Y * 32), "●");
		}
		else if (itemMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, itemMenu.pos.X + 16, itemMenu.pos.Y + 32 + (itemMenu.icon.Y * 32), "●");

			// 説明
			DrawBox(0, 352, 800, 160, true);

			if ((unsigned)itemMenu.icon.Y >= bag[charaMenu.icon.Y].size()) {
				// 範囲外
				itemMenu.icon.Y = 0;
			}

			if (menuY == ITEM) {
				// アイテム選択中

				DrawString(WHITE, 16, 352 + 16, bag[charaMenu.icon.Y][itemMenu.icon.Y]->GetInfo().c_str());
			}
			else if (menuY = SKILL) {
				// スキル選択中

				for (int i = 0, n = character[charaMenu.icon.Y]->GetSkillList_Field()[itemMenu.icon.Y]->explanatoryText.size();i < n;++i) {
					DrawString(WHITE, 16, 352 + 16 + (32 * i), (character[charaMenu.icon.Y]->GetSkillList_Field()[itemMenu.icon.Y]->explanatoryText[i]).c_str());
				}
			}
		}
		else if (doMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, doMenu.pos.X + 16, doMenu.pos.Y + 32 + (doMenu.icon.Y * 32), "●");
		}
		else if (targetMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, targetMenu.pos.X + 16, targetMenu.pos.Y + 32 + (targetMenu.icon.Y * 32), "●");
		}
		else if (decideMenu.isSelect) {
			// 操作受付中
			DrawString(WHITE, decideMenu.pos.X + 16, decideMenu.pos.Y + 32 + (decideMenu.icon.Y * 32), "●");
		}

		// ステータス
		for (int i = 0, n = character.size(); i < n; i++) {
			BattleStatus s = character[i]->GetBattleStatus();
			DrawBox(i * 300, 919, 300, 160, true);
			DrawString(WHITE, 16 + i * 300, 919 + 16, (character[i]->GetName().c_str()));	// 1
			DrawString(WHITE, 16 + i * 300, 919 + 48, " HP: %d / %d", s.HP, s.maxHP);		// 2
			DrawString(WHITE, 16 + i * 300, 919 + 80, " MP: %d / %d", s.MP, s.maxMP);		// 3
			DrawString(WHITE, 16 + i * 300, 919 + 112, " LV: %d", character[i]->GetLV());	// 4
		}

		switch (menuY) {
			// メインメニューで何を選んだかで表示を変える
		case START:
			Draw_Start();
			break;

		case ITEM:
			Draw_Item();
			break;

		case SKILL:
			Draw_Skill();
			break;

		case SKILLTREE:
			Draw_SkillTree();
			break;

		case REARRANGE:
			Draw_Rearrange();
			break;

		case STATUS:
			Draw_Status();
			break;

		case SAVE:
			Draw_Save();
			break;

		case BACK:
			Draw_Back();
			break;

		default:

			// エラー
			break;
		}

		break;

	default:
		// エラー
		break;
	}

	/*Position2D pos;
	pos.X = 100;pos.Y = 100;
	DrawDeformationGraph(playerGraph, 100, 100, 200, 100, 200, 200, 100, 200);*/
}

void Player::Draw_Start() {

}
void Player::Draw_Item() {
	for (int i = 0, n = bag[charaMenu.icon.Y].size(); i < n; ++i) {
		if (bag[charaMenu.icon.Y][i]->isEquipment) {
			DrawString(WHITE, 480 + 320 - 64, 32 + i * 32, "[E]");
		}
	}
}
void Player::Draw_Skill() {

}
void Player::Draw_SkillTree() {

}
void Player::Draw_Rearrange() {
	// 並べ替え
	if (swapCharacterY != -1) {
		// 移動元
		DrawString(WHITE, 336, swapCharacterY * 32 + 32, "▲");	// アイコン
	}
}
void Player::Draw_Status() {
	// ステータス表示
	for (int i = 0, n = character.size();i < n;i++) {
		// キャラクタと同じ数だけループ
		BattleStatus s = character[i]->GetFixed();	// エイリアス
		// 上の箱
		DrawBox(300 + 300 * i, 0, 300, 64 + 32 * 12, true);
		DrawString(WHITE, 300 + 16 + i * 300, 32, (character[i]->GetName().c_str()));									// 1
		DrawString(WHITE, 300 + 16 + i * 300, 64, " HP: %d / %d", character[i]->GetStatus(eStatus::HP), s.maxHP);	// 2
		DrawString(WHITE, 300 + 16 + i * 300, 96, " MP: %d / %d", character[i]->GetStatus(eStatus::MP), s.maxMP);	// 3
		DrawString(WHITE, 300 + 16 + i * 300, 128, " ATK: %d", s.STR);											// 4
		DrawString(WHITE, 300 + 16 + i * 300, 160, " DEX: %d", s.DEX);											// 5
		DrawString(WHITE, 300 + 16 + i * 300, 192, " AGI : %d", s.AGI);											// 6
		DrawString(WHITE, 300 + 16 + i * 300, 224, " LUC: %d", s.LUC);											// 7
		DrawString(WHITE, 300 + 16 + i * 300, 256, " 物理防御力: %d", s.DEF_PHYS);									// 8
		DrawString(WHITE, 300 + 16 + i * 300, 288, " 熱防御力　 : %d", s.DEF_HEAT);									// 9
		DrawString(WHITE, 300 + 16 + i * 300, 320, " 冷気防御力: %d", s.DEF_COLD);									// 10
		DrawString(WHITE, 300 + 16 + i * 300, 352, " 電気防御力: %d", s.DEF_ELEC);									// 11
		DrawString(WHITE, 300 + 16 + i * 300, 384, " MUE: %d", character[i]->GetMaxUseEnergy());						// 12
		// 下の箱
		DrawBox(300 + 300 * i, 64 + 32 * 12, 300, 64 + 32 * 4, true);
		DrawString(WHITE, 300 + 16 + i * 300, 460, "%s", character[i]->GetWeaponName().c_str());
		DrawString(WHITE, 300 + 16 + i * 300, 492, "%s", character[i]->GetArmorName().c_str());
		DrawString(WHITE, 300 + 16 + i * 300, 524, "%s", character[i]->GetAmuletName().c_str());
	}
}
void Player::Draw_Save() {

}
void Player::Draw_Back_Title() {
	// タイトルに戻る
}
void Player::Draw_Emote() {
	// エモート


}
void Player::Draw_Back() {

}

void Player::PosCorrection(){
	// 座標修正
	pos_ = pre_pos_;
	animation = false;
}

bool Player::InsertItem(int item_id) {
	// 先頭から見て、空きを見つけ次第、そこにアイテムを格納する

	Item* ip = Item::Factory(item_id);	// チェック用

	if (!ip) {
		// 存在しないアイテム
		return false;		// 失敗
	}
	else {
		for (int i = 0, n = character.size();i < n;++i) {
			// 先頭から順番に確認する

			if (bag[i].size() == 9) { // アイテムがいっぱい
				continue;
			}
			else {
				bag[i].push_back(ip);	// バッグにアイテムを格納

				return true;	// 成功
			}
		}
	}

	// 失敗
	return false;
}
bool Player::InsertItem(int order, int id) {
	if (bag[order].size() == 9) {
		// アイテムがいっぱい
		return false;		// 失敗
	}

	Item* ip = Item::Factory(id);	// チェック用

	if (!ip) {
		// 存在しないアイテム
		return false;		// 失敗
	}
	else {
		bag[order].push_back(ip);
	}

	// 成功
	return true;
}

// 金を手に入れる
void Player::AddMoney(int value){
	if (value < 0) return;	// マイナス値は渡せない
	money += value;
	// 所持上限999,999Ｇ
	if (money > 999999) {
		money = 999999;
	}
}
// 金を払う
void Player::PayMoney(int value) {
	if (value < 0) return;	// マイナス値は渡せない
	money -= value;
	// 0以下にならないように
	if (money < 0) {
		money = 0;
	}
}
// 金のゲッター
int Player::GetMoney() {
	return money;
}

// 【一時セーブデータ】
// ユニットのモードの一時セーブ
void Player::TmpSave_UnitMode(int saveStage, const std::map<int, MapUnit*> unitData) {
	if (tmpSave_unitMode.find(saveStage) == tmpSave_unitMode.end()) {
		// 【セーブされたデータがない場合】
		map<int, int> tmp;
		tmpSave_unitMode[saveStage] = tmp;	// 追加
	}
	// データを追加（上書き）
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		tmpSave_unitMode[saveStage][itr->first] = itr->second->GetMode();
	}
}
void Player::LoadTmpSave_UnitMode(string line) {
	//ロード用変数
	std::istringstream stream(line);
	string token;

	int tmp_mapID = 0;
	int tmp_unitID = 0;
	int tmp_modeNum = 0;

	// 【マップID】
	getline(stream, token, ',');
	tmp_mapID = boost::lexical_cast<int>(token);
	if (tmpSave_unitMode.find(tmp_mapID) == tmpSave_unitMode.end()) {
		// 【セーブされたデータがない場合】
		map<int, int> tmp;
		tmpSave_unitMode[tmp_mapID] = tmp;	// 追加
	}
	// 【ID-モード】
	while (getline(stream, token, ',')) {
		if (token == "") break;
		// 【ユニットID】
		tmp_unitID = boost::lexical_cast<int>(token);
		// 【モード】
		getline(stream, token, ',');
		tmp_modeNum = boost::lexical_cast<int>(token);
		tmpSave_unitMode[tmp_mapID][tmp_unitID] = tmp_modeNum;
	}
}
// ユニットのモードのセット
bool Player::SetUnitMode_TmpSave(std::map<int, MapUnit*>& unitData) {
	if (tmpSave_unitMode.find(stage) == tmpSave_unitMode.end()) {
		//【セーブデータなし】
		return false;
	}
	else{
		// 【セーブされたデータがある場合】
		for (auto itr = tmpSave_unitMode[stage].begin(), end = tmpSave_unitMode[stage].end(); itr != end; ++itr) {
			unitData[itr->first]->SetMode(itr->second);
		}
	}
	return true;
}
void Player::Save() {
	// ---------------------------------------------------------
	// ファイルオープン
	ofstream ofs("savedata\\savedata.txt");
	if (!ofs) {
		// 初めてセーブするときに来る場所
	}

	// データ入力
	ofs << chapter << ',';
	ofs << stage << ',';
	ofs << pos_.X << ',';
	ofs << pos_.Y << endl;
	// 並び順
	bool a = false;
	for (int i = 0, n = character.size();i < n;i++) {
		if (a) {
			ofs << ',';
		}
		ofs << character[i]->GetName();
		if (!a) {
			a = true;
		}
	}
	ofs << endl;
	// アイテム
	bool b = false;
	for (int i = 0, n = bag.size();i < n;i++) {
		for (unsigned int j = 0;j < bag[i].size();++j) {
			if (b) {
				ofs << ',';
			}
			ofs << (i * 10000) + (bag[i][j]->GetID())	<< ',';	// (キャラクターの並び順*10000)+(アイテムのID)
			ofs << bag[i][j]->isEquipment ? 1 : 0;
			if (!b) {
				b = true;
			}
		}
	}
	ofs << endl;

	// フラグ
	bool c = false;
	for (int i = 0, n = 1000;i < n;++i){
		if (c) {
			ofs << ',';
		}
		ofs << isFinish[(int)(i/100)][i%100] ? '1' : '0';
		if (!c) {
			c = true;
		}
	}
	ofs << endl;
	// 所持金
	ofs << money << endl;

	// ユニットのモードデータ
	Game::TmpSave_UnitMode();	// 今いるマップのユニットをセーブ
	for (auto itr = tmpSave_unitMode.begin(), end = tmpSave_unitMode.end(); itr != end; ++itr) {
		// マップID
		ofs << itr->first << ',';
		for (auto itr_2 = itr->second.begin(), end_2 = itr->second.end(); itr_2 != end_2; ++itr_2) {
			ofs << itr_2->first << ',' << itr_2->second << ',';
		}
		ofs << endl;
	}

	// ファイルクローズ
	ofs.close();
	// 
	// ---------------------------------------------------------

	// キャラクターデータのセーブ
	for (int i = 0, n = character.size();i < n;i++) {
		character[i]->Save();
	}

	// 正常終了を知らせる
	//TextBox::AddLine("セーブしました！");

	// セーブフラグを立てる
	saveFlag = true;
}

void Player::UseItem(Item* item) {
	TextBox::AddLine(bag[charaMenu.icon.Y][itemMenu.icon.Y]->GetName() + " を使用した ");	// ログ

	switch ((int)item->type) {
	case (int)ItemType::IMPORTANT:
		// 重要アイテム
		// 使用されることはない
		break;

	case (int)ItemType::RECOVERY:
		// 回復アイテム
		if (item->GetIsArea()) {
			for (int i = 0, n = character.size();i < n;++i) {
				item->Effect(character[i]);
			}
		}
		else {
			item->Effect(character[targetMenu.icon.Y]);
		}
		break;

	case (int)ItemType::DAMAGE:
		// 攻撃アイテム
		item->Effect(character[targetMenu.icon.Y]);
		break;

	case (int)ItemType::EQUIPMENT:
		// 装備アイテム
		// 使用されることはない
		break;

	case (int)ItemType::LIFESTEAL:
		item->Effect(character[targetMenu.icon.Y], character[charaMenu.icon.Y]);

	default:
		TextBox::AddLine("使用が想定されていないアイテムです。");
		break;
	}

	delete bag[charaMenu.icon.Y][itemMenu.icon.Y];									// アイテム削除
	bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);	// 配列の再整列
}