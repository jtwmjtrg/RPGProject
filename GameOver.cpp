#include "GameOver.h"

GameOver::GameOver() {
	// �摜
	backgroundGraph = LoadGraph("img\\gameover\\background.png");	// �w�i

	select = 0;
	selectNum = 1;
}
GameOver::~GameOver() {
	// �摜�폜
	DeleteGraph(backgroundGraph);	// �w�i
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
			//Player::InitPlayer(false);		// �v���C���[�̏�����
			nowScene = GameScene::TITLE;	// �Q�[���V�[����Title��
			break;
		default:
			break;
		}
	}
}

void GameOver::Draw() {
	DrawGraph(0, 0, backgroundGraph);		// �w�i

	DrawString(WHITE, 250, 700 + select * 64, "��");
	DrawString(WHITE, 300, 700, "�^�C�g���֖߂�");

}