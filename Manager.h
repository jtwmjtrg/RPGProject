#pragma once
#include "Opening.h"
#include "Title.h"
#include "Stage.h"
#include "GameOver.h"
#include "GameClear.h"
#include "Emote.h"

class Manager	// �V���O���g���p�^�[��
{
private:
	// �V���O���g���p�^�[��
	static Manager* _instance;	// �B���Manager�������|�C���^

	// ����֌W
	Scene* scene;		// �e�V�[���̎���
	GameScene preScene;	// ���O�̃Q�[���V�[����ۑ�

	// �A�N�Z�X�������ꂽ�R���X�g���N�^���f�X�g���N�^
	Manager();
	~Manager();

	// �����ŌĂ΂��֐�
	void ChangeScene();	// �Q�[���V�[���̕ύX���������ꍇ�̃������Ǘ�

public:
	// �V���O���g���p�^�[��
	static Manager* Instance();	// �B���Manager�������|�C���^��Ԃ�
	static void		Destroy();	// �I������

	// �Q�[�����[�v
	void UpDate();	// main�ɌĂ΂��
};