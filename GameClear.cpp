#include "GameClear.h"

GameClear::GameClear() {
	// 画像
	gr_Back = LoadGraph("img\\gameclear\\background.png");	// 背景

	select = 0;
	selectNum = 1;
}
GameClear::~GameClear() {
	// 画像削除
	DeleteGraph(gr_Back);	// 背景
}

void GameClear::Process() {
	if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
		select = (select + (selectNum - 1)) % this->selectNum;
	}
	if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
		select = (select + 1) % selectNum;
	}
	if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
		switch (select) {
		case 0:
			//Player::InitPlayer(true);		// プレイヤーの初期化
			nowScene = GameScene::TITLE;	// ゲームシーンをTitleへ
			break;
		default:
			break;
		}
	}
}

void GameClear::Draw() {
	DrawGraph(0, 0, gr_Back);		// 背景

	DrawString(BLACK, 250, 700 + select * 64, "◆");
	DrawString(BLACK, 300, 700, "タイトルへ戻る");

}