#pragma once
#include "Opening.h"
#include "Title.h"
#include "Stage.h"
#include "GameOver.h"
#include "GameClear.h"
#include "Emote.h"

class Manager	// シングルトンパターン
{
private:
	// シングルトンパターン
	static Manager* _instance;	// 唯一のManagerを示すポインタ

	// 制御関係
	Scene* scene;		// 各シーンの実体
	GameScene preScene;	// 直前のゲームシーンを保存

	// アクセス制限されたコンストラクタ＆デストラクタ
	Manager();
	~Manager();

	// 条件で呼ばれる関数
	void ChangeScene();	// ゲームシーンの変更があった場合のメモリ管理

public:
	// シングルトンパターン
	static Manager* Instance();	// 唯一のManagerを示すポインタを返す
	static void		Destroy();	// 終了処理

	// ゲームループ
	void UpDate();	// mainに呼ばれる
};