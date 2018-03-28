#pragma once
#include <vector>
#include <string>
#include "Define.h"
#include "graphicdata.h"
#include "TextBox.h"

// エモート
class Emote {
private:
	static bool isCreate;					// 生成されたかどうかのフラグ

	static int emote_num_;					// エモートの総数

	static std::vector<int> time_;			// graph_handleと添字対応。time_[x]がgraph_handle_[x]のエモート時間を示す
	static std::vector<int> graph_handle_;	// 画像ハンドル

	typedef struct _EmoteInfo {
		// エモートの情報

		int id_;			// id
		Position2D pos_;	// 座標
		int count_;			// 経過時間
	}EmoteInfo;

	static std::vector<EmoteInfo> playing_emote_;	// 進行中のエモート

public:
	Emote();
	~Emote();

	// 初期化＆棄却
	static void Create();
	static void Destroy();

	// 更新処理＆描画
	static void Process();
	static void Draw();

	// エモートセット
	static void SetEmote(int id, Position2D pos);
	static void SetEmote(int id, int X, int Y);

	static int GetEmoteSize();
	static int GetEmoteId(int i);
};
