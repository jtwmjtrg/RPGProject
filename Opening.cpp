#include "Opening.h"

Opening::Opening(){
	// 画像
	backgroundGraph = LoadGraph("img\\opening\\background.png");	// 背景

	// 演出
	count = 0;		// フレーム数のカウント
	endCount = 60;	// 演出時間
}
Opening::~Opening(){
	// 画像削除
	DeleteGraph(backgroundGraph);	// 背景
} 

void Opening::Process(){
	// endCountまで演出
	if (count < endCount){
		count++;
	}
	else{
		nowScene = GameScene::TITLE;	// ゲームシーンをTITLEへ
	}
}

void Opening::Draw(){
	DrawGraph(0, 0, backgroundGraph);	// 背景
}