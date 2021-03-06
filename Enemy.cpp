#include"Enemy.h"

vector<unsigned int> Enemy::deathList;	// 敵死亡リスト（敵ID）
int Enemy::gr_HP;

Enemy::Enemy(int x, int y, string name) : Character() {

	this->x = x;	// 座標
	this->y = y;

	// debug
	TestIni(name);
}

Enemy::Enemy(int x, int y, int id, unsigned int& turnNum) : Character() {
	this->x = x;	// 座標
	this->y = y;
	this->id = id;	// id
	
	nowMode = 0;	// 現在のモード

	// 画像読み込み
	string str = "img\\enemy\\" + to_string(id) + ".png";	// アドレス生成
	char* cstr = new char[str.size() + 1];			// メモリ確保
	strcpy_s(cstr, str.size() + 1, str.c_str());	// コピー
	this->gr = LoadGraph(cstr);	

}

Enemy::~Enemy() {

}

void Enemy::InitBattle() {
	gr_HP = LoadGraph("img\\battle\\HPBar.png");	// HPバー
}
void Enemy::FinaBattle() {
	DeleteGraph(gr_HP);		// HPバー
}

// データ読み込み
void Enemy::LoadData(const unsigned int& turnNum, const std::vector<Enemy*>& enemy) {
	// 【ファイル読み込み用変数】
	ifstream L_File;	// 読み込むファイル
	string L_Line;		// 読み込んだ行（1行）
	int L_Count = 0;	// ノード読み込み用（何行目を読み込んでいるか）
	string L_token;		// 1行を,で分割する際に使う
	unsigned int L_Mode = 0;

	nowMode = 0;	// 現在のモード

	// 読み込むファイルの指定
	L_File.open("enemydata\\" + to_string(this->id) + ".csv");	// ファイルオープン

	if (L_File.fail()) {	// ファイル読み込み
		name = "読み込めてないよ";
		TestIni(name);
	}
	else {
		// 1行目【基本情報】
		getline(L_File, L_Line);
		// 読み込んだ行をコピー
		std::istringstream L_stream(L_Line);

		//////////////////////////////////////
		//////////// 【基本情報】//////////////
		//////////////////////////////////////
		// 1【id】取得
		getline(L_stream, L_token, ',');
		this->id = id;
		// 2【名前】取得
		getline(L_stream, L_token, ',');
		name = L_token;
		// 3【固有変数の数】取得
		getline(L_stream, L_token, ',');
		//###############################
		// 4【モード数】取得
		getline(L_stream, L_token, ',');
		//###############################
		// 5【横幅】取得
		getline(L_stream, L_token, ',');
		width = boost::lexical_cast<int>(L_token);
		// 6【縦幅】取得
		getline(L_stream, L_token, ',');
		height = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// 【ステータス】////////////
		//////////////////////////////////////
		// 7【HP】取得
		getline(L_stream, L_token, ',');
		battlestatus.maxHP = battlestatus.HP = boost::lexical_cast<int>(L_token);
		// 8【MP】取得
		getline(L_stream, L_token, ',');
		battlestatus.maxMP = battlestatus.MP = boost::lexical_cast<int>(L_token);
		// 9【STR】取得
		getline(L_stream, L_token, ',');
		battlestatus.STR = boost::lexical_cast<int>(L_token);
		// 10【DEX】取得
		getline(L_stream, L_token, ',');
		battlestatus.DEX = boost::lexical_cast<int>(L_token);
		// 11【AGI】取得
		getline(L_stream, L_token, ',');
		battlestatus.AGI = boost::lexical_cast<int>(L_token);
		// 12【LUC】取得
		getline(L_stream, L_token, ',');
		battlestatus.LUC = boost::lexical_cast<int>(L_token);
		// 13【MUE】取得
		getline(L_stream, L_token, ',');
		battlestatus.MUE = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		////////////// 【防御力】//////////////
		//////////////////////////////////////
		// 14【物理防御】取得
		getline(L_stream, L_token, ',');
		battlestatus.DEF_PHYS = boost::lexical_cast<int>(L_token);
		// 15【熱防御】取得
		getline(L_stream, L_token, ',');
		battlestatus.DEF_HEAT = boost::lexical_cast<int>(L_token);
		// 16【冷気防御】取得
		getline(L_stream, L_token, ',');
		battlestatus.DEF_COLD = boost::lexical_cast<int>(L_token);
		// 17【電気防御】取得
		getline(L_stream, L_token, ',');
		battlestatus.DEF_ELEC = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		/////////// 【状態異常耐性】///////////
		//////////////////////////////////////
		// 18【毒】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[0] = boost::lexical_cast<int>(L_token);
		// 19【麻痺】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[1] = boost::lexical_cast<int>(L_token);
		// 20【睡眠】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[2] = boost::lexical_cast<int>(L_token);
		// 21【混乱】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[3] = boost::lexical_cast<int>(L_token);
		// 22【暗闇】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[4] = boost::lexical_cast<int>(L_token);
		// 23【衰弱】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[5] = boost::lexical_cast<int>(L_token);
		// 24【スタン】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[6] = boost::lexical_cast<int>(L_token);
		// 25【即死】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[7] = boost::lexical_cast<int>(L_token);
		// 26【消滅】取得
		getline(L_stream, L_token, ',');
		DEF_badStatus[8] = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		///////////// 【変換効率】/////////////
		//////////////////////////////////////
		// 27【熱+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Heat] = boost::lexical_cast<float>(L_token);
		// 28【熱-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Heat] = boost::lexical_cast<float>(L_token);
		// 29【電気+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Elec] = boost::lexical_cast<float>(L_token);
		// 30【電気-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Elec] = boost::lexical_cast<float>(L_token);
		// 31【光+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Shine] = boost::lexical_cast<float>(L_token);
		// 32【光-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Shine] = boost::lexical_cast<float>(L_token);
		// 33【音+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Sound] = boost::lexical_cast<float>(L_token);
		// 34【音-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Sound] = boost::lexical_cast<float>(L_token);
		// 35【運動+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Motion] = boost::lexical_cast<float>(L_token);
		// 36【運動-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Motion] = boost::lexical_cast<float>(L_token);
		// 37【重力+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Gravity] = boost::lexical_cast<float>(L_token);
		// 38【重力-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Gravity] = boost::lexical_cast<float>(L_token);
		// 39【生命+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Life] = boost::lexical_cast<float>(L_token);
		// 40【生命-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Life] = boost::lexical_cast<float>(L_token);
		// 41【静止+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Static] = boost::lexical_cast<float>(L_token);
		// 42【静止-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Static] = boost::lexical_cast<float>(L_token);
		// 43【核+】取得
		getline(L_stream, L_token, ',');
		efficiency[1][Nucleus] = boost::lexical_cast<float>(L_token);
		// 44【核-】取得
		getline(L_stream, L_token, ',');
		efficiency[0][Nucleus] = boost::lexical_cast<float>(L_token);
		//////////////////////////////////////
		////////////// 【戦利品】//////////////
		//////////////////////////////////////
		// 45【経験値】取得
		getline(L_stream, L_token, ',');
		exp = boost::lexical_cast<int>(L_token);
		// 46【金】取得
		getline(L_stream, L_token, ',');
		money = boost::lexical_cast<int>(L_token);
		// 47【ドロップアイテムID】取得
		getline(L_stream, L_token, ',');
		dropItem = boost::lexical_cast<int>(L_token);
		// 48【ドロップ率】取得
		getline(L_stream, L_token, ',');
		dropRate = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// 【当たり判定】////////////
		//////////////////////////////////////
		// 当たり判定範囲指定
		collision.resize(height);
		for (int i = 0; i < height; i++) {
			collision[i].resize(width);
		}
		// 当たり判定読み込み
		for (int i = 0; i < height && getline(L_File, L_Line); i++) {
			std::istringstream L_stream(L_Line);
			for (int j = 0; j < width && getline(L_stream, L_token, ','); j++) {
				collision[i][j] = boost::lexical_cast<int>(L_token);
			}
		}


		//////////////////////////////////////
		////////////// 【スキル】//////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// 一行ずつ読み込み
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);
			// 1列目をロードして判定
			getline(L_stream, L_token, ',');

			switch (boost::lexical_cast<int>(L_token)) {
			case 0:// 【モード】
				// 2【モード数】取得
				getline(L_stream, L_token, ',');
				L_Mode = boost::lexical_cast<int>(L_token);
				// モード数が少なければ
				if ((unsigned)L_Mode >= modeTerm.size()) {
					// モード数を増やす
					modeTerm.resize(L_Mode + 1);
				}
				// 3【モードの移行条件】取得
				getline(L_stream, L_token, ',');
				switch ((eModeTerm)boost::lexical_cast<int>(L_token)) {
				case eModeTerm::None:// 【条件なし】
					modeTerm[L_Mode] = new ModeTerm_None(nowMode, L_Mode);
					break;
				case eModeTerm::Hp:// 【ＨＰ判定】
					modeTerm[L_Mode] = new ModeTerm_HP(L_Line, battlestatus.HP, battlestatus.maxHP);
					break;
				case eModeTerm::Turn:// 【ターン判定（固定数）】
					modeTerm[L_Mode] = new ModeTerm_Turn(L_Line, turnNum);
					break;
				case eModeTerm::EnemyNum:// 【敵の数】
					modeTerm[L_Mode] = new ModeTerm_EnemyNum(L_Line, enemy);
					break;
				case eModeTerm::BadStatus:// 【状態異常】
					modeTerm[L_Mode] = new ModeTerm_BadStatus(L_Line, badStatus);
					break;
				case eModeTerm::Buff:
					break;
				default:
					break;
				}

				break;
			case 1:// 【スキル】
				// 2【スキルID】取得
				getline(L_stream, L_token, ',');
				modeTerm[L_Mode]->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			case 2:// 【条件発動スキルの条件】
				// 2【条件の種類】
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
			case 3:// 【条件発動スキルのスキル】
				// 2【スキルID】取得
				getline(L_stream, L_token, ',');
				termSkill.back()->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			}
		}
	}

	// ファイルクローズ
	L_File.close();

	// 【特殊スキル】
	// 混乱時の攻撃用スキル
	confusionSkill = new Skill_Confusion(width, height, collision);
	
}

// 当たり判定の作成
void Enemy::SetColision(vector<vector<Character*>>& collField) {
	// 当たり判定
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = this;
			}
		}
	}
}

// 死亡処理
void Enemy::DeleteColision(vector<vector<Character*>>& collField) {
	// 当たり判定
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = nullptr;
			}
		}
	}
}

// 発動スキルの決定
void Enemy::DecisionSkill(vector<int> playerHate, vector<Enemy*> enemy) {
	if (aliveState != eAlive::alive) return;	// 死んでたら行動しない

	// 【モードの変更】
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		if (modeTerm[i]->Check()) {
			nowMode = i;
			break;
		}
	}

	// 【スキルの確定】
	Skill* skillData = modeTerm[nowMode]->GetSkill();
	skillOrder->Add(this, skillData, skillData->totalUseEnergy);

	// 【スキル座標の確定】
	if (skillData->selectFlag && GetCharge(eCharge::Range) == 1) {
		if (skillData->selectTarget) {
			// 【プレイヤー狙い】
			int targetNum = 0;	// プレイヤーのナンバー　前からｎ番目
			int HateTotal = 0;	// ヘイト値の合計
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

			// x2y3の中のどれかのマスを選択
			skillData->x = ( 1 + targetNum * 3 - (int)(skillData->width * 0.5) + GetRand(1));
			skillData->y = ( 1	- (int)(skillData->height * 0.5) + GetRand(2));
		}
		else {
			// 【敵狙い】
			int targetNum = 0;

			// 蘇生スキルかを判定
			bool resFlag = false;
			for (int i = 0, n = (int)skillData->effect.size(); i < n; ++i) {
				// 蘇生スキル && 対象が選択範囲
				if (skillData->effect[i]->type == eSkillEffect::Resurrection && skillData->effect[i]->targetType == eSkillTarget::Select) {
					resFlag = true;
				}
			}
			if (resFlag) {
				// 蘇生スキル
				if (deathList.empty()) {
					// 死亡リストが空なら敵全体からランダムに選択
					targetNum = GetRand(enemy.size() - 1);
				}
				else {
					// 死亡リストからランダムに選択
					targetNum = deathList[GetRand(deathList.size() - 1)];
				}
			}
			else {
				// その他スキル
				// 生存者からランダムに選択
				do {
					targetNum = GetRand(enemy.size() - 1);	// 敵からランダムに抽出
				} while (enemy[targetNum]->GetAlive() != eAlive::alive);	// 対象が生きていなければ再抽選
			}

			// 敵の縦横幅からランダムにマスを選択
			skillData->x = (enemy[targetNum]->GetX() - (int)(skillData->width * 0.5) + GetRand(enemy[targetNum]->GetWidth() - 1));
			skillData->y = (enemy[targetNum]->GetY() - (int)(skillData->height * 0.5) + GetRand(enemy[targetNum]->GetHeight() - 1));
		}
	}
}

// モード変更
void Enemy::ModeChange(int modeNum) {
	// アウトオブレンジ対策
	if (modeNum < 0 || (unsigned)modeNum >= modeTerm.size()) return;

	nowMode = modeNum;
}

// スキルのメッセージ取得
void Enemy::GetEffectText(vector<string>& message) {
	// 【ダメージ】
	if (receiveSkill[(int)eSkillEffect::Attack]) {
		if (skillDamage > 0) {
			// ダメージを受けた
			message.push_back(name + "に" + to_string(skillDamage) + "のダメージ与えた");
			PlaySoundMem(se_Damage);	// ダメージse
		}
		else {
			// ダメージなし
			message.push_back(name + "にダメージを与えられなかった");
		}
		// 寝てたら起こす
		HealBadStatus(eBadStatus::Sleep);
	}
	// 【死亡】
	if (skillDeath) {
		battlestatus.HP = 0;
		aliveState = eAlive::dead;	// 生存状態を【死亡】に
		message.push_back(name + "を倒した");
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
	}

	// バフ
	std::array<string, 22> buffName{ "HP", "最大HP",	"MP", "最大MP",	"STR", "DEX", "AGI",  "LUC", "物理防御", "熱防御",  "冷気防御", "電気防御", "MUE", 	"毒耐性", "麻痺耐性", "睡眠耐性", "混乱耐性", "暗闇耐性", "衰弱耐性", "気絶耐性", "即死耐性", "消滅耐性" };
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		if (skillBuff[i]) {
			switch (skillBuffType[i]) {
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
// 描画
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
		// 【ダメージを受けた】
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

// 終了処理
void Enemy::Release() {
	// モード遷移条件クラスの解放
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		modeTerm[i]->Release();
		delete modeTerm[i];
		modeTerm[i] = nullptr;	// 一応ヌルポインタをいれとく
	}
	DeleteGraph(gr);	// 画像削除
}


// ID取得
int Enemy::GetID() {
	return id;
}
int Enemy::GetWidth() {		// 横幅
	return width;
}
int Enemy::GetHeight() {	// 縦幅
	return height;
}
int Enemy::GetX() {		// X座標
	return x;
}
int Enemy::GetY() {		// Y座標
	return y;
}

// 【戦利品】
// 経験値
unsigned int Enemy::GetEXP() {
	return exp;
}
// 金
unsigned int Enemy::GetMoney() {
	return money;
}
// ドロップアイテム
int Enemy::GetDropItem() {
	return dropItem;	
}
// ドロップ率
__int8 Enemy::GetDropRate() {
	return dropRate;
}

// debug
void Enemy::TestIni(string name) {
	// ステータス
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

	// 当たり判定範囲（3×2）
	collision.resize(2);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// 全マス0で初期化
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = 0;
		}
	}
	// 当たり判定凸
	collision[0][1] = 1;
	collision[1][0] = 1;
	collision[1][1] = 1;
	collision[1][2] = 1;

	modeTerm.push_back(new ModeTerm_None(nowMode, 0));
	modeTerm[0]->AddSkill(confusionSkill);

	
	// 状態異常
	//badStatus[(int)eBadStatus::Sleep].get = true;
}