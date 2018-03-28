#pragma once
#include <array>
#include <fstream>
#include <sstream>
#include "graphicdata.h"
#include "input.h"
#include "BattleStatus.h"
#include "Equipment.h"
#include "Item.h"
#include "Skill.h"
#include "TermSkill.h"

#include <boost/algorithm/string/split.hpp>		// �X�v���b�g
#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string.hpp>

enum class BuffEffect : __int8 {
	Negate, // �ł�����
	Buff,	// �o�t
	deBuff,	// �f�o�t
};

class Character;
class EquipmentItem;
class Battle;

// �s����
struct tSkillOrder {
	Character* attacker;	// �X�L���̎g�p��
	Skill* skill;	// �g�p�X�L��
	double speed;	// ���x
	int totalUseEnergy;	// �X�L���̔����ɕK�v��MUE�l
};

class SkillOrder {
private:
	double maxSpeed;	// �ő��s���̑��xs

public:
	SkillOrder();
	~SkillOrder();
	
	vector<tSkillOrder> order;	// �I�[�_�[

	void Add(Character* attacker, Skill* skill, int totalUseEnergy);	// �X�L�����X�L���I�[�_�[�ɒǉ�
	void Add_Velocity(Character* attacker, Skill* skill, int totalUseEnergy, bool velocity);	// �ő�/�Œx�X�L��
	void Pop_Back();	// �Ō�����|�b�v����

	Character* GetAttacker();
	Skill* GetSkill();
	tSkillOrder GetOrder();
};

class Character	// ���ۃN���X
{
protected:
	// �y�摜�z
	static array<int, 7> gr_BadStatus;		// ��Ԉُ�

	// �y���z
	static int se_Damage;		// �_���[�Wse

	// �s�σX�e�[�^�X
	string					name;			// ���O

	BattleStatus			battlestatus;	// �퓬�X�e�[�^�X
	array<int,9>			DEF_badStatus;	// ��Ԉُ�ϐ�
	array<int,9>			EBB_badStatus;	// ��Ԉُ�̒~�ϒl
	array<tBadStatus,9>		badStatus;		// ��Ԉُ�̏��
	array<tBuff,22>			buffData;		// �o�t,�f�o�t
	eAlive					aliveState;		// �������
	bool					defenceFlag;	// �h���ԃt���O
	array<array<float,9>,2> efficiency;		// �G�l���M�[�ϊ�����
	array<tCharge, 8> chargeData;			// �`���[�W

	vector<vector<__int8>>	collision;	// �����蔻��

	SkillOrder *skillOrder;		// �X�L���I�[�_�[
	Battle* battle;				// �퓬�N���X

	std::vector<TermSkill*> termSkill;		// ���������X�L��

	// �y����s���z
	Skill* nomalAttack;		// �ʏ�U��
	Skill* confusionSkill;	// �������̍U���p�X�L��

	// �y�X�L���̑ΏۂɂȂ������ɕێ�������́z
	std::array<bool, 6> receiveSkill;	// �X�L���̑ΏۂɂȂ�����true
	unsigned int skillDamage;		// �X�L���Ŏ󂯂��_���[�W
	bool skillDeath;				// �X�L���Ŏ��񂾂�true
	std::array<bool, 10> skillHealType;	// �������񕜂̃^�C�v
	int skillHeal_HP;			// �X�L���ŉ񕜂�����
	int skillHeal_MP;
	std::array<bool, 9> skillBadStatus;	// ��������Ԉُ�̎��
	std::array<bool, 22> skillBuff;		// �o�t
	std::array<BuffEffect, 22> skillBuffType;// �o�t�̌��ʃ^�C�v
	bool skillResurrection;			// �h��
	

	unsigned __int8 damageDelay;	// �_���[�W�G�t�F�N�g�̃J�E���g
	unsigned __int8 deathDelay;		// ���S�G�t�F�N�g�̃J�E���g


	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	Character();

	EquipmentItem* weapon;	// ����
	EquipmentItem* armor;	// �h��
	EquipmentItem* amulet;	// ���̑�

public:
	// �f�X�g���N�^
	virtual ~Character();
	
	static void Init();		// ������
	static void Fina();		// �I������
	void SetPointer(SkillOrder *skillOrder, Battle* battle);	// �|�C���^�Z�b�g
	static void CommonInitBattle();	// �퓬���ʏ���������
	static void CommonFinaBattle();	// �I������

	virtual void SetColision(vector<vector<Character*>>& collField) = 0;	// �����蔻��̍쐬
	virtual void DeleteColision(vector<vector<Character*>>& collField) = 0;	// �����蔻��̍폜

	// �G�t�F�N�g
	int Recovery(double value);		// HP��
	int Recovery_MP(double value);	// MP��
	int FullRecovery();				// HP�S��
	int FullRecovery_MP();			// MP�S��
	int		Damage(double value);	// HP�_���[�W
	int PhysDamage(double value);	// �����_���[�W
	int HeatDamage(double value);	// �M�_���[�W
	int ColdDamage(double value);	// ��C�_���[�W
	int ElecDamage(double value);	// �d�C�_���[�W
	int Resurrection(double value);	// �h��
	int FullResurrection();			// ���S�h��

	void IncreaseMP(int value);	// MP����
	void DecreaseMP(int value);	// MP����

	void GiveBadStatus(eBadStatus type, unsigned int power);	// ��Ԉُ��^����
	void GiveEddDEF_BadStatus(eBadStatus type, int value);	// ��Ԉُ�̒~�ϒl
	void HealBadStatus();	// �S��Ԉُ��
	void HealBadStatus(eBadStatus type);	// ��Ԉُ�񕜁i�P�̂݁j
	void GiveBuff(eBuff type, float rate, int turn);	// �o�t��^����
	void Defence();		// �h���Ԃɂ���
	void SetCharge(eCharge type, float rate, int turn);	// �`���[�W�̃Z�b�g
	virtual void ModeChange(int modeNum) {}	// ���[�h�ύX

	void StateReset();	// ��ԏ�����
	void TurnStartProcess();	// �^�[���J�n����
	void TurnEndProcess();		// �^�[���I�����̏���
	
	// �y�X�L���̑ΏۂɂȂ������ɕێ�������́z
	void ReceiveSkillIni();	// �X�L���Ώۃt���O������
	void SetReceiveSkill(eSkillEffect type);	// �X�L���Ώۃt���O�𗧂Ă�
	bool GetReceiveSkill(eSkillEffect type);	// �X�L���Ώۃt���O�擾
	int GetSkillDamage();			// �X�L���Ŏ󂯂��_���[�W�擾
	bool GetSkillDeath();			// �X�L���Ŏ��񂾂�true
	virtual void GetEffectText(vector<string>& message) = 0;	// �X�L���̃��b�Z�[�W�擾
	
	// �Q�b�^�[
	string GetName();
	int GetMaxHP();
	int GetMaxMP();
	BattleStatus GetBattleStatus();
	int GetMaxUseEnergy();
	virtual int GetStatus(eStatus status);	// �X�e�[�^�X�擾
	Skill* GetConfusionSkill();	// �������̍U���p�X�L���擾
	eAlive GetAlive();	// ������Ԃ�Ԃ�	0:����	1:���S	2:����
	double GetSpeed();	// �s�����x
	tBadStatus GetBadStatus(eBadStatus type);	// ��Ԉُ�
	int GetDEF_BadStatus(eBadStatus type);		// ��Ԉُ�ϐ��̎擾
	double GetEfficiency(eEnergy energy, bool direction);	// �G�l���M�[�ϊ�����
	int GetHit();	// ������
	int GetAvoid();	// ����
	virtual bool GetCharType() = 0;	// �L�����N�^�[�̃^�C�v�擾	�v���C���[�Ftrue	�G�Ffalse
	virtual int GetPlayerOrder() { return 0; }	// �v���C���[�̕��я�
	virtual int GetX() = 0;	// �o�g���t�B�[���h���W
	virtual int GetY() = 0;
	float GetCharge(eCharge type);	// �`���[�W�{��

	// ����
	void EquipmentWeapon(EquipmentItem* weapon);	// ����̑���
	int GetWeaponID();		// �����ID���擾
	string GetWeaponName();	// ����̖��O���擾
	void EquipmentArmor(EquipmentItem* armor);		// �h��̑���
	int GetArmorID();		// �h���ID���擾
	string GetArmorName();	// �h��̖��O���擾
	void			EquipmentAmulet(EquipmentItem* armor);	// ���̑��̑���
	int				GetAmuletID();							// ���̑���ID���擾
	string			GetAmuletName();						// ���̑��̖��O���擾

	void ChangeWeapon();			// ����̕ύX���ɒʏ�U����ς��鏈��

	// debug
	int				GetATK();
	BattleStatus	GetFixed();	// �C�����݂̃X�e�[�^�X
};

class PlayerChara :public Character {
protected:
	// �y�摜�z
	static int gr_PlayerStatus;	// �v���C���[�̃X�e�[�^�X�̘g�i���펞�j
	static int gr_PlayerStatus_Death;	// �v���C���[�̃X�e�[�^�X�̘g�i���S���j

	// �o���l
	unsigned int EXP;	// �������Ă���o���l
	short LV;			// ���݂̃��x��
	unsigned int nextEXP;	// ���̃��x���A�b�v�ɕK�v�Ȍo���l

	unsigned int skillPoint;	// �ێ��X�L���|�C���g

	vector<Skill*> skillList;	// �ێ��X�L��
	
public:
	PlayerChara(string name, bool isContinue);
	virtual ~PlayerChara();

	static void InitBattle();	// �퓬����������
	static void FinaBattle();	// �I������

	void LoadSkillTree();	// �擾�X�L���̃��[�h
	void InitSkillTree();	// �X�L���c���[�̏�����

	virtual void Lv1_Init() {};	// �L�����N�^���Ƃ�Lv.1��Ԃւ̏�����

	void Draw();	// �`��

	int order;	// ���я�

	void SetColision(vector<vector<Character*>>& collField);	// �����蔻��̍쐬
	void DeleteColision(vector<vector<Character*>>& collField);	// �����蔻��̍폜

	virtual void UniqueProcess() = 0;		// �ŗL�����i����Ƃ��j
	vector<Skill*> GetSkillList();			// �ێ��X�L���̎擾
	vector<Skill*> GetSkillList_Battle();	// �퓬�X�L���̎擾
	vector<Skill*> GetSkillList_Field();	// �t�B�[���h�X�L���̎擾
	bool CheckAbleAct();		// �L�����N�^���s���\���Ԃ� �� ��Ԃɉ������s��������
	void GetEffectText(vector<string>& message);	// �X�L���̃��b�Z�[�W�擾
	void AddEXP(int exp);		// �o���l�擾

	int GetX();	// �o�g���t�B�[���h���W
	int GetY();

	void AutoIncMP_Field();	// �t�B�[���h�ł̎���MP��
	bool DecSkillPoint(int point);	// �X�L���|�C���g�̏���

	// �Z�[�u
	void Save();

	// �Q�b�^�[
	int GetEXP();
	int GetLV();
	int GetStatus(eStatus status);		// �X�e�[�^�X�擾
	bool GetCharType() { return true; }	// �L�����N�^�[�̃^�C�v�擾	�v���C���[�Ftrue	�G�Ffalse
	Skill* GetNomalAttack();			// �ʏ�U���擾
	int GetPlayerOrder();				// �v���C���[�̕��я�
	unsigned int GetSkillPoint();		// �X�L���|�C���g
};

class Kata :public PlayerChara {
private: 
	SkillEffect_Buff* curse;	// �͂̍���
public: 
	Kata(string name, bool isContinue);
	~Kata();

	void Lv1_Init();	// Lv.1��Ԃւ̏�����

	void UniqueProcess();		// �ŗL�����i����Ƃ��j
};

class Tee :public PlayerChara {
public:
	Tee(string name, bool isContinue);
	~Tee();

	void Lv1_Init();	// Lv.1��Ԃւ̏�����

	void UniqueProcess();		// �ŗL�����i����Ƃ��j
};

// debug
class Aoi :public PlayerChara {
public:
	Aoi(string name, bool isContinue);
	~Aoi();

	void Lv1_Init();	// Lv.1��Ԃւ̏�����

	void UniqueProcess() {}		// �ŗL�����i����Ƃ��j
};