#include "ModeTerm.h"
#include "Enemy.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------スーパークラス---------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm::ModeTerm() {

}

// このモードのスキル追加
void ModeTerm::AddSkill(Skill* skillData) {
	skill.push_back(skillData);
}
// 発動スキルの取得
Skill* ModeTerm::GetSkill() {
	// モード内からランダムにスキルを選択
	return skill[GetRand(skill.size() - 1)];
}
// 終了処理
void ModeTerm::Release() {
	// スキルデータの削除
	for (int i = 0, n = (int)skill.size(); i != n; ++i) {
		skill[i]->Release();
		delete skill[i];
		skill[i] = nullptr;	// 一応ヌルポインタをいれとく
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*--------------------------------------------------とくになし-----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_None::ModeTerm_None(const unsigned int& nowMode, const unsigned int& modeNum) : nowMode(nowMode), modeNum(modeNum) {
	type = eModeTerm::None;
}

ModeTerm_None::~ModeTerm_None() {

}

bool ModeTerm_None::Check() {
	// なし（条件なし）
	// 現在このモードのときのみtrue
	return modeNum == nowMode;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------ＨＰ判定------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_HP::ModeTerm_HP(string line, const int& hp, const int&  max_hp) : hp(hp), max_hp(max_hp){

	type = eModeTerm::Hp;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【モード数】破棄
	getline(L_stream, L_token, ',');
	// 3【移行条件】破棄
	getline(L_stream, L_token, ',');
	// 4【HP（n％未満)】取得
	getline(L_stream, L_token, ',');
	this->rate = boost::lexical_cast<float>(L_token);
}

ModeTerm_HP::~ModeTerm_HP() {

}

bool ModeTerm_HP::Check() {
	// HPが規定値未満になったら
	return hp < max_hp * rate;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------- ターン数判定（固定数）-----------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_Turn::ModeTerm_Turn(string line, const unsigned int& turnNum) : turnNum(turnNum) {

	type = eModeTerm::Turn;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄s
	getline(L_stream, L_token, ',');
	// 2【モード数】破棄
	getline(L_stream, L_token, ',');
	// 3【移行条件】破棄
	getline(L_stream, L_token, ',');
	// 4【基準ターン】取得
	getline(L_stream, L_token, ',');
	baseTurn = boost::lexical_cast<unsigned int>(L_token);
	// 5【演算子】取得
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
/*------------------------------------------------- 敵の数判定 ----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_EnemyNum::ModeTerm_EnemyNum(string line, const std::vector<Enemy*>& enemy) : enemy(enemy) {

	type = eModeTerm::EnemyNum;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	std::getline(L_stream, L_token, ',');
	// 2【モード数】破棄
	std::getline(L_stream, L_token, ',');
	// 3【移行条件】破棄
	std::getline(L_stream, L_token, ',');
	// 4【対象】取得
	std::getline(L_stream, L_token, ',');
	AliveType = (eAlive)boost::lexical_cast<int>(L_token);
	// 5【敵の数】取得
	std::getline(L_stream, L_token, ',');
	enemyNum = boost::lexical_cast<unsigned int>(L_token);
	// 6【演算子】取得
	getline(L_stream, L_token, ',');
	ope = (__int8)boost::lexical_cast<int>(L_token);
}
ModeTerm_EnemyNum::~ModeTerm_EnemyNum() {

}

bool ModeTerm_EnemyNum::Check() {
	unsigned int num = 0;
	// 対象の数を計算
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
/*-------------------------------------------------- 状態異常 -----------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModeTerm_BadStatus::ModeTerm_BadStatus(string line, const std::array<tBadStatus, 9>& badStatus) : badStatus(badStatus) {
	type = eModeTerm::BadStatus;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	std::getline(L_stream, L_token, ',');
	// 2【モード数】破棄
	std::getline(L_stream, L_token, ',');
	// 3【移行条件】破棄
	std::getline(L_stream, L_token, ',');
	// 4【対象】取得
	std::getline(L_stream, L_token, ',');
	bs_Type = (eBadStatus)boost::lexical_cast<int>(L_token);
}
ModeTerm_BadStatus::~ModeTerm_BadStatus() {

}

bool ModeTerm_BadStatus::Check() {
	if (bs_Type == eBadStatus::One_any) {
		// どれか一つでもかかってればtrue
		for (int i = 0, n = badStatus.size(); i < n; ++i) {
			if (badStatus[i].get) {
				return true;
			}
		}
	}
	else {
		// 指定の状態異常にかかってればtrue
		return badStatus[(int)bs_Type].get;
	}
	return false;
}
