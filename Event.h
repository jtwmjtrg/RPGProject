#pragma once
#include "Player.h"
#include <vector>

class EventManager	// �Ɨ�����Field�Ɏg����N���X
{
public:
	// �R���X�g���N�^���f�X�g���N�^
	EventManager();
	~EventManager();

	// ���C�����[�v
	void Process();	// �C�x���g����
	void Draw();	// �`��

	// �R���X�g���N�^�ŌĂ΂�鏈��
	void LoadEvent();	// �C�x���g�̓���

	void EventProcess(int id);	// debug

	bool GetEvent();
};