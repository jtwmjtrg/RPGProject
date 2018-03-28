#pragma once
#include "Scene.h"
#include "Field.h"
#include "Battle.h"
#include "SkillTree.h"

class Stage :public Scene
{
private:
	Game*		game;		// フィールドとバトルの親クラスポインタ
	StageMode	preMode;	// 直前のゲームシーンを保存

	bool changeFlag;

public:
	// コンストラクタ＆デストラクタ
	Stage();
	~Stage();

	void Process();	// 処理系
	void Draw();	// 描写系

	void ChangeMode();
};
