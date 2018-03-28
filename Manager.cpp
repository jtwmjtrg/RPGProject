#include "Manager.h"

Manager* Manager::Instance(){
	// 一度だけManagerを生成する
	if (_instance == NULL){
		_instance = new Manager();	// 唯一のManager
	}

	return _instance;	// 常に唯一のManagerを返す
}
void Manager::Destroy(){
	delete _instance;	// メモリ解放
}

Manager::Manager(){
	// アプリケーション開始処理
	scene = new Opening();		// オープニングからスタート 
	TextBox::InitTextBox();		// テキストボックスの初期化
	Item::InitItem();			// アイテムのデータを読み込む
	EquipmentItem::InitEquipmentItem();	// 装備のデータを読み込む
	Emote::Create();			// エモートの読み込み
}
Manager::~Manager(){
	// アプリケーション終了処理
	TextBox::Destroy();			// テキストボックスの棄却
	Emote::Destroy();			// エモートの棄却
	if(scene) delete scene;		// メモリ解放
}

void Manager::UpDate(){
	// 直前のゲームシーンを保存
	preScene = Scene::GetNowScene();

	// テキストボックス更新
	TextBox::Process();

	// 各シーンの処理
	scene->Process();
	scene->Draw();

	// ゲームシーンの変更があった場合
	if (preScene != Scene::GetNowScene()){
		ChangeScene();	// ゲームシーン変更処理
	}
}

void Manager::ChangeScene(){
	// メモリ解放
	delete scene;

	// 変更先のコンストラクタでメモリ確保
	switch (Scene::GetNowScene()){
	case GameScene::OPENING:
		scene = new Opening();
		break;

	case GameScene::TITLE:
		scene = new Title();
		break;

	case GameScene::STAGE:
		scene = new Stage();
		break;

	case GameScene::GAMEOVER:
		scene = new GameOver();
		break;

	case GameScene::GAMECLEAR:
		scene = new GameClear();
		break;

	case GameScene::END:
		Scene::end = true;		// ゲーム終了
		scene = nullptr;
		break;

	default:
		//ゲームシーン不正
		break;
	}
}