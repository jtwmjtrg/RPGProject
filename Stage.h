#pragma once
#include "Scene.h"
#include "Field.h"
#include "Battle.h"
#include "SkillTree.h"

class Stage :public Scene
{
private:
	Game*		game;		// �t�B�[���h�ƃo�g���̐e�N���X�|�C���^
	StageMode	preMode;	// ���O�̃Q�[���V�[����ۑ�

	bool changeFlag;

public:
	// �R���X�g���N�^���f�X�g���N�^
	Stage();
	~Stage();

	void Process();	// �����n
	void Draw();	// �`�ʌn

	void ChangeMode();
};
