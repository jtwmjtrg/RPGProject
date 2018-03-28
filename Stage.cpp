#include "Stage.h"

Stage::Stage() {
	game = new Field();
	game->LoadUnitData();	// ���j�b�g�z�u�f�[�^�ǂݍ���
	game->LaodChipToBack();	// �}�b�v�`�b�v�ƃo�g����ʂ̔w�i�̘A�z�z����쐬
	Character::Init();		// �L�����N�^�N���X�̏�����
	preMode = StageMode::FIELD;

	changeFlag = false;
}
Stage::~Stage() {
	if (game) {
		delete game;
	}

	Player::DeletePlayer();	// �v���C���[�̏I������
	Character::Fina();		// �L�����N�^�N���X�̏I������
}

void Stage::Process() {

	preMode = Game::GetStageMode();

	game->Process();
	game->Draw();

	if (preMode != Game::GetStageMode()) {
		ChangeMode();
	}
}
void Stage::Draw() {
		// �g��Ȃ�
}

void Stage::ChangeMode() {
	TextBox::Reset();
	delete game;

	switch (Game::GetStageMode()) {
	case StageMode::FIELD:
		game = new Field();
		break;

	case StageMode::BATTLE:
		game = new Battle();
		break;

	case StageMode::SKILLTREE:
		game = new SkillTree();
		break;
	case StageMode::GAMEOVER:
		game	 = nullptr;
		nowScene = GameScene::GAMEOVER;
		break;

	default:		
		break;
	}
}