#include"Enemy.h"

vector<unsigned int> Enemy::deathList;	// “G€–SƒŠƒXƒgi“GIDj
int Enemy::gr_HP;

Enemy::Enemy(int x, int y, string name) : Character() {

	this->x = x;	// À•W
	this->y = y;

	// debug
	TestIni(name);
}

Enemy::Enemy(int x, int y, int id, unsigned int& turnNum) : Character() {
	this->x = x;	// À•W
	this->y = y;
	this->id = id;	// id
	
	nowMode = 0;	// Œ»İ‚Ìƒ‚[ƒh

	// ‰æ‘œ“Ç‚İ‚İ
	string str = "img\\enemy\\" + to_string(id) + ".png";	// ƒAƒhƒŒƒX¶¬
	char* cstr = new char[str.size() + 1];			// ƒƒ‚ƒŠŠm•Û
	strcpy_s(cstr, str.size() + 1, str.c_str());	// ƒRƒs[
	this->gr = LoadGraph(cstr);	

}

Enemy::~Enemy() {

}

void Enemy::InitBattle() {
	gr_HP = LoadGraph("img\\battle\\HPBar.png");	// HPƒo[
}
void Enemy::FinaBattle() {
	DeleteGraph(gr_HP);		// HPƒo[
}

// ƒf[ƒ^“Ç‚İ‚İ
void Enemy::LoadData(const unsigned int& turnNum, const std::vector<Enemy*>& enemy) {
	// yƒtƒ@ƒCƒ‹“Ç‚İ‚İ—p•Ï”z
	ifstream L_File;	// “Ç‚İ‚Şƒtƒ@ƒCƒ‹
	string L_Line;		// “Ç‚İ‚ñ‚¾si1sj
	int L_Count = 0;	// ƒm[ƒh“Ç‚İ‚İ—pi‰½s–Ú‚ğ“Ç‚İ‚ñ‚Å‚¢‚é‚©j
	string L_token;		// 1s‚ğ,‚Å•ªŠ„‚·‚éÛ‚Ég‚¤
	unsigned int L_Mode = 0;

	nowMode = 0;	// Œ»İ‚Ìƒ‚[ƒh

	// “Ç‚İ‚Şƒtƒ@ƒCƒ‹‚Ìw’è
	L_File.open("enemydata\\" + to_string(this->id) + ".csv");	// ƒtƒ@ƒCƒ‹ƒI[ƒvƒ“

	if (L_File.fail()) {	// ƒtƒ@ƒCƒ‹“Ç‚İ‚İ
		name = "“Ç‚İ‚ß‚Ä‚È‚¢‚æ";
		TestIni(name);
	}
	else {
		// 1s–ÚyŠî–{î•ñz
		getline(L_File, L_Line);
		// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
		std::istringstream L_stream(L_Line);

		//////////////////////////////////////
		//////////// yŠî–{î•ñz//////////////
		//////////////////////////////////////
		// 1yidzæ“¾
		getline(L_stream, L_token, ',');
		this->id = id;
		// 2y–¼‘Ozæ“¾
		getline(L_stream, L_token, ',');
		name = L_token;
		// 3yŒÅ—L•Ï”‚Ì”zæ“¾
		getline(L_stream, L_token, ',');
		//###############################
		// 4yƒ‚[ƒh”zæ“¾
		getline(L_stream, L_token, ',');
		//###############################
		// 5y‰¡•zæ“¾
		getline(L_stream, L_token, ',');
		width = boost::lexical_cast<int>(L_token);
		// 6yc•zæ“¾
		getline(L_stream, L_token, ',');
		height = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// yƒXƒe[ƒ^ƒXz////////////
		//////////////////////////////////////
		// 7yHPzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.maxHP = battlestatus.HP = boost::lexical_cast<int>(L_token);
		// 8yMPzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.maxMP = battlestatus.MP = boost::lexical_cast<int>(L_token);
		// 9ySTRzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.STR = boost::lexical_cast<int>(L_token);
		// 10yDEXzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.DEX = boost::lexical_cast<int>(L_token);
		// 11yAGIzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.AGI = boost::lexical_cast<int>(L_token);
		// 12yLUCzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.LUC = boost::lexical_cast<int>(L_token);
		// 13yMUEzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.MUE = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		////////////// y–hŒä—Íz//////////////
		//////////////////////////////////////
		// 14y•¨—–hŒäzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_PHYS = boost::lexical_cast<int>(L_token);
		// 15y”M–hŒäzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_HEAT = boost::lexical_cast<int>(L_token);
		// 16y—â‹C–hŒäzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_COLD = boost::lexical_cast<int>(L_token);
		// 17y“d‹C–hŒäzæ“¾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_ELEC = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		/////////// yó‘ÔˆÙí‘Ï«z///////////
		//////////////////////////////////////
		// 18y“Åzæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[0] = boost::lexical_cast<int>(L_token);
		// 19y–ƒáƒzæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[1] = boost::lexical_cast<int>(L_token);
		// 20y‡–°zæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[2] = boost::lexical_cast<int>(L_token);
		// 21y¬—zæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[3] = boost::lexical_cast<int>(L_token);
		// 22yˆÃˆÅzæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[4] = boost::lexical_cast<int>(L_token);
		// 23yŠãzæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[5] = boost::lexical_cast<int>(L_token);
		// 24yƒXƒ^ƒ“zæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[6] = boost::lexical_cast<int>(L_token);
		// 25y‘¦€zæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[7] = boost::lexical_cast<int>(L_token);
		// 26yÁ–Åzæ“¾
		getline(L_stream, L_token, ',');
		DEF_badStatus[8] = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		///////////// y•ÏŠ·Œø—¦z/////////////
		//////////////////////////////////////
		// 27y”M+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Heat] = boost::lexical_cast<float>(L_token);
		// 28y”M-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Heat] = boost::lexical_cast<float>(L_token);
		// 29y“d‹C+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Elec] = boost::lexical_cast<float>(L_token);
		// 30y“d‹C-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Elec] = boost::lexical_cast<float>(L_token);
		// 31yŒõ+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Shine] = boost::lexical_cast<float>(L_token);
		// 32yŒõ-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Shine] = boost::lexical_cast<float>(L_token);
		// 33y‰¹+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Sound] = boost::lexical_cast<float>(L_token);
		// 34y‰¹-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Sound] = boost::lexical_cast<float>(L_token);
		// 35y‰^“®+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Motion] = boost::lexical_cast<float>(L_token);
		// 36y‰^“®-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Motion] = boost::lexical_cast<float>(L_token);
		// 37yd—Í+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Gravity] = boost::lexical_cast<float>(L_token);
		// 38yd—Í-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Gravity] = boost::lexical_cast<float>(L_token);
		// 39y¶–½+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Life] = boost::lexical_cast<float>(L_token);
		// 40y¶–½-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Life] = boost::lexical_cast<float>(L_token);
		// 41yÃ~+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Static] = boost::lexical_cast<float>(L_token);
		// 42yÃ~-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Static] = boost::lexical_cast<float>(L_token);
		// 43yŠj+zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[1][Nucleus] = boost::lexical_cast<float>(L_token);
		// 44yŠj-zæ“¾
		getline(L_stream, L_token, ',');
		efficiency[0][Nucleus] = boost::lexical_cast<float>(L_token);
		//////////////////////////////////////
		////////////// yí—˜•iz//////////////
		//////////////////////////////////////
		// 45yŒoŒ±’lzæ“¾
		getline(L_stream, L_token, ',');
		exp = boost::lexical_cast<int>(L_token);
		// 46y‹àzæ“¾
		getline(L_stream, L_token, ',');
		money = boost::lexical_cast<int>(L_token);
		// 47yƒhƒƒbƒvƒAƒCƒeƒ€IDzæ“¾
		getline(L_stream, L_token, ',');
		dropItem = boost::lexical_cast<int>(L_token);
		// 48yƒhƒƒbƒv—¦zæ“¾
		getline(L_stream, L_token, ',');
		dropRate = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// y“–‚½‚è”»’èz////////////
		//////////////////////////////////////
		// “–‚½‚è”»’è”ÍˆÍw’è
		collision.resize(height);
		for (int i = 0; i < height; i++) {
			collision[i].resize(width);
		}
		// “–‚½‚è”»’è“Ç‚İ‚İ
		for (int i = 0; i < height && getline(L_File, L_Line); i++) {
			std::istringstream L_stream(L_Line);
			for (int j = 0; j < width && getline(L_stream, L_token, ','); j++) {
				collision[i][j] = boost::lexical_cast<int>(L_token);
			}
		}


		//////////////////////////////////////
		////////////// yƒXƒLƒ‹z//////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// ˆês‚¸‚Â“Ç‚İ‚İ
			// “Ç‚İ‚ñ‚¾s‚ğƒRƒs[
			std::istringstream L_stream(L_Line);
			// 1—ñ–Ú‚ğƒ[ƒh‚µ‚Ä”»’è
			getline(L_stream, L_token, ',');

			switch (boost::lexical_cast<int>(L_token)) {
			case 0:// yƒ‚[ƒhz
				// 2yƒ‚[ƒh”zæ“¾
				getline(L_stream, L_token, ',');
				L_Mode = boost::lexical_cast<int>(L_token);
				// ƒ‚[ƒh”‚ª­‚È‚¯‚ê‚Î
				if ((unsigned)L_Mode >= modeTerm.size()) {
					// ƒ‚[ƒh”‚ğ‘‚â‚·
					modeTerm.resize(L_Mode + 1);
				}
				// 3yƒ‚[ƒh‚ÌˆÚsğŒzæ“¾
				getline(L_stream, L_token, ',');
				switch ((eModeTerm)boost::lexical_cast<int>(L_token)) {
				case eModeTerm::None:// yğŒ‚È‚µz
					modeTerm[L_Mode] = new ModeTerm_None(nowMode, L_Mode);
					break;
				case eModeTerm::Hp:// y‚g‚o”»’èz
					modeTerm[L_Mode] = new ModeTerm_HP(L_Line, battlestatus.HP, battlestatus.maxHP);
					break;
				case eModeTerm::Turn:// yƒ^[ƒ“”»’èiŒÅ’è”jz
					modeTerm[L_Mode] = new ModeTerm_Turn(L_Line, turnNum);
					break;
				case eModeTerm::EnemyNum:// y“G‚Ì”z
					modeTerm[L_Mode] = new ModeTerm_EnemyNum(L_Line, enemy);
					break;
				case eModeTerm::BadStatus:// yó‘ÔˆÙíz
					modeTerm[L_Mode] = new ModeTerm_BadStatus(L_Line, badStatus);
					break;
				case eModeTerm::Buff:
					break;
				default:
					break;
				}

				break;
			case 1:// yƒXƒLƒ‹z
				// 2yƒXƒLƒ‹IDzæ“¾
				getline(L_stream, L_token, ',');
				modeTerm[L_Mode]->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			case 2:// yğŒ”­“®ƒXƒLƒ‹‚ÌğŒz
				// 2yğŒ‚Ìí—Şz
				getline(L_stream, L_token, ',');
				switch ((eTermSkill)boost::lexical_cast<int>(L_token)) {
				case eTermSkill::Always:
					termSkill.push_back(new TermSkill_Always(L_Line));
					break;
				case eTermSkill::Hp:
					break;
				case eTermSkill::Turn:
					break;
				case eTermSkill::EnemyNum:
					break;
				case eTermSkill::BadStatus:
					break;
				case eTermSkill::Buff:
					break;
				}
				break;
			case 3:// yğŒ”­“®ƒXƒLƒ‹‚ÌƒXƒLƒ‹z
				// 2yƒXƒLƒ‹IDzæ“¾
				getline(L_stream, L_token, ',');
				termSkill.back()->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			}
		}
	}

	// ƒtƒ@ƒCƒ‹ƒNƒ[ƒY
	L_File.close();

	// y“ÁêƒXƒLƒ‹z
	// ¬—‚ÌUŒ‚—pƒXƒLƒ‹
	confusionSkill = new Skill_Confusion(width, height, collision);
	
}

// “–‚½‚è”»’è‚Ìì¬
void Enemy::SetColision(vector<vector<Character*>>& collField) {
	// “–‚½‚è”»’è
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = this;
			}
		}
	}
}

// €–Sˆ—
void Enemy::DeleteColision(vector<vector<Character*>>& collField) {
	// “–‚½‚è”»’è
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = nullptr;
			}
		}
	}
}

// ”­“®ƒXƒLƒ‹‚ÌŒˆ’è
void Enemy::DecisionSkill(vector<int> playerHate, vector<Enemy*> enemy) {
	if (aliveState != eAlive::alive) return;	// €‚ñ‚Å‚½‚çs“®‚µ‚È‚¢

	// yƒ‚[ƒh‚Ì•ÏXz
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		if (modeTerm[i]->Check()) {
			nowMode = i;
			break;
		}
	}

	// yƒXƒLƒ‹‚ÌŠm’èz
	Skill* skillData = modeTerm[nowMode]->GetSkill();
	skillOrder->Add(this, skillData, skillData->totalUseEnergy);

	// yƒXƒLƒ‹À•W‚ÌŠm’èz
	if (skillData->selectFlag && GetCharge(eCharge::Range) == 1) {
		if (skillData->selectTarget) {
			// yƒvƒŒƒCƒ„[‘_‚¢z
			int targetNum = 0;	// ƒvƒŒƒCƒ„[‚Ìƒiƒ“ƒo[@‘O‚©‚ç‚”Ô–Ú
			int HateTotal = 0;	// ƒwƒCƒg’l‚Ì‡Œv
			int targetRand = 0;	

			for (int i = 0, n = playerHate.size(); i != n; ++i) {
				HateTotal += playerHate[i];
			}
			targetRand = GetRand(HateTotal);
			
			for (int i = 0, n = playerHate.size(), m = 0; i != n; ++i) {
				m += playerHate[i];
				if (targetRand <= m) {
					targetNum = i;
					break;
				}
			}

			// x2y3‚Ì’†‚Ì‚Ç‚ê‚©‚Ìƒ}ƒX‚ğ‘I‘ğ
			skillData->x = ( 1 + targetNum * 3 - (int)(skillData->width * 0.5) + GetRand(1));
			skillData->y = ( 1	- (int)(skillData->height * 0.5) + GetRand(2));
		}
		else {
			// y“G‘_‚¢z
			int targetNum = 0;

			// ‘h¶ƒXƒLƒ‹‚©‚ğ”»’è
			bool resFlag = false;
			for (int i = 0, n = (int)skillData->effect.size(); i < n; ++i) {
				// ‘h¶ƒXƒLƒ‹ && ‘ÎÛ‚ª‘I‘ğ”ÍˆÍ
				if (skillData->effect[i]->type == eSkillEffect::Resurrection && skillData->effect[i]->targetType == eSkillTarget::Select) {
					resFlag = true;
				}
			}
			if (resFlag) {
				// ‘h¶ƒXƒLƒ‹
				if (deathList.empty()) {
					// €–SƒŠƒXƒg‚ª‹ó‚È‚ç“G‘S‘Ì‚©‚çƒ‰ƒ“ƒ_ƒ€‚É‘I‘ğ
					targetNum = GetRand(enemy.size() - 1);
				}
				else {
					// €–SƒŠƒXƒg‚©‚çƒ‰ƒ“ƒ_ƒ€‚É‘I‘ğ
					targetNum = deathList[GetRand(deathList.size() - 1)];
				}
			}
			else {
				// ‚»‚Ì‘¼ƒXƒLƒ‹
				// ¶‘¶Ò‚©‚çƒ‰ƒ“ƒ_ƒ€‚É‘I‘ğ
				do {
					targetNum = GetRand(enemy.size() - 1);	// “G‚©‚çƒ‰ƒ“ƒ_ƒ€‚É’Šo
				} while (enemy[targetNum]->GetAlive() != eAlive::alive);	// ‘ÎÛ‚ª¶‚«‚Ä‚¢‚È‚¯‚ê‚ÎÄ’Š‘I
			}

			// “G‚Ìc‰¡•‚©‚çƒ‰ƒ“ƒ_ƒ€‚Éƒ}ƒX‚ğ‘I‘ğ
			skillData->x = (enemy[targetNum]->GetX() - (int)(skillData->width * 0.5) + GetRand(enemy[targetNum]->GetWidth() - 1));
			skillData->y = (enemy[targetNum]->GetY() - (int)(skillData->height * 0.5) + GetRand(enemy[targetNum]->GetHeight() - 1));
		}
	}
}

// ƒ‚[ƒh•ÏX
void Enemy::ModeChange(int modeNum) {
	// ƒAƒEƒgƒIƒuƒŒƒ“ƒW‘Îô
	if (modeNum < 0 || (unsigned)modeNum >= modeTerm.size()) return;

	nowMode = modeNum;
}

// ƒXƒLƒ‹‚ÌƒƒbƒZ[ƒWæ“¾
void Enemy::GetEffectText(vector<string>& message) {
	// yƒ_ƒ[ƒWz
	if (receiveSkill[(int)eSkillEffect::Attack]) {
		if (skillDamage > 0) {
			// ƒ_ƒ[ƒW‚ğó‚¯‚½
			message.push_back(name + "‚É" + to_string(skillDamage) + "‚Ìƒ_ƒ[ƒW—^‚¦‚½");
			PlaySoundMem(se_Damage);	// ƒ_ƒ[ƒWse
		}
		else {
			// ƒ_ƒ[ƒW‚È‚µ
			message.push_back(name + "‚Éƒ_ƒ[ƒW‚ğ—^‚¦‚ç‚ê‚È‚©‚Á‚½");
		}
		// Q‚Ä‚½‚ç‹N‚±‚·
		HealBadStatus(eBadStatus::Sleep);
	}
	// y€–Sz
	if (skillDeath) {
		battlestatus.HP = 0;
		aliveState = eAlive::dead;	// ¶‘¶ó‘Ô‚ğy€–Sz‚É
		message.push_back(name + "‚ğ“|‚µ‚½");
		return;	// €‚ñ‚¾‚ç‚±‚±‚ÅƒƒbƒZ[ƒWI—¹
	}
	// y‘h¶z
	if (skillResurrection) {
		aliveState = eAlive::alive;	// ¶‘¶ó‘Ô‚ğy¶‘¶z‚É
		message.push_back(name + "‚ª•œŠˆ‚µ‚½");
	}
	// yó‘ÔˆÙíz
	if (skillBadStatus[(int)eBadStatus::Poison]) {	// y“Åz
		message.push_back(name + "‚Í“Å‚ÉN‚³‚ê‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Paralysis]) {	// y–ƒáƒz
		message.push_back(name + "‚Íg‘Ì‚ª–ƒáƒ‚µ‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Sleep]) {	// y‡–°z
		message.push_back(name + "‚Í–°‚è‚É‚Â‚¢‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Confusion]) {	// y¬—z
		message.push_back(name + "‚Í³‹C‚ğ¸‚Á‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Blind]) {	// yˆÃˆÅz
		message.push_back(name + "‚Í‹ŠE‚ğ’D‚í‚ê‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Weakness]) {	// yŠãz
		message.push_back(name + "‚ÍŠãó‘Ô‚É‚È‚Á‚½");
		// HP > Å‘åHP‚È‚ç HP‚ğÅ‘åHP‚É‚·‚é
		if (battlestatus.HP > GetStatus(eStatus::maxHP)) {
			battlestatus.HP = GetStatus(eStatus::maxHP);
		}
	}
	if (skillBadStatus[(int)eBadStatus::Stun]) {	// yƒXƒ^ƒ“z
		message.push_back(name + "‚Í‹Câ‚µ‚½");
	}
	if (skillBadStatus[(int)eBadStatus::Death]) {	// y‘¦€z
		message.push_back(name + "‚Í‘¦€‚µ‚½");
		battlestatus.HP = 0;	// HP‚ğ‹­§“I‚É0‚É‚·‚é
		aliveState = eAlive::dead;	// ó‘Ô‚ğy€–Sz‚É‚·‚é
		skillDeath = true;	// ƒXƒLƒ‹‚Å€–S‚µ‚½ƒtƒ‰ƒO‚ğ—§‚Ä‚é
		return;	// €‚ñ‚¾‚ç‚±‚±‚ÅƒƒbƒZ[ƒWI—¹
	}
	if (skillBadStatus[(int)eBadStatus::Extinction]) {	// yÁ–Åz
		aliveState = eAlive::extinction;	// ó‘Ô‚ğyÁ–Åz‚É‚·‚é
		return;	// €‚ñ‚¾‚ç‚±‚±‚ÅƒƒbƒZ[ƒWI—¹
		//message.push_back(name + "");
	}
	// y‰ñ•œz
	if (receiveSkill[(int)eSkillEffect::Heal]) {
		if (skillHealType[(int)eHealType::HP]) {	// yHPz
			if (skillHeal_HP != -1) {
				message.push_back(name + "‚Í" + to_string(skillHeal_HP) + "‰ñ•œ‚µ‚½");
			}
			else {
				message.push_back(name + "‚Ì‚g‚o‚ª‘S‰õ‚µ‚½");
			}
		}
		if (skillHealType[(int)eHealType::MP]) {	// yMPz
			if (skillHeal_MP != -1) {
				message.push_back(name + "‚Ì‚l‚o‚ª" + to_string(skillHeal_MP) + "ã¸‚µ‚½");
			}
			else {
				message.push_back(name + "‚Ì‚l‚o‚ª‘S‰õ‚µ‚½");
			}
		}
		// y‘Só‘ÔˆÙízó‘ÔˆÙí‚²‚Æ‚É•W¦
		if (skillHealType[(int)eHealType::Poison]) {	// y“Åz
			message.push_back(name + "‚Ì“Å‚ª‰ñ•œ‚µ‚½");
		}
		if (skillHealType[(int)eHealType::Paralysis]) {	// y–ƒáƒz
			message.push_back(name + "‚Ì–ƒáƒ‚ª‰ñ•œ‚µ‚½");
		}
		if (skillHealType[(int)eHealType::Sleep]) {	// y‡–°z
			message.push_back(name + "‚Í–Ú‚ğŠo‚Ü‚µ‚½");
		}
		if (skillHealType[(int)eHealType::Confusion]) {	// y¬—z
			message.push_back(name + "‚Í³‹C‚É–ß‚Á‚½");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// yˆÃˆÅz
			message.push_back(name + "‚Ì‹ŠE‚ª–ß‚Á‚½");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// yŠãz
			message.push_back(name + "‚ÍŠã‚©‚ç‰ñ•œ‚µ‚½");
		}
	}

	// ƒoƒt
	std::array<string, 22> buffName{ "HP", "Å‘åHP",	"MP", "Å‘åMP",	"STR", "DEX", "AGI",  "LUC", "•¨—–hŒä", "”M–hŒä",  "—â‹C–hŒä", "“d‹C–hŒä", "MUE", 	"“Å‘Ï«", "–ƒáƒ‘Ï«", "‡–°‘Ï«", "¬—‘Ï«", "ˆÃˆÅ‘Ï«", "Šã‘Ï«", "‹Câ‘Ï«", "‘¦€‘Ï«", "Á–Å‘Ï«" };
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		if (skillBuff[i]) {
			switch (skillBuffType[i]) {
			case BuffEffect::Negate:
				message.push_back(name + "‚Ì" + buffName[i] + "‚ª‚à‚Æ‚É–ß‚Á‚½");
				break;
			case BuffEffect::Buff:
				message.push_back(name + "‚Ì" + buffName[i] + "‚ªã¸‚µ‚½");
				break;
			case BuffEffect::deBuff:
				message.push_back(name + "‚Ì" + buffName[i] + "‚ªŒ¸­‚µ‚½");
				break;
			}
		}
	}

	ReceiveSkillIni();	 // ƒXƒLƒ‹ƒtƒ‰ƒO‰Šú‰»

}
// •`‰æ
void Enemy::Draw(int fieldX, int fieldY, int cellsize) {
	
	if (aliveState == eAlive::extinction) return;

	if (aliveState == eAlive::dead) {
		if (deathDelay > 0) {
			DrawStretchGraph(fieldX + x * cellsize, fieldY + y * cellsize + deathDelay * 2, gr, cellsize * width, cellsize * height);
			--deathDelay;			
		}
		return;
	}
	else if (damageDelay > 0) {
		// yƒ_ƒ[ƒW‚ğó‚¯‚½z
		if (damageDelay & 4) {
			DrawStretchGraph(fieldX + x * cellsize + GetRand(-5, 5), fieldY + y * cellsize + GetRand(-5, 5), gr, cellsize * width, cellsize * height);
		}
		--damageDelay;
	}
	else {
		DrawStretchGraph(fieldX + x * cellsize, fieldY + y * cellsize, gr, cellsize * width, cellsize * height);
	}

	// HP
	//DrawString(WHITE, fieldX + x * cellsize, fieldY + y * cellsize - 32, name.c_str());
	DrawBox(fieldX + x * cellsize, fieldY + y * cellsize - 32, width * cellsize, 32, true);
	//DrawBox(fieldX + x * cellsize, fieldY + y * cellsize - 32, (width * cellsize) * (GetStatus(eStatus::HP) / GetStatus(eStatus::maxHP)), 32, true);
	DrawStretchGraph(fieldX + x * cellsize + 5, fieldY + y * cellsize - 27, gr_HP, (int)((width * cellsize - 10) * ((double)GetStatus(eStatus::HP) / (double)GetStatus(eStatus::maxHP))), 22);
}

// I—¹ˆ—
void Enemy::Release() {
	// ƒ‚[ƒh‘JˆÚğŒƒNƒ‰ƒX‚Ì‰ğ•ú
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		modeTerm[i]->Release();
		delete modeTerm[i];
		modeTerm[i] = nullptr;	// ˆê‰ƒkƒ‹ƒ|ƒCƒ“ƒ^‚ğ‚¢‚ê‚Æ‚­
	}
	DeleteGraph(gr);	// ‰æ‘œíœ
}


// IDæ“¾
int Enemy::GetID() {
	return id;
}
int Enemy::GetWidth() {		// ‰¡•
	return width;
}
int Enemy::GetHeight() {	// c•
	return height;
}
int Enemy::GetX() {		// XÀ•W
	return x;
}
int Enemy::GetY() {		// YÀ•W
	return y;
}

// yí—˜•iz
// ŒoŒ±’l
unsigned int Enemy::GetEXP() {
	return exp;
}
// ‹à
unsigned int Enemy::GetMoney() {
	return money;
}
// ƒhƒƒbƒvƒAƒCƒeƒ€
int Enemy::GetDropItem() {
	return dropItem;	
}
// ƒhƒƒbƒv—¦
__int8 Enemy::GetDropRate() {
	return dropRate;
}

// debug
void Enemy::TestIni(string name) {
	// ƒXƒe[ƒ^ƒX
	this->name = name;
	battlestatus.HP = battlestatus.maxHP = 100;
	battlestatus.MP = battlestatus.maxMP = 120;
	battlestatus.STR = 255;
	battlestatus.DEX = 10;
	battlestatus.AGI = 20;
	battlestatus.LUC = 40;
	battlestatus.DEF_PHYS = 50;
	battlestatus.DEF_HEAT = 60;
	battlestatus.DEF_COLD = 70;
	battlestatus.DEF_ELEC = 80;
	battlestatus.MUE = 100;

	// “–‚½‚è”»’è”ÍˆÍi3~2j
	collision.resize(2);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// ‘Sƒ}ƒX0‚Å‰Šú‰»
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = 0;
		}
	}
	// “–‚½‚è”»’è“Ê
	collision[0][1] = 1;
	collision[1][0] = 1;
	collision[1][1] = 1;
	collision[1][2] = 1;

	modeTerm.push_back(new ModeTerm_None(nowMode, 0));
	modeTerm[0]->AddSkill(confusionSkill);

	
	// ó‘ÔˆÙí
	//badStatus[(int)eBadStatus::Sleep].get = true;
}