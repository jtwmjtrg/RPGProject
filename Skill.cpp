#include "Skill.h"
#include "Character.h"

// yŒø‰Ê‚Ì•ÏŠ·—Ê‚ğ“¾‚éŠÖ”z
double GetConvValue(const tEnergyConv energy, int totalUseEnergy) {
	if (energy.rate == -1.0) return -1;		// -1‚Í‚»‚Ì‚Ü‚Ü•Ô‚·

	if (energy.convType == eConvType::None) {
		// •ÏŠ·‘ÎÛ‚ª‘ÎÛ‚È‚µ‚Ìê‡‚Í‚»‚Ì‚Ü‚Ü•ÏŠ·—Ê‚É
		return energy.rate;
	}
	else {
		// •K—v‚ÈMUE’l‚Ìn%
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
// ƒXƒLƒ‹ƒf[ƒ^“Ç‚İ‚İ
void Skill::LoadData(string path) {
	// yƒtƒ@ƒCƒ‹“Ç‚İ‚İ—p•Ï”z
	ifstream L_File;	// “Ç‚İ‚Şƒtƒ@ƒCƒ‹
	string L_Line;		// “Ç‚İ‚ñ‚¾si1sj
	int L_Count = 0;	// ƒm[ƒh“Ç‚İ‚İ—pi‰½s–Ú‚ğ“Ç‚İ‚ñ‚Å‚¢‚é‚©j
	string L_token;		// 1s‚ğ,‚Å•ªŠ„‚·‚éÛ‚Ég‚¤
	vector<vector<__int8>> L_Collision;

	// “Ç‚İ‚Şƒtƒ@ƒCƒ‹‚Ìw’è
	L_File.open(path);	// ƒtƒ@ƒCƒ‹ƒI[ƒvƒ“

	if (L_File.fail()) {	// ƒtƒ@ƒCƒ‹“Ç‚İ‚İ
		name = "“Ç‚İ‚ß‚Ä‚È‚¢‚æ";
	}
	else {
		//////////////////////////////////////
		//////////// yŠî–{î•ñz//////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);

			// 1yidzæ“¾
			getline(L_stream, L_token, ',');
			this->id = boost::lexical_cast<unsigned int>(L_token);
			// 2y–¼‘Ozæ“¾
			getline(L_stream, L_token, ',');
			name = L_token;
			// 3y‘¬“x•â³zæ“¾
			getline(L_stream, L_token, ',');
			speed = boost::lexical_cast<float>(L_token);
			// 4yUŒ‚”ÍˆÍ‘I‘ğƒtƒ‰ƒOzæ“¾
			getline(L_stream, L_token, ',');
			selectFlag = L_token == "1";
			// 5yUŒ‚”ÍˆÍ‘I‘ğ‘ÎÛzæ“¾
			getline(L_stream, L_token, ',');
			selectTarget = L_token == "0";
			// 6y‰¡•zæ“¾
			getline(L_stream, L_token, ',');
			width = boost::lexical_cast<int>(L_token);
			// 7yc•zæ“¾
			getline(L_stream, L_token, ',');
			height = boost::lexical_cast<int>(L_token);
			// yg—pMPz
			// 8y©•ª‚ÌMPzæ“¾
			getline(L_stream, L_token, ',');
			useMP[0] = boost::lexical_cast<int>(L_token);
			// 9yƒtƒB[ƒ‹ƒh‚ÌMPzæ“¾
			getline(L_stream, L_token, ',');
			useMP[1] = boost::lexical_cast<int>(L_token);
			// 10y‘Šè‚ÌMPzæ“¾
			getline(L_stream, L_token, ',');
			useMP[2] = boost::lexical_cast<int>(L_token);
			// 11y•K—v‚ÈMUEzæ“¾
			getline(L_stream, L_token, ',');
			totalUseEnergy = boost::lexical_cast<int>(L_token);
			// 11yí“¬’†g—pƒtƒ‰ƒOzæ“¾
			getline(L_stream, L_token, ',');
			battleFlag = L_token == "1";
			// 10yƒtƒB[ƒ‹ƒhg—pƒtƒ‰ƒOzæ“¾
			getline(L_stream, L_token, ',');
			fieldFlag = L_token == "1";
			// 11y•Ší§ŒÀz
			getline(L_stream, L_token, ',');
			weaponRestriction = (eWeaponType)boost::lexical_cast<int>(L_token);

			// ”ÍˆÍ‘I‘ğƒtƒ‰ƒO‚ªtrue‚È‚ç
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
		///////// y‰æ‘œƒGƒtƒFƒNƒgz///////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);

			// 1y‰æ‘œƒGƒtƒFƒNƒg‚Ì—L–³zæ“¾
			getline(L_stream, L_token, ',');
			grEffect.flag =  L_token == "1";

			// ƒGƒtƒFƒNƒg‚ ‚è
			if (grEffect.flag) {
				// 2y•ªŠ„”Xzæ“¾
				getline(L_stream, L_token, ',');
				grEffect.divX = boost::lexical_cast<int>(L_token);
				// 3y•ªŠ„”Yzæ“¾
				getline(L_stream, L_token, ',');
				grEffect.divY = boost::lexical_cast<int>(L_token);
				// 4y‘•ªŠ„”zæ“¾
				getline(L_stream, L_token, ',');
				grEffect.divNum = boost::lexical_cast<int>(L_token);
				// 5y‰æ‘œƒf[ƒ^zæ“¾
				getline(L_stream, L_token, ',');
				{// ”wŒi
					string str__S = "img\\battle\\skill\\" + L_token;	// ƒAƒhƒŒƒX¶¬
					char* cstr = new char[str__S.size() + 1];			// ƒƒ‚ƒŠŠm•Û
					strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// ƒRƒs[
					grEffect.gr = LoadGraph(cstr, grEffect.divX, grEffect.divY);
					delete cstr;	// ƒƒ‚ƒŠ‰ğ•ú
				}
				//grEffect.gr = LoadGraph("img\\battle\\skill\\" + L_token, 3, 1);
			}
		}
		//////////////////////////////////////
		/////////////// ySEz////////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);

			// 1ySE‚Ì—L–³zæ“¾
			getline(L_stream, L_token, ',');
			if (L_token == "1") {
				// 2ySE‚ÌƒpƒXzæ“¾
				getline(L_stream, L_token, ',');

				string str__S = "sound\\battle\\" + L_token;	// ƒAƒhƒŒƒX¶¬
				char* cstr = new char[str__S.size() + 1];			// ƒƒ‚ƒŠŠm•Û
				strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// ƒRƒs[zzz
				so_SE = LoadSoundMem(cstr);
				delete cstr;	// ƒƒ‚ƒŠ‰ğ•ú

			}
			else {
				so_SE = -1;
			}
		}
		//////////////////////////////////////
		////////// yƒXƒLƒ‹‚Ìà–¾•¶z//////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);
			// 1s‚¸‚Â“Ç‚İ‚Ş
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				explanatoryText.push_back(L_token);
			}
			// à–¾•¶‚ª‹ó‚È‚ç
			if (explanatoryText.empty()) {
				explanatoryText.push_back("à–¾•¶‚È‚µ");
			}
		}
		//////////////////////////////////////
		////// yƒXƒLƒ‹g—p‚ÌƒeƒLƒXƒgz//////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);
			// 1s‚¸‚Â“Ç‚İ‚Ş
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				utilizationText.push_back(L_token);
			}
			// à–¾•¶‚ª‹ó‚È‚ç
			if (utilizationText.empty()) {
				utilizationText.push_back("‚Ì");
			}
		}

		//////////////////////////////////////
		//////////// yƒXƒLƒ‹Œø‰Êz////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// ˆês‚¸‚Â“Ç‚İ‚İ
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);
			// 1—ñ–Ú‚ğƒ[ƒh‚µ‚Ä”»’è
			getline(L_stream, L_token, ',');

			switch ((eSkillEffect)boost::lexical_cast<int>(L_token)) {
			case eSkillEffect::Attack:// yUŒ‚z
				//effect.push_back(new SkillEffect_Attack("0,1,0.86,4,1,1,3,0,0,2,,,,,,,,"));
				effect.push_back(new SkillEffect_Attack(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Heal:// y‰ñ•œz
				//effect.push_back(new SkillEffect_Heal("1,0,1,4,1,1,20,0,,,,,,,,,,"));
				effect.push_back(new SkillEffect_Heal(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::BadStatus:// yó‘ÔˆÙíz
				effect.push_back(new SkillEffect_BadStatus(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Buff:// yƒoƒtEƒfƒoƒtz
				effect.push_back(new SkillEffect_Buff(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::AnotherSkill:// y‘¼‚ÌƒXƒLƒ‹”­“®z
				effect.push_back(new SkillEffect_AnotherSkill(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Resurrection:// y‘h¶z
				effect.push_back(new SkillEffect_Resurrection(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Hate:// yƒwƒCƒg‘Œ¸z
				effect.push_back(new SkillEffect_Hate(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Charge:// yƒ`ƒƒ[ƒWz
				effect.push_back(new SkillEffect_Charge(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Mode:// yƒ‚[ƒh•ÏXz
				effect.push_back(new SkillEffect_Mode(L_Line, totalUseEnergy));
				break;
			}

			// UŒ‚‘ÎÛ‚ªy‘I‘ğ”ÍˆÍz‚È‚ç“–‚½‚è”»’è‚ğ“Ç‚İ‚Ş
			if (effect.back()->targetType == eSkillTarget::Select) {
				// “–‚½‚è”»’èì¬
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

	// ƒtƒ@ƒCƒ‹ƒNƒ[ƒY
	L_File.close();
}
Skill::~Skill() {

}
// SE‚ğ‚È‚ç‚·
void Skill::PlayerSE() {
	if (so_SE != -1) {
		PlaySoundMem(so_SE);
	}
}
// ƒXƒLƒ‹”­“®iƒtƒB[ƒ‹ƒh—pj
void Skill::Execution_Field(Character* attacker, std::vector<PlayerChara*> playerList, Character* target) {
	if (attacker->GetAlive() != eAlive::alive) return;
	// ƒtƒB[ƒ‹ƒhg—p•s‰Â‚È‚ç”­“®‚µ‚È‚¢
	if (!fieldFlag) return;

	if (attacker->GetStatus(eStatus::MP) < (int)useMP[0]) {
		TextBox::AddLine("MP‚ª‘«‚è‚È‚¢");
		return;
	}

	for (int i = 0, n = effect.size(); i < n; ++i) {
		switch (effect[i]->targetType) {
		case eSkillTarget::Self:// y©•ªz
			effect[i]->Execution_Field(attacker, attacker);
			break;
		case eSkillTarget::Ally:// y–¡•û‘S‘Ìz
		case eSkillTarget::Overall:// y“G–¡•û‘S‘Ìz
			for (int j = 0, m = playerList.size(); j < m; ++m) {
				effect[i]->Execution_Field(attacker, playerList[i]);
			}
			break;
		case eSkillTarget::Select:// y‘I‘ğz
			// ‘I‘ğ‚³‚ê‚Ä‚È‚¯‚ê‚Î‚Í”­“®‚µ‚È‚¢
			if (target != nullptr) {
				effect[i]->Execution_Field(attacker, target);
			}
			break;
		default:// y‚»‚êˆÈŠOz
			break;
		}
	}
}
// I—¹ˆ—
void Skill::Release() {
	// ƒXƒLƒ‹ƒf[ƒ^‚Ìíœ
	for (int i = 0, n = effect.size(); i != n; ++i) {
		delete effect[i];
		effect[i] = nullptr;	// ˆê‰ƒkƒ‹ƒ|ƒCƒ“ƒ^‚ğ‚¢‚ê‚Æ‚­
	}
	if (grEffect.flag) {
		DeleteGraph(grEffect.gr);
	}	
}
// debug
void Skill::TestIni() {
	name = "‚±‚¤‚°‚«";

	effect.push_back(new SkillEffect_Attack());

	// “–‚½‚è”»’è”ÍˆÍi3~3j
	collision.resize(3);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// ‘Sƒ}ƒXfalse‚Å‰Šú‰»

	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = false;
		}
	}
	// “–‚½‚è”»’è
	collision[0][2] = true;
	collision[1][1] = true;
	collision[2][0] = true;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------- ¬—‚Ìs“®—pƒXƒLƒ‹ --------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Confusion::Skill_Confusion(int arWidth, int arHeight, vector<vector<__int8>> arCollision) {
	type = eSkillType::Confusion;
	//id = -1;	// ID
	//name = "¬—";	// –¼‘O
	speed = 1;	// •â³‘¬“x
	selectFlag = true;	// UŒ‚”ÍˆÍ‘I‘ğƒtƒ‰ƒO
	//selectTarget = true;	// UŒ‚”ÍˆÍ‘I‘ğ‘ÎÛ
	battleFlag = true;
	fieldFlag = false;
	width = arWidth;	// UŒ‚”ÍˆÍ ‰¡•
	height = arHeight;	//          c•
	x = 0;
	y = 0;

	// UŒ‚ƒXƒLƒ‹ƒGƒtƒFƒNƒg
	//effect.push_back(new SkillEffect_Attack("0,4,0,1,0,2,0.5,0.5,1,1", 0));
	effect.push_back(new SkillEffect_Attack());


	// “–‚½‚è”»’è
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
/* --------------------------------------------------- ‚È‚É‚à‚µ‚È‚¢ ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_None::Skill_None() {
	type = eSkillType::None;
	speed = 1;	// •â³‘¬“x
	selectFlag = false;	// UŒ‚”ÍˆÍ‘I‘ğƒtƒ‰ƒO
	battleFlag = true;
	fieldFlag = false;
}
Skill_None::~Skill_None() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- –hŒä ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Defence::Skill_Defence() {
	type = eSkillType::Defence;
	speed = 10;	// •â³‘¬“x
	selectFlag = false;	// UŒ‚”ÍˆÍ‘I‘ğƒtƒ‰ƒO
	battleFlag = true;
	fieldFlag = false;
}
Skill_Defence::~Skill_Defence() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- “¦‘– ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Escape::Skill_Escape() {
	type = eSkillType::Escape;
	speed = 1;	// •â³‘¬“x
	selectFlag = false;	// UŒ‚”ÍˆÍ‘I‘ğƒtƒ‰ƒO
	battleFlag = true;
	fieldFlag = false;
}
Skill_Escape::~Skill_Escape() {
}

//#################################################################################################################//
//#################################################################################################################//
/*################################################### ƒXƒLƒ‹Œø‰Ê ##################################################*/
//#################################################################################################################//
//#################################################################################################################//

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------ UŒ‚ -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬—‚ÌUŒ‚—p‰Šú‰»
SkillEffect_Attack::SkillEffect_Attack() {
	type = eSkillEffect::Attack;

	energyConv.type = eEnergy::None;	// •ÏŠ·‚È‚µ
	energyConv.convType = eConvType::None;
	energyConv.direction = true;
	energyConv.rate = 0;
	energyConv.correction = 2;		// STR•â³
	attackData.hit = 0.5;			// –½’†”{—¦0.5”{
	attackData.damageRate = 0.5;	// ƒ_ƒ[ƒW”{—¦0.5
	attackData.times_min = 1;		// 1hit
	attackData.times_max = 1;


}
SkillEffect_Attack::SkillEffect_Attack(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Attack;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yƒGƒlƒ‹ƒM[‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 3y•ÏŠ·‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 4y•ÏŠ·•ûŒüzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 5y•ÏŠ·—Êzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6y•ÏŠ·•â³zæ“¾
	getline(L_stream, L_token, ',');
	energyConv.correction = boost::lexical_cast<int>(L_token);
	// 7y–½’†•â³zæ“¾
	getline(L_stream, L_token, ',');
	attackData.hit = boost::lexical_cast<float>(L_token);
	// 8yƒ_ƒ[ƒW”{—¦zæ“¾
	getline(L_stream, L_token, ',');
	attackData.damageRate = boost::lexical_cast<float>(L_token);
	// 9yUŒ‚‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 10yÅ¬UŒ‚‰ñ”zæ“¾
	getline(L_stream, L_token, ',');
	attackData.times_min = boost::lexical_cast<int>(L_token);
	// 11yÅ‘åUŒ‚‰ñ”zæ“¾
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
/* ------------------------------------------------------ ‰ñ•œ -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Heal::SkillEffect_Heal() {
	type = eSkillEffect::Heal;

	// debug
	TestIni();
}
SkillEffect_Heal::SkillEffect_Heal(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Heal;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2y‰ñ•œƒ^ƒCƒvzæ“¾
	getline(L_stream, L_token, ',');
	healData.type = (eHealType)boost::lexical_cast<int>(L_token);
	// 3y”{—¦zæ“¾
	getline(L_stream, L_token, ',');
	healData.rate = boost::lexical_cast<float>(L_token);
	// 4yUŒ‚‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5y¶–½ƒGƒlƒ‹ƒM[‚Ì•ÏŠ·—Êzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6y•ÏŠ·•â³zæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);

	// ‚»‚Ì‘¼•ÏŠ·î•ñ
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
	// ‘ÎÛ‚ª€‚ñ‚Å‚½‚ç‰ñ•œ‚µ‚È‚¢
	if (target->GetAlive() != eAlive::alive) return;
	
	switch (healData.type) {
	case eHealType::HP:// yHP‰ñ•œz
	{
		// ‘ÎÛ‰ñ•œi•â³72j
		switch (energyConv.convType)
		{
		case eConvType::Self:
		case eConvType::Field:
		case eConvType::Target:
			// •K—v‚ÈMUE’l‚Ìn% 
			target->Recovery((totalUseEnergy * energyConv.rate) * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			attacker->DecreaseMP((int)
				(energyConv, totalUseEnergy));	// MPŒ¸­
			break;
		case eConvType::None:
			// •ÏŠ·‘ÎÛ‚ªy‘ÎÛ‚È‚µz‚Ìê‡‚Í‚»‚Ì‚Ü‚Ü•ÏŠ·—Ê‚É
			target->Recovery(energyConv.rate * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			break;
		default:
			break;
		}
		break;
	}
	case eHealType::MP:// yMP‰ñ•œz
		break;
	case eHealType::BadStatus:// y‘Só‘ÔˆÙíz
		target->HealBadStatus();
		break;
	case eHealType::Poison:// y“Åz
		target->HealBadStatus(eBadStatus::Poison);
		break;
	case eHealType::Paralysis:// y–ƒáƒz
		target->HealBadStatus(eBadStatus::Paralysis);
		break;
	case eHealType::Sleep:// y‡–°z
		target->HealBadStatus(eBadStatus::Sleep);
		break;
	case eHealType::Confusion:// y¬—z
		target->HealBadStatus(eBadStatus::Confusion);
		break;
	case eHealType::Blind:// yˆÃˆÅz
		target->HealBadStatus(eBadStatus::Blind);
		break;
	case eHealType::Weakness:// yŠãz
		target->HealBadStatus(eBadStatus::Weakness);
		break;
	case eHealType::Debuff:// y‘Sƒfƒoƒtz
		break;
	}
}
//debug
void SkillEffect_Heal::TestIni() {
	healData.type = eHealType::HP;
	healData.rate = 1;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- ó‘ÔˆÙí ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_BadStatus::SkillEffect_BadStatus() {
	type = eSkillEffect::BadStatus;
}
SkillEffect_BadStatus::SkillEffect_BadStatus(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::BadStatus;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2y–½’†—Ízæ“¾
	getline(L_stream, L_token, ',');
	badStatusData.hit = boost::lexical_cast<int>(L_token);
	// 3yˆĞ—Ízæ“¾
	getline(L_stream, L_token, ',');
	badStatusData.power = boost::lexical_cast<int>(L_token);
	// 4yUŒ‚‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5yó‘ÔˆÙí‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	badStatusData.type = (eBadStatus)boost::lexical_cast<int>(L_token);
	// 6yƒGƒlƒ‹ƒM[‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 7y•ÏŠ·‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 8y•ÏŠ·•ûŒüzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 9y•ÏŠ·—Êzæ“¾
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
/* -------------------------------------------------- ƒoƒtEƒfƒoƒt -------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Buff::SkillEffect_Buff() {
	type = eSkillEffect::Buff;
}
SkillEffect_Buff::SkillEffect_Buff(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Buff;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2y–½’†—Ízæ“¾
	getline(L_stream, L_token, ',');
	buffData.hit = boost::lexical_cast<float>(L_token);
	// 3y‘±ƒ^[ƒ“zæ“¾
	getline(L_stream, L_token, ',');
	buffData.trun = boost::lexical_cast<int>(L_token);
	// 4yƒoƒt‚©ƒfƒoƒt‚©zæ“¾
	getline(L_stream, L_token, ',');
	buffData.dir = L_token == "1";
	// 5yUŒ‚‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 6yƒoƒt‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	buffData.type = (eBuff)boost::lexical_cast<int>(L_token);
	// 7yƒGƒlƒ‹ƒM[‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 8y•ÏŠ·‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 9y•ÏŠ·•ûŒüzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 10y•ÏŠ·—Êzæ“¾
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
/* ------------------------------------------------- ‘¼‚ÌƒXƒLƒ‹”­“® ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill() {
	type = eSkillEffect::AnotherSkill;

	// debug
	TestIni();
}
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::AnotherSkill;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yƒXƒLƒ‹ƒf[ƒ^zæ“¾
	getline(L_stream, L_token, ',');
	anotherSkillData.skill = new Skill(boost::lexical_cast<int>(L_token));
	// 3y”­“®—¦zæ“¾
	getline(L_stream, L_token, ',');
	anotherSkillData.invocation = boost::lexical_cast<int>(L_token);
	// 4y‘¬Uƒtƒ‰ƒOzæ“¾
	getline(L_stream, L_token, ',');
	anotherSkillData.fastFlag = L_token == "1";
	

	// ˆê‰ƒGƒlƒ‹ƒM[•ÏŠ·ƒf[ƒ^‚ğ‰Šú‰»
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
/* ------------------------------------------------------ ‘h¶ -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Resurrection::SkillEffect_Resurrection() {
	type = eSkillEffect::Resurrection;

	// debug
	TestIni();
}
SkillEffect_Resurrection::SkillEffect_Resurrection(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Resurrection;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2y¬Œ÷—¦zæ“¾
	getline(L_stream, L_token, ',');
	resurrectionData.success = boost::lexical_cast<int>(L_token);
	// 3y”{—¦zæ“¾
	getline(L_stream, L_token, ',');
	resurrectionData.rate = boost::lexical_cast<float>(L_token);
	// 4y‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5y¶–½ƒGƒlƒ‹ƒM[‚Ì•ÏŠ·—Êzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6y•ÏŠ·•â³zæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);


	// ‚»‚Ì‘¼•ÏŠ·î•ñ
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
	// ‘ÎÛ‚ª€‚ñ‚Å‚¢‚È‚¯‚ê‚Î”»’è‚µ‚È‚¢
	if (target->GetAlive() != eAlive::dead) return;

	// ‘ÎÛ‘h¶i•â³2j
	int healValue = target->Resurrection(GetConvValue(energyConv, totalUseEnergy) * resurrectionData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 2);
}
//debug
void SkillEffect_Resurrection::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------- ƒwƒCƒg‘Œ¸ --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Hate::SkillEffect_Hate() {
	type = eSkillEffect::Hate;

	// debug
	TestIni();
}
SkillEffect_Hate::SkillEffect_Hate(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Hate;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yƒwƒCƒg‘Œ¸’lzæ“¾
	getline(L_stream, L_token, ',');
	hateData.value = boost::lexical_cast<int>(L_token);
	// 3yƒwƒCƒg‘Œ¸”{—¦zæ“¾
	getline(L_stream, L_token, ',');
	hateData.rate = boost::lexical_cast<float>(L_token);
	// 4yƒGƒlƒ‹ƒM[‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 5y•ÏŠ·‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 6y•ÏŠ·•ûŒüzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 7y•ÏŠ·—Êzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// ‚»‚Ì‘¼•ÏŠ·î•ñ
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
/* ---------------------------------------------------- ƒ`ƒƒ[ƒW ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Charge::SkillEffect_Charge() {
	type = eSkillEffect::Charge;

	// debug
	TestIni();
}
SkillEffect_Charge::SkillEffect_Charge(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Charge;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yí—Şzæ“¾
	getline(L_stream, L_token, ',');
	chargeData.type = (eCharge)boost::lexical_cast<int>(L_token);
	// 3y”{—¦zæ“¾
	getline(L_stream, L_token, ',');
	chargeData.rate = boost::lexical_cast<float>(L_token);
	// 4y”­“®ƒ^[ƒ“zæ“¾
	getline(L_stream, L_token, ',');
	chargeData.turn = boost::lexical_cast<int>(L_token);
	// 5yƒGƒlƒ‹ƒM[‚Ìí—Şzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 6y•ÏŠ·‘ÎÛzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 7y•ÏŠ·•ûŒüzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 8y•ÏŠ·—Êzæ“¾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// ‚»‚Ì‘¼•ÏŠ·î•ñ
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
/* --------------------------------------------------- ƒ‚[ƒh•ÏX --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Mode::SkillEffect_Mode() {
	type = eSkillEffect::Mode;

	// debug
	TestIni();
}
SkillEffect_Mode::SkillEffect_Mode(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Mode;

	//ƒ[ƒh—p•Ï”
	std::istringstream L_stream(line);
	string L_token;

	// 1yƒf[ƒ^‚Ìí—Şz”jŠü
	getline(L_stream, L_token, ',');
	// 2yƒ‚[ƒh”Ô†zæ“¾
	getline(L_stream, L_token, ',');
	modeData.modeNum = boost::lexical_cast<int>(L_token);
	
	// ˆê‰ƒGƒlƒ‹ƒM[•ÏŠ·ƒf[ƒ^‚ğ‰Šú‰»
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
