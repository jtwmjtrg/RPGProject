#pragma once
#include "Skill.h"
#include "input.h"

class Enemy;

enum class eModeTerm : __int8 {
	None,		// �Ƃ��ɂȂ�
	Hp,			// �g�o����
	Turn,		// �^�[��������
	EnemyNum,	// �G�̐�
	BadStatus,	// ��Ԉُ�
	Buff,		// �o�t
};

// ���[�h�̈ڍs�����X�[�p�[�N���X
class ModeTerm{
protected:
	vector<Skill*> skill;		// �ۗL�X�L��
public:
	eModeTerm type;		// �ڍs�����̎��

	ModeTerm();
	virtual ~ModeTerm() {};

	virtual bool Check() = 0;	// ���[�h�̈ڍs��������

	void AddSkill(Skill* skillData);	// ���̃��[�h�̃X�L���ǉ�
	Skill* GetSkill();	// �����X�L���̎擾

	void Release();		// �I������

};


//////////////////////////////////////
////////////// �y�Ȃ��z////////////////
//////////////////////////////////////
class ModeTerm_None : public ModeTerm {
private:
	const unsigned int& nowMode;	// ���݂̃��[�h
	const unsigned int& modeNum;	// ���[�h�ԍ�
public:
	ModeTerm_None(const unsigned int& nowMode, const unsigned int& modeNum);
	~ModeTerm_None();

	bool Check();	// ���[�h�̈ڍs��������
};

//////////////////////////////////////
//////////// �y�g�o����z//////////////
//////////////////////////////////////
class ModeTerm_HP : public ModeTerm {
private:
	const int& hp;
	const int& max_hp;

	float rate;	// HP�in������)

public:
	ModeTerm_HP(string line, const int& hp, const int&  max_hp);
	~ModeTerm_HP();

	bool Check();	// ���[�h�̈ڍs��������
};

//////////////////////////////////////
////////// �y�^�[��������z////////////
//////////////////////////////////////
class ModeTerm_Turn : public ModeTerm {
private:
	const unsigned int& turnNum;	// ���݂̃^�[����
	unsigned int baseTurn;	// ��^�[��
	__int8 ope;		// ���Z�q

public:
	ModeTerm_Turn(string line, const unsigned int& turnNum);
	~ModeTerm_Turn();

	bool Check();	// ���[�h�̈ڍs��������
};

//////////////////////////////////////
///////////// �y�G�̐��z///////////////
//////////////////////////////////////
class ModeTerm_EnemyNum : public ModeTerm {
private:
	const std::vector<Enemy*>& enemy;	// �G�f�[�^
	unsigned int enemyNum;	// �G�̐�
	eAlive AliveType;	// �Ώ�
	__int8 ope;		// ���Z�q

public:
	ModeTerm_EnemyNum(string line, const std::vector<Enemy*>& enemy);
	~ModeTerm_EnemyNum();

	bool Check();	// ���[�h�̈ڍs��������
};

//////////////////////////////////////
//////////// �y��Ԉُ�z//////////////
//////////////////////////////////////
class ModeTerm_BadStatus : public ModeTerm {
private:
	const std::array<tBadStatus, 9>& badStatus;		// ��Ԉُ�̏��
	eBadStatus  bs_Type;	// ����Ώ�

public:
	ModeTerm_BadStatus(string line, const std::array<tBadStatus, 9>& badStatus);
	~ModeTerm_BadStatus();

	bool Check();	// ���[�h�̈ڍs��������
};