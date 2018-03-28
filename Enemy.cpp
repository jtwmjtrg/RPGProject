#include"Enemy.h"

vector<unsigned int> Enemy::deathList;	// �G���S���X�g�i�GID�j
int Enemy::gr_HP;

Enemy::Enemy(int x, int y, string name) : Character() {

	this->x = x;	// ���W
	this->y = y;

	// debug
	TestIni(name);
}

Enemy::Enemy(int x, int y, int id, unsigned int& turnNum) : Character() {
	this->x = x;	// ���W
	this->y = y;
	this->id = id;	// id
	
	nowMode = 0;	// ���݂̃��[�h

	// �摜�ǂݍ���
	string str = "img\\enemy\\" + to_string(id) + ".png";	// �A�h���X����
	char* cstr = new char[str.size() + 1];			// �������m��
	strcpy_s(cstr, str.size() + 1, str.c_str());	// �R�s�[
	this->gr = LoadGraph(cstr);	

}

Enemy::~Enemy() {

}

void Enemy::InitBattle() {
	gr_HP = LoadGraph("img\\battle\\HPBar.png");	// HP�o�[
}
void Enemy::FinaBattle() {
	DeleteGraph(gr_HP);		// HP�o�[
}

// �f�[�^�ǂݍ���
void Enemy::LoadData(const unsigned int& turnNum, const std::vector<Enemy*>& enemy) {
	// �y�t�@�C���ǂݍ��ݗp�ϐ��z
	ifstream L_File;	// �ǂݍ��ރt�@�C��
	string L_Line;		// �ǂݍ��񂾍s�i1�s�j
	int L_Count = 0;	// �m�[�h�ǂݍ��ݗp�i���s�ڂ�ǂݍ���ł��邩�j
	string L_token;		// 1�s��,�ŕ�������ۂɎg��
	unsigned int L_Mode = 0;

	nowMode = 0;	// ���݂̃��[�h

	// �ǂݍ��ރt�@�C���̎w��
	L_File.open("enemydata\\" + to_string(this->id) + ".csv");	// �t�@�C���I�[�v��

	if (L_File.fail()) {	// �t�@�C���ǂݍ���
		name = "�ǂݍ��߂ĂȂ���";
		TestIni(name);
	}
	else {
		// 1�s�ځy��{���z
		getline(L_File, L_Line);
		// �ǂݍ��񂾍s���R�s�[
		std::istringstream L_stream(L_Line);

		//////////////////////////////////////
		//////////// �y��{���z//////////////
		//////////////////////////////////////
		// 1�yid�z�擾
		getline(L_stream, L_token, ',');
		this->id = id;
		// 2�y���O�z�擾
		getline(L_stream, L_token, ',');
		name = L_token;
		// 3�y�ŗL�ϐ��̐��z�擾
		getline(L_stream, L_token, ',');
		//###############################
		// 4�y���[�h���z�擾
		getline(L_stream, L_token, ',');
		//###############################
		// 5�y�����z�擾
		getline(L_stream, L_token, ',');
		width = boost::lexical_cast<int>(L_token);
		// 6�y�c���z�擾
		getline(L_stream, L_token, ',');
		height = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// �y�X�e�[�^�X�z////////////
		//////////////////////////////////////
		// 7�yHP�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.maxHP = battlestatus.HP = boost::lexical_cast<int>(L_token);
		// 8�yMP�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.maxMP = battlestatus.MP = boost::lexical_cast<int>(L_token);
		// 9�ySTR�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.STR = boost::lexical_cast<int>(L_token);
		// 10�yDEX�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.DEX = boost::lexical_cast<int>(L_token);
		// 11�yAGI�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.AGI = boost::lexical_cast<int>(L_token);
		// 12�yLUC�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.LUC = boost::lexical_cast<int>(L_token);
		// 13�yMUE�z�擾
		getline(L_stream, L_token, ',');
		battlestatus.MUE = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		////////////// �y�h��́z//////////////
		//////////////////////////////////////
		// 14�y�����h��z�擾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_PHYS = boost::lexical_cast<int>(L_token);
		// 15�y�M�h��z�擾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_HEAT = boost::lexical_cast<int>(L_token);
		// 16�y��C�h��z�擾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_COLD = boost::lexical_cast<int>(L_token);
		// 17�y�d�C�h��z�擾
		getline(L_stream, L_token, ',');
		battlestatus.DEF_ELEC = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		/////////// �y��Ԉُ�ϐ��z///////////
		//////////////////////////////////////
		// 18�y�Łz�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[0] = boost::lexical_cast<int>(L_token);
		// 19�y��Ⴡz�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[1] = boost::lexical_cast<int>(L_token);
		// 20�y�����z�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[2] = boost::lexical_cast<int>(L_token);
		// 21�y�����z�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[3] = boost::lexical_cast<int>(L_token);
		// 22�y�ÈŁz�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[4] = boost::lexical_cast<int>(L_token);
		// 23�y����z�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[5] = boost::lexical_cast<int>(L_token);
		// 24�y�X�^���z�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[6] = boost::lexical_cast<int>(L_token);
		// 25�y�����z�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[7] = boost::lexical_cast<int>(L_token);
		// 26�y���Łz�擾
		getline(L_stream, L_token, ',');
		DEF_badStatus[8] = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		///////////// �y�ϊ������z/////////////
		//////////////////////////////////////
		// 27�y�M+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Heat] = boost::lexical_cast<float>(L_token);
		// 28�y�M-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Heat] = boost::lexical_cast<float>(L_token);
		// 29�y�d�C+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Elec] = boost::lexical_cast<float>(L_token);
		// 30�y�d�C-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Elec] = boost::lexical_cast<float>(L_token);
		// 31�y��+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Shine] = boost::lexical_cast<float>(L_token);
		// 32�y��-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Shine] = boost::lexical_cast<float>(L_token);
		// 33�y��+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Sound] = boost::lexical_cast<float>(L_token);
		// 34�y��-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Sound] = boost::lexical_cast<float>(L_token);
		// 35�y�^��+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Motion] = boost::lexical_cast<float>(L_token);
		// 36�y�^��-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Motion] = boost::lexical_cast<float>(L_token);
		// 37�y�d��+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Gravity] = boost::lexical_cast<float>(L_token);
		// 38�y�d��-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Gravity] = boost::lexical_cast<float>(L_token);
		// 39�y����+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Life] = boost::lexical_cast<float>(L_token);
		// 40�y����-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Life] = boost::lexical_cast<float>(L_token);
		// 41�y�Î~+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Static] = boost::lexical_cast<float>(L_token);
		// 42�y�Î~-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Static] = boost::lexical_cast<float>(L_token);
		// 43�y�j+�z�擾
		getline(L_stream, L_token, ',');
		efficiency[1][Nucleus] = boost::lexical_cast<float>(L_token);
		// 44�y�j-�z�擾
		getline(L_stream, L_token, ',');
		efficiency[0][Nucleus] = boost::lexical_cast<float>(L_token);
		//////////////////////////////////////
		////////////// �y�험�i�z//////////////
		//////////////////////////////////////
		// 45�y�o���l�z�擾
		getline(L_stream, L_token, ',');
		exp = boost::lexical_cast<int>(L_token);
		// 46�y���z�擾
		getline(L_stream, L_token, ',');
		money = boost::lexical_cast<int>(L_token);
		// 47�y�h���b�v�A�C�e��ID�z�擾
		getline(L_stream, L_token, ',');
		dropItem = boost::lexical_cast<int>(L_token);
		// 48�y�h���b�v���z�擾
		getline(L_stream, L_token, ',');
		dropRate = boost::lexical_cast<int>(L_token);
		//////////////////////////////////////
		//////////// �y�����蔻��z////////////
		//////////////////////////////////////
		// �����蔻��͈͎w��
		collision.resize(height);
		for (int i = 0; i < height; i++) {
			collision[i].resize(width);
		}
		// �����蔻��ǂݍ���
		for (int i = 0; i < height && getline(L_File, L_Line); i++) {
			std::istringstream L_stream(L_Line);
			for (int j = 0; j < width && getline(L_stream, L_token, ','); j++) {
				collision[i][j] = boost::lexical_cast<int>(L_token);
			}
		}


		//////////////////////////////////////
		////////////// �y�X�L���z//////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// ��s���ǂݍ���
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);
			// 1��ڂ����[�h���Ĕ���
			getline(L_stream, L_token, ',');

			switch (boost::lexical_cast<int>(L_token)) {
			case 0:// �y���[�h�z
				// 2�y���[�h���z�擾
				getline(L_stream, L_token, ',');
				L_Mode = boost::lexical_cast<int>(L_token);
				// ���[�h�������Ȃ����
				if ((unsigned)L_Mode >= modeTerm.size()) {
					// ���[�h���𑝂₷
					modeTerm.resize(L_Mode + 1);
				}
				// 3�y���[�h�̈ڍs�����z�擾
				getline(L_stream, L_token, ',');
				switch ((eModeTerm)boost::lexical_cast<int>(L_token)) {
				case eModeTerm::None:// �y�����Ȃ��z
					modeTerm[L_Mode] = new ModeTerm_None(nowMode, L_Mode);
					break;
				case eModeTerm::Hp:// �y�g�o����z
					modeTerm[L_Mode] = new ModeTerm_HP(L_Line, battlestatus.HP, battlestatus.maxHP);
					break;
				case eModeTerm::Turn:// �y�^�[������i�Œ萔�j�z
					modeTerm[L_Mode] = new ModeTerm_Turn(L_Line, turnNum);
					break;
				case eModeTerm::EnemyNum:// �y�G�̐��z
					modeTerm[L_Mode] = new ModeTerm_EnemyNum(L_Line, enemy);
					break;
				case eModeTerm::BadStatus:// �y��Ԉُ�z
					modeTerm[L_Mode] = new ModeTerm_BadStatus(L_Line, badStatus);
					break;
				case eModeTerm::Buff:
					break;
				default:
					break;
				}

				break;
			case 1:// �y�X�L���z
				// 2�y�X�L��ID�z�擾
				getline(L_stream, L_token, ',');
				modeTerm[L_Mode]->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			case 2:// �y���������X�L���̏����z
				// 2�y�����̎�ށz
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
			case 3:// �y���������X�L���̃X�L���z
				// 2�y�X�L��ID�z�擾
				getline(L_stream, L_token, ',');
				termSkill.back()->AddSkill(new Skill(boost::lexical_cast<int>(L_token)));
				break;
			}
		}
	}

	// �t�@�C���N���[�Y
	L_File.close();

	// �y����X�L���z
	// �������̍U���p�X�L��
	confusionSkill = new Skill_Confusion(width, height, collision);
	
}

// �����蔻��̍쐬
void Enemy::SetColision(vector<vector<Character*>>& collField) {
	// �����蔻��
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = this;
			}
		}
	}
}

// ���S����
void Enemy::DeleteColision(vector<vector<Character*>>& collField) {
	// �����蔻��
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + y][j + x] = nullptr;
			}
		}
	}
}

// �����X�L���̌���
void Enemy::DecisionSkill(vector<int> playerHate, vector<Enemy*> enemy) {
	if (aliveState != eAlive::alive) return;	// ����ł���s�����Ȃ�

	// �y���[�h�̕ύX�z
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		if (modeTerm[i]->Check()) {
			nowMode = i;
			break;
		}
	}

	// �y�X�L���̊m��z
	Skill* skillData = modeTerm[nowMode]->GetSkill();
	skillOrder->Add(this, skillData, skillData->totalUseEnergy);

	// �y�X�L�����W�̊m��z
	if (skillData->selectFlag && GetCharge(eCharge::Range) == 1) {
		if (skillData->selectTarget) {
			// �y�v���C���[�_���z
			int targetNum = 0;	// �v���C���[�̃i���o�[�@�O���炎�Ԗ�
			int HateTotal = 0;	// �w�C�g�l�̍��v
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

			// x2y3�̒��̂ǂꂩ�̃}�X��I��
			skillData->x = ( 1 + targetNum * 3 - (int)(skillData->width * 0.5) + GetRand(1));
			skillData->y = ( 1	- (int)(skillData->height * 0.5) + GetRand(2));
		}
		else {
			// �y�G�_���z
			int targetNum = 0;

			// �h���X�L�����𔻒�
			bool resFlag = false;
			for (int i = 0, n = (int)skillData->effect.size(); i < n; ++i) {
				// �h���X�L�� && �Ώۂ��I��͈�
				if (skillData->effect[i]->type == eSkillEffect::Resurrection && skillData->effect[i]->targetType == eSkillTarget::Select) {
					resFlag = true;
				}
			}
			if (resFlag) {
				// �h���X�L��
				if (deathList.empty()) {
					// ���S���X�g����Ȃ�G�S�̂��烉���_���ɑI��
					targetNum = GetRand(enemy.size() - 1);
				}
				else {
					// ���S���X�g���烉���_���ɑI��
					targetNum = deathList[GetRand(deathList.size() - 1)];
				}
			}
			else {
				// ���̑��X�L��
				// �����҂��烉���_���ɑI��
				do {
					targetNum = GetRand(enemy.size() - 1);	// �G���烉���_���ɒ��o
				} while (enemy[targetNum]->GetAlive() != eAlive::alive);	// �Ώۂ������Ă��Ȃ���΍Ē��I
			}

			// �G�̏c�������烉���_���Ƀ}�X��I��
			skillData->x = (enemy[targetNum]->GetX() - (int)(skillData->width * 0.5) + GetRand(enemy[targetNum]->GetWidth() - 1));
			skillData->y = (enemy[targetNum]->GetY() - (int)(skillData->height * 0.5) + GetRand(enemy[targetNum]->GetHeight() - 1));
		}
	}
}

// ���[�h�ύX
void Enemy::ModeChange(int modeNum) {
	// �A�E�g�I�u�����W�΍�
	if (modeNum < 0 || (unsigned)modeNum >= modeTerm.size()) return;

	nowMode = modeNum;
}

// �X�L���̃��b�Z�[�W�擾
void Enemy::GetEffectText(vector<string>& message) {
	// �y�_���[�W�z
	if (receiveSkill[(int)eSkillEffect::Attack]) {
		if (skillDamage > 0) {
			// �_���[�W���󂯂�
			message.push_back(name + "��" + to_string(skillDamage) + "�̃_���[�W�^����");
			PlaySoundMem(se_Damage);	// �_���[�Wse
		}
		else {
			// �_���[�W�Ȃ�
			message.push_back(name + "�Ƀ_���[�W��^�����Ȃ�����");
		}
		// �Q�Ă���N����
		HealBadStatus(eBadStatus::Sleep);
	}
	// �y���S�z
	if (skillDeath) {
		battlestatus.HP = 0;
		aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		message.push_back(name + "��|����");
		return;	// ���񂾂炱���Ń��b�Z�[�W�I��
	}
	// �y�h���z
	if (skillResurrection) {
		aliveState = eAlive::alive;	// ������Ԃ��y�����z��
		message.push_back(name + "����������");
	}
	// �y��Ԉُ�z
	if (skillBadStatus[(int)eBadStatus::Poison]) {	// �y�Łz
		message.push_back(name + "�͓łɐN���ꂽ");
	}
	if (skillBadStatus[(int)eBadStatus::Paralysis]) {	// �y��Ⴡz
		message.push_back(name + "�͐g�̂���Ⴢ���");
	}
	if (skillBadStatus[(int)eBadStatus::Sleep]) {	// �y�����z
		message.push_back(name + "�͖���ɂ���");
	}
	if (skillBadStatus[(int)eBadStatus::Confusion]) {	// �y�����z
		message.push_back(name + "�͐��C��������");
	}
	if (skillBadStatus[(int)eBadStatus::Blind]) {	// �y�ÈŁz
		message.push_back(name + "�͎��E��D��ꂽ");
	}
	if (skillBadStatus[(int)eBadStatus::Weakness]) {	// �y����z
		message.push_back(name + "�͐����ԂɂȂ���");
		// HP > �ő�HP�Ȃ� HP���ő�HP�ɂ���
		if (battlestatus.HP > GetStatus(eStatus::maxHP)) {
			battlestatus.HP = GetStatus(eStatus::maxHP);
		}
	}
	if (skillBadStatus[(int)eBadStatus::Stun]) {	// �y�X�^���z
		message.push_back(name + "�͋C�₵��");
	}
	if (skillBadStatus[(int)eBadStatus::Death]) {	// �y�����z
		message.push_back(name + "�͑�������");
		battlestatus.HP = 0;	// HP�������I��0�ɂ���
		aliveState = eAlive::dead;	// ��Ԃ��y���S�z�ɂ���
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
		return;	// ���񂾂炱���Ń��b�Z�[�W�I��
	}
	if (skillBadStatus[(int)eBadStatus::Extinction]) {	// �y���Łz
		aliveState = eAlive::extinction;	// ��Ԃ��y���Łz�ɂ���
		return;	// ���񂾂炱���Ń��b�Z�[�W�I��
		//message.push_back(name + "");
	}
	// �y�񕜁z
	if (receiveSkill[(int)eSkillEffect::Heal]) {
		if (skillHealType[(int)eHealType::HP]) {	// �yHP�z
			if (skillHeal_HP != -1) {
				message.push_back(name + "��" + to_string(skillHeal_HP) + "�񕜂���");
			}
			else {
				message.push_back(name + "�̂g�o���S������");
			}
		}
		if (skillHealType[(int)eHealType::MP]) {	// �yMP�z
			if (skillHeal_MP != -1) {
				message.push_back(name + "�̂l�o��" + to_string(skillHeal_MP) + "�㏸����");
			}
			else {
				message.push_back(name + "�̂l�o���S������");
			}
		}
		// �y�S��Ԉُ�z��Ԉُ킲�ƂɕW��
		if (skillHealType[(int)eHealType::Poison]) {	// �y�Łz
			message.push_back(name + "�̓ł��񕜂���");
		}
		if (skillHealType[(int)eHealType::Paralysis]) {	// �y��Ⴡz
			message.push_back(name + "�̖�Ⴢ��񕜂���");
		}
		if (skillHealType[(int)eHealType::Sleep]) {	// �y�����z
			message.push_back(name + "�͖ڂ��o�܂���");
		}
		if (skillHealType[(int)eHealType::Confusion]) {	// �y�����z
			message.push_back(name + "�͐��C�ɖ߂���");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// �y�ÈŁz
			message.push_back(name + "�̎��E���߂���");
		}
		if (skillHealType[(int)eHealType::Blind]) {	// �y����z
			message.push_back(name + "�͐��ォ��񕜂���");
		}
	}

	// �o�t
	std::array<string, 22> buffName{ "HP", "�ő�HP",	"MP", "�ő�MP",	"STR", "DEX", "AGI",  "LUC", "�����h��", "�M�h��",  "��C�h��", "�d�C�h��", "MUE", 	"�őϐ�", "��ბϐ�", "�����ϐ�", "�����ϐ�", "�Èőϐ�", "����ϐ�", "�C��ϐ�", "�����ϐ�", "���őϐ�" };
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		if (skillBuff[i]) {
			switch (skillBuffType[i]) {
			case BuffEffect::Negate:
				message.push_back(name + "��" + buffName[i] + "�����Ƃɖ߂���");
				break;
			case BuffEffect::Buff:
				message.push_back(name + "��" + buffName[i] + "���㏸����");
				break;
			case BuffEffect::deBuff:
				message.push_back(name + "��" + buffName[i] + "����������");
				break;
			}
		}
	}

	ReceiveSkillIni();	 // �X�L���t���O������

}
// �`��
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
		// �y�_���[�W���󂯂��z
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

// �I������
void Enemy::Release() {
	// ���[�h�J�ڏ����N���X�̉��
	for (int i = 0, n = modeTerm.size(); i != n; ++i) {
		modeTerm[i]->Release();
		delete modeTerm[i];
		modeTerm[i] = nullptr;	// �ꉞ�k���|�C���^������Ƃ�
	}
	DeleteGraph(gr);	// �摜�폜
}


// ID�擾
int Enemy::GetID() {
	return id;
}
int Enemy::GetWidth() {		// ����
	return width;
}
int Enemy::GetHeight() {	// �c��
	return height;
}
int Enemy::GetX() {		// X���W
	return x;
}
int Enemy::GetY() {		// Y���W
	return y;
}

// �y�험�i�z
// �o���l
unsigned int Enemy::GetEXP() {
	return exp;
}
// ��
unsigned int Enemy::GetMoney() {
	return money;
}
// �h���b�v�A�C�e��
int Enemy::GetDropItem() {
	return dropItem;	
}
// �h���b�v��
__int8 Enemy::GetDropRate() {
	return dropRate;
}

// debug
void Enemy::TestIni(string name) {
	// �X�e�[�^�X
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

	// �����蔻��͈́i3�~2�j
	collision.resize(2);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// �S�}�X0�ŏ�����
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = 0;
		}
	}
	// �����蔻���
	collision[0][1] = 1;
	collision[1][0] = 1;
	collision[1][1] = 1;
	collision[1][2] = 1;

	modeTerm.push_back(new ModeTerm_None(nowMode, 0));
	modeTerm[0]->AddSkill(confusionSkill);

	
	// ��Ԉُ�
	//badStatus[(int)eBadStatus::Sleep].get = true;
}