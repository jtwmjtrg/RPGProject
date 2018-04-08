#include "Game.h"

StageMode				Game::mode = StageMode::FIELD;
std::map<int, MapUnit*> Game::unitData;			// 各ユニットのポインタ
vector<MapUnit*>		Game::exec_Unit;		// 現在行動実行中のユニットのポインタ
vector<vector<string> > Game::eventData;		// イベントデータ
int                     Game::eventNum;			// イベントの数
int                     Game::eventHead;		// どこまでイベントを読んだか
bool                    Game::isEvent;			// イベント中かどうか
int						Game::now_event_id_;	// 現在処理中のイベントのid
tEncounter				Game::encounter;		// エンカウントデータ
MapInfo					Game::info;				// マップの情報
std::map<int, int>		Game::chipToBack;		// マップチップごとにバトル画面の背景を指定
bool					Game::waitBattle;		// 戦闘待機状態フラグ
int						Game::fieldBGM;			// フィールドBGM

std::vector<std::vector< std::list<MapUnit*> > >	Game::unitArea;	// ユニットの区画分け

Game::Game()
{
	
}
Game::~Game() {
}

// マップチップとバトル画面の背景の連想配列を作成
void Game::LaodChipToBack() {
	ifstream ifs("Encounterdata\\chipToBack.csv", ios::in);
	if (!ifs) {
		return;
	}
	string str;
	string token;

	int chipNum = 0;
	int backNum = 0;

	while (getline(ifs, str)) {	// 一行ずつ読み込み
		// 読み込んだ行をコピー
		std::istringstream stream(str);
		// 1【マップチップ】
		getline(stream, token, ',');
		chipNum = boost::lexical_cast<int>(token);
		// 2【背景】
		getline(stream, token, ',');
		backNum = boost::lexical_cast<int>(token);
		chipToBack[chipNum] = backNum;
	}

	ifs.close();	// ファイルクローズ
}

// ユニットの配置データを読み込む
void Game::LoadUnitData() {
	//SaveUnitMode(playerPreStage);	// 直前にいたマップのユニットのモードのセーブ
	if(!unitData.empty()) Player::TmpSave_UnitMode(playerPreStage, unitData);
	//Player::Save();					// プレイデータのオートセーブ
	// メモリ解放
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; itr++) {
		itr->second->EndProcess();			// 終了処理
		DeleteGraph(itr->second->GetImg());	// 画像削除
		delete itr->second;					// メモリ解放
	}
	unitData.clear();
	exec_Unit.clear();
	exec_Unit.shrink_to_fit();

	ifstream ifs("unitdata\\" + to_string(Player::stage) + "\\PutUnit.csv", ios::in);
	if (!ifs) {
		// 読み込み失敗
		return;
	}
	else {
		// 読み込み成功
		string line;
		string token;

		while (getline(ifs, line)) {
			std::istringstream stream(line);
			getline(stream, token, ',');
			unitData[boost::lexical_cast<int>(token)] = new MapUnit(line);
		}
	}	

	ifs.close();	// ファイルクローズ

	// ユニットのモードデータのロード
	Player::SetUnitMode_TmpSave(unitData);

	SetUnitArea();	// ユニットの区画分け
}
// ユニットのモードデータのロード
/*
void Game::LoadUnitMode() {
	
	ifstream ifs("unitdata\\" + to_string(Player::stage) + "\\SaveMode.csv", ios::in);
	
	if (ifs.fail()) {	// ファイル読み込み
		// 読み込み失敗
	}
	else {
		string line;
		string token;

		int id = 0;			// ID
		auto itr = unitData.begin();// ユニットのイテレータ
		auto end = unitData.end();	// ユニットのendイテレータ

		while (getline(ifs, line)) {
			// 1【id】
			std::istringstream stream(line);
			getline(stream, token, ',');
			id = boost::lexical_cast<int>(token);
			itr = unitData.find(id);
			// IDが無ければスキップ
			if (itr != end) {
				// 2【モード番号】
				getline(stream, token, ',');
				itr->second->SetMode(boost::lexical_cast<int>(token));
			}
		}
	}
	ifs.close();
	
}
*/
// ユニットのモードデータのセーブ
/*
void Game::SaveUnitMode(int stage) {
	
	// ファイルオープン
	ofstream ofs("unitdata\\" + to_string(stage) + "\\SaveMode.csv");
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		ofs << itr->first << ',' << itr->second->GetMode() << endl;		// ID, モード
	}
	ofs.close();
	
}
*/
// ユニットの区画分け
void Game::SetUnitArea() {
	for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		unitArea[i].clear();
		unitArea[i].shrink_to_fit();
	}
	unitArea.clear();
	unitArea.shrink_to_fit();
	

	// 区分けする
	unitArea.resize(mapData.size() / AREASIZE + 1);
	for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		unitArea[i].resize(mapData[0].size() / AREASIZE + 1);
	}

	int tmpAreaX = 0;
	int tmpAreaY = 0;
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		tmpAreaX = itr->second->x / CHIPSIZE / AREASIZE;
		tmpAreaY = itr->second->y / CHIPSIZE / AREASIZE;
		unitArea[tmpAreaY][tmpAreaX].push_back(itr->second);		// 区画に入れる
		itr->second->SetArea(tmpAreaX, tmpAreaY);
	}

}

void Game::InfomationUpdate() {
	info.id				= Player::stage;
	info.isEncount		= true;
	info.magicAverage	= 1000;
}

StageMode Game::GetStageMode()
{
	return mode;
}

// 通常戦闘開始
void Game::Encounter(int encounterID, int back, int battleBGM) {
	encounter.id = encounterID;			// エンカウントID
	encounter.back = back;				// 背景ID
	encounter.escapeFlag = true;		// 逃走可能フラグ
	encounter.loseEventFlag = false;	// 負けイベフラグ
	encounter.battleFlag = NULL;		// 戦闘中フラグ（受け渡し用）
	encounter.battleEndType = NULL;		// 戦闘終了の状態（受け渡し用）
	encounter.battleBGM = battleBGM;	// 戦闘BGM

	// 音
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);

	PlaySoundMem(battleBGM, true);	// 再生

	waitBattle = true;
}
// イベント戦闘開始
void Game::Encounter(int encounterID, int back, int battleBGM, bool escapeFlag, bool loseEventFlag, bool* battleFlag, eEndStatus* battleEndType) {
	encounter.id = encounterID;					// エンカウントID
	encounter.back = back;						// 背景ID
	encounter.escapeFlag = escapeFlag;			// 逃走可能フラグ
	encounter.loseEventFlag = loseEventFlag;	// 負けイベフラグ
	encounter.battleFlag = battleFlag;			// 戦闘中フラグ（受け渡し用）
	encounter.battleEndType = battleEndType;	// 戦闘終了の状態（受け渡し用）
	encounter.battleBGM = battleBGM;			// 戦闘BGM

	// 音
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);

	PlaySoundMem(battleBGM, true);	// 再生

	waitBattle = true;
}

void Game::EncounterReset() {
	encounter.id = -1;					// エンカウントID
	encounter.back = 0;					// 背景ID
	encounter.escapeFlag = true;		// 逃走可能フラグ
	encounter.loseEventFlag = false;	// 負けイベフラグ
	encounter.battleFlag = NULL;		// 戦闘中フラグ（受け渡し用）
	encounter.battleEndType = NULL;		// 戦闘終了の状態（受け渡し用）
}

void Game::TmpSave_UnitMode(){
	Player::TmpSave_UnitMode(Player::stage, unitData);
}
