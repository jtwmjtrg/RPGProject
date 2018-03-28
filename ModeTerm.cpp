#include "ModeTerm.h"
#include "Enemy.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------�X�[�p�[�N���X---------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm::ModeTerm() {

}

// ���̃��[�h�̃X�L���ǉ�
void ModeTerm::AddSkill(Skill* skillData) {
	skill.push_back(skillData);
}
// �����X�L���̎擾
Skill* ModeTerm::GetSkill() {
	// ���[�h�����烉���_���ɃX�L����I��
	return skill[GetRand(skill.size() - 1)];
}
// �I������
void ModeTerm::Release() {
	// �X�L���f�[�^�̍폜
	for (int i = 0, n = (int)skill.size(); i != n; ++i) {
		skill[i]->Release();
		delete skill[i];
		skill[i] = nullptr;	// �ꉞ�k���|�C���^������Ƃ�
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------�Ƃ��ɂȂ�-----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_None::ModeTerm_None(const unsigned int& nowMode, const unsigned int& modeNum) : nowMode(nowMode), modeNum(modeNum) {
	type = eModeTerm::None;
}

ModeTerm_None::~ModeTerm_None() {

}

bool ModeTerm_None::Check() {
	// �Ȃ��i�����Ȃ��j
	// ���݂��̃��[�h�̂Ƃ��̂�true
	return modeNum == nowMode;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------�g�o����------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_HP::ModeTerm_HP(string line, const int& hp, const int&  max_hp) : hp(hp), max_hp(max_hp){

	type = eModeTerm::Hp;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y���[�h���z�j��
	getline(L_stream, L_token, ',');
	// 3�y�ڍs�����z�j��
	getline(L_stream, L_token, ',');
	// 4�yHP�in������)�z�擾
	getline(L_stream, L_token, ',');
	this->rate = boost::lexical_cast<float>(L_token);
}

ModeTerm_HP::~ModeTerm_HP() {

}

bool ModeTerm_HP::Check() {
	// HP���K��l�����ɂȂ�����
	return hp < max_hp * rate;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------- �^�[��������i�Œ萔�j-----------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_Turn::ModeTerm_Turn(string line, const unsigned int& turnNum) : turnNum(turnNum) {

	type = eModeTerm::Turn;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��s
	getline(L_stream, L_token, ',');
	// 2�y���[�h���z�j��
	getline(L_stream, L_token, ',');
	// 3�y�ڍs�����z�j��
	getline(L_stream, L_token, ',');
	// 4�y��^�[���z�擾
	getline(L_stream, L_token, ',');
	baseTurn = boost::lexical_cast<unsigned int>(L_token);
	// 5�y���Z�q�z�擾
	getline(L_stream, L_token, ',');
	ope = (__int8)boost::lexical_cast<int>(L_token);
}
ModeTerm_Turn::~ModeTerm_Turn() {

}

bool ModeTerm_Turn::Check() {
	switch (ope){
	case 0://==
		return turnNum == baseTurn;
	case 1://!=
		return turnNum != baseTurn;
	case 2://<
		return turnNum < baseTurn;
	case 3://>
		return turnNum > baseTurn;
	case 4://%
		return turnNum % baseTurn == 0;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------- �G�̐����� ----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_EnemyNum::ModeTerm_EnemyNum(string line, const std::vector<Enemy*>& enemy) : enemy(enemy) {

	type = eModeTerm::EnemyNum;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	std::getline(L_stream, L_token, ',');
	// 2�y���[�h���z�j��
	std::getline(L_stream, L_token, ',');
	// 3�y�ڍs�����z�j��
	std::getline(L_stream, L_token, ',');
	// 4�y�Ώہz�擾
	std::getline(L_stream, L_token, ',');
	AliveType = (eAlive)boost::lexical_cast<int>(L_token);
	// 5�y�G�̐��z�擾
	std::getline(L_stream, L_token, ',');
	enemyNum = boost::lexical_cast<unsigned int>(L_token);
	// 6�y���Z�q�z�擾
	getline(L_stream, L_token, ',');
	ope = (__int8)boost::lexical_cast<int>(L_token);
}
ModeTerm_EnemyNum::~ModeTerm_EnemyNum() {

}

bool ModeTerm_EnemyNum::Check() {
	unsigned int num = 0;
	// �Ώۂ̐����v�Z
	for (int i = 0, n = enemy.size(); i < n; ++i) {
		if (enemy[i]->GetAlive() == AliveType) {
			++num;
		}
	}

	switch (ope) {
	case 0://==
		return num == enemyNum;
	case 1://!=
		return num != enemyNum;
	case 2://<
		return num < enemyNum;
	case 3://>
		return num > enemyNum;
	case 4://%
		return num % enemyNum == 0;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------- ��Ԉُ� -----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_BadStatus::ModeTerm_BadStatus(string line, const std::array<tBadStatus, 9>& badStatus) : badStatus(badStatus) {
	type = eModeTerm::BadStatus;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	std::getline(L_stream, L_token, ',');
	// 2�y���[�h���z�j��
	std::getline(L_stream, L_token, ',');
	// 3�y�ڍs�����z�j��
	std::getline(L_stream, L_token, ',');
	// 4�y�Ώہz�擾
	std::getline(L_stream, L_token, ',');
	bs_Type = (eBadStatus)boost::lexical_cast<int>(L_token);
}
ModeTerm_BadStatus::~ModeTerm_BadStatus() {

}

bool ModeTerm_BadStatus::Check() {
	if (bs_Type == eBadStatus::One_any) {
		// �ǂꂩ��ł��������Ă��true
		for (int i = 0, n = badStatus.size(); i < n; ++i) {
			if (badStatus[i].get) {
				return true;
			}
		}
	}
	else {
		// �w��̏�Ԉُ�ɂ������Ă��true
		return badStatus[(int)bs_Type].get;
	}
	return false;
}
