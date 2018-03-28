#include "TermSkill.h"
#include "Character.h"

TermSkill::TermSkill(){

}
// このモードのスキル追加
void TermSkill::AddSkill(Skill* skillData) {
	skill.push_back(skillData);
}
// スキル発動
void TermSkill::SkillExecution(Character* attacker, SkillOrder *skillOrder) {
	for (int i = 0, n = skill.size(); i < n; ++i) {
		skillOrder->Add_Velocity(attacker, skill[i], skill[i]->totalUseEnergy, velocity);
	}
}
// 終了処理
void TermSkill::Release() {
	// スキルデータの削除
	for (int i = 0, n = skill.size(); i != n; ++i) {
		skill[i]->Release();
		delete skill[i];
		skill[i] = nullptr;	// 一応ヌルポインタをいれとく
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------ 毎ターン発動 ---------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル読み込み用コンストラクタ
TermSkill_Always::TermSkill_Always(string line) {
	type = eTermSkill::Always;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【ロードID】破棄
	getline(L_stream, L_token, ',');
	// 2【条件ID】破棄
	getline(L_stream, L_token, ',');
	// 3【ターン最初-最後フラグ】
	getline(L_stream, L_token, ',');
	velocity = L_token == "1";
}
// 直接作成するコンストラクタ
TermSkill_Always::TermSkill_Always(bool velocity) {
	type = eTermSkill::Always;

	this->velocity = velocity;		// 最速,最遅フラグ
}
TermSkill_Always::~TermSkill_Always(){}
void TermSkill_Always::Check(Character* attacker, SkillOrder *skillOrder) {
	// 条件なしで発動
	SkillExecution(attacker, skillOrder);
}