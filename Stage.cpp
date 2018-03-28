#include "Stage.h"

Stage::Stage() {
	game = new Field();
	game->LoadUnitData();	// ユニット配置データ読み込み
	game->LaodChipToBack();	// マップチップとバトル画面の背景の連想配列を作成
	Character::Init();		// キャラクタクラスの初期化
	preMode = StageMode::FIELD;

	changeFlag = false;
}
Stage::~Stage() {
	if (game) {
		delete game;
	}

	Player::DeletePlayer();	// プレイヤーの終了処理
	Character::Fina();		// キャラクタクラスの終了処理
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
		// 使わない
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