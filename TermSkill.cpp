#include "TermSkill.h"
#include "Character.h"

TermSkill::TermSkill(){

}
// ���̃��[�h�̃X�L���ǉ�
void TermSkill::AddSkill(Skill* skillData) {
	skill.push_back(skillData);
}
// �X�L������
void TermSkill::SkillExecution(Character* attacker, SkillOrder *skillOrder) {
	for (int i = 0, n = skill.size(); i < n; ++i) {
		skillOrder->Add_Velocity(attacker, skill[i], skill[i]->totalUseEnergy, velocity);
	}
}
// �I������
void TermSkill::Release() {
	// �X�L���f�[�^�̍폜
	for (int i = 0, n = skill.size(); i != n; ++i) {
		skill[i]->Release();
		delete skill[i];
		skill[i] = nullptr;	// �ꉞ�k���|�C���^������Ƃ�
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------ ���^�[������ ---------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �t�@�C���ǂݍ��ݗp�R���X�g���N�^
TermSkill_Always::TermSkill_Always(string line) {
	type = eTermSkill::Always;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y���[�hID�z�j��
	getline(L_stream, L_token, ',');
	// 2�y����ID�z�j��
	getline(L_stream, L_token, ',');
	// 3�y�^�[���ŏ�-�Ō�t���O�z
	getline(L_stream, L_token, ',');
	velocity = L_token == "1";
}
// ���ڍ쐬����R���X�g���N�^
TermSkill_Always::TermSkill_Always(bool velocity) {
	type = eTermSkill::Always;

	this->velocity = velocity;		// �ő�,�Œx�t���O
}
TermSkill_Always::~TermSkill_Always(){}
void TermSkill_Always::Check(Character* attacker, SkillOrder *skillOrder) {
	// �����Ȃ��Ŕ���
	SkillExecution(attacker, skillOrder);
}