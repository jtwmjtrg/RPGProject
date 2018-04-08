#include "Scene.h"

class Scene;

#pragma once

#include "Scene.h"

#include "Item.h"
#include "Character.h"
#include "Emote.h"

// �u�b��������v
typedef struct tTalkFlag {
	bool flag;	// �b���t���O�i1�t���[���̂ݗL���j
	Position2D pos_;	// �b�����������W�i�}�b�v���W�j
}TalkFlag;

// �v���C���[�̌����̖��O�t��
typedef enum ePlayerDirection{
	UP, DOWN, LEFT, RIGHT
}Direction;

// �v���C���[�̑I�����
typedef enum ePlayerMode {
	FREE, MENU
}Mode;

// ���j���[�̍���
typedef struct _MenuItem {
	int	   id;		// ���̍��ڂ�ID
	string name;	// ���̍��ڂ̖��O
}MenuItem;
// ���j���[
typedef struct _Menu {
	Position2D		 pos;	// ���j���[�̔��̍��W
	Position2D		 info;	// �I�����Ă��郁�j���[�̐���
	Position2D		 icon;	// �I���A�C�R��
	vector<MenuItem> item;	// ����
}Menu;
// ���j���[���Ƃ̕ۑ�����
typedef struct _MenuVar {
	bool		flag;
	Position2D	pos;
}MenuVar;

typedef struct _ItemBox{
	// �I�����ڂ������̕`��֌W���܂Ƃ߂��\����
	bool			isDraw;		// �`�悷�邩�ǂ���
	bool			isSelect;	// ������\���ǂ���
	Position2D		pos;		// ���̍��W
	int				width;		// ���̕�
	int				height;		// ���̍���
	Position2D		icon;		// �A�C�R���̍��W
	vector<string>	itemName;	// ���ڂ̖��O

	void Initialize() {
		// ������
		isDraw = false;
		isSelect = false;
		icon.Y = 0;
		icon.X = 0;
	}
	void Initialize(int x, int y, int w, int h) {
		// ������
		isDraw		= false;
		isSelect	= false;
		pos.X		= x;
		pos.Y		= y;
		width		= w;
		height		= h;
		icon.Y		= 0;
		icon.X		= 0;
	}

	void Process() {
		// ����

		if (!itemName.empty()) {
			// ���ڂ������

			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
				// �A�C�R������Ɉړ�
				if (icon.Y == 0) {
					// ��ԏ�Ȃ�
					icon.Y = itemName.size() - 1;	// ��ԉ��Ɉړ�
				}
				else {
					// ����ȊO�Ȃ�
					--icon.Y;	// ����
				}
			}
			else if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
				// �A�C�R�������Ɉړ�
				if (icon.Y == itemName.size() - 1) {
					// ��ԉ��Ȃ�
					icon.Y = 0;	// ��ԉ��Ɉړ�
				}
				else {
					// ����ȊO�Ȃ�
					++icon.Y;	// �����
				}
			}

			if ((unsigned)icon.Y > itemName.size() - 1) {
				// �͈͊O��߂�
				icon.Y = 0;
			}
		}
	}

	void Draw() {
		// �`��
		DrawBox(pos.X, pos.Y, width, height, true);	// ��
		for (int i = 0, n = itemName.size(); i < n; ++i) {
			// ���ڂ̐��������[�v
			DrawString(WHITE, pos.X + 16 + 32, pos.Y + 32 + (i * 32), itemName[i].c_str());	// ���ږ���`��
		}
		DrawString(WHITE, pos.X + 16, pos.Y + 32 + (icon.Y * 32), "�E");	// �A�C�R��
	}
}ItemBox;


// ���j���[�̍���
typedef enum ePlayerMenu {
	START, ITEM, SKILL, SKILLTREE, REARRANGE, STATUS, SAVE, BACK_TITLE, EMOTE, BACK
}PlayerMenu;

class Item;
class MapUnit;

class Player	// �ÓI�N���X
{
private:
	// �摜
	static int animCount;	// �A�j���[�V�����̔���
	static int animSpeed;	// �A�j���[�V�����̑��x�B�������ق�������

	// ����n
	static Position2D pre_pos_;		// ���O�̍��W
	static Mode mode;				// ���j���[��ʂ��J���Ă��邩�ǂ���
	static int	swapCharacterY;		// �Ȃ�ׂ���

	// �X�e�[�^�X
	static int speed;	// �ړ��X�s�[�h���Q�̙p��ɂ��邱��
	static int money;	// ������
	
	// �ꎞ�Z�[�u�f�[�^
	static map<int, map<int, int>> tmpSave_unitMode;	// ���j�b�g�̃��[�h	<�}�b�vID <���j�b�gID , ���[�h>>

	// ���C�����[�v
	static void MenuProcess();	// ���j���[��ʂ̏���

	// �������ŌĂ΂��֐�
	static void UseItem(Item* item);	// �A�C�e�g�p���ɌĂ΂��

	static int		menuY;

	static int cover_handle_;	// �������ȍ�
	
	static void Process_Start();
	static void Process_Item();
	static void Process_Skill();
	static void Process_SkillTree();
	static void Process_Rearrange();
	static void Process_Status();
	static void Process_Save();
	static void Process_Back_Title();	// �^�C�g���ɖ߂�
	static void Process_Emote();		// �G���[�g
	static void Process_Back();

	static void Draw_Start();
	static void Draw_Item();
	static void Draw_Skill();
	static void Draw_SkillTree();
	static void Draw_Rearrange();
	static void Draw_Status();
	static void Draw_Save();
	static void Draw_Back_Title();
	static void Draw_Emote();			// �G���[�g
	static void Draw_Back();

public:
	// ���j���[�{�b�N�X����
	static ItemBox	mainMenu;	// ���C�����j���[
	static ItemBox	charaMenu;	// �L�����N�^�[���j���[
	static ItemBox	itemMenu;	// �A�C�e�����j���[
	static ItemBox	doMenu;		// �c�����j���[
	static ItemBox	targetMenu;	// �^�[�Q�b�g���j���[
	static ItemBox	decideMenu;	// �m�F���j���[

	static int playerGraph;	// �{�̂̉摜
	
	// �����`�F�b�N
	static bool born;

	// �Z�[�u�t���O�i�Z�[�u�����t���[���ɂ̂�true�ɂȂ�j
	static bool saveFlag;

	// �������֐�
	static void InitPlayer(bool isContinue);	// ������
	static void DeletePlayer();					// �폜

	// ����n
	static int chapter;			// �X�g�[���[�i�s�x
	static bool animation;		// �A�j���[�V���������ǂ���
	static bool flag_shop;		// �����������ǂ���

	// �X�e�[�^�X
	static int stage;				// ������X�e�[�W
	static Position2D pos_;			// ���W
	static Direction direction;		// ����
	static TalkFlag talkFlag;		// �b�����������W

	// �p�[�e�B�[
	static vector<PlayerChara*> character;	// �p�[�e�B�[�����o
	static vector<vector<Item*> > bag;		// �A�C�e���o�b�O

	// debug
	static int emote_lock_;		// �G���[�g�̘A���֎~

	// ���C�����[�v
	static bool Process();	// ����		���W���ړ�������true��Ԃ��i�G���J�E���g�����p�j
	static void Draw(bool isFollow, int cameraX, int cameraY);		// �`��

	// �������ŌĂяo�����֐�
	static void Save();			// �Z�[�u�f�[�^�̍쐬
	static void PosCorrection();	// �v���C���[�̍��W�C��
	static bool InsertItem(int item_id);			// �A�C�e�����o�b�O�ɒǉ�����B�A�C�e���̊i�[��͐擪����󂫂��������悻���ɒǉ�����
	static bool InsertItem(int order, int item_id);	// �A�C�e�����o�b�O�ɒǉ�����

	static void AddMoney(int value);	// ������ɓ����
	static void PayMoney(int value);	// ���𕥂�
	static int GetMoney();				// ���̃Q�b�^�[

	// �ꎞ�Z�[�u�f�[�^
	static void LoadTmpSave_UnitMode(string line);								// ���j�b�g�̃��[�h�̃��[�h
	static void TmpSave_UnitMode(int saveStage, const std::map<int, MapUnit*> unitData);		// ���j�b�g�̃��[�h�̈ꎞ�Z�[�u
	static bool SetUnitMode_TmpSave(std::map<int, MapUnit*>& unitData);	// ���j�b�g�̃��[�h�̃Z�b�g

	static vector<vector<bool> > isFinish;
};