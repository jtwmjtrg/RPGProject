#pragma once
#include "directsound.h"
#include "graphicdata.h"
#include "input.h"

#include "Define.h"
#include "TextBox.h"

using namespace std;

// ゲームシーン列挙
enum class GameScene : __int8
{
	OPENING, TITLE, STAGE, GAMEOVER, GAMECLEAR, END
};

class Scene	// 抽象クラス
{
protected:

	// アクセス制限されたコンストラクタ
	Scene();

public:
	// 制御系
	static GameScene nowScene;	// 現在のゲームシーン
	static bool end;	// 終了フラグ

	// デストラクタ
	virtual ~Scene() {}

	// メインループ
	virtual void Process() = 0;	// 処理内容を記述する
	virtual void Draw() = 0;	// 描画用

	// ゲッター
	static GameScene	GetNowScene();	// 現在のゲームシーンを返す
	static bool			GetEnd();		// アプリケーションの終了指示を返す
};