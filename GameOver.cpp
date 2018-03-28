#include "GameOver.h"

GameOver::GameOver() {
	// 画像
	backgroundGraph = LoadGraph("img\\gameover\\background.png");	// 背景

	select = 0;
	selectNum = 1;
}
GameOver::~GameOver() {
	// 画像削除
	DeleteGraph(backgroundGraph);	// 背景
}

void GameOver::Process() {
	if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
		select = (select + (selectNum - 1)) % this->selectNum;
	}
	if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
		select = (select + 1) % selectNum;
	}
	if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
		switch (select) {
		case 0:
			//Player::InitPlayer(false);		// プレイヤーの初期化
			nowScene = GameScene::TITLE;	// ゲームシーンをTitleへ
			break;
		default:
			break;
		}
	}
}

void GameOver::Draw() {
	DrawGraph(0, 0, backgroundGraph);		// 背景

	DrawString(WHITE, 250, 700 + select * 64, "◆");
	DrawString(WHITE, 300, 700, "タイトルへ戻る");

}