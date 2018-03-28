#pragma once
#include "Player.h"
#include "Scene.h"

class GameOver :public Scene	// ゲームシーンを担うクラス
{
private:
	// 画像
	int backgroundGraph;	// 背景

	int selectNum;	// 選択肢の数
	int select;		// 選択中の項目番号

public:
	// コンストラクタ＆デストラクタ
	GameOver();
	~GameOver();

	// メインループ
	void Process();	// 処理
	void Draw();	// 描写
};