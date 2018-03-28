#include "Skill.h"
#include "Character.h"

// �y���ʂ̕ϊ��ʂ𓾂�֐��z
double GetConvValue(const tEnergyConv energy, int totalUseEnergy) {
	if (energy.rate == -1.0) return -1;		// -1�͂��̂܂ܕԂ�

	if (energy.convType == eConvType::None) {
		// �ϊ��Ώۂ��ΏۂȂ��̏ꍇ�͂��̂܂ܕϊ��ʂ�
		return energy.rate;
	}
	else {
		// �K�v��MUE�l��n%
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
// �X�L���f�[�^�ǂݍ���
void Skill::LoadData(string path) {
	// �y�t�@�C���ǂݍ��ݗp�ϐ��z
	ifstream L_File;	// �ǂݍ��ރt�@�C��
	string L_Line;		// �ǂݍ��񂾍s�i1�s�j
	int L_Count = 0;	// �m�[�h�ǂݍ��ݗp�i���s�ڂ�ǂݍ���ł��邩�j
	string L_token;		// 1�s��,�ŕ�������ۂɎg��
	vector<vector<__int8>> L_Collision;

	// �ǂݍ��ރt�@�C���̎w��
	L_File.open(path);	// �t�@�C���I�[�v��

	if (L_File.fail()) {	// �t�@�C���ǂݍ���
		name = "�ǂݍ��߂ĂȂ���";
	}
	else {
		//////////////////////////////////////
		//////////// �y��{���z//////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);

			// 1�yid�z�擾
			getline(L_stream, L_token, ',');
			this->id = boost::lexical_cast<unsigned int>(L_token);
			// 2�y���O�z�擾
			getline(L_stream, L_token, ',');
			name = L_token;
			// 3�y���x�␳�z�擾
			getline(L_stream, L_token, ',');
			speed = boost::lexical_cast<float>(L_token);
			// 4�y�U���͈͑I���t���O�z�擾
			getline(L_stream, L_token, ',');
			selectFlag = L_token == "1";
			// 5�y�U���͈͑I��Ώہz�擾
			getline(L_stream, L_token, ',');
			selectTarget = L_token == "0";
			// 6�y�����z�擾
			getline(L_stream, L_token, ',');
			width = boost::lexical_cast<int>(L_token);
			// 7�y�c���z�擾
			getline(L_stream, L_token, ',');
			height = boost::lexical_cast<int>(L_token);
			// �y�g�pMP�z
			// 8�y������MP�z�擾
			getline(L_stream, L_token, ',');
			useMP[0] = boost::lexical_cast<int>(L_token);
			// 9�y�t�B�[���h��MP�z�擾
			getline(L_stream, L_token, ',');
			useMP[1] = boost::lexical_cast<int>(L_token);
			// 10�y�����MP�z�擾
			getline(L_stream, L_token, ',');
			useMP[2] = boost::lexical_cast<int>(L_token);
			// 11�y�K�v��MUE�z�擾
			getline(L_stream, L_token, ',');
			totalUseEnergy = boost::lexical_cast<int>(L_token);
			// 11�y�퓬���g�p�t���O�z�擾
			getline(L_stream, L_token, ',');
			battleFlag = L_token == "1";
			// 10�y�t�B�[���h�g�p�t���O�z�擾
			getline(L_stream, L_token, ',');
			fieldFlag = L_token == "1";
			// 11�y���퐧���z
			getline(L_stream, L_token, ',');
			weaponRestriction = (eWeaponType)boost::lexical_cast<int>(L_token);

			// �͈͑I���t���O��true�Ȃ�
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
		///////// �y�摜�G�t�F�N�g�z///////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);

			// 1�y�摜�G�t�F�N�g�̗L���z�擾
			getline(L_stream, L_token, ',');
			grEffect.flag =  L_token == "1";

			// �G�t�F�N�g����
			if (grEffect.flag) {
				// 2�y������X�z�擾
				getline(L_stream, L_token, ',');
				grEffect.divX = boost::lexical_cast<int>(L_token);
				// 3�y������Y�z�擾
				getline(L_stream, L_token, ',');
				grEffect.divY = boost::lexical_cast<int>(L_token);
				// 4�y���������z�擾
				getline(L_stream, L_token, ',');
				grEffect.divNum = boost::lexical_cast<int>(L_token);
				// 5�y�摜�f�[�^�z�擾
				getline(L_stream, L_token, ',');
				{// �w�i
					string str__S = "img\\battle\\skill\\" + L_token;	// �A�h���X����
					char* cstr = new char[str__S.size() + 1];			// �������m��
					strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// �R�s�[
					grEffect.gr = LoadGraph(cstr, grEffect.divX, grEffect.divY);
					delete cstr;	// ���������
				}
				//grEffect.gr = LoadGraph("img\\battle\\skill\\" + L_token, 3, 1);
			}
		}
		//////////////////////////////////////
		/////////////// �ySE�z////////////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);

			// 1�ySE�̗L���z�擾
			getline(L_stream, L_token, ',');
			if (L_token == "1") {
				// 2�ySE�̃p�X�z�擾
				getline(L_stream, L_token, ',');

				string str__S = "sound\\battle\\" + L_token;	// �A�h���X����
				char* cstr = new char[str__S.size() + 1];			// �������m��
				strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// �R�s�[zzz
				so_SE = LoadSoundMem(cstr);
				delete cstr;	// ���������

			}
			else {
				so_SE = -1;
			}
		}
		//////////////////////////////////////
		////////// �y�X�L���̐������z//////////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);
			// 1�s���ǂݍ���
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				explanatoryText.push_back(L_token);
			}
			// ����������Ȃ�
			if (explanatoryText.empty()) {
				explanatoryText.push_back("�������Ȃ�");
			}
		}
		//////////////////////////////////////
		////// �y�X�L���g�p���̃e�L�X�g�z//////
		//////////////////////////////////////
		{
			getline(L_File, L_Line);
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);
			// 1�s���ǂݍ���
			while (getline(L_stream, L_token, ',')) {
				if (L_token == "") break;
				utilizationText.push_back(L_token);
			}
			// ����������Ȃ�
			if (utilizationText.empty()) {
				utilizationText.push_back("��");
			}
		}

		//////////////////////////////////////
		//////////// �y�X�L�����ʁz////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// ��s���ǂݍ���
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);
			// 1��ڂ����[�h���Ĕ���
			getline(L_stream, L_token, ',');

			switch ((eSkillEffect)boost::lexical_cast<int>(L_token)) {
			case eSkillEffect::Attack:// �y�U���z
				//effect.push_back(new SkillEffect_Attack("0,1,0.86,4,1,1,3,0,0,2,,,,,,,,"));
				effect.push_back(new SkillEffect_Attack(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Heal:// �y�񕜁z
				//effect.push_back(new SkillEffect_Heal("1,0,1,4,1,1,20,0,,,,,,,,,,"));
				effect.push_back(new SkillEffect_Heal(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::BadStatus:// �y��Ԉُ�z
				effect.push_back(new SkillEffect_BadStatus(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Buff:// �y�o�t�E�f�o�t�z
				effect.push_back(new SkillEffect_Buff(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::AnotherSkill:// �y���̃X�L�������z
				effect.push_back(new SkillEffect_AnotherSkill(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Resurrection:// �y�h���z
				effect.push_back(new SkillEffect_Resurrection(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Hate:// �y�w�C�g�����z
				effect.push_back(new SkillEffect_Hate(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Charge:// �y�`���[�W�z
				effect.push_back(new SkillEffect_Charge(L_Line, totalUseEnergy));
				break;
			case eSkillEffect::Mode:// �y���[�h�ύX�z
				effect.push_back(new SkillEffect_Mode(L_Line, totalUseEnergy));
				break;
			}

			// �U���Ώۂ��y�I��͈́z�Ȃ瓖���蔻���ǂݍ���
			if (effect.back()->targetType == eSkillTarget::Select) {
				// �����蔻��쐬
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

	// �t�@�C���N���[�Y
	L_File.close();
}
Skill::~Skill() {

}
// SE���Ȃ炷
void Skill::PlayerSE() {
	if (so_SE != -1) {
		PlaySoundMem(so_SE);
	}
}
// �X�L�������i�t�B�[���h�p�j
void Skill::Execution_Field(Character* attacker, std::vector<PlayerChara*> playerList, Character* target) {
	if (attacker->GetAlive() != eAlive::alive) return;
	// �t�B�[���h�g�p�s�Ȃ甭�����Ȃ�
	if (!fieldFlag) return;

	if (attacker->GetStatus(eStatus::MP) < (int)useMP[0]) {
		TextBox::AddLine("MP������Ȃ�");
		return;
	}

	for (int i = 0, n = effect.size(); i < n; ++i) {
		switch (effect[i]->targetType) {
		case eSkillTarget::Self:// �y�����z
			effect[i]->Execution_Field(attacker, attacker);
			break;
		case eSkillTarget::Ally:// �y�����S�́z
		case eSkillTarget::Overall:// �y�G�����S�́z
			for (int j = 0, m = playerList.size(); j < m; ++m) {
				effect[i]->Execution_Field(attacker, playerList[i]);
			}
			break;
		case eSkillTarget::Select:// �y�I���z
			// �I������ĂȂ���΂͔������Ȃ�
			if (target != nullptr) {
				effect[i]->Execution_Field(attacker, target);
			}
			break;
		default:// �y����ȊO�z
			break;
		}
	}
}
// �I������
void Skill::Release() {
	// �X�L���f�[�^�̍폜
	for (int i = 0, n = effect.size(); i != n; ++i) {
		delete effect[i];
		effect[i] = nullptr;	// �ꉞ�k���|�C���^������Ƃ�
	}
	if (grEffect.flag) {
		DeleteGraph(grEffect.gr);
	}	
}
// debug
void Skill::TestIni() {
	name = "��������";

	effect.push_back(new SkillEffect_Attack());

	// �����蔻��͈́i3�~3�j
	collision.resize(3);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(3);
	}
	// �S�}�Xfalse�ŏ�����

	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = false;
		}
	}
	// �����蔻��
	collision[0][2] = true;
	collision[1][1] = true;
	collision[2][0] = true;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------- �������̍s���p�X�L�� --------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Confusion::Skill_Confusion(int arWidth, int arHeight, vector<vector<__int8>> arCollision) {
	type = eSkillType::Confusion;
	//id = -1;	// ID
	//name = "����";	// ���O
	speed = 1;	// �␳���x
	selectFlag = true;	// �U���͈͑I���t���O
	//selectTarget = true;	// �U���͈͑I��Ώ�
	battleFlag = true;
	fieldFlag = false;
	width = arWidth;	// �U���͈� ����
	height = arHeight;	//          �c��
	x = 0;
	y = 0;

	// �U���X�L���G�t�F�N�g
	//effect.push_back(new SkillEffect_Attack("0,4,0,1,0,2,0.5,0.5,1,1", 0));
	effect.push_back(new SkillEffect_Attack());


	// �����蔻��
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
/* --------------------------------------------------- �Ȃɂ����Ȃ� ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_None::Skill_None() {
	type = eSkillType::None;
	speed = 1;	// �␳���x
	selectFlag = false;	// �U���͈͑I���t���O
	battleFlag = true;
	fieldFlag = false;
}
Skill_None::~Skill_None() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- �h�� ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Defence::Skill_Defence() {
	type = eSkillType::Defence;
	speed = 10;	// �␳���x
	selectFlag = false;	// �U���͈͑I���t���O
	battleFlag = true;
	fieldFlag = false;
}
Skill_Defence::~Skill_Defence() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------- ���� ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Skill_Escape::Skill_Escape() {
	type = eSkillType::Escape;
	speed = 1;	// �␳���x
	selectFlag = false;	// �U���͈͑I���t���O
	battleFlag = true;
	fieldFlag = false;
}
Skill_Escape::~Skill_Escape() {
}

//#################################################################################################################//
//#################################################################################################################//
/*################################################### �X�L������ ##################################################*/
//#################################################################################################################//
//#################################################################################################################//

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------ �U�� -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������̍U���p������
SkillEffect_Attack::SkillEffect_Attack() {
	type = eSkillEffect::Attack;

	energyConv.type = eEnergy::None;	// �ϊ��Ȃ�
	energyConv.convType = eConvType::None;
	energyConv.direction = true;
	energyConv.rate = 0;
	energyConv.correction = 2;		// STR�␳
	attackData.hit = 0.5;			// �����{��0.5�{
	attackData.damageRate = 0.5;	// �_���[�W�{��0.5
	attackData.times_min = 1;		// 1hit
	attackData.times_max = 1;


}
SkillEffect_Attack::SkillEffect_Attack(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Attack;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�G�l���M�[�̎�ށz�擾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 3�y�ϊ��Ώہz�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 4�y�ϊ������z�擾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 5�y�ϊ��ʁz�擾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6�y�ϊ��␳�z�擾
	getline(L_stream, L_token, ',');
	energyConv.correction = boost::lexical_cast<int>(L_token);
	// 7�y�����␳�z�擾
	getline(L_stream, L_token, ',');
	attackData.hit = boost::lexical_cast<float>(L_token);
	// 8�y�_���[�W�{���z�擾
	getline(L_stream, L_token, ',');
	attackData.damageRate = boost::lexical_cast<float>(L_token);
	// 9�y�U���Ώہz�擾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 10�y�ŏ��U���񐔁z�擾
	getline(L_stream, L_token, ',');
	attackData.times_min = boost::lexical_cast<int>(L_token);
	// 11�y�ő�U���񐔁z�擾
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
/* ------------------------------------------------------ �� -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Heal::SkillEffect_Heal() {
	type = eSkillEffect::Heal;

	// debug
	TestIni();
}
SkillEffect_Heal::SkillEffect_Heal(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Heal;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�񕜃^�C�v�z�擾
	getline(L_stream, L_token, ',');
	healData.type = (eHealType)boost::lexical_cast<int>(L_token);
	// 3�y�{���z�擾
	getline(L_stream, L_token, ',');
	healData.rate = boost::lexical_cast<float>(L_token);
	// 4�y�U���Ώہz�擾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5�y�����G�l���M�[�̕ϊ��ʁz�擾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6�y�ϊ��␳�z�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);

	// ���̑��ϊ����
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
	// �Ώۂ�����ł���񕜂��Ȃ�
	if (target->GetAlive() != eAlive::alive) return;
	
	switch (healData.type) {
	case eHealType::HP:// �yHP�񕜁z
	{
		// �Ώۉ񕜁i�␳72�j
		switch (energyConv.convType)
		{
		case eConvType::Self:
		case eConvType::Field:
		case eConvType::Target:
			// �K�v��MUE�l��n% 
			target->Recovery((totalUseEnergy * energyConv.rate) * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			attacker->DecreaseMP((int)
				(energyConv, totalUseEnergy));	// MP����
			break;
		case eConvType::None:
			// �ϊ��Ώۂ��y�ΏۂȂ��z�̏ꍇ�͂��̂܂ܕϊ��ʂ�
			target->Recovery(energyConv.rate * healData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 72);
			break;
		default:
			break;
		}
		break;
	}
	case eHealType::MP:// �yMP�񕜁z
		break;
	case eHealType::BadStatus:// �y�S��Ԉُ�z
		target->HealBadStatus();
		break;
	case eHealType::Poison:// �y�Łz
		target->HealBadStatus(eBadStatus::Poison);
		break;
	case eHealType::Paralysis:// �y��Ⴡz
		target->HealBadStatus(eBadStatus::Paralysis);
		break;
	case eHealType::Sleep:// �y�����z
		target->HealBadStatus(eBadStatus::Sleep);
		break;
	case eHealType::Confusion:// �y�����z
		target->HealBadStatus(eBadStatus::Confusion);
		break;
	case eHealType::Blind:// �y�ÈŁz
		target->HealBadStatus(eBadStatus::Blind);
		break;
	case eHealType::Weakness:// �y����z
		target->HealBadStatus(eBadStatus::Weakness);
		break;
	case eHealType::Debuff:// �y�S�f�o�t�z
		break;
	}
}
//debug
void SkillEffect_Heal::TestIni() {
	healData.type = eHealType::HP;
	healData.rate = 1;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- ��Ԉُ� ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_BadStatus::SkillEffect_BadStatus() {
	type = eSkillEffect::BadStatus;
}
SkillEffect_BadStatus::SkillEffect_BadStatus(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::BadStatus;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����́z�擾
	getline(L_stream, L_token, ',');
	badStatusData.hit = boost::lexical_cast<int>(L_token);
	// 3�y�З́z�擾
	getline(L_stream, L_token, ',');
	badStatusData.power = boost::lexical_cast<int>(L_token);
	// 4�y�U���Ώہz�擾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5�y��Ԉُ�̎�ށz�擾
	getline(L_stream, L_token, ',');
	badStatusData.type = (eBadStatus)boost::lexical_cast<int>(L_token);
	// 6�y�G�l���M�[�̎�ށz�擾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 7�y�ϊ��Ώہz�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 8�y�ϊ������z�擾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 9�y�ϊ��ʁz�擾
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
/* -------------------------------------------------- �o�t�E�f�o�t -------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Buff::SkillEffect_Buff() {
	type = eSkillEffect::Buff;
}
SkillEffect_Buff::SkillEffect_Buff(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Buff;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����́z�擾
	getline(L_stream, L_token, ',');
	buffData.hit = boost::lexical_cast<float>(L_token);
	// 3�y�����^�[���z�擾
	getline(L_stream, L_token, ',');
	buffData.trun = boost::lexical_cast<int>(L_token);
	// 4�y�o�t���f�o�t���z�擾
	getline(L_stream, L_token, ',');
	buffData.dir = L_token == "1";
	// 5�y�U���Ώہz�擾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 6�y�o�t�̎�ށz�擾
	getline(L_stream, L_token, ',');
	buffData.type = (eBuff)boost::lexical_cast<int>(L_token);
	// 7�y�G�l���M�[�̎�ށz�擾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 8�y�ϊ��Ώہz�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 9�y�ϊ������z�擾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 10�y�ϊ��ʁz�擾
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
/* ------------------------------------------------- ���̃X�L������ ------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill() {
	type = eSkillEffect::AnotherSkill;

	// debug
	TestIni();
}
SkillEffect_AnotherSkill::SkillEffect_AnotherSkill(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::AnotherSkill;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�X�L���f�[�^�z�擾
	getline(L_stream, L_token, ',');
	anotherSkillData.skill = new Skill(boost::lexical_cast<int>(L_token));
	// 3�y�������z�擾
	getline(L_stream, L_token, ',');
	anotherSkillData.invocation = boost::lexical_cast<int>(L_token);
	// 4�y���U�t���O�z�擾
	getline(L_stream, L_token, ',');
	anotherSkillData.fastFlag = L_token == "1";
	

	// �ꉞ�G�l���M�[�ϊ��f�[�^��������
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
/* ------------------------------------------------------ �h�� -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Resurrection::SkillEffect_Resurrection() {
	type = eSkillEffect::Resurrection;

	// debug
	TestIni();
}
SkillEffect_Resurrection::SkillEffect_Resurrection(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Resurrection;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�������z�擾
	getline(L_stream, L_token, ',');
	resurrectionData.success = boost::lexical_cast<int>(L_token);
	// 3�y�{���z�擾
	getline(L_stream, L_token, ',');
	resurrectionData.rate = boost::lexical_cast<float>(L_token);
	// 4�y�Ώہz�擾
	getline(L_stream, L_token, ',');
	targetType = (eSkillTarget)boost::lexical_cast<int>(L_token);
	// 5�y�����G�l���M�[�̕ϊ��ʁz�擾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);
	// 6�y�ϊ��␳�z�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);


	// ���̑��ϊ����
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
	// �Ώۂ�����ł��Ȃ���Δ��肵�Ȃ�
	if (target->GetAlive() != eAlive::dead) return;

	// �Ώۑh���i�␳2�j
	int healValue = target->Resurrection(GetConvValue(energyConv, totalUseEnergy) * resurrectionData.rate * attacker->GetEfficiency(eEnergy::Life, true) * 2);
}
//debug
void SkillEffect_Resurrection::TestIni() {
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------- �w�C�g���� --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Hate::SkillEffect_Hate() {
	type = eSkillEffect::Hate;

	// debug
	TestIni();
}
SkillEffect_Hate::SkillEffect_Hate(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Hate;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�w�C�g�����l�z�擾
	getline(L_stream, L_token, ',');
	hateData.value = boost::lexical_cast<int>(L_token);
	// 3�y�w�C�g�����{���z�擾
	getline(L_stream, L_token, ',');
	hateData.rate = boost::lexical_cast<float>(L_token);
	// 4�y�G�l���M�[�̎�ށz�擾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 5�y�ϊ��Ώہz�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 6�y�ϊ������z�擾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 7�y�ϊ��ʁz�擾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// ���̑��ϊ����
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
/* ---------------------------------------------------- �`���[�W ---------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Charge::SkillEffect_Charge() {
	type = eSkillEffect::Charge;

	// debug
	TestIni();
}
SkillEffect_Charge::SkillEffect_Charge(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Charge;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y��ށz�擾
	getline(L_stream, L_token, ',');
	chargeData.type = (eCharge)boost::lexical_cast<int>(L_token);
	// 3�y�{���z�擾
	getline(L_stream, L_token, ',');
	chargeData.rate = boost::lexical_cast<float>(L_token);
	// 4�y�����^�[���z�擾
	getline(L_stream, L_token, ',');
	chargeData.turn = boost::lexical_cast<int>(L_token);
	// 5�y�G�l���M�[�̎�ށz�擾
	getline(L_stream, L_token, ',');
	energyConv.type = (eEnergy)boost::lexical_cast<int>(L_token);
	// 6�y�ϊ��Ώہz�擾
	getline(L_stream, L_token, ',');
	energyConv.convType = (eConvType)boost::lexical_cast<int>(L_token);
	// 7�y�ϊ������z�擾
	getline(L_stream, L_token, ',');
	energyConv.direction = L_token == "1";
	// 8�y�ϊ��ʁz�擾
	getline(L_stream, L_token, ',');
	energyConv.rate = boost::lexical_cast<double>(L_token);


	// ���̑��ϊ����
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
/* --------------------------------------------------- ���[�h�ύX --------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkillEffect_Mode::SkillEffect_Mode() {
	type = eSkillEffect::Mode;

	// debug
	TestIni();
}
SkillEffect_Mode::SkillEffect_Mode(string line, int totalUseEnergy) : SkillEffect(totalUseEnergy) {
	type = eSkillEffect::Mode;

	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y���[�h�ԍ��z�擾
	getline(L_stream, L_token, ',');
	modeData.modeNum = boost::lexical_cast<int>(L_token);
	
	// �ꉞ�G�l���M�[�ϊ��f�[�^��������
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
