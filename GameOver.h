#pragma once
#include "Player.h"
#include "Scene.h"

class GameOver :public Scene	// �Q�[���V�[����S���N���X
{
private:
	// �摜
	int backgroundGraph;	// �w�i

	int selectNum;	// �I�����̐�
	int select;		// �I�𒆂̍��ڔԍ�

public:
	// �R���X�g���N�^���f�X�g���N�^
	GameOver();
	~GameOver();

	// ���C�����[�v
	void Process();	// ����
	void Draw();	// �`��
};