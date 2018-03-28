#pragma once
#include<memory>
#include <string>
#include <vector>
#include <array>
#define _USE_MATH_DEFINES	// M_PI���g����悤��
#include <math.h>		// �O�p��
#include <boost/lexical_cast.hpp>
#include "sounddata.h"

// �t�@�C���ǂݍ���
#include <fstream>
#include <sstream>

#include "TextBox.h"
#include "Equipment.h"

using namespace std;

enum class eAlive : __int8 {
	alive,		// ����
	dead,		// ���S
	extinction,	// ����
};

// �X�L���̎��
enum class eSkillType : __int8{
	Nomal,		// ���ʂ̃X�L��
	Confusion,	// �������̍U���p�X�L��
	None,		//�u�Ȃɂ����Ȃ��v�X�L��
	Defence,	// �h��
	Escape,		// ������
};

// �X�L���G�t�F�N�g�̎��
enum class eSkillEffect : __int8 {
	Attack,		// �U��
	Heal,		// ��
	BadStatus,	// ��Ԉُ�
	Buff,		// �o�t�A�f�o�t
	AnotherSkill,	// ���X�L������
	Resurrection,	// �h��
	Hate,			// �w�C�g����
	Charge,			// �`���[�W
	Mode,			// ���[�h�ύX
	MoveMP,			// ���f�̈ړ�
};

// �G�l���M�[�̎��
enum eEnergy {
	None = -1, // �ϊ��Ȃ�
	Heat,	// �M
	Elec,	// �d�C
	Shine,	// ��
	Sound,	// ��
	Motion,	// �^��
	Gravity,// �d��
	Life,	// ����
	Static,	// �Î~
	Nucleus,// �j
};

// �G�l���M�[�ϊ��̍ۂɎg�p���閂�f�A�G�l���M�[�̑Ώ�
enum class eConvType : __int8 {
	Self,	// �����ۗ̕L���f�A�G�l���M�[
	Field,	// �t�B�[���h�̖��f�A�G�l���M�[
	Target,	// ����
	None,	// �G�l���M�[�ϊ��ȊO�̌��ہi���d�튯�Ƃ��j
};

enum class eSkillTarget : __int8 {
	Self,	// ����
	Enemy,	// �G�S��
	Ally,	// �����S��
	Overall,// �G�����S��
	Select,	// �I��͈�
	Previous,// �����p��
};

// �G�l���M�[�ϊ�
struct tEnergyConv {
	eEnergy type;	// �G�l���M�[�̎��
	eConvType convType;	// �ϊ����閂�f�̑Ώ�
	bool direction;	// �ϊ������i1:+�@0:-�j
	double rate;	// �ϊ������i�K�v��MUE�l��n%�A�ϊ��Ώۂ��ΏۂȂ��̏ꍇ�͕ϊ��ʁj
	__int8 correction;	// �␳
};

// �X�e�[�^�X
enum class eStatus : __int8 {
	HP,
	maxHP,
	MP,
	maxMP,
	STR,
	DEX,
	AGI,
	LUC,
	DEF_PHYS,
	DEF_HEAT,
	DEF_COLD,
	DEF_ELEC,
	MUE
};

// �񕜃^�C�v
enum class eHealType : __int8 {
	HP,		// HP
	MP,		// MP
	BadStatus,	// �S��Ԉُ�
	Poison,		// ��
	Paralysis,	// ���
	Sleep,		// ����
	Confusion,	// ����
	Blind,		// �È�
	Weakness,	// ����
	Debuff	// �S�f�o�t
};

// �o�t,�f�o�t�p
enum class eBuff :__int8 {
	One_any = -1,	// �ǂꂩ���
	// �y�X�e�[�^�X�z
	HP,		// �̗�
	maxHP,	// �ő�̗�
	MP,		// �}�W�b�N�p���[
	maxMP,	// �ő�MP
	STR,
	DEX,
	AGI,
	LUC,
	DEF_PHYS,
	DEF_HEAT,
	DEF_COLD,
	DEF_ELEC,
	MUE,
	// �y��Ԉُ�z
	Poison,		// ��
	Paralysis,	// ���
	Sleep,		// ����
	confusion,	// ����
	Blind,		// �È�
	Weakness,	// ����
	Stun,		// �X�^��
	Death,		// ����
	Extinction, // ����
};

// �o�t�f�[�^
struct tBuff {
	float rate;	// �{��
	int trun;	// �����^�[����
};
// ��Ԉُ�
enum class eBadStatus : __int8 {
	One_any = -1,	// �ǂꂩ���
	Poison,		// ��
	Paralysis,	// ���
	Sleep,		// ����
	Confusion,	// ����
	Blind,		// �È�
	Weakness,	// ����
	Stun,		// �X�^��
	Death,		// ����
	Extinction, // ����
};
struct tBadStatus {
	bool get;	// ��Ԉُ�Ȃ�true
	int turn;	// ��Ԉُ�ɂȂ��Ă���o�߂����^�[��
	unsigned int power;	// ��Ԉُ�̋���
};

enum class eCharge : __int8 {
	Damage,		// �_���[�W�{��
	Defence,	// �h��{��
	MUE,		// MUE
	Avoid,		// ���
	Hit,		// ������
	Range,		// �͈͊g��
	Speed,		// �s�����x
	Continuing,	// �A���s��
};
class SkillEffect;
class Skill;
class Character;
class PlayerChara;

// �\����
// �y�U���z
struct tSAttack {
	float damageRate;	// �_���[�W�{��
	float hit;			// �����␳
	unsigned __int8 times_min;	// �ŏ��s����
	unsigned __int8 times_max;	// �ő�s����
};
// �y�񕜁z
struct tHeal {
	eHealType type;	// �񕜃^�C�v
	float rate;	// �{��
};
// �y��Ԉُ�z
struct tBadStatus_Skill {
	eBadStatus type;	// ���
	int hit;			// ������
	unsigned int power;	// �З�
};
// �y�o�t�z
struct tBuff_Skill {
	eBuff type;		// �o�t�̎��
	float hit;	// ����
	bool dir;	// true:�o�t	false:�f�o�t
	unsigned __int8 trun;	// �����^�[����
};
// �y���̃X�L�������z
struct tAnotherSkill {
	string skillID;	// �X�L��ID
	Skill* skill;	// ��������X�L���f�[�^
	__int8 invocation;	// ������
	bool fastFlag;	// ���U�t���O
};
// �h��
struct tResurrection {
	__int8 success;	// ������
	float rate;		// �{��
};
// �w�C�g����
struct tHate {
	int value;	// �����l
	float rate;	// �����{��
};
// �`���[�W
struct tCharge {
	eCharge type;	// ���
	float rate;		// �{��
	int turn;		// �����^�[����
};
// ���[�h�ύX
struct tModeChange {
	int modeNum;	// ���[�h�ԍ�
};

// �y���ʂ̕ϊ��ʂ𓾂�֐��z
double GetConvValue(const tEnergyConv energy, int totalUseEnergy);

// �y�X�L���̌��ʃX�[�p�[�N���X�z
class SkillEffect {
protected:
	int totalUseEnergy;
public:
	eSkillEffect	type;
	tEnergyConv		energyConv;					// �G�l���M�[�ϊ�
	eSkillTarget	targetType;					// �U���Ώ�
	std::vector<std::vector<__int8>> collision;	// �����蔻��
	
	SkillEffect() {};
	SkillEffect(int totalUseEnergy) : totalUseEnergy(totalUseEnergy){}
	virtual ~SkillEffect() {}

	virtual void Execution_Field(Character* attacker, Character* target) {}	// ���ʔ����i�t�B�[���h�p�j

	virtual const tSAttack*				GetAttack()			{ return NULL; }	// �U��
	virtual const tHeal*				GetHeal()			{ return NULL; }	// ��
	virtual const tBadStatus_Skill*		GetBadStatus()		{ return NULL; }	// ��Ԉُ�
	virtual const tBuff_Skill*			GetBuff()			{ return NULL; }	// �o�t-�f�o�t
	virtual const tAnotherSkill*		GetAnotherSkill()	{ return NULL; }	// ���̃X�L������
	virtual const tResurrection*		GetResurrection()	{ return NULL; }	// �h��
	virtual const tHate*				GetHate()			{ return NULL; }	// �w�C�g����
	virtual const tCharge*				GetCharge()			{ return NULL; }	// �`���[�W
	virtual const tModeChange*			GetMode()			{ return NULL; }	// ���[�h�ύX
};

//////////////////////////////////////
////////////// �y�U���z////////////////
//////////////////////////////////////
class SkillEffect_Attack : public SkillEffect {
private:
	tSAttack attackData;
public:
	SkillEffect_Attack();	// �������̍U���p������
	SkillEffect_Attack(string line, int totalUseEnergy);
	~SkillEffect_Attack();

	//void Execution_Field(Character* attacker, Character* target);	// ���ʔ����i�t�B�[���h�p�j

	// debug
	void TestIni();
	const tSAttack* GetAttack();
};


//////////////////////////////////////
////////////// �y�񕜁z////////////////
//////////////////////////////////////
class SkillEffect_Heal : public SkillEffect {
private:
	tHeal healData;
public:
	SkillEffect_Heal();
	SkillEffect_Heal(string line, int totalUseEnergy);
	~SkillEffect_Heal();

	void Execution_Field(Character* attacker, Character* target);	// ���ʔ����i�t�B�[���h�p�j

	const tHeal* GetHeal();
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// �y��Ԉُ�z//////////////
//////////////////////////////////////
class SkillEffect_BadStatus : public SkillEffect {
private:
	tBadStatus_Skill badStatusData;
public:
	SkillEffect_BadStatus();
	SkillEffect_BadStatus(string line, int totalUseEnergy);
	SkillEffect_BadStatus(tBadStatus_Skill badStatusData);
	~SkillEffect_BadStatus();

	const tBadStatus_Skill* GetBadStatus();
};

//////////////////////////////////////
/////////// �y�o�t-�f�o�t�z////////////
//////////////////////////////////////
class SkillEffect_Buff : public SkillEffect {
private:
	tBuff_Skill buffData;
public:
	SkillEffect_Buff();
	SkillEffect_Buff(string line, int totalUseEnergy);
	SkillEffect_Buff(tBuff_Skill buffData, tEnergyConv energyConv);
	~SkillEffect_Buff();

	const tBuff_Skill* GetBuff();
};

//////////////////////////////////////
////////// �y���̃X�L�������z//////////
//////////////////////////////////////
class SkillEffect_AnotherSkill : public SkillEffect {
private:
	tAnotherSkill anotherSkillData;
public:
	SkillEffect_AnotherSkill();
	SkillEffect_AnotherSkill(string line, int totalUseEnergy);
	~SkillEffect_AnotherSkill();

	const tAnotherSkill* GetAnotherSkill();
	// debug
	void TestIni();
};

//////////////////////////////////////
/////////////// �y�h���z///////////////
//////////////////////////////////////
class SkillEffect_Resurrection : public SkillEffect {
private:
	tResurrection resurrectionData;
public:
	SkillEffect_Resurrection();
	SkillEffect_Resurrection(string line, int totalUseEnergy);
	~SkillEffect_Resurrection();

	const tResurrection* GetResurrection();
	void Execution_Field(Character* attacker, Character* target);	// ���ʔ����i�t�B�[���h�p�j
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// �y�w�C�g�����z////////////
//////////////////////////////////////
class SkillEffect_Hate : public SkillEffect {
private:
	tHate hateData;
public:
	SkillEffect_Hate();
	SkillEffect_Hate(string line, int totalUseEnergy);
	~SkillEffect_Hate();

	const tHate* GetHate();
	// debug
	void TestIni();
};

//////////////////////////////////////
///////////// �y�`���[�W�z/////////////
//////////////////////////////////////
class SkillEffect_Charge : public SkillEffect {
private:
	tCharge chargeData;
public:
	SkillEffect_Charge();
	SkillEffect_Charge(string line, int totalUseEnergy);
	~SkillEffect_Charge();

	const tCharge* GetCharge();
	// debug
	void TestIni();
};

//////////////////////////////////////
//////////// �y���[�h�ύX�z////////////
//////////////////////////////////////
class SkillEffect_Mode : public SkillEffect {
private:
	tModeChange modeData;
public:
	SkillEffect_Mode();
	SkillEffect_Mode(string line, int totalUseEnergy);
	~SkillEffect_Mode();

	const tModeChange* GetMode();
	// debug
	void TestIni();
};

// �X�L���̉摜�G�t�F�N�g
struct tSkillGrEffect {
	bool flag;		// �G�t�F�N�g�̗L��
	int gr;
	unsigned __int8 divX;		// ������X
	unsigned __int8 divY;		// �@�@�@Y
	unsigned short divNum;		// ��������
};

// �y�X�L���̃��C���N���X�z
class Skill {
private:

public:
	Skill();
	Skill(int id);
	Skill(string path);
	~Skill();

	void LoadData(string path);		// �X�L���f�[�^�ǂݍ���

	eSkillType type;	// �X�L���̎��
	unsigned int id;	// �X�L����ID
	string name;		// �X�L����
	float speed;		// ���x�␳
	bool selectFlag;	// �U���͈͑I���t���O
	bool selectTarget;	// �U���͈͑I��Ώ�		F:�G	T:����
	unsigned __int8 width;			// �U���͈� ����
	unsigned __int8 height;			//          �c��
	std::array< unsigned int, 3> useMP;	// �g�pMP
	int totalUseEnergy;	// �K�v��MUE�l
	bool battleFlag;	// �퓬���g�p�t���O
	bool fieldFlag;		// �t�B�[���h�g�p�t���O
	eWeaponType weaponRestriction;		// ���퐧��
	
	std::vector<std::string> explanatoryText;	// ������
	std::vector<std::string> utilizationText;	// �X�L���g�p�̃e�L�X�g

	tSkillGrEffect grEffect;	// �摜�G�t�F�N�g
	int so_SE;		// SE

	void PlayerSE();	// SE���Ȃ炷

	__int8 x, y;	// ���W�i�U�����Ɍ��肷��j
	vector<SkillEffect*> effect;		// �X�L������
	vector<vector<__int8>> collision;	// �����蔻��

	void Execution_Field(Character* attacker, std::vector<PlayerChara*> playerList, Character* target = nullptr);	// �X�L�������i�t�B�[���h�p�j
	void Release();		// �I������

	// debug
	void TestIni();
};

// �y�������̍s���p�X�L���z
class Skill_Confusion : public Skill{
private:

public:
	Skill_Confusion(int arWidth, int arHeight, vector<vector<__int8>> arCollision);
	~Skill_Confusion();
};

// �y�Ȃɂ����Ȃ��z
class Skill_None : public Skill {
private:

public:
	Skill_None();
	~Skill_None();
};

// �y�h��z
class Skill_Defence : public Skill {
private:

public:
	Skill_Defence();
	~Skill_Defence();
};

// �y�����z
class Skill_Escape : public Skill {
private:

public:
	Skill_Escape();
	~Skill_Escape();
};


