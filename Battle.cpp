#include "Battle.h"

Battle::Battle() {
	turnNum = 0;	// �^�[����

	// �y�摜�z
	Character::CommonInitBattle();	// �L�����N�^�퓬���ʏ���������
	PlayerChara::InitBattle();	// �v���C���[�̐퓬������
	Enemy::InitBattle();	// �G�̃o�g���p�̉摜�ǂݍ���
	{// �w�i
		string str__S = "img\\battle\\back\\" + to_string(encounter.back) + ".png";	// �A�h���X����
		char* cstr = new char[str__S.size() + 1];			// �������m��
		strcpy_s(cstr, str__S.size() + 1, str__S.c_str());	// �R�s�[
		gr_Back = LoadGraph(cstr);
		delete cstr;	// ���������
	}
	gr_SkillSelect = LoadGraph("img\\battle\\skillSelect.png");	// �X�L���I���E�B���h�E
	gr_scroll = LoadGraph("img\\battle\\scroll.png");			// �X�N���[���o�[
	
	// �X�L���̓����蔻��̉摜
	gr_SkillHit1	= LoadGraph("img\\battle\\skillHit1.png");
	gr_SkillHit2	= LoadGraph("img\\battle\\skillHit2.png");
	gr_SkillHit3	= LoadGraph("img\\battle\\skillHit3.png");
	gr_SkillHit4	= LoadGraph("img\\battle\\skillHit4.png");
	gr_target		= LoadGraph("img\\battle\\target.png");
	gr_NA			= LoadGraph("img\\battle\\skill\\NA_sword.png", 3, 1);
	
	DamageEffect::Init();	// �_���[�W���G�t�F�N�g�̏�����
	
	// �v���C���[���̃��[�h
	player = Player::character;

	// �y�v���C���[�t�B�[���h�������z4�~((3*�l��)+1)
	playerFieldHeight = 4;
	playerFieldWidth = player.size() * 3 + 1;

	playerField.resize(playerFieldHeight);
	for (int i = 0; i != playerFieldHeight; ++i) {
		playerField[i].resize(playerFieldWidth);
	}
	// �S�}�Xnullptr�ŏ�����
	for (int i = 0; i != playerFieldHeight; ++i) {
		for (int j = 0; j != playerFieldWidth; ++j) {
			playerField[i][j] = nullptr;
		}
	}
	// �y�X�L���Ώەۑ��p�ϐ��������z
	playerField_Save.resize(playerFieldHeight);
	for (int i = 0; i != playerFieldHeight; ++i) {
		playerField_Save[i].resize(playerFieldWidth);
	}
	for (int i = 0; i != playerFieldHeight; ++i) {
		for (int j = 0; j != playerFieldWidth; ++j) {
			playerField_Save[i][j] = false;
		}
	}

	// �G���J�E���g�f�[�^�ǂݍ���
	LoadEncounter();

	// �y�X�L���I�[�_�[�z
	skillOrder = new SkillOrder();
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		enemy[i]->SetPointer(skillOrder, this);
	}
	for (int i = 0, n = player.size(); i != n; ++i) {
		player[i]->SetPointer(skillOrder, this);
	}

	// �G�̎��S�҃��X�g�̃��Z�b�g
	Enemy::deathList.clear();
	Enemy::deathList.shrink_to_fit();

	// �v���C���[�̕��я� & �����蔻��̍쐬
	playerHate.resize(player.size());
	for (int i = 0, n = player.size(); i != n; ++i) {
		player[i]->order = i;
		player[i]->SetColision(playerField);
		if (player[i]->GetAlive() == eAlive::alive) {
			playerHate[i] = 1000;	// �w�C�g�l�i�����l1000�j
		}
		else {
			playerHate[i] = 0;	// �w�C�g0
		}
	}
	maxHate = 999999;	// �w�C�g�l���

	// �y�R�}���h�I����ʂ̕ϐ��������z
	selectChara = 0;
	selectAction = 0;
	ActionNum = 5;
	selectSkill = 0;
	selectSkillCursor = 0;
	selectSkillArea = 0;
	skillNum = 0;
	selectTargetX = 0;
	selectTargetY = 0;
	selectTargetAlly = 0;

	// �O�^�[���ɑI�������J�[�\���̈ʒu���L�����Ă����\���̏�����
	commandPreSelect.resize(player.size());
	for (int i = 0, n = player.size(); i < n; ++i) {
		commandPreSelect[i] = { 0,0,0,enemyFieldWidth/2,enemyFieldHeight-2, 0, false };
	}

	// �y�험�i�������z
	exp = 0;
	money = 0;

	battleEndType = eEndStatus::Lose;		// �퓬�I���̏��

	// ����X�L���쐬
	skill_Defence = new Skill_Defence();	// �h��
	skill_None = new Skill_None();	// �u�Ȃɂ����Ȃ��v�X�L��
	skill_Escape = new Skill_Escape();	// ����

	skillStep = true;		// true:�X�L���G�t�F�N�g�\��	false:�X�L������

	// �t�B�[���h��MP
	fieldMaxMP = 1000;
	fieldMP = fieldMaxMP;

	// �퓬�J�n��ʂɈڍs
	StepChange_START();

	// debug

	// suzuki_debug
	selectItem.pos.X	= 0;
	selectItem.pos.Y	= 0;
	selectItem.flag		= false;
	usedItem.resize(Player::character.size());
	for (int i = 0, n = usedItem.size(); i < n; ++i) {
		usedItem[i].player = 0;
		usedItem[i].item.X = 0;
		usedItem[i].item.Y = 0;
		usedItem[i].isUse = false;
	}
}

// �G���J�E���g�f�[�^�̓ǂݍ���
void Battle::LoadEncounter() {
	// �y�t�@�C���ǂݍ��ݗp�ϐ��z
	ifstream L_File;	// �ǂݍ��ރt�@�C��
	string L_Line;		// �ǂݍ��񂾍s�i1�s�j
	int L_Count = 0;	// �m�[�h�ǂݍ��ݗp�i���s�ڂ�ǂݍ���ł��邩�j
	string L_token;		// 1�s��,�ŕ�������ۂɎg��

	int L_EnemyID;
	int L_EnemyX;
	int L_EnemyY;

	// �ǂݍ��ރt�@�C���̎w��
	L_File.open("encounterdata\\" + to_string(encounter.id)+ ".csv");	// �t�@�C���I�[�v��

	if (L_File.fail()) {	// �t�@�C���ǂݍ���
	}
	else {
		// 1�s�ځy��{���z
		getline(L_File, L_Line);
		// �ǂݍ��񂾍s���R�s�[
		std::istringstream L_stream(L_Line);

		//////////////////////////////////////
		//////////// �y��{���z//////////////
		//////////////////////////////////////
		// 1�y�G�t�B�[���h�̉����z�擾
		getline(L_stream, L_token, ',');
		enemyFieldWidth = boost::lexical_cast<int>(L_token);
		// 2�y�G�t�B�[���h�̏c���z�擾
		getline(L_stream, L_token, ',');
		enemyFieldHeight = boost::lexical_cast<int>(L_token);
		// 3�y1�}�X�̃T�C�Y�z�擾
		getline(L_stream, L_token, ',');
		cellSize = boost::lexical_cast<int>(L_token);

		// �G�t�B�[���h�̐���
		enemyField.resize(enemyFieldHeight);
		for (int i = 0; i != enemyFieldHeight; ++i) {
			enemyField[i].resize(enemyFieldWidth);
		}
		// �S�}�Xnullptr�ŏ�����
		for (int i = 0; i != enemyFieldHeight; ++i) {
			for (int j = 0; j != enemyFieldWidth; ++j) {
				enemyField[i][j] = nullptr;
			}
		}
		// �y�X�L���Ώەۑ��p�z
		enemyField_Save.resize(enemyFieldHeight);
		for (int i = 0; i != enemyFieldHeight; ++i) {
			enemyField_Save[i].resize(enemyFieldWidth);
		}
		for (int i = 0; i != enemyFieldHeight; ++i) {
			for (int j = 0; j != enemyFieldWidth; ++j) {
				enemyField_Save[i][j] = false;
			}
		}

		// �}�X�̍���̍��W�v�Z
		enemyFieldX = (WINDOW_WIDTH / 2) - (enemyField[0].size() * cellSize) / 2;
		enemyFieldY = (WINDOW_HEIGHT - 300) - (enemyField.size() * cellSize);

		//////////////////////////////////////
		//////////// �y�G�f�[�^�z//////////////
		//////////////////////////////////////
		while (getline(L_File, L_Line)) {	// ��s���ǂݍ���
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);

			// 1�y�GID�z�擾
			getline(L_stream, L_token, ',');
			L_EnemyID = boost::lexical_cast<int>(L_token);
			// 2�yX���W�z�擾
			getline(L_stream, L_token, ',');
			L_EnemyX = boost::lexical_cast<int>(L_token);
			// 3�yY���W�z�擾
			getline(L_stream, L_token, ',');
			L_EnemyY = boost::lexical_cast<int>(L_token);

			// �G�̍쐬
			enemy.push_back(new Enemy(L_EnemyX, L_EnemyY, L_EnemyID, turnNum));
		}
		// �G�f�[�^�ǂݍ��� && �����蔻��쐬
		for (int i = 0, n = enemy.size(); i < n; ++i) {
			enemy[i]->LoadData(turnNum, enemy);
			enemy[i]->SetColision(enemyField);
		}
	}
	// �t�@�C���N���[�Y
	L_File.close();
}

// debug
void Battle::TestIni() {
	/*// �G�t�B�[���h�}�X�i10�~5�j�ŏ�����
	enemyField.resize(5);
	for (int i = 0, n = enemyField.size(); i < n; i++) {
		enemyField[i].resize(10);
	}
	// 1�}�X�̃T�C�Y
	cellSize = 128;
	// �S�}�Xnullptr�ŏ�����
	for (int i = 0, n = enemyField.size(); i < n; i++) {
		for (int j = 0, m = enemyField[i].size(); j < m; j++) {
			enemyField[i][j] = nullptr;
		}
	}

	�����J�g�i�N����
	�G�e���R�N�g�D�X

	*/

	//enemy.push_back(new Enemy(0, 3, "�Ă��P"));
	//enemy.push_back(new Enemy(4, 1, "�Ă��Q"));
	//enemy.push_back(new Enemy(7, 3, "�Ă��R"));

	


	/*// �G�f�[�^�쐬
	enemy.push_back(new Enemy("�Ă��P"));
	enemy.push_back(new Enemy("�Ă��Q"));
	enemy.push_back(new Enemy("�Ă��R"));
	// �G1�̓����蔻��
	enemyField[3][1] = enemy[0];
	enemyField[3][2] = enemy[0];
	enemyField[4][1] = enemy[0];
	enemyField[4][2] = enemy[0];
	// �G2�̓����蔻��
	enemyField[1][5] = enemy[1];
	enemyField[1][6] = enemy[1];
	// �G3�̓����蔻��
	enemyField[2][7] = enemy[2];
	enemyField[2][8] = enemy[2];
	enemyField[2][9] = enemy[2];
	enemyField[3][7] = enemy[2];
	enemyField[3][8] = enemy[2];
	enemyField[3][9] = enemy[2];
	enemyField[4][7] = enemy[2];
	enemyField[4][8] = enemy[2];
	enemyField[4][9] = enemy[2];*/

	
}

Battle::~Battle() {
	// �y�摜�z
	DeleteGraph(gr_Back);			// �w�i
	DeleteGraph(gr_SkillHit1);		// �X�L���̓����蔻��
	DeleteGraph(gr_SkillHit2);		// �X�L���̓����蔻��
	DeleteGraph(gr_SkillHit3);		// �X�L���̓����蔻��
	DeleteGraph(gr_SkillHit4);		// �X�L���̓����蔻��
	DeleteGraph(gr_target);			// �G�̓����蔻��
	DeleteGraph(gr_NA);				// 
	DeleteGraph(gr_SkillSelect);	// �X�L���I���E�B���h�E
	PlayerChara::FinaBattle();	// �v���C���[�̃o�g���p�̉摜���
	Enemy::FinaBattle();	// �G�̃o�g���p�̉摜���
	Character::CommonFinaBattle();	// �L�����N�^�퓬���ʏI������
	DamageEffect::Fina();	// �_���[�W���G�t�F�N�g�̏I������

	// �G�f�[�^delete
	for (int i = 0, n = (int)enemy.size(); i < n; i++) {
		enemy[i]->Release();
		delete enemy[i];
		enemy[i] = nullptr;
	}

	// �G�̎��S�҃��X�g�̃��Z�b�g
	Enemy::deathList.clear();
	Enemy::deathList.shrink_to_fit();

	StopSoundMem(encounter.battleBGM);
	DeleteSoundMem(encounter.battleBGM);
}

void Battle::Process() {

	// �y�G�t�F�N�g�����z
	// �摜��\������G�t�F�N�g
	for (int i = 0, n = graphicEffect.size(); i != n; ++i) {
		if (graphicEffect[i]->Process()) {
			delete graphicEffect[i];
			graphicEffect.erase(graphicEffect.begin() + i);
			--i;
			--n;
		}
	}
	/*if (Input::Get(VK_Z) == 1) {
		for (int i = 0, n = graphicEffect.size(); i != n; ++i) {
			delete graphicEffect[i];
		}
		graphicEffect.clear();
		graphicEffect.shrink_to_fit();
	}*/
	//if (!graphicEffect.empty()) {
	//	return;
	//}
	// �_���[�W�ʕ\��
	for (int i = 0, n = damageEffect.size(); i != n; ++i) {
		if (damageEffect[i].Process()) {
			damageEffect.erase(damageEffect.begin() + i);
			--i;
			--n;
		}
	}

	switch (step)
	{
	case eBattleStep::START:
		Process_START();
		break;
	case eBattleStep::COMMAND:
		Process_COMMAND();
		break;
	case eBattleStep::BATTLE:
		Process_BATTLE();
		break;
	case eBattleStep::END:
		Process_END();
		break;
	}	
}
// �퓬�J�n���
void Battle::Process_START() {
	
	if (TextBox::textBoxFlag) return;	// �e�L�X�g��\�����I���܂�
	StepChange_COMMAND();	// �R�}���h��ʂֈڍs
}
// �J�n��ʂւ̈ڍs����
void Battle::StepChange_START() {
	step = eBattleStep::START;
	// �G���J�E���g���̕\��
	vector<string>	message;
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		message.push_back(enemy[i]->GetName() + "�����ꂽ");
	}
	TextBox::AddLine(message, 0, 10, 50);

	// �S�L�����퓬�J�n����
	for (int i = 0, n = player.size(); i < n; ++i) {
		player[i]->StateReset();
		player[i]->UniqueProcess();		// �ŗL�����i����Ƃ��j
	}
	for (int i = 0, n = enemy.size(); i < n; ++i) {
		enemy[i]->StateReset();
	}
}

// �R�}���h��ʂւ̈ڍs����
void Battle::StepChange_COMMAND() {
	// suzuki_debug
	for (int i = 0, n = usedItem.size(); i < n; ++i) {
		// �A�C�e���̍폜�Ƃ���̃|�C���^�̑��
		if (usedItem[i].isUse) {
			// ���̂�����ꍇ
			if (Player::bag[usedItem[i].player][usedItem[i].item.Y]->GetIsCunsumption()) {
				// ���̂̍폜�ƃo�b�O�̐���
				delete Player::bag[usedItem[i].player][usedItem[i].item.Y];												// ���̂̍폜
				Player::bag[usedItem[i].player].erase(Player::bag[usedItem[i].player].begin() +  usedItem[i].item.Y);	// �z��̐���
			}
		}
		// ������
		usedItem[i].player = 0;
		usedItem[i].item.X = 0;
		usedItem[i].item.Y = 0;
		usedItem[i].isUse = false;
	}

	step = eBattleStep::COMMAND;

	++turnNum;	// �^�[�����{�P

	commandStep = eCommand::CharaSelect;	// �L�����I������
	selectChara = -1;	// �v���C���[�̗v�f�ԍ�

	// �R�}���h�őI�������X�L���̏�����
	commandOrder.clear();
	commandOrder.shrink_to_fit();
}

// �R�}���h���
void Battle::Process_COMMAND() {

	// �y�e�L�X�g�\�����I���܂őҋ@�z
	if (TextBox::textBoxFlag) return;

	switch (commandStep){
	case eCommand::CharaSelect:// �y�L�����I���z
	{
		// ���̃L�����Ɉڍs
		++selectChara;

		// �A�C�e���I���̃A�C�R���̏�����
		selectItem.pos.X	= 0;
		selectItem.pos.Y	= 0;
		selectItem.flag		= false;

		for (int n = player.size(); selectChara < n; ++selectChara) {
			// ���̃L�������s���\���𔻒�
			if (player[selectChara]->CheckAbleAct()) {
				break;
			}
			else {
				commandOrder.push_back({ player[selectChara], skill_None, 0, 0 });	//�u�Ȃɂ����Ȃ��v�X�L����ǉ�
			}
		}

		// �S�v���C���[�̍s�������肵��������
		if (selectChara >= (int)player.size()) {
			// �G�̍s������
			for (int i = 0, n = enemy.size(); i != n; ++i) {
				enemy[i]->DecisionSkill(playerHate, enemy);
			}
			// �o�g���V�[���ֈڍs
			//step = eBattleStep::BATTLE;
			StepChange_BATTLE();
		}
		else {
			// �s���I���Ɉڍs
			commandStep = eCommand::ActionSelect;
			selectAction = commandPreSelect[selectChara].action;	// �J�[�\���͑O�^�[���ɑI�������ʒu�ɂ���
		}
		break;
	}
	case eCommand::ActionSelect:// �y�s���I���z
	{
		// suzuki_debug
		usedItem[selectChara].isUse = false;

		// �J�[�\���ړ�
		if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
			|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
			selectAction = (selectAction + (ActionNum - 1)) % this->ActionNum;
		}
		if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
			|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
			selectAction = (selectAction + 1) % ActionNum;
		}
		// ����
		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			commandPreSelect[selectChara].action = selectAction;	// �J�[�\���ʒu�ۑ�
			switch (selectAction) {
			case 0:// �y�U���z
				// �ʏ�U��
				selectSkillData = player[selectChara]->GetNomalAttack();

				selectSkillData->y = commandPreSelect[selectChara].targetY - (selectSkillData->height / 2);
				selectSkillData->x = commandPreSelect[selectChara].targetX - (selectSkillData->width / 2);

				// �`���[�W�␳�i�͈͊g��j
				if (player[selectChara]->GetCharge(eCharge::Range) != 1) {
					// �͈͑I���Ȃ�
					// �S�̍U��
					// ���̂܂܃X�L���I�[�_�[�ɃZ�b�g
					if (selectSkillData->totalUseEnergy == 0) {
						// �yMUE�s�v�z
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
					}
					else if (commandPreSelect[selectChara].fullPowerFlag) {
						// �y�S�́z
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
					}
					else {
						// �y�ʏ�z
						commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
					}
					// ���̃L������
					commandStep = eCommand::CharaSelect;
				}
				else {
					// �͈͑I����ʂ�
					commandStep = eCommand::TargetSelect;
				}
				break;
			case 1:// �y�X�L���z
				selectSkill = commandPreSelect[selectChara].skill;
				selectSkillCursor = 0;
				selectSkillArea = commandPreSelect[selectChara].skill;
				
				selectSkillList = player[selectChara]->GetSkillList_Battle();	// �I���L�����̐퓬�X�L�����X�g�擾
				skillNum = selectSkillList.size();

				// �X�L����0�Ȃ�ڍs���Ȃ�
				if (!selectSkillList.empty()) {
					if (selectSkill >= skillNum) {
						selectSkill = skillNum - 1;
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4;
					}
					else if (selectSkillArea + 5 >= skillNum) {
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4 - (skillNum - 1 - selectSkill);
					}
					// �X�L���I���Ɉڍs
					commandStep = eCommand::SkillSelect;
				}
				break;
			case 2:// �y�A�C�e���z
				// �A�C�e���I���Ɉڍs
				if (Player::bag[selectChara].empty()) {
					// �A�C�e�����������Ă��Ȃ��ꍇ
					TextBox::AddLine("�A�C�e�����������Ă��܂���E�E�E");	// �A�C�e�����������Ă��Ȃ��|��`����
				}
				else {
					// �A�C�e�����������Ă���ꍇ
					commandStep = eCommand::ItemSelect;	// �A�C�e���I���Ɉړ�
				}
				break;
			case 3:// �y�h��z
				commandOrder.push_back({ player[selectChara], skill_Defence, 0, 0 });	// �h��X�L����ǉ�
				// ���̃L������
				commandStep = eCommand::CharaSelect;
				break;
			case 4:// �y������z
				if (!encounter.escapeFlag) {
					// �����s��
					TextBox::AddLine("���̐퓬����͓������Ȃ��I");
					break;
				}
				commandOrder.push_back({ player[selectChara], skill_Escape, 0, 0 });	// �����X�L����ǉ�

				// ���̃L������
				commandStep = eCommand::CharaSelect;
				break;
			}
		}
		// �߂�
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {

			// ���̃R�}���h�L�������擪�łȂ����
			if (selectChara > 0) {
				int nowSelectChara = selectChara;	// ���̃R�}���h�L������ۑ����Ă���
				--selectChara;	// �L������1�߂�

				while (selectChara >= 0) {
					// ���̃L�������s���\���𔻒�
					if (player[selectChara]->CheckAbleAct()) {
						commandOrder.pop_back();	// �X�L���폜
						break;
					}
					--selectChara;	// �L������1�߂�
				}
				// �����L������-1�Ȃ�i�������O���S���s���s�\�j
				if (selectChara < 0) {
					selectChara = nowSelectChara;	// �R�}���h�L������߂��Ȃ�
				}
			}
		}
		break;

	}
	case eCommand::SkillSelect:// �y�g�p�X�L���I���z
	{
		// �y�J�[�\���ړ�
		// ��
		if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
			|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
			// �J�[�\����ړ�
			selectSkill = (selectSkill + (skillNum - 1)) % this->skillNum;
			// �X�L���̐����U�ȏォ�ŏ����𕪂���
			if (skillNum > 5) {
				if (selectSkillCursor == 0) {
					if (selectSkill == skillNum - 1) {
						selectSkillArea = skillNum - 5;
						selectSkillCursor = 4;
					}
					else {
						--selectSkillArea;
					}
				}
				else {
					--selectSkillCursor;
				}
			}
		}
		// ��
		if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
			|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
			// �J�[�\�����ړ�
			selectSkill = (selectSkill + 1) % skillNum;
			// �X�L���̐����U�ȏォ�ŏ����𕪂���
			if (skillNum > 5) {
				if (selectSkillCursor == 4) {
					if (selectSkill == 0) {
						selectSkillArea = 0;
						selectSkillCursor = 0;
					}
					else {
						++selectSkillArea;
					}
				}
				else {
					++selectSkillCursor;
				}
			}
		}
		// ��
		if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
			|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
			selectSkill -= 5;
			selectSkillArea -= 5;
			if (selectSkill < 0) {
				selectSkill = 0;
				selectSkillArea = 0;
				selectSkillCursor = 0;
			}
			if (selectSkillArea < 0) {
				selectSkillArea = 0;
				selectSkillCursor = selectSkill;
			}
		}
		// �E
		if (Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
			|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
			selectSkill += 5;
			selectSkillArea += 5;
			if (selectSkill >= skillNum) {
				selectSkill = skillNum-1;
				selectSkillArea = skillNum - 5;
				selectSkillCursor = 4;
			}
			else if (selectSkillArea + 5 >= skillNum) {
				selectSkillArea = skillNum - 5;
				selectSkillCursor = 4 - (skillNum-1 - selectSkill);
			}
		}
		// ����
		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			commandPreSelect[selectChara].skill = selectSkill;	// �J�[�\���ʒu�ۑ�
			selectSkillData = selectSkillList[selectSkill];

			// �͈͑I���t���O�Ŕ��� && �`���[�W�␳�i�͈͊g��j
			if (selectSkillData->selectFlag && player[selectChara]->GetCharge(eCharge::Range) == 1) {
				if (selectSkillData->selectTarget) {
				// �G�I���ֈڍs
				selectSkillData->y = commandPreSelect[selectChara].targetY - (selectSkillData->height / 2);
				selectSkillData->x = commandPreSelect[selectChara].targetX - (selectSkillData->width / 2);
				}
				else {
					// �����I���Ɉڍs
					selectTargetAlly = commandPreSelect[selectChara].selectTargetAlly;
				}
				commandStep = eCommand::TargetSelect;
			}
			else {
				// ���̂܂܃X�L���I�[�_�[�ɃZ�b�g
				if (selectSkillData->totalUseEnergy == 0) {
					// �yMUE�s�v�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// �y�S�́z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE)});
				}
				else {
					// �y�ʏ�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				
				// ���̃L������
				commandStep = eCommand::CharaSelect;
			}
		}
		// �߂�
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			commandStep = eCommand::ActionSelect;	// �s���I���ɖ߂�
		}
		// �ʏ� - �S�� �ؑ�
		if (Input::Get(VK_Q) == 1 || Input::Get(0, XINPUT_GAMEPAD_START) == 1) {
			commandPreSelect[selectChara].fullPowerFlag = !commandPreSelect[selectChara].fullPowerFlag;
		}
		break;
	}
	case eCommand::ItemSelect:// �y�g�p�A�C�e���I���z
	{
		if ((Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1)
			&& selectItem.pos.Y != 0) {
			// ��Ɉړ�
			--selectItem.pos.Y;
		}
		else if ((Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1)
			&& selectItem.pos.Y != Player::bag[selectChara].size() - 1) {
			// ���Ɉړ�
			++selectItem.pos.Y;
		}

		if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
			// ����
			if (!Player::bag[selectChara][selectItem.pos.Y]->GetSkill()) {
				TextBox::AddLine("�g���܂���");
				// �A�C�e���I���̃A�C�R���̏�����
				selectItem.pos.X = 0;
				selectItem.pos.Y = 0;
				selectItem.flag = false;

				commandStep = eCommand::ActionSelect;	// �s���I���ɖ߂�
				return;
			}
			// debug�@���󖡕��ɂ̂�
			usedItem[selectChara].isUse		= true;														// �g�p�\��̃A�C�e���Ƃ��ĕۑ�����
			usedItem[selectChara].player	= selectChara;												// �g�p�\��̃A�C�e���̃o�b�O��ۑ�����
			usedItem[selectChara].item.Y	= selectItem.pos.Y;											// �g�p�\��̃A�C�e���̃o�b�O���̍��W��ۑ�����
			selectItem.flag					= true;														// �A�C�e����I���������Ƃ�ۑ�����
			selectTargetAlly				= commandPreSelect[selectChara].selectTargetAlly;			// �����I���Ɉڍs
			selectSkillData					= Player::bag[selectChara][selectItem.pos.Y]->GetSkill();	// �X�L�����Z�b�g
			commandStep						= eCommand::TargetSelect;									// �ΏۑI����
		}

		// �߂�
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �A�C�e���I���̃A�C�R���̏�����
			selectItem.pos.X = 0;
			selectItem.pos.Y = 0;
			selectItem.flag = false;

			commandStep = eCommand::ActionSelect;	// �s���I���ɖ߂�
		}
		break;
	}
	case eCommand::TargetSelect:// �y�X�L�����̑Ώ۔͈͑I���z
	{
		// �y�G����I���z
		if (selectSkillData->selectTarget) {
			// �^�[�Q�b�g�ړ�
			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
				|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
				if (selectSkillData->y + selectSkillData->collision.size() > 1) {
					--selectSkillData->y;	// ��
				}
			}
			if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
				|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
				if (selectSkillData->y + 1 < enemyFieldHeight) {
					++selectSkillData->y;	// ��
				}
			}
			if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
				|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
				if (selectSkillData->x + selectSkillData->collision[0].size() > 1) {
					--selectSkillData->x;	// ��
				}
			}
			if (Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
				|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
				if (selectSkillData->x + 1 < enemyFieldWidth) {
					++selectSkillData->x;	// �E
				}
			}
			// ����
			if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				commandPreSelect[selectChara].targetY = selectSkillData->y + (selectSkillData->height / 2);
				commandPreSelect[selectChara].targetX = selectSkillData->x + (selectSkillData->width / 2);

				// �X�L���I�[�_�[�ɃZ�b�g
				if (selectSkillData->totalUseEnergy == 0) {
					// �yMUE�s�v�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// �y�S�́z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
				}
				else {
					// �y�ʏ�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				// ���̃L������
				commandStep = eCommand::CharaSelect;
			}
			// �߂�
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				commandStep = eCommand::ActionSelect;	// �s���I���ɖ߂�
			}
		}
		// �y��������I���z
		else {
			// �����̓}�X�I���ł͂Ȃ��L�����I��
			// �^�[�Q�b�g�ړ�
			if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
				|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
				selectTargetAlly = (selectTargetAlly + (player.size() - 1)) % player.size();
			}
			if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
				|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
				selectTargetAlly = (selectTargetAlly + 1) % player.size();
			}
			if (Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
				|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) {
				selectTargetAlly = 0;
			}
			if (	Input::Get(VK_RIGHT)						== 1	|| Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT)		== 1
				|| (Input::Get(VK_RIGHT)						> 10	&& Input::Get(VK_RIGHT) % 5						== 0)
				|| (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT)	> 10	&& Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) {
				selectTargetAlly = player.size() - 1;
			}
			// ����
			if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				// ���W�m��
				selectSkillData->y = 1;
				selectSkillData->x = 1 + selectTargetAlly * 3;

				commandPreSelect[selectChara].selectTargetAlly = selectTargetAlly;	// �R�}���h�̃Z�[�u

				// �X�L���I�[�_�[�ɃZ�b�g
				if (selectSkillData->totalUseEnergy == 0) {
					// �yMUE�s�v�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, 0 });
				}
				else if (commandPreSelect[selectChara].fullPowerFlag) {
					// �y�S�́z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, player[selectChara]->GetStatus(eStatus::MUE) });
				}
				else {
					// �y�ʏ�z
					commandOrder.push_back({ player[selectChara], selectSkillData, 0, selectSkillData->totalUseEnergy });
				}
				// ���̃L������
				commandStep = eCommand::CharaSelect;
			}
			// �߂�
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				commandStep = eCommand::ActionSelect;	// �s���I���ɖ߂�
			}
		}

		break;
	}
	}
}

// �o�g���V�[���ւ̈ڍs����
void Battle::StepChange_BATTLE() {
	step = eBattleStep::BATTLE;

	// �X�L���I�[�_�[�ɒǉ�
	for (int i = 0, n = commandOrder.size(); i < n; ++i) {
		skillOrder->Add(commandOrder[i].attacker, commandOrder[i].skill, commandOrder[i].totalUseEnergy);
	}

	for (int i = 0, n = player.size(); i < n; ++i) {
		player[i]->TurnStartProcess();
	}
	for (int i = 0, n = enemy.size(); i < n; ++i) {
		enemy[i]->TurnStartProcess();
	}

	// �R�}���h�őI�������X�L���̏�����
	commandOrder.clear();
	commandOrder.shrink_to_fit();
}

// ���C���퓬���
void Battle::Process_BATTLE() {

	// �y�e�L�X�g�\�����I���܂őҋ@�z
	if (TextBox::textBoxFlag) return;


	//////////////////////////////////////
	//////////// �y�X�L�������z////////////
	//////////////////////////////////////
	if (skillOrder->order.empty()) {
		// �y�X�L���I�[�_�[����Ȃ�z

		// �^�[���I������
		for (int i = 0, n = enemy.size(); i < n; i++) {
			enemy[i]->TurnEndProcess();
			// ���񂾂瓖���蔻��폜
			if (enemy[i]->GetSkillDeath()) {
				enemy[i]->StateReset();	// ��Ԃ̏�����
			}
		}
		for (int i = 0, n = player.size(); i < n; i++) {
			player[i]->TurnEndProcess();
			player[i]->UniqueProcess();		// �ŗL�����i����Ƃ��j
			// ���񂾂瓖���蔻��폜
			if (player[i]->GetSkillDeath()) {
				player[i]->StateReset();	// ��Ԃ̏�����
			}
		}
		
		// �t�B�[���hMP�̎�����
		if (fieldMP < fieldMaxMP) {
			IncreaseFieldMP((int)(fieldMaxMP * 0.1));	// �t�B�[���h�ő�MP��1/10��
			// �ő�MP�𒴂�����߂�
			if (fieldMP > fieldMaxMP) {
				fieldMP = fieldMaxMP;
			}
		}
		// �t�B�[���hMP�̎�������
		else if(fieldMP > fieldMaxMP){
			DecreaseFieldMP((int)(fieldMaxMP * 0.1));	// �t�B�[���h�ő�MP��1/10��
			// �ő�MP�𒴂�����߂�
			if (fieldMP < fieldMaxMP) {
				fieldMP = fieldMaxMP;
			}
		}

		// ���S���X�g�̓G�����S��ԂłȂ��Ȃ����烊�X�g����O��
		for (int i = 0, n = Enemy::deathList.size(); i < n; ++i) {
			if (enemy[Enemy::deathList[i]]->GetAlive() != eAlive::dead) {
				Enemy::deathList.erase(Enemy::deathList.begin() + i);
				--i;
				--n;
			}
		}

		DeleteDeadEnemy();	// ���񂾓G�̃f�[�^���폜

		StepChange_COMMAND();	// �R�}���h��ʂֈڍs
	}
	else{
		// �G����������
		for (int i = 0, n = enemy.size(); i != n; ++i) {
			enemy[i]->ReceiveSkillIni();
		}
		for (int i = 0, n = player.size(); i != n; ++i) {
			player[i]->ReceiveSkillIni();
		}

		vector<string>	message;
		// �y����ł���s�������Ȃ��z
		if (skillOrder->GetAttacker()->GetAlive() != eAlive::alive) {
			skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
			return;	//  �����ŏ����I��
		}
		// �y�X�L���̌��ʂ��e�L�X�g�{�b�N�X�ɕ\���z
		// �y��Ԉُ픻��z
		// �y��Ⴡz
		if (skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Paralysis).get) {
			// 1/2�ōs���s��
			if (GetRand(1) == 0) {
				message.push_back(skillOrder->GetAttacker()->GetName() + "�͐g�̂���Ⴢ��ē����Ȃ�");
				skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
				return;	//  �����ŏ����I��
			}
		}
		// �y�X�^���z
		if (skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Stun).get) {
			message.push_back(skillOrder->GetAttacker()->GetName() + "�͈ӎ��������Ă���");
			skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
		}
		// �y�����z
		else if(skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Sleep).get){
			message.push_back(skillOrder->GetAttacker()->GetName() + "�͖����Ă���");
			skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
		}
		// �y�����z
		else if(skillOrder->GetAttacker()->GetBadStatus(eBadStatus::Confusion).get) {
			message.push_back(skillOrder->GetAttacker()->GetName() + "�͐��C�������Ă���");
			// �����_���s��
			switch (GetRand(2)){
			case 0:// �y�N���ɍU���z
			{
				int targetNum = 0;	// �U���Ώۂ̃C���f�b�N�X
				Skill* ConfusionSkillData = skillOrder->GetAttacker()->GetConfusionSkill();	// �������̍U���p�X�L�����擾
				message.push_back(skillOrder->GetAttacker()->GetName() + "�͓ˑR�\�ꂾ����");

				if (GetRand(1) == 0) {
					// �y�v���C���[�z
					// �U���Ώۂ��v���C���[�Ɂi�U���҂��v���C���[�Ȃ�false,�G�Ȃ�true�j
					ConfusionSkillData->selectTarget = !skillOrder->GetAttacker()->GetCharType();
					
					do {
						targetNum = GetRand(player.size() - 1);	// �v���C���[���烉���_���ɒ��o
					} while (player[targetNum]->GetAlive() != eAlive::alive);	// �Ώۂ������Ă��Ȃ���΍Ē��I

					// x2y3�̒��̂ǂꂩ�̃}�X��I��
					ConfusionSkillData->x = (player[targetNum]->GetX() - (int)(ConfusionSkillData->width * 0.5) + GetRand(1));
					ConfusionSkillData->y = (player[targetNum]->GetY() - (int)(ConfusionSkillData->height * 0.5) + GetRand(2));

				}
				else {
					// �y�G�z
					// �U���Ώۂ�G�Ɂi�U���҂��v���C���[�Ȃ�true,�G�Ȃ�false�j
					ConfusionSkillData->selectTarget = skillOrder->GetAttacker()->GetCharType();
					do {
						targetNum = GetRand(enemy.size() - 1);	// �G���烉���_���ɒ��o
					} while (enemy[targetNum]->GetAlive() != eAlive::alive);	// �Ώۂ������Ă��Ȃ���΍Ē��I

					// �G�̏c�������烉���_���Ƀ}�X��I��
					ConfusionSkillData->x = (enemy[targetNum]->GetX() - (int)(ConfusionSkillData->width * 0.5) + GetRand(enemy[targetNum]->GetWidth() - 1));
					ConfusionSkillData->y = (enemy[targetNum]->GetY() - (int)(ConfusionSkillData->height * 0.5) + GetRand(enemy[targetNum]->GetHeight() - 1));
					
				}
				// �X�L������
				SkillExec(skillOrder->GetAttacker(), ConfusionSkillData);
				break;
			}
			case 1:// �y�s���s�z
				message.push_back(skillOrder->GetAttacker()->GetName() + "�͐k���ē����Ȃ�");

				break;
			case 2:// �y�C��z
				//message.push_back(skillOrder->GetAttacker()->GetName() + "�͋C�₵��");
				// ��Ԉُ�y�X�^���z�ɂ���
				skillOrder->GetAttacker()->GiveBadStatus(eBadStatus::Stun, 0);
				break;
			}
			skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
		}
		// �y�s���\�z
		else {
			// �X�L���̔���
			switch (skillOrder->GetSkill()->type){
			case eSkillType::Nomal:// �y�ʏ�X�L���z

				if (skillStep) {
					// �X�L���g�p���e�L�X�g
					if (!skillOrder->GetSkill()->utilizationText.empty()) {
						// 1�s��
						message.push_back(skillOrder->GetAttacker()->GetName() + skillOrder->GetSkill()->utilizationText[0]);
						// 2�s�ڈȍ~
						for (int i = 1, n = skillOrder->GetSkill()->utilizationText.size(); i < n; ++i) {
							message.push_back(skillOrder->GetSkill()->utilizationText[i]);
						}
					}
					// �X�L���������\�����ׂ�
					if (CheckAbleSkill(skillOrder->GetOrder())) {
						// ��������
						if (skillOrder->GetSkill()->selectFlag && 
							skillOrder->GetSkill()->selectTarget == skillOrder->GetAttacker()->GetCharType() &&
							skillOrder->GetSkill()->grEffect.flag) {
							graphicEffect.push_back(new DivGraphicEffect(enemyFieldX + skillOrder->GetSkill()->x * cellSize, enemyFieldY + skillOrder->GetSkill()->y * cellSize, skillOrder->GetSkill()->grEffect.gr, skillOrder->GetSkill()->grEffect.divX, skillOrder->GetSkill()->grEffect.divY, skillOrder->GetSkill()->grEffect.divNum, skillOrder->GetSkill()->width, skillOrder->GetSkill()->height));
						}
						skillOrder->GetSkill()->PlayerSE();	// SE��炷
					}
					else {
						// �������s
						message.push_back("MP������Ȃ�");
					}
					if (!message.empty()) {
						TextBox::AddLine(message, 0, 10, 3*5+30);
					}
					skillStep = false;
					return;
				}
				else {
					// �X�L������
					SkillExec(skillOrder->GetOrder());
					skillStep = true;
				}
				break;
			case eSkillType::Confusion:// �y�������̃X�L���z
				// �o�O�łȂ���΂����ɂ��邱�Ƃ͂Ȃ�
				// �Ƃ肠�����u�Ȃɂ����Ȃ��v�Ɠ��������������Ƃ�
				skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
				break;
			case eSkillType::None:// �y�Ȃɂ����Ȃ��z
				skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
				break;
			case eSkillType::Defence:// �y�h��z
				skillOrder->GetAttacker()->Defence();		// �Ώۂ�h���Ԃ�
				message.push_back(skillOrder->GetAttacker()->GetName() + "�͐g������Ă���");
				// �w�C�g�l�v�Z
				if (skillOrder->GetAttacker()->GetCharType()) {
					// �w�C�g�l��0.9�{��
					playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] = (int)(playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] * 0.9);
					// �w�C�g�l����	1�����ɂȂ�����1��
					if (playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] < 1) {
						playerHate[skillOrder->GetAttacker()->GetPlayerOrder()] = 1;
					}
				}

				skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
				break;
			case eSkillType::Escape:// �y�����z
				message.push_back(skillOrder->GetAttacker()->GetName() + "�͓����o����");
				if (skillOrder->GetAttacker()->GetCharType()) {
					// �y�v���C���[�z
					if (!encounter.escapeFlag) {
						// �����s�퓬
						message.push_back("�������A���̐퓬����͓������Ȃ��I");
						break;
					}
					// ������25%�@����������퓬�I��
					if (GetRand(100) < 25) {
						TextBox::AddLine(message, 0, 10, 50);
						battleEndType = eEndStatus::Escape;	// ��������
						step = eBattleStep::END;	// �I����ʂɈڍs

						DeleteDeadEnemy();	// ���񂾓G�̃f�[�^���폜

						EndText();	// �퓬�I���e�L�X�g��W��
						return;		//  �����ŏ����I��
					}
					else {
						message.push_back("��������������Ȃ�����");
					}
				}
				else {
					// �y�G�z
					// ���܂̂Ƃ���G�͊m�蓦��
					skillOrder->GetAttacker()->GiveBadStatus(eBadStatus::Extinction, 0);	// �G�����ł�����
				}

				skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
				break;
			}
		}

		// �X�L�����ʃ��b�Z�[�W�W��
		// �G
		for (int i = 0, n = enemy.size(); i < n; i++) {
			enemy[i]->GetEffectText(message);
		}
		// �v���C���[
		for (int i = 0, n = player.size(); i < n; i++) {
			player[i]->GetEffectText(message);
		}
		// �\��������̂��������
		//if (message.size() == 1) {
		//	message.push_back("���ʂ��Ȃ�����");
		//}
		if (!message.empty()) {
			TextBox::AddLine(message, 0, 10, -1);
		}


		// ���񂾂瓖���蔻��폜
		for (int i = 0, n = enemy.size(); i < n; i++) {
			if (enemy[i]->GetSkillDeath()) {
				//enemy[i]->DeleteColision(enemyField);
				enemy[i]->StateReset();	// ��Ԃ̏�����
				// �험�i�擾
				exp += enemy[i]->GetEXP();		// �o���l
				money += enemy[i]->GetMoney();	// ��

				enemy[i]->StateReset();	// ��Ԃ̏�����

				// ���S�����G�̏������S�҃��X�g�ɓo�^
				Enemy::deathList.push_back(i);
			}
		}
		for (int i = 0, n = player.size(); i < n; i++) {
			if (player[i]->GetSkillDeath()) {
				//player[i]->DeleteColision(playerField);
				playerHate[i] = 0;	// �w�C�g��0�ɂ���
				player[i]->StateReset();	// ��Ԃ̏�����
			}
		}
	}
	// �y�v���C���[�S�Ŕ���z
	if (CheckPlayersDead()) {
		battleEndType = eEndStatus::Lose;	// �S��
		step = eBattleStep::END;	// �I����ʂɈڍs

		EndText();	// �퓬�I���e�L�X�g��W��
		return;
	}
	// �y�G�S�Ŕ���z
	if (CheckEnemysDead()) {
		battleEndType = eEndStatus::Win;	// �퓬����
		step = eBattleStep::END;	// �I����ʂɈڍs

		DeleteDeadEnemy();	// ���񂾓G�̃f�[�^���폜

		EndText();	// �퓬�I���e�L�X�g��W��
		return;
	}
	
}
// �퓬�I�����
void Battle::Process_END() {
	// �y�e�L�X�g�\�����I���܂őҋ@�z
	if (TextBox::textBoxFlag) return;
	
	// �퓬�I���̏�Ԃŕ���
	switch (battleEndType)
	{
	case eEndStatus::Win:// �y�����z
		mode = StageMode::FIELD;	// �t�B�[���h�Ɉڍs
		break;
	case eEndStatus::Lose:// �y�S�Łz
		if (encounter.loseEventFlag) {
			// �����C�x�Ȃ�t�B�[���h�Ɉڍs
			mode = StageMode::FIELD;
		}
		else {
			// �Q�[���I�[�o�[��ʂ�
			mode =  StageMode::GAMEOVER;
		}
		break;
	case eEndStatus::Escape:// �y�����z
		mode = StageMode::FIELD;	// �t�B�[���h�Ɉڍs
		break;
	default:// �G���[
		// �Q�[���I�[�o�[��ʂ�
		mode = StageMode::GAMEOVER;
		break;
	}
	
	// �C�x���g�퓬��Ԃ̎󂯓n������
	if (encounter.battleFlag != NULL) {
		(*encounter.battleFlag) = false;
	}
	if (encounter.battleEndType != NULL) {
		(*encounter.battleEndType) = battleEndType;
	}
	EncounterReset();	// �G���J�E���^�[�̃��Z�b�g

}

// �I����ʂ̕\��
void Battle::EndText() {
	vector<string>	message;
	// �퓬�I���̏�Ԃŕ���
	switch (battleEndType)
	{
	case eEndStatus::Win:// �y�����z
		// �y�퓬�����z
		for (int i = 0, n = (int)player.size(); i < n; i++) {
			player[i]->StateReset();		// ��Ԃ̏�����
			player[i]->TurnEndProcess();	// �^�[���I������
		}
		message.push_back("�G��|����");
		message.push_back(to_string(exp) + "�̌o���l���l��");
		message.push_back(to_string(money) + "�f����ɓ��ꂽ");
		// �A�C�e���h���b�v
		for (int i = 0, n = (int)enemy.size(); i < n; ++i) {
			// �h���b�v����
			if (GetRand(100) <= enemy[i]->GetDropRate()) {
				// �A�C�e�������Ă邩����
				if (Player::InsertItem(enemy[i]->GetDropItem())) {
					message.push_back(enemy[i]->GetName() + " ���� " + Item::GetName(enemy[i]->GetDropItem()) + " ����ɓ��ꂽ");
				}
			}
		}

		TextBox::AddLine(message, 0, 20, 50);
		
		for (int i = 0, n = (int)player.size(); i != n; ++i) {
			// �����Ă���Όo���l���擾
			if (player[i]->GetAlive() == eAlive::alive) {
				player[i]->AddEXP(exp);
			}
		}
		// ������ɓ����
		Player::AddMoney(money);
		break;
	case eEndStatus::Lose:// �y�S�Łz
		message.push_back("�S�ł����c");
		break;
	case eEndStatus::Escape:// �y�����z
		message.push_back(to_string(exp) + "�̌o���l���l��");
		message.push_back(to_string(money) + "�f����ɓ��ꂽ");
		TextBox::AddLine(message, 0, 20, 50);

		for (int i = 0, n = (int)player.size(); i != n; ++i) {
			// �����Ă���Όo���l���擾
			if (player[i]->GetAlive() == eAlive::alive) {
				player[i]->AddEXP(exp);
			}
		}
		break;
	default:
		break;
	}
	
}
// �X�L���������\�����ׂ�
bool Battle::CheckAbleSkill(tSkillOrder order) {
	// �y����MP����z
	if (order.skill->useMP[0] > (unsigned)order.attacker->GetStatus(eStatus::MP) || order.skill->useMP[1] > (unsigned)fieldMP) {
		return false;		// �������s
	}
	return true;
}
// �ʏ�X�L������
bool Battle::SkillExec(tSkillOrder order) {
	skillOrder->Pop_Back();	// �X�L���I�[�_�[�������
	
	// �y�X�L�������z
	for (int r = 0, l = order.skill->effect.size(); r < l; ++r) {
		switch (order.skill->effect[r]->type){
		//////////////////////////////////////
		////// �y�͈͑I��������X�L���z////////
		//////////////////////////////////////
		default:
		// �y�U���z
		// �y�񕜁z
		// �y��Ԉُ�z
		// �y�o�t�E�f�o�t�z
		// �y�h���z
		{
			// �Ώ۔͈͑I��
			switch (order.skill->effect[r]->targetType) {
			case eSkillTarget::Self:// �y�����z
			{
				// �X�L���Ώەۑ�������
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				// �����ɃX�L���𔭓�
				SkillEffectExec(order.attacker->GetX(), order.attacker->GetY(), order.attacker, order.attacker, order.skill->effect[r], order.totalUseEnergy, r);

				break;
			}
			case eSkillTarget::Enemy:// �y�G�S�́z
			{
				// �X�L���Ώەۑ�������
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				if (order.attacker->GetCharType()) {
					// �v���C���[���猩���G���G
					// �G�S�̂ɃX�L�����g��
					for (int i = 0; i != enemyFieldHeight; ++i) {
						for (int j = 0; j != enemyFieldWidth; ++j) {
							graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (enemyField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				else {
					// �G���猩���G���v���C���[
					// �����S�̂ɃX�L�����g��
					for (int i = 0; i != playerFieldHeight; ++i) {
						for (int j = 0; j != playerFieldWidth; ++j) {
							//graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (playerField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Ally:// �y�����S�́z
			{
				// �X�L���Ώەۑ�������
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				if (order.attacker->GetCharType()) {
					// �v���C���[���猩���������v���C���[
					// �����S�̂ɃX�L�����g��
					for (int i = 0; i != playerFieldHeight; ++i) {
						for (int j = 0; j != playerFieldWidth; ++j) {
							//graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit, 10));
							if (playerField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				else {
					// �G���猩���������G
					// �G�S�̂ɃX�L�����g��
					for (int i = 0; i != enemyFieldHeight; ++i) {
						for (int j = 0; j != enemyFieldWidth; ++j) {
							graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
							if (enemyField[i][j] != nullptr) {
								SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Overall:// �y�G�����S�́z
			{
				// �X�L���Ώەۑ�������
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();


				// �G�S�̂ɃX�L�����g��
				for (int i = 0; i != enemyFieldHeight; ++i) {
					for (int j = 0; j != enemyFieldWidth; ++j) {
						graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
						if (enemyField[i][j] != nullptr) {
							SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				// �����S�̂ɃX�L�����g��
				for (int i = 0; i != playerFieldHeight; ++i) {
					for (int j = 0; j != playerFieldWidth; ++j) {
						if (playerField[i][j] != nullptr) {
							SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				break;
			}
			case eSkillTarget::Select:// �y�I��͈́z
			{
				// �X�L���Ώەۑ�������
				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						playerField_Save[i][j] = false;
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						enemyField_Save[i][j] = false;
					}
				}
				target_Save.clear();
				target_Save.shrink_to_fit();

				// �I��͈͂ɃX�L�����g��
				if (order.skill->selectTarget != order.attacker->GetCharType()) {
					// �v���C���[���������ƌ����΃v���C���[�@�G���G���ƌ����΃v���C���[
					// �v���C���[

					// �y�`���[�W�␳�i�͈͊g��j�z
					if (order.attacker->GetCharge(eCharge::Range) != 1) {
						// �v���C���[�S��
						for (int i = 0; i != playerFieldHeight; ++i) {
							for (int j = 0; j != playerFieldWidth; ++j) {
								if (playerField[i][j] != nullptr) {
									SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
								}
							}
						}
					}
					else {
						// �I��͈�
						for (int i = 0, n = order.skill->effect[r]->collision.size(); i != n; ++i) {
							for (int j = 0, m = order.skill->effect[r]->collision[i].size(); j != m; ++j) {
								if (order.skill->effect[r]->collision[i][j]) {
									// �A�E�g�I�u�����W�΍�
									if (i + order.skill->y >= 0 && i + order.skill->y < playerFieldHeight &&
										j + order.skill->x >= 0 && j + order.skill->x < playerFieldWidth) {

										//graphicEffect.push_back(new GraphicEffect(enemyFieldX + (j + order.skill->x) * cellSize, enemyFieldY + (i + order.skill->y) * cellSize, gr_SkillHit2, 10));
										if (playerField[i + order.skill->y][j + order.skill->x] != nullptr) {
											SkillEffectExec(j + order.skill->x, i + order.skill->y, order.attacker, playerField[i + order.skill->y][j + order.skill->x], order.skill->effect[r], order.totalUseEnergy, r);
										}
									}
								}
							}
						}
					}

				}
				else {
					// �v���C���[���G���ƌ����ΓG�@�G���������ƌ����ΓG
					// �G

					// �y�`���[�W�␳�i�͈͊g��j�z
					if (order.attacker->GetCharge(eCharge::Range) != 1) {
						// �G�S��
						for (int i = 0; i != enemyFieldHeight; ++i) {
							for (int j = 0; j != enemyFieldWidth; ++j) {
								graphicEffect.push_back(new GraphicEffect(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_SkillHit2, 10));
								if (enemyField[i][j] != nullptr) {
									SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
								}
							}
						}
					}
					else {
						// �I��͈�
						// �G�t�F�N�g
						//graphicEffect.push_back(new DivGraphicEffect(enemyFieldX + order.skill->x * cellSize, enemyFieldY + order.skill->y * cellSize, gr_NA, 3));
						for (int i = 0, n = order.skill->effect[r]->collision.size(); i != n; ++i) {
							for (int j = 0, m = order.skill->effect[r]->collision[i].size(); j != m; ++j) {
								if (order.skill->effect[r]->collision[i][j]) {
									// �A�E�g�I�u�����W�΍�
									if (i + order.skill->y >= 0 && i + order.skill->y < enemyFieldHeight &&
										j + order.skill->x >= 0 && j + order.skill->x < enemyFieldWidth) {
										if (enemyField[i + order.skill->y][j + order.skill->x] != nullptr) {
											SkillEffectExec(j + order.skill->x, i + order.skill->y, order.attacker, enemyField[i + order.skill->y][j + order.skill->x], order.skill->effect[r], order.totalUseEnergy, r);
										}
									}
								}
							}
						}
					}
				}
				break;
			}
			case eSkillTarget::Previous:// �y�����p���z
			{
				// �X�L���Ώەۑ�������
				target_Save.clear();
				target_Save.shrink_to_fit();

				for (int i = 0; i < playerFieldHeight; i++) {
					for (int j = 0; j < playerFieldWidth; j++) {
						if (playerField_Save[i][j]) {
							SkillEffectExec(j, i, order.attacker, playerField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				for (int i = 0; i < enemyFieldHeight; i++) {
					for (int j = 0; j < enemyFieldWidth; j++) {
						if (enemyField_Save[i][j]) {
							SkillEffectExec(j, i, order.attacker, enemyField[i][j], order.skill->effect[r], order.totalUseEnergy, r);
						}
					}
				}
				break;
			}
			}
			// ************************************** //
			// **�y���߃_���[�W�ɂ���Ԉُ�t�^�z** //
			// ************************************** //
			if (order.skill->effect[r]->type == eSkillEffect::Attack) {
				switch (order.skill->effect[r]->energyConv.type) {
				case eEnergy::Heat:// �y�M�z
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-
						// �ϊ��ʁi�������݁j��100�ȏ�ő����t�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Heat, false)) - 100;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Death, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					break;
				case eEnergy::Elec:// �y�d�C�z
					if (order.skill->effect[r]->energyConv.direction) { //+
						// �ϊ��ʁi�������݁j��10�ȏ�Ŗ�ვt�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Elec, true)) - 10;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Paralysis, value * 10, (unsigned int)(value * 0.1)});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Shine:// �y���z
					if (order.skill->effect[r]->energyConv.direction) { //+
						// �ϊ��ʁi�������݁j��50�ȏ�ňÈŕt�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Shine, true)) - 50;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Blind, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-
						// �ϊ��ʁi�������݁j��50�ȏ�ňÈŕt�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Shine, false)) - 50;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Blind, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					break;
				case eEnergy::Sound:// �y���z
					if (order.skill->effect[r]->energyConv.direction) { //+
						// �ϊ��ʁi�������݁j��100�ȏ�ō����t�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Sound, true)) - 100;
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Confusion, value, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Motion:// �y�^���z
					if (order.skill->effect[r]->energyConv.direction) { //+
					}
					else { //-

					}
					break;
				case eEnergy::Gravity:// �y�d�́z
					if (order.skill->effect[r]->energyConv.direction) { //+
						// �ϊ��ʁi�������݁j��0�ȏ�ō����t�^
						int value = (int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(eEnergy::Gravity, true));
						if (value > 0) {
							SkillEffect_BadStatus* Addition = new SkillEffect_BadStatus({ eBadStatus::Stun, value * 10, 0});
							for (int i = 0, n = target_Save.size(); i < n; ++i) {
								SkillEffectExec(target_Save[i]->GetX(), target_Save[i]->GetY(), order.attacker, target_Save[i], Addition, order.totalUseEnergy, r);
							}
							delete Addition;
							Addition = nullptr;
						}
					}
					else { //-

					}
					break;
				case eEnergy::Life:// �y�����z
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				case eEnergy::Static:// �y�Î~�z
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				case eEnergy::Nucleus:// �y�j�z
					if (order.skill->effect[r]->energyConv.direction) { //+

					}
					else { //-

					}
					break;
				}

			}
			
			// ************************************** //
			// *�y�G�l���M�[�ϊ��ɂ�閂�f�ʂ̑����z* //
			// ************************************** //
			// �ϊ��Ȃ��łȂ����
			if (order.skill->effect[r]->energyConv.type != eEnergy::None) {
				// �ϊ�����
				if (order.skill->effect[r]->energyConv.direction) { // �y+�ϊ��z
					switch (order.skill->effect[r]->energyConv.convType) { // �g�pMP
					case eConvType::Self:// �y�X�L���g�p�҂�MP�z
						// �X�L���g�p�҂�MP��ϊ��ʕ�����(�ϊ��ʂ��̂܂�)
						order.attacker->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));

						break;
					case eConvType::Field:// �y�t�B�[���h��MP�z
						// �t�B�[���h��MP��ϊ��ʕ�����(�ϊ��ʂ��̂܂�)
						DecreaseFieldMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
						break;
					case eConvType::Target:// �y�X�L���Ώۂ�MP�z
						for (int i = 0, n = target_Save.size(); i < n; ++i) {
							// �Ώۂ������Ȃ甼�����Ȃ�
							if (target_Save[i] == order.attacker) {
								// �X�L���Ώۂ�MP��ϊ��ʕ�����(�ϊ���)
								target_Save[i]->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
							}
							else {
								// �X�L���Ώۂ�MP��ϊ��ʕ�����(�ϊ���)

								// MUE�΍R
								// �{�����P�ȏ�Ȃ猸���Ȃ�
								if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) >= 1) {
									target_Save[i]->DecreaseMP((int)GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy));
								}
								// �{�����O�ȏ�Ȃ炻�̕�����
								else if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) > 0) {
									target_Save[i]->DecreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * (100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100))));
								}
							}
						}
						break;
					case eConvType::None:// �y// �G�l���M�[�ϊ��ȊO�̌��ہz
						break;
					default:
						break;
					}
				}
				else { // �y-�ϊ��z
					switch (order.skill->effect[r]->energyConv.convType) {
					case eConvType::Self:// �y�X�L���g�p�҂�MP�z
						// �����ۗ̕L����G�l���M�[��-�ϊ�

						// �X�L���ΏۂŔh��
						if (order.skill->effect[r]->targetType == eSkillTarget::Self) {
							// �������Ώ�
							// �ϊ��������f�͎�������荞��
							// ������MP��ϊ��ʕ�����(�ϊ��ʁ~�ϊ�����)
							order.attacker->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						}
						else {
							// �G or �t�B�[���h
							// �ϊ��������f�͏�ɕY��
							// �t�B�[���h��MP��ϊ��ʕ�����(�ϊ��ʁ~�ϊ�����)
							IncreaseFieldMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						}
						break;
					case eConvType::Field:// �y�t�B�[���h��MP�z
						// ��ɂ���G�l���M�[��-�ϊ�
						// �ϊ��������f�͏�ɕY��

						// �t�B�[���h��MP��ϊ��ʕ�����(�ϊ��ʁ~�ϊ�����)
						IncreaseFieldMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
						break;
					case eConvType::Target:// �y�X�L���Ώۂ�MP�z
						// ���肪���G�l���M�[�Ɋ����Ē���-�ϊ�
						// ���ɂ̓R�X�g���K�v�Ȃ��߁A�ϊ��ʂ���������
						// �ϊ��������f�͑���ɒ~�ς���iMP��������j

						// �X�L���ΏۂƂȂ�������S���񂷁i���S�҂��܂߂�j
						for (int i = 0, n = target_Save.size(); i < n; ++i) {
							// �Ώۂ������Ȃ甼�����Ȃ�
							if (target_Save[i] == order.attacker) {
								// ������MP��ϊ��ʕ�����(�ϊ��ʁ~�ϊ�����)
								order.attacker->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
							}
							else {
								// �X�L���Ώۂ�MP��ϊ��ʕ�����(�ϊ��ʁ~�ϊ�����)
								//target_Save[i]->IncreaseMP((int)(GetConvValue(skill->effect[r]->energyConv, order.totalUseEnergy) * attacker->GetEfficiency(skill->effect[r]->energyConv.type, skill->effect[r]->energyConv.direction)));

								// MUE�΍R
								// �{�����P�ȏ�Ȃ猸���Ȃ�
								if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) >= 1) {
									target_Save[i]->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
								}
								// �{�����O�ȏ�Ȃ炻�̕�����
								else if ((100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) > 0) {
									target_Save[i]->IncreaseMP((int)(GetConvValue(order.skill->effect[r]->energyConv, order.totalUseEnergy) * (100 - (cos((M_PI / 2000) * (order.attacker->GetStatus(eStatus::MUE) - target_Save[i]->GetStatus(eStatus::MUE))) * 100)) * order.attacker->GetEfficiency(order.skill->effect[r]->energyConv.type, order.skill->effect[r]->energyConv.direction)));
								}
							}
						}
						break;
					case eConvType::None:// �y// �G�l���M�[�ϊ��ȊO�̌��ہz
						break;
					default:
						break;
					}
				}
			}
			break;
		}

		//////////////////////////////////////
		////// �y�͈͑I�����Ȃ��X�L���z////////
		//////////////////////////////////////
		case eSkillEffect::AnotherSkill:// �y���̃X�L�������z
		{
			// ��������
			if (GetRand(99) < order.skill->effect[r]->GetAnotherSkill()->invocation) {
				// �X�L����������
				// �X�L���I�[�_�[�ɒǉ�
				skillOrder->Add_Velocity(order.attacker, order.skill->effect[r]->GetAnotherSkill()->skill, order.skill->effect[r]->GetAnotherSkill()->skill->totalUseEnergy, order.skill->effect[r]->GetAnotherSkill()->fastFlag);

				// ��������X�L���Ƀt�B�[���h�I�����K�v�Ȃ�A�����I�����Ă�����W�ɂ���
				if (order.skill->effect[r]->GetAnotherSkill()->skill->selectFlag) {
					order.skill->effect[r]->GetAnotherSkill()->skill->x = (order.skill->x + (int)(order.skill->width * 0.5)) - (int)(order.skill->effect[r]->GetAnotherSkill()->skill->width * 0.5);
					order.skill->effect[r]->GetAnotherSkill()->skill->y = (order.skill->y + (int)(order.skill->height * 0.5)) - (int)(order.skill->effect[r]->GetAnotherSkill()->skill->height * 0.5);
				}
			}
			break;
		}
		case eSkillEffect::Hate:// �y�w�C�g�����z
		{
			if (order.attacker->GetCharType()) {
				// �y�v���C���[���g�p�����ꍇ�z
				// �w�C�g�l�̑���
				playerHate[order.attacker->GetPlayerOrder()] += order.skill->effect[r]->GetHate()->value;			// �萔
				playerHate[order.attacker->GetPlayerOrder()] = 
					(int)(playerHate[order.attacker->GetPlayerOrder()] * order.skill->effect[r]->GetHate()->rate);	// �{��

				// �w�C�g�l���
				if (playerHate[order.attacker->GetPlayerOrder()] > maxHate) {
					playerHate[order.attacker->GetPlayerOrder()] = maxHate;
				}
				// �w�C�g�l����
				if (playerHate[order.attacker->GetPlayerOrder()] < 1) {
					playerHate[order.attacker->GetPlayerOrder()] = 1;
				}
			}
			break;
		}
		case eSkillEffect::Charge:// �y�`���[�W�z
		{
			// �`���[�W�X�L�����Z�b�g
			order.attacker->SetCharge(order.skill->effect[r]->GetCharge()->type, order.skill->effect[r]->GetCharge()->rate, order.skill->effect[r]->GetCharge()->turn);
			break;
		}
		case eSkillEffect::Mode:// �y���[�h�ύX�z
		{
			if (!order.attacker->GetCharType()) {
				// �y�G�̏����z
				// ���[�h�̕ύX
				order.attacker->ModeChange(order.skill->effect[r]->GetMode()->modeNum);
			}
			break;
		}
		}
	}

	return true;	// ��������
}
bool Battle::SkillExec(Character* attacker, Skill* skill) {
	return SkillExec({ attacker, skill, 0, skill->totalUseEnergy });
}

// �ΏۂɃX�L���̌��ʂ�^����
void Battle::SkillEffectExec(int FieldX, int FieldY, Character* attacker, Character* target, SkillEffect* effect, int totalUseEnergy, int effectNum) {
	
	switch (effect->type) {
	case eSkillEffect::Attack:
		//////////////////////////////////////
		////////////// �y�U���z////////////////
		//////////////////////////////////////
	{
		// �Ώۂ�����ł���U�����Ȃ�
		if (target->GetAlive() != eAlive::alive) return;


		// �U���ŗ^�����_���[�W
		int damageValue = 0;

		target->SetReceiveSkill(eSkillEffect::Attack);	// �_���[�W�t���O
		// �U����
		for (int r = 0, l = GetRand(effect->GetAttack()->times_min, effect->GetAttack()->times_max); r < l; r++) {
			damageValue = 0;	// �_���[�W��������

			damageValue += EnergyConvDmage(attacker, target, effect->energyConv, effect->GetAttack()->hit, effect->GetAttack()->damageRate, totalUseEnergy);	// �_���[�W�ʕۑ�			
			
			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l���Z�i�_���[�W���~�P�j
				playerHate[attacker->GetPlayerOrder()] += damageValue;
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
			//****�y�X�L���Ώێ҂̏����z****//
			// �y�v���C���[�z
			if (target->GetCharType()) {
				// �_���[�W���\��
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), damageValue, 20, RED, r * 10);
				
				// �_���[�W����-1�łȂ����
				if (damageValue != -1) {
					// �X�L���Ώەۑ�
					playerField_Save[FieldY][FieldX] = true;
					{
						int i = 0, n = target_Save.size();
						// �Z�[�u�ς��𔻒�
						for (; i < n; ++i) {
							if (target_Save[i] == target) {
								break;
							}
						}
						if (i == n) target_Save.push_back(target);
					}
				}
			}
			// �y�G�z
			else {
				// �_���[�W���\��
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), damageValue, 20, RED, effectNum * 30 + r * 10);
				// �_���[�W�����O�łȂ���� && �Ώۂ������Ă�Εۑ�
				if (damageValue != -1) {
					// �X�L���Ώەۑ�
					enemyField_Save[FieldY][FieldX] = true;
					{
						int i = 0, n = target_Save.size();
						// �Z�[�u�ς��𔻒�
						for (; i < n; ++i) {
							if (target_Save[i] == target) {
								break;
							}
						}
						if (i == n) target_Save.push_back(target);
					}
				}
			}
		}
		break;
	}
	case eSkillEffect::Heal:
		//////////////////////////////////////
		////////////// �y�񕜁z////////////////
		//////////////////////////////////////
	{
		// �Ώۂ�����ł���񕜂��Ȃ�
		if (target->GetAlive() != eAlive::alive) return;

		int healValue = 0;	// �񕜗ʏ�����
		switch (effect->GetHeal()->type){
		case eHealType::HP:// �yHP�񕜁z
		{
			// �Ώۉ񕜁i�␳12�j
			healValue += target->Recovery(GetConvValue(effect->energyConv, totalUseEnergy) * effect->GetHeal()->rate * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(eEnergy::Life, true)) * 12);

			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l���Z�i�񕜗ʁ~2�j
				playerHate[attacker->GetPlayerOrder()] += healValue * 2;
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// �y�X�L���Ώێ҂̏����z****//
			// �y�v���C���[�����z
			if (target->GetCharType()) {
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), healValue, 20, YELLOW);
			}
			// �y�G�����z
			else {
				// �_���[�W���\��
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), healValue, 20, YELLOW);
			}
			break;
		}
		case eHealType::MP:// �yMP�񕜁z
			/*

			*/
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
			/*
			
			*/
			break;
		}	
		// �y�X�L���Ώێ҂̏����z****//
		// �y�v���C���[�����z
		if (target->GetCharType()) {

			playerField_Save[FieldY][FieldX] = true;
			{
				int i = 0, n = target_Save.size();
				// �Z�[�u�ς��𔻒�
				for (; i < n; ++i) {
					if (target_Save[i] == target) {
						break;
					}
				}
				if (i == n) target_Save.push_back(target);
			}
		}
		// �y�G�����z
		else {
			enemyField_Save[FieldY][FieldX] = true;
			{
				int i = 0, n = target_Save.size();
				// �Z�[�u�ς��𔻒�
				for (; i < n; ++i) {
					if (target_Save[i] == target) {
						break;
					}
				}
				if (i == n) target_Save.push_back(target);
			}
		}
		break;
	}
	case eSkillEffect::BadStatus:
		//////////////////////////////////////
		//////////// �y��Ԉُ�z//////////////
		//////////////////////////////////////
	{
		// �Ώۂ�����ł��画�肵�Ȃ�
		if (target->GetAlive() != eAlive::alive) return;

		eBadStatus bs_Type = effect->GetBadStatus()->type;	// ��Ԉُ�̎�ގ擾
		// ��Ԉُ킪�y�ǂꂩ��������_���t�^�z�ł���Ώ��ňȊO�̂ǂꂩ�ɕύX
		if (bs_Type == eBadStatus::One_any) {
			bs_Type = (eBadStatus)GetRand(7);
		}

		// �ϐ���-1 �� ��Αϐ�
		if (target->GetDEF_BadStatus(bs_Type) == -1) {
			// ��𐬌� & �~�ς��Ȃ�
		}
		// ������-1 �� �K��
		// 0�`�ϐ��l �� 0�`������ �̑΍R
		// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���
		else if (effect->GetBadStatus()->hit != -1 &&
			(GetRand(effect->GetBadStatus()->hit * 1000)) <
			(GetRand(target->GetDEF_BadStatus(bs_Type) * 1000))) {
			// ��𐬌�
			// �~�ϒl�㏸�@Rand(0 ~ �З�*0.2)
			target->GiveEddDEF_BadStatus(bs_Type, GetRand((int)(effect->GetBadStatus()->power * 0.2)));
		
			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l�v�Z
				switch (bs_Type) {
				case eBadStatus::Poison:// �y�Łz�����~�P
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Paralysis:// �y��Ⴡz�����~�P
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Sleep:// �y�����z�����~�P�D�T
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Confusion:// �y�����z�����~�P�D�T
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Blind:// �y�ÈŁz�����~�P
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit;
					break;
				case eBadStatus::Weakness:// �y����z�����~�R
					playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					break;
				case eBadStatus::Stun:// �y�C��z�����~�P�D�T
					playerHate[attacker->GetPlayerOrder()] += (int)(effect->GetBadStatus()->hit * 1.5);
					break;
				case eBadStatus::Death:// �y�����z�w�C�g2�{
					playerHate[attacker->GetPlayerOrder()] *= 2;
					break;
				case eBadStatus::Extinction:// �y���Łz�w�C�g2�{
					playerHate[attacker->GetPlayerOrder()] *= 2;
					break;
				}
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		
		}
		else {
			// ������s
			// ��Ԉُ��^����
			target->GiveBadStatus(bs_Type, (int)(effect->GetBadStatus()->power* (1.0 + (double)GetRand(-10, 11)*0.01)));
			
			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l�v�Z
				switch (bs_Type){
				case eBadStatus::Poison:// �y�Łz�З́~�R	�K���Ȃ�З́~�P�O
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->power * 10;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->power * 3;
					}
					break;
				case eBadStatus::Paralysis:// �y��Ⴡz�����~�R	�K���Ȃ�w�C�g2�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 2;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					}
					break;
				case eBadStatus::Sleep:// �y�����z�����~�T	�K���Ȃ�w�C�g3�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 3;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 5;
					}
					break;
				case eBadStatus::Confusion:// �y�����z�����~4	�K���Ȃ�w�C�g2�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 2;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 4;
					}
					break;
				case eBadStatus::Blind:// �y�ÈŁz�����~�R	�K���Ȃ�w�C�g1.5�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] = (int)(playerHate[attacker->GetPlayerOrder()] * 1.5);
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 3;
					}
					break;
				case eBadStatus::Weakness:// �y����z�����~�P�O	�K���Ȃ�w�C�g5�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 5;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 10;
					}
					break;
				case eBadStatus::Stun:// �y�C��z�����~4	�K���Ȃ�w�C�g3�{
					if (effect->GetBadStatus()->hit == -1) {
						playerHate[attacker->GetPlayerOrder()] *= 3;
					}
					else {
						playerHate[attacker->GetPlayerOrder()] += effect->GetBadStatus()->hit * 4;
					}
					break;
				case eBadStatus::Death:// �y�����z�����Ńw�C�g�lMax
					playerHate[attacker->GetPlayerOrder()] = maxHate;
					break;
				case eBadStatus::Extinction:// �y���Łz�����Ńw�C�g�lMax
					playerHate[attacker->GetPlayerOrder()] = maxHate;
					break;
				}
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
			// �y�X�L���Ώێ҂̏����z****//
			// �y�v���C���[�����z
			if (target->GetCharType()) {
				// �ΏۃZ�[�u
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// �y�G�����z
			else {
				// �ΏۃZ�[�u
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}

		
		break;
	}
	case eSkillEffect::Buff:
		//////////////////////////////////////
		/////////// �y�o�t�E�f�o�t�z///////////
		//////////////////////////////////////
	{
		// �Ώۂ�����ł��画�肵�Ȃ�
		if (target->GetAlive() != eAlive::alive) return;

		// �����ւ̃o�t�͖�������Ȃ�
		// 0�`���(AGI��DEX�̕���/10) �� 0�`����(DEX�~�Z�␳)�@�̑΍R
		// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���
		if (attacker->GetCharType() == target->GetCharType() ||
			(GetRand((attacker->GetStatus(eStatus::DEX)) * 1000) * effect->GetBuff()->hit * (attacker->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) >
			(GetRand((int)(((target->GetStatus(eStatus::AGI) * 1000) + (target->GetStatus(eStatus::DEX)) * 1000) * 0.05)) * (target->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1))) {
			
			eBuff buffType = effect->GetBuff()->type;	// �o�t�̎�ގ擾
			// �o�t���y�ǂꂩ��������_���t�^�z�ł���΂ǂꂩ�ɕύX
			if (buffType == eBuff::One_any) {
				buffType = (eBuff)GetRand(21);
			}
			
			// �y�����z
			float rate = 0;	// �{��
			// �{���v�Z
			if (effect->GetBuff()->dir) {
				// �o�t	1+sin(��/2000*(�g�pMP * �ϊ�����(����+)))*4
				rate = 1.0f + (float)sin(M_PI * 0.0005 * (GetConvValue(effect->energyConv, totalUseEnergy) * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(effect->energyConv.type, effect->energyConv.direction)))) * 4;
				// ����1.01�{
				if (rate < 1.01f) {
					rate = 1.01f;
				}
			}
			else {
				// �f�o�t	1 / (1+sin(��/2000*(�ϊ��� * �ϊ�����(����-)))*4)
				rate = 1.0f / (1.0f + (float)sin(M_PI * 0.0005 * (GetConvValue(effect->energyConv, totalUseEnergy) * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(effect->energyConv.type, effect->energyConv.direction)))) * 4);
				// ����1.1�{
				if (rate > 0.99f) {
					rate = 0.99f;
				}
			}
			// �o�t�E�f�o�t��^����
			target->GiveBuff(buffType, rate, effect->GetBuff()->trun);

			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l�v�Z	�g�p���f�i�G�l���M�[�j�~�R
				playerHate[attacker->GetPlayerOrder()] += (int)(GetConvValue(effect->energyConv, totalUseEnergy) * 3);

				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// �y�X�L���Ώێ҂̏����z****//
			// �y�v���C���[�����z
			if (target->GetCharType()) {
				// �ΏۃZ�[�u
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// �y�G�����z
			else {
				// �ΏۃZ�[�u
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}
		else {
			//****�y�U���҂̏����z****//
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l�v�Z	�g�p���f�i�G�l���M�[�j�~�P
				playerHate[attacker->GetPlayerOrder()] += (int)(GetConvValue(effect->energyConv, totalUseEnergy));

				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		}

		
		break;
	}
	case eSkillEffect::AnotherSkill:
		//////////////////////////////////////
		////////// �y���̃X�L�������z//////////
		//////////////////////////////////////
	{		
		/*�����ł͂Ȃɂ����Ȃ�*/
		break;
	}
	case eSkillEffect::Resurrection:
		//////////////////////////////////////
		/////////////// �y�h���z///////////////
		//////////////////////////////////////
	{
		// �Ώۂ�����ł��Ȃ���Δ��肵�Ȃ�
		if (target->GetAlive() != eAlive::dead) return;

		// �h����������
		if (GetRand(99) < effect->GetResurrection()->success) {
			// �y�����z
			// �Ώۑh���i�␳2�j
			int healValue = target->Resurrection(GetConvValue(effect->energyConv, totalUseEnergy) * effect->GetResurrection()->rate * (effect->energyConv.convType == eConvType::None ? 1.0 : attacker->GetEfficiency(eEnergy::Life, true)) * 2);

			// ****�y�s���҂̏����z**** //
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l���Z�i�񕜗ʁ~5 -> �w�C�g2�{�j
				playerHate[attacker->GetPlayerOrder()] += healValue * 5;
				playerHate[attacker->GetPlayerOrder()] *= 2;
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}

			// �y�X�L���Ώێ҂̏����z**** //
			// �y�v���C���[�����z
			if (target->GetCharType()) {
				damageEffect.emplace_back(64 + GetRand(0, 200), 26 + target->GetPlayerOrder() * 200 + GetRand(0, 150), healValue, 20, YELLOW);
				playerHate[target->GetPlayerOrder()] = 500;	// �h���Ώۂ̃w�C�g��500�ɂ���
				playerField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
			// �y�G�����z
			else {
				damageEffect.emplace_back(enemyFieldX + FieldX * cellSize + GetRand(0, cellSize / 2), enemyFieldY + FieldY * cellSize + GetRand(0, cellSize / 2), YELLOW, healValue, 20);
				enemyField_Save[FieldY][FieldX] = true;
				{
					int i = 0, n = target_Save.size();
					// �Z�[�u�ς��𔻒�
					for (; i < n; ++i) {
						if (target_Save[i] == target) {
							break;
						}
					}
					if (i == n) target_Save.push_back(target);
				}
			}
		}
		else {
			// �y���s�z
			// ****�y�s���҂̏����z**** //
			// �y�v���C���[�z
			if (attacker->GetCharType()) {
				// �w�C�g�l���Z�i�w�C�g1.5�{�j
				playerHate[attacker->GetPlayerOrder()] = (int)(playerHate[attacker->GetPlayerOrder()] * 1.5);
				// �w�C�g�l���
				if (playerHate[attacker->GetPlayerOrder()] > maxHate) {
					playerHate[attacker->GetPlayerOrder()] = maxHate;
				}
			}
		}

		break;
	}
	}

}

// �G�l���M�[���Ƃ̃_���[�W�v�Z
int Battle::EnergyConvDmage(Character* attacker, Character* target, tEnergyConv energy, float hit, float damageRate, int totalUseEnergy) {

	// �y�_���[�W�v�Z���z���ڕϊ��ȊO�̏ꍇ
	// �ϊ��_���[�W = �ϊ��ʁ~�ϊ������~�X�L���␳�{���~�`���[�W�␳�{���~�N���e�B�J���␳�{���~�G�l���M�[�␳�{���~�_���[�W�Ԃ�{�␳�_���[�W
	// �␳�_���[�W = �␳�l�~�X�L���␳�{���~�`���[�W�␳�{���~�N���e�B�J���␳�{���~�G�l���M�[�␳�{���~�_���[�W�Ԃ�
	// �_���[�W��10%�͊m��_���[�W
	// 90%�͖h��͂��������_���[�W
	

	//**********************************//
	//***********�y��𔻒�z***********//
	//**********************************//
	// �y����\����z
	if (!target->GetBadStatus(eBadStatus::Sleep).get &&	// ����
		!target->GetBadStatus(eBadStatus::Stun).get &&	// �X�^��
		!target->GetBadStatus(eBadStatus::Paralysis).get) {	// ���
		// �s���j�Q�n�̏�Ԉُ�������Ă��Ȃ����

		// �y��𔻒�z
		// hit == -1�F�K��
		// �`���[�W�␳�i�����␳�j== -1 �K��
		// 0�`���(AGI��DEX�̕���/20) �� 0�`����(DEX�~�Z�␳)�@�̑΍R
		// ���͋ύt�Ȃ�m��5%�ŉ������
		// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���
		if( hit != -1 && attacker->GetCharge(eCharge::Hit) != -1 &&
			(GetRand((attacker->GetStatus(eStatus::DEX)) * 1000) * hit * (attacker->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) * attacker->GetCharge(eCharge::Hit) <
			(GetRand((int)(((target->GetStatus(eStatus::AGI) * 1000) + (target->GetStatus(eStatus::DEX)) * 1000) * 0.025)) * (target->GetBadStatus(eBadStatus::Blind).get ? 0.1 : 1)) * target->GetCharge(eCharge::Avoid)){
			// ��𐬌�
			return -1;
		}
	}

	//**********************************//
	//*********�y�_���[�W�v�Z�z*********//
	//**********************************//

	int totalDamage = 0;	// ���v�_���[�W��

	double energyDamage = 0;	// �G�l���M�[�_���[�W��

	// �␳�_���[�W
	double damageCorrection = 0;	// �␳�_���[�W��
	switch (energy.correction){
	case 2:// �ySTR�␳�z
		// STR �~ 95%�`115% + -2�`2 �_���[�W
		damageCorrection = attacker->GetStatus(eStatus::STR) * (1.0 + (double)GetRand(-500, 1500)*0.0001) + GetRand(-2, 2);
		break;
	}

	// �y�G�l���M�[�ϊ������z
	energy.rate = GetConvValue(energy, totalUseEnergy);	// �ϊ�������ϊ��ʂ�
	// �G�l���M�[�ϊ��Ȃ��łȂ����
	if (energy.type != eEnergy::None) {
		if (energy.convType == eConvType::Target) {
			// ����̖��f�̒��ڕϊ��̏ꍇ

			// �ϊ��ʂɕϊ�������������
			energy.rate *= attacker->GetEfficiency(energy.type, energy.direction);

			// +�ϊ��ő����MP���ϊ��ʁi�ϊ������v�Z�ς݁j�ɑ���Ȃ�������
			if (energy.direction && energy.rate > target->GetStatus(eStatus::MP)) {
				// �ϊ��ʂ��c��MP���ɂ���
				energy.rate = target->GetStatus(eStatus::MP);
			}

			// MUE�΍R
			// ������MUE�������MUE�������Ă��Ȃ���Ζ���
			if (attacker->GetStatus(eStatus::MUE) <= target->GetStatus(eStatus::MUE)) {
				energy.rate = 0;	// �ϊ��_���[�W��0��
			}
			// ������MUE�������MUE��1000�ȏ�����Ă���Δ{��100
			else if (attacker->GetStatus(eStatus::MUE) > target->GetStatus(eStatus::MUE) + 1000) {
				energy.rate *= 100;
			}
			else {
				// MUE�̍���
				// 100 - cos((M_PI_2 / 1000)�~MUE�̍�)�~100
				energy.rate *= (100 - (cos((M_PI / 2000) * (attacker->GetStatus(eStatus::MUE) - target->GetStatus(eStatus::MUE))) * 100));
			}

		}
		else {
			// ����ȊO
			// �����̖��f�̕ϊ� or ��̖��f�̕ϊ��̏ꍇ

			// �ϊ��ʂɕϊ�������������
			energy.rate *= attacker->GetEfficiency(energy.type, energy.direction);
		}
		energyDamage = energy.rate;	// �G�l���M�[�ϊ��ʂ��G�l���M�[�_���[�W�ɉ��Z
	}

	// �y�X�L���_���[�W�{���z
	energyDamage *= damageRate;
	damageCorrection *= damageRate;

	// �y�`���[�W�␳�z�i�_���[�W�␳�j
	energyDamage *= attacker->GetCharge(eCharge::Damage);
	damageCorrection *= attacker->GetCharge(eCharge::Damage);


	// �y�N���e�B�J������z
	// 0�`��Q�҂�DEX �� 0�`�U���҂�DEX/20�@�̑΍R
	// ���͋ύt�Ȃ�m��5%�ŃN���e�B�J��
	// �X�e�[�^�X���Ⴗ�����0�����o�Ȃ������肷�邩��~1000���Ĕ���
	if (GetRand((attacker->GetStatus(eStatus::DEX) * 1000) / 20) >= GetRand(target->GetStatus(eStatus::DEX) * 1000)) {
		// �_���[�W2�{
		energyDamage *= 2;
		damageCorrection *= 2;
	}

	// �y�_���[�W�Ԃ�z�~90%�`110%
	energyDamage *= (1.0 + (double)GetRand(-1000, 1000)*0.0001);

	// �y�G�l���M�[�␳�z
	switch (energy.type){
	case eEnergy::Heat:// �y�M�z
		if (energy.direction)
		{ //+
			// �_���[�W��10%�͊m��_���[�W
			// 90%�͖h��͂��������_���[�W
			energyDamage *= 5;	// �␳5
			// �M�_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-
			energyDamage *= 5;	// �␳5
			// ��C�_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->ColdDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		break;
	case eEnergy::Elec:// �y�d�C�z
		if (energy.direction)
		{ //+
			energyDamage *= 10;	// �␳10
			// �d�C�_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->ElecDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	case eEnergy::Shine:// �y���z
		if (energy.direction)
		{ //+
			//energyDamage *= 1;	// �␳1
			// �M�_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	case eEnergy::Sound:// �y���z
		if (energy.direction)
		{ //+
			energyDamage *= 0.5;	// �␳0.5
			// �����_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	case eEnergy::Motion:// �y�^���z
		if (energy.direction)
		{ //+
			energyDamage *= 3;	// �␳3
			// �����_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	case eEnergy::Gravity:// �y�d�́z
		if (energy.direction)
		{ //+
			energyDamage *= 20;	// �␳20
			// �����_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	case eEnergy::Life:// �y�����z
		if (energy.direction)
		{ //+

		}
		else
		{ //-

		}
		break;
	case eEnergy::Static:// �y�Î~�z
		if (energy.direction)
		{ //+

		}
		else
		{ //-

		}
		break;
	case eEnergy::Nucleus:// �y�j�z
		if (energy.direction)
		{ //+
			energyDamage *= 100;	// �␳100
			// �M�_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->HeatDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
			// �����_���[�W
			totalDamage += target->Damage(energyDamage * 0.1);		// �m��_���[�W
			totalDamage += target->PhysDamage(energyDamage * 0.9);	// �h��͂��������_���[�W
		}
		else
		{ //-

		}
		break;
	}

	// �␳�_���[�W
	totalDamage += target->Damage(damageCorrection * 0.1);		// �m��_���[�W
	totalDamage += target->PhysDamage(damageCorrection * 0.9);	// �h��͂��������_���[�W


	return (int)totalDamage;	// ���v�_���[�W��Ԃ�
}
// �t�B�[���hMP�̃Q�b�^�[
int Battle::GetFieldMP() {
	return fieldMP;
}
// �t�B�[���hMP����
void Battle::IncreaseFieldMP(int value) {
	if (value <= 0)	return;		// 0�ȉ��̓G���[

	fieldMP += value;	// MP�̑���
}
// �t�B�[���hMP����
void Battle::DecreaseFieldMP(int value) {
	if (value <= 0)	return;		// 0�ȉ��̓G���[

	fieldMP -= value;	// MP�̌���
	// 0�����ɂȂ�����0�ɖ߂�
	if (fieldMP < 0) {
		fieldMP = 0;
	}
}

// ���񂾓G�̃f�[�^�̍폜
void Battle::DeleteDeadEnemy() {
	// �X�L���Ώەۑ�������
	for (int i = 0; i < playerFieldHeight; i++) {
		for (int j = 0; j < playerFieldWidth; j++) {
			playerField_Save[i][j] = false;
		}
	}
	for (int i = 0; i < enemyFieldHeight; i++) {
		for (int j = 0; j < enemyFieldWidth; j++) {
			enemyField_Save[i][j] = false;
		}
	}
}

// �G�S�Ŕ���
bool Battle::CheckEnemysDead() {
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		if (enemy[i]->GetAlive() == eAlive::alive){
			// �N����l�ł������Ă����false
			return false;
		}
	}

	return true;	// �S��
}

// �v���C���[�S�Ŕ���
bool Battle::CheckPlayersDead() {
	for (int i = 0, n = player.size(); i != n; ++i) {
		if (player[i]->GetAlive() == eAlive::alive) {
			// �N����l�ł������Ă����false
			return false;
		}
	}

	return true;	// �S��
}

void Battle::Draw() {
	DrawGraph(0, 0, gr_Back);

	// �y�e�L�X�g�{�b�N�X�\���z
	TextBox::Draw();

	// debug
	
	// �G
	for (int i = 0, n = enemy.size(); i != n; ++i) {
		enemy[i]->Draw(enemyFieldX, enemyFieldY, cellSize);

	}

	/*for (int i = 0, n = enemy.size(); i < n; i++) {
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 0, "%s", enemy[i]->GetName().c_str());
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 1, "HP :%d", enemy[i]->GetStatus(eStatus::HP));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 2, "MP :%d", enemy[i]->GetStatus(eStatus::MP));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 3, "STR:%d", enemy[i]->GetStatus(eStatus::STR));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 4, "DEX:%d", enemy[i]->GetStatus(eStatus::DEX));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 5, "AGI:%d", enemy[i]->GetStatus(eStatus::AGI));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 6, "LUC:%d", enemy[i]->GetStatus(eStatus::LUC));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 7, "D_P:%d", enemy[i]->GetStatus(eStatus::DEF_PHYS));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 8, "D_H:%d", enemy[i]->GetStatus(eStatus::DEF_HEAT));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 9, "D_C:%d", enemy[i]->GetStatus(eStatus::DEF_COLD));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 10, "D_E:%d", enemy[i]->GetStatus(eStatus::DEF_ELEC));
		DrawString(BLACK, 0 + i * 150, 0 + 32 * 11, "MUE:%d", enemy[i]->GetStatus(eStatus::MUE));
	}
	for (int i = 0, n = player.size(); i < n; i++) {		
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 0, "%s", player[i]->GetName().c_str());
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 1, "HP :%d", player[i]->GetStatus(eStatus::HP));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 2, "MP :%d", player[i]->GetStatus(eStatus::MP));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 3, "STR:%d", player[i]->GetStatus(eStatus::STR));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 4, "DEX:%d", player[i]->GetStatus(eStatus::DEX));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 5, "AGI:%d", player[i]->GetStatus(eStatus::AGI));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 6, "LUC:%d", player[i]->GetStatus(eStatus::LUC));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 7, "D_P:%d", player[i]->GetStatus(eStatus::DEF_PHYS));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 8, "D_H:%d", player[i]->GetStatus(eStatus::DEF_HEAT));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 9, "D_C:%d", player[i]->GetStatus(eStatus::DEF_COLD));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 10, "D_E:%d", player[i]->GetStatus(eStatus::DEF_ELEC));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 11, "MUE:%d", player[i]->GetStatus(eStatus::MUE));
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 12, "��ِ�:%d", player[i]->GetSkillList().size());
		DrawString(BLACK, 1500 + i * 150, 0 + 32 * 13, "�w�C�g:%d", playerHate[i]);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 13, "�񕜗�:%d", player[i]->GetNomalAttack()->effect.back()->GetHeal()->value);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 14, ":%f", player[i]->GetNomalAttack()->effect.back()->GetHeal()->rate);
		//DrawString(BLACK, 1500 + i * 150, 0 + 32 * 15, ":%d", player[i]->GetNomalAttack()->effect.back()->GetHeal()->type);
		
	}*/

	/*// �t�B�[���h��MP
	DrawString(BLUE, 800, 0, "��MP:%d/%d", fieldMP, fieldMaxMP);
	// �^�[����
	DrawString(BLUE, 800, 32, "�^�[����:%d", turnNum);

	for (int i = 0, n = skillOrder->order.size(); i < n; ++i) {
		DrawString(BLUE, 800, 64 + i*32, "���x:%f", skillOrder->order[i].speed);
	}*/
	
	// �v���C���[�̃X�e�[�^�X
	for (int i = 0, n = player.size(); i < n; i++) {
		player[i]->Draw();
	}


	// �_���[�W�ʕ\��
	for (int i = 0, n = graphicEffect.size(); i < n; i++) {
		graphicEffect[i]->Draw(cellSize);
	}
	for (int i = 0, n = damageEffect.size(); i < n; i++) {
		damageEffect[i].Draw();
	}
	

	switch (step)
	{
	case eBattleStep::START:

		break;
	case eBattleStep::COMMAND:
		Draw_COMMAND();
		break;
	case eBattleStep::BATTLE:

		break;
	case eBattleStep::END:

		break;
	}
}

void Battle::Draw_COMMAND() {

	if (TextBox::textBoxFlag) return;

	const int text_Top = WINDOW_HEIGHT - 300;	// �e�L�X�g�{�b�N�X�̏�[

	switch (commandStep){
	case eCommand::CharaSelect:// �y�L�����I���z
		break;
	case eCommand::SkillSelect:// �y�g�p�X�L���I���z
	{
		// �X�N���[���o�[
		if (skillNum > 5) {
			DrawStretchGraph(752, (int)(text_Top + 15 + selectSkillArea * (270.0 / skillNum)), gr_scroll, 14, (int)(270 * (5.0 / skillNum)));
		}
		else {
			DrawStretchGraph(752, text_Top + 15, gr_scroll, 14, 270);
		}

		// �X�L���I���E�B���h�E
		DrawGraph(256, text_Top + 10, gr_SkillSelect);

		// �X�L���I��
		// �X�L���̐����U�ȏォ�ŏ����𕪂���
		if (skillNum > 5) {
			for (int i = 0; i < 5; ++i) {
				if (selectSkillArea + i >= 0 && selectSkillArea + i < skillNum) {
					DrawString(WHITE, 320, text_Top + 40 + i * 48, selectSkillList[selectSkillArea + i]->name.c_str());
				}
			}
			// �J�[�\��
			DrawString(WHITE, 288, text_Top + 40 + (48 * selectSkillCursor), "��");
		}
		else {
			for (int i = 0; i < skillNum; ++i) {
				DrawString(WHITE, 320, text_Top + 40 + i * 48, selectSkillList[i]->name.c_str());
			}
			// �J�[�\��
			DrawString(WHITE, 288, text_Top + 40 + (48 * selectSkill), "��");
		}

		// �y�X�L���ڍׁz
		DrawString(WHITE, 960, text_Top + 20, "MP");
		if (commandPreSelect[selectChara].fullPowerFlag) {	// �S�̓t���O
			// �y�S�́z
			DrawString(YELLOW, 784, text_Top + 20, "�y �S �� �z");
			// �g�pMP
			if (selectSkillList[selectSkill]->totalUseEnergy == 0) {	// 0�Ŋ���Ȃ��悤��
				DrawString(RED, 960 + 96, text_Top + 20, "0");
				DrawString(BLUE, 960 + 192, text_Top + 20, "0");
				DrawString(YELLOW, 960 + 288, text_Top + 20, "0");
				//DrawString(WHITE, 960 + 384, text_Top + 20, "�K�vINT�l�F0");
			}
			else {
				DrawString(RED, 960 + 96, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[0] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				DrawString(BLUE, 960 + 192, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[1] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", (int)(selectSkillList[selectSkill]->useMP[2] * ((double)player[selectChara]->GetStatus(eStatus::MUE) / selectSkillList[selectSkill]->totalUseEnergy)));
				//DrawString(WHITE, 960 + 384, text_Top + 20, "�K�vINT�l�F%d", player[selectChara]->GetStatus(eStatus::MUE));
			}
			if (selectChara == 0) {

			}
		}
		else {
			// �y�ʏ�z
			DrawString(WHITE, 784, text_Top + 20, "�y �� �� �z");
			// �g�pMP
			DrawString(RED, 960 + 96, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[0]);
			DrawString(BLUE, 960 + 192, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[1]);
			DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", selectSkillList[selectSkill]->useMP[2]);
			//DrawString(WHITE, 960 + 384, text_Top + 20, "�K�vINT�l�F%d", selectSkillList[selectSkill]->totalUseEnergy);
		}
		// �X�L��������	
		for (int i = 0, n = selectSkillList[selectSkill]->explanatoryText.size(); i < n; ++i) {
			DrawString(WHITE, 784, text_Top + 72 + i * 32, selectSkillList[selectSkill]->explanatoryText[i].c_str());
		}
		
		//break;
	}
	case eCommand::ActionSelect:// �y�s���I���z
	{
		// �R�}���h�I�𒆂̃L������
		DrawString(WHITE, 16, text_Top, player[selectChara]->GetName().c_str());
		// �I����
		DrawString(WHITE, 64, text_Top + 32, "�U��");
		DrawString(WHITE, 64, text_Top + 64, "�X�L��");
		DrawString(WHITE, 64, text_Top + 96, "�A�C�e��");
		DrawString(WHITE, 64, text_Top + 128, "�h��");
		DrawString(WHITE, 64, text_Top + 160, "������");
		// �J�[�\��
		DrawString(WHITE, 32, text_Top + 32 + (32 * selectAction), "��");
		break;
	}
	case eCommand::ItemSelect:	// �y�g�p�A�C�e���I���z
	{
		vector<string> itemname;	// �G�C���A�X

		for (int i = 0, n = Player::bag[selectChara].size();i < n;++i) {
			// ���O���X�g�쐬
			itemname.push_back(Player::bag[selectChara][i]->GetName());
		}

		// �X�N���[���o�[
		if (itemname.size() > 5) {
			DrawStretchGraph(752, (int)(text_Top + 15 + selectSkillArea * (270.0 / itemname.size())), gr_scroll, 14, (int)(270 * (5.0 / itemname.size())));
		}
		else {
			DrawStretchGraph(752, text_Top + 15, gr_scroll, 14, 270);
		}

		// �X�L���I���E�B���h�E
		DrawGraph(256, text_Top + 10, gr_SkillSelect);

		for (int i = selectItem.pos.Y / 5 * 5, n = itemname.size(), j = 0; (i < n && j < 5); ++i, ++j) {
			// �������Ă���A�C�e����\��
			DrawString(WHITE, 256 + 64, text_Top + 40 + j * 48, itemname[i].c_str());	// ���O��\��
		}

		// �K�C�h
		if (itemname.size() / 5 > 0 && selectItem.pos.Y / 5 * 5 != itemname.size() / 5) {
			// ���̃y�[�W������Ƃ�
			DrawString(WHITE, 352, text_Top + 16 + 5 * 48, "�F");
		}
		if (itemname.size() > 5) {
			// �O�̃y�[�W������Ƃ�
			DrawString(WHITE, 352, text_Top, "�F");
		}

		// �X�L��������
		if (Player::bag[selectChara][selectItem.pos.Y]->GetSkill()) {
			for (int i = 0, n = Player::bag[selectChara][selectItem.pos.Y]->GetSkill()->explanatoryText.size(); i < n; ++i) {
				DrawString(WHITE, 784, text_Top + 72 + i * 48, Player::bag[selectChara][selectItem.pos.Y]->GetSkill()->explanatoryText[i].c_str());
			}
		}
		DrawString(WHITE, 256 - 32 + 64, text_Top + 40 + selectItem.pos.Y % 5 * 48, "��");	// �A�C�R��
		break;
	}
	case eCommand::TargetSelect:// �y�X�L�����̑Ώ۔͈͑I���z
	{
		// �}�X�`��
		for (int i = 0, n = enemyField.size(); i < n; i++) {
			for (int j = 0, m = enemyField[i].size(); j < m; j++) {
				DrawBox(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, cellSize, cellSize, false);
			}
		}

		// �y�G����I���z
		if (selectSkillData->selectTarget) {
			// �G�̓����蔻��\��
			for (int i = 0, n = enemyField.size(); i < n; i++) {
				for (int j = 0, m = enemyField[i].size(); j < m; j++) {
					//DrawBox(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, cellSize, cellSize, false);
					if (enemyField[i][j] != nullptr && enemyField[i][j]->GetAlive() == eAlive::alive) {
						DrawStretchGraph(enemyFieldX + j * cellSize, enemyFieldY + i * cellSize, gr_target, cellSize, cellSize);
					}
				}
			}
			// �X�L���͈͕\��
			for (int i = 0, n = selectSkillData->collision.size(); i < n; ++i) {
				for (int j = 0, m = selectSkillData->collision[i].size(); j < m; ++j) {

					// �A�E�g�I�u�����W�΍�

					//	j + selectSkillData->x >= 0 && j + selectSkillData->x < enemyFieldWidth) {

						switch (selectSkillData->collision[i][j]) {
						case 0:	// �y����Ȃ��z
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit1, cellSize, cellSize);
							break;
						case 1:// �y����P�z
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit2, cellSize, cellSize);
							break;
						case 2:// �y����Q�z
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit3, cellSize, cellSize);
							break;
						default:// �y����R�ȏ�z
							DrawStretchGraph(enemyFieldX + (j + selectSkillData->x) * cellSize, enemyFieldY + (i + selectSkillData->y) * cellSize, gr_SkillHit4, cellSize, cellSize);
							break;
						}
					//}
				}
			}
		}
		// �y��������I���z
		else {
			// �R�}���h�I�𒆂̃L������
			DrawString(WHITE, 16, text_Top, player[selectChara]->GetName().c_str());
			// �I����
			DrawString(WHITE, 64, text_Top + 32, "�U��");
			DrawString(WHITE, 64, text_Top + 64, "�X�L��");
			DrawString(WHITE, 64, text_Top + 96, "�A�C�e��");
			DrawString(WHITE, 64, text_Top + 128, "�h��");
			DrawString(WHITE, 64, text_Top + 160, "������");
			// �J�[�\��
			DrawString(WHITE, 32, text_Top + 32 + (32 * selectAction), "��");

			// �I�𒆂̃X�L��
			DrawString(WHITE, 256, text_Top + 32, selectSkillData->name.c_str());

			// �ΏۃL�����I��
			for (int i = 0, n = player.size(); i < n; ++i) {
				DrawString(WHITE, 544, text_Top + 32 + i * 32, player[i]->GetName().c_str());
			}
			// �J�[�\��
			DrawString(WHITE, 512, text_Top + 32 + (32 * selectTargetAlly), "��");

		}

		break;
	}
	}

	// �t�B�[���h�ł̑������
	//DrawGraph(WINDOW_WIDTH - 679, WINDOW_HEIGHT - 400, gr_Operation);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*------------------------------------------------�_���[�W���̕\��-------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::array<int, 10> DamageEffect::gr_RedNum;	// �����摜�i�ԁj

// �_���[�W�G�t�F�N�g�i�_���[�W�ʂ̕\���Ƃ��j
DamageEffect::DamageEffect(int x, int y, int value, int frame, int color, int delay) {
	this->x = x;
	this->y = y;
	this->value = value;
	this->count = 0;
	this->frame = frame;
	this->delay = delay;
	this->color = color;
}
DamageEffect::~DamageEffect() {

}
void DamageEffect::Init() {
	// �����摜�i�ԁj�ǂݍ���
	gr_RedNum[0] = LoadGraph("img\\battle\\num\\red_0.png");
	gr_RedNum[1] = LoadGraph("img\\battle\\num\\red_1.png");
	gr_RedNum[2] = LoadGraph("img\\battle\\num\\red_2.png");
	gr_RedNum[3] = LoadGraph("img\\battle\\num\\red_3.png");
	gr_RedNum[4] = LoadGraph("img\\battle\\num\\red_4.png");
	gr_RedNum[5] = LoadGraph("img\\battle\\num\\red_5.png");
	gr_RedNum[6] = LoadGraph("img\\battle\\num\\red_6.png");
	gr_RedNum[7] = LoadGraph("img\\battle\\num\\red_7.png");
	gr_RedNum[8] = LoadGraph("img\\battle\\num\\red_8.png");
	gr_RedNum[9] = LoadGraph("img\\battle\\num\\red_9.png");
}
void DamageEffect::Fina(){
	// �����摜�i�ԁj
	for (int i = 0, n = gr_RedNum.size(); i < n; ++i) {
		DeleteGraph(gr_RedNum[i]);
	}
}
bool DamageEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > 50;
}
void DamageEffect::Draw() {
	if (delay-- > 0) return;
	if (value == -1) {
		DrawString(color, x, y, "MISS");
	}
	else {
		//DrawString(color, x, y, "%d", value);
		int tmp = value;
		int i = 0;
		while (tmp > 0) {
			DrawGraph(x - i * 25, y - count, gr_RedNum[tmp % 10]);

			tmp /= 10;
			++i;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------�摜��\������G�t�F�N�g--------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicEffect::GraphicEffect(short x, short y, int gr, short frame, short delay) :
	x(x),
	y(y),
	gr(gr),
	frame(frame),
	count(0),
	delay(0)
{
}
GraphicEffect::~GraphicEffect() {
}
bool GraphicEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > frame;
}
void GraphicEffect::Draw(int cellsize) {
	if (delay-- > 0) return;

	DrawStretchGraph(x, y, gr, cellsize, cellsize);
}

DivGraphicEffect::DivGraphicEffect(short x, short y, int gr, unsigned __int8 divX, unsigned __int8 divY, short divNum, unsigned __int8 width, unsigned __int8 height, short delay) :
	GraphicEffect(x, y, gr, divNum * 4 - 1, delay),
	divX(divX),		// ������X
	divY(divY),		// ������Y
	divNum(divNum),	// ��������
	width(width),
	height(height)
{
}
DivGraphicEffect::~DivGraphicEffect() {
}
bool DivGraphicEffect::Process() {
	if (delay-- > 0) return false;

	count++;

	return count > frame;
}
void DivGraphicEffect::Draw(int cellsize) {
	if (delay-- > 0) return;

	//const int divX = (count / 4) % divNum;
	//const int divY = 0;

	const unsigned __int8 dx = (count / 4) % divX;
	const unsigned __int8 dy = (count / 4) / divX;

	//DrawStretchGraph(x, y, gr, cellsize, cellsize);
	DrawStretchGraph(x, y, gr, dx, dy, cellsize * width, cellsize * height);
}
