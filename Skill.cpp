#include "Skill.h"
#include "Character.h"

// 【効果の変換量を得る関数】
double GetConvValue(const tEnergyConv energy, int totalUseEnergy) {
	if (energy.rate == -1.0) return -1;		// -1はそのまま返す

	if (energy.convType == eConvType::None) {
		// 変換対象が対象なしの場合はそのまま変換量に
		return energy.rate;
	}
	else {
		// 必要なMUE値のn%
		return (int)(totalUseEnergy * energy.rate);
	}		
}

Skill::Skill() {
	type = eSkillType::Nomal;
	//TestIni();
}
Skill::Skill(string path) {
	type = eSkillType::Nomal;
	LoadData(path);
}
Skill::Skill(int id) {
	type = eSkillType::Nomal;
	LoadData("skilldata\\" + to_string(id) + ".csv");
}
// スキルデータ読み込み
void Skill::LoadData(string path) {
	// 【ファイル読み込み用変数】
	ifstream L_File;	// 読み込むファイル
	string L_Line;		// 読み込んだ行（1行）
	int L_Count = 0;	// ノード読み込み用（何行目を読み込んでいるか）
	string L_token;		// 1行を,で分割する際に使う
	vector<vector<__int8>> L_Collision;

	// 読み込むファイルの指定
	L_File.open(path);	// ファイルオープン

	if (L_File.fail()) {	// ファイル読み込み
		name = "読み込めてないよ";
	}
	else {
		//////////////////////////////////////
		//////////// 【基本情報】//////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);

			// 1【id】取得
			getline(L_stream, L_token, ',');
			this->id = boost::lexical_cast<unsigned int>(L_token);
			// 2【名前】取得
			getline(L_stream, L_token, ',');
			name = L_token;
			// 3【速度補正】取得
			getline(L_stream, L_token, ',');
			speed = boost::lexical_cast<float>(L_token);
			// 4【攻撃範囲選択フラグ】取得
			getline(L_stream, L_token, ',');
			selectFlag = L_token == "1";
			// 5【攻撃範囲選択対象】取得
			getline(L_stream, L_token, ',');
			selectTarget = L_token == "0";
			// 6【横幅】取得
			getline(L_stream, L_token, ',');
			width = boost::lexical_cast<int>(L_token);
			// 7【縦幅】取得
			getline(L_stream, L_token, ',');
			height = boost::lexical_cast<int>(L_token);
			// 【使用MP】
			// 8【自分のMP】取得
			getline(L_stream, L_token, ',');
			useMP[0] = boost::lexical_cast<int>(L_token);
			// 9【フィールドのMP】取得
			getline(L_stream, L_token, ',');
			useMP[1] = boost::lexical_cast<int>(L_token);
			// 10【相手のMP】取得
			getline(L_stream, L_token, ',');
			useMP[2] = boost::lexical_cast<int>(L_token);
			// 11【必要なMUE】取得
			getline(L_stream, L_token, ',');
			totalUseEnergy = boost::lexical_cast<int>(L_token);
			// 11【戦闘中使用フラグ】取得
			getline(L_stream, L_token, ',');
			battleFlag = L_token == "1";
			// 10【フィールド使用フラグ】取得
			getline(L_stream, L_token, ',');
			fieldFlag = L_token == "1";
			// 11【武器制限】
			getline(L_stream, L_token, ',');
			weaponRestriction = (eWeaponType)boost::lexical_cast<int>(L_token);

			// 範囲選択フラグがtrueなら
			if (selectFlag) {
				collision.resize(height);
				for (int i = 0; i < height; i++) {
					collision[i].resize(width);
				}
				for (int i = 0, n = collision.size(); i < n; i++) {
					for (int j = 0, m = collision[i].size(); j < m; j++) {
						collision[i][j] = 0;
					}
				}
			}
		}
		//////////////////////////////////////
		///////// 【画像エフェクト】///////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);

			// 1【画像エフェクトの有無】取得
			getline(L_stream, L_token, ',');
			grEffect.flag =  L_token == "1";

			// エフェクトあり
			if (grEffect.flag) {
				// 2【分割数X】取得
				getline(L_stream, L_token, ',');
				grEffect.divX = boost::lexical_cast<int>(L_token);
				// 3【分割数Y】取得
				getline(L_stream, L_token, ',');
				grEffect.divY = boost::lexical_cast<int>(L_token);
				// 4【総分割数】取得
				getline(L_stream, L_token, ',');
				grEffect.divNum = boost::lexical_cast<int>(L_token);
				// 5【画像データ】取得
				getline(L_stream, L_token, ',');
				{// 背景
					string str__S = "img\\battle\\skill\\" + L_token;	// アドレス生成
					char* cstr = new char[str__S.size() + 1];			// メモリ確保
					strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// コピー
					grEffect.gr = LoadGraph(cstr, grEffect.divX, grEffect.divY);
					delete cstr;	// メモリ解放
				}
				//grEffect.gr = LoadGraph("img\\battle\\skill\\" + L_token, 3, 1);
			}
		}
		//////////////////////////////////////
		/////////////// 【SE】////////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);

			// 1【SEの有無】取得
			getline(L_stream, L_token, ',');
			if (L_token == "1") {
				// 2【SEのパス】取得
				getline(L_stream, L_token, ',');

				string str__S = "sound\\battle\\" + L_token;	// アドレス生成
				char* cstr = new char[str__S.size() + 1];			// メモリ確保
				strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// コピーzzz
				so_SE = LoadSoundMem(cstr);
				delete cstr;	// メモリ解放

			}
			else {
				so_SE = -1;
			}
		}
		//////////////////////////////////////
		////////// 【スキルの説明文】//////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);
			// 1行ずつ読み込む
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				explanatoryText.push_back(L_token);
			}
			// 説明文が空なら
			if (explanatoryText.empty()) {
				explanatoryText.push_back("説明文なし");
			}
		}
		//////////////////////////////////////
		////// 【スキル使用時のテキスト】//////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);
			// 1行ずつ読み込む
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				utilizationText.push_back(L_token);
			}
			// 説明文が空なら
			if (utilizationText.empty()) {
				utilizationText.push_back("の");
			}
		}

		//////////////////////////////////////
		//////////// 【スキル効果】////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// 一行ずつ読み込み
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);
			// 1列目をロードして判定
			getline(L_stream, L_token, ',');

			switch ((eSkillEffect)boost::lexical_cast<int>(L_token)) {
			case eSkillEffect::Attack:// 【攻撃】
				//effect.push_back(new SkillEffect_Attack("0,1,0.86,4,1,1,3,0,0,2,,,,,,,,"));
				effect.push_back(new SkillEffect_Attack(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Heal:// 【回復】
				//effect.push_back(new SkillEffect_Heal("1,0,1,4,1,1,20,0,,,,,,,,,,"));
				effect.push_back(new SkillEffect_Heal(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::BadStatus:// 【状態異常】
				effect.push_back(new SkillEffect_BadStatus(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Buff:// 【バフ・デバフ】
				effect.push_back(new SkillEffect_Buff(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::AnotherSkill:// 【他のスキル発動】
				effect.push_back(new SkillEffect_AnotherSkill(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Resurrection:// 【蘇生】
				effect.push_back(new SkillEffect_Resurrection(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Hate:// 【ヘイト増減】
				effect.push_back(new SkillEffect_Hate(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Charge:// 【チャージ】
				effect.push_back(new SkillEffect_Charge(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Mode:// 【モード変更】
				effect.push_back(new SkillEffect_Mode(L_Line, totalUseEnergy));
				break;
			}

			// 攻撃対象が【選択範囲】なら当たり判定を読み込む
			if (effect.back()->targetType == eSkillTarget::Select) {
				// 当たり判定作成
				effect.back()->collision.resize(height);
				for (int i = 0; i < height && getline(L_File, L_Line); i++) {
					effect.back()->collision[i].resize(width);
					std::istringstream L_stream(L_Line);
					for (int j = 0; j < width && getline(L_stream, L_token, ','); j++) {
						effect.back()->collision[i][j] = boost::lexical_cast<int>(L_token);
						collision[i][j] += boost::lexical_cast<int>(L_token);
					}
				}
			}
		}
	}

	// ファイルクローズ
	L_File.close();
}
Skill::~Skill() {

}
// SEをならす
void Skill::PlayerSE() {
	if (so_SE != -1) {
		PlaySoundMem(so_SE);
	}
}
// スキル発動（フィールド用）
void Skill::Execution_Field(Character* attacker, std::vector<PlayerChara*> playerList, Character* target) {
	if (attacker->GetAlive() != eAlive::alive) return;
	// フィールド使用不可なら発動しない
	if (!fieldFlag) return;

	if (attacker->GetStatus(eStatus::MP) < (int)useMP[0]) {
		TextBox::AddLine("MPが足りない");
		return;
	}

	for (int i = 0, n = effect.size(); i < n; ++i) {
		switch (effect[i]->targetType) {
		case eSkillTarget::Self:// 【自分】
			effect[i]->Execution_Field(attacker, attacker);
			break;
		case eSkillTarget::Ally:// 【味方全体】
		case eSkillTarget::Overall:// 【敵味方全体】
			for (int j = 0, m = playerList.size(); j < m; ++m) {
				effect[i]->Execution_Field(attacker, playerList[i]);
			}
			break;
		case eSkillTarget::Select:// 【選択】
			// 選択されてなければは発動しない
			if (target != nullptr) {
				effect[i]->Execution_Field(attacker, target);
			}
			break;
		default:// 【それ以外】
			break;
		}
	}
}
// 終了処理
void Skill::Release() {
	// スキルデータの削除
	for (int i = 0, n = effect.size(); i != n; ++i) {
		delete effect[i];
		effect[i] = nullptr;	// 一応ヌルポインタをいれとく
	}
	if (grEffect.flag) {
		DeleteGraph(grEffect.gr);
	}	
}
// debug
void Skill::TestIni() {
	name = "こうげき";

	effect.push_back(new SkillEffect_Attack());

	// 当たり判定範囲（3×3）
	collision.resize(3);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// 全マスfalseで初期化

	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = false;
		}
	}
	// 当たり判定
	collision[0][2] = true;
	collision[1][1] = true;
	collision[2][0] = true;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------- 混乱時の行動用スキル --------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Confusion::Skill_Confusion(int arWidth, int arHeight, vector<vector<__int8>> arCollision) {
	type = eSkillType::Confusion;
	//id = -1;	// ID
	//name = "混乱";	// 名前
	speed = 1;	// 補正速度
	selectFlag = true;	// 攻撃範囲選択フラグ
	//selectTarget = true;	// 攻撃範囲選択対象
	battleFlag = true;
	fieldFlag = false;
	width = arWidth;	// 攻撃範囲 横幅
	height = arHeight;	//          縦幅
	x = 0;
	y = 0;

	// 攻撃スキルエフェクト
	//effect.push_back(new SkillEffect_Attack("0,4,0,1,0,2,0.5,0.5,1,1", 0));
	effect.push_back(new SkillEffect_Attack());


	// 当たり判定
	collision.resize(height);
	for (int i = 0; i < height; i++) {
		collision[i].resize(width);
	}
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = 0;
		}
	}
	effect.back()->collision.resize(height);
	for (int i = 0; i < height; i++) {
		effect.back()->collision[i].resize(width);
		for (int j = 0; j < width; j++) {
			if(arCollision[i][j])
			effect.back()->collision[i][j] = 1;
			collision[i][j] += 1;
		}
	}
}
Skill_Confusion::~Skill_Confusion() {

}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------- なにもしない ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_None::Skill_None() {
	type = eSkillType::None;
	speed = 1;	// 補正速度
	selectFlag = false;	// 攻撃範囲選択フラグ
	battleFlag = true;
	fieldFlag = false;
}
Skill_None::~Skill_None() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- 防御 ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Defence::Skill_Defence() {
	type = eSkillType::Defence;
	speed = 10;	// 補正速度
	selectFlag = false;	// 攻撃範囲選択フラグ
	battleFlag = true;
	fieldFlag = false;
}
Skill_Defence::~Skill_Defence() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- 逃走 ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Escape::Skill_Escape() {
	type = eSkillType::Escape;
	speed = 1;	// 補正速度
	selectFlag = false;	// 攻撃範囲選択フラグ
	battleFlag = true;
	fieldFlag = false;
}
Skill_Escape::~Skill_Escape() {
}

//#################################################################################################################//
//#################################################################################################################//
/*################################################### スキル効果 ##################################################*/
//#################################################################################################################//
//#################################################################################################################//

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------ 攻撃 -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 混乱時の攻撃用初期化
SkillEffect_Attack::SkillEffect_Attack() {
	type = eSkillEffect::Attack;

	energyConv.type = eEnergy::None;	// 変換なし
	energyConv.convType = eConvType::None;
	energyConv.direction = true;
	energyConv.rate = 0;
	energyConv.correction = 2;		// STR補正
	attackData.hit = 0.5;			// 命中倍率0.5倍
	attackData.damageRate = 0.5;	// ダメージ倍率0.5
	attackData.times_min = 1;		// 1hit
	attackData.times_max = 1;


}
SkillEffect_Attack::SkillEffect_Attack(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Attack;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【エネルギーの種類】取得
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 3【変換対象】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 4【変換方向】取得
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 5【変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6【変換補正】取得
	getline(L_stream, L_token, ',');
	energyConv.correction = boost::lexical_cast<int>(L_token);
	// 7【命中補正】取得
	getline(L_stream, L_token, ',');
	attackData.hit = boost::lexical_cast<float>(L_token);
	// 8【ダメージ倍率】取得
	getline(L_stream, L_token, ',');
	attackData.damageRate = boost::lexical_cast<float>(L_token);
	// 9【攻撃対象】取得
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 10【最小攻撃回数】取得
	getline(L_stream, L_token, ',');
	attackData.times_min = boost::lexical_cast<int>(L_token);
	// 11【最大攻撃回数】取得
	getline(L_stream, L_token, ',');
	attackData.times_max = boost::lexical_cast<int>(L_token);
	
}
SkillEffect_Attack::~SkillEffect_Attack() {

}
const tSAttack* SkillEffect_Attack::GetAttack() {
	return &attackData;
}
//debug
void SkillEffect_Attack::TestIni(){
	attackData.hit = 1;
	attackData.times_min = 1;
	attackData.times_max = 3;
	//data.energy.push_back(test);
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------ 回復 -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Heal::SkillEffect_Heal() {
	type = eSkillEffect::Heal;

	// debug
	TestIni();
}
SkillEffect_Heal::SkillEffect_Heal(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Heal;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【回復タイプ】取得
	getline(L_stream, L_token, ',');
	healData.type = (eHealType)boost::lexical_cast<int>(L_token);
	// 3【倍率】取得
	getline(L_stream, L_token, ',');
	healData.rate = boost::lexical_cast<float>(L_token);
	// 4【攻撃対象】取得
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5【生命エネルギーの変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6【変換補正】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);

	// その他変換情報
	energyConv.type = eEnergy::Life;
	energyConv.direction = true;
	energyConv.correction = 0;

}
SkillEffect_Heal::~SkillEffect_Heal() {

}
const tHeal* SkillEffect_Heal::GetHeal() {
	return &healData;
}
void SkillEffect_Heal::Execution_Field(Character* attacker, Character* target) {
	if (attacker->GetAlive() != eAlive::alive) return;
	// 対象が死んでたら回復しない
	if (target->GetAlive() != eAlive::alive) return;
	
	switch (healData.type) {
	case eHealType::HP:// 【HP回復】
	{
		// 対象回復（補正72）
		switch (energyConv.convType)
		{
		case eConvType::Self:
		case eConvType::Field:
		case eConvType::Target:
			// 必要なMUE値のn% 
			target->Recovery((totalUseEnergy * energyConv.rate) * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			attacker->DecreaseMP((int)
				(energyConv, totalUseEnergy));	// MP減少
			break;
		case eConvType::None:
			// 変換対象が【対象なし】の場合はそのまま変換量に
			target->Recovery(energyConv.rate * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			break;
		default:
			break;
		}
		break;
	}
	case eHealType::MP:// 【MP回復】
		break;
	case eHealType::BadStatus:// 【全状態異常】
		target->HealBadStatus();
		break;
	case eHealType::Poison:// 【毒】
		target->HealBadStatus(eBadStatus::Poison);
		break;
	case eHealType::Paralysis:// 【麻痺】
		target->HealBadStatus(eBadStatus::Paralysis);
		break;
	case eHealType::Sleep:// 【睡眠】
		target->HealBadStatus(eBadStatus::Sleep);
		break;
	case eHealType::Confusion:// 【混乱】
		target->HealBadStatus(eBadStatus::Confusion);
		break;
	case eHealType::Blind:// 【暗闇】
		target->HealBadStatus(eBadStatus::Blind);
		break;
	case eHealType::Weakness:// 【衰弱】
		target->HealBadStatus(eBadStatus::Weakness);
		break;
	case eHealType::Debuff:// 【全デバフ】
		break;
	}
}
//debug
void SkillEffect_Heal::TestIni() {
	healData.type = eHealType::HP;
	healData.rate = 1;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- 状態異常 ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_BadStatus::SkillEffect_BadStatus() {
	type = eSkillEffect::BadStatus;
}
SkillEffect_BadStatus::SkillEffect_BadStatus(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::BadStatus;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【命中力】取得
	getline(L_stream, L_token, ',');
	badStatusData.hit = boost::lexical_cast<int>(L_token);
	// 3【威力】取得
	getline(L_stream, L_token, ',');
	badStatusData.power = boost::lexical_cast<int>(L_token);
	// 4【攻撃対象】取得
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5【状態異常の種類】取得
	getline(L_stream, L_token, ',');
	badStatusData.type = (eBadStatus)boost::lexical_cast<int>(L_token);
	// 6【エネルギーの種類】取得
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 7【変換対象】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 8【変換方向】取得
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 9【変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);

	energyConv.correction = 0;
}
SkillEffect_BadStatus::SkillEffect_BadStatus(tBadStatus_Skill badStatusData) :
	badStatusData(badStatusData)
{
	type = eSkillEffect::BadStatus;

	energyConv = { eEnergy::None, eConvType::None, false, 0, 0 };
}
SkillEffect_BadStatus::~SkillEffect_BadStatus() {

}
const tBadStatus_Skill* SkillEffect_BadStatus::GetBadStatus() {
	return &badStatusData;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* -------------------------------------------------- バフ・デバフ -------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Buff::SkillEffect_Buff() {
	type = eSkillEffect::Buff;
}
SkillEffect_Buff::SkillEffect_Buff(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Buff;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【命中力】取得
	getline(L_stream, L_token, ',');
	buffData.hit = boost::lexical_cast<float>(L_token);
	// 3【持続ターン】取得
	getline(L_stream, L_token, ',');
	buffData.trun = boost::lexical_cast<int>(L_token);
	// 4【バフかデバフか】取得
	getline(L_stream, L_token, ',');
	buffData.dir = L_token == "1";
	// 5【攻撃対象】取得
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 6【バフの種類】取得
	getline(L_stream, L_token, ',');
	buffData.type = (eBuff)boost::lexical_cast<int>(L_token);
	// 7【エネルギーの種類】取得
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 8【変換対象】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 9【変換方向】取得
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 10【変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);

	energyConv.correction = 0;
}
SkillEffect_Buff::SkillEffect_Buff(tBuff_Skill buffData, tEnergyConv energyConv)
	: buffData(buffData)
{
	type = eSkillEffect::Buff;
	this->energyConv = energyConv;

}
SkillEffect_Buff::~SkillEffect_Buff() {

}
const tBuff_Skill* SkillEffect_Buff::GetBuff() {
	return &buffData;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------- 他のスキル発動 ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill() {
	type = eSkillEffect::AnotherSkill;

	// debug
	TestIni();
}
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::AnotherSkill;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【スキルデータ】取得
	getline(L_stream, L_token, ',');
	anotherSkillData.skill = new Skill(boost::lexical_cast<int>(L_token));
	// 3【発動率】取得
	getline(L_stream, L_token, ',');
	anotherSkillData.invocation = boost::lexical_cast<int>(L_token);
	// 4【速攻フラグ】取得
	getline(L_stream, L_token, ',');
	anotherSkillData.fastFlag = L_token == "1";
	

	// 一応エネルギー変換データを初期化
	energyConv.type = eEnergy::None;
	energyConv.convType = eConvType::Self;
	energyConv.direction = true;
	energyConv.rate = 0;
	energyConv.correction = 0;
	targetType = eSkillTarget::Self;
}
SkillEffect_AnotherSkill::~SkillEffect_AnotherSkill() {

}
const tAnotherSkill* SkillEffect_AnotherSkill::GetAnotherSkill() {
	return &anotherSkillData;
}
//debug
void SkillEffect_AnotherSkill::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------ 蘇生 -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Resurrection::SkillEffect_Resurrection() {
	type = eSkillEffect::Resurrection;

	// debug
	TestIni();
}
SkillEffect_Resurrection::SkillEffect_Resurrection(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Resurrection;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【成功率】取得
	getline(L_stream, L_token, ',');
	resurrectionData.success = boost::lexical_cast<int>(L_token);
	// 3【倍率】取得
	getline(L_stream, L_token, ',');
	resurrectionData.rate = boost::lexical_cast<float>(L_token);
	// 4【対象】取得
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5【生命エネルギーの変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6【変換補正】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);


	// その他変換情報
	energyConv.type = eEnergy::Life;
	energyConv.direction = true;
	energyConv.correction = 0;
}
SkillEffect_Resurrection::~SkillEffect_Resurrection() {

}
const tResurrection* SkillEffect_Resurrection::GetResurrection() {
	return &resurrectionData;
}
void SkillEffect_Resurrection::Execution_Field(Character* attacker, Character* target) {
	if (attacker->GetAlive() != eAlive::alive) return;
	// 対象が死んでいなければ判定しない
	if (target->GetAlive() != eAlive::dead) return;

	// 対象蘇生（補正2）
	int healValue = target->Resurrection(GetConvValue(energyConv, totalUseEnergy) * resurrectionData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 2);
}
//debug
void SkillEffect_Resurrection::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------- ヘイト増減 --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Hate::SkillEffect_Hate() {
	type = eSkillEffect::Hate;

	// debug
	TestIni();
}
SkillEffect_Hate::SkillEffect_Hate(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Hate;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【ヘイト増減値】取得
	getline(L_stream, L_token, ',');
	hateData.value = boost::lexical_cast<int>(L_token);
	// 3【ヘイト増減倍率】取得
	getline(L_stream, L_token, ',');
	hateData.rate = boost::lexical_cast<float>(L_token);
	// 4【エネルギーの種類】取得
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 5【変換対象】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 6【変換方向】取得
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 7【変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// その他変換情報
	energyConv.correction = 0;
}
SkillEffect_Hate::~SkillEffect_Hate() {

}
const tHate* SkillEffect_Hate::GetHate() {
	return &hateData;
}
//debug
void SkillEffect_Hate::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- チャージ ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Charge::SkillEffect_Charge() {
	type = eSkillEffect::Charge;

	// debug
	TestIni();
}
SkillEffect_Charge::SkillEffect_Charge(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Charge;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【種類】取得
	getline(L_stream, L_token, ',');
	chargeData.type = (eCharge)boost::lexical_cast<int>(L_token);
	// 3【倍率】取得
	getline(L_stream, L_token, ',');
	chargeData.rate = boost::lexical_cast<float>(L_token);
	// 4【発動ターン】取得
	getline(L_stream, L_token, ',');
	chargeData.turn = boost::lexical_cast<int>(L_token);
	// 5【エネルギーの種類】取得
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 6【変換対象】取得
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 7【変換方向】取得
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 8【変換量】取得
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// その他変換情報
	energyConv.correction = 0;
}
SkillEffect_Charge::~SkillEffect_Charge() {

}
const tCharge* SkillEffect_Charge::GetCharge() {
	return &chargeData;
}
//debug
void SkillEffect_Charge::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------- モード変更 --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Mode::SkillEffect_Mode() {
	type = eSkillEffect::Mode;

	// debug
	TestIni();
}
SkillEffect_Mode::SkillEffect_Mode(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Mode;

	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【モード番号】取得
	getline(L_stream, L_token, ',');
	modeData.modeNum = boost::lexical_cast<int>(L_token);
	
	// 一応エネルギー変換データを初期化
	energyConv.type = eEnergy::None;
	energyConv.convType = eConvType::Self;
	energyConv.direction = true;
	energyConv.rate = 0;
	energyConv.correction = 0;
	targetType = eSkillTarget::Self;
}
SkillEffect_Mode::~SkillEffect_Mode() {

}
const tModeChange* SkillEffect_Mode::GetMode() {
	return &modeData;
}
//debug
void SkillEffect_Mode::TestIni() {
}
