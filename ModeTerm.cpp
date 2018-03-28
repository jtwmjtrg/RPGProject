#include "ModeTerm.h"
#include "Enemy.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------ƒX[ƒp[ƒNƒ‰ƒX---------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm::ModeTerm() {

}

// ‚±‚Ìƒ‚[ƒh‚ÌƒXƒLƒ‹’Ç‰Á
void ModeTerm::AddSkill(Skill* skillData) {
	skill.push_back(skillData);
}
// ”­“®ƒXƒLƒ‹‚Ìæ“¾
Skill* ModeTerm::GetSkill() {
	// ƒ‚[ƒh“à‚©‚çƒ‰ƒ“ƒ_ƒ€‚ÉƒXƒLƒ‹‚ğ‘I‘ğ
	return skill[GetRand(skill.size() - 1)];
}
// I—¹ˆ—
void ModeTerm::Release() {
	// ƒXƒLƒ‹ƒf[ƒ^‚Ìíœ
	for (int i = 0, n = (int)skill.size(); i != n; ++i) {
		skill[i]->Release();
		delete skill[i];
		skill[i] = nullptr;	// ˆê‰ƒkƒ‹ƒ|ƒCƒ“ƒ^‚ğ‚¢‚ê‚Æ‚­
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------‚Æ‚­‚É‚È‚µ-----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_None::ModeTerm_None(const unsigned int& nowMode, const unsigned int& modeNum) : nowMode(nowMode), modeNum(modeNum) {
	type = eModeTerm::None;
}

ModeTerm_None::~ModeTerm_None() {

}

bool ModeTerm_None::Check() {
	// ‚È‚µiğŒ‚È‚µj
	// Œ»İ‚±‚Ìƒ‚[ƒh‚Ì‚Æ‚«‚Ì‚İtrue
	return modeNum == nowMode;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------‚g‚o”»’è------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_HP::ModeTerm_HP(string line, const int& hp, const int&  max_hp) : hp(hp), max_hp(max_hp){

	type = eModeTerm::Hp;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yƒ‚[ƒh”z”jŠü
	getline(L_stream, L_token, ',');
	// 3yˆÚsğŒz”jŠü
	getline(L_stream, L_token, ',');
	// 4yHPin“–¢–)zæ“¾
	getline(L_stream, L_token, ',');
	this->rate = boost::lexical_cast<float>(L_token);
}

ModeTerm_HP::~ModeTerm_HP() {

}

bool ModeTerm_HP::Check() {
	// HP‚ª‹K’è’l–¢–‚É‚È‚Á‚½‚ç
	return hp < max_hp * rate;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------- ƒ^[ƒ“””»’èiŒÅ’è”j-----------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_Turn::ModeTerm_Turn(string line, const unsigned int& turnNum) : turnNum(turnNum) {

	type = eModeTerm::Turn;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠüs
	getline(L_stream, L_token, ',');
	// 2yƒ‚[ƒh”z”jŠü
	getline(L_stream, L_token, ',');
	// 3yˆÚsğŒz”jŠü
	getline(L_stream, L_token, ',');
	// 4yŠî€ƒ^[ƒ“zæ“¾
	getline(L_stream, L_token, ',');
	baseTurn = boost::lexical_cast<unsigned int>(L_token);
	// 5y‰‰Zqzæ“¾
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
/*------------------------------------------------- “G‚Ì””»’è ----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_EnemyNum::ModeTerm_EnemyNum(string line, const std::vector<Enemy*>& enemy) : enemy(enemy) {

	type = eModeTerm::EnemyNum;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	std::getline(L_stream, L_token, ',');
	// 2yƒ‚[ƒh”z”jŠü
	std::getline(L_stream, L_token, ',');
	// 3yˆÚsğŒz”jŠü
	std::getline(L_stream, L_token, ',');
	// 4y‘ÎÛzæ“¾
	std::getline(L_stream, L_token, ',');
	AliveType = (eAlive)boost::lexical_cast<int>(L_token);
	// 5y“G‚Ì”zæ“¾
	std::getline(L_stream, L_token, ',');
	enemyNum = boost::lexical_cast<unsigned int>(L_token);
	// 6y‰‰Zqzæ“¾
	getline(L_stream, L_token, ',');
	ope = (__int8)boost::lexical_cast<int>(L_token);
}
ModeTerm_EnemyNum::~ModeTerm_EnemyNum() {

}

bool ModeTerm_EnemyNum::Check() {
	unsigned int num = 0;
	// ‘ÎÛ‚Ì”‚ğŒvZ
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
/*-------------------------------------------------- ó‘ÔˆÙí -----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_BadStatus::ModeTerm_BadStatus(string line, const std::array<tBadStatus, 9>& badStatus) : badStatus(badStatus) {
	type = eModeTerm::BadStatus;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	std::getline(L_stream, L_token, ',');
	// 2yƒ‚[ƒh”z”jŠü
	std::getline(L_stream, L_token, ',');
	// 3yˆÚsğŒz”jŠü
	std::getline(L_stream, L_token, ',');
	// 4y‘ÎÛzæ“¾
	std::getline(L_stream, L_token, ',');
	bs_Type = (eBadStatus)boost::lexical_cast<int>(L_token);
}
ModeTerm_BadStatus::~ModeTerm_BadStatus() {

}

bool ModeTerm_BadStatus::Check() {
	if (bs_Type == eBadStatus::One_any) {
		// ‚Ç‚ê‚©ˆê‚Â‚Å‚à‚©‚©‚Á‚Ä‚ê‚Îtrue
		for (int i = 0, n = badStatus.size(); i < n; ++i) {
			if (badStatus[i].get) {
				return true;
			}
		}
	}
	else {
		// w’è‚Ìó‘ÔˆÙí‚É‚©‚©‚Á‚Ä‚ê‚Îtrue
		return badStatus[(int)bs_Type].get;
	}
	return false;
}
