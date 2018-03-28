#include "Event.h"

// 緊急デバッグ
namespace emergency {
	vector<vector<string> > eventdata;	// イベントデータ
	int eventnum;			// イベントの数
	bool isPerformance;		// 
	int head;				// イベント中の進行度
}
using namespace emergency;

EventManager::EventManager(){
	isPerformance = false;
}
EventManager::~EventManager(){
	
}

void EventManager::Process(){
	for (int i = stoi(eventdata[0][0]), j = 0;j < eventnum;++i, ++j) {
		if (Player::isFinish[i]) {
			// 消化済み
			head = 2;
			isPerformance = false;
		}
		else {
			// 初めて訪れたとき
			if (eventdata[j][1] == "FIRST")
			{
				EventProcess(j);
				isPerformance = true;
				break;
			}
		}
	}
}
void EventManager::Draw(){
	
}

void EventManager::LoadEvent()
{
	// ファイルオープン
	ifstream ifs;
	ifs.open("eventdata\\" + to_string(Player::stage) + ".csv");
	// ファイルがない場合
	if (!ifs){
		return;
	}

	int i = 0;	// ループ回数をカウント
	string str;	// 入力された文字列(行)を保存

	// 1列取得
	while (getline(ifs, str)){
		eventdata.resize(i + 1);	// メモリ確保

		stringstream stream(str);	// strをストリームとして持つ
		string token;				// 入力された文字列(カンマ区切り)を保存

		// カンマ区切りでベクターへ格納する
		while (getline(stream, token, ',')){
			eventdata[i].push_back(token);		// tokenを数値に変換して末尾に挿入
		}

		++i;		// カウントを進める
	}

	// ファイルクローズ
	ifs.close();

	eventnum = eventdata.size();
	head = 2;
}

void EventManager::EventProcess(int id) {
	// 移動
	if (eventdata[id][head] == "WARP") {
		Player::X = stoi(eventdata[id][head + 1]);
		Player::Y = stoi(eventdata[id][head + 2]);
		head += 3;
	}
	else if (eventdata[id][head] == "MOVE") {
		// X座標
		if (Player::X < stoi(eventdata[id][head + 1])) {
			Player::X += 8;
		}
		else if (Player::X > stoi(eventdata[id][head + 1])) {
			Player::X -= 8;
		}
		// Y座標
		if (Player::Y < stoi(eventdata[id][head + 2])) {
			Player::Y += 8;
		}
		else // Y座標
		if (Player::Y > stoi(eventdata[id][head + 2])) {
			Player::Y -= 8;
		}

		if (Player::X == stoi(eventdata[id][head + 1]) && Player::Y == stoi(eventdata[id][head + 2])) {
			head += 3;
		}
	}
	else if (eventdata[id][head] == "TALK") {
		TextBox::AddLine(eventdata[id][head + 1]);
		head += 2;
	}

	if (head == eventdata[id].size()) {
		Player::isFinish[0] = true;
	}
}

bool EventManager::GetEvent() {
	return isPerformance;
}