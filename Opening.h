#pragma once
#include "Scene.h"

class Opening :public Scene	// ゲームシーンを担うクラス
{
private:
	// 画像
	int backgroundGraph;	// 背景

	// 演出
	int count;		// フレームカウント
	int endCount;	// 演出時間

public:
	// コンストラクタ＆デストラクタ
	Opening();
	~Opening();

	// メインループ
	void Process();	// 処理
	void Draw();	// 描写
};