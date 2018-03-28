#pragma once
#include "Skill.h"
#include "input.h"

enum class eTermSkill : __int8 {
	Always,		// ���^�[������
	Hp,			// �g�o����
	Turn,		// �^�[��������
	EnemyNum,	// �G�̐�
	BadStatus,	// ��Ԉُ�
	Buff,		// �o�t
};

class SkillOrder;
class Character;

// ���������X�L���X�[�p�[�N���X
class TermSkill {
protected:
	vector<Skill*> skill;		// �ۗL�X�L��
	bool velocity;	// true:�^�[���̍ŏ��ɔ���	false:�^�[���̍Ō�
public:
	eTermSkill type;	// ��������

	TermSkill();
	virtual ~TermSkill() {};

	virtual void Check(Character* character, SkillOrder *skillOrder) = 0;	// ���[�h�̈ڍs��������

	void AddSkill(Skill* skillData);	// �X�L���ǉ�
	void SkillExecution(Character* attacker, SkillOrder *skillOrder);	// �X�L������

	void Release();		// �I������
};

//////////////////////////////////////
////////// �y���^�[�������z////////////
//////////////////////////////////////
class TermSkill_Always : public TermSkill {
public:
	TermSkill_Always(string line);
	TermSkill_Always(bool velocity);
	~TermSkill_Always();

	void Check(Character* attacker, SkillOrder *skillOrder);	// ���[�h�̈ڍs��������
};