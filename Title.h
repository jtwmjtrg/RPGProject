#pragma once
#include "Player.h"
#include "Scene.h"

#include "Effect3D.h"

class Title :public Scene	// �Q�[���V�[����S���N���X
{
private:
	// �摜
	int backgroundGraph;	// �w�i

	int selectNum;	// �I�����̐�
	int select;		// �I�𒆂̍��ڔԍ�

public:
	// �R���X�g���N�^���f�X�g���N�^
	Title();
	~Title();

	// ���C�����[�v
	void Process();	// ����
	void Draw();	// �`��

	// �}�b�v���j�b�g�̃��[�h�̏�����
	void InitUnitMode();
};