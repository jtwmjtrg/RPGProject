#pragma once
#include "Player.h"
#include <vector>

class EventManager	// 独立したFieldに使われるクラス
{
public:
	// コンストラクタ＆デストラクタ
	EventManager();
	~EventManager();

	// メインループ
	void Process();	// イベント処理
	void Draw();	// 描写

	// コンストラクタで呼ばれる処理
	void LoadEvent();	// イベントの入力

	void EventProcess(int id);	// debug

	bool GetEvent();
};