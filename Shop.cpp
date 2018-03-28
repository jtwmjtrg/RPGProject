#include "Shop.h"

vector<Item*>	Shop::item;
ItemBox			Shop::forSale;
string			Shop::greeting;
string			Shop::confirmation;
string			Shop::who;
vector<BattleStatus> Shop::bs;
vector<vector<BattleStatus> > Shop::player_bs;	// �v���C���[�̑����̃o�g���X�e�[�^�X
ItemBox			Shop::charaMenu;
ItemBox			Shop::decideMenu;

Shop::Shop(){
	forSale.Initialize(0, 0, 512, 256);
	decideMenu.itemName.push_back("�͂�");
	decideMenu.itemName.push_back("������");
	decideMenu.Initialize(1024, 96, 320, 128);
	for (int i = 0, n = Player::character.size(); i < n; ++i) {
		// �L�����N�^�[�̐��������[�v
		charaMenu.itemName.push_back(Player::character[i]->GetName());
	}
	charaMenu.Initialize(1344, 96, 160, 64 + Player::character.size() * 32);
}
Shop::~Shop() {
	for (int i = 0, n = item.size(); i < n; ++i) {
		if (item[i]) {
			delete item[i];
		}
	}
}

void Shop::InitShop(int id) {
	for (int i = 0, n = item.size();i < n;++i) {
		// �A�C�e���ꗗ���폜
		delete item[i];
	}
	item.clear();			// �z����N���A
	item.shrink_to_fit();	// �T�C�Y�����킹��

	ifstream ifs("shop\\" + to_string(id) + ".csv");	// �t�@�C���I�[�v��
	if (!ifs) {
		// �t�@�C�����Ȃ��ꍇ
		return;	// �G���[
	}

	string s;	// �ǂݍ��ݗp
	for (int i = 0;i <= id;++i) {
		// �w�肵���f�[�^�܂œǂݐi�߂�
		getline(ifs, s);
	}
	stringstream stream(s);	// 
	string token;			// 
	while (getline(stream, token, ',')) {
		// �I�_�܂ŃJ���}��؂�œǂݍ���
		item.push_back(Item::Factory(stoi(token)));	// id����A�C�e�����擾����
	}
	int itemnum = item.size();
	// ��������
	getline(ifs, s);
	greeting = s;
	for (int i = 0;i < itemnum - 1;++i) {
		greeting.pop_back();
	}
	// �m�F
	getline(ifs, s);
	confirmation = s;
	for (int i = 0;i < itemnum - 1;++i) {
		confirmation.pop_back();
	}
	// �N�H
	getline(ifs, s);
	who = s;
	for (int i = 0;i < itemnum - 1;++i) {
		who.pop_back();
	}

	// �A�C�e���̐퓬�X�e�[�^�X
	EquipmentItem* e;	// ��
	bs.clear();
	bs.shrink_to_fit();
	bs.resize(itemnum); // �A�C�e���T�C�Y�ɍ��킹��
	for (int i = 0;i < itemnum;++i) {

		switch (item[i]->type) {

		case ItemType::IMPORTANT:
			// �d�v�A�C�e��
			// �퓬�X�e�[�^�X�Ȃ�
			break;

		case ItemType::RECOVERY:
			// �񕜃A�C�e��
			// �퓬�X�e�[�^�X�Ȃ�
			break;

		case ItemType::DAMAGE:
			// �U���A�C�e��
			// �퓬�X�e�[�^�X�Ȃ�
			break;

		case ItemType::EQUIPMENT:
			// �����A�C�e��

			e = new EquipmentItem(item[i]->GetMyData()[0]);
			bs[i] = e->GetCorrection();
			delete e;

			break;

		case ItemType::LIFESTEAL:
			// ���C�t�X�e�B�[���A�C�e��
			// �퓬�X�e�[�^�X�Ȃ�
			break;

		default:
			break;
		}
	}

	// debug
	forSale.itemName.clear();
	forSale.itemName.shrink_to_fit();
	for (int i = 0, n = item.size(); i < n; ++i) {
		forSale.itemName.push_back(item[i]->GetName());
	}
	forSale.isDraw		= true;
	forSale.isSelect	= true;

	InitPlayerBattleStatus();
}
void Shop::InitPlayerBattleStatus() {
	// �����̃X�e�[�^�X��ۑ�����

	EquipmentItem* e;

	player_bs.resize(Player::character.size());	// ���T�C�Y
	for (int i = 0, n = player_bs.size();i < n;++i) {
		player_bs[i].resize(3);	// ���T�C�Y
		for (int j = 0;j < 3;++j) {
			e = new EquipmentItem(0);
			player_bs[i][j] = e->GetCorrection();
			delete e;
		}
	}

	for (int i = 0, n = player_bs.size();i < n;++i) {
		// �L�����N�^�[�̐��������[�v

		for (int j = 0, m = Player::bag[i].size();j < m;++j) {
			// �A�C�e���̐��������[�v

			if (Player::bag[i][j]->isEquipment) {
				// �������Ă�����

				e = new EquipmentItem(Player::bag[i][j]->GetMyData()[0]);
				player_bs[i][(int)Player::bag[i][j]->equipmentType - 1] = e->GetCorrection();
				delete e;
			}
		}
	}
}

void Shop::Process() {
	if (forSale.isSelect) {
		// ���i��I��ł���Ƃ�
		forSale.Process();

		if ((	Input::Get(VK_Z)				== 1	|| Input::Get(VK_V)					== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1	|| Input::Get(0, XINPUT_GAMEPAD_Y)	== 1)) {
			// ����
			if (Player::Player::GetMoney() >= item[forSale.icon.Y]->GetPrice()) {
				// ����������Ă���
				forSale.isSelect	= false;
				decideMenu.isDraw	= true;
				decideMenu.isSelect = true;
			}
			else {
				// ����������Ă��Ȃ�
				TextBox::AddLine("����������Ȃ���");
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �X���o��
			forSale.isSelect	= false;
			forSale.isDraw		= false;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
			Player::flag_shop	= false;
		}
	}
	else if(decideMenu.isSelect) {
		// ���i��I�񂾂���
		decideMenu.Process();

		if (	Input::Get(VK_Z)				== 1 || Input::Get(VK_V)				== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����
			if (decideMenu.icon.Y == 0) {
				charaMenu.isDraw	= true;
				charaMenu.isSelect	= true;
				decideMenu.icon.Y	= 0;
				decideMenu.isSelect = false;
			}
			else {
				// ������
				forSale.isSelect	= true;
				decideMenu.icon.Y	= 0;
				decideMenu.isSelect = false;
				decideMenu.isDraw	= false;
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �L�����Z��
			forSale.isSelect	= true;
			decideMenu.icon.Y	= 0;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}
	}
	else if (charaMenu.isSelect) {
		// �L�����N�^�[���j���[
		charaMenu.Process();

		if (	Input::Get(VK_Z)				== 1 || Input::Get(VK_V)				== 1
			||	Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����
			if (Player::InsertItem(charaMenu.icon.Y, item[forSale.icon.Y]->GetID())) {
				// ����������
				// �܂ɃA�C�e��������
				Player::PayMoney(item[forSale.icon.Y]->GetPrice());		// ���������炷
				TextBox::AddLine("�܂���");								// �Z���t
			}
			else {
				// ���������s(�A�C�e�������ĂȂ�)
				TextBox::AddLine("����ȏ㎝�ĂȂ�");
			}

			forSale.isSelect	= true;
			decideMenu.icon.Y	= 0;
			charaMenu.isSelect	= false;
			charaMenu.isDraw	= false;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �L�����Z��
			forSale.isSelect	= true;

			charaMenu.isSelect	= false;
			charaMenu.isDraw	= false;
			decideMenu.icon.Y	= 0;
			decideMenu.isSelect = false;
			decideMenu.isDraw	= false;
		}
	}
}
void Shop::Draw() {
	// ������
	DrawBox(1000, 0, 300, 64, true);	// ��
	DrawString(WHITE, 1024, 16, "%d", Player::GetMoney());

	// ��̔�
	forSale.Draw();					
	for (int i = 0, n = item.size(); i < n; ++i) {
		// �l�i
		DrawString(WHITE, 512 - (32 * 6), 32 + i * 32, "%d", item[i]->GetPrice());
	}

	// ������Ə�̔�
	DrawBox(0, 256, 1024, 256, true);	// �w�i��
	DrawString(WHITE, 16, 256 + 16, item[forSale.icon.Y]->GetInfo().c_str());	// ������

	// �A�C�e���̃X�e�[�^�X(�E��)
	DrawBox(512, 0, 512, 256, true);
	for (int i = 0, n = item.size(); i < n; ++i) {
		// �A�C�e���̎�ނɂ���ĕ\����ω�
		switch (item[forSale.icon.Y]->type) {
		case ItemType::IMPORTANT:
			// �d�v�A�C�e��
			// ���ꂪ�X�ɔ����Ă�^�C�~���O����H
			break;

		case ItemType::RECOVERY:
			// �񕜃A�C�e��

			DrawString(WHITE, 512 + 16, 0 + 16, ((item[forSale.icon.Y]->GetMyData()[1] == 0 ? "" : "�����S�̂� ") + to_string(item[forSale.icon.Y]->GetMyData()[0]) + "�񕜂���").c_str());
			break;

		case ItemType::DAMAGE:
			// �U���A�C�e��

			// ������

		case ItemType::EQUIPMENT:
			// �����A�C�e��

			DrawString(WHITE, 512 + 32, 0 + 16, ("HP : " + to_string(bs[forSale.icon.Y].maxHP)).c_str());
			DrawString(WHITE, 512 + 32, 0 + 48, ("MP : " + to_string(bs[forSale.icon.Y].maxMP)).c_str());
			DrawString(WHITE, 512 + 16, 0 + 80, ("ATK : " + to_string(bs[forSale.icon.Y].STR)).c_str());
			DrawString(WHITE, 512 + 16, 0 +112, ("DEX : " + to_string(bs[forSale.icon.Y].DEX)).c_str());
			DrawString(WHITE, 512 + 16, 0 +144, "AG");DrawString(WHITE, 512 + 64, 0 + 144, ("I : " + to_string(bs[forSale.icon.Y].AGI)).c_str());
			DrawString(WHITE, 512 + 16, 0 +176, ("LUC : " + to_string(bs[forSale.icon.Y].LUC)).c_str());

			DrawString(WHITE, 768 + 16, 0 + 16, ("���h : " + to_string(bs[forSale.icon.Y].DEF_PHYS)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 48, ("�ϔM : " + to_string(bs[forSale.icon.Y].DEF_HEAT)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 80, ("�ϊ� : " + to_string(bs[forSale.icon.Y].DEF_COLD)).c_str());
			DrawString(WHITE, 768 + 16, 0 + 112,("�ϓd : " + to_string(bs[forSale.icon.Y].DEF_ELEC)).c_str());

			break;

		case ItemType::LIFESTEAL:
			// ���C�t�X�e�B�[���A�C�e��

			// ������

			break;

		default:
			// �G���[
			break;
		}
	}

	// ������Ɖ��̔�
	for (int i = 0, n = player_bs.size();i < n;++i) {
		// �v���C���[�̐��������[�v

		DrawBox(512 * i, 512, 512, 256, true);
	}

	for (int i = 0, n = item.size(); i < n; ++i) {
		// �A�C�e���̎�ނɂ���ĕ\����ω�
		switch (item[forSale.icon.Y]->type) {
		case ItemType::IMPORTANT:
			// �d�v�A�C�e��
			// ���ꂪ�X�ɔ����Ă�^�C�~���O����H
			break;

		case ItemType::RECOVERY:
			// �񕜃A�C�e��

			DrawString(WHITE, 0 + 16, 512 + 16, ((item[forSale.icon.Y]->GetMyData()[1] == 0 ? "" : "�����S�̂� ") + to_string(item[forSale.icon.Y]->GetMyData()[0]) + "�񕜂���").c_str());
			break;

		case ItemType::DAMAGE:
			// �U���A�C�e��

			// ������

		case ItemType::EQUIPMENT:
			// �����A�C�e��

			for (int i = 0, n = player_bs.size();i < n;++i) {
				// �v���C���[�̐��������[�v

				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxHP < bs[forSale.icon.Y].maxHP ? GREEN : WHITE, (512 * i) + 32, 512 + 16, ("HP : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxHP) + " �� " + to_string(bs[forSale.icon.Y].maxHP)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxMP < bs[forSale.icon.Y].maxMP ? GREEN : WHITE, (512 * i) + 32, 512 + 48, ("MP : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].maxMP) + " �� " + to_string(bs[forSale.icon.Y].maxMP)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].STR < bs[forSale.icon.Y].STR ? GREEN : WHITE, (512 * i) + 16, 512 + 80, ("ATK : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].STR) + " �� " + to_string(bs[forSale.icon.Y].STR)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEX < bs[forSale.icon.Y].DEX ? GREEN : WHITE, (512 * i) + 16, 512 + 112,("DEX : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEX) + " �� " + to_string(bs[forSale.icon.Y].DEX)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI < bs[forSale.icon.Y].AGI ? GREEN : WHITE, (512 * i) + 16, 512 + 144, "AG");DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI < bs[forSale.icon.Y].AGI ? GREEN : WHITE, (512 * i) + 64, 512 + 144, ("I : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].AGI) + " �� " + to_string(bs[forSale.icon.Y].AGI)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].LUC < bs[forSale.icon.Y].LUC ? GREEN : WHITE, (512 * i) + 16, 512 + 176,("LUC : "+ to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].LUC) + " �� " + to_string(bs[forSale.icon.Y].LUC)).c_str());

				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_PHYS < bs[forSale.icon.Y].DEF_PHYS ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 16, ("���h : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_PHYS) + " �� " + to_string(bs[forSale.icon.Y].DEF_PHYS)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_HEAT < bs[forSale.icon.Y].DEF_HEAT ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 48, ("�ϔM : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_HEAT) + " �� " + to_string(bs[forSale.icon.Y].DEF_HEAT)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_COLD < bs[forSale.icon.Y].DEF_COLD ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 80, ("�ϊ� : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_COLD) + " �� " + to_string(bs[forSale.icon.Y].DEF_COLD)).c_str());
				DrawString(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_ELEC < bs[forSale.icon.Y].DEF_ELEC ? GREEN : WHITE, (512 * i) + 256 + 16, 512 + 112,("�ϓd : " + to_string(player_bs[i][(int)item[forSale.icon.Y]->equipmentType - 1].DEF_ELEC) + " �� " + to_string(bs[forSale.icon.Y].DEF_ELEC)).c_str());
			}
			break;

		case ItemType::LIFESTEAL:
			// ���C�t�X�e�B�[���A�C�e��

			// ������

			break;

		default:
			// �G���[
			break;
		}
	}
	
	// ���̔�
	DrawBox(0, 1080 - 256, 1024, 256, true);	// �w�i��
	if (forSale.isSelect) {
		DrawString(WHITE, 16, 1080 - 256 + 16, greeting.c_str());	// ������
	}
	else if (decideMenu.isSelect) {
		DrawString(WHITE, 16, 1080 - 256 + 16, confirmation.c_str());	// ������
	}
	else {
		DrawString(WHITE, 16, 1080 - 256 + 16, who.c_str());	// ������
	}

	// �E�̔�
	if (decideMenu.isDraw) {
		decideMenu.Draw();
	}

	if (charaMenu.isDraw) {
		charaMenu.Draw();
	}

	if (forSale.isSelect) {
		DrawString(WHITE, 0, 32 + forSale.icon.Y * 32 , "��");
	}
	else if (decideMenu.isSelect) {
		DrawString(WHITE, 1024, 32 + 96 + decideMenu.icon.Y * 32, "��");
	}
	else if (charaMenu.isSelect) {
		DrawString(WHITE, 1344, 32 + 96 + charaMenu.icon.Y * 32, "��");
	}
}