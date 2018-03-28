#pragma once
#include "Game.h"
#include "Shop.h"

// �X�e�[�W���̖��O�t��
typedef enum eStageName
{
	WORLD,OZGAUTSS,TEEG,VISTE 
}StageName;

// �錾���̊֌W
class EventManager;
class MapUnit;
class Game;

class Field :public Game{
private:
	// �摜
	static map<int, int> mapChip;	// �}�b�v�`�b�v
	bool isFollow;					// �J�����̒Ǐ]
	Position2D camera_pos_;			// �J�������W
	int performanceCount;			// ���o�̂��߂̎��Ԃ��J�E���g����
	int performanceSpeed;			// ���o�̃X�s�[�h
	int gr_Operation;				// �t�B�[���h�ł̑������
	vector<int> waitBattleGraph;	// �퓬�V�[�����O

	// ��

	// �y�G���J�E���g�����p�z
	static bool			encFlag;		// �G���J�E���g�t���O
	static vector<int>	encList;		// �G���J�E���g���X�g
	int encCount;		// �G���J�E���g�܂ł̎c�����
	int encMinCount;	// �ŏ��G���J�E���g����
	int encMaxCount;	// �ő�G���J�E���g����

	Shop* shop;	// �V���b�v�̓��e�̕ۑ���������|�C���^

	int	 waitBattleCount;	// �퓬�ҋ@����

	// debug
	int wait_cam_time_;	// �C�x���g�p�̃J�E���^�[

public:
	// �R���X�g���N�^���f�X�g���N�^
	Field();
	~Field();

	void Process();	// ����
	void Draw();	// �`��

	void LoadMapData();			// �}�b�v�f�[�^��ǂݍ���

	void CameraProcess();					// �J�����ړ��ɂ��Ă̏���
	void MapEffect(int player_position);	// �}�b�v�f�[�^�̌��ʂ��L�q���A���s����֐�
	int PlayerPosition();					// �v���C���[�̍��W���瓥��ł���}�b�v�`�b�v�̐��l��Ԃ�
	int PlayerPosition(int i);				// ����E����2�p
	int PlayerPosition(int i, int j);		// 4�p
	bool CollisionChack(int x, int y);		// �}�b�v�Ƃ̏Փ˔���
	void EventProcess(int id);				// �C�x���g����
};