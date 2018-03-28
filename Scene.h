#pragma once
#include "directsound.h"
#include "graphicdata.h"
#include "input.h"

#include "Define.h"
#include "TextBox.h"

using namespace std;

// �Q�[���V�[����
enum class GameScene : __int8
{
	OPENING, TITLE, STAGE, GAMEOVER, GAMECLEAR, END
};

class Scene	// ���ۃN���X
{
protected:

	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	Scene();

public:
	// ����n
	static GameScene nowScene;	// ���݂̃Q�[���V�[��
	static bool end;	// �I���t���O

	// �f�X�g���N�^
	virtual ~Scene() {}

	// ���C�����[�v
	virtual void Process() = 0;	// �������e���L�q����
	virtual void Draw() = 0;	// �`��p

	// �Q�b�^�[
	static GameScene	GetNowScene();	// ���݂̃Q�[���V�[����Ԃ�
	static bool			GetEnd();		// �A�v���P�[�V�����̏I���w����Ԃ�
};