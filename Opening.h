#pragma once
#include "Scene.h"

class Opening :public Scene	// �Q�[���V�[����S���N���X
{
private:
	// �摜
	int backgroundGraph;	// �w�i

	// ���o
	int count;		// �t���[���J�E���g
	int endCount;	// ���o����

public:
	// �R���X�g���N�^���f�X�g���N�^
	Opening();
	~Opening();

	// ���C�����[�v
	void Process();	// ����
	void Draw();	// �`��
};