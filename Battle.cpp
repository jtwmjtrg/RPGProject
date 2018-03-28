#include "Battle.h"

Battle::Battle() {
	turnNum = 0;	// ターン数

	// 【画像】
	Character::CommonInitBattle();	// キャラクタ戦闘共通初期化処理
	PlayerChara::InitBattle();	// プレイヤーの戦闘初期化
	Enemy::InitBattle();	// 敵のバトル用の画像読み込み
	{// 背景
		string str__S = "img\\battle\\back\\" + to_string(encounter.back) + ".png";	// アドレス生成
		char* cstr = new char[str__S.size() + 1];			// メモリ確保
		strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// コピー
		gr_Back = LoadGraph(cstr);
		delete cstr;	// メモリ解放
	}
	gr_SkillSelect = LoadGraph("img\\battle\\skillSelect.png");	// スキル選択ウィンドウ
	gr_scroll = LoadGraph("img\\battle\\scroll.png");			// スクロールバー
	
	// スキルの当たり判定の画像
	gr_SkillHit1	= LoadGraph("img\\battle\\skillHit1.png");
	gr_SkillHit2	= LoadGraph("img\\battle\\skillHit2.png");
	gr_SkillHit3	= LoadGraph("img\\battle\\skillHit3.png");
	gr_SkillHit4	= LoadGraph("img\\battle\\skillHit4.png");
	gr_target		= LoadGraph("img\\battle\\target.png");
	gr_NA			= LoadGraph("img\\battle\\skill\\NA_sword.png", 3, 1);
	
	DamageEffect::Init();	// ダメージ数エフェクトの初期化
	
	// プレイヤー情報のロード
	player = Player::character;

	// 【プレイヤーフィールド初期化】4×((3*人数)+1)
	playerFieldHeight = 4;
	playerFieldWidth = player.size() * 3 + 1;

	playerField.resize(playerFieldHeight);
	for (int i = 0; i != playerFieldHeight; ++i) {
		playerField[i].resize(playerFieldWidth);
	}
	// 全マスnullptrで初期化
	for (int i = 0; i != playerFieldHeight; ++i) {
		for (int j = 0; j != playerFieldWidth; ++j) {
			playerField[i][j] = nullptr;
		}
	}
	// 【スキル対象保存用変数初期化】
	playerField_Save.resize(playerFieldHeight);
	for (int i = 0; i != playerFieldHeight; ++i) {
		playerField_Save[i].resize(playerFieldWidth);
	}
	for (int i = 0; i != playerFieldHeight; ++i) {
		for (int j = 0; j != playerFieldWidth; ++j) {
			playerField_Save[i][j] = false;
		}
	}

	// エンカウントデータ読み込み
	LoadEncounter();

	// 【スキルオーダー】
	skillOrder = new SkillOrder();
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		enemy[i]->SetPointer(skillOrder, this);
	}
	for (int i = 0, n = player.size(); i != n; ++i) {
		player[i]->SetPointer(skillOrder, this);
	}

	// 敵の死亡者リストのリセット
	Enemy::deathList.clear();
	Enemy::deathList.shrink_to_fit();

	// プレイヤーの並び順 & 当たり判定の作成
	playerHate.resize(player.size());
	for (int i = 0, n = player.size(); i != n; ++i) {
		player[i]->order = i;
		player[i]->SetColision(playerField);
		if (player[i]->GetAlive() == eAlive::alive) {
			playerHate[i] = 1000;	// ヘイト値（初期値1000）
		}
		else {
			playerHate[i] = 0;	// ヘイト0
		}
	}
	maxHate = 999999;	// ヘイト値上限

	// 【コマンド選択画面の変数初期化】
	selectChara = 0;
	selectAction = 0;
	ActionNum = 5;
	selectSkill = 0;
	selectSkillCursor = 0;
	selectSkillArea = 0;
	skillNum = 0;
	selectTargetX = 0;
	selectTargetY = 0;
	selectTargetAlly = 0;

	// 前ターンに選択したカーソルの位置を記憶しておく構造体初期化
	commandPreSelect.resize(player.size());
	for (int i = 0, n = player.size(); i < n; ++i) {
		commandPreSelect[i] = { 0,0,0,enemyFieldWidth/2,enemyFieldHeight-2, 0, false };
	}

	// 【戦利品初期化】
	exp = 0;
	money = 0;

	battleEndType = eEndStatus::Lose;		// 戦闘終了の状態

	// 特殊スキル作成
	skill_Defence = new Skill_Defence();	// 防御
	skill_None = new Skill_None();	// 「なにもしない」スキル
	skill_Escape = new Skill_Escape();	// 逃走

	skillStep = true;		// true:スキルエフェクト表示	false:スキル発動

	// フィールドのMP
	fieldMaxMP = 1000;
	fieldMP = fieldMaxMP;

	// 戦闘開始画面に移行
	StepChange_START();

	// debug

	// suzuki_debug
	selectItem.pos.X	= 0;
	selectItem.pos.Y	= 0;
	selectItem.flag		= false;
	usedItem.resize(Player::character.size());
	for (int i = 0, n = usedItem.size(); i < n; ++i) {
		usedItem[i].player = 0;
		usedItem[i].item.X = 0;
		usedItem[i].item.Y = 0;
		usedItem[i].isUse = false;
	}
}

// エンカウントデータの読み込み
void Battle::LoadEncounter() {
	// 【ファイル読み込み用変数】
	ifstream L_File;	// 読み込むファイル
	string L_Line;		// 読み込んだ行（1行）
	int L_Count = 0;	// ノード読み込み用（何行目を読み込んでいるか）
	string L_token;		// 1行を,で分割する際に使う

	int L_EnemyID;
	int L_EnemyX;
	int L_EnemyY;

	// 読み込むファイルの指定
	L_File.open("encounterdata\\" + to_string(encounter.id)+ ".csv");	// ファイルオープン

	if (L_File.fail()) {	// ファイル読み込み
	}
	else {
		// 1行目【基本情報】
		getline(L_File, L_Line);
		// 読み込んだ行をコピー
		std::istringstream L_stream(L_Line);

		//////////////////////////////////////
		//////////// 【基本情報】//////////////
		//////////////////////////////////////
		// 1【敵フィールドの横幅】取得
		getline(L_stream, L_token, ',');
		enemyFieldWidth = boost::lexical_cast<int>(L_token);
		// 2【敵フィールドの縦幅】取得
		getline(L_stream, L_token, ',');
		enemyFieldHeight = boost::lexical_cast<int>(L_token);
		// 3【1マスのサイズ】取得
		getline(L_stream, L_token, ',');
		cellSize = boost::lexical_cast<int>(L_token);

		// 敵フィールドの生成
		enemyField.resize(enemyFieldHeight);
		for (int i = 0; i != enemyFieldHeight; ++i) {
			enemyField[i].resize(enemyFieldWidth);
		}
		// 全マスnullptrで初期化
		for (int i = 0; i != enemyFieldHeight; ++i) {
			for (int j = 0; j != enemyFieldWidth; ++j) {
				enemyField[i][j] = nullptr;
			}
		}
		// 【スキル対象保存用】
		enemyField_Save.resize(enemyFieldHeight);
		for (int i = 0; i != enemyFieldHeight; ++i) {
			enemyField_Save[i].resize(enemyFieldWidth);
		}
		for (int i = 0; i != enemyFieldHeight; ++i) {
			for (int j = 0; j != enemyFieldWidth; ++j) {
				enemyField_Save[i][j] = false;
			}
		}

		// マスの左上の座標計算
		enemyFieldX = (WINDOW_WIDTH / 2) - (enemyField[0].size() * cellSize) / 2;
		enemyFieldY = (WINDOW_HEIGHT - 300) - (enemyField.size() * cellSize);

		//////////////////////////////////////
		//////////// 【敵データ】//////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// 一行ずつ読み込み
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);

			// 1【敵ID】取得
			getline(L_stream, L_token, ',');
			L_EnemyID = boost::lexical_cast<int>(L_token);
			// 2【X座標】取得
			getline(L_stream, L_token, ',');
			L_EnemyX = boost::lexical_cast<int>(L_token);
			// 3【Y座標】取得
			getline(L_stream, L_token, ',');
			L_EnemyY = boost::lexical_cast<int>(L_token);

			// 敵の作成
			enemy.push_back(new Enemy(L_EnemyX, L_EnemyY, L_EnemyID, turnNum));
		}
		// 敵データ読み込み && 当たり判定作成
		for (int i = 0, n = enemy.size(); i < n; ++i) {
			enemy[i]->LoadData(turnNum, enemy);
			enemy[i]->SetColision(enemyField);
		}
	}
	// ファイルクローズ
	L_File.close();
}

// debug
void Battle::TestIni() {
	/*// 敵フィールドマス（10×5）で初期化
	enemyField.resize(5);
	for (int i = 0, n = enemyField.size(); i < n; i++) {
		enemyField[i].resize(10);
	}
	// 1マスのサイズ
	cellSize = 128;
	// 全マスnullptrで初期化
	for (int i = 0, n = enemyField.size(); i < n; i++) {
		for (int j = 0, m = enemyField[i].size(); j < m; j++) {
			enemyField[i][j] = nullptr;
		}
	}

	レムカトナクラム
	エテルコクトゥス

	*/

	//enemy.push_back(new Enemy(0, 3, "てき１"));
	//enemy.push_back(new Enemy(4, 1, "てき２"));
	//enemy.push_back(new Enemy(7, 3, "てき３"));

	


	/*// 敵データ作成
	enemy.push_back(new Enemy("てき１"));
	enemy.push_back(new Enemy("てき２"));
	enemy.push_back(new Enemy("てき３"));
	// 敵1の当たり判定
	enemyField[3][1] = enemy[0];
	enemyField[3][2] = enemy[0];
	enemyField[4][1] = enemy[0];
	enemyField[4][2] = enemy[0];
	// 敵2の当たり判定
	enemyField[1][5] = enemy[1];
	enemyField[1][6] = enemy[1];
	// 敵3の当たり判定
	enemyField[2][7] = enemy[2];
	enemyField[2][8] = enemy[2];
	enemyField[2][9] = enemy[2];
	enemyField[3][7] = enemy[2];
	enemyField[3][8] = enemy[2];
	enemyField[3][9] = enemy[2];
	enemyField[4][7] = enemy[2];
	enemyField[4][8] = enemy[2];
	enemyField[4][9] = enemy[2];*/

	
}

Battle::~Battle() {
	// 【画像】
	DeleteGraph(gr_Back);			// 背景
	DeleteGraph(gr_SkillHit1);		// スキルの当たり判定
	DeleteGraph(gr_SkillHit2);		// スキルの当たり判定
	DeleteGraph(gr_SkillHit3);		// スキルの当たり判定
	DeleteGraph(gr_SkillHit4);		// スキルの当たり判定
	DeleteGraph(gr_target);			// 敵の当たり判定
	DeleteGraph(gr_NA);				// 
	DeleteGraph(gr_SkillSelect);	// スキル選択ウィンドウ
	PlayerChara::FinaBattle();	// プレイヤーのバトル用の画像解放
	Enemy::FinaBattle();	// 敵のバトル用の画像解放
	Character::CommonFinaBattle();	// キャラクタ戦闘共通終了処理
	DamageEffect::Fina();	// ダメージ数エフェクトの終了処理

	// 敵データdelete
	for (int i = 0, n = (int)enemy.size(); i < n; i++) {
		enemy[i]->Release();
		delete enemy[i];
		enemy[i] = nullptr;
	}

	// 敵の死亡者リストのリセット
	Enemy::deathList.clear();
	Enemy::deathList.shrink_to_fit();

	StopSoundMem(encounter.battleBGM);
	DeleteSoundMem(encounter.battleBGM);
}

void Battle::Process() {

	// 【エフェクト処理】
	// 画像を表示するエフェクト
	for (int i = 0, n = graphicEffect.size(); i != n; ++i) {
		if (graphicEffect[i]->Process()) {
			delete graphicEffect[i];
			graphicEffect.erase(graphicEffect.begin() + i);
			--i;
			--n;
		}
	}
	/*if (Input::Get(VK_Z) == 1) {
		for (int i = 0, n = graphicEffect.size(); i != n; ++i) {
			delete graphicEffect[i];
		}
		graphicEffect.clear();
		graphicEffect.shrink_to_fit();
	}*/
	//if (!graphicEffect.empty()) {
	//	return;
	//}
	// ダメージ量表示
	for (int i = 0, n = damageEffect.size(); i != n; ++i) {
		if (damageEffect[i].Process()) {
			damageEffect.erase(damageEffect.begin() + i);
			--i;
			--n;
		}
	}

	switch (step)
	{
	case eBattleStep::START:
		Process_START();
		break;
	case eBattleStep::COMMAND:
		Process_COMMAND();
		break;
	case eBattleStep::BATTLE:
		Process_BATTLE();
		break;
	case eBattleStep::END:
		Process_END();
		break;
	}	
}
// 戦闘開始画面
void Battle::Process_START() {
	
	if (TextBox::textBoxFlag) return;	// テキストを表示し終わるまで
	StepChange_COMMAND();	// コマンド画面へ移行
}
// 開始画面への移行処理
void Battle::StepChange_START() {
	step = eBattleStep::START;
	// エンカウント情報の表示
	vector<string>	message;
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		message.push_back(enemy[i]->GetName() + "が現れた");
	}
	TextBox::AddLine(message, 0, 10, 50);

	// 全キャラ戦闘開始処理
	for (int i = 0, n = player.size(); i < n; ++i) {
		player[i]->StateReset();
		player[i]->UniqueProcess();		// 固有処理（刻印とか）
	}
	for (int i = 0, n = enemy.size(); i < n; ++i) {
		enemy[i]->StateReset();
	}
}

// コマンド画面への移行処理
void Battle::StepChange_COMMAND() {
	// suzuki_debug
	for (int i = 0, n = usedItem.size(); i < n; ++i) {
		// アイテムの削除とからのポインタの代入
		if (usedItem[i].isUse) {
			// 実体がある場合
			if (Player::bag[usedItem[i].player][usedItem[i].item.Y]->GetIsCunsumption()) {
				// 実体の削除とバッグの整列
				delete Player::bag[usedItem[i].player][usedItem[i].item.Y];												// 実体の削除
				Player::bag[usedItem[i].player].erase(Player::bag[usedItem[i].player].begin() +  usedItem[i].item.Y);	// 配列の整列
			}
		}
		// 初期化
		usedItem[i].player = 0;
		usedItem[i].item.X = 0;
		usedItem[i].item.Y = 0;
		usedItem[i].isUse = false;
	}

	step = eBattleStep::COMMAND;

	++turnNum;	// ターン数＋１

	commandStep = eCommand::CharaSelect;	// キャラ選択から
	selectChara = -1;	// プレイヤーの要素番号

	// コマンドで選択したスキルの初期化
	commandOrder.clear();
	commandOrder.shrink_to_fit();
}

// コマンド画面
void Battle::Process_COMMAND() {

	// 【テキスト表示し終わるまで待機】
	if (TextBox::textBoxFlag) return;

	switch (commandStep){
	case eCommand::CharaSelect:// 【キャラ選択】
	{
		// 次のキャラに移行
		++selectChara;

		// アイテム選択のアイコンの初期化
		selectItem.pos.X	= 0;
		selectItem.pos.Y	= 0;
		selectItem.flag		= false;

		for (int n = player.size(); selectChara < n; ++selectChara) {
			// そのキャラが行動可能かを判定
			if (player[selectChara]->CheckAbleAct()) {
				break;
			}
			else {
				commandOrder.push_back({ player[selectChara], skill_None, 0, 0 });	//「なにもしない」スキルを追加
			}
		}

		// 全プレイヤーの行動が決定したか判定
		if (selectChara >= (int)player.size()) {
			// 敵の行動決定
			for (int i = 0, n = enemy.size(); i != n; ++i) {
				enemy[i]->DecisionSkill(playerHate, enemy);
			}
			// バトルシーンへ移行
			//step = eBattleStep::BATTLE;
			StepChange_BATTLE();
		}
		else {
			// 行動選択に移行
			commandStep = eCommand::ActionSelect;
			selectAction = commandPreSelect[selectChara].action;	// カーソルは前ターンに選択した位置にする
		}
		break;
	}
	case eCommand::ActionSelect:// 【行動選択】
	{
		// suzuki_debug
		usedItem[selectChara].isUse = false;

		// カーソル移動
		if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
			|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
			selectAction = (selectAction + (ActionNum - 1)) % this->ActionNum;
		}
		if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
			|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
			selectAction = (selectAction + 1) % ActionNum;
		}
		// 決定
		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			commandPreSelect[selectChara].action = selectAction;	// カーソル位置保存
			switch (selectAction) {
			case 0:// 【攻撃】
				// 通常攻撃
				selectSkillData = player[selectChara]->GetNomalAttack();

				selectSkillData->y = commandPreSelect[selectChara].targetY - (selectSkillData->height / 2);
				selectSkillData->x = commandPreSelect[selectChara].targetX - (selectSkillData->width / 2);

				// チャージ補正（範囲拡大）
				if (player[selectChara]->GetCharge(eCharge::Range) != 1) {
					// 範囲選択なし
					// 全体攻撃
					// そのままスキルオーダーにセット
					if (selectSkillData->totalUseEnergy == 0) {
						// 【MUE不要】
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
					}
					else if (commandPreSelect[selectChara].fullPowerFlag) {
						// 【全力】
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
					}
					else {
						// 【通常】
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
					}
					// 次のキャラに
					commandStep = eCommand::CharaSelect;
				}
				else {
					// 範囲選択画面へ
					commandStep = eCommand::TargetSelect;
				}
				break;
			case 1:// 【スキル】
				selectSkill = commandPreSelect[selectChara].skill;
				selectSkillCursor = 0;
				selectSkillArea = commandPreSelect[selectChara].skill;
				
				selectSkillList = player[selectChara]->GetSkillList_Battle();	// 選択キャラの戦闘スキルリスト取得
				skillNum = selectSkillList.size();

				// スキルが0なら移行しない
				if (!selectSkillList.empty()) {
					if (selectSkill >= skillNum) {
						selectSkill = skillNum - 1;
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4;
					}
					else if (selectSkillArea + 5 >= skillNum) {
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4 - (skillNum - 1 - selectSkill);
					}
					// スキル選択に移行
					commandStep = eCommand::SkillSelect;
				}
				break;
			case 2:// 【アイテム】
				// アイテム選択に移行
				if (Player::bag[selectChara].empty()) {
					// アイテムを所持していない場合
					TextBox::AddLine("アイテムを所持していません・・・");	// アイテムを所持していない旨を伝える
				}
				else {
					// アイテムを所持している場合
					commandStep = eCommand::ItemSelect;	// アイテム選択に移動
				}
				break;
			case 3:// 【防御】
				commandOrder.push_back({ player[selectChara], skill_Defence, 0, 0 });	// 防御スキルを追加
				// 次のキャラに
				commandStep = eCommand::CharaSelect;
				break;
			case 4:// 【逃げる】
				if (!encounter.escapeFlag) {
					// 逃走不可
					TextBox::AddLine("この戦闘からは逃げられない！");
					break;
				}
				commandOrder.push_back({ player[selectChara], skill_Escape, 0, 0 });	// 逃走スキルを追加

				// 次のキャラに
				commandStep = eCommand::CharaSelect;
				break;
			}
		}
		// 戻る
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {

			// 今のコマンドキャラが先頭でなければ
			if (selectChara > 0) {
				int nowSelectChara = selectChara;	// 今のコマンドキャラを保存しておく
				--selectChara;	// キャラを1戻す

				while (selectChara >= 0) {
					// そのキャラが行動可能かを判定
					if (player[selectChara]->CheckAbleAct()) {
						commandOrder.pop_back();	// スキル削除
						break;
					}
					--selectChara;	// キャラを1戻す
				}
				// もしキャラが-1なら（自分より前が全員行動不能）
				if (selectChara < 0) {
					selectChara = nowSelectChara;	// コマンドキャラを戻さない
				}
			}
		}
		break;

	}
	case eCommand::SkillSelect:// 【使用スキル選択】
	{
		// 【カーソル移動
		// 上
		if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
			|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
			// カーソル上移動
			selectSkill = (selectSkill + (skillNum - 1)) % this->skillNum;
			// スキルの数が６個以上かで処理を分ける
			if (skillNum > 5) {
				if (selectSkillCursor == 0) {
					if (selectSkill == skillNum - 1) {
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4;
					}
					else {
						--selectSkillArea;
					}
				}
				else {
					--selectSkillCursor;
				}
			}
		}
		// 下
		if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
			|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
			// カーソル下移動
			selectSkill = (selectSkill + 1) % skillNum;
			// スキルの数が６個以上かで処理を分ける
			if (skillNum > 5) {
				if (selectSkillCursor == 4) {
					if (selectSkill == 0) {
						selectSkillArea = 0;
						selectSkillCursor = 0;
					}
					else {
						++selectSkillArea;
					}
				}
				else {
					++selectSkillCursor;
				}
			}
		}
		// 左
		if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
			|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
			selectSkill -= 5;
			selectSkillArea -= 5;
			if (selectSkill < 0) {
				selectSkill = 0;
				selectSkillArea = 0;
				selectSkillCursor = 0;
			}
			if (selectSkillArea < 0) {
				selectSkillArea = 0;
				selectSkillCursor = selectSkill;
			}
		}
		// 右
		if (Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
			|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
			selectSkill += 5;
			selectSkillArea += 5;
			if (selectSkill >= skillNum) {
				selectSkill = skillNum-1;
				selectSkillArea = skillNum - 5;
				selectSkillCursor = 4;
			}
			else if (selectSkillArea + 5 >= skillNum) {
				selectSkillArea = skillNum - 5;
				selectSkillCursor = 4 - (skillNum-1 - selectSkill);
			}
		}
		// 決定
		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			commandPreSelect[selectChara].skill = selectSkill;	// カーソル位置保存
			selectSkillData = selectSkillList[selectSkill];

			// 範囲選択フラグで判定 && チャージ補正（範囲拡大）
			if (selectSkillData->selectFlag && player[selectChara]->GetCharge(eCharge::Range) == 1) {
				if (selectSkillData->selectTarget) {
				// 敵選択へ移行
				selectSkillData->y = commandPreSelect[selectChara].targetY - (selectSkillData->height / 2);
				selectSkillData->x = commandPreSelect[selectChara].targetX - (selectSkillData->width / 2);
				}
				else {
					// 味方選択に移行
					selectTargetAlly = commandPreSelect[selectChara].selectTargetAlly;
				}
				commandStep = eCommand::TargetSelect;
			}
			else {
				// そのままスキルオーダーにセット
				if (selectSkillData->totalUseEnergy == 0) {
					// 【MUE不要】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// 【全力】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE)});
				}
				else {
					// 【通常】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				
				// 次のキャラに
				commandStep = eCommand::CharaSelect;
			}
		}
		// 戻る
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			commandStep = eCommand::ActionSelect;	// 行動選択に戻る
		}
		// 通常 - 全力 切替
		if (Input::Get(VK_Q) == 1 || Input::Get(0, XINPUT_GAMEPAD_START) == 1) {
			commandPreSelect[selectChara].fullPowerFlag = !commandPreSelect[selectChara].fullPowerFlag;
		}
		break;
	}
	case eCommand::ItemSelect:// 【使用アイテム選択】
	{
		if ((Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1)
			&& selectItem.pos.Y != 0) {
			// 上に移動
			--selectItem.pos.Y;
		}
		else if ((Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1)
			&& selectItem.pos.Y != Player::bag[selectChara].size() - 1) {
			// 下に移動
			++selectItem.pos.Y;
		}

		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			// 決定
			if (!Player::bag[selectChara][selectItem.pos.Y]->GetSkill()) {
				TextBox::AddLine("使えません");
				// アイテム選択のアイコンの初期化
				selectItem.pos.X = 0;
				selectItem.pos.Y = 0;
				selectItem.flag = false;

				commandStep = eCommand::ActionSelect;	// 行動選択に戻る
				return;
			}
			// debug　現状味方にのみ
			usedItem[selectChara].isUse		= true;														// 使用予定のアイテムとして保存する
			usedItem[selectChara].player	= selectChara;												// 使用予定のアイテムのバッグを保存する
			usedItem[selectChara].item.Y	= selectItem.pos.Y;											// 使用予定のアイテムのバッグ内の座標を保存する
			selectItem.flag					= true;														// アイテムを選択したことを保存する
			selectTargetAlly				= commandPreSelect[selectChara].selectTargetAlly;			// 味方選択に移行
			selectSkillData					= Player::bag[selectChara][selectItem.pos.Y]->GetSkill();	// スキルをセット
			commandStep						= eCommand::TargetSelect;									// 対象選択へ
		}

		// 戻る
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// アイテム選択のアイコンの初期化
			selectItem.pos.X = 0;
			selectItem.pos.Y = 0;
			selectItem.flag = false;

			commandStep = eCommand::ActionSelect;	// 行動選択に戻る
		}
		break;
	}
	case eCommand::TargetSelect:// 【スキル等の対象範囲選択】
	{
		// 【敵から選択】
		if (selectSkillData->selectTarget) {
			// ターゲット移動
			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
				|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
				if (selectSkillData->y + selectSkillData->collision.size() > 1) {
					--selectSkillData->y;	// 上
				}
			}
			if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
				|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
				if (selectSkillData->y + 1 < enemyFieldHeight) {
					++selectSkillData->y;	// 下
				}
			}
			if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
				|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
				if (selectSkillData->x + selectSkillData->collision[0].size() > 1) {
					--selectSkillData->x;	// 左
				}
			}
			if (Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
				|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
				if (selectSkillData->x + 1 < enemyFieldWidth) {
					++selectSkillData->x;	// 右
				}
			}
			// 決定
			if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				commandPreSelect[selectChara].targetY = selectSkillData->y + (selectSkillData->height / 2);
				commandPreSelect[selectChara].targetX = selectSkillData->x + (selectSkillData->width / 2);

				// スキルオーダーにセット
				if (selectSkillData->totalUseEnergy == 0) {
					// 【MUE不要】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// 【全力】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
				}
				else {
					// 【通常】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				// 次のキャラに
				commandStep = eCommand::CharaSelect;
			}
			// 戻る
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				commandStep = eCommand::ActionSelect;	// 行動選択に戻る
			}
		}
		// 【味方から選択】
		else {
			// 味方はマス選択ではなくキャラ選択
			// ターゲット移動
			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
				|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
				selectTargetAlly = (selectTargetAlly + (player.size() - 1)) % player.size();
			}
			if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
				|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
				selectTargetAlly = (selectTargetAlly + 1) % player.size();
			}
			if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
				|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
				selectTargetAlly = 0;
			}
			if (	Input::Get(VK_RIGHT)						== 1	|| Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT)		== 1
				|| (Input::Get(VK_RIGHT)						> 10	&& Input::Get(VK_RIGHT) % 5						== 0)
				|| (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT)	> 10	&& Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
				selectTargetAlly = player.size() - 1;
			}
			// 決定
			if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				// 座標確定
				selectSkillData->y = 1;
				selectSkillData->x = 1 + selectTargetAlly * 3;

				commandPreSelect[selectChara].selectTargetAlly = selectTargetAlly;	// コマンドのセーブ

				// スキルオーダーにセット
				if (selectSkillData->totalUseEnergy == 0) {
					// 【MUE不要】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// 【全力】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
				}
				else {
					// 【通常】
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				// 次のキャラに
				commandStep = eCommand::CharaSelect;
			}
			// 戻る
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				commandStep = eCommand::ActionSelect;	// 行動選択に戻る
			}
		}

		break;
	}
	}
}

// バトルシーンへの移行処理
void Battle::StepChange_BATTLE() {
	step = eBattleStep::BATTLE;

	// スキルオーダーに追加
	for (int i = 0, n = commandOrder.size(); i < n; ++i) {
		skillOrder->Add(commandOrder[i].attacker, commandOrder[i].skill, commandOrder[i].totalUseEnergy);
	}

	for (int i = 0, n = player.size(); i < n; ++i) {
		player[i]->TurnStartProcess();
	}
	for (int i = 0, n = enemy.size(); i < n; ++i) {
		enemy[i]->TurnStartProcess();
	}

	// コマンドで選択したスキルの初期化
	commandOrder.clear();
	commandOrder.shrink_to_fit();
}

// メイン戦闘画面
void Battle::Process_BATTLE() {

	// 【テキスト表示し終わるまで待機】
	if (TextBox::textBoxFlag) return;


	//////////////////////////////////////
	//////////// 【スキル発動】////////////
	//////////////////////////////////////
	if (skillOrder->order.empty()) {
		// 【スキルオーダーが空なら】

		// ターン終了処理
		for (int i = 0, n = enemy.size(); i < n; i++) {
			enemy[i]->TurnEndProcess();
			// 死んだら当たり判定削除
			if (enemy[i]->GetSkillDeath()) {
				enemy[i]->StateReset();	// 状態の初期化
			}
		}
		for (int i = 0, n = player.size(); i < n; i++) {
			player[i]->TurnEndProcess();
			player[i]->UniqueProcess();		// 固有処理（刻印とか）
			// 死んだら当たり判定削除
			if (player[i]->GetSkillDeath()) {
				player[i]->StateReset();	// 状態の初期化
			}
		}
		
		// フィールドMPの自動回復
		if (fieldMP < fieldMaxMP) {
			IncreaseFieldMP((int)(fieldMaxMP * 0.1));	// フィールド最大MPの1/10回復
			// 最大MPを超えたら戻す
			if (fieldMP > fieldMaxMP) {
				fieldMP = fieldMaxMP;
			}
		}
		// フィールドMPの自動減少
		else if(fieldMP > fieldMaxMP){
			DecreaseFieldMP((int)(fieldMaxMP * 0.1));	// フィールド最大MPの1/10回復
			// 最大MPを超えたら戻す
			if (fieldMP < fieldMaxMP) {
				fieldMP = fieldMaxMP;
			}
		}

		// 死亡リストの敵が死亡状態でなくなったらリストから外す
		for (int i = 0, n = Enemy::deathList.size(); i < n; ++i) {
			if (enemy[Enemy::deathList[i]]->GetAlive() != eAlive::dead) {
				Enemy::deathList.erase(Enemy::deathList.begin() + i);
				--i;
				--n;
			}
		}

		DeleteDeadEnemy();	// 死んだ敵のデータを削除

		StepChange_COMMAND();	// コマンド画面へ移行
	}
	else{
		// 敵味方初期化
		for (int i = 0, n = enemy.size(); i != n; ++i) {
			enemy[i]->ReceiveSkillIni();
		}
		for (int i = 0, n = player.size(); i != n; ++i) {
			player[i]->ReceiveSkillIni();
		}

		vector<string>	message;
		// 【死んでたら行動させない】
		if (skillOrder->GetAttacker()->GetAlive() != eAlive::alive) {
			skillOrder->Pop_Back();	// スキルオーダーから消す
			return;	//  ここで処理終了
		}
		// 【スキルの効果をテキストボックスに表示】
		// 【状態異常判定】
		// 【麻痺】
		if (skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Paralysis).get) {
			// 1/2で行動不可
			if (GetRand(1) == 0) {
				message.push_back(skillOrder->GetAttacker()->GetName() + "は身体が麻痺して動けない");
				skillOrder->Pop_Back();	// スキルオーダーから消す
				return;	//  ここで処理終了
			}
		}
		// 【スタン】
		if (skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Stun).get) {
			message.push_back(skillOrder->GetAttacker()->GetName() + "は意識を失っている");
			skillOrder->Pop_Back();	// スキルオーダーから消す
		}
		// 【睡眠】
		else if(skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Sleep).get){
			message.push_back(skillOrder->GetAttacker()->GetName() + "は眠っている");
			skillOrder->Pop_Back();	// スキルオーダーから消す
		}
		// 【混乱】
		else if(skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Confusion).get) {
			message.push_back(skillOrder->GetAttacker()->GetName() + "は正気を失っている");
			// ランダム行動
			switch (GetRand(2)){
			case 0:// 【誰かに攻撃】
			{
				int targetNum = 0;	// 攻撃対象のインデックス
				Skill* ConfusionSkillData = skillOrder->GetAttacker()->GetConfusionSkill();	// 混乱時の攻撃用スキルを取得
				message.push_back(skillOrder->GetAttacker()->GetName() + "は突然暴れだした");

				if (GetRand(1) == 0) {
					// 【プレイヤー】
					// 攻撃対象をプレイヤーに（攻撃者がプレイヤーならfalse,敵ならtrue）
					ConfusionSkillData->selectTarget = !skillOrder->GetAttacker()->GetCharType();
					
					do {
						targetNum = GetRand(player.size() - 1);	// プレイヤーからランダムに抽出
					} while (player[targetNum]->GetAlive() != eAlive::alive);	// 対象が生きていなければ再抽選

					// x2y3の中のどれかのマスを選択
					ConfusionSkillData->x = (player[targetNum]->GetX() - (int)(ConfusionSkillData->width * 0.5) + GetRand(1));
					ConfusionSkillData->y = (player[targetNum]->GetY() - (int)(ConfusionSkillData->height * 0.5) + GetRand(2));

				}
				else {
					// 【敵】
					// 攻撃対象を敵に（攻撃者がプレイヤーならtrue,敵ならfalse）
					ConfusionSkillData->selectTarget = skillOrder->GetAttacker()->GetCharType();
					do {
						targetNum = GetRand(enemy.size() - 1);	// 敵からランダムに抽出
					} while (enemy[targetNum]->GetAlive() != eAlive::alive);	// 対象が生きていなければ再抽選

					// 敵の縦横幅からランダムにマスを選択
					ConfusionSkillData->x = (enemy[targetNum]->GetX() - (int)(ConfusionSkillData->width * 0.5) + GetRand(enemy[targetNum]->GetWidth() - 1));
					ConfusionSkillData->y = (enemy[targetNum]->GetY() - (int)(ConfusionSkillData->height * 0.5) + GetRand(enemy[targetNum]->GetHeight() - 1));
					
				}
				// スキル発動
				SkillExec(skillOrder->GetAttacker(), ConfusionSkillData);
				break;
			}
			case 1:// 【行動不可】
				message.push_back(skillOrder->GetAttacker()->GetName() + "は震えて動けない");

				break;
			case 2:// 【気絶】
				//message.push_back(skillOrder->GetAttacker()->GetName() + "は気絶した");
				// 状態異常【スタン】にする
				skillOrder->GetAttacker()->GiveBadStatus(eBadStatus::Stun, 0);
				break;
			}
			skillOrder->Pop_Back();	// スキルオーダーから消す
		}
		// 【行動可能】
		else {
			// スキルの発動
			switch (skillOrder->GetSkill()->type){
			case eSkillType::Nomal:// 【通常スキル】

				if (skillStep) {
					// スキル使用時テキスト
					if (!skillOrder->GetSkill()->utilizationText.empty()) {
						// 1行目
						message.push_back(skillOrder->GetAttacker()->GetName() + skillOrder->GetSkill()->utilizationText[0]);
						// 2行目以降
						for (int i = 1, n = skillOrder->GetSkill()->utilizationText.size(); i < n; ++i) {
							message.push_back(skillOrder->GetSkill()->utilizationText[i]);
						}
					}
					// スキルが発動可能か調べる
					if (CheckAbleSkill(skillOrder->GetOrder())) {
						// 発動成功
						if (skillOrder->GetSkill()->selectFlag && 
							skillOrder->GetSkill()->selectTarget == skillOrder->GetAttacker()->GetCharType() &&
							skillOrder->GetSkill()->grEffect.flag) {
							graphicEffect.push_back(new DivGraphicEffect(enemyFieldX + skillOrder->GetSkill()->x * cellSize, enemyFieldY + skillOrder->GetSkill()->y * cellSize, skillOrder->GetSkill()->grEffect.gr, skillOrder->GetSkill()->grEffect.divX, skillOrder->GetSkill()->grEffect.divY, skillOrder->GetSkill()->grEffect.divNum, skillOrder->GetSkill()->width, skillOrder->GetSkill()->height));
						}
						skillOrder->GetSkill()->PlayerSE();	// SEを鳴らす
					}
					else {
						// 発動失敗
						message.push_back("MPが足りない");
					}
					if (!message.empty()) {
						TextBox::AddLine(message, 0, 10, 3*5+30);
					}
					skillStep = false;
					return;
				}
				else {
					// スキル発動
					SkillExec(skillOrder->GetOrder());
					skillStep = true;
				}
				break;
			case eSkillType::Confusion:// 【混乱時のスキル】
				// バグでなければここにくることはない
				// とりあえず「なにもしない」と同じ処理を書いとく
				skillOrder->Pop_Back();	// スキルオーダーから消す
				break;
			case eSkillType::None:// 【なにもしない】
				skillOrder->Pop_Back();	// スキルオーダーから消す
				break;
			case eSkillType::Defence:// 【防御】
				skillOrder->GetAttacker()->Defence();		// 対象を防御状態に
				message.push_back(skillOrder->GetAttacker()->GetName() + "は身を守っている");
				// ヘイト値計算
				if (skillOrder->GetAttacker()->GetCharType()) {
					// ヘイト値を0.9倍に
					playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] = (int)(playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] * 0.9);
					// ヘイト値下限	1未満になったら1に
					if (playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] < 1) {
						playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] = 1;
					}
				}

				skillOrder->Pop_Back();	// スキルオーダーから消す
				break;
			case eSkillType::Escape:// 【逃走】
				message.push_back(skillOrder->GetAttacker()->GetName() + "は逃げ出した");
				if (skillOrder->GetAttacker()->GetCharType()) {
					// 【プレイヤー】
					if (!encounter.escapeFlag) {
						// 逃走不可戦闘
						message.push_back("しかし、この戦闘からは逃げられない！");
						break;
					}
					// 成功率25%　成功したら戦闘終了
					if (GetRand(100) < 25) {
						TextBox::AddLine(message, 0, 10, 50);
						battleEndType = eEndStatus::Escape;	// 逃走成功
						step = eBattleStep::END;	// 終了画面に移行

						DeleteDeadEnemy();	// 死んだ敵のデータを削除

						EndText();	// 戦闘終了テキストを標示
						return;		//  ここで処理終了
					}
					else {
						message.push_back("しかし逃げきれなかった");
					}
				}
				else {
					// 【敵】
					// いまのところ敵は確定逃げ
					skillOrder->GetAttacker()->GiveBadStatus(eBadStatus::Extinction, 0);	// 敵を消滅させる
				}

				skillOrder->Pop_Back();	// スキルオーダーから消す
				break;
			}
		}

		// スキル効果メッセージ標示
		// 敵
		for (int i = 0, n = enemy.size(); i < n; i++) {
			enemy[i]->GetEffectText(message);
		}
		// プレイヤー
		for (int i = 0, n = player.size(); i < n; i++) {
			player[i]->GetEffectText(message);
		}
		// 表示するものが無ければ
		//if (message.size() == 1) {
		//	message.push_back("効果がなかった");
		//}
		if (!message.empty()) {
			TextBox::AddLine(message, 0, 10, -1);
		}


		// 死んだら当たり判定削除
		for (int i = 0, n = enemy.size(); i < n; i++) {
			if (enemy[i]->GetSkillDeath()) {
				//enemy[i]->DeleteColision(enemyField);
				enemy[i]->StateReset();	// 状態の初期化
				// 戦利品取得
				exp += enemy[i]->GetEXP();		// 経験値
				money += enemy[i]->GetMoney();	// 金

				enemy[i]->StateReset();	// 状態の初期化

				// 死亡した敵の情報を死亡者リストに登録
				Enemy::deathList.push_back(i);
			}
		}
		for (int i = 0, n = player.size(); i < n; i++) {
			if (player[i]->GetSkillDeath()) {
				//player[i]->DeleteColision(playerField);
				playerHate[i] = 0;	// ヘイトを0にする
				player[i]->StateReset();	// 状態の初期化
			}
		}
	}
	// 【プレイヤー全滅判定】
	if (CheckPlayersDead()) {
		battleEndType = eEndStatus::Lose;	// 全滅
		step = eBattleStep::END;	// 終了画面に移行

		EndText();	// 戦闘終了テキストを標示
		return;
	}
	// 【敵全滅判定】
	if (CheckEnemysDead()) {
		battleEndType = eEndStatus::Win;	// 戦闘勝利
		step = eBattleStep::END;	// 終了画面に移行

		DeleteDeadEnemy();	// 死んだ敵のデータを削除

		EndText();	// 戦闘終了テキストを標示
		return;
	}
	
}
// 戦闘終了画面
void Battle::Process_END() {
	// 【テキスト表示し終わるまで待機】
	if (TextBox::textBoxFlag) return;
	
	// 戦闘終了の状態で分岐
	switch (battleEndType)
	{
	case eEndStatus::Win:// 【勝利】
		mode = StageMode::FIELD;	// フィールドに移行
		break;
	case eEndStatus::Lose:// 【全滅】
		if (encounter.loseEventFlag) {
			// 負けイベならフィールドに移行
			mode = StageMode::FIELD;
		}
		else {
			// ゲームオーバー画面へ
			mode =  StageMode::GAMEOVER;
		}
		break;
	case eEndStatus::Escape:// 【逃走】
		mode = StageMode::FIELD;	// フィールドに移行
		break;
	default:// エラー
		// ゲームオーバー画面へ
		mode = StageMode::GAMEOVER;
		break;
	}
	
	// イベント戦闘状態の受け渡し処理
	if (encounter.battleFlag != NULL) {
		(*encounter.battleFlag) = false;
	}
	if (encounter.battleEndType != NULL) {
		(*encounter.battleEndType) = battleEndType;
	}
	EncounterReset();	// エンカウンターのリセット

}

// 終了画面の表示
void Battle::EndText() {
	vector<string>	message;
	// 戦闘終了の状態で分岐
	switch (battleEndType)
	{
	case eEndStatus::Win:// 【勝利】
		// 【戦闘勝利】
		for (int i = 0, n = (int)player.size(); i < n; i++) {
			player[i]->StateReset();		// 状態の初期化
			player[i]->TurnEndProcess();	// ターン終了処理
		}
		message.push_back("敵を倒した");
		message.push_back(to_string(exp) + "の経験値を獲得");
		message.push_back(to_string(money) + "Ｇを手に入れた");
		// アイテムドロップ
		for (int i = 0, n = (int)enemy.size(); i < n; ++i) {
			// ドロップ判定
			if (GetRand(100) <= enemy[i]->GetDropRate()) {
				// アイテムを持てるか判定
				if (Player::InsertItem(enemy[i]->GetDropItem())) {
					message.push_back(enemy[i]->GetName() + " から " + Item::GetName(enemy[i]->GetDropItem()) + " を手に入れた");
				}
			}
		}

		TextBox::AddLine(message, 0, 20, 50);
		
		for (int i = 0, n = (int)player.size(); i != n; ++i) {
			// 生きていれば経験値を取得
			if (player[i]->GetAlive() == eAlive::alive) {
				player[i]->AddEXP(exp);
			}
		}
		// 金を手に入れる
		Player::AddMoney(money);
		break;
	case eEndStatus::Lose:// 【全滅】
		message.push_back("全滅した…");
		break;
	case eEndStatus::Escape:// 【逃走】
		message.push_back(to_string(exp) + "の経験値を獲得");
		message.push_back(to_string(money) + "Ｇを手に入れた");
		TextBox::AddLine(message, 0, 20, 50);

		for (int i = 0, n = (int)player.size(); i != n; ++i) {
			// 生きていれば経験値を取得
			if (player[i]->GetAlive() == eAlive::alive) {
				player[i]->AddEXP(exp);
			}
		}
		break;
	default:
		break;
	}
	
}
// スキルが発動可能か調べる
bool Battle::CheckAbleSkill(tSkillOrder order) {
	// 【消費MP判定】
	if (order.skill->useMP[0] > (unsigned)order.attacker->GetStatus(eStatus::MP) || order.skill->useMP[1] > (unsigned)fieldMP) {
		return false;		// 発動失敗
	}
	return true;
}
// 通常スキル発動
bool Battle::SkillExec(tSkillOrder order) {
	skillOrder->Pop_Back();	// スキルオーダーから消す
	
	// 【スキル発動】
	for (int r = 0, l = order.skill->effect.size(); r < l; ++r) {
		switch (order.skill->effect[r]->type){
		//////////////////////////////////////
		////// 【範囲選択があるスキル】////////
		//////////////////////////////////////
		default:
		// 【攻撃】
		// 【回復】
		// 【状態異常】
		// 【バフ・デバフ】
		// 【蘇生】
		{
			// 対象範囲選択
			switch (order.skill->effect[r]->targetType) {
			case eSkillTarget::Self:// 【自分】
			{
				// スキル対象保存初期化
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				// 自分にスキルを発動
				SkillEffectExec(order.attacker->GetX(), order.attacker->GetY(), order.attacker, order.attacker, order.skill->effect[r], order.totalUseEnergy, r);

				break;
			}
			case eSkillTarget::Enemy:// 【敵全体】
			{
				// スキル対象保存初期化
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				if (order.attacker->GetCharType()) {
					// プレイヤーから見た敵＝敵
					// 敵全体にスキルを使う
					for (int i = 0; i != enemyFieldHeight; ++i) {
						for (int j = 0; j != enemyFieldWidth; ++j) {
							graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (enemyField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				else {
					// 敵から見た敵＝プレイヤー
					// 味方全体にスキルを使う
					for (int i = 0; i != playerFieldHeight; ++i) {
						for (int j = 0; j != playerFieldWidth; ++j) {
							//graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (playerField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Ally:// 【味方全体】
			{
				// スキル対象保存初期化
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				if (order.attacker->GetCharType()) {
					// プレイヤーから見た味方＝プレイヤー
					// 味方全体にスキルを使う
					for (int i = 0; i != playerFieldHeight; ++i) {
						for (int j = 0; j != playerFieldWidth; ++j) {
							//graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit, 10));
							if (playerField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				else {
					// 敵から見た味方＝敵
					// 敵全体にスキルを使う
					for (int i = 0; i != enemyFieldHeight; ++i) {
						for (int j = 0; j != enemyFieldWidth; ++j) {
							graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (enemyField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Overall:// 【敵味方全体】
			{
				// スキル対象保存初期化
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();


				// 敵全体にスキルを使う
				for (int i = 0; i != enemyFieldHeight; ++i) {
					for (int j = 0; j != enemyFieldWidth; ++j) {
						graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
						if (enemyField[i][j] != nullptr) {
							SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				// 味方全体にスキルを使う
				for (int i = 0; i != playerFieldHeight; ++i) {
					for (int j = 0; j != playerFieldWidth; ++j) {
						if (playerField[i][j] != nullptr) {
							SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				break;
			}
			case eSkillTarget::Select:// 【選択範囲】
			{
				// スキル対象保存初期化
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				// 選択範囲にスキルを使う
				if (order.skill->selectTarget != order.attacker->GetCharType()) {
					// プレイヤーが味方だと言えばプレイヤー　敵が敵だと言えばプレイヤー
					// プレイヤー

					// 【チャージ補正（範囲拡大）】
					if (order.attacker->GetCharge(eCharge::Range) != 1) {
						// プレイヤー全体
						for (int i = 0; i != playerFieldHeight; ++i) {
							for (int j = 0; j != playerFieldWidth; ++j) {
								if (playerField[i][j] != nullptr) {
									SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
								}
							}
						}
					}
					else {
						// 選択範囲
						for (int i = 0, n = order.skill->effect[r]->collision.size(); i != n; ++i) {
							for (int j = 0, m = order.skill->effect[r]->collision[i].size(); j != m; ++j) {
								if (order.skill->effect[r]->collision[i][j]) {
									// アウトオブレンジ対策
									if (i + order.skill->y >= 0 && i + order.skill->y < playerFieldHeight &&
										j + order.skill->x >= 0 && j + order.skill->x < playerFieldWidth) {

										//graphicEffect.push_back(new GraphicEffect(enemyFieldX + (j + order.skill->x) * cellSize, enemyFieldY + (i + order.skill->y) * cellSize, gr_SkillHit2, 10));
										if (playerField[i + order.skill->y][j + order.skill->x] != nullptr) {
											SkillEffectExec(j + order.skill->x, i + order.skill->y, order.attacker, playerField[i + order.skill->y][j + order.skill->x], order.skill->effect[r], order.totalUseEnergy, r);
										}
									}
								}
							}
						}
					}

				}
				else {
					// プレイヤーが敵だと言えば敵　敵が味方だと言えば敵
					// 敵

					// 【チャージ補正（範囲拡大）】
					if (order.attacker->GetCharge(eCharge::Range) != 1) {
						// 敵全体
						for (int i = 0; i != enemyFieldHeight; ++i) {
							for (int j = 0; j != enemyFieldWidth; ++j) {
								graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
								if (enemyField[i][j] != nullptr) {
									SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
								}
							}
						}
					}
					else {
						// 選択範囲
						// エフェクト
						//graphicEffect.push_back(new DivGraphicEffect(enemyFieldX + order.skill->x * cellSize, enemyFieldY + order.skill->y * cellSize, gr_NA, 3));
						for (int i = 0, n = order.skill->effect[r]->collision.size(); i != n; ++i) {
							for (int j = 0, m = order.skill->effect[r]->collision[i].size(); j != m; ++j) {
								if (order.skill->effect[r]->collision[i][j]) {
									// アウトオブレンジ対策
									if (i + order.skill->y >= 0 && i + order.skill->y < enemyFieldHeight &&
										j + order.skill->x >= 0 && j + order.skill->x < enemyFieldWidth) {
										if (enemyField[i + order.skill->y][j + order.skill->x] != nullptr) {
											SkillEffectExec(j + order.skill->x, i + order.skill->y, order.attacker, enemyField[i + order.skill->y][j + order.skill->x], order.skill->effect[r], order.totalUseEnergy, r);
										}
									}
								}
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Previous:// 【引き継ぎ】
			{
				// スキル対象保存初期化
				target_Save.clear();
				target_Save.shrink_to_fit();

				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						if (playerField_Save[i][j]) {
							SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						if (enemyField_Save[i][j]) {
							SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				break;
			}
			}
			// ************************************** //
			// **【超過ダメージによる状態異常付与】** //
			// ************************************** //
			if (order.skill->effect[r]->type == eSkillEffect::Attack) {
				switch (order.skill->effect[r]->energyConv.type) {
				case eEnergy::Heat:// 【熱】
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-
						// 変換量（効率込み）が100以上で即死付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Heat, false)) - 100;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Death, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					break;
				case eEnergy::Elec:// 【電気】
					if (order.skill->effect[r]->energyConv.direction) { //+
						// 変換量（効率込み）が10以上で麻痺付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Elec, true)) - 10;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Paralysis, value * 10, (unsigned int)(value * 0.1)});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Shine:// 【光】
					if (order.skill->effect[r]->energyConv.direction) { //+
						// 変換量（効率込み）が50以上で暗闇付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Shine, true)) - 50;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Blind, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-
						// 変換量（効率込み）が50以上で暗闇付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Shine, false)) - 50;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Blind, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					break;
				case eEnergy::Sound:// 【音】
					if (order.skill->effect[r]->energyConv.direction) { //+
						// 変換量（効率込み）が100以上で混乱付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Sound, true)) - 100;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Confusion, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Motion:// 【運動】
					if (order.skill->effect[r]->energyConv.direction) { //+
					}
					else { //-

					}
					break;
				case eEnergy::Gravity:// 【重力】
					if (order.skill->effect[r]->energyConv.direction) { //+
						// 変換量（効率込み）が0以上で混乱付与
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Gravity, true));
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Stun, value * 10, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Life:// 【生命】
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				case eEnergy::Static:// 【静止】
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				case eEnergy::Nucleus:// 【核】
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				}

			}
			
			// ************************************** //
			// *【エネルギー変換による魔素量の増減】* //
			// ************************************** //
			// 変換なしでなければ
			if (order.skill->effect[r]->energyConv.type != eEnergy::None) {
				// 変換方向
				if (order.skill->effect[r]->energyConv.direction) { // 【+変換】
					switch (order.skill->effect[r]->energyConv.convType) { // 使用MP
					case eConvType::Self:// 【スキル使用者のMP】
						// スキル使用者のMPを変換量分減少(変換量そのまま)
						order.attacker->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));

						break;
					case eConvType::Field:// 【フィールドのMP】
						// フィールドのMPを変換量分減少(変換量そのまま)
						DecreaseFieldMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
						break;
					case eConvType::Target:// 【スキル対象のMP】
						for (int i = 0, n = target_Save.size(); i < n; ++i) {
							// 対象が自分なら半減しない
							if (target_Save[i] == order.attacker) {
								// スキル対象のMPを変換量分減少(変換量)
								target_Save[i]->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
							}
							else {
								// スキル対象のMPを変換量分減少(変換量)

								// MUE対抗
								// 倍率が１以上なら減衰なし
								if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) >= 1) {
									target_Save[i]->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
								}
								// 倍率が０以上ならその分減衰
								else if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) > 0) {
									target_Save[i]->DecreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * (100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100))));
								}
							}
						}
						break;
					case eConvType::None:// 【// エネルギー変換以外の現象】
						break;
					default:
						break;
					}
				}
				else { // 【-変換】
					switch (order.skill->effect[r]->energyConv.convType) {
					case eConvType::Self:// 【スキル使用者のMP】
						// 自分の保有するエネルギーを-変換

						// スキル対象で派生
						if (order.skill->effect[r]->targetType == eSkillTarget::Self) {
							// 自分が対象
							// 変換した魔素は自分が取り込む
							// 自分のMPを変換量分増加(変換量×変換効率)
							order.attacker->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						}
						else {
							// 敵 or フィールド
							// 変換した魔素は場に漂う
							// フィールドのMPを変換量分増加(変換量×変換効率)
							IncreaseFieldMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						}
						break;
					case eConvType::Field:// 【フィールドのMP】
						// 場にあるエネルギーを-変換
						// 変換した魔素は場に漂う

						// フィールドのMPを変換量分増加(変換量×変換効率)
						IncreaseFieldMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						break;
					case eConvType::Target:// 【スキル対象のMP】
						// 相手がもつエネルギーに干渉して直接-変換
						// 干渉にはコストが必要なため、変換量が半減する
						// 変換した魔素は相手に蓄積する（MPが増える）

						// スキル対象となった相手全員回す（死亡者も含める）
						for (int i = 0, n = target_Save.size(); i < n; ++i) {
							// 対象が自分なら半減しない
							if (target_Save[i] == order.attacker) {
								// 自分のMPを変換量分増加(変換量×変換効率)
								order.attacker->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
							}
							else {
								// スキル対象のMPを変換量分増加(変換量×変換効率)
								//target_Save[i]->IncreaseMP((int)(GetConvValue(skill->effect[r]->energyConv, order.totalUseEnergy) * attacker->GetEfficiency(skill->effect[r]->energyConv.type, skill->effect[r]->energyConv.direction)));

								// MUE対抗
								// 倍率が１以上なら減衰なし
								if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) >= 1) {
									target_Save[i]->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
								}
								// 倍率が０以上ならその分減衰
								else if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) > 0) {
									target_Save[i]->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * (100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
								}
							}
						}
						break;
					case eConvType::None:// 【// エネルギー変換以外の現象】
						break;
					default:
						break;
					}
				}
			}
			break;
		}

		//////////////////////////////////////
		////// 【範囲選択がないスキル】////////
		//////////////////////////////////////
		case eSkillEffect::AnotherSkill:// 【他のスキル発動】
		{
			// 発動判定
			if (GetRand(99) < order.skill->effect[r]->GetAnotherSkill()->invocation) {
				// スキル発動成功
				// スキルオーダーに追加
				skillOrder->Add_Velocity(order.attacker, order.skill->effect[r]->GetAnotherSkill()->skill, order.skill->effect[r]->GetAnotherSkill()->skill->totalUseEnergy, order.skill->effect[r]->GetAnotherSkill()->fastFlag);

				// 発動するスキルにフィールド選択が必要なら、現左選択している座標にする
				if (order.skill->effect[r]->GetAnotherSkill()->skill->selectFlag) {
					order.skill->effect[r]->GetAnotherSkill()->skill->x = (order.skill->x + (int)(order.skill->width * 0.5)) - (int)(order.skill->effect[r]->GetAnotherSkill()->skill->width * 0.5);
					order.skill->effect[r]->GetAnotherSkill()->skill->y = (order.skill->y + (int)(order.skill->height * 0.5)) - (int)(order.skill->effect[r]->GetAnotherSkill()->skill->height * 0.5);
				}
			}
			break;
		}
		case eSkillEffect::Hate:// 【ヘイト増減】
		{
			if (order.attacker->GetCharType()) {
				// 【プレイヤーが使用した場合】
				// ヘイト値の増減
				playerHate[order.attacker->GetPlayerOrder()] += order.skill->effect[r]->GetHate()->value;			// 定数
				playerHate[order.attacker->GetPlayerOrder()] = 
					(int)(playerHate[order.attacker->GetPlayerOrder()] * order.skill->effect[r]->GetHate()->rate);	// 倍率

				// ヘイト値上限
				if (playerHate[order.attacker->GetPlayerOrder()] > maxHate) {
					playerHate[order.attacker->GetPlayerOrder()] = maxHate;
				}
				// ヘイト値下限
				if (playerHate[order.attacker->GetPlayerOrder()] < 1) {
					playerHate[order.attacker->GetPlayerOrder()] = 1;
				}
			}
			break;
		}
		case eSkillEffect::Charge:// 【チャージ】
		{
			// チャージスキルをセット
			order.attacker->SetCharge(order.skill->effect[r]->GetCharge()->type, order.skill->effect[r]->GetCharge()->rate, order.skill->effect[r]->GetCharge()->turn);
			break;
		}
		case eSkillEffect::Mode:// 【モード変更】
		{
			if (!order.attacker->GetCharType()) {
				// 【敵の処理】
				// モードの変更
				order.attacker->ModeChange(order.skill->effect[r]->GetMode()->modeNum);
			}
			break;
		}
		}
	}

	return true;	// 発動成功
}
bool Battle::SkillExec(Character* attacker, Skill* skill) {
	return SkillExec({ attacker, skill, 0, skill->totalUseEnergy });
}

// 対象にスキルの効果を与える
void Battle::SkillEffectExec(int FieldX, int FieldY, Character* attacker, Character* target, SkillEffect* effect, int totalUseEnergy, int effectNum) {
	
	switch (effect->type) {
	case eSkillEffect::Attack:
		//////////////////////////////////////
		////////////// 【攻撃】////////////////
		//////////////////////////////////////
	{
		// 対象が死んでたら攻撃しない
		if (target->GetAlive() != eAlive::alive) return;


		// 攻撃で与えたダメージ
		int damageValue = 0;

		target->SetReceiveSkill(eSkillEffect::Attack);	// ダメージフラグ
		// 攻撃回数
		for (int r = 0, l = GetRand(effect->GetAttack()->times_min, effect->GetAttack()->times_max); r < l; r++) {
			damageValue = 0;	// ダメージ数初期化

			damageValue += EnergyConvDmage(attacker, target, effect->energyConv, effect->GetAttack()->hit, effect->GetAttack()->damageRate, totalUseEnergy);	// ダメージ量保存			
			
			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値加算（ダメージ数×１）
				playerHate[attacker->GetPlayerOrder()] += damageValue;
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
			//****【スキル対象者の処理】****//
			// 【プレイヤー】
			if (target->GetCharType()) {
				// ダメージ数表示
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), damageValue, 20, RED, r * 10);
				
				// ダメージ数が-1でなければ
				if (damageValue != -1) {
					// スキル対象保存
					playerField_Save[FieldY][FieldX] = true;
					{
						int i = 0, n = target_Save.size();
						// セーブ済かを判定
						for (; i < n; ++i) {
							if (target_Save[i] == target) {
								break;
							}
						}
						if (i == n) target_Save.push_back(target);
					}
				}
			}
			// 【敵】
			else {
				// ダメージ数表示
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), damageValue, 20, RED, effectNum * 30 + r * 10);
				// ダメージ数が０でなければ && 対象が生きてれば保存
				if (damageValue != -1) {
					// スキル対象保存
					enemyField_Save[FieldY][FieldX] = true;
					{
						int i = 0, n = target_Save.size();
						// セーブ済かを判定
						for (; i < n; ++i) {
							if (target_Save[i] == target) {
								break;
							}
						}
						if (i == n) target_Save.push_back(target);
					}
				}
			}
		}
		break;
	}
	case eSkillEffect::Heal:
		//////////////////////////////////////
		////////////// 【回復】////////////////
		//////////////////////////////////////
	{
		// 対象が死んでたら回復しない
		if (target->GetAlive() != eAlive::alive) return;

		int healValue = 0;	// 回復量初期化
		switch (effect->GetHeal()->type){
		case eHealType::HP:// 【HP回復】
		{
			// 対象回復（補正12）
			healValue += target->Recovery(GetConvValue(effect->energyConv, totalUseEnergy) * effect->GetHeal()->rate * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(eEnergy::Life, true)) * 12);

			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値加算（回復量×2）
				playerHate[attacker->GetPlayerOrder()] += healValue * 2;
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// 【スキル対象者の処理】****//
			// 【プレイヤー処理】
			if (target->GetCharType()) {
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), healValue, 20, YELLOW);
			}
			// 【敵処理】
			else {
				// ダメージ数表示
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), healValue, 20, YELLOW);
			}
			break;
		}
		case eHealType::MP:// 【MP回復】
			/*

			*/
			break;
		case eHealType::BadStatus:// 【全状態異常】
			target->HealBadStatus();
			break;
		case eHealType::Poison:// 【毒】
			target->HealBadStatus(eBadStatus::Poison);
			break;
		case eHealType::Paralysis:// 【麻痺】
			target->HealBadStatus(eBadStatus::Paralysis);
			break;
		case eHealType::Sleep:// 【睡眠】
			target->HealBadStatus(eBadStatus::Sleep);
			break;
		case eHealType::Confusion:// 【混乱】
			target->HealBadStatus(eBadStatus::Confusion);
			break;
		case eHealType::Blind:// 【暗闇】
			target->HealBadStatus(eBadStatus::Blind);
			break;
		case eHealType::Weakness:// 【衰弱】
			target->HealBadStatus(eBadStatus::Weakness);
			break;
		case eHealType::Debuff:// 【全デバフ】
			/*
			
			*/
			break;
		}	
		// 【スキル対象者の処理】****//
		// 【プレイヤー処理】
		if (target->GetCharType()) {

			playerField_Save[FieldY][FieldX] = true;
			{
				int i = 0, n = target_Save.size();
				// セーブ済かを判定
				for (; i < n; ++i) {
					if (target_Save[i] == target) {
						break;
					}
				}
				if (i == n) target_Save.push_back(target);
			}
		}
		// 【敵処理】
		else {
			enemyField_Save[FieldY][FieldX] = true;
			{
				int i = 0, n = target_Save.size();
				// セーブ済かを判定
				for (; i < n; ++i) {
					if (target_Save[i] == target) {
						break;
					}
				}
				if (i == n) target_Save.push_back(target);
			}
		}
		break;
	}
	case eSkillEffect::BadStatus:
		//////////////////////////////////////
		//////////// 【状態異常】//////////////
		//////////////////////////////////////
	{
		// 対象が死んでたら判定しない
		if (target->GetAlive() != eAlive::alive) return;

		eBadStatus bs_Type = effect->GetBadStatus()->type;	// 状態異常の種類取得
		// 状態異常が【どれか一つをランダム付与】であれば消滅以外のどれかに変更
		if (bs_Type == eBadStatus::One_any) {
			bs_Type = (eBadStatus)GetRand(7);
		}

		// 耐性が-1 ＝ 絶対耐性
		if (target->GetDEF_BadStatus(bs_Type) == -1) {
			// 回避成功 & 蓄積もなし
		}
		// 命中が-1 ＝ 必中
		// 0～耐性値 と 0～命中力 の対抗
		// ステータスが低すぎると0しか出なかったりするから×1000して判定
		else if (effect->GetBadStatus()->hit != -1 &&
			(GetRand(effect->GetBadStatus()->hit * 1000)) <
			(GetRand(target->GetDEF_BadStatus(bs_Type) * 1000))) {
			// 回避成功
			// 蓄積値上昇　Rand(0 ~ 威力*0.2)
			target->GiveEddDEF_BadStatus(bs_Type, GetRand((int)(effect->GetBadStatus()->power * 0.2)));
		
			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値計算
				switch (bs_Type) {
				case eBadStatus::Poison:// 【毒】命中×１
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Paralysis:// 【麻痺】命中×１
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Sleep:// 【睡眠】命中×１．５
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Confusion:// 【混乱】命中×１．５
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Blind:// 【暗闇】命中×１
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Weakness:// 【衰弱】命中×３
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					break;
				case eBadStatus::Stun:// 【気絶】命中×１．５
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Death:// 【即死】ヘイト2倍
					playerHate[attacker->GetPlayerOrder()] *= 2;
					break;
				case eBadStatus::Extinction:// 【消滅】ヘイト2倍
					playerHate[attacker->GetPlayerOrder()] *= 2;
					break;
				}
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		
		}
		else {
			// 回避失敗
			// 状態異常を与える
			target->GiveBadStatus(bs_Type, (int)(effect->GetBadStatus()->power* (1.0 + (double)GetRand(-10, 11)*0.01)));
			
			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値計算
				switch (bs_Type){
				case eBadStatus::Poison:// 【毒】威力×３	必中なら威力×１０
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->power * 10;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->power * 3;
					}
					break;
				case eBadStatus::Paralysis:// 【麻痺】命中×３	必中ならヘイト2倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 2;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					}
					break;
				case eBadStatus::Sleep:// 【睡眠】命中×５	必中ならヘイト3倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 3;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 5;
					}
					break;
				case eBadStatus::Confusion:// 【混乱】命中×4	必中ならヘイト2倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 2;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 4;
					}
					break;
				case eBadStatus::Blind:// 【暗闇】命中×３	必中ならヘイト1.5倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] = (int)(playerHate[attacker->GetPlayerOrder()] * 1.5);
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					}
					break;
				case eBadStatus::Weakness:// 【衰弱】命中×１０	必中ならヘイト5倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 5;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 10;
					}
					break;
				case eBadStatus::Stun:// 【気絶】命中×4	必中ならヘイト3倍
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 3;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 4;
					}
					break;
				case eBadStatus::Death:// 【即死】強制でヘイト値Max
					playerHate[attacker->GetPlayerOrder()] = maxHate;
					break;
				case eBadStatus::Extinction:// 【消滅】強制でヘイト値Max
					playerHate[attacker->GetPlayerOrder()] = maxHate;
					break;
				}
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
			// 【スキル対象者の処理】****//
			// 【プレイヤー処理】
			if (target->GetCharType()) {
				// 対象セーブ
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// 【敵処理】
			else {
				// 対象セーブ
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}

		
		break;
	}
	case eSkillEffect::Buff:
		//////////////////////////////////////
		/////////// 【バフ・デバフ】///////////
		//////////////////////////////////////
	{
		// 対象が死んでたら判定しない
		if (target->GetAlive() != eAlive::alive) return;

		// 味方へのバフは命中判定なし
		// 0～回避(AGIとDEXの平均/10) と 0～命中(DEX×技補正)　の対抗
		// ステータスが低すぎると0しか出なかったりするから×1000して判定
		if (attacker->GetCharType() == target->GetCharType() ||
			(GetRand((attacker->GetStatus(eStatus::DEX)) * 1000) * effect->GetBuff()->hit * (attacker->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) >
			(GetRand((int)(((target->GetStatus(eStatus::AGI) * 1000) + (target->GetStatus(eStatus::DEX)) * 1000) * 0.05)) * (target->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1))) {
			
			eBuff buffType = effect->GetBuff()->type;	// バフの種類取得
			// バフが【どれか一つをランダム付与】であればどれかに変更
			if (buffType == eBuff::One_any) {
				buffType = (eBuff)GetRand(21);
			}
			
			// 【命中】
			float rate = 0;	// 倍率
			// 倍率計算
			if (effect->GetBuff()->dir) {
				// バフ	1+sin(π/2000*(使用MP * 変換効率(生命+)))*4
				rate = 1.0f + (float)sin(M_PI * 0.0005 * (GetConvValue(effect->energyConv, totalUseEnergy) * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(effect->energyConv.type, effect->energyConv.direction)))) * 4;
				// 下限1.01倍
				if (rate < 1.01f) {
					rate = 1.01f;
				}
			}
			else {
				// デバフ	1 / (1+sin(π/2000*(変換量 * 変換効率(生命-)))*4)
				rate = 1.0f / (1.0f + (float)sin(M_PI * 0.0005 * (GetConvValue(effect->energyConv, totalUseEnergy) * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(effect->energyConv.type, effect->energyConv.direction)))) * 4);
				// 下限1.1倍
				if (rate > 0.99f) {
					rate = 0.99f;
				}
			}
			// バフ・デバフを与える
			target->GiveBuff(buffType, rate, effect->GetBuff()->trun);

			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値計算	使用魔素（エネルギー）×３
				playerHate[attacker->GetPlayerOrder()] += (int)(GetConvValue(effect->energyConv, totalUseEnergy) * 3);

				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// 【スキル対象者の処理】****//
			// 【プレイヤー処理】
			if (target->GetCharType()) {
				// 対象セーブ
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// 【敵処理】
			else {
				// 対象セーブ
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}
		else {
			//****【攻撃者の処理】****//
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値計算	使用魔素（エネルギー）×１
				playerHate[attacker->GetPlayerOrder()] += (int)(GetConvValue(effect->energyConv, totalUseEnergy));

				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		}

		
		break;
	}
	case eSkillEffect::AnotherSkill:
		//////////////////////////////////////
		////////// 【他のスキル発動】//////////
		//////////////////////////////////////
	{		
		/*ここではなにもしない*/
		break;
	}
	case eSkillEffect::Resurrection:
		//////////////////////////////////////
		/////////////// 【蘇生】///////////////
		//////////////////////////////////////
	{
		// 対象が死んでいなければ判定しない
		if (target->GetAlive() != eAlive::dead) return;

		// 蘇生成功判定
		if (GetRand(99) < effect->GetResurrection()->success) {
			// 【成功】
			// 対象蘇生（補正2）
			int healValue = target->Resurrection(GetConvValue(effect->energyConv, totalUseEnergy) * effect->GetResurrection()->rate * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(eEnergy::Life, true)) * 2);

			// ****【行動者の処理】**** //
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値加算（回復量×5 -> ヘイト2倍）
				playerHate[attacker->GetPlayerOrder()] += healValue * 5;
				playerHate[attacker->GetPlayerOrder()] *= 2;
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// 【スキル対象者の処理】**** //
			// 【プレイヤー処理】
			if (target->GetCharType()) {
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), healValue, 20, YELLOW);
				playerHate[target->GetPlayerOrder()] = 500;	// 蘇生対象のヘイトを500にする
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// 【敵処理】
			else {
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), YELLOW, healValue, 20);
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// セーブ済かを判定
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}
		else {
			// 【失敗】
			// ****【行動者の処理】**** //
			// 【プレイヤー】
			if (attacker->GetCharType()) {
				// ヘイト値加算（ヘイト1.5倍）
				playerHate[attacker->GetPlayerOrder()] = (int)(playerHate[attacker->GetPlayerOrder()] * 1.5);
				// ヘイト値上限
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		}

		break;
	}
	}

}

// エネルギーごとのダメージ計算
int Battle::EnergyConvDmage(Character* attacker, Character* target, tEnergyConv energy, float hit, float damageRate, int totalUseEnergy) {

	// 【ダメージ計算式】直接変換以外の場合
	// 変換ダメージ = 変換量×変換効率×スキル補正倍率×チャージ補正倍率×クリティカル補正倍率×エネルギー補正倍率×ダメージぶれ＋補正ダメージ
	// 補正ダメージ = 補正値×スキル補正倍率×チャージ補正倍率×クリティカル補正倍率×エネルギー補正倍率×ダメージぶれ
	// ダメージの10%は確定ダメージ
	// 90%は防御力を引いたダメージ
	

	//**********************************//
	//***********【回避判定】***********//
	//**********************************//
	// 【回避可能判定】
	if (!target->GetBadStatus(eBadStatus::Sleep).get &&	// 睡眠
		!target->GetBadStatus(eBadStatus::Stun).get &&	// スタン
		!target->GetBadStatus(eBadStatus::Paralysis).get) {	// 麻痺
		// 行動阻害系の状態異常をうけていなければ

		// 【回避判定】
		// hit == -1：必中
		// チャージ補正（命中補正）== -1 必中
		// 0～回避(AGIとDEXの平均/20) と 0～命中(DEX×技補正)　の対抗
		// 実力均衡なら確率5%で回避する
		// ステータスが低すぎると0しか出なかったりするから×1000して判定
		if( hit != -1 && attacker->GetCharge(eCharge::Hit) != -1 &&
			(GetRand((attacker->GetStatus(eStatus::DEX)) * 1000) * hit * (attacker->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) * attacker->GetCharge(eCharge::Hit) <
			(GetRand((int)(((target->GetStatus(eStatus::AGI) * 1000) + (target->GetStatus(eStatus::DEX)) * 1000) * 0.025)) * (target->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) * target->GetCharge(eCharge::Avoid)){
			// 回避成功
			return -1;
		}
	}

	//**********************************//
	//*********【ダメージ計算】*********//
	//**********************************//

	int totalDamage = 0;	// 合計ダメージ数

	double energyDamage = 0;	// エネルギーダメージ数

	// 補正ダメージ
	double damageCorrection = 0;	// 補正ダメージ数
	switch (energy.correction){
	case 2:// 【STR補正】
		// STR × 95%～115% + -2～2 ダメージ
		damageCorrection = attacker->GetStatus(eStatus::STR) * (1.0 + (double)GetRand(-500, 1500)*0.0001) + GetRand(-2, 2);
		break;
	}

	// 【エネルギー変換効率】
	energy.rate = GetConvValue(energy, totalUseEnergy);	// 変換割合を変換量に
	// エネルギー変換なしでなければ
	if (energy.type != eEnergy::None) {
		if (energy.convType == eConvType::Target) {
			// 相手の魔素の直接変換の場合

			// 変換量に変換効率をかける
			energy.rate *= attacker->GetEfficiency(energy.type, energy.direction);

			// +変換で相手のMPが変換量（変換効率計算済み）に足りなかったら
			if (energy.direction && energy.rate > target->GetStatus(eStatus::MP)) {
				// 変換量を残存MP分にする
				energy.rate = target->GetStatus(eStatus::MP);
			}

			// MUE対抗
			// 自分のMUEが相手のMUEを上回っていなければ無効
			if (attacker->GetStatus(eStatus::MUE) <= target->GetStatus(eStatus::MUE)) {
				energy.rate = 0;	// 変換ダメージを0に
			}
			// 自分のMUEが相手のMUEを1000以上上回っていれば倍率100
			else if (attacker->GetStatus(eStatus::MUE) > target->GetStatus(eStatus::MUE) + 1000) {
				energy.rate *= 100;
			}
			else {
				// MUEの差で
				// 100 - cos((M_PI_2 / 1000)×MUEの差)×100
				energy.rate *= (100 - (cos((M_PI / 2000) * (attacker->GetStatus(eStatus::MUE) - target->GetStatus(eStatus::MUE))) * 100));
			}

		}
		else {
			// それ以外
			// 自分の魔素の変換 or 場の魔素の変換の場合

			// 変換量に変換効率をかける
			energy.rate *= attacker->GetEfficiency(energy.type, energy.direction);
		}
		energyDamage = energy.rate;	// エネルギー変換量をエネルギーダメージに加算
	}

	// 【スキルダメージ倍率】
	energyDamage *= damageRate;
	damageCorrection *= damageRate;

	// 【チャージ補正】（ダメージ補正）
	energyDamage *= attacker->GetCharge(eCharge::Damage);
	damageCorrection *= attacker->GetCharge(eCharge::Damage);


	// 【クリティカル判定】
	// 0～被害者のDEX と 0～攻撃者のDEX/20　の対抗
	// 実力均衡なら確率5%でクリティカル
	// ステータスが低すぎると0しか出なかったりするから×1000して判定
	if (GetRand((attacker->GetStatus(eStatus::DEX) * 1000) / 20) >= GetRand(target->GetStatus(eStatus::DEX) * 1000)) {
		// ダメージ2倍
		energyDamage *= 2;
		damageCorrection *= 2;
	}

	// 【ダメージぶれ】×90%～110%
	energyDamage *= (1.0 + (double)GetRand(-1000, 1000)*0.0001);

	// 【エネルギー補正】
	switch (energy.type){
	case eEnergy::Heat:// 【熱】
		if (energy.direction)
		{ //+
			// ダメージの10%は確定ダメージ
			// 90%は防御力を引いたダメージ
			energyDamage *= 5;	// 補正5
			// 熱ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-
			energyDamage *= 5;	// 補正5
			// 冷気ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->ColdDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		break;
	case eEnergy::Elec:// 【電気】
		if (energy.direction)
		{ //+
			energyDamage *= 10;	// 補正10
			// 電気ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->ElecDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	case eEnergy::Shine:// 【光】
		if (energy.direction)
		{ //+
			//energyDamage *= 1;	// 補正1
			// 熱ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	case eEnergy::Sound:// 【音】
		if (energy.direction)
		{ //+
			energyDamage *= 0.5;	// 補正0.5
			// 物理ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	case eEnergy::Motion:// 【運動】
		if (energy.direction)
		{ //+
			energyDamage *= 3;	// 補正3
			// 物理ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	case eEnergy::Gravity:// 【重力】
		if (energy.direction)
		{ //+
			energyDamage *= 20;	// 補正20
			// 物理ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	case eEnergy::Life:// 【生命】
		if (energy.direction)
		{ //+

		}
		else
		{ //-

		}
		break;
	case eEnergy::Static:// 【静止】
		if (energy.direction)
		{ //+

		}
		else
		{ //-

		}
		break;
	case eEnergy::Nucleus:// 【核】
		if (energy.direction)
		{ //+
			energyDamage *= 100;	// 補正100
			// 熱ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
			// 物理ダメージ
			totalDamage += target->Damage(energyDamage * 0.1);		// 確定ダメージ
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// 防御力を引いたダメージ
		}
		else
		{ //-

		}
		break;
	}

	// 補正ダメージ
	totalDamage += target->Damage(damageCorrection * 0.1);		// 確定ダメージ
	totalDamage += target->PhysDamage(damageCorrection * 0.9);	// 防御力を引いたダメージ


	return (int)totalDamage;	// 合計ダメージを返す
}
// フィールドMPのゲッター
int Battle::GetFieldMP() {
	return fieldMP;
}
// フィールドMP増加
void Battle::IncreaseFieldMP(int value) {
	if (value <= 0)	return;		// 0以下はエラー

	fieldMP += value;	// MPの増加
}
// フィールドMP減少
void Battle::DecreaseFieldMP(int value) {
	if (value <= 0)	return;		// 0以下はエラー

	fieldMP -= value;	// MPの減少
	// 0未満になったら0に戻す
	if (fieldMP < 0) {
		fieldMP = 0;
	}
}

// 死んだ敵のデータの削除
void Battle::DeleteDeadEnemy() {
	// スキル対象保存初期化
	for (int i = 0; i < playerFieldHeight; i++) {
		for (int j = 0; j < playerFieldWidth; j++) {
			playerField_Save[i][j] = false;
		}
	}
	for (int i = 0; i < enemyFieldHeight; i++) {
		for (int j = 0; j < enemyFieldWidth; j++) {
			enemyField_Save[i][j] = false;
		}
	}
}

// 敵全滅判定
bool Battle::CheckEnemysDead() {
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		if (enemy[i]->GetAlive() == eAlive::alive){
			// 誰か一人でも生きていればfalse
			return false;
		}
	}

	return true;	// 全滅
}

// プレイヤー全滅判定
bool Battle::CheckPlayersDead() {
	for (int i = 0, n = player.size(); i != n; ++i) {
		if (player[i]->GetAlive() == eAlive::alive) {
			// 誰か一人でも生きていればfalse
			return false;
		}
	}

	return true;	// 全滅
}

void Battle::Draw() {
	DrawGraph(0, 0, gr_Back);

	// 【テキストボックス表示】
	TextBox::Draw();

	// debug
	
	// 敵
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		enemy[i]->Draw(enemyFieldX, enemyFieldY, cellSize);

	}

	/*for (int i = 0, n = enemy.size(); i < n; i++) {
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 0, "%s", enemy[i]->GetName().c_str());
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 1, "HP :%d", enemy[i]->GetStatus(eStatus::HP));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 2, "MP :%d", enemy[i]->GetStatus(eStatus::MP));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 3, "STR:%d", enemy[i]->GetStatus(eStatus::STR));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 4, "DEX:%d", enemy[i]->GetStatus(eStatus::DEX));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 5, "AGI:%d", enemy[i]->GetStatus(eStatus::AGI));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 6, "LUC:%d", enemy[i]->GetStatus(eStatus::LUC));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 7, "D_P:%d", enemy[i]->GetStatus(eStatus::DEF_PHYS));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 8, "D_H:%d", enemy[i]->GetStatus(eStatus::DEF_HEAT));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 9, "D_C:%d", enemy[i]->GetStatus(eStatus::DEF_COLD));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 10, "D_E:%d", enemy[i]->GetStatus(eStatus::DEF_ELEC));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 11, "MUE:%d", enemy[i]->GetStatus(eStatus::MUE));
	}
	for (int i = 0, n = player.size(); i < n; i++) {		
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 0, "%s", player[i]->GetName().c_str());
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 1, "HP :%d", player[i]->GetStatus(eStatus::HP));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 2, "MP :%d", player[i]->GetStatus(eStatus::MP));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 3, "STR:%d", player[i]->GetStatus(eStatus::STR));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 4, "DEX:%d", player[i]->GetStatus(eStatus::DEX));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 5, "AGI:%d", player[i]->GetStatus(eStatus::AGI));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 6, "LUC:%d", player[i]->GetStatus(eStatus::LUC));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 7, "D_P:%d", player[i]->GetStatus(eStatus::DEF_PHYS));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 8, "D_H:%d", player[i]->GetStatus(eStatus::DEF_HEAT));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 9, "D_C:%d", player[i]->GetStatus(eStatus::DEF_COLD));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 10, "D_E:%d", player[i]->GetStatus(eStatus::DEF_ELEC));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 11, "MUE:%d", player[i]->GetStatus(eStatus::MUE));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 12, "ｽｷﾙ数:%d", player[i]->GetSkillList().size());
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 13, "ヘイト:%d", playerHate[i]);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 13, "回復量:%d", player[i]->GetNomalAttack()->effect.back()->GetHeal()->value);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 14, ":%f", player[i]->GetNomalAttack()->effect.back()->GetHeal()->rate);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 15, ":%d", player[i]->GetNomalAttack()->effect.back()->GetHeal()->type);
		
	}*/

	/*// フィールドのMP
	DrawString(BLUE, 800, 0, "場MP:%d/%d", fieldMP, fieldMaxMP);
	// ターン数
	DrawString(BLUE, 800, 32, "ターン数:%d", turnNum);

	for (int i = 0, n = skillOrder->order.size(); i < n; ++i) {
		DrawString(BLUE, 800, 64 + i*32, "速度:%f", skillOrder->order[i].speed);
	}*/
	
	// プレイヤーのステータス
	for (int i = 0, n = player.size(); i < n; i++) {
		player[i]->Draw();
	}


	// ダメージ量表示
	for (int i = 0, n = graphicEffect.size(); i < n; i++) {
		graphicEffect[i]->Draw(cellSize);
	}
	for (int i = 0, n = damageEffect.size(); i < n; i++) {
		damageEffect[i].Draw();
	}
	

	switch (step)
	{
	case eBattleStep::START:

		break;
	case eBattleStep::COMMAND:
		Draw_COMMAND();
		break;
	case eBattleStep::BATTLE:

		break;
	case eBattleStep::END:

		break;
	}
}

void Battle::Draw_COMMAND() {

	if (TextBox::textBoxFlag) return;

	const int text_Top = WINDOW_HEIGHT - 300;	// テキストボックスの上端

	switch (commandStep){
	case eCommand::CharaSelect:// 【キャラ選択】
		break;
	case eCommand::SkillSelect:// 【使用スキル選択】
	{
		// スクロールバー
		if (skillNum > 5) {
			DrawStretchGraph(752, (int)(text_Top + 15 + selectSkillArea * (270.0 / skillNum)), gr_scroll, 14, (int)(270 * (5.0 / skillNum)));
		}
		else {
			DrawStretchGraph(752, text_Top + 15, gr_scroll, 14, 270);
		}

		// スキル選択ウィンドウ
		DrawGraph(256, text_Top + 10, gr_SkillSelect);

		// スキル選択
		// スキルの数が６個以上かで処理を分ける
		if (skillNum > 5) {
			for (int i = 0; i < 5; ++i) {
				if (selectSkillArea + i >= 0 && selectSkillArea + i < skillNum) {
					DrawString(WHITE, 320, text_Top + 40 + i * 48, selectSkillList[selectSkillArea + i]->name.c_str());
				}
			}
			// カーソル
			DrawString(WHITE, 288, text_Top + 40 + (48 * selectSkillCursor), "◆");
		}
		else {
			for (int i = 0; i < skillNum; ++i) {
				DrawString(WHITE, 320, text_Top + 40 + i * 48, selectSkillList[i]->name.c_str());
			}
			// カーソル
			DrawString(WHITE, 288, text_Top + 40 + (48 * selectSkill), "◆");
		}

		// 【スキル詳細】
		DrawString(WHITE, 960, text_Top + 20, "MP");
		if (commandPreSelect[selectChara].fullPowerFlag) {	// 全力フラグ
			// 【全力】
			DrawString(YELLOW, 784, text_Top + 20, "【 全 力 】");
			// 使用MP
			if (selectSkillList[selectSkill]->totalUseEnergy == 0) {	// 0で割らないように
				DrawString(RED, 960 + 96, text_Top + 20, "0");
				DrawString(BLUE, 960 + 192, text_Top + 20, "0");
				DrawString(YELLOW, 960 + 288, text_Top + 20, "0");
				//DrawString(WHITE, 960 + 384, text_Top + 20, "必要INT値：0");
			}
			else {
				DrawString(RED, 960 + 96, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[0] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				DrawString(BLUE, 960 + 192, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[1] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[2] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				//DrawString(WHITE, 960 + 384, text_Top + 20, "必要INT値：%d", player[selectChara]->GetStatus(eStatus::MUE));
			}
			if (selectChara == 0) {

			}
		}
		else {
			// 【通常】
			DrawString(WHITE, 784, text_Top + 20, "【 通 常 】");
			// 使用MP
			DrawString(RED, 960 + 96, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[0]);
			DrawString(BLUE, 960 + 192, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[1]);
			DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[2]);
			//DrawString(WHITE, 960 + 384, text_Top + 20, "必要INT値：%d", selectSkillList[selectSkill]->totalUseEnergy);
		}
		// スキル説明文	
		for (int i = 0, n = selectSkillList[selectSkill]->explanatoryText.size(); i < n; ++i) {
			DrawString(WHITE, 784, text_Top + 72 + i * 32, selectSkillList[selectSkill]->explanatoryText[i].c_str());
		}
		
		//break;
	}
	case eCommand::ActionSelect:// 【行動選択】
	{
		// コマンド選択中のキャラ名
		DrawString(WHITE, 16, text_Top, player[selectChara]->GetName().c_str());
		// 選択肢
		DrawString(WHITE, 64, text_Top + 32, "攻撃");
		DrawString(WHITE, 64, text_Top + 64, "スキル");
		DrawString(WHITE, 64, text_Top + 96, "アイテム");
		DrawString(WHITE, 64, text_Top + 128, "防御");
		DrawString(WHITE, 64, text_Top + 160, "逃げる");
		// カーソル
		DrawString(WHITE, 32, text_Top + 32 + (32 * selectAction), "◆");
		break;
	}
	case eCommand::ItemSelect:	// 【使用アイテム選択】
	{
		vector<string> itemname;	// エイリアス

		for (int i = 0, n = Player::bag[selectChara].size();i < n;++i) {
			// 名前リスト作成
			itemname.push_back(Player::bag[selectChara][i]->GetName());
		}

		// スクロールバー
		if (itemname.size() > 5) {
			DrawStretchGraph(752, (int)(text_Top + 15 + selectSkillArea * (270.0 / itemname.size())), gr_scroll, 14, (int)(270 * (5.0 / itemname.size())));
		}
		else {
			DrawStretchGraph(752, text_Top + 15, gr_scroll, 14, 270);
		}

		// スキル選択ウィンドウ
		DrawGraph(256, text_Top + 10, gr_SkillSelect);

		for (int i = selectItem.pos.Y / 5 * 5, n = itemname.size(), j = 0; (i < n && j < 5); ++i, ++j) {
			// 所持しているアイテムを表示
			DrawString(WHITE, 256 + 64, text_Top + 40 + j * 48, itemname[i].c_str());	// 名前を表示
		}

		// ガイド
		if (itemname.size() / 5 > 0 && selectItem.pos.Y / 5 * 5 != itemname.size() / 5) {
			// 次のページがあるとき
			DrawString(WHITE, 352, text_Top + 16 + 5 * 48, "：");
		}
		if (itemname.size() > 5) {
			// 前のページがあるとき
			DrawString(WHITE, 352, text_Top, "：");
		}

		// スキル説明文
		if (Player::bag[selectChara][selectItem.pos.Y]->GetSkill()) {
			for (int i = 0, n = Player::bag[selectChara][selectItem.pos.Y]->GetSkill()->explanatoryText.size(); i < n; ++i) {
				DrawString(WHITE, 784, text_Top + 72 + i * 48, Player::bag[selectChara][selectItem.pos.Y]->GetSkill()->explanatoryText[i].c_str());
			}
		}
		DrawString(WHITE, 256 - 32 + 64, text_Top + 40 + selectItem.pos.Y % 5 * 48, "●");	// アイコン
		break;
	}
	case eCommand::TargetSelect:// 【スキル等の対象範囲選択】
	{
		// マス描画
		for (int i = 0, n = enemyField.size(); i < n; i++) {
			for (int j = 0, m = enemyField[i].size(); j < m; j++) {
				DrawBox(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, cellSize, cellSize, false);
			}
		}

		// 【敵から選択】
		if (selectSkillData->selectTarget) {
			// 敵の当たり判定表示
			for (int i = 0, n = enemyField.size(); i < n; i++) {
				for (int j = 0, m = enemyField[i].size(); j < m; j++) {
					//DrawBox(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, cellSize, cellSize, false);
					if (enemyField[i][j] != nullptr && enemyField[i][j]->GetAlive() == eAlive::alive) {
						DrawStretchGraph(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_target, cellSize, cellSize);
					}
				}
			}
			// スキル範囲表示
			for (int i = 0, n = selectSkillData->collision.size(); i < n; ++i) {
				for (int j = 0, m = selectSkillData->collision[i].size(); j < m; ++j) {

					// アウトオブレンジ対策

					//	j + selectSkillData->x >= 0 && j + selectSkillData->x < enemyFieldWidth) {

						switch (selectSkillData->collision[i][j]) {
						case 0:	// 【判定なし】
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit1, cellSize, cellSize);
							break;
						case 1:// 【判定１個】
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit2, cellSize, cellSize);
							break;
						case 2:// 【判定２個】
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit3, cellSize, cellSize);
							break;
						default:// 【判定３個以上】
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit4, cellSize, cellSize);
							break;
						}
					//}
				}
			}
		}
		// 【味方から選択】
		else {
			// コマンド選択中のキャラ名
			DrawString(WHITE, 16, text_Top, player[selectChara]->GetName().c_str());
			// 選択肢
			DrawString(WHITE, 64, text_Top + 32, "攻撃");
			DrawString(WHITE, 64, text_Top + 64, "スキル");
			DrawString(WHITE, 64, text_Top + 96, "アイテム");
			DrawString(WHITE, 64, text_Top + 128, "防御");
			DrawString(WHITE, 64, text_Top + 160, "逃げる");
			// カーソル
			DrawString(WHITE, 32, text_Top + 32 + (32 * selectAction), "◆");

			// 選択中のスキル
			DrawString(WHITE, 256, text_Top + 32, selectSkillData->name.c_str());

			// 対象キャラ選択
			for (int i = 0, n = player.size(); i < n; ++i) {
				DrawString(WHITE, 544, text_Top + 32 + i * 32, player[i]->GetName().c_str());
			}
			// カーソル
			DrawString(WHITE, 512, text_Top + 32 + (32 * selectTargetAlly), "◆");

		}

		break;
	}
	}

	// フィールドでの操作説明
	//DrawGraph(WINDOW_WIDTH - 679, WINDOW_HEIGHT - 400, gr_Operation);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------ダメージ数の表示-------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::array<int, 10> DamageEffect::gr_RedNum;	// 数字画像（赤）

// ダメージエフェクト（ダメージ量の表示とか）
DamageEffect::DamageEffect(int x, int y, int value, int frame, int color, int delay) {
	this->x = x;
	this->y = y;
	this->value = value;
	this->count = 0;
	this->frame = frame;
	this->delay = delay;
	this->color = color;
}
DamageEffect::~DamageEffect() {

}
void DamageEffect::Init() {
	// 数字画像（赤）読み込み
	gr_RedNum[0] = LoadGraph("img\\battle\\num\\red_0.png");
	gr_RedNum[1] = LoadGraph("img\\battle\\num\\red_1.png");
	gr_RedNum[2] = LoadGraph("img\\battle\\num\\red_2.png");
	gr_RedNum[3] = LoadGraph("img\\battle\\num\\red_3.png");
	gr_RedNum[4] = LoadGraph("img\\battle\\num\\red_4.png");
	gr_RedNum[5] = LoadGraph("img\\battle\\num\\red_5.png");
	gr_RedNum[6] = LoadGraph("img\\battle\\num\\red_6.png");
	gr_RedNum[7] = LoadGraph("img\\battle\\num\\red_7.png");
	gr_RedNum[8] = LoadGraph("img\\battle\\num\\red_8.png");
	gr_RedNum[9] = LoadGraph("img\\battle\\num\\red_9.png");
}
void DamageEffect::Fina(){
	// 数字画像（赤）
	for (int i = 0, n = gr_RedNum.size(); i < n; ++i) {
		DeleteGraph(gr_RedNum[i]);
	}
}
bool DamageEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > 50;
}
void DamageEffect::Draw() {
	if (delay-- > 0) return;
	if (value == -1) {
		DrawString(color, x, y, "MISS");
	}
	else {
		//DrawString(color, x, y, "%d", value);
		int tmp = value;
		int i = 0;
		while (tmp > 0) {
			DrawGraph(x - i * 25, y - count, gr_RedNum[tmp % 10]);

			tmp /= 10;
			++i;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------画像を表示するエフェクト--------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicEffect::GraphicEffect(short x, short y, int gr, short frame, short delay) :
	x(x),
	y(y),
	gr(gr),
	frame(frame),
	count(0),
	delay(0)
{
}
GraphicEffect::~GraphicEffect() {
}
bool GraphicEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > frame;
}
void GraphicEffect::Draw(int cellsize) {
	if (delay-- > 0) return;

	DrawStretchGraph(x, y, gr, cellsize, cellsize);
}

DivGraphicEffect::DivGraphicEffect(short x, short y, int gr, unsigned __int8 divX, unsigned __int8 divY, short divNum, unsigned __int8 width, unsigned __int8 height, short delay) :
	GraphicEffect(x, y, gr, divNum * 4 - 1, delay),
	divX(divX),		// 分割数X
	divY(divY),		// 分割数Y
	divNum(divNum),	// 総分割数
	width(width),
	height(height)
{
}
DivGraphicEffect::~DivGraphicEffect() {
}
bool DivGraphicEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > frame;
}
void DivGraphicEffect::Draw(int cellsize) {
	if (delay-- > 0) return;

	//const int divX = (count / 4) % divNum;
	//const int divY = 0;

	const unsigned __int8 dx = (count / 4) % divX;
	const unsigned __int8 dy = (count / 4) / divX;

	//DrawStretchGraph(x, y, gr, cellsize, cellsize);
	DrawStretchGraph(x, y, gr, dx, dy, cellsize * width, cellsize * height);
}
