#pragma once
#include "Player.h"
#include "Scene.h"

class GameClear :public Scene	// ゲームシーンを担うクラス
{
private:
	// 画像
	int gr_Back;	// 背景

	int selectNum;	// 選択肢の数
	int select;		// 選択中の項目番号

public:
	// コンストラクタ＆デストラクタ
	GameClear();
	~GameClear();

	// メインループ
	void Process();	// 処理
	void Draw();	// 描写
};