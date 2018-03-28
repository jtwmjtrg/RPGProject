#include "Title.h"

Title::Title(){
	// 画像
	backgroundGraph = LoadGraph("img\\title\\background.png");	// 背景
	
	select = 0;
	selectNum = 3;
}
Title::~Title(){
	// 画像削除
	DeleteGraph(backgroundGraph);	// 背景
}

void Title::Process(){
	// カーソルを上に
	if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
		select = (select + (selectNum - 1)) % this->selectNum;
	}
	// カーソルを下に
	if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
		select = (select + 1) % selectNum;
	}
	// 決定
	if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
		switch (select) {
		case 0:// 【続きから】
			Player::InitPlayer(true);		// プレイヤーの初期化
			nowScene = GameScene::STAGE;	// ゲームシーンをFIELDへ
			break;
		case 1:// 【はじめから】
			Player::InitPlayer(false);		// プレイヤーの初期化
			InitUnitMode();					// マップユニットのモードの初期化
			nowScene = GameScene::STAGE;	// ゲームシーンをFIELDへ
			break;
		case 2:// 【やめる】
			nowScene = GameScene::END;		// ゲーム終了
			break;
		default:
			break;
		}
	}
}

void Title::Draw(){
	DrawGraph(0, 0, backgroundGraph);		// 背景
		
	DrawString(WHITE, 250, 700 + select * 64, "◆");
	DrawString(WHITE, 300, 700, "続きから");
	DrawString(WHITE, 300, 764, "はじめから");
	DrawString(WHITE, 300, 828, "ゲームをやめる");
}

// マップユニットのモードの初期化
void Title::InitUnitMode() {
	HANDLE hFind;	// ハンドル	
	WIN32_FIND_DATA fileData;// ファイル情報の構造体(これにフォルダ名が入る)
	std::vector<std::string> folders;	// フォルダリストfolder

	//検索するディレクトリの指定
	std::string search_name = "unitdata\\*";

	hFind = FindFirstFile(search_name.c_str(), &fileData);	// 検索ハンドルの取得

	// 検索できなければエラーを返す
	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
	}

	do {
		// ファイル名が"."か".."でなければ格納
		folders.push_back(fileData.cFileName);	// ファイル名格納
		if (folders.back() == "." || folders.back() == "..") {
			folders.pop_back();	// 削除
		}
	} while (FindNextFile(hFind, &fileData));	// 次のファイルを検索

	FindClose(hFind);	// 検索終了

	std::map<int, int> unitMode;	// ファイルのデータ（ID,初期モード）

	// ユニットのモードのセーブデータを全て初期化する
	for (int i = 0, n = folders.size(); i < n; ++i) {
		// ユニットモード初期化ファイルを開く
		std::ifstream lFile("unitdata\\" + folders[i] + "\\InitMode.csv");
		if (lFile.fail()) {
			// 読み込み失敗
		}
		else {
			// 読み込み成功

			// 初期化データの読み込み
			unitMode.clear();
			std::string line;
			std::string token;
			int id = 0;	// ID
			while (getline(lFile, line)) {	// 一行ずつ読み込み
				std::istringstream stream(line);
				getline(stream, token, ',');
				id = boost::lexical_cast<int>(token);
				getline(stream, token, ',');
				unitMode[id] = boost::lexical_cast<int>(token);
			}
			
			// ユニットモードのセーブファイルを上書き
			ofstream ofs("unitdata\\" + folders[i] + "\\SaveMode.csv");
			for (auto itr = unitMode.begin(), end = unitMode.end(); itr != end; ++itr) {
				ofs << itr->first << ',' << itr->second << endl;		// ID, モード
			}
			ofs.close();
		}
		// ファイルを閉じる 
		lFile.close();
	}

}