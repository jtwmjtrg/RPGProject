#pragma once
#include "sounddata.h"
#include <vector>
#include <map>
#include "MapObject.h"
#include <list>
#include "Emote.h"

#define AREASIZE 15

using namespace std;

typedef struct _MapInfo {
	int  id;			// �Ǘ��ԍ�
	bool isEncount;		// �G���o��̂�
	int  magicAverage;	// ���ϓI�ȃ}�\��
}MapInfo;

// �t�B�[���h��ʂ��o�g����ʂ�
typedef enum class eStageMode {
	FIELD, BATTLE, SKILLTREE, GAMEOVER, NONE
}StageMode;

// �G���J�E���g�f�[�^
struct tEncounter {
	int id;		// �G���J�E���gID
	int back;	// �w�iID
	bool escapeFlag;	// �����\�t���O
	bool loseEventFlag;	// �����C�x�t���O
	bool* battleFlag;	// �퓬���t���O
	eEndStatus* battleEndType;	// �퓬�I���̏��
	int	battleBGM;	// �퓬BGM
};

class Game {
protected:
	static StageMode				mode;		// �t�B�[���h��ʂ��o�g����ʂ�
	static vector<vector<int> >		mapData;	// �}�b�v�f�[�^
	static std::map<int, MapUnit*>	unitData;	// �e���j�b�g�̃|�C���^
	static vector<MapUnit*>			exec_Unit;	// ���ݍs�����s���̃��j�b�g�̃|�C���^
	static vector<vector<string> >	eventData;	// �C�x���g�f�[�^
	static int						eventNum;	// �C�x���g�̐�
	static int						eventHead;	// �ǂ��܂ŃC�x���g��ǂ񂾂�
	static bool						isEvent;	// �C�x���g�����ǂ���
	static int						now_event_id_;	// �������̃C�x���g��id
	static tEncounter				encounter;	// �G���J�E���g�f�[�^
	static MapInfo					info;		// ���̃X�e�[�W�̏��
	static std::map<int,int>		chipToBack;	// �}�b�v�`�b�v���ƂɃo�g����ʂ̔w�i���w��
	static bool						waitBattle;	// �퓬�ҋ@��ԃt���O
	static int 						fieldBGM;	// �t�B�[���hBGM

	static std::vector<std::vector< std::list<MapUnit*> > >	unitArea;	// ���j�b�g�̋�敪��

	void InfomationUpdate();				// �}�b�v�̏��̍X�V

	int playerPreStage;	// ���O�̃X�e�[�W

	Game();	// �A�N�Z�X�������ꂽ�R���X�g���N�^

public:
	// �p���O��̒�`
	virtual ~Game();
	virtual void Process() = 0;
	virtual void Draw() = 0;

	void LoadUnitData();			// ���j�b�g�̔z�u�f�[�^��ǂݍ���
	//void LoadUnitMode();			// ���j�b�g�̃��[�h�f�[�^�̃��[�h
	//void SaveUnitMode(int stage);	// ���j�b�g�̃��[�h�f�[�^�̃Z�[�u
	void SetUnitArea();				// ���j�b�g�̋�敪��
	void LaodChipToBack();			// �}�b�v�`�b�v�ƃo�g����ʂ̔w�i�̘A�z�z����쐬
	static void Encounter(int encounterID, int back, int battleBGM);	// �ʏ�퓬�J�n
	static void Encounter(int encounterID, int back, int battleBGM, bool escapeFlag, bool loseEventFlag, bool* battleFlag, eEndStatus* battleEndType);	// �C�x���g�퓬�J�n
	static void EncounterReset();	// �G���J�E���^�[�̃��Z�b�g
	static void TmpSave_UnitMode();	// ���j�b�g�̃��[�h�̈ꎞ�Z�[�u

	static StageMode GetStageMode();		// ���݂̃��[�h��Ԃ�
};