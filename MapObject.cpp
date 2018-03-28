#include "MapObject.h"
#include "Game.h"
#include "Scene.h"

/*-------------------------------------------------���j�b�g�̋����f�[�^--------------------------------------------*/
void MapUnit_Action::SetActionNum(int* actionNum) {		// ���s���̍s���̗v�f�ԍ��̃Z�b�^�[
	this->actionNum = actionNum;
}
// �y�]�ځz
MapUnit_Action_Warp::MapUnit_Action_Warp(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�}�b�vID�z�擾
	getline(L_stream, L_token, ',');
	this->mapId = boost::lexical_cast<int>(L_token);
	// 3�yX���W�z�擾�i�}�b�v���W�j
	getline(L_stream, L_token, ',');
	this->x = boost::lexical_cast<int>(L_token);
	// 4�yY���W�z�擾�i�}�b�v���W�j
	getline(L_stream, L_token, ',');
	this->y = boost::lexical_cast<int>(L_token);
	// 5�y�]�ډ��o�z�擾
	getline(L_stream, L_token, ',');
	this->effect = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Warp::~MapUnit_Action_Warp() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Warp::Execution() {
	// �v���C���[�̈ړ����I�������
	if (!Player::animation)
	{
		// �v���C���[�̍��W���C�����āA�Ⴄ�}�b�v�֔�΂�
		Player::pos_.set_pos(x * CHIPSIZE, y * CHIPSIZE);
		Player::stage = mapId;
	}
	// �G�t�F�N�g����
	// ����΁��ɏ���

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Stop;	// ���f
}

// �y�Z���t�z
MapUnit_Action_Talk::MapUnit_Action_Talk(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�Z���t���e�z�擾
	//getline(L_stream, L_token, ',');
	//boost::algorithm::split(str, L_token, boost::is_any_of("@"));
	while (getline(L_stream, L_token, ',') ){
		if (L_token == "") break;
		this->str.push_back(L_token);
	}

}
MapUnit_Action_Talk::~MapUnit_Action_Talk() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Talk::Execution() {
	TextBox::AddLine(str);	// �\�����镶�����TextBox�N���X�ɑ���

	// �G�t�F�N�g����
	// ����΁��ɏ���

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Stop;	// ���f
}

// �y�퓬�z
MapUnit_Action_Battle::MapUnit_Action_Battle(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�G���J�E���gID�z�擾
	getline(L_stream, L_token, ',');
	encID = boost::lexical_cast<int>(L_token);
	// 4�y�w�iID�z�擾
	getline(L_stream, L_token, ',');
	backID = boost::lexical_cast<int>(L_token);
	// 5�y�����\�t���O�z�擾
	getline(L_stream, L_token, ',');
	escapeFlag = L_token == "1";
	// 6�y�����C�x�t���O�z�擾
	getline(L_stream, L_token, ',');
	loseEventFlag = L_token == "1";
	// 7�y�������̕���z�擾
	getline(L_stream, L_token, ',');
	branch[0] = boost::lexical_cast<int>(L_token);
	// 8�y�S�Ŏ��̕���z�擾
	getline(L_stream, L_token, ',');
	branch[1] = boost::lexical_cast<int>(L_token);
	// 9�y�������̕���z�擾
	getline(L_stream, L_token, ',');
	branch[2] = boost::lexical_cast<int>(L_token);

	// ������
	startFlag = true;		// �J�n�����t���O
	battleFlag = false;		// �퓬���t���O
	battleEndType = eEndStatus::Lose;	// �퓬�I���̏��
}
MapUnit_Action_Battle::~MapUnit_Action_Battle() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Battle::Execution() {
	//Game::Encounter(encID);	// �퓬�J�n

	if (startFlag) {
		// �J�n����
		// ������
		battleFlag = true;		// �퓬���t���O
		battleEndType = eEndStatus::Lose;	// �퓬�I���̏��

		startFlag = false;		// �퓬�I���҂���Ԃ�
		Game::Encounter(encID, backID, LoadSoundMem("sound\\bossbattle.wav"), escapeFlag, loseEventFlag, &battleFlag, &battleEndType);		// �퓬�J�n
		return eMUAReturn::Stop;	// ���f
	}
	else {
		// �퓬�I���҂����
		if (battleFlag) {
			// �퓬��
			return eMUAReturn::Stop;	// ���f
		}

		// �퓬�I��
		(*actionNum) += branch[(int)battleEndType];

		// �����C�x�Ȃ�S��HP1�Ő����Ԃ点��
		if (loseEventFlag) {
			std::vector<std::string> str;
			for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
				Player::character[i]->Resurrection(0);
				Player::character[i]->GetEffectText(str);
			}
		}

		// ������
		startFlag = true;		// �J�n�����t���O
		battleFlag = false;		// �퓬���t���O
		battleEndType = eEndStatus::Lose;	// �퓬�I���̏��

		return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
	}
}

// �y�_���[�W�z
MapUnit_Action_Damage::MapUnit_Action_Damage(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�Ώہz�擾
	getline(L_stream, L_token, ',');
	target = boost::lexical_cast<int>(L_token);
	// 4�y�_���[�W�ʁz�擾
	getline(L_stream, L_token, ',');
	damage = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Damage::~MapUnit_Action_Damage() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Damage::Execution() {
	std::vector<string> message;

	switch (target){
	case 0:// �y�S���z
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			Player::character[i]->ReceiveSkillIni();
			Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
			Player::character[i]->Damage(damage);	// �_���[�W��^����
			Player::character[i]->GetEffectText(message);
		}
		break;
	case 1:// �y�擪�z
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			if (Player::character[i]->GetAlive() == eAlive::alive) {
				Player::character[i]->ReceiveSkillIni();
				Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
				Player::character[i]->Damage(damage);	// �_���[�W��^����
				Player::character[i]->GetEffectText(message);
				break;
			}
		}
		break;
	case 2:// �y�����_���z
	{
		int i = 0;
		do {
			i = GetRand(Player::character.size() - 1);
		} while (Player::character[i]->GetAlive() != eAlive::alive);
		Player::character[i]->ReceiveSkillIni();
		Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
		Player::character[i]->Damage(damage);	// �_���[�W��^����
		Player::character[i]->GetEffectText(message);
		break;
	}
	default:
		break;
	}
	if (!message.empty()) {
		TextBox::AddLine(message);
	}

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Stop;	// ���f
}

// �y�񕜁z
MapUnit_Action_Recovery::MapUnit_Action_Recovery(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�Ώہz�擾
	getline(L_stream, L_token, ',');
	target = boost::lexical_cast<int>(L_token);
	// 4�yHP�񕜗ʁz�擾
	getline(L_stream, L_token, ',');
	heal_HP = boost::lexical_cast<int>(L_token);
	// 5�yMP�񕜗ʁz�擾
	getline(L_stream, L_token, ',');
	heal_MP = boost::lexical_cast<int>(L_token);
	// 6�y�h���t���O�z�擾
	getline(L_stream, L_token, ',');
	resurrection = L_token == "1";
}
MapUnit_Action_Recovery::~MapUnit_Action_Recovery() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Recovery::Execution() {
	std::vector<string> message;

	switch (target) {
	case 0:// �y�S���z
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			// �X�L���t���O������
			Player::character[i]->ReceiveSkillIni();
			// �h��
			if (resurrection) {
				Player::character[i]->FullResurrection();
			}
			// HP��
			if (heal_HP == -1)	Player::character[i]->FullRecovery();
			else				Player::character[i]->Recovery(heal_HP);
			// MP��
			if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
			else				Player::character[i]->Recovery_MP(heal_MP);
			// ���b�Z�[�W�擾
			Player::character[i]->GetEffectText(message);
		}
		break;
	case 1:// �y�擪�z
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			if (Player::character[i]->GetAlive() == eAlive::alive) {
				Player::character[i]->ReceiveSkillIni();
				if (heal_HP == -1)	Player::character[i]->FullRecovery();
				else				Player::character[i]->Recovery(heal_HP);
				if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
				else				Player::character[i]->Recovery_MP(heal_MP);
				Player::character[i]->GetEffectText(message);
				break;
			}
		}
		break;
	case 2:// �y�����_���z
	{
		int i = 0;
		do {
			i = GetRand(Player::character.size() - 1);
		} while (Player::character[i]->GetAlive() != eAlive::alive);
		Player::character[i]->ReceiveSkillIni();
		if (heal_HP == -1)	Player::character[i]->FullRecovery();
		else				Player::character[i]->Recovery(heal_HP);
		if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
		else				Player::character[i]->Recovery_MP(heal_MP);
		Player::character[i]->GetEffectText(message);
		break;
	}
	default:
		break;
	}
	if (!message.empty()) {
		TextBox::AddLine(message);
	}

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Stop;	// ���f
}

// �y�A�C�e���擾�z
MapUnit_Action_Item::MapUnit_Action_Item(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream stream(line);
	string token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(stream, token, ',');
	// 2 �y�����̃^�C�v�z
	getline(stream, token, ',');
	// 3 �y�擾�A�C�e����ID�z�擾
	getline(stream, token, ',');
	itemID = boost::lexical_cast<int>(token);
	// 4 �y�A�C�e������ɓ��ꂽ�ꍇ�̃W�����v�z�擾
	getline(stream, token, ',');
	jump_True = boost::lexical_cast<int>(token);
	// 5�y�������������ς��������ꍇ�̃W�����v�z�擾
	getline(stream, token, ',');
	jump_False = boost::lexical_cast<int>(token);
}
MapUnit_Action_Item::~MapUnit_Action_Item() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Item::Execution() {
	if (Player::InsertItem(itemID)) {		// �A�C�e�������
		TextBox::AddLine(Item::GetName(itemID) + " ����ɓ��ꂽ");
		(*actionNum) += jump_True;	// �W�����v
		return eMUAReturn::Stop;	// ���f
	}
	else {
		TextBox::AddLine("�������������ς��� " + Item::GetName(itemID) + " �����ĂȂ�����");
		(*actionNum) += jump_False;	// �W�����v
		return eMUAReturn::Stop;	// ���f
	}
}

// �y�X�z
MapUnit_Action_Shop::MapUnit_Action_Shop(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�XID�z�擾
	getline(L_stream, L_token, ',');
	shopID = L_token;
}
MapUnit_Action_Shop::~MapUnit_Action_Shop() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Shop::Execution() {
	Shop::InitShop(stoi(shopID));	// �X�̂h�c�ŏ��i������������
	Player::flag_shop = true;		// �X�t���O�𗧂Ă�

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Stop;	// ���f
}

// �y����-�o���z
MapUnit_Action_Existence::MapUnit_Action_Existence(string line, bool& appearFlag) :
	appearFlag(appearFlag)
{
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�t���O�z�擾
	getline(L_stream, L_token, ',');
	flag = L_token == "1";
	// 4�y���o�z�擾
	getline(L_stream, L_token, ',');
	staging = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Existence::~MapUnit_Action_Existence() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Existence::Execution() {
	appearFlag = flag;

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
}

// �y���[�h�ύX�z
MapUnit_Action_ChangeMode::MapUnit_Action_ChangeMode(string line, int& nowMode, const vector<MapUnit_Mode*>& mode) :
	nowMode(nowMode),
	mode(mode)
{
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y���[�h���z�擾
	getline(L_stream, L_token, ',');
	modeNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_ChangeMode::~MapUnit_Action_ChangeMode() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_ChangeMode::Execution() {
	// ���[�h��ύX
	if (modeNum >= 0 && (unsigned)modeNum < mode.size()) {
		nowMode = modeNum;
	}

	++(*actionNum);		// ���̋�����
	return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
}

// �y���b�Z�[�W���M�z
MapUnit_Action_Message::MapUnit_Action_Message(string line) : MapUnit_Action() {
	// �y�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y���b�Z�[�W�ԍ��z�擾
	getline(L_stream, L_token, ',');
	messageNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Message::~MapUnit_Action_Message() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Message::Execution() {
	MapObject::messageData_Stack.push_back(messageNum);		// ���b�Z�[�W�X�^�b�N�ɒǉ�
	return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
}

// �y�I�𕪊�z
MapUnit_Action_SelectBranch::MapUnit_Action_SelectBranch(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y���╶�z�擾
	getline(L_stream, L_token, ',');
	question = L_token;
	// �y�I�����z�擾
	while (getline(L_stream, L_token, ',')) {
		if (L_token == "") break;
		// �y�I�������z�擾
		choices.push_back({ L_token, 1});
		// �y�����z�擾
		getline(L_stream, L_token, ',');
		choices.back().destination = boost::lexical_cast<int>(L_token);
	}

	// ������
	step = eSelectBranch::question;
	select = -1;
	selectFlag = false;
}
MapUnit_Action_SelectBranch::~MapUnit_Action_SelectBranch() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_SelectBranch::Execution() {
	switch (step)
	{
	case eSelectBranch::question:// �y����z
		// ������
		select = -1;
		selectFlag = false;
		// ���╶�\��
		TextBox::AddSelect(question, choices, selectFlag, select);
		step = eSelectBranch::wait;	// �𓚑҂���Ԃ�
		return eMUAReturn::Stop;	// ���f
	case eSelectBranch::wait:// �y�񓚑҂��z
		if (!selectFlag) {
			return eMUAReturn::Stop;	// ���f
		}
		// �y�𓚎󂯎��z
		// ���򏈗�
		if (select >= 0 && select < (int)choices.size()) {
			// �A�E�g�I�u�����W�΍�
			(*actionNum) += choices[select].destination;
		}
		else {
			++(*actionNum);
		}
		
		// ������
		step = eSelectBranch::question;
		select = -1;
		selectFlag = false;

		return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
	default:
		// ������
		step = eSelectBranch::question;
		select = -1;
		selectFlag = false;
		++(*actionNum);		// ���̋�����
		return eMUAReturn::End;	// �����I��
	}
}

// �y�����I���z
MapUnit_Action_End::MapUnit_Action_End(){

}
MapUnit_Action_End::~MapUnit_Action_End() {

}
eMUAReturn MapUnit_Action_End::Execution() {
	return eMUAReturn::End;	// �����I��
}

// �y�����ԍ��̈ړ��z
MapUnit_Action_Goto::MapUnit_Action_Goto(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y���╶�z�擾
	getline(L_stream, L_token, ',');
	moveNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Goto::~MapUnit_Action_Goto() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_Goto::Execution() {
	(*actionNum) += moveNum;
	return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
}

// �y�����Q�[���I�[�o�[�z
MapUnit_Action_GameOver::MapUnit_Action_GameOver() {

}
MapUnit_Action_GameOver::~MapUnit_Action_GameOver() {

}
eMUAReturn MapUnit_Action_GameOver::Execution() {
	Scene::nowScene = GameScene::GAMEOVER;
	return eMUAReturn::End;	// �����I��
}

// �y�Q�[���N���A�z
MapUnit_Action_GameClear::MapUnit_Action_GameClear() {

}
MapUnit_Action_GameClear::~MapUnit_Action_GameClear() {

}
eMUAReturn MapUnit_Action_GameClear::Execution() {
	Scene::nowScene = GameScene::GAMECLEAR;
	return eMUAReturn::End;	// �����I��
}

// �y���𕥂��z
MapUnit_Action_PayMoney::MapUnit_Action_PayMoney(string line) {
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream stream(line);
	string token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(stream, token, ',');
	// 2�y�����̃^�C�v�z
	getline(stream, token, ',');
	// 3�y�������z�z�擾
	getline(stream, token, ',');
	price = boost::lexical_cast<int>(token);
	// 4�y�������肽�ꍇ�̃W�����v�z�擾
	getline(stream, token, ',');
	jump_True = boost::lexical_cast<int>(token);
	// 5�y��������Ȃ������ꍇ�̃W�����v�z�擾
	getline(stream, token, ',');
	jump_False = boost::lexical_cast<int>(token);
}
MapUnit_Action_PayMoney::~MapUnit_Action_PayMoney() {
	// ���ɂȂ�
}
eMUAReturn MapUnit_Action_PayMoney::Execution() {
	if (price <= Player::GetMoney()) {
		Player::PayMoney(price);
		(*actionNum) += jump_True;
	}
	else {
		(*actionNum) += jump_False;
	}
	
	return eMUAReturn::Next;	// �����Ɏ��̋����ɂ���
}
/*-------------------------------------------------���j�b�g�̋��������f�[�^--------------------------------------------*/
// �����̎��s
bool MapUnit_If::ActionExecution() {
	/*// �����Ă�������̐�������������
	for (int i = 0, n = action.size(); i < n; i++) {
		action[i]->Execution();	// �e�����̕]���Ǝ��s
	}*/

	bool nextFlag = true;

	while (actionNum < (int)action.size() && nextFlag) {
		switch (action[actionNum]->Execution()){
		case eMUAReturn::Next:// �y���̋����ɂ���z

			break;
		case eMUAReturn::Stop:// �y���f�z
			nextFlag = false;
			break;
		case eMUAReturn::End:// �y�����I���z
			return false;

		default:// �y�G���[�z
			return false;	// �I��
			break;
		}
	}

	/*if (action[actionNum]->Execution()) {	// �s��������s
		return false;	// �����I��
	}*/

	return actionNum < (int)action.size();		// ���̍s�������邩��Ԃ�
}
// �����ǉ�
void MapUnit_If::AddAction(MapUnit_Action* action) {
	this->action.push_back(action);	// ������ǉ�����
	this->action.back()->SetActionNum(&actionNum);
}
// �I������
void MapUnit_If::EndProcess() {
	// �����N���X�폜
	for (int i = 0, n = action.size(); i < n; i++) {
		delete action[i];
	}
}

// �y��l�������̃��j�b�g�ɘb��������z
MapUnit_If_Talk::MapUnit_If_Talk(string line, const int& x, const int& y)
	:x(x),
	y(y) 
{
	ifType = eUnit_If::Talk;	// �����̎��
}
MapUnit_If_Talk::~MapUnit_If_Talk() {
	// ���ɂȂ�
}
bool MapUnit_If_Talk::ChackIf() {
	// �v���C���[���b�������铮����s�����ꍇ
	if (Player::talkFlag.flag) {
		// �b��������ꂽ���ǂ����𔻒肷��
		if (abs(x - Player::talkFlag.pos_.X) <= (CHIPSIZE - 1) && abs(y - Player::talkFlag.pos_.Y) <= (CHIPSIZE - 1)) {
			actionNum = 0;	// ���s���̍s���̗v�f�ԍ����O�ɖ߂�
			return ActionExecution();
		}
	}
	return false;
}

// �y��l�������̃��j�b�g�ɐG���z
MapUnit_If_Touch::MapUnit_If_Touch(string line, const int& x, const int& y)
	:x(x),
	y(y)
{
	ifType = eUnit_If::Touch;	// �����̎��
}
MapUnit_If_Touch::~MapUnit_If_Touch() {
	// ���ɂȂ�
}
bool MapUnit_If_Touch::ChackIf() {
	// �G�ꂽ���ǂ����𔻒肷��
	if (abs(x - Player::pos_.X) <= (CHIPSIZE - 1) && abs(y - Player::pos_.Y) <= (CHIPSIZE - 1)) {
		actionNum = 0;	// ���s���̍s���̗v�f�ԍ����O�ɖ߂�
		return ActionExecution();	// ��ڂ̍s���������s�A���̍s���������true��ς���
	}
	return false;
}

// �y���b�Z�[�W��M�z
MapUnit_If_Message::MapUnit_If_Message(string line) {
	ifType = eUnit_If::Message;	// �����̎��

	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y���b�Z�[�W�ԍ��z�擾
	getline(L_stream, L_token, ',');
	messageNum = boost::lexical_cast<int>(L_token);
}
MapUnit_If_Message::~MapUnit_If_Message() {
	// ���ɂȂ�
}
bool MapUnit_If_Message::ChackIf() {
	// ���b�Z�[�W��S����
	for (int i = 0, n = (int)MapObject::messageData.size(); i < n; ++i) {
		if (messageNum == MapObject::messageData[i]) {
			actionNum = 0;	// ���s���̍s���̗v�f�ԍ����O�ɖ߂�
			return ActionExecution();	// ��ڂ̍s���������s�A���̍s���������true��ς���
		}
	}
	return false;
}

// �y�펞�z
MapUnit_If_Always::MapUnit_If_Always() {
	ifType = eUnit_If::Always;	// �����̎��
}
MapUnit_If_Always::~MapUnit_If_Always() {
	// ���ɂȂ�
}
bool MapUnit_If_Always::ChackIf() {
	actionNum = 0;	// ���s���̍s���̗v�f�ԍ����O�ɖ߂�
	return ActionExecution();	// ��ڂ̍s���������s�A���̍s���������true��ς���
}

// �y�߂Â�or�����z
MapUnit_If_Distance::MapUnit_If_Distance(string line, const int& x, const int& y)
	:x(x),
	y(y)
{
	ifType = eUnit_If::Distance;	// �����̎��

	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�����̃^�C�v�z
	getline(L_stream, L_token, ',');
	// 3�y�����i���a�j�z�擾
	getline(L_stream, L_token, ',');
	distance = boost::lexical_cast<int>(L_token);
	// 3�y�߂Â�/�����z�擾
	getline(L_stream, L_token, ',');
	In_Out = L_token == "1";
}
MapUnit_If_Distance::~MapUnit_If_Distance() {
	// ���ɂȂ�
}
bool MapUnit_If_Distance::ChackIf() {
	if (abs(x - Player::pos_.X) <= (distance) && abs(y - Player::pos_.Y) <= (distance)) {
		actionNum = 0;	// ���s���̍s���̗v�f�ԍ����O�ɖ߂�
		return ActionExecution();	// ��ڂ̍s���������s�A���̍s���������true��ς���
	}
	return false;
}
/*-------------------------------------------------���j�b�g�̈ړ��f�[�^--------------------------------------------*/
// �y�X�[�p�[�N���X�z
MapUnit_Move::MapUnit_Move(int& x, int& y, Direction& dir) : x(x), y(y), dir(dir){
	preX = x;
	preY = y;
}
// ���W�C��
void MapUnit_Move::PosCorrection() {
	x = preX;	// ���W�𒼑O�̍��W�ɖ߂�
	y = preY;
	moveRange -= speed;	// �ړ�������߂�
	// �������t�ɂ���
	switch (dir) {
	case UP:	dir = DOWN;
		break;
	case DOWN:	dir = UP;
		break;
	case LEFT:	dir = RIGHT;
		break;
	case RIGHT:	dir = LEFT;
		break;
	default:
		break;
	}
	moveRange = 64 - moveRange;
}

// �y�����Ȃ��z
MapUnit_Move_None::MapUnit_Move_None(int& x, int& y , Direction& dir) :
	MapUnit_Move(x, y, dir) 
{
	// ���Ƀ��[�h�Ȃ�
	moveFlag = false;
	moveRange = 0;
	speed = 0;
	delay = 0;
}
MapUnit_Move_None::~MapUnit_Move_None() {
	// ���ɂȂ�
}
void MapUnit_Move_None::Move() {
	preX = x;
	preY = y;
}

// �y�����_���z
MapUnit_Move_Random::MapUnit_Move_Random(string line, int& x, int& y, Direction& dir) :
	MapUnit_Move(x, y, dir)
{
	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y�ړ��̎�ށz
	getline(L_stream, L_token, ',');
	// 3�y�ړ����x�z�擾
	getline(L_stream, L_token, ',');
	speed = boost::lexical_cast<int>(L_token);
	// 4�y�ړ����Ƃ�delay�z�擾
	getline(L_stream, L_token, ',');
	delay = boost::lexical_cast<int>(L_token);

	iniX = x;
	iniY = y;
	count = 0;
	moveFlag = false;
	moveRange = 0;
	moveCount = 0;

	// �ړ����x��0�ȉ��ɂȂ�Ȃ��悤��
	if (speed <= 0) {
		speed = 1;
	}

}
MapUnit_Move_Random::~MapUnit_Move_Random() {

}
void MapUnit_Move_Random::Move() {
	if (!moveFlag) {
		if (count >= delay) {
			if (moveCount >= 2) {
				// �R�����Ƃɏ������W�̕����ɓ���
				moveCount = 0;
				if (abs(x - iniX) > abs(y - iniY)){
					if (x < iniX)	dir = RIGHT;	// �E
					else			dir = LEFT;		// ��
				}
				else {
					if (y < iniY)	dir = DOWN;		// ��
					else			dir = UP;		// ��
				}
			}
			else {
				dir = (Direction)GetRand(3);	// �����_���Ɉړ���������
				++moveCount;
			}
			moveFlag = true;// �ړ��t���O�𗧂Ă�
			moveRange = 0;	// �ړ��������Z�b�g
			count = 0;		// �ړ��J�E���g���Z�b�g
		}
		else {
			++count;	// �ړ��J�E���g+
		}
	}
	if (moveFlag) {
		preX = x;	// �ړ��O�̍��W
		preY = y;

		// �ړ���
		switch (dir){
		case UP:	y -= speed;
			break;
		case DOWN:	y += speed;
			break;
		case LEFT:	x -= speed;
			break;
		case RIGHT:	x += speed;
			break;
		default:
			break;
		}
		moveRange += speed;	// �ړ�����+
		if (moveRange > 64) {
			// 1�}�X�ړ�������ړ�����߂�
			moveFlag = false;	// �ړ��t���O��܂�
			// �ړ�������������߂�
			switch (dir) {
			case UP:	y += moveRange - 64;
				break;
			case DOWN:	y -= moveRange - 64;
				break;
			case LEFT:	x += moveRange - 64;
				break;
			case RIGHT:	x -= moveRange - 64;
				break;
			default:
				break;
			}

		}
	}
	

}

/*-------------------------------------------------���j�b�g�̃��[�h--------------------------------------------*/
// �y�X�[�p�[�N���X�z
MapUnit_Mode::MapUnit_Mode() {
	move = nullptr;
	termNum = 0;
}
// �����̎��s������ǉ�
void MapUnit_Mode::AddTerm(MapUnit_If* term) {
	terms.push_back(term);
}
// �����ǉ�
void MapUnit_Mode::AddAction(MapUnit_Action* action) {
	terms.back()->AddAction(action);
}
// �����̎��s��������
bool MapUnit_Mode::TermsCheck() {
	// ���[�h�̏����S����
	for (int i = 0, n = terms.size(); i < n; i++) {
		// ���s������̂�����Ύ��s���Ɉڍs
		if (terms[i]->ChackIf()) {
			termNum = i;
			return true;
		}
	}
	return false;
}
// �s�����s
bool MapUnit_Mode::ActionExecution() {
	return terms[termNum]->ActionExecution();
}
// �y�ړ��z
void MapUnit_Mode::Move() {
	if(move) move->Move();
}
// �y���W�C���z
void MapUnit_Mode::PosCorrection() {
	if (move) move->PosCorrection();
}
// �y�ړ��N���X�̃Z�b�^�[�z
void MapUnit_Mode::SetMove(MapUnit_Move* data) {
	if (move) {
		// ���łɈړ��N���X������ꍇ�̓Z�b�g���Ȃ�
		delete data;
	}
	else {
		move = data;
	}
}
// �y�I�������z
void MapUnit_Mode::EndProcess() {
	// �����N���X�폜
	for (int i = 0, n = terms.size(); i < n; i++) {
		terms[i]->EndProcess();
		delete terms[i];
	}
	// �ړ��N���X�폜
	if(move) delete move;
}

// �y�����Ȃ��z
MapUnit_Mode_None::MapUnit_Mode_None(int modeNum, int& nowMode) :
	modeNum(modeNum),
	nowMode(nowMode),
	MapUnit_Mode()
{
	type = eUnitMode::None;
}
MapUnit_Mode_None::~MapUnit_Mode_None() {

}
bool MapUnit_Mode_None::ModeCheck() {
	return modeNum == nowMode;
}

// �y�X�g�[���[�i�s�x�z
MapUnit_Mode_Chapter::MapUnit_Mode_Chapter(string line) {
	type = eUnitMode::None;
	//���[�h�p�ϐ�
	std::istringstream L_stream(line);
	string L_token;

	// 1�y�f�[�^�̎�ށz�j��
	getline(L_stream, L_token, ',');
	// 2�y���[�h���z�j��
	getline(L_stream, L_token, ',');
	// 3�y�ڍs�����z�j��
	getline(L_stream, L_token, ',');
	// 4�y��̐i�s�x�z
	getline(L_stream, L_token, ',');
	chapter = boost::lexical_cast<int>(L_token);
	// 5�y�����z
	getline(L_stream, L_token, ',');
	term = boost::lexical_cast<int>(L_token);

}
MapUnit_Mode_Chapter::~MapUnit_Mode_Chapter() {

}
bool MapUnit_Mode_Chapter::ModeCheck() {
	switch (term) {
	case 0:// �yn�̂Ƃ��z
		return Player::chapter == chapter;
	case 1:// �yn�ȊO�̂Ƃ��z
		return Player::chapter != chapter;
	case 2:// �yn�ȑO�z
		return Player::chapter <= chapter;
	case 3:// �yn�ȍ~�z
		return Player::chapter >= chapter;
	default:
		return false;
		break;
	}
}
/*-------------------------------------------------���j�b�g�f�[�^--------------------------------------------*/
// ���b�Z�[�W�֘A
vector<int> MapObject::messageData;			// ���b�Z�[�W�̃f�[�^�i1�t���[���O�ɌĂ΂ꂽ���́j
vector<int> MapObject::messageData_Stack;	// ���b�Z�[�W�̃f�[�^�i���t���[���ɌĂ΂ꂽ���́j

// ���b�Z�[�W�X�V����
void MapObject::MessageUpDate() {
	// ����ꂽ���b�Z�[�W�͂܂�messageData_Stack�ɓ���
	// ���̌�A���̃t���[����messageData�Ɉړ����A�S���j�b�g���Q�Ƃ���
	// �܂�A����ꂽ���b�Z�[�W�́A���̃t���[���ɓ͂�

	if (!messageData.empty()) {
		if (!messageData_Stack.empty()) {
			// �y�X�^�b�N������z
			// �X�^�b�N�̓��e�����b�Z�[�W�f�[�^�ɃR�s�[
			messageData = messageData_Stack;
			// �X�^�b�N����ɂ���
			messageData_Stack.clear();
			messageData_Stack.shrink_to_fit();
		}
		else {
			// �y�X�^�b�N���Ȃ��z
			// ���b�Z�[�W�f�[�^����ɂ���
			messageData.clear();
			messageData.shrink_to_fit();
		}
	}
	else {
		if (!messageData_Stack.empty()) {
			// �y�X�^�b�N������z
			// �X�^�b�N�̓��e�����b�Z�[�W�f�[�^�ɃR�s�[
			messageData = messageData_Stack;
			// �X�^�b�N����ɂ���
			messageData_Stack.clear();
			messageData_Stack.shrink_to_fit();
		}
	}
}

MapUnit::MapUnit(string line) {
	// ������
	collisionFlag	= true;
	appearFlag		= true;
	execFlag		= false;	// �s�����s���t���O
	nowMode			= 0;
	areaX			= 0;
	areaY			= 0;
	count			= 0;
	dir				= DOWN;

	// �y�z�u�f�[�^�ǂݍ��݁z
	//���[�h�p�ϐ�
	istringstream L_stream(line);
	string L_token;
	
	// 1�y���j�b�gID�z
	getline(L_stream, L_token, ',');
	id = boost::lexical_cast<int>(L_token);
	// 2�y���O�z�擾
	getline(L_stream, L_token, ',');
	name = L_token;
	// 3�yX���W�z�擾�i�}�b�v���W�j
	getline(L_stream, L_token, ',');
	initX = x = boost::lexical_cast<int>(L_token) * CHIPSIZE;
	// 4�yY���W�z�擾�i�}�b�v���W�j
	getline(L_stream, L_token, ',');
	initY = y = boost::lexical_cast<int>(L_token) * CHIPSIZE;
	// 5�y�摜�z�擾
	getline(L_stream, L_token, ',');
	if (L_token == "0") {
		L_token = "null";
	}
	string str = "img\\unit\\" + L_token + ".png";	// �A�h���X����
	char* cstr = new char[str.size() + 1];			// �������m��
	strcpy_s(cstr, str.size() + 1, str.c_str());	// �R�s�[
	img = LoadGraph(cstr, 4, 4);
	delete cstr;	// ���������	
	// 6�y�����蔻��z�擾
	getline(L_stream, L_token, ',');
	collisionFlag = L_token == "1";
	// 7�y�o���t���O�z�擾
	getline(L_stream, L_token, ',');
	appearFlag = L_token == "1";
	// 8�y���݂̃��[�h���z�擾
	getline(L_stream, L_token, ',');
	nowMode = boost::lexical_cast<int>(L_token);

	LoadData();		// �f�[�^�̃��[�h
}

void MapUnit::LoadData() {
	// �y�t�@�C���ǂݍ��ݗp�ϐ��z
	ifstream L_File;	// �ǂݍ��ރt�@�C��
	string L_Line;		// �ǂݍ��񂾍s�i1�s�j
	int L_Count = 0;	// �m�[�h�ǂݍ��ݗp�i���s�ڂ�ǂݍ���ł��邩�j
	string L_token;		// 1�s��,�ŕ�������ۂɎg��

	int L_Mode;		// �ǂݍ��ݎ��̃��[�h�w��

	// �ǂݍ��ރt�@�C���̎w��
	L_File.open("unitdata\\" + to_string(Player::stage) + "\\data\\" + to_string(id) + ".csv", ios::in);	// �t�@�C���I�[�v��

	if (L_File.fail()) {	// �t�@�C���ǂݍ���
		name = "�ǂݍ��߂ĂȂ���";
	}
	else {

		while (getline(L_File, L_Line)) {	// ��s���ǂݍ���
			// �ǂݍ��񂾍s���R�s�[
			std::istringstream L_stream(L_Line);
			// 1��ڂ����[�h���Ĕ���
			getline(L_stream, L_token, ',');

			switch (boost::lexical_cast<int>(L_token)){
			case 0:	// �y���[�h�z
			{
				// 2�y���[�h���z
				getline(L_stream, L_token, ',');
				L_Mode = boost::lexical_cast<int>(L_token);
				// ���[�h�������Ȃ����
				if ((unsigned)L_Mode >= mode.size()) {
					// ���[�h���𑝂₷
					mode.resize(L_Mode + 1);
				}
				// 3�y����ID�z����
				getline(L_stream, L_token, ',');
				switch (static_cast<eUnitMode>(boost::lexical_cast<int>(L_token))){
				case eUnitMode::None:// �y�����Ȃ��z
					mode[L_Mode] = new MapUnit_Mode_None(L_Mode, nowMode);
					break;
				default:
					break;
				}
				break;
			}
			case 1:	// �y�ړ��z
				// 2�y�ړ��^�C�v�z����
				getline(L_stream, L_token, ',');
				switch (boost::lexical_cast<int>(L_token)){
				case 0:	// �y�����Ȃ��z
					mode[L_Mode]->SetMove(new MapUnit_Move_None(x, y, dir));
					break;
				case 1:	// �y��l����ǂ�or������z

					break;
				case 2:	// �y���[�v�ړ��z

					break;
				case 3:	// �y�����_���ړ��z
					mode[L_Mode]->SetMove(new MapUnit_Move_Random(L_Line, x, y, dir));
					break;
				}
				break;
			case 2:	// �y�����̎��s�����z
				// 2�y�����^�C�v�z����
				getline(L_stream, L_token, ',');
				switch ((eUnit_If)boost::lexical_cast<int>(L_token)){
				case eUnit_If::Talk:	// �y�b��������z
					mode[L_Mode]->AddTerm(new MapUnit_If_Talk(L_Line, x, y));
					break;
				case eUnit_If::Touch:	// �y�G���z
					mode[L_Mode]->AddTerm(new MapUnit_If_Touch(L_Line, x, y));
					break;
				case eUnit_If::Message:	// �y���b�Z�[�W����M����z
					mode[L_Mode]->AddTerm(new MapUnit_If_Message(L_Line));
					break;
				case eUnit_If::Always:	// �y�펞�z
					mode[L_Mode]->AddTerm(new MapUnit_If_Always());
					break;
				case eUnit_If::Distance:// �y��l���Ƃ̋�������z
					mode[L_Mode]->AddTerm(new MapUnit_If_Distance(L_Line, x, y));
					break;
				}
				break;
			case 3:	// �y�����z
				// 2�y�����^�C�v�z����
				getline(L_stream, L_token, ',');
				switch (static_cast<eUnit_Act>(boost::lexical_cast<int>(L_token))) {
				case eUnit_Act::Warp:// �y�]�ځz
					mode[L_Mode]->AddAction(new MapUnit_Action_Warp(L_Line));
					break;
				case eUnit_Act::Talk:// �y�Z���t�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Talk(L_Line));
					break;
				case eUnit_Act::Battle:// �y�퓬�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Battle(L_Line));
					break;
				case eUnit_Act::Damage:// �y�_���[�W�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Damage(L_Line));
					break;
				case eUnit_Act::Recovery:// �y�񕜁z
					mode[L_Mode]->AddAction(new MapUnit_Action_Recovery(L_Line));
					break;
				case eUnit_Act::Item:// �y�A�C�e���擾�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Item(L_Line));
					break;
				case eUnit_Act::Shop:// �y�X�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Shop(L_Line));
					break;
				case eUnit_Act::Existence:	// �y����-�o���z
					mode[L_Mode]->AddAction(new MapUnit_Action_Existence(L_Line, appearFlag));
					break;
				case eUnit_Act::ChangeMode:// �y���[�h�ύX�z
					mode[L_Mode]->AddAction(new MapUnit_Action_ChangeMode(L_Line, nowMode, mode));
					break;
				case eUnit_Act::Message:// �y���b�Z�[�W�z
					mode[L_Mode]->AddAction(new MapUnit_Action_Message(L_Line));
					break;
				case eUnit_Act::SelectBranch:// �y�I�𕪊�z
					mode[L_Mode]->AddAction(new MapUnit_Action_SelectBranch(L_Line));
					break;
				case eUnit_Act::End:// �y�����I���z
					mode[L_Mode]->AddAction(new MapUnit_Action_End());
					break;
				case eUnit_Act::Goto:// �y�����ԍ��̈ړ��z
					mode[L_Mode]->AddAction(new MapUnit_Action_Goto(L_Line));
					break;
				case eUnit_Act::GameOver:// �y�����Q�[���I�[�o�[�z
					mode[L_Mode]->AddAction(new MapUnit_Action_GameOver());
					break;
				case eUnit_Act::GameClear:// �y�Q�[���N���A�z
					mode[L_Mode]->AddAction(new MapUnit_Action_GameClear());
					break;
				case eUnit_Act::PayMoney:// �y���𕥂��z
					mode[L_Mode]->AddAction(new MapUnit_Action_PayMoney(L_Line));
					break;
				default:
					break;
				}
				break;
			}
		}
	}

	// �t�@�C�������
	L_File.close();

}
MapUnit::~MapUnit() {
	// ���ɂȂ�
}

// ���t���[���̏���
bool MapUnit::Process() {
	// �o�����̂�
	if (appearFlag) {
		
		// ���[�h�̏����S����
		for (int i = 0, n = mode.size(); i < n; ++i) {
			if (mode[i]->ModeCheck()) {
				nowMode = i;
				break;
			}
		}
		// �ړ�
		mode[nowMode]->Move();
		// �s����������
		if (mode[nowMode]->TermsCheck()) {
			execFlag = true;
			return true;
		}
		
	}
	return false;
}

// �`��
void MapUnit::Draw(int cameraX, int cameraY) {
	if (!appearFlag) return;
	int drawX = x - cameraX;
	int drawY = y - cameraY;
	if (drawX >= -64 && drawX <= 1920 && drawY >= -64 && drawY <= 1920) {
		DrawGraph(drawX, drawY, img, count / 10, dir);
	}

	++count;	// �`��J�E���g
	if (count > 40) {
		count = 0;
	}
}

// �s�����s
bool MapUnit::ActionExecution() {
	return mode[nowMode]->ActionExecution();
}

// ���j�b�g�̏�Ԃ̕ύX
void MapObject::ChangeStatus(eUnitStatus type) {
	switch (type){
	case eUnitStatus::Apear:	// �y�o���z
		appearFlag = true;
		break;
	case eUnitStatus::Disapear:	// �y���Łz
		appearFlag = false;
		break;
	case eUnitStatus::SetColl:	// �y�����蔻���t����z
		collisionFlag = true;
		break;
	case eUnitStatus::DeletColl:	// �y�����蔻��������z
		collisionFlag = false;
		break;
	default:
		break;
	}
}
// �ړ�����
void MapObject::SetDir(Direction dir) {
	this->dir = dir;
}
bool MapObject::CheckColl(int x, int y) {
	// �����蔻�肠�� & �o�����̂�
	if (appearFlag) {
		if (collisionFlag) {
			if (abs(this->x - x) <= (CHIPSIZE - 1) && abs(this->y - y) <= (CHIPSIZE - 1)) {
				return true;
			}
		}
	}

	return false;
}

void MapUnit::EndProcess() {
	// �����N���X�폜
	for (int i = 0, n = mode.size(); i < n; i++) {
		mode[i]->EndProcess();
		delete mode[i];
	}
}

// ���W�C��
void MapUnit::PosCorrection() {
	mode[nowMode]->PosCorrection();
}

// ���[�h�̃Z�b�^�[
void MapUnit::SetMode(int modeNum) {
	
	// �A�E�g�I�u�����W�΍�
	if (modeNum < 0 || mode.size() <= (unsigned)modeNum) {
		nowMode = 0;
	}
	else {
		nowMode = modeNum;
	}
}
// ���[�h�̃Q�b�^�[
int MapUnit::GetMode() {
	return nowMode;
}
// �敪�����W�̃Z�b�^�[
void MapUnit::SetArea(int areaX, int areaY) {
	this->areaX = areaX;
	this->areaY = areaY;
}