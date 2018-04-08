#include "Player.h"
#include "SkillTree.h"
#include "MapObject.h"
#include "Game.h"

bool					Player::saveFlag	= false;// �Z�[�u�t���O�i�Z�[�u�����t���[���ɂ̂�true�ɂȂ�j
bool					Player::born		= false;// �����͈��̂�
int						Player::playerGraph;		// �{�̂̉摜
int						Player::animCount;			// �A�j���[�V�����̔���
int						Player::animSpeed;			// �A�j���[�V�����̑��x�B�������ق�������
Position2D				Player::pos_;				// ���W
int						Player::speed;				// �ړ��X�s�[�h���Q�̙p��ɂ��邱��
Position2D				Player::pre_pos_;			// ���O���W
int						Player::chapter;			// �X�g�[���[�̐i�s�x
int						Player::stage;				// ������X�e�[�W
Direction				Player::direction;			// ����
TalkFlag				Player::talkFlag;			// �b�����������W
bool					Player::animation;			// �A�j���[�V���������ǂ���
Mode					Player::mode;				// ���j���[���J���Ă��邩�ǂ���
int						Player::swapCharacterY;		// �Ȃ�ׂ���
bool					Player::flag_shop;			// �����������ǂ���
vector<PlayerChara*>	Player::character;			// �p�[�e�B�[�����o�[
vector<vector<Item*> >	Player::bag;				// �A�C�e���o�b�O
vector<vector<bool> >	Player::isFinish;			// �C�x���g�t���O
int						Player::money;				// ��
ItemBox					Player::mainMenu;			// ���C�����j���[�{�b�N�X
ItemBox					Player::charaMenu;			// �L�����N�^�[���j���[�{�b�N�X
ItemBox					Player::itemMenu;			// �A�C�e�����j���[�{�b�N�X
ItemBox					Player::doMenu;				// �c�����j���[
ItemBox					Player::targetMenu;			// �^�[�Q�b�g���j���[
ItemBox					Player::decideMenu;			// �m�F���j���[
int						Player::menuY;				// �厖�ȍ��W
int						Player::cover_handle_;		// �������ȍ�

// �ꎞ�Z�[�u�f�[�^
map<int, map<int, int>> Player::tmpSave_unitMode;	// ���j�b�g�̃��[�h	<�}�b�vID <���j�b�gID , ���[�h>>

// debug
int						Player::emote_lock_;		// �G���[�g�A���֎~

void Player::InitPlayer(bool isContinue) {
	if (!character.empty()) {
		DeletePlayer();
	}

	// �����`�F�b�N
	born = true;

	// �摜�ƃA�j���[�V����
	if (playerGraph == 0){
		playerGraph = LoadGraph("img\\player\\kata.png", 4, 4);		// �{��
	}
	animCount = 0;	// �A�j���[�V�����̔���
	animSpeed = 10;	// �A�j���[�V�����̑��x�B�������ق�������

	// ��������
	if (isContinue) {
		// -------------------------------------------------------
		// �t�@�C���I�[�v��
		ifstream ifs("savedata\\savedata.txt");
		// �Z�[�u�f�[�^���Ȃ��Ƃ�
		if (!ifs) {
			chapter = 0;
			stage = 0;
			pos_.set_pos(CHIPSIZE * 25, CHIPSIZE * 50);	// �J�n���̍��W

			// debug
			// �p�[�e�B�[
			for (int i = 0, n = character.size(); i < n; ++i) {
				for (int j = 0, m = bag[i].size(); j < m; ++j) {
					if (bag[i][j] != NULL) {
						delete bag[i][j];
					}
				}
				bag[i].clear();
				bag[i].shrink_to_fit();
			}
			bag.clear();
			bag.shrink_to_fit();
			for (int i = 0, n = character.size(); i < n; ++i) {
				delete character[i];
			}
			character.clear();
			character.shrink_to_fit();
			int back = 0;
			character.push_back(new Kata("kata", false));
			bag.resize(character.size());
			// �A�C�e������������
			Player::InsertItem(back, 11);	// �{���{���̌�
			Player::InsertItem(back, 12);	// ���񂾃}���g
			// �����𑕔�
			for (int i = 0;i < 2;++i) {
				Player::bag[back][i]->Effect(Player::character[back]);	// ����
				Player::bag[back][i]->isEquipment = true;
			}
			for (int i = 0, n = (int)character.size(); i < n; ++i) {
				character[i]->Lv1_Init();		// Lv.1��Ԃւ̏�����
			}
			money = 10000;

			// debug
			InsertItem(0, 0);
			isFinish.resize(10);
			for (int i = 0;i < 10;++i) {
				isFinish[i].resize(100);
			}
			for (int i = 0;i < 10;++i) {
				for (int j = 0;j < 100;++j) {
					isFinish[i][j] = false;
				}
			}
		}
		// �Z�[�u�f�[�^������Ƃ�
		else {
			// ����n
			int loop = 0;	// ���[�v��
			string line;		// ��i�[�p

			// ��ɂ���ď����𕪂���
			while (getline(ifs, line)) {
				istringstream stream(line);
				string token;
				vector<string> vStr;
				for (int i = 0;getline(stream, token, ','); i++) {
					vStr.push_back(token);
				}

				// �X�e�[�^�X
				if (loop == 0) {
					chapter = boost::lexical_cast<int>(vStr[0]);
					stage = boost::lexical_cast<int>(vStr[1]);
					pos_.set_pos(boost::lexical_cast<int>(vStr[2]), boost::lexical_cast<int>(vStr[3]));	// �Z�[�u���ꂽ���W
				}
				// �L�����N�^�[
				else if (loop == 1) {
					for (int i = 0, n = vStr.size();i < n;i++) {
						// ���O�ŌĂԃL�����N�^�[�𕪂���
						if (vStr[i] == "kata") {
							// �P�[�^
							character.push_back(new Kata("kata", isContinue));
						}
						else if (vStr[i] == "tee") {
							// �e�B�[
							character.push_back(new Tee("tee", isContinue));
						}

						// ���ʏ���
						character[i]->order = i;		// ����
					}

					bag.resize(character.size());
				}
				// �A�C�e��
				else if (loop == 2) {
					for (int i = 0, n = vStr.size();i < n;++i) {
						InsertItem((int)(boost::lexical_cast<int>(vStr[i]) / 10000), (int)((boost::lexical_cast<int>(vStr[i]) % 10000)));
						++i;
						if ((int)(boost::lexical_cast<int>(vStr[i])) == 1) {
							// �������Ă���
							bag[(int)(boost::lexical_cast<int>(vStr[i - 1]) / 10000)].back()->isEquipment = true;
						}
						else {
							// �������Ă��Ȃ�
							bag[(int)(boost::lexical_cast<int>(vStr[i - 1]) / 10000)].back()->isEquipment = false;
						}
					}
				}
				// �t���O
				else if (loop == 3) {
					isFinish.resize(10);
					for (int j = 0;j < 1000;++j) {
						isFinish[(int)(j/100)].push_back(vStr[j] == "1" ? true : false);
					}
				}
				// ������
				else if (loop == 4) {
					money = boost::lexical_cast<int>(vStr[0]);
				}
				// ���j�b�g�̃��[�h�f�[�^
				else if (loop >= 5) {
					Player::LoadTmpSave_UnitMode(line);
				}

				// �J�E���g�A�b�v
				loop++;
			}
		}

		ifs.close();
		// �t�@�C���N���[�Y
		// -----------------------------------------------------
	}
	// ���߂���
	else {
		chapter = 0;
		stage = 0;
		pos_.set_pos(CHIPSIZE * 25, CHIPSIZE * 50);	// �J�n���̍��W

		// debug
		// �p�[�e�B�[
		for (int i = 0, n = character.size(); i < n; ++i) {
			for (int j = 0, m = bag[i].size(); j < m; ++j) {
				if (bag[i][j] != NULL) {
					delete bag[i][j];
				}
			}
			bag[i].clear();
			bag[i].shrink_to_fit();
		}
		bag.clear();
		bag.shrink_to_fit();
		for (int i = 0, n = character.size(); i < n; ++i) {
			delete character[i];
		}
		character.clear();
		character.shrink_to_fit();
		int back = 0;
		character.push_back(new Kata("kata", false));
		bag.resize(character.size());
		// �A�C�e������������
		Player::InsertItem(back, 11);	// �{���{���̌�
		Player::InsertItem(back, 12);	// ���񂾃}���g
		// �����𑕔�
		for (int i = 0;i < 2;++i) {
			Player::bag[back][i]->Effect(Player::character[back]);	// ����
			Player::bag[back][i]->isEquipment = true;
		}
		for (int i = 0, n = (int)character.size(); i < n; ++i) {
			character[i]->Lv1_Init();		// Lv.1��Ԃւ̏�����
		}
		money = 10000;

		// debug
		InsertItem(0, 0);
		isFinish.resize(10);
		for (int i = 0;i < 10;++i) {
			isFinish[i].resize(100);
		}
		for (int i = 0;i < 10;++i) {
			for (int j = 0;j < 100;++j) {
				isFinish[i][j] = false;
			}
		}
	}

	// ����n
	direction			= DOWN;		// ��ԍŏ��̌���
	pre_pos_ = pos_;				// ���O���W
	animation			= false;	// ���͂��󂯕t���邩�ǂ���
	mode				= FREE;		// �v���C���[�̑I�����
	swapCharacterY		= -1;		// �Ȃ�ׂ���
	
	// �X�e�[�^�X
	speed = 8;	// �ړ��̑���(�A�j���[�V����)

	flag_shop = false;	// �V���b�v��ʂ��ǂ���

	mainMenu.itemName.clear();
	mainMenu.itemName.shrink_to_fit();
	charaMenu.itemName.clear();
	charaMenu.itemName.shrink_to_fit();
	itemMenu.itemName.clear();
	itemMenu.itemName.shrink_to_fit();
	doMenu.itemName.clear();
	doMenu.itemName.shrink_to_fit();
	targetMenu.itemName.clear();
	targetMenu.itemName.shrink_to_fit();
	decideMenu.itemName.clear();
	decideMenu.itemName.shrink_to_fit();

	mainMenu.itemName.resize(9);
	mainMenu.itemName[0] = "�A�C�e��";
	mainMenu.itemName[1] = "�X�L��";
	mainMenu.itemName[2] = "�X�L���c���[";
	mainMenu.itemName[3] = "�Ȃ�т���";
	mainMenu.itemName[4] = "�X�e�[�^�X";
	mainMenu.itemName[5] = "�Z�[�u";
	mainMenu.itemName[6] = "�^�C�g���ɂ��ǂ�";
	mainMenu.itemName[7] = "�G���[�g";
	mainMenu.itemName[8] = "���ǂ�";
	mainMenu.Initialize(0, 0, 320, 64 + 9 * 32);
	for (int i = 0, n = character.size(); i < n; ++i) {
		// �L�����N�^�[�̐��������[�v
		charaMenu.itemName.push_back(character[i]->GetName());
	}
	charaMenu.Initialize(320, 0, 160, 64 + 9 * 32);
	itemMenu.Initialize(480, 0, 320, 64 + 9 * 32);
	doMenu.itemName.resize(5);
	doMenu.itemName[0] = "�g��";
	doMenu.itemName[1] = "�n��";
	doMenu.itemName[2] = "����";
	doMenu.itemName[3] = "�̂Ă�";
	doMenu.itemName[4] = "�߂�";
	doMenu.Initialize(800, 0, 160, 64 + doMenu.itemName.size() * 32);
	for (int i = 0, n = character.size(); i < n; ++i) {
		// �L�����N�^�[�̐��������[�v
		targetMenu.itemName.push_back(character[i]->GetName());
	}
	targetMenu.Initialize(960, 0, 160, 64 + character.size() * 32);
	decideMenu.itemName.push_back("�͂�");
	decideMenu.itemName.push_back("������");
	decideMenu.Initialize(960, 0, 160, 128);
	menuY = 0;

	cover_handle_ = LoadGraph("img\\cover.png");	// �������ȍ�

	// debug
	emote_lock_ = 0;
}
void Player::DeletePlayer() {
	born = false;	// �I���������s����

	// ���������
	for (int i = 0, n = character.size();i < n;i++) {
		delete character[i];
	}
	character.clear();
	character.shrink_to_fit();
	
	// �A�C�e���폜
	for (unsigned int i = 0, n = bag.size();i < n;++i) {
		for (unsigned int j = 0;j < bag[i].size();++j) {
			delete bag[i][j];
		}
		bag[i].clear();
		bag[i].shrink_to_fit();
	}
	bag.clear();
	bag.shrink_to_fit();

	DeleteGraph(cover_handle_);	// �������ȍ�
}

bool Player::Process(){
	// ���O���W��ۑ�
	pre_pos_ = pos_;

	// �Z�[�u�t���O��؂�
	saveFlag = false;

	// ��b�t���O��؂�
	if (talkFlag.flag) {
		talkFlag.flag = false;
	}

	// ���͂��󂯕t���Ă����Ԃ̏ꍇ
	if (!animation){
		// �ʏ���
		switch (mode) {
		case FREE:
			/////////////////////////////////////////////////
			// �ړ�
			/////////////////////////////////////////////////
			if (Input::Get(VK_UP) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 0) {
				// ������ς��Ĉړ����[�V����(���͂��󂯕t���Ȃ����)��
				direction = UP;
				animation = true;
			}
			else if (Input::Get(VK_DOWN) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 0) {
				direction = DOWN;
				animation = true;
			}
			else if (Input::Get(VK_LEFT) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 0) {
				direction = LEFT;
				animation = true;
			}
			else if (Input::Get(VK_RIGHT) > 0 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 0) {
				direction = RIGHT;
				animation = true;
			}
			////////////////////////////////////////////////
			// ���j���[���J��
			/////////////////////////////////////////////////
			if (Input::Get(VK_C) == 1 || Input::Get(0, XINPUT_GAMEPAD_X) == 1) {
				mode		= MENU;		// ���j���[��ʂ�
				// debug
				mainMenu.icon.Y		= 0;
				mainMenu.isSelect	= true;
				mainMenu.isDraw		= true;
				charaMenu.isSelect	= false;
				charaMenu.isDraw	= false;
				itemMenu.isSelect	= false;
				itemMenu.isDraw		= false;
				doMenu.isSelect		= false;
				doMenu.isDraw		= false;
				targetMenu.isSelect = false;
				targetMenu.isDraw	= false;
			}
			////////////////////////////////////////////////
			// �V���[�g�J�b�g�i�b���j
			/////////////////////////////////////////////////
			if (Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
				talkFlag.flag = true;
				switch (direction)
				{
				case UP:
					talkFlag.pos_.set_pos(pos_.X, pos_.Y - 64);
					break;
				case DOWN:
					talkFlag.pos_.set_pos(pos_.X, pos_.Y + 64);
					break;
				case LEFT:
					talkFlag.pos_.set_pos(pos_.X - 64, pos_.Y);
					break;
				case RIGHT:
					talkFlag.pos_.set_pos(pos_.X + 64, pos_.Y);
					break;
				default:
					break;
				}
			}
			////////////////////////////////////////////////
			// �V���[�g�J�b�g�i�G���[�g�j
			////////////////////////////////////////////////
			if (emote_lock_ <= 0) {
				if (Input::Get(VK_E) == 1) {
					Emote::SetEmote(GetRand(9), WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 96);	// �G���[�g�Z�b�g

					emote_lock_ = 30;	// �A���֎~
				}
			}
			else {
				emote_lock_--;
			}
			break;

		case MENU:
			MenuProcess();	// ���j���[��ʂ̃v���Z�X
			break;

		default:
			// �G���[
			break;
		}
	}
	////////////////////////////////////////////////
	// �A�j���[�V����
	/////////////////////////////////////////////////
	else {
		// ������Ƃ��ړ�
		switch (direction)
		{
		case UP:
			pos_.Y -= speed;
			break;
		case DOWN:
			pos_.Y += speed;
			break;
		case LEFT:
			pos_.X -= speed;
			break;
		case RIGHT:
			pos_.X += speed;
			break;
		default:
			break;
		}

		// �}�X�ڂ҂�����ňړ����[�V�����I��
		if (Player::pos_.X%CHIPSIZE == 0 && Player::pos_.Y%CHIPSIZE == 0) {
			animation = false;
			return true;	//���W���ړ�������true��Ԃ��i�G���J�E���g�����p�j
		}
	}
	return false;
}
void Player::MenuProcess() {
	// ���j���[��ʂ̏���
	
	switch (menuY) {
	case START:
		Process_Start();
		break;

	case ITEM:
		Process_Item();
		break;

	case SKILL:
		Process_Skill();
		break;

	case SKILLTREE:
		Process_SkillTree();
		break;

	case REARRANGE:
		Process_Rearrange();
		break;

	case STATUS:
		Process_Status();
		break;

	case SAVE:
		Process_Save();
		break;

	case BACK_TITLE:
		Process_Back_Title();
		break;

	case EMOTE:
		Process_Emote();
		break;

	case BACK:
		Process_Back();
		break;

	default:
		// �G���[
		break;
	}

	// ���j���[�I��
	if (Input::Get(VK_C) == 1 || Input::Get(0, XINPUT_GAMEPAD_X) == 1) {
		// ������
		mode				= FREE;

		itemMenu.itemName.clear();
		itemMenu.itemName.shrink_to_fit();

		// debug
		menuY				= START;
		mainMenu.isSelect	= false;
		mainMenu.isDraw		= false;
		charaMenu.isSelect	= false;
		charaMenu.isDraw	= false;
		itemMenu.isSelect	= false;
		itemMenu.isDraw		= false;
		doMenu.isSelect		= false;
		doMenu.isDraw		= false;
		targetMenu.isSelect = false;
		targetMenu.isDraw	= false;
		decideMenu.isSelect = false;
		decideMenu.isDraw	= false;
	}
}

void Player::Process_Start() {
	if (mainMenu.isSelect) {
		// ���C�����j���[�̏���
		mainMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����

			switch (mainMenu.icon.Y + 1) {
				// ����I��������

			case START:
				// �X�^�[�g���j���[
				// �G���[
				break;

			case ITEM:
				// �A�C�e�����j���[
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				itemMenu.isDraw = true;
				itemMenu.isSelect = true;
				menuY = ITEM;
				break;

			case SKILL:
				// �X�L�����j���[
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				itemMenu.isDraw = true;
				itemMenu.isSelect = true;
				menuY = SKILL;
				break;

			case SKILLTREE:
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				menuY = SKILLTREE;
				break;

			case REARRANGE:
				// ���בւ����j���[
				charaMenu.isDraw = true;
				charaMenu.isSelect = true;
				menuY = REARRANGE;	// �Ȃ�ׂ�����ʂ�
				break;

			case STATUS:
				// �X�e�[�^�X���j���[
				menuY = STATUS;		// �X�e�[�^�X��ʂ�
				break;

			case SAVE:
				// �Z�[�u���j���[
				menuY = SAVE;
				break;

			case BACK_TITLE:
				// �^�C�g���ɖ߂�
				mainMenu.isSelect	= false;
				decideMenu.icon.Y	= 1;
				decideMenu.isSelect = true;
				decideMenu.isDraw	= true;		// �m�F���b�Z�[�W
				TextBox::AddLine("�Z�[�u����Ă��Ȃ��i�s�͎����Ă��܂��܂�");	// �x��
				menuY = BACK_TITLE;
				break;

			case EMOTE:
				// �G���[�g
				mainMenu.isSelect = false;
				menuY = EMOTE;
				break;

			case BACK:
				// �߂�
				mainMenu.icon.Y = 0;
				mainMenu.isSelect = true;
				mainMenu.isDraw = true;
				charaMenu.isSelect = false;
				charaMenu.isDraw = false;
				itemMenu.isSelect = false;
				itemMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				mode = FREE;	// ���j���[��ʂ����
				break;

			default:
				// ��O
				// �G���[
				break;
			}

			mainMenu.isSelect = false;		// �����t��؂�
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			menuY = 0;
			mainMenu.isSelect = false;
			mainMenu.isDraw = false;
			mode = FREE;		// ���j���[��ʂ����
		}
	}
}
void Player::Process_Item() {
	if (itemMenu.isSelect) {
		// �A�C�e�����j���[�̏���
		if (charaMenu.isSelect) {
			// �L�����N�^�[���j���[�̏���
			charaMenu.Process();

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// ���ݑI��ł���o�b�O�̃A�C�e�������j���[��ʂɃR�s�[
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// ����

				if (Player::bag[charaMenu.icon.Y].empty()) {
					// �o�b�O����
					TextBox::AddLine("�A�C�e���������Ă��܂���E�E�E");
					return;
				}
				itemMenu.icon.Y = 0;
				charaMenu.isSelect = false;
			}

			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// �߂�
				itemMenu.itemName.clear();
				itemMenu.itemName.shrink_to_fit();
				mainMenu.isSelect	= true;
				charaMenu.isDraw	= false;
				charaMenu.isSelect	= false;
				itemMenu.isDraw		= false;
				itemMenu.isSelect	= false;
				menuY				= START;
			}
		}
		else {
			// �A�C�e�����j���[�̏���
			itemMenu.Process();

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// ����
				doMenu.isDraw = true;
				doMenu.isSelect = true;
				itemMenu.isSelect = false;
			}

			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// �߂�
				charaMenu.isSelect = true;
			}
		}
	}
	else if (doMenu.isSelect) {
		// �c�����j���[�̏���
		doMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����

			Item* selectedItem = bag[charaMenu.icon.Y][itemMenu.icon.Y];	// �G�C���A�X

			switch (doMenu.icon.Y) {
			case 0:
				// �g��

				if (selectedItem->type == ItemType::EQUIPMENT
					|| selectedItem->type == ItemType::IMPORTANT) {
					// �����������ꍇ
					TextBox::AddLine("���܂͂����Ȃ�");	// ���s���s���b�Z�[�W
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					itemMenu.isSelect = true;
					return;
				}

				// �g�p�\�A�C�e���̏���
				if (selectedItem->GetIsArea()) {
					// �S�̌���
					UseItem(selectedItem);	// ���ʔ���

					doMenu.isSelect = false;
					doMenu.isDraw = false;
					itemMenu.isSelect = true;

					if (bag[charaMenu.icon.Y].empty()) {
						// �A�C�e�����Ȃ��Ȃ���
						doMenu.icon.Y = 0;
						itemMenu.isSelect = false;
						itemMenu.isDraw = false;
					}
				}
				else {
					// �P�̌���
					targetMenu.isDraw = true;
					targetMenu.isSelect = true;
					doMenu.isSelect = false;
				}
				break;

			case 1:
				// �n��
				if (selectedItem->isEquipment) {
					// �������̃A�C�e���͓n���Ȃ�
					TextBox::AddLine("�������ł�");		// ���s���s���b�Z�[�W
					return;
				}
				doMenu.isSelect = false;
				targetMenu.isDraw = true;
				targetMenu.isSelect = true;
				break;

			case 2:
				// ����
				if (selectedItem->type != ItemType::EQUIPMENT) {
					// �����i�łȂ�
					TextBox::AddLine("�����ł��܂���");	// �I�񂾍s����ۑ�
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				for (int i = 0, n = bag[charaMenu.icon.Y].size(); i < n; ++i) {
					// �����^�C�v�̑��������ɕt���Ă��Ȃ����`�F�b�N����
					if (i == itemMenu.icon.Y)continue;	// �����̓X�L�b�v����

					if (selectedItem->equipmentType == bag[charaMenu.icon.Y][i]->equipmentType) {
						// �������ꍇ
						if (bag[charaMenu.icon.Y][i]->isEquipment == true) {
							// �������O��
							bag[charaMenu.icon.Y][i]->isEquipment = false;	// �������O��
							break;
						}
					}
				}
				selectedItem->Effect(character[charaMenu.icon.Y]);	// ����
				TextBox::AddLine("����");	// ���O�𗬂�
				itemMenu.isSelect = true;	// �A�C�e�����j���[�ɑ����߂�
				doMenu.isSelect = false;	// �t���O��܂�
				doMenu.isDraw = false;
				break;

			case 3:
				// �̂Ă�
				if (selectedItem->isEquipment) {
					// �������̃A�C�e���͎̂Ă��Ȃ�
					TextBox::AddLine("�������ł�");		// ���s���s���b�Z�[�W
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				if (selectedItem->type == ItemType::IMPORTANT) {
					// ������̂Ă�Ȃ�ĂƂ�ł��Ȃ��I
					TextBox::AddLine("������̂Ă�Ȃ�ĂƂ�ł��Ȃ��I");	// ���s���s���b�Z�[�W
					itemMenu.isSelect = true;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
					return;
				}

				decideMenu.icon.Y	= 1;		// ���������f�t�H���g��
				decideMenu.isDraw	= true;		// �Ō�̑I����ʂ�
				decideMenu.isSelect = true;
				doMenu.isSelect		= false;
				break;

			case 4:
				// �߂�
				itemMenu.isSelect = true;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
				break;

			default:
				// �G���[
				break;
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// ���ݑI��ł���o�b�O�̃A�C�e�������j���[��ʂɃR�s�[
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			itemMenu.isSelect = true;
			doMenu.isSelect = false;
			doMenu.isDraw = false;
		}
	}
	else if (targetMenu.isSelect) {
		// �^�[�Q�b�g���j���[�̏���
		targetMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����

			Item* selectedItem = bag[charaMenu.icon.Y][itemMenu.icon.Y];	// �G�C���A�X

			switch (doMenu.icon.Y) {
			case 0:
				// �g�p
				UseItem(selectedItem);	// ���ʔ���

				itemMenu.isSelect = true;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;

				if (bag[charaMenu.icon.Y].empty()) {
					// �A�C�e�����Ȃ��Ȃ���
					charaMenu.isSelect = true;
					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
				}
				break;

			case 1:
				// �n��

				if (InsertItem(targetMenu.icon.Y, selectedItem->GetID())) {
					// �n����
					TextBox::AddLine("�n����");

					bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);	// �z��̍Đ���

					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;

					if (bag[charaMenu.icon.Y].empty()) {
						// �A�C�e�����Ȃ��Ȃ���
						charaMenu.isSelect = true;
						itemMenu.isSelect = true;
						targetMenu.isSelect = false;
						targetMenu.isDraw = false;
						doMenu.isSelect = false;
						doMenu.isDraw = false;
					}
				}
				else {
					TextBox::AddLine("�������������ς��ł�");

					itemMenu.isSelect = true;
					targetMenu.isSelect = false;
					targetMenu.isDraw = false;
					doMenu.isSelect = false;
					doMenu.isDraw = false;
				}

				break;

			default:
				// �G���[
				break;
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// ���ݑI��ł���o�b�O�̃A�C�e�������j���[��ʂɃR�s�[
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
			doMenu.isSelect = false;
			doMenu.isDraw = false;
		}
	}
	else if (decideMenu.isSelect) {
		// �m�F���j���[�̏���
		decideMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����

			if (decideMenu.icon.Y == 0) {
				// �͂�
				delete bag[charaMenu.icon.Y][itemMenu.icon.Y];
				bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);
			}
			else {
				// ������
			}

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::bag[charaMenu.icon.Y].size(); i < n; ++i) {
				// ���ݑI��ł���o�b�O�̃A�C�e�������j���[��ʂɃR�s�[
				itemMenu.itemName.push_back(Player::bag[charaMenu.icon.Y][i]->GetName());
			}

			itemMenu.icon.Y = 0;
			itemMenu.isSelect = true;
			decideMenu.isDraw = false;
			decideMenu.isSelect = false;
			doMenu.isDraw = false;
			doMenu.isSelect = false;

			if (bag[charaMenu.icon.Y].empty()) {
				// �A�C�e�����Ȃ��Ȃ���
				charaMenu.isSelect = true;
				itemMenu.isSelect = true;
				targetMenu.isSelect = false;
				targetMenu.isDraw = false;
				doMenu.isSelect = false;
				doMenu.isDraw = false;
			}
		}

		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			decideMenu.isDraw	= false;		// �Ō�̑I����ʂ�
			decideMenu.isSelect = false;
			doMenu.isSelect		= true;
		}
	}
}
void Player::Process_Skill() {
	if (itemMenu.isSelect) {
		// �L�����N�^�[��I�ԑO�̏���

		if (charaMenu.isSelect) {
			// �L�����I��
			charaMenu.Process();

			itemMenu.itemName.clear();
			itemMenu.itemName.shrink_to_fit();
			for (int i = 0, n = Player::character[charaMenu.icon.Y]->GetSkillList().size(); i < n; ++i) {
				// ���ݑI��ł���o�b�O�̃A�C�e�������j���[��ʂɃR�s�[
				if (Player::character[charaMenu.icon.Y]->GetSkillList()[i]->fieldFlag) {
					itemMenu.itemName.push_back(Player::character[charaMenu.icon.Y]->GetSkillList()[i]->name);
				}
			}

			if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
				// ����
				if (itemMenu.itemName.empty()) {
					// �X�L���������Ă��Ȃ�
					TextBox::AddLine("�X�L�����������Ă��܂���c");	// �x��
					return;
				}
				else {
					// �����Ă���
					itemMenu.icon.Y = 0;
					itemMenu.isDraw = true;
					itemMenu.isSelect	= true;
					charaMenu.isSelect	= false;
				}
			}
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// �߂�
				menuY = START;
				mainMenu.isSelect = true;
				charaMenu.isDraw = false;
				charaMenu.isSelect = false;
				itemMenu.isDraw = false;
				itemMenu.isSelect = false;
			}
		}
		else {
			itemMenu.Process();

			if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
				|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
				// �I�񂾃A�C�e�����W��ۑ�
				targetMenu.isDraw = true;
				targetMenu.isSelect = true;
				itemMenu.isSelect = false;
			}
			if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
				// �߂�
				charaMenu.isSelect = true;
				itemMenu.isDraw = false;
			}
		}
	}
	else if (targetMenu.isSelect) {
		// �L�����N�^�[��I�񂾌�̏���
		targetMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����
			for (int i = 0, n = Player::character[charaMenu.icon.Y]->GetSkillList().size(); i < n; ++i) {
				// ���O�̊��S��v������
				if (Player::character[charaMenu.icon.Y]->GetSkillList()[i]->name == itemMenu.itemName[itemMenu.icon.Y]) {
					// ���O����v�����X�L���̌��ʂ����ׂĔ���������
					//for (int j = 0, m = character[charaMenu.icon.Y]->GetSkillList()[i]->effect.size(); j < m; ++j) {
					//	character[charaMenu.icon.Y]->GetSkillList()[i]->effect[j]->Execution_Field(character[charaMenu.icon.Y], character[targetMenu.icon.Y]);
					//}
					character[charaMenu.icon.Y]->GetSkillList()[i]->Execution_Field(character[charaMenu.icon.Y], character, character[targetMenu.icon.Y]);

					break;
				}
			}

			vector<string> a;
			character[targetMenu.icon.Y]->GetEffectText(a);
			if (a.empty()) {
				// ���b�Z�[�W���Ȃ�
			}
			else {
				// ���b�Z�[�W�L��
				TextBox::AddLine(a);
			}
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
		}
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			itemMenu.isSelect = true;
			targetMenu.isSelect = false;
			targetMenu.isDraw = false;
		}
	}
}
void Player::Process_SkillTree() {

	// �L�����I��
	charaMenu.Process();

	if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
		|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
		// ����
		SkillTree::SetData(Player::character[charaMenu.icon.Y]);
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
	}
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		// �߂�
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
	}

}
void Player::Process_Rearrange() {
	// ���בւ�
	if (charaMenu.isSelect) {
		charaMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����
			if (swapCharacterY == -1) {
				// �ړ���
				swapCharacterY = charaMenu.icon.Y;	// ���W��ۑ�
			}
			else {
				// �ړ���
				PlayerChara* pc = character[swapCharacterY];				// �ꎞ���	
				character[swapCharacterY] = character[charaMenu.icon.Y];	// �ړ�����ړ�
				character[charaMenu.icon.Y] = pc;							// �ړ������ړ�
				bag[swapCharacterY].swap(bag[charaMenu.icon.Y]);			// �ו�������ւ�
																			// ���я�
				for (int i = 0, n = character.size(); i < n; i++) {
					character[i]->order = i;								// ���я����X�V
				}
				swapCharacterY = -1;										// ������

				charaMenu.itemName.clear();
				charaMenu.itemName.shrink_to_fit();
				for (int i = 0, n = character.size(); i < n; ++i) {
					// �L�����N�^�[�̐��������[�v
					charaMenu.itemName.push_back(character[i]->GetName());
				}
				targetMenu.itemName.clear();
				targetMenu.itemName.shrink_to_fit();
				for (int i = 0, n = character.size(); i < n; ++i) {
					// �L�����N�^�[�̐��������[�v
					targetMenu.itemName.push_back(character[i]->GetName());
				}
				targetMenu.Initialize(960, 0, 160, 64 + character.size() * 32);
			}
		}
		if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �߂�
			menuY = START;
			mainMenu.isSelect = true;
			charaMenu.isDraw = false;
			charaMenu.isSelect = false;
			itemMenu.isDraw = false;
			itemMenu.isSelect = false;
		}
	}
}
void Player::Process_Status() {
	// �X�e�[�^�X�\��
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		// �߂�
		menuY = START;
		mainMenu.isSelect = true;
		charaMenu.isDraw = false;
		charaMenu.isSelect = false;
		itemMenu.isDraw = false;
		itemMenu.isSelect = false;
	}
}
void Player::Process_Save() {
	Save();
	TextBox::AddLine("�Z�[�u���܂����B");
	mainMenu.isSelect = true;
	menuY = START;
}
void Player::Process_Back_Title() {
	// �^�C�g���ɖ߂�

	if (decideMenu.isSelect) {
		// �{���ɂ��ǂ邩�̊m�F
		
		decideMenu.Process();

		if (Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1
			|| Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) {
			// ����L�[����

			if (decideMenu.icon.Y == 0) {
				// �͂�

				Scene::nowScene = GameScene::TITLE;
			}
			else {
				// ������

				decideMenu.Initialize(960, 0, 160, 128);	// ������
				mainMenu.isSelect = true;
				menuY = 0;
			}
		}
		else if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
			// �L�����Z��

			if (decideMenu.icon.Y == 0) {
				// �͂���I��ł���Ƃ�

				decideMenu.icon.Y = 1;	// �������Ɉړ�
			}
			else {
				// ��������I��ł���Ƃ�

				decideMenu.Initialize(960, 0, 160, 128);	// ������
				mainMenu.isSelect = true;
				menuY = 0;
			}
		}
	}
}
void Player::Process_Emote() {
	// �G���[�g

	Emote::SetEmote(GetRand(9), WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 96);	// �G���[�g�Z�b�g

	mainMenu.isSelect = false;
	mainMenu.isDraw = false;
	menuY = START;
	mode = FREE;
}
void Player::Process_Back() {

}

void Player::Draw(bool isFollow, int cameraX, int cameraY){
	// �A�j���[�V��������
	if (animCount < animSpeed * 4){
		animCount++;
	}
	else{
		animCount = 0;
	}

	// �v���C���[�̉摜
	if (isFollow) {
		DrawGraph(CHIPSIZE * 15, CHIPSIZE * 8, playerGraph, (int)(animCount / animSpeed) % 4, direction);
	}
	else {
		DrawGraph(CHIPSIZE * (pos_.X / 64) - cameraX, CHIPSIZE * (pos_.Y / 64) -cameraY, playerGraph, (int)(animCount / animSpeed) % 4, direction);
	}

	// ���j���[
	switch (mode) {
	case FREE:
		// �����Ȃ�
		break;

	case MENU:
		// ���C�����j���[

		// ���𔼓����ȍ��ŃJ�o�[
		DrawGraph(0, 0, cover_handle_);
		
		if (mainMenu.isDraw) {
			// ���C�����j���[�`��
			mainMenu.Draw();	// �`��
			DrawBox(320, 0, 160, 64, true);	// �����̔�
			DrawString(WHITE, 320 + 16, 16, to_string(money).c_str());
			DrawString(WHITE, 320 + 160 -32, 16, "G");
		}

		if (charaMenu.isDraw) {
			// �L�����N�^�[���j���[�`��
			charaMenu.Draw();
		}

		if (itemMenu.isDraw) {
			// �A�C�e�����j���[�`��
			itemMenu.Draw();
		}

		if (doMenu.isDraw) {
			// �ǂ����j���[�`��
			doMenu.Draw();
		}

		if (targetMenu.isDraw) {
			// �^�[�Q�b�g���j���[�`��
			targetMenu.Draw();
		}

		if (decideMenu.isDraw) {
			// �m�F���j���[�`��
			decideMenu.Draw();
		}

		if (mainMenu.isSelect) {
			// �����t��
			DrawString(WHITE, mainMenu.pos.X + 16, mainMenu.pos.Y + 32 + (mainMenu.icon.Y * 32), "��");
		}
		else if (charaMenu.isSelect) {
			// �����t��
			DrawString(WHITE, charaMenu.pos.X + 16, charaMenu.pos.Y + 32 + (charaMenu.icon.Y * 32), "��");
		}
		else if (itemMenu.isSelect) {
			// �����t��
			DrawString(WHITE, itemMenu.pos.X + 16, itemMenu.pos.Y + 32 + (itemMenu.icon.Y * 32), "��");

			// ����
			DrawBox(0, 352, 800, 160, true);

			if ((unsigned)itemMenu.icon.Y >= bag[charaMenu.icon.Y].size()) {
				// �͈͊O
				itemMenu.icon.Y = 0;
			}

			if (menuY == ITEM) {
				// �A�C�e���I��

				DrawString(WHITE, 16, 352 + 16, bag[charaMenu.icon.Y][itemMenu.icon.Y]->GetInfo().c_str());
			}
			else if (menuY = SKILL) {
				// �X�L���I��

				for (int i = 0, n = character[charaMenu.icon.Y]->GetSkillList_Field()[itemMenu.icon.Y]->explanatoryText.size();i < n;++i) {
					DrawString(WHITE, 16, 352 + 16 + (32 * i), (character[charaMenu.icon.Y]->GetSkillList_Field()[itemMenu.icon.Y]->explanatoryText[i]).c_str());
				}
			}
		}
		else if (doMenu.isSelect) {
			// �����t��
			DrawString(WHITE, doMenu.pos.X + 16, doMenu.pos.Y + 32 + (doMenu.icon.Y * 32), "��");
		}
		else if (targetMenu.isSelect) {
			// �����t��
			DrawString(WHITE, targetMenu.pos.X + 16, targetMenu.pos.Y + 32 + (targetMenu.icon.Y * 32), "��");
		}
		else if (decideMenu.isSelect) {
			// �����t��
			DrawString(WHITE, decideMenu.pos.X + 16, decideMenu.pos.Y + 32 + (decideMenu.icon.Y * 32), "��");
		}

		// �X�e�[�^�X
		for (int i = 0, n = character.size(); i < n; i++) {
			BattleStatus s = character[i]->GetBattleStatus();
			DrawBox(i * 300, 919, 300, 160, true);
			DrawString(WHITE, 16 + i * 300, 919 + 16, (character[i]->GetName().c_str()));	// 1
			DrawString(WHITE, 16 + i * 300, 919 + 48, " HP: %d / %d", s.HP, s.maxHP);		// 2
			DrawString(WHITE, 16 + i * 300, 919 + 80, " MP: %d / %d", s.MP, s.maxMP);		// 3
			DrawString(WHITE, 16 + i * 300, 919 + 112, " LV: %d", character[i]->GetLV());	// 4
		}

		switch (menuY) {
			// ���C�����j���[�ŉ���I�񂾂��ŕ\����ς���
		case START:
			Draw_Start();
			break;

		case ITEM:
			Draw_Item();
			break;

		case SKILL:
			Draw_Skill();
			break;

		case SKILLTREE:
			Draw_SkillTree();
			break;

		case REARRANGE:
			Draw_Rearrange();
			break;

		case STATUS:
			Draw_Status();
			break;

		case SAVE:
			Draw_Save();
			break;

		case BACK:
			Draw_Back();
			break;

		default:

			// �G���[
			break;
		}

		break;

	default:
		// �G���[
		break;
	}

	/*Position2D pos;
	pos.X = 100;pos.Y = 100;
	DrawDeformationGraph(playerGraph, 100, 100, 200, 100, 200, 200, 100, 200);*/
}

void Player::Draw_Start() {

}
void Player::Draw_Item() {
	for (int i = 0, n = bag[charaMenu.icon.Y].size(); i < n; ++i) {
		if (bag[charaMenu.icon.Y][i]->isEquipment) {
			DrawString(WHITE, 480 + 320 - 64, 32 + i * 32, "[E]");
		}
	}
}
void Player::Draw_Skill() {

}
void Player::Draw_SkillTree() {

}
void Player::Draw_Rearrange() {
	// ���בւ�
	if (swapCharacterY != -1) {
		// �ړ���
		DrawString(WHITE, 336, swapCharacterY * 32 + 32, "��");	// �A�C�R��
	}
}
void Player::Draw_Status() {
	// �X�e�[�^�X�\��
	for (int i = 0, n = character.size();i < n;i++) {
		// �L�����N�^�Ɠ������������[�v
		BattleStatus s = character[i]->GetFixed();	// �G�C���A�X
		// ��̔�
		DrawBox(300 + 300 * i, 0, 300, 64 + 32 * 12, true);
		DrawString(WHITE, 300 + 16 + i * 300, 32, (character[i]->GetName().c_str()));									// 1
		DrawString(WHITE, 300 + 16 + i * 300, 64, " HP: %d / %d", character[i]->GetStatus(eStatus::HP), s.maxHP);	// 2
		DrawString(WHITE, 300 + 16 + i * 300, 96, " MP: %d / %d", character[i]->GetStatus(eStatus::MP), s.maxMP);	// 3
		DrawString(WHITE, 300 + 16 + i * 300, 128, " ATK: %d", s.STR);											// 4
		DrawString(WHITE, 300 + 16 + i * 300, 160, " DEX: %d", s.DEX);											// 5
		DrawString(WHITE, 300 + 16 + i * 300, 192, " AGI : %d", s.AGI);											// 6
		DrawString(WHITE, 300 + 16 + i * 300, 224, " LUC: %d", s.LUC);											// 7
		DrawString(WHITE, 300 + 16 + i * 300, 256, " �����h���: %d", s.DEF_PHYS);									// 8
		DrawString(WHITE, 300 + 16 + i * 300, 288, " �M�h��́@ : %d", s.DEF_HEAT);									// 9
		DrawString(WHITE, 300 + 16 + i * 300, 320, " ��C�h���: %d", s.DEF_COLD);									// 10
		DrawString(WHITE, 300 + 16 + i * 300, 352, " �d�C�h���: %d", s.DEF_ELEC);									// 11
		DrawString(WHITE, 300 + 16 + i * 300, 384, " MUE: %d", character[i]->GetMaxUseEnergy());						// 12
		// ���̔�
		DrawBox(300 + 300 * i, 64 + 32 * 12, 300, 64 + 32 * 4, true);
		DrawString(WHITE, 300 + 16 + i * 300, 460, "%s", character[i]->GetWeaponName().c_str());
		DrawString(WHITE, 300 + 16 + i * 300, 492, "%s", character[i]->GetArmorName().c_str());
		DrawString(WHITE, 300 + 16 + i * 300, 524, "%s", character[i]->GetAmuletName().c_str());
	}
}
void Player::Draw_Save() {

}
void Player::Draw_Back_Title() {
	// �^�C�g���ɖ߂�
}
void Player::Draw_Emote() {
	// �G���[�g


}
void Player::Draw_Back() {

}

void Player::PosCorrection(){
	// ���W�C��
	pos_ = pre_pos_;
	animation = false;
}

bool Player::InsertItem(int item_id) {
	// �擪���猩�āA�󂫂���������A�����ɃA�C�e�����i�[����

	Item* ip = Item::Factory(item_id);	// �`�F�b�N�p

	if (!ip) {
		// ���݂��Ȃ��A�C�e��
		return false;		// ���s
	}
	else {
		for (int i = 0, n = character.size();i < n;++i) {
			// �擪���珇�ԂɊm�F����

			if (bag[i].size() == 9) { // �A�C�e���������ς�
				continue;
			}
			else {
				bag[i].push_back(ip);	// �o�b�O�ɃA�C�e�����i�[

				return true;	// ����
			}
		}
	}

	// ���s
	return false;
}
bool Player::InsertItem(int order, int id) {
	if (bag[order].size() == 9) {
		// �A�C�e���������ς�
		return false;		// ���s
	}

	Item* ip = Item::Factory(id);	// �`�F�b�N�p

	if (!ip) {
		// ���݂��Ȃ��A�C�e��
		return false;		// ���s
	}
	else {
		bag[order].push_back(ip);
	}

	// ����
	return true;
}

// ������ɓ����
void Player::AddMoney(int value){
	if (value < 0) return;	// �}�C�i�X�l�͓n���Ȃ�
	money += value;
	// �������999,999�f
	if (money > 999999) {
		money = 999999;
	}
}
// ���𕥂�
void Player::PayMoney(int value) {
	if (value < 0) return;	// �}�C�i�X�l�͓n���Ȃ�
	money -= value;
	// 0�ȉ��ɂȂ�Ȃ��悤��
	if (money < 0) {
		money = 0;
	}
}
// ���̃Q�b�^�[
int Player::GetMoney() {
	return money;
}

// �y�ꎞ�Z�[�u�f�[�^�z
// ���j�b�g�̃��[�h�̈ꎞ�Z�[�u
void Player::TmpSave_UnitMode(int saveStage, const std::map<int, MapUnit*> unitData) {
	if (tmpSave_unitMode.find(saveStage) == tmpSave_unitMode.end()) {
		// �y�Z�[�u���ꂽ�f�[�^���Ȃ��ꍇ�z
		map<int, int> tmp;
		tmpSave_unitMode[saveStage] = tmp;	// �ǉ�
	}
	// �f�[�^��ǉ��i�㏑���j
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		tmpSave_unitMode[saveStage][itr->first] = itr->second->GetMode();
	}
}
void Player::LoadTmpSave_UnitMode(string line) {
	//���[�h�p�ϐ�
	std::istringstream stream(line);
	string token;

	int tmp_mapID = 0;
	int tmp_unitID = 0;
	int tmp_modeNum = 0;

	// �y�}�b�vID�z
	getline(stream, token, ',');
	tmp_mapID = boost::lexical_cast<int>(token);
	if (tmpSave_unitMode.find(tmp_mapID) == tmpSave_unitMode.end()) {
		// �y�Z�[�u���ꂽ�f�[�^���Ȃ��ꍇ�z
		map<int, int> tmp;
		tmpSave_unitMode[tmp_mapID] = tmp;	// �ǉ�
	}
	// �yID-���[�h�z
	while (getline(stream, token, ',')) {
		if (token == "") break;
		// �y���j�b�gID�z
		tmp_unitID = boost::lexical_cast<int>(token);
		// �y���[�h�z
		getline(stream, token, ',');
		tmp_modeNum = boost::lexical_cast<int>(token);
		tmpSave_unitMode[tmp_mapID][tmp_unitID] = tmp_modeNum;
	}
}
// ���j�b�g�̃��[�h�̃Z�b�g
bool Player::SetUnitMode_TmpSave(std::map<int, MapUnit*>& unitData) {
	if (tmpSave_unitMode.find(stage) == tmpSave_unitMode.end()) {
		//�y�Z�[�u�f�[�^�Ȃ��z
		return false;
	}
	else{
		// �y�Z�[�u���ꂽ�f�[�^������ꍇ�z
		for (auto itr = tmpSave_unitMode[stage].begin(), end = tmpSave_unitMode[stage].end(); itr != end; ++itr) {
			unitData[itr->first]->SetMode(itr->second);
		}
	}
	return true;
}
void Player::Save() {
	// ---------------------------------------------------------
	// �t�@�C���I�[�v��
	ofstream ofs("savedata\\savedata.txt");
	if (!ofs) {
		// ���߂ăZ�[�u����Ƃ��ɗ���ꏊ
	}

	// �f�[�^����
	ofs << chapter << ',';
	ofs << stage << ',';
	ofs << pos_.X << ',';
	ofs << pos_.Y << endl;
	// ���я�
	bool a = false;
	for (int i = 0, n = character.size();i < n;i++) {
		if (a) {
			ofs << ',';
		}
		ofs << character[i]->GetName();
		if (!a) {
			a = true;
		}
	}
	ofs << endl;
	// �A�C�e��
	bool b = false;
	for (int i = 0, n = bag.size();i < n;i++) {
		for (unsigned int j = 0;j < bag[i].size();++j) {
			if (b) {
				ofs << ',';
			}
			ofs << (i * 10000) + (bag[i][j]->GetID())	<< ',';	// (�L�����N�^�[�̕��я�*10000)+(�A�C�e����ID)
			ofs << bag[i][j]->isEquipment ? 1 : 0;
			if (!b) {
				b = true;
			}
		}
	}
	ofs << endl;

	// �t���O
	bool c = false;
	for (int i = 0, n = 1000;i < n;++i){
		if (c) {
			ofs << ',';
		}
		ofs << isFinish[(int)(i/100)][i%100] ? '1' : '0';
		if (!c) {
			c = true;
		}
	}
	ofs << endl;
	// ������
	ofs << money << endl;

	// ���j�b�g�̃��[�h�f�[�^
	Game::TmpSave_UnitMode();	// ������}�b�v�̃��j�b�g���Z�[�u
	for (auto itr = tmpSave_unitMode.begin(), end = tmpSave_unitMode.end(); itr != end; ++itr) {
		// �}�b�vID
		ofs << itr->first << ',';
		for (auto itr_2 = itr->second.begin(), end_2 = itr->second.end(); itr_2 != end_2; ++itr_2) {
			ofs << itr_2->first << ',' << itr_2->second << ',';
		}
		ofs << endl;
	}

	// �t�@�C���N���[�Y
	ofs.close();
	// 
	// ---------------------------------------------------------

	// �L�����N�^�[�f�[�^�̃Z�[�u
	for (int i = 0, n = character.size();i < n;i++) {
		character[i]->Save();
	}

	// ����I����m�点��
	//TextBox::AddLine("�Z�[�u���܂����I");

	// �Z�[�u�t���O�𗧂Ă�
	saveFlag = true;
}

void Player::UseItem(Item* item) {
	TextBox::AddLine(bag[charaMenu.icon.Y][itemMenu.icon.Y]->GetName() + " ���g�p���� ");	// ���O

	switch ((int)item->type) {
	case (int)ItemType::IMPORTANT:
		// �d�v�A�C�e��
		// �g�p����邱�Ƃ͂Ȃ�
		break;

	case (int)ItemType::RECOVERY:
		// �񕜃A�C�e��
		if (item->GetIsArea()) {
			for (int i = 0, n = character.size();i < n;++i) {
				item->Effect(character[i]);
			}
		}
		else {
			item->Effect(character[targetMenu.icon.Y]);
		}
		break;

	case (int)ItemType::DAMAGE:
		// �U���A�C�e��
		item->Effect(character[targetMenu.icon.Y]);
		break;

	case (int)ItemType::EQUIPMENT:
		// �����A�C�e��
		// �g�p����邱�Ƃ͂Ȃ�
		break;

	case (int)ItemType::LIFESTEAL:
		item->Effect(character[targetMenu.icon.Y], character[charaMenu.icon.Y]);

	default:
		TextBox::AddLine("�g�p���z�肳��Ă��Ȃ��A�C�e���ł��B");
		break;
	}

	delete bag[charaMenu.icon.Y][itemMenu.icon.Y];									// �A�C�e���폜
	bag[charaMenu.icon.Y].erase(bag[charaMenu.icon.Y].begin() + itemMenu.icon.Y);	// �z��̍Đ���
}