#include "Emote.h"

bool Emote::isCreate;									// 生成されたかどうか
int Emote::emote_num_;									// エモートの総数
std::vector<int> Emote::time_;							// 経過時間
std::vector<int> Emote::graph_handle_;					// 画像ハンドル
std::vector<Emote::EmoteInfo> Emote::playing_emote_;	// 進行中のエモート

Emote::Emote() {

}
Emote::~Emote() {

}

void Emote::Create() {
	// 初期化
	
	emote_num_ = 10;	// 今は適当

	for (int i = 0;i < emote_num_;++i) {
		// 画像読み込み
		
		string str = "img\\emote\\" + std::to_string(i) + ".png";	// アドレス生成
		char* cstr = new char[str.size() + 1];						// メモリ確保
		strcpy_s(cstr, str.size() + 1, str.c_str());				// コピー
		graph_handle_.push_back(LoadGraph(cstr));					// 画像ロード
		delete cstr;

		// 時間

		time_.push_back(30);
	}

	isCreate = true;	// 生成した
}
void Emote::Destroy() {
	// 棄却

	for (int i = 0;i < emote_num_;++i) {
		// 画像破棄
		DeleteGraph(graph_handle_[i]);
	}

	// メモリ解放
	graph_handle_.clear();
	graph_handle_.shrink_to_fit();
	time_.clear();
	time_.shrink_to_fit();
	playing_emote_.clear();
	playing_emote_.shrink_to_fit();
}

void Emote::Process() {
	// 更新処理

	if (!isCreate) {
		return;
	}

	for (int i = 0;i < playing_emote_.size();++i) {
		// スタックにあるエモートを更新する

		if (playing_emote_[i].count_ + 1 >= time_[playing_emote_[i].id_]) {
			// 終了

			playing_emote_.erase(playing_emote_.begin() + i);	// 終了したエモートをスタックから削除する
			i--;
		}
		else {
			// 更新

			playing_emote_[i].count_++;	// カウントを進める
		}
	}
}
void Emote::Draw() {
	// 描画処理

	if (!isCreate) {
		return;
	}

	for (int i = 0, n = playing_emote_.size();i < n;++i) {
		// スタックにあるエモートを描画更新する

		DrawGraph(playing_emote_[i].pos_.X, playing_emote_[i].pos_.Y, graph_handle_[playing_emote_[i].id_]);
	}
}

void Emote::SetEmote(int id, Position2D pos) {
	// エモートの予約

	if (!isCreate) {
		return;
	}

	EmoteInfo emote;
	emote.id_ = id;
	emote.pos_ = pos;
	emote.count_ = 0;
	playing_emote_.push_back(emote);
}
void Emote::SetEmote(int id, int X, int Y) {
	// エモートの予約

	if (!isCreate) {
		return;
	}

	EmoteInfo emote;
	emote.id_ = id;
	emote.pos_.X = X;
	emote.pos_.Y = Y;
	emote.count_ = 0;
	playing_emote_.push_back(emote);
}

int Emote::GetEmoteSize() {
	return playing_emote_.size();
}
int Emote::GetEmoteId(int i) {
	if (i >= playing_emote_.size()) {
		return -1;
	}

	return playing_emote_[i].id_;
}