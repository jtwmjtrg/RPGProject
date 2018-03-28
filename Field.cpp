#include "Field.h"
#include "SkillTree.h"

map<int, int>           Field::mapChip;			// マップデータ
vector<vector<int> >    Field::mapData;			// マップデータ
bool					Field::encFlag;	// エンカウントフラグ
vector<int>				Field::encList;	// エンカウントリスト

Field::Field(){
	mode = StageMode::FIELD;
	// 画像
	// 音
	fieldBGM = 0;

	encCount = -1;		// エンカウントまでの残り歩数
	encMinCount = -1;	// 最小エンカウント歩数
	encMaxCount = -1;	// 最大エンカウント歩数

	shop = new Shop();	// ショップのメモリ確保

	LoadMapData();		// マップデータの読み込み
	//LoadUnitData();	// ユニット配置データ読み込み

	waitBattle		= false;
	waitBattleCount = 0;

	for (int i = 0;i < 2;++i) {
		// string->const_char*変換
		string str = "img\\field\\wait_battle" + to_string(i) + ".png";	// アドレス生成
		char* cstr = new char[str.size() + 1];			// メモリ確保
		strcpy_s(cstr, str.size() + 1, str.c_str());	// コピー
		waitBattleGraph.push_back(LoadGraph(cstr));

		delete cstr;
	}

	isFollow = true;	// カメラの追従
	wait_cam_time_ = 0; // カメラの待機時間
}
Field::~Field(){
	// 音を止める
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);
	// 画像削除
	for (int i = 0;i < (int)waitBattleGraph.size();++i) {
		DeleteGraph(waitBattleGraph[i]);
	}
}

void Field::LoadMapData(){
	if (fieldBGM != 0)
	{
		// BGMが流れているならとめてから処理スタート
		StopSoundMem(fieldBGM);
		DeleteSoundMem(fieldBGM);
	}

	performanceCount = 0;	// 切り替え演出スタート
	performanceSpeed = 16;	// 切り替えスピード

	// ---------------------------------------------
	// マップデータ---------------------------------

	// マップチップ削除
	for (auto itr = mapChip.begin();itr != mapChip.end();itr++) {
		DeleteGraph(itr->second);
	}
	mapChip.clear();
	mapData.clear();
	mapData.shrink_to_fit();
	encList.clear();		// エンカウントリスト初期化
	encList.shrink_to_fit();

	ifstream ifs("mapdata\\" + to_string(Player::stage) + ".csv", ios::in);
	if (!ifs){
		// 読み込み失敗
		return;
	}

	int i = 0;	// ループ回数をカウント
	string str;	// 入力された文字列(行)を保存


	int mapHeight = 0;		// マップの縦幅
	int mapWidth = 0;		// マップの横幅
	
	//////////////////////////////////////
	//////// 【1行目-マップサイズ】////////
	//////////////////////////////////////
	{
		getline(ifs, str);
		stringstream stream(str);	// strをストリームとして持つ
		string token;				// 入力された文字列(カンマ区切り)を保存
		// 1【マップ横幅】
		getline(stream, token, ',');
		mapWidth = boost::lexical_cast<int>(token);
		// 2【マップ縦幅】
		getline(stream, token, ',');
		mapHeight = boost::lexical_cast<int>(token);
	}
	//////////////////////////////////////
	//////// 【2行目-エンカウント】////////
	//////////////////////////////////////
	{
		getline(ifs, str);
		stringstream stream(str);	// strをストリームとして持つ
		string token;				// 入力された文字列(カンマ区切り)を保存
		// 1【エンカウントフラグ】
		getline(stream, token, ',');
		encFlag = token == "1";
		if (encFlag) {
			// 2【最小エンカウント歩数】
			getline(stream, token, ',');
			encMinCount = boost::lexical_cast<int>(token);
			// 3【最大エンカウント歩数】
			getline(stream, token, ',');
			encMaxCount = boost::lexical_cast<int>(token);
			encCount = GetRand(encMinCount, encMaxCount);	// エンカウントまでの歩数を更新
			// 4【エンカウントid】
			while (getline(stream, token, ',')) {
				if (token == "") break;
				encList.push_back(boost::lexical_cast<int>(token));
			}
		}
		else {
			encMinCount = -1;
			encMaxCount = -1;
			encCount = -1;
		}
	}
	mapData.resize(mapHeight);		// 縦幅メモリ確保

	while (getline(ifs, str)){
		//mapData.resize(i + 1);	// メモリ確保

		stringstream stream(str);	// strをストリームとして持つ
		string token;				// 入力された文字列(カンマ区切り)を保存

		// ループ２回目以降
		/*if (i != 0) {
			mapData[i].resize(mapData[i - 1].size());	// 変則サイズを受け付けられない代わりに処理の高速化を狙う
		}*/
		mapData[i].resize(mapWidth);		// 横幅メモリ確保
		int j = 0;	// ループ回数をカウント

		while (getline(stream, token, ',')) {
			/*// ループ一回目
			if (i == 0) {
				mapData[i].push_back(boost::lexical_cast<int>(token));
			}
			// ループ２回目以降
			else {
				mapData[i][j] = boost::lexical_cast<int>(token);
			}*/
			mapData[i][j] = boost::lexical_cast<int>(token);
			// 新しい要素であればマップに登録
			auto itr = mapChip.find(mapData[i][j]);
			if (itr == mapChip.end()) {
				// string->const_char*変換
				string str = "img\\mapchip\\" + token + ".png";	// アドレス生成
				char* cstr = new char[str.size() + 1];			// メモリ確保
				strcpy_s(cstr, str.size() + 1, str.c_str());	// コピー

				mapChip.insert(make_pair(mapData[i][j], LoadGraph(cstr)));	// 画像ハンドルの関連付け

				delete cstr;	// メモリ開放
			}

			j++;	// カウントを進める
		}
		
		i++;		// カウントを進める
	}

	ifs.close();	// ファイルクローズ

	// マップデータ---------------------------------
	// ---------------------------------------------
	

	// ---------------------------------------------
	// イベントデータ

	eventData.clear();
	eventData.shrink_to_fit();

	ifstream ifs2;
	ifs2.open("eventdata\\" + to_string(Player::stage) + ".csv");
	if (!ifs2) {
		// ファイルがない場合
		eventNum = 0;
	}
	else {
		int i2 = 0;		// ループ回数をカウント
		string str2;	// 入力された文字列(行)を保存

		// 1列取得
		while (getline(ifs2, str2)) {
			eventData.resize(i2 + 1);	// メモリ確保

			stringstream stream2(str2);	// strをストリームとして持つ
			string token2;				// 入力された文字列(カンマ区切り)を保存

			while (getline(stream2, token2, ',')) {
				eventData[i2].push_back(token2);
			}

			++i2;		// カウントを進める
		}

		ifs2.close();	// ファイルクローズ

		eventNum = 100;		// イベントの数を保存
		eventHead = 1;		// イベントの進行度を表す
	}
	// イベントデータ
	// ---------------------------------------------

	// 音
	{
		string str = "sound\\" + to_string(Player::stage) + ".wav";	// アドレス生成
		char* cstr = new char[str.size() + 1];			// メモリ確保
		strcpy_s(cstr, str.size() + 1, str.c_str());	// コピー
		fieldBGM = LoadSoundMem(cstr);
		PlaySoundMem(fieldBGM, true);
	}
}

void Field::Process(){
	
	// テキストボックスの表示中は毎フレーム処理を行わない
	if (TextBox::textBoxFlag) return;

	// 戦闘開始処理
	if (waitBattle) {
		if (waitBattleCount < 30 - 1) {
			++waitBattleCount;
		}
		else {
			encCount = GetRand(encMinCount, encMaxCount);	// エンカウントまでの歩数を更新
			Game::mode = eStageMode::BATTLE;	// 戦闘開始
		}

		return;
	}

	playerPreStage = Player::stage;	// 直前のステージを保存

	CameraProcess();

	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed > 0) {
		// ステージ切り替え時の演出
		if (performanceCount * performanceSpeed < WINDOW_HEIGHT)
		{
			++performanceCount;
		}
		return;
	}

	if (isEvent) {
		EventProcess(now_event_id_);
	}
	if (eventNum != 0) {
		// イベントがあるなら
		for (unsigned int i = 0,n= eventData.size(); i < n; ++i) {
			if (Player::isFinish[Player::stage][i]) {
				// 消化済みイベント
			}
			else {
				if (eventData[i][1] == "FIRST"){
					// 初めて訪れたとき
					if (!isEvent) {
						eventHead = 2;
						isEvent = true;
					}
					now_event_id_ = i;
					EventProcess(i);
					break;
				}
				else if(eventData[i][1] == "TALK") {
					// 特定のユニットに話しかけた時

					if (Player::talkFlag.flag) {
						if (Player::talkFlag.pos_.X == unitData[boost::lexical_cast<int>(eventData[i][2])]->x
							&& Player::talkFlag.pos_.Y == unitData[boost::lexical_cast<int>(eventData[i][2])]->y) {
							if (!isEvent) {
								eventHead = 3;
								isEvent = true;
							}
							now_event_id_ = i;
							EventProcess(i);
							break;
						}
					}
				}
				else if (eventData[i][1] == "CATCH_EMOTE") {
					// 特定のエモートが使用されたとき

					if (Player::emote_lock_ > 0) {
						// プレイヤーがエモートしている

						for (int j = 0, m = Emote::GetEmoteSize();j < m;++j) {
							if (std::stoi(eventData[i][2]) == Emote::GetEmoteId(j)) {
								// 指定のエモートが使用された

								// イベント起動
								if (!isEvent) {
									eventHead += 2;
									isEvent = true;
								}
								now_event_id_ = i;
								EventProcess(i);	// イベント進行
								i = 999;
								break;
							}
						}
					}
				}
				else if(eventData[i][1] == "FINISH"){
					// ほかのイベントが終わったかどうか監視する

					bool check_check_check = true;

					if (!isEvent) {
						for (eventHead = eventHead + 1;eventData[i][eventHead] != "FINISH_END";eventHead += 2) {
							if (Player::isFinish[std::stoi(eventData[i][eventHead])][std::stoi(eventData[i][eventHead + 1])]) {

							}
							else {
								eventHead = 1;				// 超重要初期化
								check_check_check = false;	// イベントが終わってない
								break;
							}
						}

						if (check_check_check) {
							// イベント起動
							if (!isEvent) {
								++eventHead;
								isEvent = true;
							}
							now_event_id_ = i;
							EventProcess(i);	// イベント進行
							break;
						}
					}
				}
			}
		}
	}

	CameraProcess();	// カメラ更新

	Emote::Process();	// エモート

	if (isEvent) {
		// イベント中は以下の操作・処理を行わない
		return;
	}
	// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
	// イベント中でも受け付ける処理
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// イベント中受け付けない処理
	// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	if (Player::flag_shop) {
		shop->Process();
		return;
	}

	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed <= 0) {
		// 演出中でないなら
		// 行動実行中のユニットがいれば行動させる
		if (!exec_Unit.empty()) {
			if (!exec_Unit.front()->ActionExecution()) {
				// 次の行動が無ければ行動実行中のユニットから消す
				exec_Unit.erase(exec_Unit.begin());
			}
			return;
		}

		// プレイヤーの移動処理
		if(Player::Process() && encFlag) {
			// MP自動回復
			for (int i = 0, n = Player::character.size(); i < n; ++i) {
				Player::character[i]->AutoIncMP_Field();
			}
			// 移動したらエンカウントまでの歩数を1減らす
			--encCount;
		}

		// セーブ時にユニットのモードもセーブする
		if (Player::saveFlag) {
			SaveUnitMode(Player::stage);	// 現在いるマップのユニットのモードのセーブ
		}

		// 移動に対応してプレイヤーに位置の修正等を行う
		for (int i = 0;i != 2;++i) {
			for (int j = 0;j != 2;++j) {
				MapEffect(PlayerPosition(i, j));
			}
		}
		
		// ユニット処理
		{
			// プレイヤーの区分け座標
			const int playerAreaX = Player::pos_.X / CHIPSIZE / AREASIZE;
			const int playerAreaY = Player::pos_.Y / CHIPSIZE / AREASIZE;

			// プレイヤーの周囲1エリアのみ処理する
			for (int i = (playerAreaY > 0 ? -1 : 0), n = (playerAreaY < (signed)(unitArea.size() - 1) ? 1 : 0); i <= n; ++i) {
				for (int j = (playerAreaX > 0 ? -1 : 0), m = (playerAreaX < (signed)(unitArea[playerAreaY + i].size() - 1) ? 1 : 0); j <= m; ++j) {
					
					for (auto itr = unitArea[playerAreaY + i][playerAreaX + j].begin(), end = unitArea[playerAreaY + i][playerAreaX + j].end(); itr != end; ++itr) {
						// プレイヤーからユニットへの当たり判定
						if ((*itr)->CheckColl(Player::pos_.X, Player::pos_.Y)) {
							Player::PosCorrection();
						}
						// 更新処理
						if ((*itr)->Process()) {
							exec_Unit.push_back((*itr));	// 実行中のユニットに追加
						}
						// 区分け間を移動したら
						if ((*itr)->x / CHIPSIZE / AREASIZE != playerAreaX + j ||
							(*itr)->y / CHIPSIZE / AREASIZE != playerAreaY + i) {
							(*itr)->PosCorrection();	// もとに戻す
						}
						// ユニットからプレイヤーへの当たり判定
						if ((*itr)->CheckColl(Player::pos_.X, Player::pos_.Y)) {
							(*itr)->PosCorrection();	// 座標修正
						}
					}

				}
			}
			
		}
		/*
		for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
			// プレイヤーからユニットへの当たり判定
			if (itr->second->CheckColl(Player::X, Player::Y)) {
				Player::PosCorrection();
			}
			if (itr->second->Process()) {
				exec_Unit.push_back(itr->second);	// 実行中のユニットに追加
			}
			if(CollisionChack(itr->second->x, itr->second->y)) {
				itr->second->PosCorrection();	// 座標修正
			}
			// 他のユニットとの当たり判定
			for (auto j = unitData.begin(), m = unitData.end(); j != m; ++j) {
				if (j->second->GetAppearFlag() && j->second->GetCollisionFlag() && itr->second != j->second) {
					if (itr->second->CheckColl(j->second->x, j->second->y)){
						itr->second->PosCorrection();	// 座標修正
					}
				}
			}
			// ユニットからプレイヤーへの当たり判定
			if (itr->second->CheckColl(Player::X, Player::Y)) {
				itr->second->PosCorrection();	// 座標修正
			}
		}
		*/
		if (playerPreStage != Player::stage) {
			// マップ移動
			LoadMapData();	// マップデータ再読み込み
			LoadUnitData();	// ユニットデータ再読み込み
		}

		// エンカウント判定
		if (encFlag && encCount == 0 && !encList.empty()) {
			// エンカウントリストからランダムで戦闘開始
			Encounter(encList[GetRand(encList.size() - 1)], chipToBack[PlayerPosition()], LoadSoundMem("sound\\battle.wav"));
		}

		// debug
		if (Input::Get(VK_B) == 1) {
			//Encounter(101);	// 戦闘開始
			encCount = 0;
			//Encounter(110, chipToBack[PlayerPosition()]);
		}
	}
}
void Field::CameraProcess(){
	// カメラ座標修正
	if (isFollow) {
		camera_pos_.X = Player::pos_.X - (CHIPSIZE * 15);
		camera_pos_.Y = Player::pos_.Y - (CHIPSIZE * 8);
	}
}
void Field::MapEffect(int position){
	// 奇数偶数で大きく分ける
	// 偶数
	if (position % 2 == 0){
		// 通行可能
	}
	//奇数
	else{
		// 通行不可
		Player::PosCorrection();	// プレイヤーの座標を直前のものに戻す
	}
}
int Field::PlayerPosition(){
	return mapData[(Player::pos_.Y + (CHIPSIZE / 2)) / CHIPSIZE][(Player::pos_.X + (CHIPSIZE / 2)) / CHIPSIZE];
}
int Field::PlayerPosition(int i){
	return mapData[(Player::pos_.Y + (i*(CHIPSIZE - 1))) / CHIPSIZE][(Player::pos_.X + (i*(CHIPSIZE - 1))) / CHIPSIZE];
}
int Field::PlayerPosition(int i, int j){
	return mapData[(Player::pos_.Y + (i*(CHIPSIZE - 1))) / CHIPSIZE][(Player::pos_.X + (j*(CHIPSIZE - 1))) / CHIPSIZE];
}
// マップとの衝突判定
bool Field::CollisionChack(int x, int y) {
	return {
		// 1ドット内側を判定する
		mapData[(y + 1) / CHIPSIZE    ][(x + 1) / CHIPSIZE    ] % 2 == 1 ||		// 左上
		mapData[(y - 1) / CHIPSIZE + 1][(x + 1) / CHIPSIZE    ] % 2 == 1 ||		// 左下
		mapData[(y + 1) / CHIPSIZE    ][(x - 1) / CHIPSIZE + 1] % 2 == 1 ||		// 右上
		mapData[(y - 1) / CHIPSIZE + 1][(x - 1) / CHIPSIZE + 1] % 2 == 1		// 右下
	};
}

void Field::Draw(){
	// マップチップ
	for (int i = (camera_pos_.Y / CHIPSIZE), n = i + 18;i != n;++i) {
		if (i < 0) {
			// OutOfRange対策
			i = 0;
		}
		for (int j = (camera_pos_.X / CHIPSIZE), m = j + 31;j != m;++j) {
			if (j < 0) {
				// OutOfRange対策
				j = 0;
			}
			// mapDataに記述されていない部分の描画に対応している
			if ((unsigned)i < mapData.size()) {
				if ((unsigned)j < mapData[i].size()) {
					// マップチップの描画
					DrawGraph(j*CHIPSIZE - camera_pos_.X, i*CHIPSIZE - camera_pos_.Y, mapChip[mapData[i][j]]);
				}
			}
		}
	}

	// ユニット
	//for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
	//	itr->second->Draw(cameraX, cameraY);
	//}
	// ユニット
	{
		// プレイヤーの区分け座標
		const int playerAreaX = Player::pos_.X / CHIPSIZE / AREASIZE;
		const int playerAreaY = Player::pos_.Y / CHIPSIZE / AREASIZE;
		// プレイヤーの周囲1エリアのみ処理する
		for (int i = (playerAreaY > 0 ? -1 : 0), n = (playerAreaY < (signed)(unitArea.size() - 1) ? 1 : 0); i <= n; ++i) {
			for (int j = (playerAreaX > 0 ? -1 : 0), m = (playerAreaX < (signed)(unitArea[playerAreaY + i].size() - 1) ? 1 : 0); j <= m; ++j) {
				for (auto itr = unitArea[playerAreaY + i][playerAreaX + j].begin(), end = unitArea[playerAreaY + i][playerAreaX + j].end(); itr != end; ++itr) {
					(*itr)->Draw(camera_pos_.X, camera_pos_.Y);	// 座標修正
				}
			}
		}
	}
	
	// プレイヤー
	Player::Draw(isFollow, camera_pos_.X, camera_pos_.Y);

	// ステージ切り替え時の演出
	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed > 0) {
		DrawBox(0, 0, WINDOW_WIDTH, (WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed, true);				// スライド演出
		DrawBox(0, (WINDOW_HEIGHT / 2) + performanceCount*performanceSpeed, WINDOW_WIDTH, WINDOW_HEIGHT, true);	// スライド演出
	}

	if (Player::flag_shop) {
		shop->Draw();
	}

	// フィールドでの操作説明
	//DrawGraph(WINDOW_WIDTH - 679, WINDOW_HEIGHT - 400, gr_Operation);

	// テキストボックス表示
	if (TextBox::textBoxFlag) {
		TextBox::Draw();
	}

	if (waitBattle) {
		DrawGraph(0,  360 - waitBattleCount * ((1080 / 30) + (360 / 30)), waitBattleGraph[0]);
		DrawGraph(0, 1440 - waitBattleCount * ((1080 / 30) + (360 / 30)), waitBattleGraph[1]);
	}

	Emote::Draw();	// エモート
	
	// debug
	// 区分け線
	/*for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		for (int j = 0, m = (int)unitArea[i].size(); j < m; ++j) {
			DrawBox(j * CHIPSIZE * AREASIZE - cameraX, i * CHIPSIZE * AREASIZE - cameraY, CHIPSIZE * AREASIZE, CHIPSIZE * AREASIZE, false);
		}
	}*/
	
	/*
	DrawString(BLUE, 10, 10, "エンカウントまでの歩数：%d", encCount);
	DrawString(BLUE, 10, 40, "最小エンカウント歩数：%d", encMinCount);
	DrawString(BLUE, 10, 70, "最大エンカウント歩数：%d", encMaxCount);
	DrawString(BLUE, 10, 100, "エンカウントリスト：");
	for (int i = 0, n = encList.size(); i < n; ++i) {
		DrawString(BLUE, 230 + i * 60, 100, "%d,", encList[i]);
	}
	DrawString(BLUE, 10, 10, "実行中のユニット：%d", exec_Unit.size());
	*/

	/*DrawString(BLUE, 10, 10, "実行中のユニット：%d", camera_pos_.X);
	DrawString(BLUE, 10, 30, "実行中のユニット：%d", camera_pos_.Y);*/

	/*for (int i = 0;i < Player::isFinish[Player::stage].size();++i) {
		DrawString(WHITE, 0, i * 32, "%d", (int)Player::isFinish[Player::stage][i]);
	}*/

	/*if (Player::stage == 2) {
		for (int i = 0;i < eventData[2].size();++i) {
			DrawString(BLUE, i*200, 256, eventData[2][i].c_str());
		}
	}
	*/
}

void Field::EventProcess(int id) {
	// イベントの処理

	vector<string> event_data = eventData[id];	// エイリアス
	string event_name = event_data[eventHead];	// エイリアス

	if (event_name == "WARP") {
		// 移動
		Player::pos_.set_pos(boost::lexical_cast<int>(event_data[eventHead + 1]), boost::lexical_cast<int>(event_data[eventHead + 2]));
		eventHead += 3;
	}
	else if (event_name == "MOVE") {
		// X座標
		if (Player::pos_.X < boost::lexical_cast<int>(event_data[eventHead + 1])) {
			Player::pos_.X += 8;
		}
		else if (Player::pos_.X > boost::lexical_cast<int>(event_data[eventHead + 1])) {
			Player::pos_.X -= 8;
		}
		// Y座標
		if (Player::pos_.Y < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			Player::pos_.Y += 8;
		}
		// Y座標
		else if (Player::pos_.Y > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			Player::pos_.Y -= 8;
		}

		if (Player::pos_.X == boost::lexical_cast<int>(event_data[eventHead + 1]) && Player::pos_.Y == boost::lexical_cast<int>(event_data[eventHead + 2])) {
			eventHead += 3;
		}
	}
	else if (event_name == "DIRECTION") {
		// プレイヤーを指定の方向に向ける

		Player::direction = (Direction)std::stoi(event_data[eventHead + 1]);

		eventHead += 2;
	}
	else if (event_name == "SPEAK") {
		// 文章

		vector<string> str;		// 文章をベクターに保存

		++eventHead;			// 読み込みヘッダ

		do {
			// 改行の処理
			str.push_back(event_data[eventHead]);	// 文章をベクターに保存
			++eventHead;								// 読み込みヘッダ
		} while (event_data[eventHead] != "SPEAK_END");

		++eventHead;			// 読み込みヘッダ

		TextBox::AddLine(str);	// 出力
	}
	else if (event_name == "APPEAR_UNIT") {
		// Unit[id]を表示/非表示する

		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->SetAppearFlag((bool)std::stoi(event_data[eventHead + 2]));

		eventHead += 3;
	}
	else if (event_name == "WARP_UNIT") {
		// Unit[id]を指定の座標に即時に移動させる

		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x = boost::lexical_cast<int>(event_data[eventHead + 2]);
		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y = boost::lexical_cast<int>(event_data[eventHead + 3]);

		eventHead += 4;
	}
	else if (event_name == "MOVE_UNIT") {
		// X座標
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x += 8;
		}
		else if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x -= 8;
		}
		// Y座標
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y < boost::lexical_cast<int>(event_data[eventHead + 3])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y += 8;
		}
		else if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y > boost::lexical_cast<int>(event_data[eventHead + 3])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y -= 8;
		}

		// 終了条件
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x == boost::lexical_cast<int>(event_data[eventHead + 2])
			&& unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y == boost::lexical_cast<int>(event_data[eventHead + 3])) {
			eventHead += 4;
		}
	}
	else if (event_name == "WARP_CAM") {
		// 移動
		camera_pos_.set_pos(boost::lexical_cast<int>(event_data[eventHead + 1]), boost::lexical_cast<int>(event_data[eventHead + 2]));
		eventHead += 3;
	}
	else if (event_name == "MOVE_CAM") {
		isFollow = false;

		// X座標
		if (camera_pos_.X < boost::lexical_cast<int>(event_data[eventHead + 1])) {
			camera_pos_.X += 4;
		}
		else if (camera_pos_.X > boost::lexical_cast<int>(event_data[eventHead + 1])) {
			camera_pos_.X -= 4;
		}
		// Y座標
		if (camera_pos_.Y < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			camera_pos_.Y += 4;
		}
		else if (camera_pos_.Y > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			camera_pos_.Y -= 4;
		}

		// 終了条件
		if ((camera_pos_.X == boost::lexical_cast<int>(event_data[eventHead + 1]))
			&& (camera_pos_.Y == boost::lexical_cast<int>(event_data[eventHead + 2]))) {
			eventHead += 3;
		}
	}
	else if (event_name == "WAIT_CAM") {
		isFollow = false;
		if (boost::lexical_cast<int>(event_data[eventHead + 1]) <= wait_cam_time_) {
			eventHead += 2;

			wait_cam_time_ = 0;
		}
		else {
			wait_cam_time_++;
		}
	}
	else if (event_name == "EMOTE") {
		// エモート

		Emote::SetEmote(
			boost::lexical_cast<int>(event_data[eventHead + 1]),
			boost::lexical_cast<int>(event_data[eventHead + 2]),
			boost::lexical_cast<int>(event_data[eventHead + 3])
			);

		eventHead += 4;
	}
	else if (event_name == "JOIN_CHARACTER") {
		// キャラクター名のキャラクターをセーブデータから読み込んで/読み込まずにパーティーに加える

		string name = event_data[eventHead + 1];	// エイリアス

		if (name == "tee") {
			// ティー

			Player::character.push_back(new Tee(name, (bool)(std::stoi(event_data[eventHead + 2]))));
			Player::bag.resize(Player::character.size());	// バッグ調整

			if (!(bool)(std::stoi(event_data[eventHead + 2]))) {
				// 初めてパーティーに加わった
				Player::character.back()->Lv1_Init();			// Lv.1状態への初期化

				int back = Player::character.size() - 1;

				// アイテムを持たせる
				Player::InsertItem(back, 10);	// マジックワンド
				Player::InsertItem(back, 4);	// 布の服
				Player::InsertItem(back, 5);	// お守り
				// それらを装備
				for (int i = 0;i < 3;++i) {
					Player::bag[back][i]->Effect(Player::character[back]);	// 装備
					Player::bag[back][i]->isEquipment = true;
				}
			}

			// メニューボックスに追加
			Player::charaMenu.itemName.push_back(name);
			Player::targetMenu.itemName.push_back(name);
			Player::targetMenu.height += 32;
			Shop::charaMenu.itemName.push_back(name);
			Shop::charaMenu.height += 32;
		}

		eventHead += 3;
	}

	// 終了条件
	else if (event_name == "END") {
		// 一度きりのイベント

		eventHead = 1;	// 消化済み
		Player::isFinish[Player::stage][id] = true;
		now_event_id_ = 0;
		isEvent = false;

		isFollow = true;
	}
	else if (event_name == "CONTINUE") {
		// 何度でも起こるイベント

		eventHead = 1;	// 消化済み
		now_event_id_ = 0;
		isEvent = false;

		isFollow = true;
	}
	else {
		// ここから再帰処理

		bool check_check_check = true;

		if (event_name == "FIRST") {
			// 初めて訪れたとき

			eventHead += 1;
			EventProcess(id);
		}
		else if (event_name == "TALK") {
			// 特定のユニットに話しかけた時

			if (Player::talkFlag.flag) {
				if (Player::talkFlag.pos_.X == unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x
					&& Player::talkFlag.pos_.Y == unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y) {
					eventHead += 2;
					EventProcess(id);
					check_check_check = false;
				}
			}
		}
		else if (event_name == "CATCH_EMOTE") {
			// 特定のエモートが使用されたとき

			if (Player::emote_lock_ > 0) {
				// プレイヤーがエモートしている

				for (int j = 0, m = Emote::GetEmoteSize();j < m;++j) {
					if (std::stoi(event_data[eventHead + 2]) == Emote::GetEmoteId(j)) {
						// 指定のエモートが使用された

						eventHead += 2;
						EventProcess(id);	// イベント進行
						check_check_check = false;
					}
				}
			}
		}
		else if (event_name == "FINISH") {
			// ほかのイベントが終わったかどうか監視する

			bool check_check_check_check_check = true;
			
			for (eventHead = eventHead + 1;event_data[eventHead] != "FINISH_END";eventHead += 2) {
				if (Player::isFinish[std::stoi(event_data[eventHead])][std::stoi(event_data[eventHead + 1])]) {

				}
				else {
					eventHead = 1;							// 超重要初期化
					check_check_check_check_check = false;	// イベントが終わってない
					break;
				}
			}

			if (check_check_check_check_check) {
				// イベント起動

				eventHead += 1;
				EventProcess(id);	// イベント進行
				check_check_check = false;
			}
		}

		if (check_check_check) {
			eventHead = 1;
			isEvent = false;
			isFollow = true;
		}
	}
}