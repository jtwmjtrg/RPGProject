#include "Character.h"
#include "Battle.h"


SkillOrder::SkillOrder() {
	maxSpeed = 9999999;
}
SkillOrder::~SkillOrder() {

}

void SkillOrder::Add(Character* attacker, Skill* skill, int totalUseEnergy) {
	double speed;	// �s�����x	

	// �ő��s���@�`���[�W�␳�i���x�␳�j== -1
	if (skill->speed == -1||attacker->GetCharge(eCharge::Speed) == -1) {
		speed = maxSpeed + GetRand(100);
	}
	else {
		// �ʏ�␳���x
		// AGI�~�X�L���␳�~�`���[�W�␳�i���x�␳�j�~����(0.9�`1.1)
		speed = attacker->GetStatus(eStatus::AGI) * skill->speed * attacker->GetCharge(eCharge::Speed) * (1.0 + (double)GetRand(-1000, 1000) * 0.0001);
		// �ő��s���͔����Ȃ�
		if (speed > maxSpeed) {
			speed = maxSpeed - 1;
		}
		// 0�����ɂȂ�Ȃ��悤��
		if (speed < 0) {
			speed = 0;
		}
	}

	// �s�����x���ɓ����
	for (int i = 0, n = order.size(); i < n; ++i) {
		if (order[i].speed > speed) {
			// �y�`���[�W�␳�i�A���s���j�z
			for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
				order.insert(order.begin() + i, { attacker, skill, speed, totalUseEnergy });
			}
			return;
		}
	}
	// �y�`���[�W�␳�i�A���s���j�z
	for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
		order.push_back({ attacker, skill, speed, totalUseEnergy });
	}
}
void SkillOrder::Add_Velocity(Character* attacker, Skill* skill, int totalUseEnergy, bool velocity) {
	double speed;	// �s�����x

	if (velocity) {
		// �ő��s��
		speed = maxSpeed + GetRand(100);
	}
	else {
		// �Œx�s��
		speed = -GetRand(100);
	}

	// �s�����x���ɓ����
	for (int i = 0, n = order.size(); i < n; ++i) {
		if (order[i].speed > speed) {
			// �y�`���[�W�␳�i�A���s���j�z
			for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
				order.insert(order.begin() + i, { attacker, skill, speed, totalUseEnergy });
			}
			return;
		}
	}
	// �y�`���[�W�␳�i�A���s���j�z
	for (int j = 0, m = (int)attacker->GetCharge(eCharge::Continuing); j < m; ++j) {
		order.push_back({ attacker, skill, speed, totalUseEnergy });
	}
}
// �Ō�����|�b�v����
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

array<int, 7> Character::gr_BadStatus;		// ��Ԉُ�
int Character::se_Damage;					// �_���[�Wse

Character::Character(){
	aliveState = eAlive::alive;	// ��Ԃ��u�����v��
	for (int i = 0; i < 9; i++) {
		// ��Ԉُ평����
		badStatus[i].get = false;
		// ��Ԉُ�̒~�ϒl��������
		EBB_badStatus[i] = 0;
	}

	// �o�t������
	for (int i = 0; i < 22; ++i) {
		buffData[i].rate = 1;	// �{�����P��
		buffData[i].trun = 0;	// �^�[�����O��
	}

	damageDelay = 0;	// �_���[�W�G�t�F�N�g�̃J�E���g
	deathDelay = 0;		// ���S�G�t�F�N�g�̃J�E���g

	// �|�C���^������
	battle = nullptr;		// �퓬�N���X
	skillOrder = nullptr;	// �X�L���I�[�_�[
	confusionSkill = nullptr;// �������̍U���p�X�L��
	nomalAttack = nullptr;	// �ʏ�U��
	weapon = nullptr;		// ����
	armor = nullptr;		// �h��
	amulet = nullptr;		// ���̑�

	// debug
	// �y�G�l���M�[�ϊ������z
	efficiency[1][Heat]		= 0.50f;	// �M+
	efficiency[0][Heat]		= 0.30f;	// �M-
	efficiency[1][Elec]		= 0.25f;	// �d�C+
	efficiency[0][Elec]		= 0.05f;	// �d�C-
	efficiency[1][Shine]	= 0.70f;	// ��+
	efficiency[0][Shine]	= 0.50f;	// ��-
	efficiency[1][Sound]	= 0.40f;	// ��+
	efficiency[0][Sound]	= 0.35f;	// ��-
	efficiency[1][Motion]	= 0.50f;	// �^��+
	efficiency[0][Motion]	= 0.20f;	// �^��-
	efficiency[1][Gravity]	= 0.02f;	// �d��+
	efficiency[0][Gravity]	= 0.01f;	// �d��-
	efficiency[1][Life]		= 0.20f;	// ����+
	efficiency[0][Life]		= 0.10f;	// ����-
	efficiency[1][Static]	= 0.01f;	// �Î~+
	efficiency[0][Static]	= 0.000002f;	// �Î~-
	efficiency[1][Nucleus]	= 0.001f;	// �j+
	efficiency[0][Nucleus]	= 0.00004f;	// �j-
}
Character::~Character() {
	battle = nullptr;		// �퓬�N���X
	skillOrder = nullptr;	// �X�L���I�[�_�[

	// ���������X�L��
	for (int i = 0, n = (int)termSkill.size(); i < n; ++i) {
		termSkill[i]->Release();
		delete termSkill[i];
		termSkill[i] = nullptr;
	}
	delete nomalAttack;	// �ʏ�U��
	nomalAttack = nullptr;
	delete confusionSkill;	// �������̍U���p�X�L��
	confusionSkill = nullptr;
	delete weapon;	// ����
	weapon = nullptr;
	delete armor;	// �h��
	armor = nullptr;
	delete amulet;	// ���̑�
	amulet = nullptr;
}
// ������
void Character::Init() {
	// �y���z
	se_Damage = LoadSoundMem("sound\\battle\\damage.wav");		// �_���[�Wse
}
// �I������
void Character::Fina() {
	// �y���z
	DeleteSoundMem(se_Damage);	// �_���[�Wse
}
// �X�L���I�[�_�[�̃|�C���^�Z�b�g
void Character::SetPointer(SkillOrder *skillOrder, Battle* battle) {
	this->skillOrder = skillOrder;
	this->battle = battle;
}
// �퓬���ʏ���������
void Character::CommonInitBattle() {
	// �y�摜�z
	// ��Ԉُ�
	gr_BadStatus[0] = LoadGraph("img\\battle\\poison.png");	// ��
	gr_BadStatus[1] = LoadGraph("img\\battle\\para.png");	// ���
	gr_BadStatus[2] = LoadGraph("img\\battle\\sleep.png");	// ����
	gr_BadStatus[3] = LoadGraph("img\\battle\\panic.png");	// ����
	gr_BadStatus[4] = LoadGraph("img\\battle\\dark.png");	// �È�
	gr_BadStatus[5] = LoadGraph("img\\battle\\half.png");	// ����
	gr_BadStatus[6] = LoadGraph("img\\battle\\stun.png");	// �C��

	
}
// ���ʏI������
void Character::CommonFinaBattle() {
	// �y�摜�z
	// ��Ԉُ�
	for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
		DeleteGraph(gr_BadStatus[i]);
	}

}

int Character::Recovery(double recovery_value){
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::HP] = true;			// �������񕜂̃^�C�v

	int value = (int)recovery_value;

	if (value < 0) return 0;	// �}�C�i�X�񕜂͋����Ȃ�
	// �񕜂���
	battlestatus.HP += value;

	// HP�̏���`�F�b�N
	if (battlestatus.HP >= battlestatus.maxHP) {
		value -= battlestatus.HP - battlestatus.maxHP;
		battlestatus.HP = battlestatus.maxHP;
	}
	skillHeal_HP += value;

	return value;
}
int Character::Recovery_MP(double recovery_value) {
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::MP] = true;			// �������񕜂̃^�C�v

	int value = (int)recovery_value;

	if (value < 0) return 0;	// �}�C�i�X�񕜂͋����Ȃ�
	// �񕜂���
	battlestatus.MP += value;

	skillHeal_MP += value;

	return value;
}
int Character::FullRecovery() {
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::HP] = true;			// �������񕜂̃^�C�v

	int value = battlestatus.maxHP - battlestatus.HP;

	// HP���ő�l��
	battlestatus.HP = battlestatus.maxHP;
	skillHeal_HP = -1;

	return value;
}
int Character::FullRecovery_MP() {
	if (aliveState != eAlive::alive || battlestatus.maxMP < battlestatus.MP) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ� MP���ő�MP�𒴂��Ă����珈�����Ȃ�

	receiveSkill[(int)eSkillEffect::Heal] = true;
	skillHealType[(int)eHealType::MP] = true;			// �������񕜂̃^�C�v

	int value = battlestatus.maxMP - battlestatus.MP;

	// HP���ő�l��
	battlestatus.MP = battlestatus.maxMP;
	skillHeal_MP = -1;

	return value;
}
int Character::Damage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	int value = (int)(damage_value * GetCharge(eCharge::Defence));
	if (value < 0) return 0;
	skillDamage += (int)value;
	battlestatus.HP -= (int)value;
	// ���S����
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HP���}�C�i�X�ɂ��Ȃ�
		// aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
	}
	damageDelay = 50;	// �_���[�W�G�t�F�N�g�̃J�E���g
	return (int)value;
}
// �����_���[�W
int Character::PhysDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	// �_���[�W�v�Z�i�_���[�W - �h��́j�~�`���[�W�{���i�h��{���j
	int value = (int)((damage_value - GetStatus(eStatus::DEF_PHYS)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// �}�C�i�X�_���[�W�͋����Ȃ�
	skillDamage += value;		// �_���[�W�ۑ�
	battlestatus.HP -= value;
	// ���S����
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HP���}�C�i�X�ɂ��Ȃ�
		//aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
	}
	damageDelay = 50;
	return value;

}
// �M�_���[�W
int Character::HeatDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	int value = (int)((damage_value - GetStatus(eStatus::DEF_HEAT)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// �}�C�i�X�_���[�W�͋����Ȃ�
	skillDamage += value;
	battlestatus.HP -= value;
	// ���S����
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HP���}�C�i�X�ɂ��Ȃ�
		//aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
	}
	damageDelay = 50;
	return value;
}
// ��C�_���[�W
int Character::ColdDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	int value = (int)((damage_value - GetStatus(eStatus::DEF_COLD)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// �}�C�i�X�_���[�W�͋����Ȃ�
	skillDamage += value;
	battlestatus.HP -= value;
	// ���S����
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HP���}�C�i�X�ɂ��Ȃ�
		//aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
	}
	damageDelay = 50;
	return value;
}
// �d�C�_���[�W
int Character::ElecDamage(double damage_value)
{
	if (aliveState != eAlive::alive) return 0;	// ���łɎ���ł���or���łȂ珈�����Ȃ�

	int value = (int)(((double)damage_value - (double)GetStatus(eStatus::DEF_ELEC)) * GetCharge(eCharge::Defence));
	if (value < 0) return 0;	// �}�C�i�X�_���[�W�͋����Ȃ�
	skillDamage += value;
	battlestatus.HP -= value;
	// ���S����
	if (battlestatus.HP <= 0) {
		battlestatus.HP = 0;	// HP���}�C�i�X�ɂ��Ȃ�
		//aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		skillDeath = true;	// �X�L���Ŏ��S�����t���O�𗧂Ă�
	}
	damageDelay = 50;
	return value;
}
// �h��
int Character::Resurrection(double value) {
	if (aliveState != eAlive::dead) return 0;	// ���S���̂ݗL��
	if (value < 0) return 0;	// �񕜗ʂ�0�����Ȃ疳��

	skillResurrection = true;	// �h���t���O�𗧂Ă�
	battlestatus.HP += 1 + (int)value;	// �Œ�l�P
	// HP�̏���`�F�b�N
	if (battlestatus.HP >= battlestatus.maxHP) {
		value -= battlestatus.HP - battlestatus.maxHP;
		battlestatus.HP = battlestatus.maxHP;
	}
	
	return (int)value;
}
// ���S�h��
int Character::FullResurrection() {
	if (aliveState != eAlive::dead) return 0;	// ���S���̂ݗL��

	skillResurrection = true;	// �h���t���O�𗧂Ă�
	int value = battlestatus.maxHP - battlestatus.HP;
	battlestatus.HP = battlestatus.maxHP;	// HP�S��
	battlestatus.MP = battlestatus.maxMP;	// MP���S��
	return value;	// HP�̉񕜗ʂ�Ԃ�
}

// MP����
void Character::IncreaseMP(int value) {
	if (value <= 0)	return;		// 0�ȉ��̓G���[

	battlestatus.MP += value;	// MP�̑���
}
// MP����
void Character::DecreaseMP(int value) {
	if (value <= 0)	return;		// 0�ȉ��̓G���[

	battlestatus.MP -= value;	// MP�̌���
	// 0�����ɂȂ�����0�ɖ߂�
	if (battlestatus.MP < 0) {
		battlestatus.MP = 0;
	}
}

// ��Ԉُ��^����
void Character::GiveBadStatus(eBadStatus type, unsigned int power) {
	skillBadStatus[(int)type] = true;	// �X�L����Ԉُ�t���O�𗧂Ă�

	// ���łɂ��̏�Ԉُ�ɂ������Ă���ꍇ
	if (badStatus[(int)type].get) {
		badStatus[(int)type].turn = 0;	// �^�[�������O�ɖ߂�
		// �З͂���������I��
		if (badStatus[(int)type].power < power) {
			badStatus[(int)type].power = power;	// �З͏㏑��
		}
	}
	else {
		badStatus[(int)type].get = true;	// ��Ԉُ��
		badStatus[(int)type].turn = 0;		// �^�[�������O��
		badStatus[(int)type].power = power;	// ��Ԉُ�̋���
	}
}
// ��Ԉُ�̒~�ϒl
void Character::GiveEddDEF_BadStatus(eBadStatus type, int value) {
	EBB_badStatus[(int)type] += value;
}

// �S��Ԉُ��
void Character::HealBadStatus() {
	receiveSkill[(int)eSkillEffect::Heal] = true;		// �X�L���񕜃t���O�𗧂Ă�
	
	for (int i = 0; i < 7; ++i) {
		// ��Ԉُ�ɂ������Ă������
		if (badStatus[i].get) {
			skillHealType[3 + i] = true;	// �������񕜂̃^�C�v
			badStatus[i].get = false;
		}
	}
}
// ��Ԉُ�񕜁i�P�̂݁j
void Character::HealBadStatus(eBadStatus type) {
	// ��Ԉُ�ɂ������Ă������
	if (badStatus[(int)type].get) {
		receiveSkill[(int)eSkillEffect::Heal ] = true;		// �X�L���񕜃t���O�𗧂Ă�
		skillHealType[3 + (int)type] = true;	// �������񕜂̃^�C�v

		badStatus[(int)type].get = false;	// ��Ԉُ�̉���
	}
}

// �o�t��^����
void Character::GiveBuff(eBuff type, float rate, int turn) {
	skillBuff[(int)type] = true;	// �t���O�𗧂Ă�

	// �y�o�t�z
	if (rate > 1) {
		if (buffData[(int)type].rate < 1) {
			// ��̑ŏ���
			skillBuffType[(int)type] = BuffEffect::Negate;
			buffData[(int)type].rate = 1;
			buffData[(int)type].trun = 0;
		}
		else {
			// �o�t��������
			skillBuffType[(int)type] = BuffEffect::Buff;
			buffData[(int)type].rate *= rate;	// �{����������
			buffData[(int)type].trun += turn;	// �^�[������

			// �{�����
			if (buffData[(int)type].rate > 10) {
				buffData[(int)type].rate = 10;
			}
			// �^�[�����
			if (buffData[(int)type].rate > 5) {
				buffData[(int)type].trun = 5;
			}
		}
	}
	// �y�f�o�t�z
	else {
		if (buffData[(int)type].rate > 1) {
			// �����ŏ���
			skillBuffType[(int)type] = BuffEffect::Negate;
			buffData[(int)type].rate = 1;
			buffData[(int)type].trun = 0;
		}
		else {
			// �f�o�t��������
			skillBuffType[(int)type] = BuffEffect::deBuff;	// �f�o�t�t���O
			buffData[(int)type].rate *= rate;	// �{����������
			buffData[(int)type].trun += turn;	// �^�[������

			// �{�����
			if (buffData[(int)type].rate < 0.1) {
				buffData[(int)type].rate = 0.1f;
			}
			// �^�[�����
			if (buffData[(int)type].rate > 5) {
				buffData[(int)type].trun = 5;
			}
		}
	}
}

// �h���Ԃɂ���
void Character::Defence() {
	defenceFlag = true;
}

// �`���[�W�̃Z�b�g
void Character::SetCharge(eCharge type, float rate, int turn) {
	// �`���[�W�X�L�����Z�b�g�i������ނ͏㏑���j
	chargeData[(int)type].rate = rate;
	chargeData[(int)type].turn = turn;
}

// ��ԏ�����
void Character::StateReset() {
	// ��Ԉُ평����
	for (int i = 0; i < 9; i++) {
		// ��Ԉُ평����
		badStatus[i].get = false;
		// ��Ԉُ�̒~�ϒl��������
		EBB_badStatus[i] = 0;
	}

	// �o�t������
	for (int i = 0, n = buffData.size(); i < n; ++i) {
		buffData[i].trun = 0;
		buffData[i].rate = 1;
	}
	// �`���[�W������
	for (int i = 0, n = chargeData.size(); i < n; ++i) {
		chargeData[i].type = (eCharge)i;
		chargeData[i].turn = -1;
		chargeData[i].rate = 1;
	}

	damageDelay = 0;	// �_���[�W�G�t�F�N�g�̃J�E���g
	deathDelay = 0;		// ���S�G�t�F�N�g�̃J�E���g

	// �h���Ԃ�����
	defenceFlag = false;

	ReceiveSkillIni();	// �X�L���Ώۃt���O������

}
// �^�[���J�n����
void Character::TurnStartProcess() {
	// ���������X�L��
	for (int i = 0, n = termSkill.size(); i < n; ++i) {
		termSkill[i]->SkillExecution(this, skillOrder);
	}
}
// �^�[���I�����̏���
void Character::TurnEndProcess() {
	defenceFlag = false;	// �h���Ԃ�����

	if (aliveState != eAlive::alive) return;	// ����ł��珈�����Ȃ�

	vector<string>	message;	// �W�����b�Z�[�W

	//////////////////////////////////////
	/////////// �y��Ԉُ폈���z///////////
	//////////////////////////////////////
	// �y�Łz
	if (badStatus[(int)eBadStatus::Poison].get) {
		// �Ń_���[�W(�З́~0.75�`1.5)
		message.push_back(name + "��" + to_string(this->Damage(this->badStatus[(int)eBadStatus::Poison].power * (GetRand(75, 150)) * 0.01)) + "�̓Ń_���[�W���󂯂�") ;
		// �Ŏ�
		if (skillDeath) {
			aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
			message.push_back(name + "�͓|�ꂽ");
			TextBox::AddLine(message, 0, 10, 50);	// �e�L�X�g�\��
			return;	// ���񂾂炱���Ń��b�Z�[�W�I��
		}
		
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Poison].turn) != 0) {
			message.push_back(name + "�̓ł��񕜂���");
			// ����
			this->badStatus[(int)eBadStatus::Poison].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Poison, this->badStatus[(int)eBadStatus::Poison].power + GetRand(this->badStatus[(int)eBadStatus::Poison].power));
		}
		++this->badStatus[(int)eBadStatus::Poison].turn;	// �����^�[��+1
	}
	// �y��Ⴡz
	if (badStatus[(int)eBadStatus::Paralysis].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Paralysis].turn) != 0) {
			message.push_back(name + "�̖�Ⴢ��񕜂���");
			// ����
			this->badStatus[(int)eBadStatus::Paralysis].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Paralysis, this->badStatus[(int)eBadStatus::Paralysis].power + GetRand(this->badStatus[(int)eBadStatus::Paralysis].power));
		}
		++this->badStatus[(int)eBadStatus::Paralysis].turn;	// �����^�[��+1
	}
	// �y�����z
	if (badStatus[(int)eBadStatus::Sleep].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Sleep].turn) != 0) {
			message.push_back(name + "�͖ڂ��o�܂���");
			// ����
			this->badStatus[(int)eBadStatus::Sleep].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Sleep, this->badStatus[(int)eBadStatus::Sleep].power + GetRand(this->badStatus[(int)eBadStatus::Sleep].power));
		}
		++this->badStatus[(int)eBadStatus::Sleep].turn;	// �����^�[��+1
	}
	// �y�����z
	if (badStatus[(int)eBadStatus::Confusion].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Confusion].turn) != 0) {
			message.push_back(name + "�͐��C�ɖ߂���");
			// ����
			this->badStatus[(int)eBadStatus::Confusion].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Confusion, this->badStatus[(int)eBadStatus::Confusion].power + GetRand(this->badStatus[(int)eBadStatus::Confusion].power));
		}
		++this->badStatus[(int)eBadStatus::Confusion].turn;	// �����^�[��+1
	}
	// �y�ÈŁz
	if (badStatus[(int)eBadStatus::Blind].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Blind].turn) != 0) {
			message.push_back(name + "�̎��E���߂���");
			// ����
			this->badStatus[(int)eBadStatus::Blind].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Blind, this->badStatus[(int)eBadStatus::Blind].power + GetRand(this->badStatus[(int)eBadStatus::Blind].power));
		}
		++this->badStatus[(int)eBadStatus::Blind].turn;	// �����^�[��+1
	}
	// �y����z
	if (badStatus[(int)eBadStatus::Weakness].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Weakness].turn) != 0) {
			message.push_back(name + "�͐��ォ��񕜂���");
			// ����
			this->badStatus[(int)eBadStatus::Weakness].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Weakness, this->badStatus[(int)eBadStatus::Weakness].power + GetRand(this->badStatus[(int)eBadStatus::Weakness].power));
		}
		++this->badStatus[(int)eBadStatus::Weakness].turn;	// �����^�[��+1
	}
	// �y�X�^���z
	if (badStatus[(int)eBadStatus::Stun].get) {
		// ��������
		if (GetRand(this->badStatus[(int)eBadStatus::Stun].turn) != 0) {
			message.push_back(name + "�͈ӎ������߂���");
			// ����
			this->badStatus[(int)eBadStatus::Stun].get = false;
			// �ϐ��~�ϒl���グ��i+�З́~1~2�j
			GiveEddDEF_BadStatus(eBadStatus::Stun, this->badStatus[(int)eBadStatus::Stun].power + GetRand(this->badStatus[(int)eBadStatus::Stun].power));
		}
		++this->badStatus[(int)eBadStatus::Stun].turn;	// �����^�[��+1
	}

	//////////////////////////////////////
	/////////// �y�o�t��������z///////////
	//////////////////////////////////////
	// �o�t�̖��O
	std::array<string, 22> buffName{ "HP", "�ő�HP",	"MP", "�ő�MP",	"STR", "DEX", "AGI",  "LUC", "�����h��", "�M�h��",  "��C�h��", "�d�C�h��", "MUE", 	"�őϐ�", "��ბϐ�", "�����ϐ�", "�����ϐ�", "�Èőϐ�", "����ϐ�", "�C��ϐ�", "�����ϐ�", "���őϐ�" };
	for (int i = 0, n = buffData.size(); i < n; ++i) {
		if (buffData[i].rate == 1.0) {
			--buffData[i].trun;	// �^�[��-1
			// �c��^�[���O�ŉ���
			if (buffData[i].trun == 0) {
				buffData[i].rate = 1.0;	// �{�����P��
				message.push_back(name + "��" + buffName[i] + "�����Ƃɖ߂���");
			}
		}
	}

	//////////////////////////////////////
	///////////// �y�`���[�W�z/////////////
	//////////////////////////////////////
	for (int i = 0, n = chargeData.size(); i < n; ++i) {
		// �c��^�[�������炷
		if (chargeData[i].turn > -1) {
			--chargeData[i].turn;
		}
	}
	
	//////////////////////////////////////
	//////////// �yMP�����񕜁z////////////
	//////////////////////////////////////
	// MP���ő�MP�����Ȃ玩����
	if (GetStatus(eStatus::MP) < GetStatus(eStatus::maxMP)) {
		IncreaseMP((int)(GetStatus(eStatus::MUE) * 0.25));	// MUE��1/4��
		// �ő�MP�𒴂�����߂�
		if (GetStatus(eStatus::MP) > GetStatus(eStatus::maxMP)) {
			battlestatus.MP = GetStatus(eStatus::maxMP);
		}
	}
	// MP���ő�MP�𒴂��Ă���Ζ��f�\��
	else if (GetStatus(eStatus::MP) > GetStatus(eStatus::maxMP) * 1) {
		// �ő�MP���璴�ߕ������_���[�W�i�␳10�j�~0.8�`1.5
		message.push_back(name + "��MP���\������");
		message.push_back(name + "��" + to_string(Damage((GetStatus(eStatus::MP) - GetStatus(eStatus::maxMP) * 1) * (GetRand(700, 1500) * 0.001))) + "�̃_���[�W���󂯂�");
		battlestatus.MP = GetStatus(eStatus::maxMP);

		// ��
		if (skillDeath) {
			aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
			message.push_back(name + "�͓|�ꂽ");
			TextBox::AddLine(message, 0, 10, 50);	// �e�L�X�g�\��
			return;	// ���񂾂炱���Ń��b�Z�[�W�I��
		}
	}
	
	// �e�L�X�g�\��
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
// �X�L���t���O������
void Character::ReceiveSkillIni() {
	// �X�L���Ώۃt���O��܂�
	for (int i = 0; i < 6; i++) {
		receiveSkill[i] = false;
	}
	// �񕜃X�L���̃t���O��܂�
	for (int i = 0; i < 10; ++i) {
		skillHealType[i] = false;
	}
	// ��Ԉُ�t���O��܂�
	for (int i = 0; i < 9; ++i) {
		skillBadStatus[i] = false;
	}
	// �o�t
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		skillBuff[i] = false;
	}
	

	skillHeal_HP = 0;
	skillHeal_MP = 0;
	skillDamage = 0;
	skillDeath = false;
	skillResurrection = false;	// �h��
}
// �X�L���Ώۃt���O�𗧂Ă�
void Character::SetReceiveSkill(eSkillEffect type){
	receiveSkill[(int)type] = true;
}
// �X�L���Ώۃt���O�擾
bool Character::GetReceiveSkill(eSkillEffect type) {
	return receiveSkill[(int)type];
}
int Character::GetSkillDamage() {
	return skillDamage;
}
bool Character::GetSkillDeath() {
	return skillDeath;
}

// �X�e�[�^�X
int Character::GetStatus(eStatus status) {
	// ��Ԉُ�y����z�̏ꍇ�A�S�X�e�[�^�X����
	// ��Ԉُ�y�����z�̏ꍇ�A�S�X�e�[�^�X����
	// �h���Ԃ̏ꍇ�ADEF,AGI,DEX 2�{

	double value = 0;	// �Ԃ��l

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
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		return (int)value;
	case eStatus::DEX:
		value = (int)(battlestatus.DEX * buffData[(int)eBuff::DEX].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::AGI:
		value = (int)(battlestatus.AGI * buffData[(int)eBuff::AGI].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::LUC:
		return (int)(battlestatus.LUC * buffData[(int)eBuff::LUC].rate);
	case eStatus::DEF_PHYS:
		value = (int)(battlestatus.DEF_PHYS * buffData[(int)eBuff::DEF_PHYS].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_HEAT:
		value = (int)(battlestatus.DEF_HEAT * buffData[(int)eBuff::DEF_HEAT].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_COLD:
		value = (int)(battlestatus.DEF_COLD * buffData[(int)eBuff::DEF_COLD].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_ELEC:
		value = (int)(battlestatus.DEF_ELEC * buffData[(int)eBuff::DEF_ELEC].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::MUE:
		value = (int)(battlestatus.MUE * buffData[(int)eBuff::MUE].rate * GetCharge(eCharge::MUE));
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		return (int)value;
	default:// �G���[
		return 0;
	}
}

// ��Ԉُ�
tBadStatus Character::GetBadStatus(eBadStatus type) {
	return badStatus[(int)type];
}

// ��Ԉُ�ϐ��̎擾
int Character::GetDEF_BadStatus(eBadStatus type) {
	int type_res = (int)type + 13;
	// �ϐ���-1 �� ��Αϐ�
	if (DEF_badStatus[(int)type] == -1) {
		return -1;
	}
	else {
		// (�ϐ�*�o�t�␳)-�~�ϒl
		return (int)(DEF_badStatus[(int)type] * buffData[type_res].rate) - EBB_badStatus[(int)type];
	}
}
// �������
eAlive Character::GetAlive() {
	return aliveState;
}
// �s�����x
double Character::GetSpeed() {
	// AGI�~�X�L���̑��x�␳
	return GetStatus(eStatus::AGI);
}
// ����
int Character::GetHit() {
	// hit��-1�F�K��
	// 0�`���(AGI��DEX�̕���/10) �� 0�`����(DEX�~�Z�␳)�@�̑΍R
	// ���͋ύt�Ȃ�m��10%�ŉ������
	// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���

	// �Z�␳ == -1�͕K���U��

	// DEX�~�Z�␳�i��Ԉُ�F�ÈłȂ炳���1/10�j
	return (int)(GetRand((GetStatus(eStatus::DEX)) * 1000) * (badStatus[(int)eBadStatus::Blind].get ? 0.1 : 1.0));
}
// ����
int Character::GetAvoid() {
	// hit��-1�F�K��
	// 0�`���(AGI��DEX�̕���/20) �� 0�`����(DEX�~�Z�␳)�@�̑΍R
	// ���͋ύt�Ȃ�m��5%�ŉ������
	// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���

	// (DEX+AGI)/20�i��Ԉُ�F�ÈłȂ炳���1/10�j
	return (int)(GetRand(((GetStatus(eStatus::AGI) * 1000) + (GetStatus(eStatus::DEX)) * 1000) / 40) * (badStatus[(int)eBadStatus::Blind].get ? 0.1 : 1));
}
// �G�l���M�[�ϊ�����
double Character::GetEfficiency(eEnergy energy, bool direction) {
	if (energy == eEnergy::None) return 1.0;
	return efficiency[direction][energy];
}
// �`���[�W�{��
float Character::GetCharge(eCharge type) {

	// �c��^�[������0�̎��̂ݔ���
	if (chargeData[(int)type].turn == 0) {
		return chargeData[(int)type].rate;
	}

	return 1;
}

// �������̍U���p�X�L���擾
Skill* Character::GetConfusionSkill() {
	return confusionSkill;
}

// ����̕ύX���ɒʏ�U����ς��鏈��
void Character::ChangeWeapon() {
	switch (weapon->GetType()) {
	case FIST:// �y���z
		// �ʏ�U��
		nomalAttack = new Skill("skilldata\\unique\\NA_fist.csv");
		// �������̍U���p�X�L��
		confusionSkill = new Skill("skilldata\\unique\\NA_fist.csv");
		break;
	case SWORD:// �y���z
		nomalAttack = new Skill("skilldata\\unique\\NA_sword.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_sword.csv");
		break;
	case SPEAR:// �y���z
		nomalAttack = new Skill("skilldata\\unique\\NA_spear.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_spear.csv");
		break;
	case AX:// �y���z
		nomalAttack = new Skill("skilldata\\unique\\NA_ax.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_ax.csv");
		break;
	case WAND:// �y��z
		nomalAttack = new Skill("skilldata\\unique\\NA_wand.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_wand.csv");
		break;
	default:// �y����ȊO�z
		// �Ƃ肠�������Ɠ����ɂ��Ă���
		nomalAttack = new Skill("skilldata\\unique\\NA_fist.csv");
		confusionSkill = new Skill("skilldata\\unique\\NA_fist.csv");
		break;
	}
}

void	Character::EquipmentWeapon	(EquipmentItem* weapon) {
	delete this->weapon;	// ���ܑ������Ă��鑕��������
	this->weapon = weapon;	// �V��������𑕔�����
	ChangeWeapon();			// ����ύX�ɔ����ʏ�U���̕ύX
}
int		Character::GetWeaponID		() {
	return weapon->GetID();
}
string	Character::GetWeaponName	() {
	return weapon->GetName();
}
void	Character::EquipmentArmor	(EquipmentItem* armor) {
	delete this->armor;	// ���ܑ������Ă��鑕��������
	this->armor = armor;	// �V��������𑕔�����
}
int		Character::GetArmorID		() {
	return armor->GetID();
}
string	Character::GetArmorName		() {
	return armor->GetName();
}
void	Character::EquipmentAmulet	(EquipmentItem* amulet) {
	delete this->amulet;	// ���ܑ������Ă��鑕��������
	this->amulet = amulet;	// �V��������𑕔�����
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
/*-----------------------------------------�v���C���[�L�����̃X�[�p�[�N���X----------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlayerChara::gr_PlayerStatus;	// �v���C���[�̃X�e�[�^�X�̘g�i���펞�j
int PlayerChara::gr_PlayerStatus_Death;	// �v���C���[�̃X�e�[�^�X�̘g�i���S���j

PlayerChara::PlayerChara(string name, bool isContinue) {
	// ���O�����
	this->name = name;

	ReceiveSkillIni();	// �X�L���t���O�̏�����

	if (isContinue) {
		// �y�Z�[�u�f�[�^�擾�z
		ifstream ifs("savedata\\" + name + "\\save.txt");
		// �t�@�C�����Ȃ��ꍇ
		if (!ifs) {
			EXP = 0;
			LV = 1;
			skillPoint = 0;
			weapon = new EquipmentItem(0);
			armor = new EquipmentItem(0);
			amulet = new EquipmentItem(0);

			InitSkillTree();	// �X�L���c���[�̏�����
		}
		// �t�@�C��������ꍇ
		else {
			// �ꎞ�ۑ��p
			string str;

			// ����擾����
			while (getline(ifs, str)) {
				// �ꎞ�ۑ��p
				istringstream stream(str);
				string token;
				int j = 0;

				// �y��{���z
				// �J���}�ŋ�؂��ēǂݍ���
				while (getline(stream, token, ',')) {
					switch (j) {
					case 0:
						battlestatus.HP = boost::lexical_cast<int>(token);
						// HP��0�Ȃ玀�S��Ԃɂ���
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
					// �J�E���g�A�b�v�Ŏ��̕�����
					++j;
				}
				break;
			}
		}
		ifs.close();	// �t�@�C���N���[�Y

		LoadSkillTree();	// �擾�X�L���̃��[�h

	}
	else {
		// �͂��߂���
		EXP = 0;
		LV = 1;
		skillPoint = 10;
		weapon = new EquipmentItem(0);
		armor = new EquipmentItem(0);
		amulet = new EquipmentItem(0);

		InitSkillTree();	// �X�L���c���[�̏�����
	}

	// �y���x�����Ƃ̃X�e�[�^�X���擾�z
	ifstream level("playerdata\\levelup\\" + name + ".csv");
	if (!level) {
		name = "�ǂݍ��݃G���[";
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
	// �t�@�C��������ꍇ
	else {
		// �ꎞ�ۑ��p
		string str;

		// ���݂̃��x�����܂Ŕj��
		for (int i = 0; i < LV; ++i) {
			getline(level, str);
		}
		// �ꎞ�ۑ��p
		istringstream stream(str);
		string token;

		//1�y�ő�HP�z
		getline(stream, token, ',');
		battlestatus.maxHP = boost::lexical_cast<int>(token);
		//2�y�ő�MP�z
		getline(stream, token, ',');
		battlestatus.maxMP = boost::lexical_cast<int>(token);
		//3�ySTR�z
		getline(stream, token, ',');
		battlestatus.STR = boost::lexical_cast<int>(token);
		//4�yDEX�z
		getline(stream, token, ',');
		battlestatus.DEX = boost::lexical_cast<int>(token);
		//5�yAGI�z
		getline(stream, token, ',');
		battlestatus.AGI = boost::lexical_cast<int>(token);
		//6�yLUC�z
		getline(stream, token, ',');
		battlestatus.LUC = boost::lexical_cast<int>(token);
		//7�y�����h��z
		getline(stream, token, ',');
		battlestatus.DEF_PHYS = boost::lexical_cast<int>(token);
		//8�y�M�h��z
		getline(stream, token, ',');
		battlestatus.DEF_HEAT = boost::lexical_cast<int>(token);
		//9�y��C�h��z
		getline(stream, token, ',');
		battlestatus.DEF_COLD = boost::lexical_cast<int>(token);
		//10�y�d�C�h��z
		getline(stream, token, ',');
		battlestatus.DEF_ELEC = boost::lexical_cast<int>(token);
		//11�yMUE�z
		getline(stream, token, ',');
		battlestatus.MUE = boost::lexical_cast<int>(token);
	}
	level.close();	// �t�@�C���N���[�Y

	// �y�����蔻��͈́z�i2�~3�j
	collision.resize(3);
	for (int i = 0, n = collision.size(); i < n; i++) {
		collision[i].resize(2);
	}
	// �S�}�Xtrue�ŏ�����
	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			collision[i][j] = true;
		}
	}

	// ���̃��x���A�b�v�ɕK�v�Ȍo���l
	nextEXP = 0;
	for (int i = 0; i != LV; ++i) {
		nextEXP += 100 + LV * LV * 20;
	}

	// �y����X�L���z
	ChangeWeapon();
}
PlayerChara::~PlayerChara() {
	// �����X�L��
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		skillList[i]->Release();
		delete skillList[i];
		skillList[i] = nullptr;
	}
}

// �擾�X�L���̃��[�h
void PlayerChara::LoadSkillTree() {
	ifstream ifs("savedata\\" + name + "\\skilltree.csv");
	// �t�@�C�����Ȃ��ꍇ
	if (!ifs) {

	}
	// �t�@�C��������ꍇ
	else {
		string line;				// �ǂݍ��񂾍s�i1�s�j
		string token;				// 1�s��,�ŕ�������ۂɎg��
		bool loadTypeFlag = true;	// true:�Z���̃��[�h	false:�X�L���f�[�^�̃��[�h
		int skillID;	// �X�L��ID

		// �X�L�����X�g�̍폜�@�S�ēǂݒ���
		for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
			skillList[i]->Release();
			delete skillList[i];
			skillList[i] = nullptr;
		}
		skillList.clear();
		skillList.shrink_to_fit();

		while (getline(ifs, line)) {	// ��s���ǂݍ���
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream stream(line);
			if (line == "") break;
			// 1�s�ڔ���
			getline(stream, token, ',');
			if (token[0] == '#') {
				// �Z���̃��[�h
				if (token == "#cell")	loadTypeFlag = true;
				// �X�L���f�[�^�̃��[�h
				else if (token == "#skills")	loadTypeFlag = false;

				continue;
			}
			if (loadTypeFlag) {

			}
			else {
				// �y�X�L���f�[�^�̃��[�h�z
				// 1�yID�z
				skillID = boost::lexical_cast<int>(token);
				// 2�y�R�X�g�z�j��
				getline(stream, token, ',');
				// 3�y�擾�t���O�z
				getline(stream, token, ',');
				if (token == "1") {
					// �y�X�L���ǉ��z
					skillList.push_back(new Skill(skillID));
				}
			}
		}
	}
	ifs.close();	// �t�@�C���N���[�Y
}
// �X�L���c���[�̏�����
void PlayerChara::InitSkillTree() {
	// �t�@�C���p�ϐ�
	std::vector< std::vector<std::string> > data ;
	string line;				// �ǂݍ��񂾍s�i1�s�j
	string token;				// 1�s��,�ŕ�������ۂɎg��


	ifstream ifs("savedata\\" + name + "\\skilltree.csv");
	// �t�@�C�����Ȃ��ꍇ
	if (!ifs) {
		return;
	}
	// �t�@�C��������ꍇ
	else {
		string line;				// �ǂݍ��񂾍s�i1�s�j
		bool loadTypeFlag = true;	// true:�Z���̃��[�h	false:�X�L���f�[�^�̃��[�h
		// �X�L�����X�g�̍폜�@�S�ēǂݒ���
		for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
			skillList[i]->Release();
			delete skillList[i];
			skillList[i] = nullptr;
		}
		skillList.clear();
		skillList.shrink_to_fit();

		while (getline(ifs, line)) {	// ��s���ǂݍ���

			data.resize(data.size() + 1);	// �s�ǉ�
			boost::algorithm::split(data.back(), line, boost::is_any_of(","));	// ����
			if (line == "") break;
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream stream(line);
			// 1�s�ڔ���
			//getline(stream, token, ',');
			if (data.back().front()[0] == '#') {
				// �Z���̃��[�h
				if (data.back().front() == "#cell")	loadTypeFlag = true;
				// �X�L���f�[�^�̃��[�h
				else if (data.back().front() == "#skills")	loadTypeFlag = false;

				continue;
			}
			if (loadTypeFlag) {

			}
			else {
				// �y�X�L���f�[�^�̃��[�h�z
				data.back()[2] = "0";	// �擾�t���O��0�ɂ���
			}
		}
	}
	ifs.close();	// �t�@�C���N���[�Y

	// �t�@�C���I�[�v��
	ofstream ofs("savedata\\" + name + "\\skilltree.csv");
	// �y�Z�[�u�z
	for (int i = 0, n = (int)data.size(); i < n; ++i) {
		for (int j = 0, m = (int)data[i].size() - 1; j < m; ++j) {
			ofs << data[i][j] << ',';
		}
		ofs << data[i].back();
		ofs << endl;
	}
	ofs.close();
}

// �����蔻��̍쐬
void PlayerChara::SetColision(vector<vector<Character*>>& collField) {
	// �v���C���[�̓����蔻��z�u�i��l�Fx2y3�j

	for (int i = 0, n = collision.size(); i < n; i++) {
		for (int j = 0, m = collision[i].size(); j < m; j++) {
			if (collision[i][j]) {
				collField[i + 1][1 + order * 3 + j] = this;
			}
		}
	}
}
// �����蔻��̍폜
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
	gr_PlayerStatus = LoadGraph("img\\battle\\PlayerStatus.png");	// �v���C���[�̃X�e�[�^�X�̘g
	gr_PlayerStatus_Death = LoadGraph("img\\battle\\PlayerStatus_Death.png");	// �v���C���[�̃X�e�[�^�X�̘g�i���S���j
	// ��Ԉُ�
	gr_BadStatus[0] = LoadGraph("img\\battle\\poison.png");	// ��
	gr_BadStatus[1] = LoadGraph("img\\battle\\para.png");	// ���
	gr_BadStatus[2] = LoadGraph("img\\battle\\sleep.png");	// ����
	gr_BadStatus[3] = LoadGraph("img\\battle\\panic.png");	// ����
	gr_BadStatus[4] = LoadGraph("img\\battle\\dark.png");	// �È�
	gr_BadStatus[5] = LoadGraph("img\\battle\\half.png");	// ����
	gr_BadStatus[6] = LoadGraph("img\\battle\\stun.png");	// �C��
}
void PlayerChara::FinaBattle() {
	DeleteGraph(gr_PlayerStatus);		// �v���C���[�̃X�e�[�^�X�̘g
	DeleteGraph(gr_PlayerStatus_Death);	// �v���C���[�̃X�e�[�^�X�̘g�i���S���j
	// ��Ԉُ�
	for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
		DeleteGraph(gr_BadStatus[i]);
	}
}

void PlayerChara::Draw() {
	unsigned int color = WHITE;
	if (aliveState == eAlive::alive) {
		if (GetStatus(eStatus::HP) < GetStatus(eStatus::maxHP) / 3) {
			// �m���iHP��1/3�ȉ��j�͉��F�ŕ\��
			color = YELLOW;
		}
		else {
			// �ʏ펞�͔�
			color = WHITE;
		}		
	}
	else {
		// �擪�s�\�Ő�
		color = RED;
	}

	if (damageDelay > 0) {
		// �y�_���[�W���󂯂��z
		// �U��&�_��
		if (damageDelay & 4) {
			const int vibX = GetRand(-5, 5);
			const int vibY = GetRand(-5, 5);
			DrawGraph(16 + vibX, 16 + order * 200 + vibY, (aliveState == eAlive::alive ? gr_PlayerStatus : gr_PlayerStatus_Death));		// �g
			DrawString(color, 32 + vibX, 26 + order * 200 + vibY, "%s�@Lv.%d", name.c_str(), LV);
			DrawString(color, 32 + vibX, 48 + order * 200 + 32 * 1 + vibY, "�g�o�F%d / %d",GetStatus(eStatus::HP), GetStatus(eStatus::maxHP));
			DrawString(color, 32 + vibX, 48 + order * 200 + 32 * 2 + vibY, "�l�o�F%d / %d", GetStatus(eStatus::MP), GetStatus(eStatus::maxMP));
			// ��Ԉُ�
			for (int i = 0, n = gr_BadStatus.size(); i < n; ++i) {
				if (badStatus[i].get) {
					DrawGraph(16 + i * 42 + vibX, 160 + order * 200 + vibY, gr_BadStatus[i]);
				}
			}
		}
		--damageDelay;
	}
	else {
		DrawGraph(16, 16 + order * 200, (aliveState == eAlive::alive ? gr_PlayerStatus : gr_PlayerStatus_Death));		// �g
		DrawString(color, 32, 26 + order * 200, "%s�@Lv.%d", name.c_str(), LV);
		DrawString(color, 32, 48 + order * 200 + 32 * 1, "�g�o�F%d / %d", GetStatus(eStatus::HP), GetStatus(eStatus::maxHP));
		DrawString(color, 32, 48 + order * 200 + 32 * 2, "�l�o�F%d / %d", GetStatus(eStatus::MP), GetStatus(eStatus::maxMP));
		// ��Ԉُ�
		for (int i = 0, n = (int)gr_BadStatus.size(); i < n; ++i) {
			if (badStatus[i].get) {
				DrawGraph(16 + i * 42, 160 + order * 200, gr_BadStatus[i]);
			}
		}
	}
	
}

// �o���l�擾
void PlayerChara::AddEXP(int exp) {
	if (exp <= 0) return;

	this->EXP += exp;	// �o���l�𑝂₷

	unsigned __int8 levelUpCount = 0;	// ���x���A�b�v��
	int preLV = LV;

	// ���x���A�b�v����
	while (nextEXP <= EXP && LV < 99) {
		++LV;	// ���x���A�b�v��+1

		// ���̃��x���A�b�v�ɕK�v�Ȍo���l�ʂ����Z
		nextEXP += 100 + LV * LV * 20; //(100 + ���x���� * ���x���� * 20)
	}

	// ���x���A�b�v����
	if (LV > preLV) {
		vector<string> message;	// ���x���A�b�v�̕\�����b�Z�[�W
		message.push_back(name + "�̃��x�����オ����");

		// �y���x���z
		message.push_back(to_string(preLV) + " �� " + to_string(LV));

		// �y�t�@�C���ǂݍ��݁z
		// �t�@�C���I�[�v��
		ifstream ifs("playerdata\\levelup\\" + name + ".csv");
		// �t�@�C�����Ȃ��ꍇ
		if (!ifs) {
			message.push_back("���x���t�@�C��������");
		}
		// �t�@�C��������ꍇ
		else {
			// �ꎞ�ۑ��p
			string str;

			// ���x���������V�[�N
			for (int i = 0; i < LV; ++i) {
				getline(ifs, str);
			}

			// �ꎞ�ۑ��p
			istringstream stream(str);
			string token;

			//1�y�ő�HP�z
			getline(stream, token, ',');
			message.push_back("�ő�HP�F" + to_string(battlestatus.maxHP) + " �� " + token);
			battlestatus.maxHP = boost::lexical_cast<int>(token);
			//2�y�ő�MP�z
			getline(stream, token, ',');
			message.push_back("�ő�MP�F" + to_string(battlestatus.maxMP) + " �� " + token);
			battlestatus.maxMP = boost::lexical_cast<int>(token);
			//3�ySTR�z
			getline(stream, token, ',');
			message.push_back("STR�F" + to_string(battlestatus.STR) + " �� " + token);
			battlestatus.STR = boost::lexical_cast<int>(token);
			//4�yDEX�z
			getline(stream, token, ',');
			message.push_back("DEX�F" + to_string(battlestatus.DEX) + " �� " + token);
			battlestatus.DEX = boost::lexical_cast<int>(token);
			//5�yAGI�z
			getline(stream, token, ',');
			message.push_back("AGI�F" + to_string(battlestatus.AGI) + " �� " + token);
			battlestatus.AGI = boost::lexical_cast<int>(token);
			//6�yLUC�z
			getline(stream, token, ',');
			message.push_back("LUC�F" + to_string(battlestatus.LUC) + " �� " + token);
			battlestatus.LUC = boost::lexical_cast<int>(token);
			//7�y�����h��z
			getline(stream, token, ',');
			message.push_back("�����h��F" + to_string(battlestatus.DEF_PHYS) + " �� " + token);
			battlestatus.DEF_PHYS = boost::lexical_cast<int>(token);
			//8�y�M�h��z
			getline(stream, token, ',');
			message.push_back("�M�h��F" + to_string(battlestatus.DEF_HEAT) + " �� " + token);
			battlestatus.DEF_HEAT = boost::lexical_cast<int>(token);
			//9�y��C�h��z
			getline(stream, token, ',');
			message.push_back("��C�h��F" + to_string(battlestatus.DEF_COLD) + " �� " + token);
			battlestatus.DEF_COLD = boost::lexical_cast<int>(token);
			//10�y�d�C�h��z
			getline(stream, token, ',');
			message.push_back("�d�C�h��F" + to_string(battlestatus.DEF_ELEC) + " �� " + token);
			battlestatus.DEF_ELEC = boost::lexical_cast<int>(token);
			//11�yMUE�z
			getline(stream, token, ',');
			message.push_back("�ő�g�p���́F" + to_string(battlestatus.MUE) + " �� " + token);
			battlestatus.MUE = boost::lexical_cast<int>(token);
		}

		// �y�X�L���|�C���g�z
		skillPoint += (LV - preLV) * 5;
		message.push_back(to_string(((LV - preLV) * 5)) + "�̃X�L���|�C���g���l������");

		TextBox::AddLine(message, 0, 20, 50);

		ifs.close();
		// �t�@�C���N���[�Y
	}
}

// �o�g���t�B�[���h���W
int PlayerChara::GetX() {
	return 1 + order * 3;
}
int PlayerChara::GetY() {
	return 1;
}

// �t�B�[���h�ł̎���MP��
void PlayerChara::AutoIncMP_Field() {
	// �����Ă�Ƃ��̂�
	if (aliveState == eAlive::alive) {
		IncreaseMP(LV / 10 + 1);	// ���x����1/10��
		// �ő�MP�͒����Ȃ�
		if (battlestatus.MP > battlestatus.maxMP) {
			battlestatus.MP = battlestatus.maxMP;
		}
	}
}

// �X�L���|�C���g�̏���
bool PlayerChara::DecSkillPoint(int point) {
	// ���0���� or ����ʂ��X�L���|�C���g�𒴂���ꍇ�@�͔F�߂Ȃ�
	if (point < 0 || (signed)skillPoint < point) return false;
	
	skillPoint -= point;
	return true;
}

void PlayerChara::Save() {
	// �t�@�C���I�[�v��
	ofstream ofs("savedata\\" + name + "\\save.txt");
	//1�y��b���z
	ofs << battlestatus.HP	<< ',';		// 1�c��HP
	ofs << battlestatus.MP	<< ',';		// 2�c��MP
	ofs << EXP	<< ',';					// 3�o���l
	ofs << LV	<< ',';					// 4���x��
	ofs << weapon->GetID() << ',';		// 5�����i����j
	ofs << armor->GetID()  << ',';		// 6�����i�h��j
	ofs << amulet->GetID() << ',';		// 7�����i�����i�j
	ofs << skillPoint << ',';			// 8�X�L���|�C���g
	ofs << endl;
	/*
	//2�y�X�L���z
	for (int i = 0, n = skillList.size(); i < n; ++i) {
		ofs << skillList[i]->id << ',';		// �X�L��ID�ۑ�
	}
	*/
	ofs.close();
}
// �ێ��X�L���̎擾
vector<Skill*> PlayerChara::GetSkillList() {
	return skillList;
}
// �퓬�X�L���̎擾
vector<Skill*> PlayerChara::GetSkillList_Battle() {
	vector<Skill*> tmp_List;
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		// �퓬���Ɏg���邩
		if (skillList[i]->battleFlag) {
			// �X�L���ɕ��퐧�����Ȃ� or �Ή���������𑕔����Ă���
			if (skillList[i]->weaponRestriction == eWeaponType::LIMITLESS || skillList[i]->weaponRestriction == weapon->GetType()) {
				tmp_List.push_back(skillList[i]);
			}
		}
	}
	return tmp_List;
}
// �t�B�[���h�X�L���̎擾
vector<Skill*> PlayerChara::GetSkillList_Field() {
	vector<Skill*> tmp_List;
	for (int i = 0, n = (int)skillList.size(); i < n; ++i) {
		// �t�B�[���h�Ŏg���邩
		if (skillList[i]->fieldFlag) {
			// �X�L���ɕ��퐧�����Ȃ� or �Ή���������𑕔����Ă���
			if (skillList[i]->weaponRestriction == eWeaponType::LIMITLESS || skillList[i]->weaponRestriction == weapon->GetType()) {
				tmp_List.push_back(skillList[i]);
			}
		}
	}
	return tmp_List;
}

// �L�����N�^���s���\���Ԃ� �� ��Ԃɉ������s��������
bool PlayerChara::CheckAbleAct() {

	// �y���z
	if (aliveState != eAlive::alive) {
		return false;
	}

	// �y�����z
	if (badStatus[(int)eBadStatus::Sleep].get) {
		// �u�Ȃɂ����Ȃ��v���X�L���I�[�_�[�ɒǉ�
		//skillOrder->Add(this, skillNone);
		return false;
	}
	// �y�����z
	if (badStatus[(int)eBadStatus::Confusion].get) {
		// �u�Ȃɂ����Ȃ��v���X�L���I�[�_�[�ɒǉ�
		//killOrder->Add(this, skillNone);
		return false;
	}
	// �y�X�^���z
	if (badStatus[(int)eBadStatus::Stun].get) {
		// �u�Ȃɂ����Ȃ��v���X�L���I�[�_�[�ɒǉ�
		//skillOrder->Add(this, skillNone);
		return false;
	}
	
	// �s���\
	return true;
}

// �X�L���̃��b�Z�[�W�擾
void PlayerChara::GetEffectText(vector<string>& message) {
	// �y�_���[�W�z
	if (receiveSkill[(int)eSkillEffect::Attack]) {
		if (skillDamage > 0) {
			// �_���[�W���󂯂�
			message.push_back(name + "��" + to_string(skillDamage) + "�̃_���[�W���󂯂�");
			PlaySoundMem(se_Damage);	// �_���[�Wse
		}
		else {
			// �_���[�W�Ȃ�
			message.push_back(name + "�̓_���[�W���󂯂Ȃ�����");
		}
		// �Q�Ă���N����
		HealBadStatus(eBadStatus::Sleep);
	}
	// �y���S�z
	if (skillDeath) {
		battlestatus.HP = 0;
		aliveState = eAlive::dead;	// ������Ԃ��y���S�z��
		message.push_back(name + "�͓|�ꂽ");
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
		if (skillHealType[(int)eHealType::Debuff]) {	// �y�f�o�t�z
			message.push_back(name + "�̎�̂�������");
		}

	}
	

	// �o�t
	std::array<string, 22> buffName{ "HP", "�ő�HP",	"MP", "�ő�MP",	"STR", "DEX", "AGI",  "LUC", "�����h��", "�M�h��",  "��C�h��", "�d�C�h��", "MUE", 	"�őϐ�", "��ბϐ�", "�����ϐ�", "�����ϐ�", "�Èőϐ�", "����ϐ�", "�C��ϐ�", "�����ϐ�", "���őϐ�" };
	for (int i = 0, n = skillBuff.size(); i < n; ++i) {
		if (skillBuff[i]) {
			switch (skillBuffType[i]){
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
int PlayerChara::GetEXP() {
	return EXP;
}
int PlayerChara::GetLV() {
	return LV;
}
// �ʏ�U���擾
Skill* PlayerChara::GetNomalAttack() {
	return nomalAttack;
}

// �X�e�[�^�X�擾
int PlayerChara::GetStatus(eStatus status) {
	// ��Ԉُ�y����z�̏ꍇ�A�S�X�e�[�^�X����
	// ��Ԉُ�y�����z�̏ꍇ�A�S�X�e�[�^�X����
	// �h���Ԃ̏ꍇ�ADEF,AGI,DEX 2�{

	double value = 0;	// �Ԃ��l

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
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		return (int)value;
	case eStatus::DEX:
		value = (int)((battlestatus.DEX + weapon->GetCorrection().DEX + armor->GetCorrection().DEX + amulet->GetCorrection().DEX) * buffData[(int)eBuff::DEX].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::AGI:
		value = (int)((battlestatus.AGI + weapon->GetCorrection().AGI + armor->GetCorrection().AGI + amulet->GetCorrection().AGI) * buffData[(int)eBuff::AGI].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::LUC:
		return (int)((battlestatus.LUC + weapon->GetCorrection().LUC + armor->GetCorrection().LUC + amulet->GetCorrection().LUC) * buffData[(int)eBuff::LUC].rate);
	case eStatus::DEF_PHYS:
		value = (int)((battlestatus.DEF_PHYS + weapon->GetCorrection().DEF_PHYS + armor->GetCorrection().DEF_PHYS + amulet->GetCorrection().DEF_PHYS) * buffData[(int)eBuff::DEF_PHYS].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_HEAT:
		value = (int)((battlestatus.DEF_HEAT + weapon->GetCorrection().DEF_HEAT + armor->GetCorrection().DEF_HEAT + amulet->GetCorrection().DEF_HEAT) * buffData[(int)eBuff::DEF_HEAT].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_COLD:
		value = (int)((battlestatus.DEF_COLD + weapon->GetCorrection().DEF_COLD + armor->GetCorrection().DEF_COLD + amulet->GetCorrection().DEF_COLD) * buffData[(int)eBuff::DEF_COLD].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::DEF_ELEC:
		value = (int)((battlestatus.DEF_ELEC + weapon->GetCorrection().DEF_ELEC + armor->GetCorrection().DEF_ELEC + amulet->GetCorrection().DEF_ELEC) * buffData[(int)eBuff::DEF_ELEC].rate);
		// �y�����z
		if (badStatus[(int)eBadStatus::Sleep].get) {
			value *= 0.5;	// ����
		}
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		// �y�h���ԁz
		if (defenceFlag) {
			value *= 2;	// 2�{
		}
		return (int)value;
	case eStatus::MUE:
		value = (int)(battlestatus.MUE * buffData[(int)eBuff::MUE].rate * GetCharge(eCharge::MUE));
		// �y����z
		if (badStatus[(int)eBadStatus::Weakness].get) {
			value *= 0.5;	// ����
		}
		return (int)value;
	default:// �G���[
		return 0;
	}
}

int PlayerChara::GetPlayerOrder() {
	return order;
}
// �X�L���|�C���g
unsigned int PlayerChara::GetSkillPoint() {
	return skillPoint;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ----------------------------------------------------- �P�[�^ ----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Kata::Kata(string name, bool isContinue) : PlayerChara(name, isContinue) {
	// �P�[�^���L�̏���������

	// ����iSTR�㏸�j
	curse = new SkillEffect_Buff(
	{
		eBuff::STR,			// �͏㏸
		1,					// �����i�Ӗ��Ȃ��j
		true,				// true:�o�t	false:�f�o�t
		2,					// �����^�[��
	},
	{
		eEnergy::Life,		// �ϊ��G�l���M�[
		eConvType::Self,	// �����̖��f���g��
		true,				// +�ϊ�
		1,					// �ϊ������F�S�ϊ�
		0,					// �␳�Ȃ�
	});
}
Kata::~Kata() {

}
// Lv.1��Ԃւ̏�����
void Kata::Lv1_Init() {
	battlestatus.HP = 108;
	battlestatus.MP = 32;
	/*
	skillList.push_back(new Skill(64));		// ����
	skillList.push_back(new Skill(67));		// �t���X�g�X���C�X
	skillList.push_back(new Skill(85));		// �N���X�X���b�V��
	skillList.push_back(new Skill(86));		// ��M
	skillList.push_back(new Skill(80));		// �͗���
	*/
}
// �ŗL�����i����Ƃ��j
void Kata::UniqueProcess() {
	if (aliveState != eAlive::alive) return;	// ����ł��珈�����Ȃ�

	vector<string>	message;	// �W�����b�Z�[�W

	// �y�͂̍��󔭓��z
	message.push_back(name + " �� �͂̍��� ����������");
	// MUE��1 / 10������MP������
	const int tmp_useMP = (battlestatus.MP >= battlestatus.MUE / 10 ? battlestatus.MUE / 10 : battlestatus.MP);
	// ���ʔ���
	battle->SkillEffectExec(GetX(), GetY(), this, this, curse, battlestatus.MUE / 10, 0);
	// MP����
	battlestatus.MP -= tmp_useMP;
	// ���ʃe�L�X�g�擾
	GetEffectText(message);


	// �e�L�X�g�\��
	if (!message.empty()) {
		TextBox::AddLine(message, 0, 10, 50);
	}

}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- �e�B�[ -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tee::Tee(string name, bool isContinue) : PlayerChara(name, isContinue) {
	// �e�B�[���L�̏���������
	int a = 0;
}
Tee::~Tee() {

}
// Lv.1��Ԃւ̏�����
void Tee::Lv1_Init() {
	battlestatus.HP = 76;
	battlestatus.MP = 59;
	/*
	skillList.push_back(new Skill(68));		// �t�@�C�A�V���b�g
	skillList.push_back(new Skill(60));		// �t���[�Y�~�X�g
	skillList.push_back(new Skill(83));		// ���C�g�j���O
	skillList.push_back(new Skill(56));		// �q�[��
	skillList.push_back(new Skill(92));		// �L���A
	skillList.push_back(new Skill(63));		// ���U���N�V����
	*/
}
// �ŗL�����i����Ƃ��j
void Tee::UniqueProcess() {
	if (aliveState != eAlive::alive) return;	// ����ł��珈�����Ȃ�

	vector<string>	message;	// �W�����b�Z�[�W

	// �y�W���̍��󔭓��z
	message.push_back(name + " �� �W���̍��� ����������");
	// MUE��1 / 4�������t�B�[���h����MP���z��
	const int tmp_moveMP = (battle->GetFieldMP() >= battlestatus.MUE / 4 ? battlestatus.MUE / 4 : battle->GetFieldMP());
	// �t�B�[���hMP����
	battle->DecreaseFieldMP(tmp_moveMP);
	// MP�㏸
	IncreaseMP(tmp_moveMP);
	message.push_back(name + " ��MP�� " + to_string(tmp_moveMP) + " ��������");

	// �e�L�X�g�\��
	if (!message.empty()) {
		TextBox::AddLine(message, 0, 10, 50);
	}
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ---------------------------------------------------- �A�I�C -----------------------------------------------------*/
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Aoi::Aoi(string name, bool isContinue) :PlayerChara(name, isContinue) {
	// 
}
Aoi::~Aoi() {

}

// Lv.1��Ԃւ̏�����
void Aoi::Lv1_Init() {
	battlestatus.HP = 76;
	battlestatus.MP = 59;
}