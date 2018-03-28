#include "Character.h"
#include "Battle.h"


SkillOrder::SkillOrder() {
	maxSpeed = 9999999;
}
SkillOrder::~SkillOrder() {

}

void SkillOrder::Add(Character* attacker, Skill* skill, int totalUseEnergy) {
	double speed;	// 行動速度	

	// 最速行動　チャージ補正（速度補正）== -1
	if (skill->speed == -1||attacker->GetCharge(eCharge::Speed) == -1) {
		speed = maxSpeed + GetRand(100);
	}
	else {
		// 通常補正速度
		// AGI×スキル補正×チャージ補正（速度補正）×乱数(0.9～1.1)
		speed = attacker->GetStatus(eStatus::AGI) * skill->speed * attacker->GetCharge(eCharge::Speed) * (1.0 + (double)GetRand(-1000, 1000) * 0.0001);
		// 最速行動は抜けない
		if (speed > maxSpeed) {
			speed = maxSpeed - 1;
		}
		// 0未満にならないように
		if (speed < 0) {
			speed = 0;
		}
	}

	// 行動速度順に入れる
	for (int i = 0, n = order.size(); i < n; ++i) {
		if (order[i].speed > speed) {
			// 【チャージ補正（連続行動）】
			for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
				order.insert(order.begin() + i, { attacker, skill, speed, totalUseEnergy });
			}
			return;
		}
	}
	// 【チャージ補正（連続行動）】
	for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
		order.push_back({ attacker, skill, speed, totalUseEnergy });
	}
}
void SkillOrder::Add_Velocity(Character* attacker, Skill* skill, int totalUseEnergy, bool velocity) {
	double speed;	// 行動速度

	if (velocity) {
		// 最速行動
		speed = maxSpeed + GetRand(100);
	}
	else {
		// 最遅行動
		speed = -GetRand(100);
	}

	// 行動速度順に入れる
	for (int i = 0, n = order.size(); i < n; ++i) {
		if (order[i].speed > speed) {
			// 【チャージ補正（連続行動）】
			for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
				order.insert(order.begin() + i, { attacker, skill, speed, totalUseEnergy });
			}
			return;
		}
	}
	// 【チャージ補正（連続行動）】
	for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
		order.push_back({ attacker, skill, speed, totalUseEnergy });
	}
}
// 最後尾をポップする
void SkillOrder::Pop_Back() {
	if (!order.empty()) {
		order.pop_back();
	}
}

Character* SkillOrder::GetAttacker() {
	return order.back().attacker;
}
Skill* SkillOrder::GetSkill() {
	return order.back().skill;
}
tSkillOrder SkillOrder::GetOrder() {
	return order.back();
}

array<int, 7> Character::gr_BadStatus;		// 状態異常
int Character::se_Damage;					// ダメージse

Character::Character(){
	aliveState = eAlive::alive;	// 状態を「生存」に
	for (int i = 0; i < 9; i++) {
		// 状態異常初期化
		badStatus[i].get = false;
		// 状態異常の蓄積値を初期化
		EBB_badStatus[i] = 0;
	}

	// バフ初期化
	for (int i = 0; i < 22; ++i) {
		buffData[i].rate = 1;	// 倍率を１に
		buffData[i].trun = 0;	// ターンを０に
	}

	damageDelay = 0;	// ダメージエフェクトのカウント
	deathDelay = 0;		// 死亡エフェクトのカウント

	// ポインタ初期化
	battle = nullptr;		// 戦闘クラス
	skillOrder = nullptr;	// スキルオーダー
	confusionSkill = nullptr;// 混乱時の攻撃用スキル
	nomalAttack = nullptr;	// 通常攻撃
	weapon = nullptr;		// 武器
	armor = nullptr;		// 防具
	amulet = nullptr;		// その他

	// debug
	// 【エネルギー変換効率】
	efficiency[1][Heat]		= 0.50f;	// 熱+
	efficiency[0][Heat]		= 0.30f;	// 熱-
	efficiency[1][Elec]		= 0.25f;	// 電気+
	efficiency[0][Elec]		= 0.05f;	// 電気-
	efficiency[1][Shine]	= 0.70f;	// 光+
	efficiency[0][Shine]	= 0.50f;	// 光-
	efficiency[1][Sound]	= 0.40f;	// 音+
	efficiency[0][Sound]	= 0.35f;	// 音-
	efficiency[1][Motion]	= 0.50f;	// 運動+
	efficiency[0][Motion]	= 0.20f;	// 運動-
	efficiency[1][Gravity]	= 0.02f;	// 重力+
	efficiency[0][Gravity]	= 0.01f;	// 重力-
	efficiency[1][Life]		= 0.20f;	// 生命+
	efficiency[0][Life]		= 0.10f;	// 生命-
	efficiency[1][Static]	= 0.01f;	// 静止+
	efficiency[0][Static]	= 0.000002f;	// 静止-
	efficiency[1][Nucleus]	= 0.001f;	// 核+
	efficiency[0][Nucleus]	= 0.00004f;	// 核-
}
Character::~Character() {
	battle = nullptr;		// 戦闘クラス
	skillOrder = nullptr;	// スキルオーダー

	// 条件発動スキル
	for (int i = 0, n = (int)termSkill.size(); i < n; ++i) {
		termSkill[i]->Release();
		delete termSkill[i];
		termSkill[i] = nullptr;
	}
	delete nomalAttack;	// 通常攻撃
	nomalAttack = nullptr;
	delete confusionSkill;	// 混乱時の攻撃用スキル
	confusionSkill = nullptr;
	delete weapon;	// 武器
	weapon = nullptr;
	delete armor;	// 防具
	armor = nullptr;
	delete amulet;	// その他
	amulet = nullptr;
}
// 初期化
void Character::Init() {
	// 【音】
	se_Damage = LoadSoundMem("sound\\battle\\damage.wav");		// ダメージse
}
// 終了処理
void Character::Fina() {
	// 【音】
	DeleteSoundMem(se_Damage);	// ダメージse
}
// スキルオーダーのポインタセット
void Character::SetPointer(SkillOrder *skillOrder, Battle* battle) {
	this->skillOrder = skillOrder;
	this->battle = battle;
}
// 戦闘共通初期化処理
void Character::CommonInitBattle() {
	// 【画像】
	// 状態異常
	gr_BadStatus[0] = LoadGraph("img\\battle\\poison.png");	// 毒
	gr_BadStatus[1] = LoadGraph("img\\battle\\para.png");	// 麻痺
	gr_BadStatus[2] = LoadGraph("img\\battle\\sleep.png");	// 睡眠
	gr_BadStatus[3] = LoadGraph("img\\battle\\panic.png");	// 混乱
	gr_BadStatus[4] = LoadGraph("img\\battle\\dark.png");	// 暗闇
	gr_BadStatus[5] = LoadGraph("img\\battle\\half.png");	// 衰弱
	gr_BadStatus[6] = LoadGraph("img\\battle\\stun.png");	// 気絶

	
}
// 共通終了処理
void Character::CommonFinaBattle() {
	// 【画像】
	// 状態異常
	for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
		DeleteGraph(gr_BadStatus[i]);
	}

}

int Character::Recovery(double recovery_value){
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::HP] = true;			// うけた回復のタイプ

	int value = (int)recovery_value;

	if (value < 0) return 0;	// マイナス回復は許さない
	// 回復する
	battlestatus.HP += value;

	// HPの上限チェック
	if (battlestatus.HP >= battlestatus.maxHP) {
		value -= battlestatus.HP - battlestatus.maxHP;
		battlestatus.HP = battlestatus.maxHP;
	}
	skillHeal_HP += value;

	return value;
}
int Character::Recovery_MP(double recovery_value) {
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::MP] = true;			// うけた回復のタイプ

	int value = (int)recovery_value;

	if (value < 0) return 0;	// マイナス回復は許さない
	// 回復する
	battlestatus.MP += value;

	skillHeal_MP += value;

	return value;
}
int Character::FullRecovery() {
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::HP] = true;			// うけた回復のタイプ

	int value = battlestatus.maxHP - battlestatus.HP;

	// HPを最大値に
	battlestatus.HP = battlestatus.maxHP;
	skillHeal_HP = -1;

	return value;
}
int Character::FullRecovery_MP() {
	if (aliveState != eAlive::alive || battlestatus.maxMP < battlestatus.MP) return 0;	// すでに死んでいるor消滅なら処理しない MPが最大MPを超えていたら処理しない

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::MP] = true;			// うけた回復のタイプ

	int value = battlestatus.maxMP - battlestatus.MP;

	// HPを最大値に
	battlestatus.MP = battlestatus.maxMP;
	skillHeal_MP = -1;

	return value;
}
int Character::Damage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	int value = (int)(damage_value * GetCharge(eCharge::Defence));
	if (value < 0) return 0;
	skillDamage += (int)value;
	battlestatus.HP -= (int)value;
	// 死亡判定
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HPをマイナスにしない
		// aliveState = eAlive::dead;	// 生存状態を【死亡】に
		skillDeath = true;	// スキルで死亡したフラグを立てる
	}
	damageDelay = 50;	// ダメージエフェクトのカウント
	return (int)value;
}
// 物理ダメージ
int Character::PhysDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	// ダメージ計算（ダメージ - 防御力）×チャージ倍率（防御倍率）
	int value = (int)((damage_value - GetStatus(eStatus::DEF_PHYS)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// マイナスダメージは許さない
	skillDamage += value;		// ダメージ保存
	battlestatus.HP -= value;
	// 死亡判定
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HPをマイナスにしない
		//aliveState = eAlive::dead;	// 生存状態を【死亡】に
		skillDeath = true;	// スキルで死亡したフラグを立てる
	}
	damageDelay = 50;
	return value;

}
// 熱ダメージ
int Character::HeatDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	int value = (int)((damage_value - GetStatus(eStatus::DEF_HEAT)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// マイナスダメージは許さない
	skillDamage += value;
	battlestatus.HP -= value;
	// 死亡判定
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HPをマイナスにしない
		//aliveState = eAlive::dead;	// 生存状態を【死亡】に
		skillDeath = true;	// スキルで死亡したフラグを立てる
	}
	damageDelay = 50;
	return value;
}
// 冷気ダメージ
int Character::ColdDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	int value = (int)((damage_value - GetStatus(eStatus::DEF_COLD)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// マイナスダメージは許さない
	skillDamage += value;
	battlestatus.HP -= value;
	// 死亡判定
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HPをマイナスにしない
		//aliveState = eAlive::dead;	// 生存状態を【死亡】に
		skillDeath = true;	// スキルで死亡したフラグを立てる
	}
	damageDelay = 50;
	return value;
}
// 電気ダメージ
int Character::ElecDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// すでに死んでいるor消滅なら処理しない

	int value = (int)(((double)damage_value - (double)GetStatus(eStatus::DEF_ELEC)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// マイナスダメージは許さない
	skillDamage += value;
	battlestatus.HP -= value;
	// 死亡判定
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HPをマイナスにしない
		//aliveState = eAlive::dead;	// 生存状態を【死亡】に
		skillDeath = true;	// スキルで死亡したフラグを立てる
	}
	damageDelay = 50;
	return value;
}
// 蘇生
int Character::Resurrection(double value) {
	if (aliveState != eAlive::dead) return 0;	// 死亡時のみ有効
	if (value < 0) return 0;	// 回復量が0未満なら無効

	skillResurrection = true;	// 蘇生フラグを立てる
	battlestatus.HP += 1 + (int)value;	// 最低値１
	// HPの上限チェック
	if (battlestatus.HP >= battlestatus.maxHP) {
		value -= battlestatus.HP - battlestatus.maxHP;
		battlestatus.HP = battlestatus.maxHP;
	}
	
	return (int)value;
}
// 完全蘇生
int Character::FullResurrection() {
	if (aliveState != eAlive::dead) return 0;	// 死亡時のみ有効

	skillResurrection = true;	// 蘇生フラグを立てる
	int value = battlestatus.maxHP - battlestatus.HP;
	battlestatus.HP = battlestatus.maxHP;	// HP全快
	battlestatus.MP = battlestatus.maxMP;	// MPも全快
	return value;	// HPの回復量を返す
}

// MP増加
void Character::IncreaseMP(int value) {
	if (value <= 0)	return;		// 0以下はエラー

	battlestatus.MP += value;	// MPの増加
}
// MP減少
void Character::DecreaseMP(int value) {
	if (value <= 0)	return;		// 0以下はエラー

	battlestatus.MP -= value;	// MPの減少
	// 0未満になったら0に戻す
	if (battlestatus.MP < 0) {
		battlestatus.MP = 0;
	}
}

// 状態異常を与える
void Character::GiveBadStatus(eBadStatus type, unsigned int power) {
	skillBadStatus[(int)type] = true;	// スキル状態異常フラグを立てる

	// すでにその状態異常にかかっている場合
	if (badStatus[(int)type].get) {
		badStatus[(int)type].turn = 0;	// ターン数を０に戻す
		// 威力が高い方を選択
		if (badStatus[(int)type].power < power) {
			badStatus[(int)type].power = power;	// 威力上書き
		}
	}
	else {
		badStatus[(int)type].get = true;	// 状態異常に
		badStatus[(int)type].turn = 0;		// ターン数を０に
		badStatus[(int)type].power = power;	// 状態異常の強さ
	}
}
// 状態異常の蓄積値
void Character::GiveEddDEF_BadStatus(eBadStatus type, int value) {
	EBB_badStatus[(int)type] += value;
}

// 全状態異常回復
void Character::HealBadStatus() {
	receiveSkill[(int)eSkillEffect::Heal] = true;		// スキル回復フラグを立てる
	
	for (int i = 0; i < 7; ++i) {
		// 状態異常にかかっていいれば
		if (badStatus[i].get) {
			skillHealType[3 + i] = true;	// うけた回復のタイプ
			badStatus[i].get = false;
		}
	}
}
// 状態異常回復（１つのみ）
void Character::HealBadStatus(eBadStatus type) {
	// 状態異常にかかっていいれば
	if (badStatus[(int)type].get) {
		receiveSkill[(int)eSkillEffect::Heal ] = true;		// スキル回復フラグを立てる
		skillHealType[3 + (int)type] = true;	// うけた回復のタイプ

		badStatus[(int)type].get = false;	// 状態異常の解除
	}
}

// バフを与える
void Character::GiveBuff(eBuff type, float rate, int turn) {
	skillBuff[(int)type] = true;	// フラグを立てる

	// 【バフ】
	if (rate > 1) {
		if (buffData[(int)type].rate < 1) {
			// 弱体打消し
			skillBuffType[(int)type] = BuffEffect::Negate;
			buffData[(int)type].rate = 1;
			buffData[(int)type].trun = 0;
		}
		else {
			// バフをかける
			skillBuffType[(int)type] = BuffEffect::Buff;
			buffData[(int)type].rate *= rate;	// 倍率をかける
			buffData[(int)type].trun += turn;	// ターン延長

			// 倍率上限
			if (buffData[(int)type].rate > 10) {
				buffData[(int)type].rate = 10;
			}
			// ターン上限
			if (buffData[(int)type].rate > 5) {
				buffData[(int)type].trun = 5;
			}
		}
	}
	// 【デバフ】
	else {
		if (buffData[(int)type].rate > 1) {
			// 強化打消し
			skillBuffType[(int)type] = BuffEffect::Negate;
			buffData[(int)type].rate = 1;
			buffData[(int)type].trun = 0;
		}
		else {
			// デバフをかける
			skillBuffType[(int)type] = BuffEffect::deBuff;	// デバフフラグ
			buffData[(int)type].rate *= rate;	// 倍率をかける
			buffData[(int)type].trun += turn;	// ターン延長

			// 倍率上限
			if (buffData[(int)type].rate < 0.1) {
				buffData[(int)type].rate = 0.1f;
			}
			// ターン上限
			if (buffData[(int)type].rate > 5) {
				buffData[(int)type].trun = 5;
			}
		}
	}
}

// 防御状態にする
void Character::Defence() {
	defenceFlag = true;
}

// チャージのセット
void Character::SetCharge(eCharge type, float rate, int turn) {
	// チャージスキルをセット（同じ種類は上書き）
	chargeData[(int)type].rate = rate;
	chargeData[(int)type].turn = turn;
}

// 状態初期化
void Character::StateReset() {
	// 状態異常初期化
	for (int i = 0; i < 9; i++) {
		// 状態異常初期化
		badStatus[i].get = false;
		// 状態異常の蓄積値を初期化
		EBB_badStatus[i] = 0;
	}

	// バフ初期化
	for (int i = 0, n = buffData.size(); i < n; ++i) {
		buffData[i].trun = 0;
		buffData[i].rate = 1;
	}
	// チャージ初期化
	for (int i = 0, n = chargeData.size(); i < n; ++i) {
		chargeData[i].type = (eCharge)i;
		chargeData[i].turn = -1;
		chargeData[i].rate = 1;
	}

	damageDelay = 0;	// ダメージエフェクトのカウント
	deathDelay = 0;		// 死亡エフェクトのカウント

	// 防御状態を解除
	defenceFlag = false;

	ReceiveSkillIni();	// スキル対象フラグ初期化

}
// ターン開始処理
void Character::TurnStartProcess() {
	// 条件発動スキル
	for (int i = 0, n = termSkill.size(); i < n; ++i) {
		termSkill[i]->SkillExecution(this, skillOrder);
	}
}
// ターン終了時の処理
void Character::TurnEndProcess() {
	defenceFlag = false;	// 防御状態を解除

	if (aliveState != eAlive::alive) return;	// 死んでたら処理しない

	vector<string>	message;	// 標示メッセージ

	//////////////////////////////////////
	/////////// 【状態異常処理】///////////
	//////////////////////////////////////
	// 【毒】
	if (badStatus[(int)eBadStatus::Poison].get) {
		// 毒ダメージ(威力×0.75～1.5)
		message.push_back(name + "は" + to_string(this->Damage(this->badStatus[(int)eBadStatus::Poison].power * (GetRand(75, 150)) * 0.01)) + "の毒ダメージを受けた") ;
		// 毒死
		if (skillDeath) {
			aliveState = eAlive::dead;	// 生存状態を【死亡】に
			message.push_back(name + "は倒れた");
			TextBox::AddLine(message, 0, 10, 50);	// テキスト表示
			return;	// 死んだらここでメッセージ終了
		}
		
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Poison].turn) != 0) {
			message.push_back(name + "の毒が回復した");
			// 解除
			this->badStatus[(int)eBadStatus::Poison].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Poison, this->badStatus[(int)eBadStatus::Poison].power + GetRand(this->badStatus[(int)eBadStatus::Poison].power));
		}
		++this->badStatus[(int)eBadStatus::Poison].turn;	// 持続ターン+1
	}
	// 【麻痺】
	if (badStatus[(int)eBadStatus::Paralysis].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Paralysis].turn) != 0) {
			message.push_back(name + "の麻痺が回復した");
			// 解除
			this->badStatus[(int)eBadStatus::Paralysis].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Paralysis, this->badStatus[(int)eBadStatus::Paralysis].power + GetRand(this->badStatus[(int)eBadStatus::Paralysis].power));
		}
		++this->badStatus[(int)eBadStatus::Paralysis].turn;	// 持続ターン+1
	}
	// 【睡眠】
	if (badStatus[(int)eBadStatus::Sleep].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Sleep].turn) != 0) {
			message.push_back(name + "は目を覚ました");
			// 解除
			this->badStatus[(int)eBadStatus::Sleep].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Sleep, this->badStatus[(int)eBadStatus::Sleep].power + GetRand(this->badStatus[(int)eBadStatus::Sleep].power));
		}
		++this->badStatus[(int)eBadStatus::Sleep].turn;	// 持続ターン+1
	}
	// 【混乱】
	if (badStatus[(int)eBadStatus::Confusion].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Confusion].turn) != 0) {
			message.push_back(name + "は正気に戻った");
			// 解除
			this->badStatus[(int)eBadStatus::Confusion].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Confusion, this->badStatus[(int)eBadStatus::Confusion].power + GetRand(this->badStatus[(int)eBadStatus::Confusion].power));
		}
		++this->badStatus[(int)eBadStatus::Confusion].turn;	// 持続ターン+1
	}
	// 【暗闇】
	if (badStatus[(int)eBadStatus::Blind].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Blind].turn) != 0) {
			message.push_back(name + "の視界が戻った");
			// 解除
			this->badStatus[(int)eBadStatus::Blind].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Blind, this->badStatus[(int)eBadStatus::Blind].power + GetRand(this->badStatus[(int)eBadStatus::Blind].power));
		}
		++this->badStatus[(int)eBadStatus::Blind].turn;	// 持続ターン+1
	}
	// 【衰弱】
	if (badStatus[(int)eBadStatus::Weakness].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Weakness].turn) != 0) {
			message.push_back(name + "は衰弱から回復した");
			// 解除
			this->badStatus[(int)eBadStatus::Weakness].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Weakness, this->badStatus[(int)eBadStatus::Weakness].power + GetRand(this->badStatus[(int)eBadStatus::Weakness].power));
		}
		++this->badStatus[(int)eBadStatus::Weakness].turn;	// 持続ターン+1
	}
	// 【スタン】
	if (badStatus[(int)eBadStatus::Stun].get) {
		// 解除判定
		if (GetRand(this->badStatus[(int)eBadStatus::Stun].turn) != 0) {
			message.push_back(name + "は意識を取り戻した");
			// 解除
			this->badStatus[(int)eBadStatus::Stun].get = false;
			// 耐性蓄積値を上げる（+威力×1~2）
			GiveEddDEF_BadStatus(eBadStatus::Stun, this->badStatus[(int)eBadStatus::Stun].power + GetRand(this->badStatus[(int)eBadStatus::Stun].power));
		}
		++this->badStatus[(int)eBadStatus::Stun].turn;	// 持続ターン+1
	}

	//////////////////////////////////////
	/////////// 【バフ解除判定】///////////
	//////////////////////////////////////
	// バフの名前
	std::array<string, 22> buffName{ "HP", "最大HP",	"MP", "最大MP",	"STR", "DEX", "AGI",  "LUC", "物理防御", "熱防御",  "冷気防御", "電気防御", "MUE", 	"毒耐性", "麻痺耐性", "睡眠耐性", "混乱耐性", "暗闇耐性", "衰弱耐性", "気絶耐性", "即死耐性", "消滅耐性" };
	for (int i = 0, n = buffData.size(); i < n; ++i) {
		if (buffData[i].rate == 1.0) {
			--buffData[i].trun;	// ターン-1
			// 残りターン０で解除
			if (buffData[i].trun == 0) {
				buffData[i].rate = 1.0;	// 倍率を１に
				message.push_back(name + "の" + buffName[i] + "がもとに戻った");
			}
		}
	}

	//////////////////////////////////////
	///////////// 【チャージ】/////////////
	//////////////////////////////////////
	for (int i = 0, n = chargeData.size(); i < n; ++i) {
		// 残りターンを減らす
		if (chargeData[i].turn > -1) {
			--chargeData[i].turn;
		}
	}
	
	//////////////////////////////////////
	//////////// 【MP自動回復】////////////
	//////////////////////////////////////
	// MPが最大MP未満なら自動回復
	if (GetStatus(eStatus::MP) < GetStatus(eStatus::maxMP)) {
		IncreaseMP((int)(GetStatus(eStatus::MUE) * 0.25));	// MUEの1/4回復
		// 最大MPを超えたら戻す
		if (GetStatus(eStatus::MP) > GetStatus(eStatus::maxMP)) {
			battlestatus.MP = GetStatus(eStatus::maxMP);
		}
	}
	// MPが最大MPを超えていれば魔素暴走
	else if (GetStatus(eStatus::MP) > GetStatus(eStatus::maxMP) * 1) {
		// 最大MPから超過分だけダメージ（補正10）×0.8～1.5
		message.push_back(name + "のMPが暴走した");
		message.push_back(name + "は" + to_string(Damage((GetStatus(eStatus::MP) - GetStatus(eStatus::maxMP) * 1) * (GetRand(700, 1500) * 0.001))) + "のダメージを受けた");
		battlestatus.MP = GetStatus(eStatus::maxMP);

		// 死
		if (skillDeath) {
			aliveState = eAlive::dead;	// 生存状態を【死亡】に
			message.push_back(name + "は倒れた");
			TextBox::AddLine(message, 0, 10, 50);	// テキスト表示
			return;	// 死んだらここでメッセージ終了
		}
	}
	
	// テキスト表示
	if (!message.empty()) {
		TextBox::AddLine(message, 0, 10, 50);
	}
	return;
}

string Character::GetName(){
	return name;
}
int Character::GetMaxHP() {
	return battlestatus.maxHP;
}
int Character::GetMaxMP() {
	return battlestatus.maxMP;
}
BattleStatus Character::GetBattleStatus() {
	return battlestatus;
}
int Character::GetMaxUseEnergy() {
	return battlestatus.MUE;
}
// スキルフラグ初期化
void Character::ReceiveSkillIni() {
	// スキル対象フラグを折る
	for (int i = 0; i < 6; i++) {
		receiveSkill[i] = false;
	}
	// 回復スキルのフラグを折る
	for (int i = 0; i < 10; ++i) {
		skillHealType[i] = false;
	}
	// 状態異常フラグを折る
	for (int i = 0; i < 9; ++i) {
		skillBadStatus[i] = false;
	}
	// バフ
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		skillBuff[i] = false;
	}
	

	skillHeal_HP = 0;
	skillHeal_MP = 0;
	skillDamage = 0;
	skillDeath = false;
	skillResurrection = false;	// 蘇生
}
// スキル対象フラグを立てる
void Character::SetReceiveSkill(eSkillEffect type){
	receiveSkill[(int)type] = true;
}
// スキル対象フラグ取得
bool Character::GetReceiveSkill(eSkillEffect type) {
	return receiveSkill[(int)type];
}
int Character::GetSkillDamage() {
	return skillDamage;
}
bool Character::GetSkillDeath() {
	return skillDeath;
}

// ステータス
int Character::GetStatus(eStatus status) {
	// 状態異常【虚弱】の場合、全ステータス半減
	// 状態異常【睡眠】の場合、全ステータス半減
	// 防御状態の場合、DEF,AGI,DEX 2倍

	double value = 0;	// 返す値

	switch (status) {
	case eStatus::HP:
		return (int)(battlestatus.HP * buffData[(int)eBuff::HP].rate);
	case eStatus::maxHP:
		return (int)(battlestatus.maxHP * buffData[(int)eBuff::maxHP].rate);
	case eStatus::MP:
		return (int)(battlestatus.MP * buffData[(int)eBuff::MP].rate);
	case eStatus::maxMP:
		return (int)(battlestatus.maxMP * buffData[(int)eBuff::maxMP].rate);
	case eStatus::STR:
		value = (int)(battlestatus.STR * buffData[(int)eBuff::STR].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		return (int)value;
	case eStatus::DEX:
		value = (int)(battlestatus.DEX * buffData[(int)eBuff::DEX].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::AGI:
		value = (int)(battlestatus.AGI * buffData[(int)eBuff::AGI].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::LUC:
		return (int)(battlestatus.LUC * buffData[(int)eBuff::LUC].rate);
	case eStatus::DEF_PHYS:
		value = (int)(battlestatus.DEF_PHYS * buffData[(int)eBuff::DEF_PHYS].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_HEAT:
		value = (int)(battlestatus.DEF_HEAT * buffData[(int)eBuff::DEF_HEAT].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_COLD:
		value = (int)(battlestatus.DEF_COLD * buffData[(int)eBuff::DEF_COLD].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_ELEC:
		value = (int)(battlestatus.DEF_ELEC * buffData[(int)eBuff::DEF_ELEC].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::MUE:
		value = (int)(battlestatus.MUE * buffData[(int)eBuff::MUE].rate * GetCharge(eCharge::MUE));
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		return (int)value;
	default:// エラー
		return 0;
	}
}

// 状態異常
tBadStatus Character::GetBadStatus(eBadStatus type) {
	return badStatus[(int)type];
}

// 状態異常耐性の取得
int Character::GetDEF_BadStatus(eBadStatus type) {
	int type_res = (int)type + 13;
	// 耐性が-1 ＝ 絶対耐性
	if (DEF_badStatus[(int)type] == -1) {
		return -1;
	}
	else {
		// (耐性*バフ補正)-蓄積値
		return (int)(DEF_badStatus[(int)type] * buffData[type_res].rate) - EBB_badStatus[(int)type];
	}
}
// 生存状態
eAlive Character::GetAlive() {
	return aliveState;
}
// 行動速度
double Character::GetSpeed() {
	// AGI×スキルの速度補正
	return GetStatus(eStatus::AGI);
}
// 命中
int Character::GetHit() {
	// hit＝-1：必中
	// 0～回避(AGIとDEXの平均/10) と 0～命中(DEX×技補正)　の対抗
	// 実力均衡なら確率10%で回避する
	// ステータスが低すぎると0しか出なかったりするから×1000して判定

	// 技補正 == -1は必中攻撃

	// DEX×技補正（状態異常：暗闇ならさらに1/10）
	return (int)(GetRand((GetStatus(eStatus::DEX)) * 1000) * (badStatus[(int)eBadStatus::Blind].get ? 0.1 : 1.0));
}
// 回避力
int Character::GetAvoid() {
	// hit＝-1：必中
	// 0～回避(AGIとDEXの平均/20) と 0～命中(DEX×技補正)　の対抗
	// 実力均衡なら確率5%で回避する
	// ステータスが低すぎると0しか出なかったりするから×1000して判定

	// (DEX+AGI)/20（状態異常：暗闇ならさらに1/10）
	return (int)(GetRand(((GetStatus(eStatus::AGI) * 1000) + (GetStatus(eStatus::DEX)) * 1000) / 40) * (badStatus[(int)eBadStatus::Blind].get ? 0.1 : 1));
}
// エネルギー変換効率
double Character::GetEfficiency(eEnergy energy, bool direction) {
	if (energy == eEnergy::None) return 1.0;
	return efficiency[direction][energy];
}
// チャージ倍率
float Character::GetCharge(eCharge type) {

	// 残りターン数が0の時のみ発動
	if (chargeData[(int)type].turn == 0) {
		return chargeData[(int)type].rate;
	}

	return 1;
}

// 混乱時の攻撃用スキル取得
Skill* Character::GetConfusionSkill() {
	return confusionSkill;
}

// 武器の変更時に通常攻撃を変える処理
void Character::ChangeWeapon() {
	switch (weapon->GetType()) {
	case FIST:// 【拳】
		// 通常攻撃
		nomalAttack = new Skill("skilldata\\unique\\NA_fist.csv");
		// 混乱時の攻撃用スキル
		confusionSkill = new Skill("skilldata\\unique\\NA_fist.csv");
		break;
	case SWORD:// 【剣】
		nomalAttack = new Skill("skilldata\\unique\\NA_sword.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_sword.csv");
		break;
	case SPEAR:// 【槍】
		nomalAttack = new Skill("skilldata\\unique\\NA_spear.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_spear.csv");
		break;
	case AX:// 【斧】
		nomalAttack = new Skill("skilldata\\unique\\NA_ax.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_ax.csv");
		break;
	case WAND:// 【杖】
		nomalAttack = new Skill("skilldata\\unique\\NA_wand.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_wand.csv");
		break;
	default:// 【それ以外】
		// とりあえず拳と同じにしておく
		nomalAttack = new Skill("skilldata\\unique\\NA_fist.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_fist.csv");
		break;
	}
}

void	Character::EquipmentWeapon	(EquipmentItem* weapon) {
	delete this->weapon;	// いま装備している装備を消す
	this->weapon = weapon;	// 新しい武器を装備する
	ChangeWeapon();			// 武器変更に伴う通常攻撃の変更
}
int		Character::GetWeaponID		() {
	return weapon->GetID();
}
string	Character::GetWeaponName	() {
	return weapon->GetName();
}
void	Character::EquipmentArmor	(EquipmentItem* armor) {
	delete this->armor;	// いま装備している装備を消す
	this->armor = armor;	// 新しい武器を装備する
}
int		Character::GetArmorID		() {
	return armor->GetID();
}
string	Character::GetArmorName		() {
	return armor->GetName();
}
void	Character::EquipmentAmulet	(EquipmentItem* amulet) {
	delete this->amulet;	// いま装備している装備を消す
	this->amulet = amulet;	// 新しい武器を装備する
}
int		Character::GetAmuletID		() {
	return amulet->GetID();
}
string	Character::GetAmuletName	() {
	return amulet->GetName();
}

// debug
int Character::GetATK() {
	return battlestatus.STR + weapon->GetCorrection().STR;
}
BattleStatus Character::GetFixed() {
	//BattleStatus fixed = 
	return {
		battlestatus.maxHP		+ weapon->GetCorrection().maxHP		+ armor->GetCorrection().maxHP		+ amulet->GetCorrection().maxHP,
		battlestatus.maxMP		+ weapon->GetCorrection().maxMP		+ armor->GetCorrection().maxMP		+ amulet->GetCorrection().maxMP,
		battlestatus.HP,
		battlestatus.MP,
		battlestatus.STR		+ weapon->GetCorrection().STR		+ armor->GetCorrection().STR		+ amulet->GetCorrection().STR,
		battlestatus.DEX		+ weapon->GetCorrection().DEX		+ armor->GetCorrection().DEX		+ amulet->GetCorrection().DEX,
		battlestatus.AGI		+ weapon->GetCorrection().AGI		+ armor->GetCorrection().AGI		+ amulet->GetCorrection().AGI,
		battlestatus.LUC		+ weapon->GetCorrection().LUC		+ armor->GetCorrection().LUC		+ amulet->GetCorrection().LUC,
		battlestatus.DEF_PHYS	+ weapon->GetCorrection().DEF_PHYS	+ armor->GetCorrection().DEF_PHYS	+ amulet->GetCorrection().DEF_PHYS,
		battlestatus.DEF_HEAT	+ weapon->GetCorrection().DEF_HEAT	+ armor->GetCorrection().DEF_HEAT	+ amulet->GetCorrection().DEF_HEAT,
		battlestatus.DEF_COLD	+ weapon->GetCorrection().DEF_COLD	+ armor->GetCorrection().DEF_COLD	+ amulet->GetCorrection().DEF_COLD,
		battlestatus.DEF_ELEC	+ weapon->GetCorrection().DEF_ELEC	+ armor->GetCorrection().DEF_ELEC	+ amulet->GetCorrection().DEF_ELEC,
		battlestatus.MUE
	};
	//return fixed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------プレイヤーキャラのスーパークラス----------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlayerChara::gr_PlayerStatus;	// プレイヤーのステータスの枠（正常時）
int PlayerChara::gr_PlayerStatus_Death;	// プレイヤーのステータスの枠（死亡時）

PlayerChara::PlayerChara(string name, bool isContinue) {
	// 名前を入力
	this->name = name;

	ReceiveSkillIni();	// スキルフラグの初期化

	if (isContinue) {
		// 【セーブデータ取得】
		ifstream ifs("savedata\\" + name + "\\save.txt");
		// ファイルがない場合
		if (!ifs) {
			EXP = 0;
			LV = 1;
			skillPoint = 0;
			weapon = new EquipmentItem(0);
			armor = new EquipmentItem(0);
			amulet = new EquipmentItem(0);

			InitSkillTree();	// スキルツリーの初期化
		}
		// ファイルがある場合
		else {
			// 一時保存用
			string str;

			// 列を取得する
			while (getline(ifs, str)) {
				// 一時保存用
				istringstream stream(str);
				string token;
				int j = 0;

				// 【基本情報】
				// カンマで区切って読み込む
				while (getline(stream, token, ',')) {
					switch (j) {
					case 0:
						battlestatus.HP = boost::lexical_cast<int>(token);
						// HPが0なら死亡状態にする
						if (battlestatus.HP <= 0) {
							aliveState = eAlive::dead;
						}
						break;
					case 1:
						battlestatus.MP = boost::lexical_cast<int>(token);
						break;
					case 2:
						EXP = boost::lexical_cast<int>(token);
						break;
					case 3:
						LV = boost::lexical_cast<int>(token);
						break;
					case 4:
						weapon = new EquipmentItem(boost::lexical_cast<int>(token));
						break;
					case 5:
						armor = new EquipmentItem(boost::lexical_cast<int>(token));
						break;
					case 6:
						amulet = new EquipmentItem(boost::lexical_cast<int>(token));
						break;
					case 7:
						skillPoint = boost::lexical_cast<int>(token);
						break;
					default:
						// error
						break;
					}
					// カウントアップで次の文字へ
					++j;
				}
				break;
			}
		}
		ifs.close();	// ファイルクローズ

		LoadSkillTree();	// 取得スキルのロード

	}
	else {
		// はじめから
		EXP = 0;
		LV = 1;
		skillPoint = 10;
		weapon = new EquipmentItem(0);
		armor = new EquipmentItem(0);
		amulet = new EquipmentItem(0);

		InitSkillTree();	// スキルツリーの初期化
	}

	// 【レベルごとのステータスを取得】
	ifstream level("playerdata\\levelup\\" + name + ".csv");
	if (!level) {
		name = "読み込みエラー";
		battlestatus.maxHP = 100;
		battlestatus.maxMP = 100;
		battlestatus.STR = 10;
		battlestatus.DEX = 10;
		battlestatus.AGI = 10;
		battlestatus.LUC = 10;
		battlestatus.DEF_PHYS = 10;
		battlestatus.DEF_HEAT = 10;
		battlestatus.DEF_COLD = 10;
		battlestatus.DEF_ELEC = 10;
		battlestatus.MUE = 10;
	}
	// ファイルがある場合
	else {
		// 一時保存用
		string str;

		// 現在のレベル数まで破棄
		for (int i = 0; i < LV; ++i) {
			getline(level, str);
		}
		// 一時保存用
		istringstream stream(str);
		string token;

		//1【最大HP】
		getline(stream, token, ',');
		battlestatus.maxHP = boost::lexical_cast<int>(token);
		//2【最大MP】
		getline(stream, token, ',');
		battlestatus.maxMP = boost::lexical_cast<int>(token);
		//3【STR】
		getline(stream, token, ',');
		battlestatus.STR = boost::lexical_cast<int>(token);
		//4【DEX】
		getline(stream, token, ',');
		battlestatus.DEX = boost::lexical_cast<int>(token);
		//5【AGI】
		getline(stream, token, ',');
		battlestatus.AGI = boost::lexical_cast<int>(token);
		//6【LUC】
		getline(stream, token, ',');
		battlestatus.LUC = boost::lexical_cast<int>(token);
		//7【物理防御】
		getline(stream, token, ',');
		battlestatus.DEF_PHYS = boost::lexical_cast<int>(token);
		//8【熱防御】
		getline(stream, token, ',');
		battlestatus.DEF_HEAT = boost::lexical_cast<int>(token);
		//9【冷気防御】
		getline(stream, token, ',');
		battlestatus.DEF_COLD = boost::lexical_cast<int>(token);
		//10【電気防御】
		getline(stream, token, ',');
		battlestatus.DEF_ELEC = boost::lexical_cast<int>(token);
		//11【MUE】
		getline(stream, token, ',');
		battlestatus.MUE = boost::lexical_cast<int>(token);
	}
	level.close();	// ファイルクローズ

	// 【当たり判定範囲】（2×3）
	collision.resize(3);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(2);
	}
	// 全マスtrueで初期化
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = true;
		}
	}

	// 次のレベルアップに必要な経験値
	nextEXP = 0;
	for (int i = 0; i != LV; ++i) {
		nextEXP += 100 + LV * LV * 20;
	}

	// 【特殊スキル】
	ChangeWeapon();
}
PlayerChara::~PlayerChara() {
	// 所持スキル
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		skillList[i]->Release();
		delete skillList[i];
		skillList[i] = nullptr;
	}
}

// 取得スキルのロード
void PlayerChara::LoadSkillTree() {
	ifstream ifs("savedata\\" + name + "\\skilltree.csv");
	// ファイルがない場合
	if (!ifs) {

	}
	// ファイルがある場合
	else {
		string line;				// 読み込んだ行（1行）
		string token;				// 1行を,で分割する際に使う
		bool loadTypeFlag = true;	// true:セルのロード	false:スキルデータのロード
		int skillID;	// スキルID

		// スキルリストの削除　全て読み直す
		for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
			skillList[i]->Release();
			delete skillList[i];
			skillList[i] = nullptr;
		}
		skillList.clear();
		skillList.shrink_to_fit();

		while (getline(ifs, line)) {	// 一行ずつ読み込み
			// 読み込んだ行をコピー
			std::istringstream stream(line);
			if (line == "") break;
			// 1行目判定
			getline(stream, token, ',');
			if (token[0] == '#') {
				// セルのロード
				if (token == "#cell")	loadTypeFlag = true;
				// スキルデータのロード
				else if (token == "#skills")	loadTypeFlag = false;

				continue;
			}
			if (loadTypeFlag) {

			}
			else {
				// 【スキルデータのロード】
				// 1【ID】
				skillID = boost::lexical_cast<int>(token);
				// 2【コスト】破棄
				getline(stream, token, ',');
				// 3【取得フラグ】
				getline(stream, token, ',');
				if (token == "1") {
					// 【スキル追加】
					skillList.push_back(new Skill(skillID));
				}
			}
		}
	}
	ifs.close();	// ファイルクローズ
}
// スキルツリーの初期化
void PlayerChara::InitSkillTree() {
	// ファイル用変数
	std::vector< std::vector<std::string> > data ;
	string line;				// 読み込んだ行（1行）
	string token;				// 1行を,で分割する際に使う


	ifstream ifs("savedata\\" + name + "\\skilltree.csv");
	// ファイルがない場合
	if (!ifs) {
		return;
	}
	// ファイルがある場合
	else {
		string line;				// 読み込んだ行（1行）
		bool loadTypeFlag = true;	// true:セルのロード	false:スキルデータのロード
		// スキルリストの削除　全て読み直す
		for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
			skillList[i]->Release();
			delete skillList[i];
			skillList[i] = nullptr;
		}
		skillList.clear();
		skillList.shrink_to_fit();

		while (getline(ifs, line)) {	// 一行ずつ読み込み

			data.resize(data.size() + 1);	// 行追加
			boost::algorithm::split(data.back(), line, boost::is_any_of(","));	// 分割
			if (line == "") break;
			// 読み込んだ行をコピー
			std::istringstream stream(line);
			// 1行目判定
			//getline(stream, token, ',');
			if (data.back().front()[0] == '#') {
				// セルのロード
				if (data.back().front() == "#cell")	loadTypeFlag = true;
				// スキルデータのロード
				else if (data.back().front() == "#skills")	loadTypeFlag = false;

				continue;
			}
			if (loadTypeFlag) {

			}
			else {
				// 【スキルデータのロード】
				data.back()[2] = "0";	// 取得フラグを0にする
			}
		}
	}
	ifs.close();	// ファイルクローズ

	// ファイルオープン
	ofstream ofs("savedata\\" + name + "\\skilltree.csv");
	// 【セーブ】
	for (int i = 0, n = (int)data.size(); i < n; ++i) {
		for (int j = 0, m = (int)data[i].size() - 1; j < m; ++j) {
			ofs << data[i][j] << ',';
		}
		ofs << data[i].back();
		ofs << endl;
	}
	ofs.close();
}

// 当たり判定の作成
void PlayerChara::SetColision(vector<vector<Character*>>& collField) {
	// プレイヤーの当たり判定配置（一人：x2y3）

	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + 1][1 + order * 3 + j] = this;
			}
		}
	}
}
// 当たり判定の削除
void PlayerChara::DeleteColision(vector<vector<Character*>>& collField) {
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + 1][1 + order * 3 + j] = nullptr;
			}
		}
	}
}

void PlayerChara::InitBattle() {
	gr_PlayerStatus = LoadGraph("img\\battle\\PlayerStatus.png");	// プレイヤーのステータスの枠
	gr_PlayerStatus_Death = LoadGraph("img\\battle\\PlayerStatus_Death.png");	// プレイヤーのステータスの枠（死亡時）
	// 状態異常
	gr_BadStatus[0] = LoadGraph("img\\battle\\poison.png");	// 毒
	gr_BadStatus[1] = LoadGraph("img\\battle\\para.png");	// 麻痺
	gr_BadStatus[2] = LoadGraph("img\\battle\\sleep.png");	// 睡眠
	gr_BadStatus[3] = LoadGraph("img\\battle\\panic.png");	// 混乱
	gr_BadStatus[4] = LoadGraph("img\\battle\\dark.png");	// 暗闇
	gr_BadStatus[5] = LoadGraph("img\\battle\\half.png");	// 衰弱
	gr_BadStatus[6] = LoadGraph("img\\battle\\stun.png");	// 気絶
}
void PlayerChara::FinaBattle() {
	DeleteGraph(gr_PlayerStatus);		// プレイヤーのステータスの枠
	DeleteGraph(gr_PlayerStatus_Death);	// プレイヤーのステータスの枠（死亡時）
	// 状態異常
	for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
		DeleteGraph(gr_BadStatus[i]);
	}
}

void PlayerChara::Draw() {
	unsigned int color = WHITE;
	if (aliveState == eAlive::alive) {
		if (GetStatus(eStatus::HP) < GetStatus(eStatus::maxHP) / 3) {
			// 瀕死（HPが1/3以下）は黄色で表示
			color = YELLOW;
		}
		else {
			// 通常時は白
			color = WHITE;
		}		
	}
	else {
		// 先頭不能で赤
		color = RED;
	}

	if (damageDelay > 0) {
		// 【ダメージを受けた】
		// 振動&点滅
		if (damageDelay & 4) {
			const int vibX = GetRand(-5, 5);
			const int vibY = GetRand(-5, 5);
			DrawGraph(16 + vibX, 16 + order * 200 + vibY, (aliveState == eAlive::alive ? gr_PlayerStatus : gr_PlayerStatus_Death));		// 枠
			DrawString(color, 32 + vibX, 26 + order * 200 + vibY, "%s　Lv.%d", name.c_str(), LV);
			DrawString(color, 32 + vibX, 48 + order * 200 + 32 * 1 + vibY, "ＨＰ：%d / %d",GetStatus(eStatus::HP), GetStatus(eStatus::maxHP));
			DrawString(color, 32 + vibX, 48 + order * 200 + 32 * 2 + vibY, "ＭＰ：%d / %d", GetStatus(eStatus::MP), GetStatus(eStatus::maxMP));
			// 状態異常
			for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
				if (badStatus[i].get) {
					DrawGraph(16 + i * 42 + vibX, 160 + order * 200 + vibY, gr_BadStatus[i]);
				}
			}
		}
		--damageDelay;
	}
	else {
		DrawGraph(16, 16 + order * 200, (aliveState == eAlive::alive ? gr_PlayerStatus : gr_PlayerStatus_Death));		// 枠
		DrawString(color, 32, 26 + order * 200, "%s　Lv.%d", name.c_str(), LV);
		DrawString(color, 32, 48 + order * 200 + 32 * 1, "ＨＰ：%d / %d", GetStatus(eStatus::HP), GetStatus(eStatus::maxHP));
		DrawString(color, 32, 48 + order * 200 + 32 * 2, "ＭＰ：%d / %d", GetStatus(eStatus::MP), GetStatus(eStatus::maxMP));
		// 状態異常
		for (int i = 0, n = (int)gr_BadStatus.size(); i < n; ++i) {
			if (badStatus[i].get) {
				DrawGraph(16 + i * 42, 160 + order * 200, gr_BadStatus[i]);
			}
		}
	}
	
}

// 経験値取得
void PlayerChara::AddEXP(int exp) {
	if (exp <= 0) return;

	this->EXP += exp;	// 経験値を増やす

	unsigned __int8 levelUpCount = 0;	// レベルアップ数
	int preLV = LV;

	// レベルアップ判定
	while (nextEXP <= EXP && LV < 99) {
		++LV;	// レベルアップ数+1

		// 次のレベルアップに必要な経験値量を加算
		nextEXP += 100 + LV * LV * 20; //(100 + レベル数 * レベル数 * 20)
	}

	// レベルアップ処理
	if (LV > preLV) {
		vector<string> message;	// レベルアップの表示メッセージ
		message.push_back(name + "のレベルが上がった");

		// 【レベル】
		message.push_back(to_string(preLV) + " → " + to_string(LV));

		// 【ファイル読み込み】
		// ファイルオープン
		ifstream ifs("playerdata\\levelup\\" + name + ".csv");
		// ファイルがない場合
		if (!ifs) {
			message.push_back("レベルファイルが無い");
		}
		// ファイルがある場合
		else {
			// 一時保存用
			string str;

			// レベル分だけシーク
			for (int i = 0; i < LV; ++i) {
				getline(ifs, str);
			}

			// 一時保存用
			istringstream stream(str);
			string token;

			//1【最大HP】
			getline(stream, token, ',');
			message.push_back("最大HP：" + to_string(battlestatus.maxHP) + " → " + token);
			battlestatus.maxHP = boost::lexical_cast<int>(token);
			//2【最大MP】
			getline(stream, token, ',');
			message.push_back("最大MP：" + to_string(battlestatus.maxMP) + " → " + token);
			battlestatus.maxMP = boost::lexical_cast<int>(token);
			//3【STR】
			getline(stream, token, ',');
			message.push_back("STR：" + to_string(battlestatus.STR) + " → " + token);
			battlestatus.STR = boost::lexical_cast<int>(token);
			//4【DEX】
			getline(stream, token, ',');
			message.push_back("DEX：" + to_string(battlestatus.DEX) + " → " + token);
			battlestatus.DEX = boost::lexical_cast<int>(token);
			//5【AGI】
			getline(stream, token, ',');
			message.push_back("AGI：" + to_string(battlestatus.AGI) + " → " + token);
			battlestatus.AGI = boost::lexical_cast<int>(token);
			//6【LUC】
			getline(stream, token, ',');
			message.push_back("LUC：" + to_string(battlestatus.LUC) + " → " + token);
			battlestatus.LUC = boost::lexical_cast<int>(token);
			//7【物理防御】
			getline(stream, token, ',');
			message.push_back("物理防御：" + to_string(battlestatus.DEF_PHYS) + " → " + token);
			battlestatus.DEF_PHYS = boost::lexical_cast<int>(token);
			//8【熱防御】
			getline(stream, token, ',');
			message.push_back("熱防御：" + to_string(battlestatus.DEF_HEAT) + " → " + token);
			battlestatus.DEF_HEAT = boost::lexical_cast<int>(token);
			//9【冷気防御】
			getline(stream, token, ',');
			message.push_back("冷気防御：" + to_string(battlestatus.DEF_COLD) + " → " + token);
			battlestatus.DEF_COLD = boost::lexical_cast<int>(token);
			//10【電気防御】
			getline(stream, token, ',');
			message.push_back("電気防御：" + to_string(battlestatus.DEF_ELEC) + " → " + token);
			battlestatus.DEF_ELEC = boost::lexical_cast<int>(token);
			//11【MUE】
			getline(stream, token, ',');
			message.push_back("最大使用魔力：" + to_string(battlestatus.MUE) + " → " + token);
			battlestatus.MUE = boost::lexical_cast<int>(token);
		}

		// 【スキルポイント】
		skillPoint += (LV - preLV) * 5;
		message.push_back(to_string(((LV - preLV) * 5)) + "のスキルポイントを獲得した");

		TextBox::AddLine(message, 0, 20, 50);

		ifs.close();
		// ファイルクローズ
	}
}

// バトルフィールド座標
int PlayerChara::GetX() {
	return 1 + order * 3;
}
int PlayerChara::GetY() {
	return 1;
}

// フィールドでの自動MP回復
void PlayerChara::AutoIncMP_Field() {
	// 生きてるときのみ
	if (aliveState == eAlive::alive) {
		IncreaseMP(LV / 10 + 1);	// レベルの1/10回復
		// 最大MPは超えない
		if (battlestatus.MP > battlestatus.maxMP) {
			battlestatus.MP = battlestatus.maxMP;
		}
	}
}

// スキルポイントの消費
bool PlayerChara::DecSkillPoint(int point) {
	// 消費が0未満 or 消費量がスキルポイントを超える場合　は認めない
	if (point < 0 || (signed)skillPoint < point) return false;
	
	skillPoint -= point;
	return true;
}

void PlayerChara::Save() {
	// ファイルオープン
	ofstream ofs("savedata\\" + name + "\\save.txt");
	//1【基礎情報】
	ofs << battlestatus.HP	<< ',';		// 1残りHP
	ofs << battlestatus.MP	<< ',';		// 2残りMP
	ofs << EXP	<< ',';					// 3経験値
	ofs << LV	<< ',';					// 4レベル
	ofs << weapon->GetID() << ',';		// 5装備（武器）
	ofs << armor->GetID()  << ',';		// 6装備（防具）
	ofs << amulet->GetID() << ',';		// 7装備（装飾品）
	ofs << skillPoint << ',';			// 8スキルポイント
	ofs << endl;
	/*
	//2【スキル】
	for (int i = 0, n = skillList.size(); i < n; ++i) {
		ofs << skillList[i]->id << ',';		// スキルID保存
	}
	*/
	ofs.close();
}
// 保持スキルの取得
vector<Skill*> PlayerChara::GetSkillList() {
	return skillList;
}
// 戦闘スキルの取得
vector<Skill*> PlayerChara::GetSkillList_Battle() {
	vector<Skill*> tmp_List;
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		// 戦闘中に使えるか
		if (skillList[i]->battleFlag) {
			// スキルに武器制限がない or 対応した武器を装備している
			if (skillList[i]->weaponRestriction == eWeaponType::LIMITLESS || skillList[i]->weaponRestriction == weapon->GetType()) {
				tmp_List.push_back(skillList[i]);
			}
		}
	}
	return tmp_List;
}
// フィールドスキルの取得
vector<Skill*> PlayerChara::GetSkillList_Field() {
	vector<Skill*> tmp_List;
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		// フィールドで使えるか
		if (skillList[i]->fieldFlag) {
			// スキルに武器制限がない or 対応した武器を装備している
			if (skillList[i]->weaponRestriction == eWeaponType::LIMITLESS || skillList[i]->weaponRestriction == weapon->GetType()) {
				tmp_List.push_back(skillList[i]);
			}
		}
	}
	return tmp_List;
}

// キャラクタが行動可能か返す ＆ 状態に応じた行動をする
bool PlayerChara::CheckAbleAct() {

	// 【死】
	if (aliveState != eAlive::alive) {
		return false;
	}

	// 【睡眠】
	if (badStatus[(int)eBadStatus::Sleep].get) {
		// 「なにもしない」をスキルオーダーに追加
		//skillOrder->Add(this, skillNone);
		return false;
	}
	// 【混乱】
	if (badStatus[(int)eBadStatus::Confusion].get) {
		// 「なにもしない」をスキルオーダーに追加
		//killOrder->Add(this, skillNone);
		return false;
	}
	// 【スタン】
	if (badStatus[(int)eBadStatus::Stun].get) {
		// 「なにもしない」をスキルオーダーに追加
		//skillOrder->Add(this, skillNone);
		return false;
	}
	
	// 行動可能
	return true;
}

// スキルのメッセージ取得
void PlayerChara::GetEffectText(vector<string>& message) {
	// 【ダメージ】
	if (receiveSkill[(int)eSkillEffect::Attack]) {
		if (skillDamage > 0) {
			// ダメージを受けた
			message.push_back(name + "は" + to_string(skillDamage) + "のダメージを受けた");
			PlaySoundMem(se_Damage);	// ダメージse
		}
		else {
			// ダメージなし
			message.push_back(name + "はダメージを受けなかった");
		}
		// 寝てたら起こす
		HealBadStatus(eBadStatus::Sleep);
	}
	// 【死亡】
	if (skillDeath) {
		battlestatus.HP = 0;
		aliveState = eAlive::dead;	// 生存状態を【死亡】に
		message.push_back(name + "は倒れた");
		return;	// 死んだらここでメッセージ終了
	}
	// 【蘇生】
	if (skillResurrection) {
		aliveState = eAlive::alive;	// 生存状態を【生存】に
		message.push_back(name + "が復活した");
	}
	// 【状態異常】
	if (skillBadStatus[(int)eBadStatus::Poison]) {	// 【毒】
		message.push_back(name + "は毒に侵された");
	}
	if (skillBadStatus[(int)eBadStatus::Paralysis]) {	// 【麻痺】
		message.push_back(name + "は身体が麻痺した");
	}
	if (skillBadStatus[(int)eBadStatus::Sleep]) {	// 【睡眠】
		message.push_back(name + "は眠りについた");
	}
	if (skillBadStatus[(int)eBadStatus::Confusion]) {	// 【混乱】
		message.push_back(name + "は正気を失った");
	}
	if (skillBadStatus[(int)eBadStatus::Blind]) {	// 【暗闇】
		message.push_back(name + "は視界を奪われた");
	}
	if (skillBadStatus[(int)eBadStatus::Weakness]) {	// 【衰弱】
		message.push_back(name + "は衰弱状態になった");
		// HP > 最大HPなら HPを最大HPにする
		if (battlestatus.HP > GetStatus(eStatus::maxHP)) {
			battlestatus.HP = GetStatus(eStatus::maxHP);
		}
	}
	if (skillBadStatus[(int)eBadStatus::Stun]) {	// 【スタン】
		message.push_back(name + "は気絶した");
	}
	if (skillBadStatus[(int)eBadStatus::Death]) {	// 【即死】
		message.push_back(name + "は即死した");
		battlestatus.HP = 0;	// HPを強制的に0にする
		aliveState = eAlive::dead;	// 状態を【死亡】にする
		skillDeath = true;	// スキルで死亡したフラグを立てる
		return;	// 死んだらここでメッセージ終了
	}
	if (skillBadStatus[(int)eBadStatus::Extinction]) {	// 【消滅】
		aliveState = eAlive::extinction;	// 状態を【消滅】にする
		return;	// 死んだらここでメッセージ終了
		//message.push_back(name + "");
	}
	// 【回復】
	if (receiveSkill[(int)eSkillEffect::Heal]) {
		if (skillHealType[(int)eHealType::HP]) {	// 【HP】
			if (skillHeal_HP != -1) {
				message.push_back(name + "は" + to_string(skillHeal_HP) + "回復した");
			}
			else {
				message.push_back(name + "のＨＰが全快した");
			}
		}
		if (skillHealType[(int)eHealType::MP]) {	// 【MP】
			if (skillHeal_MP != -1) {
				message.push_back(name + "のＭＰが" + to_string(skillHeal_MP) + "上昇した");
			}
			else {
				message.push_back(name + "のＭＰが全快した");
			}
		}
		// 【全状態異常】状態異常ごとに標示
		if (skillHealType[(int)eHealType::Poison]) {	// 【毒】
			message.push_back(name + "の毒が回復した");
		}
		if (skillHealType[(int)eHealType::Paralysis]) {	// 【麻痺】
			message.push_back(name + "の麻痺が回復した");
		}
		if (skillHealType[(int)eHealType::Sleep]) {	// 【睡眠】
			message.push_back(name + "は目を覚ました");
		}
		if (skillHealType[(int)eHealType::Confusion]) {	// 【混乱】
			message.push_back(name + "は正気に戻った");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// 【暗闇】
			message.push_back(name + "の視界が戻った");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// 【衰弱】
			message.push_back(name + "は衰弱から回復した");
		}
		if (skillHealType[(int)eHealType::Debuff]) {	// 【デバフ】
			message.push_back(name + "の弱体が治った");
		}

	}
	

	// バフ
	std::array<string, 22> buffName{ "HP", "最大HP",	"MP", "最大MP",	"STR", "DEX", "AGI",  "LUC", "物理防御", "熱防御",  "冷気防御", "電気防御", "MUE", 	"毒耐性", "麻痺耐性", "睡眠耐性", "混乱耐性", "暗闇耐性", "衰弱耐性", "気絶耐性", "即死耐性", "消滅耐性" };
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		if (skillBuff[i]) {
			switch (skillBuffType[i]){
			case BuffEffect::Negate:
				message.push_back(name + "の" + buffName[i] + "がもとに戻った");
				break;
			case BuffEffect::Buff:
				message.push_back(name + "の" + buffName[i] + "が上昇した");
				break;
			case BuffEffect::deBuff:
				message.push_back(name + "の" + buffName[i] + "が減少した");
				break;
			}			
		}
	}

	ReceiveSkillIni();	 // スキルフラグ初期化

}
int PlayerChara::GetEXP() {
	return EXP;
}
int PlayerChara::GetLV() {
	return LV;
}
// 通常攻撃取得
Skill* PlayerChara::GetNomalAttack() {
	return nomalAttack;
}

// ステータス取得
int PlayerChara::GetStatus(eStatus status) {
	// 状態異常【虚弱】の場合、全ステータス半減
	// 状態異常【睡眠】の場合、全ステータス半減
	// 防御状態の場合、DEF,AGI,DEX 2倍

	double value = 0;	// 返す値

	switch (status) {
	case eStatus::HP:
		return (int)(battlestatus.HP * buffData[(int)eBuff::HP].rate);
	case eStatus::maxHP:
		return (int)((battlestatus.maxHP + weapon->GetCorrection().maxHP + armor->GetCorrection().maxHP + amulet->GetCorrection().maxHP) * buffData[(int)eBuff::maxHP].rate);
	case eStatus::MP:
		return (int)(battlestatus.MP * buffData[(int)eBuff::MP].rate);
	case eStatus::maxMP:
		return (int)((battlestatus.maxMP + weapon->GetCorrection().maxMP + armor->GetCorrection().maxMP + amulet->GetCorrection().maxMP) * buffData[(int)eBuff::maxMP].rate);
	case eStatus::STR:
		value = (int)((battlestatus.STR + weapon->GetCorrection().STR + armor->GetCorrection().STR + amulet->GetCorrection().STR) * buffData[(int)eBuff::STR].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		return (int)value;
	case eStatus::DEX:
		value = (int)((battlestatus.DEX + weapon->GetCorrection().DEX + armor->GetCorrection().DEX + amulet->GetCorrection().DEX) * buffData[(int)eBuff::DEX].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::AGI:
		value = (int)((battlestatus.AGI + weapon->GetCorrection().AGI + armor->GetCorrection().AGI + amulet->GetCorrection().AGI) * buffData[(int)eBuff::AGI].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::LUC:
		return (int)((battlestatus.LUC + weapon->GetCorrection().LUC + armor->GetCorrection().LUC + amulet->GetCorrection().LUC) * buffData[(int)eBuff::LUC].rate);
	case eStatus::DEF_PHYS:
		value = (int)((battlestatus.DEF_PHYS + weapon->GetCorrection().DEF_PHYS + armor->GetCorrection().DEF_PHYS + amulet->GetCorrection().DEF_PHYS) * buffData[(int)eBuff::DEF_PHYS].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_HEAT:
		value = (int)((battlestatus.DEF_HEAT + weapon->GetCorrection().DEF_HEAT + armor->GetCorrection().DEF_HEAT + amulet->GetCorrection().DEF_HEAT) * buffData[(int)eBuff::DEF_HEAT].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_COLD:
		value = (int)((battlestatus.DEF_COLD + weapon->GetCorrection().DEF_COLD + armor->GetCorrection().DEF_COLD + amulet->GetCorrection().DEF_COLD) * buffData[(int)eBuff::DEF_COLD].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::DEF_ELEC:
		value = (int)((battlestatus.DEF_ELEC + weapon->GetCorrection().DEF_ELEC + armor->GetCorrection().DEF_ELEC + amulet->GetCorrection().DEF_ELEC) * buffData[(int)eBuff::DEF_ELEC].rate);
		// 【睡眠】
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// 半減
		}
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		// 【防御状態】
		if (defenceFlag) {
			value *= 2;	// 2倍
		}
		return (int)value;
	case eStatus::MUE:
		value = (int)(battlestatus.MUE * buffData[(int)eBuff::MUE].rate * GetCharge(eCharge::MUE));
		// 【衰弱】
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// 半減
		}
		return (int)value;
	default:// エラー
		return 0;
	}
}

int PlayerChara::GetPlayerOrder() {
	return order;
}
// スキルポイント
unsigned int PlayerChara::GetSkillPoint() {
	return skillPoint;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------------- ケータ ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Kata::Kata(string name, bool isContinue) : PlayerChara(name, isContinue) {
	// ケータ特有の処理を書く

	// 刻印（STR上昇）
	curse = new SkillEffect_Buff(
	{
		eBuff::STR,			// 力上昇
		1,					// 命中（意味なし）
		true,				// true:バフ	false:デバフ
		2,					// 持続ターン
	},
	{
		eEnergy::Life,		// 変換エネルギー
		eConvType::Self,	// 自分の魔素を使う
		true,				// +変換
		1,					// 変換割合：全変換
		0,					// 補正なし
	});
}
Kata::~Kata() {

}
// Lv.1状態への初期化
void Kata::Lv1_Init() {
	battlestatus.HP = 108;
	battlestatus.MP = 32;
	/*
	skillList.push_back(new Skill(64));		// 挑発
	skillList.push_back(new Skill(67));		// フロストスライス
	skillList.push_back(new Skill(85));		// クロススラッシュ
	skillList.push_back(new Skill(86));		// 一閃
	skillList.push_back(new Skill(80));		// 力溜め
	*/
}
// 固有処理（刻印とか）
void Kata::UniqueProcess() {
	if (aliveState != eAlive::alive) return;	// 死んでたら処理しない

	vector<string>	message;	// 標示メッセージ

	// 【力の刻印発動】
	message.push_back(name + " の 力の刻印 が発動した");
	// MUEの1 / 10分だけMPを消費
	const int tmp_useMP = (battlestatus.MP >= battlestatus.MUE / 10 ? battlestatus.MUE / 10 : battlestatus.MP);
	// 効果発動
	battle->SkillEffectExec(GetX(), GetY(), this, this, curse, battlestatus.MUE / 10, 0);
	// MP減少
	battlestatus.MP -= tmp_useMP;
	// 効果テキスト取得
	GetEffectText(message);


	// テキスト表示
	if (!message.empty()) {
		TextBox::AddLine(message, 0, 10, 50);
	}

}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- ティー -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tee::Tee(string name, bool isContinue) : PlayerChara(name, isContinue) {
	// ティー特有の処理を書く
	int a = 0;
}
Tee::~Tee() {

}
// Lv.1状態への初期化
void Tee::Lv1_Init() {
	battlestatus.HP = 76;
	battlestatus.MP = 59;
	/*
	skillList.push_back(new Skill(68));		// ファイアショット
	skillList.push_back(new Skill(60));		// フリーズミスト
	skillList.push_back(new Skill(83));		// ライトニング
	skillList.push_back(new Skill(56));		// ヒール
	skillList.push_back(new Skill(92));		// キュア
	skillList.push_back(new Skill(63));		// リザレクション
	*/
}
// 固有処理（刻印とか）
void Tee::UniqueProcess() {
	if (aliveState != eAlive::alive) return;	// 死んでたら処理しない

	vector<string>	message;	// 標示メッセージ

	// 【集魔の刻印発動】
	message.push_back(name + " の 集魔の刻印 が発動した");
	// MUEの1 / 4分だけフィールドからMPを吸収
	const int tmp_moveMP = (battle->GetFieldMP() >= battlestatus.MUE / 4 ? battlestatus.MUE / 4 : battle->GetFieldMP());
	// フィールドMP減少
	battle->DecreaseFieldMP(tmp_moveMP);
	// MP上昇
	IncreaseMP(tmp_moveMP);
	message.push_back(name + " のMPが " + to_string(tmp_moveMP) + " 増加した");

	// テキスト表示
	if (!message.empty()) {
		TextBox::AddLine(message, 0, 10, 50);
	}
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- アオイ -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Aoi::Aoi(string name, bool isContinue) :PlayerChara(name, isContinue) {
	// 
}
Aoi::~Aoi() {

}

// Lv.1状態への初期化
void Aoi::Lv1_Init() {
	battlestatus.HP = 76;
	battlestatus.MP = 59;
}