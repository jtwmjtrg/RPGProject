#pragma once
#include "Game.h"
#include "Enemy.h"

// �o�g���V�[���̒i�K
enum class eBattleStep : __int8 {
	START,	// �퓬�J�n
	COMMAND,// �v���C���[�R�}���h����
	BATTLE,	// �o�g���V�[��
	END,	// �퓬�I��
};
// �R�}���h��ʂ̒i�K
enum class eCommand : __int8 {
	CharaSelect,	// �L�����I��
	ActionSelect,	// �s���I��
	SkillSelect,	// �g�p�X�L���I��
	ItemSelect,		// �g�p�A�C�e���I��
	TargetSelect,	// �X�L�����̑Ώ۔͈͑I��
};

// �O�^�[���ɑI�������J�[�\���̈ʒu���L�����Ă���
struct tCommandPreSelect {
	int action;		// �s���I��
	int skill;		// �g�p�X�L���I��
	int item;		// �g�p�A�C�e���I��
	int targetX;	// �X�L�����̑Ώ۔͈͑I��	X���W	�G�I��p
	int targetY;	//							Y���W
	int selectTargetAlly;	// �����I��p
	bool fullPowerFlag;		// �S�̓X�L���t���O
};

enum class eActionSelect : __int8 {
	Action,		// �U��
	Skill,		// �X�L��
	Item,		// �A�C�e��
	Defence,	// �h��
	Escape,		// ������
};

// �_���[�W�G�t�F�N�g
class DamageEffect {
private:
	static std::array<int, 10> gr_RedNum;	// �����摜�i�ԁj

	int x, y;	// ���W
	int value;	// �\�����鐔�l
	int frame;	// �\������t���[����
	int count;	// �\���J�E���g
	int delay;	// �\���܂ł�delay
	int color;	// �F
public:
	DamageEffect(int x, int y, int value, int frame, int color = WHITE, int delay = 0);
	~DamageEffect();

	static void Init();	// ������
	static void Fina();	// �I������

	bool Process();
	void Draw();
};

// �摜��\������G�t�F�N�g
class GraphicEffect {
protected:
	const int gr;			// �`�悷��摜
	const short x, y;		// ���W
	short frame;	// �\������t���[����
	short count;	// �\���J�E���g
	short delay;	// �\���܂ł�delay
public:
	GraphicEffect(short x, short y, int gr, short frame, short delay = 0);
	~GraphicEffect();

	virtual bool Process();
	virtual void Draw(int cellSize);
};
// �摜��\������G�t�F�N�g�i�����j
class DivGraphicEffect : public GraphicEffect {
private:
	const unsigned __int8 divX;	// ������X
	const unsigned __int8 divY;	// ������Y
	const int divNum;			// ��������

	const unsigned __int8 width;	// ����
	const unsigned __int8 height;	// �c��
public:
	DivGraphicEffect(short x, short y, int gr, unsigned __int8 divX, unsigned __int8 divY, short divNum, unsigned __int8 width, unsigned __int8 height, short delay = 0);
	~DivGraphicEffect();

	virtual bool Process();
	virtual void Draw(int cellSize);
};

/*
// �X�L���I�[�_�[�p
struct tSkillOrder {
	Character* attacker;	// �X�L���̎g�p��
	Skill* skill;	// �X�L���f�[�^
};
*/

// debug
// �A�C�e�����g�p����ۂ̏��
typedef struct _UsedItem {
	bool		isUse;		// �������s����
	int			player;		// ���я��ŉ��Ԗڂ�
	Position2D	item;		// �o�b�O�̂ǂ̈ʒu�̃A�C�e����
}UsedItem;

class Battle :public Game{
private:
	eEndStatus battleEndType;	// �퓬�I���̏��
	eBattleStep step;			// �o�g���V�[���̒i�K
	unsigned int turnNum;		// �^�[����
	bool EscapeFlag;			// �����\�t���O
	bool loseEventFlag;			// �����C�x�t���O

	// �y�摜�z
	int gr_Back;			// �w�i
	int gr_Operation;		// �퓬��ʂł̑������
	int gr_SkillSelect;		// �X�L���I���E�B���h�E
	int gr_scroll;			// �X�N���[���o�[

	vector<PlayerChara*> player;	// �v���C���[�̃f�[�^
	vector<Enemy*> enemy;	// �G�̃f�[�^

	vector<vector<Character*>> enemyField;	// �G�t�B�[���h�i�}�X���G�̃|�C���^�������Ă���j
	vector<vector<Character*>> playerField;	// �v���C���[�t�B�[���h�i�}�X���v���C���[�̃|�C���^�������Ă���j
	int enemyFieldWidth;	// �G�t�B�[���h�̉���
	int enemyFieldHeight;	// �G�t�B�[���h�̏c��
	int cellSize;	// 1�}�X�̃T�C�Y
	int enemyFieldX;
	int enemyFieldY;

	int playerFieldWidth;	// �v���C���[�t�B�[���h�̉���
	int playerFieldHeight;	// �v���C���[�t�B�[���h�̏c��

	int fieldMP;	// �t�B�[���h��MP
	int fieldMaxMP;	// �t�B�[���h�̍ő�MP
	
	vector<Character*> targetList;		// �U���Ώۃ��X�g
	vector<Character*> targetSave;		// �U���Ώۃ��X�g�̕ۑ��i�����U���Ƃ��Ɏg���j

	vector<Character*>	order;			// �G�����̍s�����i�x�����ɓ����j
	SkillOrder			*skillOrder;	// �X�L���̔�����

	vector<int>			playerHate;		// �v���C���[�̃w�C�g
	int					maxHate;		// �w�C�g�l���

	vector<vector<bool>>	enemyField_Save;	// �X�L���Ώەۑ��p�i�G�j
	vector<vector<bool>>	playerField_Save;	// �X�L���Ώەۑ��p�i�����j
	vector<Character*>		target_Save;		// �X�L���Ώەۑ��p�i�L�����N�^�[���Ɓj

	// ����X�L��
	Skill_Defence*	skill_Defence;	// �h��
	Skill_None*		skill_None;		// �u�Ȃɂ����Ȃ��v�X�L��
	Skill_Escape*	skill_Escape;	// ����

	// �y�R�}���h��ʁz
	eCommand					commandStep;		// �R�}���h��ʂ̒i�K
	vector<tCommandPreSelect>	commandPreSelect;	// �O�^�[���ɑI�������J�[�\���̈ʒu���L�����Ă���
	int							selectChara;
	int							selectAction;		// �s���I��
	int							ActionNum;			// �s���I��

	int							selectSkill;		// �X�L���I��
	int							skillNum;			// �X�L���I��
	int							selectSkillCursor;	// �X�L���I����ʂ̃J�[�\���̈ʒu
	int							selectSkillArea;
	vector<Skill*>				selectSkillList;	// �X�L�����X�g
	//vector<Skill*> skillOrder_C;	// �R�}���h��ʒ��ɒǉ������X�L��
	//vector<Character*>	attackerOrder_C;	// �X�L���̎g�p��
	vector<tSkillOrder>			commandOrder;		// �R�}���h�ł̃X�L���I�[�_�[

	Skill*	selectSkillData;	// �I�������X�L��
	int		selectTargetX;		// �I�𒆂̍��W�i�G�I��p�j
	int		selectTargetY;
	int		selectTargetAlly;	// �����I��p



	// �험�i
	int				exp;		// �퓬�œ����o���l�̍��v
	int				money;		// �퓬�œ������̍��v

	// debug
	deque<DamageEffect>	damageEffect;	// �_���[�W�G�t�F�N�g�i�_���[�W�ʂ̕\���Ƃ��j
	deque<GraphicEffect*>	graphicEffect;	// �摜��\������G�t�F�N�g
	Skill*					testSkill;		// �X�L��
	int						gr_SkillHit1;	// �X�L���̓����蔻��
	int						gr_SkillHit2;	// �X�L���̓����蔻��
	int						gr_SkillHit3;	// �X�L���̓����蔻��
	int						gr_SkillHit4;	// �X�L���̓����蔻��
	int						gr_target;		// �G�̓����蔻��
	int						gr_NA;
	bool					skillStep;		// true:�X�L���G�t�F�N�g�\��	false:�X�L������

	// suzuki_debug
	MenuVar					selectItem;		// �A�C�e���I�����̗v�f
	vector<UsedItem>		usedItem;		// ���̃^�[���Ɏg�p����\��̃A�C�e���̃|�C���^��ۑ�

public:
	Battle();
	~Battle();

	void Process();
	void Draw();

	void Process_START();	// �퓬�J�n
	//void Draw_START();
	void Process_END();		// �퓬�I��
	//void Draw_END();
	void Process_COMMAND();	// �v���C���[�R�}���h���
	void Draw_COMMAND();
	void Process_BATTLE();	// �o�g���V�[��
	//void Draw_BATTLE();

	void StepChange_START();	// �J�n��ʂւ̈ڍs����
	void StepChange_COMMAND();	// �R�}���h��ʂւ̈ڍs����
	void StepChange_BATTLE();	// �o�g���V�[���ւ̈ڍs����
	//void StepChange_END();		// �I����ʂւ̈ڍs����
	//void StepChange_TARGET();	// 

	int EnergyConvDmage(Character* attacker, Character* target, tEnergyConv energy, float hit, float damageRate, int totalUseEnergy);	// �G�l���M�[���Ƃ̃_���[�W�␳�v�Z
	bool CheckAbleSkill(tSkillOrder order);				// �X�L���������\�����ׂ�
	bool SkillExec(Character* attacker, Skill* skill);	// �X�L������
	bool SkillExec(tSkillOrder order);	// �X�L������
	void SkillEffectExec(int FieldX, int FieldY, Character* attacker, Character* target, SkillEffect* effect, int totalUseEnergy, int effectNum);	// �ΏۂɃX�L���̌��ʂ�^����
	void DeleteDeadEnemy();	// ���񂾓G�̃f�[�^�̍폜

	int GetFieldMP();					// �t�B�[���hMP�̃Q�b�^�[
	void IncreaseFieldMP(int value);	// �t�B�[���hMP����
	void DecreaseFieldMP(int value);	// �t�B�[���hMP����
	
	void EndText();	// �G���J�E���g���̕\��

	bool CheckEnemysDead();		// �G�S�Ŕ���
	bool CheckPlayersDead();	// �v���C���[�S�Ŕ���

	void LoadEncounter();	// �G���J�E���g�f�[�^�̓ǂݍ���

	// debug
	void TestIni();	// �e�X�g�p������
	
};