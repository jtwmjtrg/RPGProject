#include "Field.h"
#include "SkillTree.h"

map<int, int>           Field::mapChip;			// �}�b�v�f�[�^
vector<vector<int> >    Field::mapData;			// �}�b�v�f�[�^
bool					Field::encFlag;	// �G���J�E���g�t���O
vector<int>				Field::encList;	// �G���J�E���g���X�g

Field::Field(){
	mode = StageMode::FIELD;
	// �摜
	// ��
	fieldBGM = 0;

	encCount = -1;		// �G���J�E���g�܂ł̎c�����
	encMinCount = -1;	// �ŏ��G���J�E���g����
	encMaxCount = -1;	// �ő�G���J�E���g����

	shop = new Shop();	// �V���b�v�̃������m��

	LoadMapData();		// �}�b�v�f�[�^�̓ǂݍ���
	//LoadUnitData();	// ���j�b�g�z�u�f�[�^�ǂݍ���

	waitBattle		= false;
	waitBattleCount = 0;

	for (int i = 0;i < 2;++i) {
		// string->const_char*�ϊ�
		string str = "img\\field\\wait_battle" + to_string(i) + ".png";	// �A�h���X����
		char* cstr = new char[str.size() + 1];			// �������m��
		strcpy_s(cstr, str.size() + 1, str.c_str());	// �R�s�[
		waitBattleGraph.push_back(LoadGraph(cstr));

		delete cstr;
	}

	isFollow = true;	// �J�����̒Ǐ]
	wait_cam_time_ = 0; // �J�����̑ҋ@����
}
Field::~Field(){
	// �����~�߂�
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);
	// �摜�폜
	for (int i = 0;i < (int)waitBattleGraph.size();++i) {
		DeleteGraph(waitBattleGraph[i]);
	}
}

void Field::LoadMapData(){
	if (fieldBGM != 0)
	{
		// BGM������Ă���Ȃ�Ƃ߂Ă��珈���X�^�[�g
		StopSoundMem(fieldBGM);
		DeleteSoundMem(fieldBGM);
	}

	performanceCount = 0;	// �؂�ւ����o�X�^�[�g
	performanceSpeed = 16;	// �؂�ւ��X�s�[�h

	// ---------------------------------------------
	// �}�b�v�f�[�^---------------------------------

	// �}�b�v�`�b�v�폜
	for (auto itr = mapChip.begin();itr != mapChip.end();itr++) {
		DeleteGraph(itr->second);
	}
	mapChip.clear();
	mapData.clear();
	mapData.shrink_to_fit();
	encList.clear();		// �G���J�E���g���X�g������
	encList.shrink_to_fit();

	ifstream ifs("mapdata\\" + to_string(Player::stage) + ".csv", ios::in);
	if (!ifs){
		// �ǂݍ��ݎ��s
		return;
	}

	int i = 0;	// ���[�v�񐔂��J�E���g
	string str;	// ���͂��ꂽ������(�s)��ۑ�


	int mapHeight = 0;		// �}�b�v�̏c��
	int mapWidth = 0;		// �}�b�v�̉���
	
	//////////////////////////////////////
	//////// �y1�s��-�}�b�v�T�C�Y�z////////
	//////////////////////////////////////
	{
		getline(ifs, str);
		stringstream stream(str);	// str���X�g���[���Ƃ��Ď���
		string token;				// ���͂��ꂽ������(�J���}��؂�)��ۑ�
		// 1�y�}�b�v�����z
		getline(stream, token, ',');
		mapWidth = boost::lexical_cast<int>(token);
		// 2�y�}�b�v�c���z
		getline(stream, token, ',');
		mapHeight = boost::lexical_cast<int>(token);
	}
	//////////////////////////////////////
	//////// �y2�s��-�G���J�E���g�z////////
	//////////////////////////////////////
	{
		getline(ifs, str);
		stringstream stream(str);	// str���X�g���[���Ƃ��Ď���
		string token;				// ���͂��ꂽ������(�J���}��؂�)��ۑ�
		// 1�y�G���J�E���g�t���O�z
		getline(stream, token, ',');
		encFlag = token == "1";
		if (encFlag) {
			// 2�y�ŏ��G���J�E���g�����z
			getline(stream, token, ',');
			encMinCount = boost::lexical_cast<int>(token);
			// 3�y�ő�G���J�E���g�����z
			getline(stream, token, ',');
			encMaxCount = boost::lexical_cast<int>(token);
			encCount = GetRand(encMinCount, encMaxCount);	// �G���J�E���g�܂ł̕������X�V
			// 4�y�G���J�E���gid�z
			while (getline(stream, token, ',')) {
				if (token == "") break;
				encList.push_back(boost::lexical_cast<int>(token));
			}
		}
		else {
			encMinCount = -1;
			encMaxCount = -1;
			encCount = -1;
		}
	}
	mapData.resize(mapHeight);		// �c���������m��

	while (getline(ifs, str)){
		//mapData.resize(i + 1);	// �������m��

		stringstream stream(str);	// str���X�g���[���Ƃ��Ď���
		string token;				// ���͂��ꂽ������(�J���}��؂�)��ۑ�

		// ���[�v�Q��ڈȍ~
		/*if (i != 0) {
			mapData[i].resize(mapData[i - 1].size());	// �ϑ��T�C�Y���󂯕t�����Ȃ�����ɏ����̍�������_��
		}*/
		mapData[i].resize(mapWidth);		// �����������m��
		int j = 0;	// ���[�v�񐔂��J�E���g

		while (getline(stream, token, ',')) {
			/*// ���[�v����
			if (i == 0) {
				mapData[i].push_back(boost::lexical_cast<int>(token));
			}
			// ���[�v�Q��ڈȍ~
			else {
				mapData[i][j] = boost::lexical_cast<int>(token);
			}*/
			mapData[i][j] = boost::lexical_cast<int>(token);
			// �V�����v�f�ł���΃}�b�v�ɓo�^
			auto itr = mapChip.find(mapData[i][j]);
			if (itr == mapChip.end()) {
				// string->const_char*�ϊ�
				string str = "img\\mapchip\\" + token + ".png";	// �A�h���X����
				char* cstr = new char[str.size() + 1];			// �������m��
				strcpy_s(cstr, str.size() + 1, str.c_str());	// �R�s�[

				mapChip.insert(make_pair(mapData[i][j], LoadGraph(cstr)));	// �摜�n���h���̊֘A�t��

				delete cstr;	// �������J��
			}

			j++;	// �J�E���g��i�߂�
		}
		
		i++;		// �J�E���g��i�߂�
	}

	ifs.close();	// �t�@�C���N���[�Y

	// �}�b�v�f�[�^---------------------------------
	// ---------------------------------------------
	

	// ---------------------------------------------
	// �C�x���g�f�[�^

	eventData.clear();
	eventData.shrink_to_fit();

	ifstream ifs2;
	ifs2.open("eventdata\\" + to_string(Player::stage) + ".csv");
	if (!ifs2) {
		// �t�@�C�����Ȃ��ꍇ
		eventNum = 0;
	}
	else {
		int i2 = 0;		// ���[�v�񐔂��J�E���g
		string str2;	// ���͂��ꂽ������(�s)��ۑ�

		// 1��擾
		while (getline(ifs2, str2)) {
			eventData.resize(i2 + 1);	// �������m��

			stringstream stream2(str2);	// str���X�g���[���Ƃ��Ď���
			string token2;				// ���͂��ꂽ������(�J���}��؂�)��ۑ�

			while (getline(stream2, token2, ',')) {
				eventData[i2].push_back(token2);
			}

			++i2;		// �J�E���g��i�߂�
		}

		ifs2.close();	// �t�@�C���N���[�Y

		eventNum = 100;		// �C�x���g�̐���ۑ�
		eventHead = 1;		// �C�x���g�̐i�s�x��\��
	}
	// �C�x���g�f�[�^
	// ---------------------------------------------

	// ��
	{
		string str = "sound\\" + to_string(Player::stage) + ".wav";	// �A�h���X����
		char* cstr = new char[str.size() + 1];			// �������m��
		strcpy_s(cstr, str.size() + 1, str.c_str());	// �R�s�[
		fieldBGM = LoadSoundMem(cstr);
		PlaySoundMem(fieldBGM, true);
	}
}

void Field::Process(){
	
	// �e�L�X�g�{�b�N�X�̕\�����͖��t���[���������s��Ȃ�
	if (TextBox::textBoxFlag) return;

	// �퓬�J�n����
	if (waitBattle) {
		if (waitBattleCount < 30 - 1) {
			++waitBattleCount;
		}
		else {
			encCount = GetRand(encMinCount, encMaxCount);	// �G���J�E���g�܂ł̕������X�V
			Game::mode = eStageMode::BATTLE;	// �퓬�J�n
		}

		return;
	}

	playerPreStage = Player::stage;	// ���O�̃X�e�[�W��ۑ�

	CameraProcess();

	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed > 0) {
		// �X�e�[�W�؂�ւ����̉��o
		if (performanceCount * performanceSpeed < WINDOW_HEIGHT)
		{
			++performanceCount;
		}
		return;
	}

	if (isEvent) {
		EventProcess(now_event_id_);
	}
	if (eventNum != 0) {
		// �C�x���g������Ȃ�
		for (unsigned int i = 0,n= eventData.size(); i < n; ++i) {
			if (Player::isFinish[Player::stage][i]) {
				// �����ς݃C�x���g
			}
			else {
				if (eventData[i][1] == "FIRST"){
					// ���߂ĖK�ꂽ�Ƃ�
					if (!isEvent) {
						eventHead = 2;
						isEvent = true;
					}
					now_event_id_ = i;
					EventProcess(i);
					break;
				}
				else if(eventData[i][1] == "TALK") {
					// ����̃��j�b�g�ɘb����������

					if (Player::talkFlag.flag) {
						if (Player::talkFlag.pos_.X == unitData[boost::lexical_cast<int>(eventData[i][2])]->x
							&& Player::talkFlag.pos_.Y == unitData[boost::lexical_cast<int>(eventData[i][2])]->y) {
							if (!isEvent) {
								eventHead = 3;
								isEvent = true;
							}
							now_event_id_ = i;
							EventProcess(i);
							break;
						}
					}
				}
				else if (eventData[i][1] == "CATCH_EMOTE") {
					// ����̃G���[�g���g�p���ꂽ�Ƃ�

					if (Player::emote_lock_ > 0) {
						// �v���C���[���G���[�g���Ă���

						for (int j = 0, m = Emote::GetEmoteSize();j < m;++j) {
							if (std::stoi(eventData[i][2]) == Emote::GetEmoteId(j)) {
								// �w��̃G���[�g���g�p���ꂽ

								// �C�x���g�N��
								if (!isEvent) {
									eventHead += 2;
									isEvent = true;
								}
								now_event_id_ = i;
								EventProcess(i);	// �C�x���g�i�s
								i = 999;
								break;
							}
						}
					}
				}
				else if(eventData[i][1] == "FINISH"){
					// �ق��̃C�x���g���I��������ǂ����Ď�����

					bool check_check_check = true;

					if (!isEvent) {
						for (eventHead = eventHead + 1;eventData[i][eventHead] != "FINISH_END";eventHead += 2) {
							if (Player::isFinish[std::stoi(eventData[i][eventHead])][std::stoi(eventData[i][eventHead + 1])]) {

							}
							else {
								eventHead = 1;				// ���d�v������
								check_check_check = false;	// �C�x���g���I����ĂȂ�
								break;
							}
						}

						if (check_check_check) {
							// �C�x���g�N��
							if (!isEvent) {
								++eventHead;
								isEvent = true;
							}
							now_event_id_ = i;
							EventProcess(i);	// �C�x���g�i�s
							break;
						}
					}
				}
			}
		}
	}

	CameraProcess();	// �J�����X�V

	Emote::Process();	// �G���[�g

	if (isEvent) {
		// �C�x���g���͈ȉ��̑���E�������s��Ȃ�
		return;
	}
	// ����������������������������������������������������������������
	// �C�x���g���ł��󂯕t���鏈��
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// ----------------------------------------------------------------
	// �C�x���g���󂯕t���Ȃ�����
	// ����������������������������������������������������������������

	if (Player::flag_shop) {
		shop->Process();
		return;
	}

	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed <= 0) {
		// ���o���łȂ��Ȃ�
		// �s�����s���̃��j�b�g������΍s��������
		if (!exec_Unit.empty()) {
			if (!exec_Unit.front()->ActionExecution()) {
				// ���̍s����������΍s�����s���̃��j�b�g�������
				exec_Unit.erase(exec_Unit.begin());
			}
			return;
		}

		// �v���C���[�̈ړ�����
		if(Player::Process() && encFlag) {
			// MP������
			for (int i = 0, n = Player::character.size(); i < n; ++i) {
				Player::character[i]->AutoIncMP_Field();
			}
			// �ړ�������G���J�E���g�܂ł̕�����1���炷
			--encCount;
		}

		// �Z�[�u���Ƀ��j�b�g�̃��[�h���Z�[�u����
		if (Player::saveFlag) {
			SaveUnitMode(Player::stage);	// ���݂���}�b�v�̃��j�b�g�̃��[�h�̃Z�[�u
		}

		// �ړ��ɑΉ����ăv���C���[�Ɉʒu�̏C�������s��
		for (int i = 0;i != 2;++i) {
			for (int j = 0;j != 2;++j) {
				MapEffect(PlayerPosition(i, j));
			}
		}
		
		// ���j�b�g����
		{
			// �v���C���[�̋敪�����W
			const int playerAreaX = Player::pos_.X / CHIPSIZE / AREASIZE;
			const int playerAreaY = Player::pos_.Y / CHIPSIZE / AREASIZE;

			// �v���C���[�̎���1�G���A�̂ݏ�������
			for (int i = (playerAreaY > 0 ? -1 : 0), n = (playerAreaY < (signed)(unitArea.size() - 1) ? 1 : 0); i <= n; ++i) {
				for (int j = (playerAreaX > 0 ? -1 : 0), m = (playerAreaX < (signed)(unitArea[playerAreaY + i].size() - 1) ? 1 : 0); j <= m; ++j) {
					
					for (auto itr = unitArea[playerAreaY + i][playerAreaX + j].begin(), end = unitArea[playerAreaY + i][playerAreaX + j].end(); itr != end; ++itr) {
						// �v���C���[���烆�j�b�g�ւ̓����蔻��
						if ((*itr)->CheckColl(Player::pos_.X, Player::pos_.Y)) {
							Player::PosCorrection();
						}
						// �X�V����
						if ((*itr)->Process()) {
							exec_Unit.push_back((*itr));	// ���s���̃��j�b�g�ɒǉ�
						}
						// �敪���Ԃ��ړ�������
						if ((*itr)->x / CHIPSIZE / AREASIZE != playerAreaX + j ||
							(*itr)->y / CHIPSIZE / AREASIZE != playerAreaY + i) {
							(*itr)->PosCorrection();	// ���Ƃɖ߂�
						}
						// ���j�b�g����v���C���[�ւ̓����蔻��
						if ((*itr)->CheckColl(Player::pos_.X, Player::pos_.Y)) {
							(*itr)->PosCorrection();	// ���W�C��
						}
					}

				}
			}
			
		}
		/*
		for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
			// �v���C���[���烆�j�b�g�ւ̓����蔻��
			if (itr->second->CheckColl(Player::X, Player::Y)) {
				Player::PosCorrection();
			}
			if (itr->second->Process()) {
				exec_Unit.push_back(itr->second);	// ���s���̃��j�b�g�ɒǉ�
			}
			if(CollisionChack(itr->second->x, itr->second->y)) {
				itr->second->PosCorrection();	// ���W�C��
			}
			// ���̃��j�b�g�Ƃ̓����蔻��
			for (auto j = unitData.begin(), m = unitData.end(); j != m; ++j) {
				if (j->second->GetAppearFlag() && j->second->GetCollisionFlag() && itr->second != j->second) {
					if (itr->second->CheckColl(j->second->x, j->second->y)){
						itr->second->PosCorrection();	// ���W�C��
					}
				}
			}
			// ���j�b�g����v���C���[�ւ̓����蔻��
			if (itr->second->CheckColl(Player::X, Player::Y)) {
				itr->second->PosCorrection();	// ���W�C��
			}
		}
		*/
		if (playerPreStage != Player::stage) {
			// �}�b�v�ړ�
			LoadMapData();	// �}�b�v�f�[�^�ēǂݍ���
			LoadUnitData();	// ���j�b�g�f�[�^�ēǂݍ���
		}

		// �G���J�E���g����
		if (encFlag && encCount == 0 && !encList.empty()) {
			// �G���J�E���g���X�g���烉���_���Ő퓬�J�n
			Encounter(encList[GetRand(encList.size() - 1)], chipToBack[PlayerPosition()], LoadSoundMem("sound\\battle.wav"));
		}

		// debug
		if (Input::Get(VK_B) == 1) {
			//Encounter(101);	// �퓬�J�n
			encCount = 0;
			//Encounter(110, chipToBack[PlayerPosition()]);
		}
	}
}
void Field::CameraProcess(){
	// �J�������W�C��
	if (isFollow) {
		camera_pos_.X = Player::pos_.X - (CHIPSIZE * 15);
		camera_pos_.Y = Player::pos_.Y - (CHIPSIZE * 8);
	}
}
void Field::MapEffect(int position){
	// ������ő傫��������
	// ����
	if (position % 2 == 0){
		// �ʍs�\
	}
	//�
	else{
		// �ʍs�s��
		Player::PosCorrection();	// �v���C���[�̍��W�𒼑O�̂��̂ɖ߂�
	}
}
int Field::PlayerPosition(){
	return mapData[(Player::pos_.Y + (CHIPSIZE / 2)) / CHIPSIZE][(Player::pos_.X + (CHIPSIZE / 2)) / CHIPSIZE];
}
int Field::PlayerPosition(int i){
	return mapData[(Player::pos_.Y + (i*(CHIPSIZE - 1))) / CHIPSIZE][(Player::pos_.X + (i*(CHIPSIZE - 1))) / CHIPSIZE];
}
int Field::PlayerPosition(int i, int j){
	return mapData[(Player::pos_.Y + (i*(CHIPSIZE - 1))) / CHIPSIZE][(Player::pos_.X + (j*(CHIPSIZE - 1))) / CHIPSIZE];
}
// �}�b�v�Ƃ̏Փ˔���
bool Field::CollisionChack(int x, int y) {
	return {
		// 1�h�b�g�����𔻒肷��
		mapData[(y + 1) / CHIPSIZE    ][(x + 1) / CHIPSIZE    ] % 2 == 1 ||		// ����
		mapData[(y - 1) / CHIPSIZE + 1][(x + 1) / CHIPSIZE    ] % 2 == 1 ||		// ����
		mapData[(y + 1) / CHIPSIZE    ][(x - 1) / CHIPSIZE + 1] % 2 == 1 ||		// �E��
		mapData[(y - 1) / CHIPSIZE + 1][(x - 1) / CHIPSIZE + 1] % 2 == 1		// �E��
	};
}

void Field::Draw(){
	// �}�b�v�`�b�v
	for (int i = (camera_pos_.Y / CHIPSIZE), n = i + 18;i != n;++i) {
		if (i < 0) {
			// OutOfRange�΍�
			i = 0;
		}
		for (int j = (camera_pos_.X / CHIPSIZE), m = j + 31;j != m;++j) {
			if (j < 0) {
				// OutOfRange�΍�
				j = 0;
			}
			// mapData�ɋL�q����Ă��Ȃ������̕`��ɑΉ����Ă���
			if ((unsigned)i < mapData.size()) {
				if ((unsigned)j < mapData[i].size()) {
					// �}�b�v�`�b�v�̕`��
					DrawGraph(j*CHIPSIZE - camera_pos_.X, i*CHIPSIZE - camera_pos_.Y, mapChip[mapData[i][j]]);
				}
			}
		}
	}

	// ���j�b�g
	//for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
	//	itr->second->Draw(cameraX, cameraY);
	//}
	// ���j�b�g
	{
		// �v���C���[�̋敪�����W
		const int playerAreaX = Player::pos_.X / CHIPSIZE / AREASIZE;
		const int playerAreaY = Player::pos_.Y / CHIPSIZE / AREASIZE;
		// �v���C���[�̎���1�G���A�̂ݏ�������
		for (int i = (playerAreaY > 0 ? -1 : 0), n = (playerAreaY < (signed)(unitArea.size() - 1) ? 1 : 0); i <= n; ++i) {
			for (int j = (playerAreaX > 0 ? -1 : 0), m = (playerAreaX < (signed)(unitArea[playerAreaY + i].size() - 1) ? 1 : 0); j <= m; ++j) {
				for (auto itr = unitArea[playerAreaY + i][playerAreaX + j].begin(), end = unitArea[playerAreaY + i][playerAreaX + j].end(); itr != end; ++itr) {
					(*itr)->Draw(camera_pos_.X, camera_pos_.Y);	// ���W�C��
				}
			}
		}
	}
	
	// �v���C���[
	Player::Draw(isFollow, camera_pos_.X, camera_pos_.Y);

	// �X�e�[�W�؂�ւ����̉��o
	if ((WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed > 0) {
		DrawBox(0, 0, WINDOW_WIDTH, (WINDOW_HEIGHT / 2) - performanceCount*performanceSpeed, true);				// �X���C�h���o
		DrawBox(0, (WINDOW_HEIGHT / 2) + performanceCount*performanceSpeed, WINDOW_WIDTH, WINDOW_HEIGHT, true);	// �X���C�h���o
	}

	if (Player::flag_shop) {
		shop->Draw();
	}

	// �t�B�[���h�ł̑������
	//DrawGraph(WINDOW_WIDTH - 679, WINDOW_HEIGHT - 400, gr_Operation);

	// �e�L�X�g�{�b�N�X�\��
	if (TextBox::textBoxFlag) {
		TextBox::Draw();
	}

	if (waitBattle) {
		DrawGraph(0,  360 - waitBattleCount * ((1080 / 30) + (360 / 30)), waitBattleGraph[0]);
		DrawGraph(0, 1440 - waitBattleCount * ((1080 / 30) + (360 / 30)), waitBattleGraph[1]);
	}

	Emote::Draw();	// �G���[�g
	
	// debug
	// �敪����
	/*for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		for (int j = 0, m = (int)unitArea[i].size(); j < m; ++j) {
			DrawBox(j * CHIPSIZE * AREASIZE - cameraX, i * CHIPSIZE * AREASIZE - cameraY, CHIPSIZE * AREASIZE, CHIPSIZE * AREASIZE, false);
		}
	}*/
	
	/*
	DrawString(BLUE, 10, 10, "�G���J�E���g�܂ł̕����F%d", encCount);
	DrawString(BLUE, 10, 40, "�ŏ��G���J�E���g�����F%d", encMinCount);
	DrawString(BLUE, 10, 70, "�ő�G���J�E���g�����F%d", encMaxCount);
	DrawString(BLUE, 10, 100, "�G���J�E���g���X�g�F");
	for (int i = 0, n = encList.size(); i < n; ++i) {
		DrawString(BLUE, 230 + i * 60, 100, "%d,", encList[i]);
	}
	DrawString(BLUE, 10, 10, "���s���̃��j�b�g�F%d", exec_Unit.size());
	*/

	/*DrawString(BLUE, 10, 10, "���s���̃��j�b�g�F%d", camera_pos_.X);
	DrawString(BLUE, 10, 30, "���s���̃��j�b�g�F%d", camera_pos_.Y);*/

	/*for (int i = 0;i < Player::isFinish[Player::stage].size();++i) {
		DrawString(WHITE, 0, i * 32, "%d", (int)Player::isFinish[Player::stage][i]);
	}*/

	/*if (Player::stage == 2) {
		for (int i = 0;i < eventData[2].size();++i) {
			DrawString(BLUE, i*200, 256, eventData[2][i].c_str());
		}
	}
	*/
}

void Field::EventProcess(int id) {
	// �C�x���g�̏���

	vector<string> event_data = eventData[id];	// �G�C���A�X
	string event_name = event_data[eventHead];	// �G�C���A�X

	if (event_name == "WARP") {
		// �ړ�
		Player::pos_.set_pos(boost::lexical_cast<int>(event_data[eventHead + 1]), boost::lexical_cast<int>(event_data[eventHead + 2]));
		eventHead += 3;
	}
	else if (event_name == "MOVE") {
		// X���W
		if (Player::pos_.X < boost::lexical_cast<int>(event_data[eventHead + 1])) {
			Player::pos_.X += 8;
		}
		else if (Player::pos_.X > boost::lexical_cast<int>(event_data[eventHead + 1])) {
			Player::pos_.X -= 8;
		}
		// Y���W
		if (Player::pos_.Y < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			Player::pos_.Y += 8;
		}
		// Y���W
		else if (Player::pos_.Y > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			Player::pos_.Y -= 8;
		}

		if (Player::pos_.X == boost::lexical_cast<int>(event_data[eventHead + 1]) && Player::pos_.Y == boost::lexical_cast<int>(event_data[eventHead + 2])) {
			eventHead += 3;
		}
	}
	else if (event_name == "DIRECTION") {
		// �v���C���[���w��̕����Ɍ�����

		Player::direction = (Direction)std::stoi(event_data[eventHead + 1]);

		eventHead += 2;
	}
	else if (event_name == "SPEAK") {
		// ����

		vector<string> str;		// ���͂��x�N�^�[�ɕۑ�

		++eventHead;			// �ǂݍ��݃w�b�_

		do {
			// ���s�̏���
			str.push_back(event_data[eventHead]);	// ���͂��x�N�^�[�ɕۑ�
			++eventHead;								// �ǂݍ��݃w�b�_
		} while (event_data[eventHead] != "SPEAK_END");

		++eventHead;			// �ǂݍ��݃w�b�_

		TextBox::AddLine(str);	// �o��
	}
	else if (event_name == "APPEAR_UNIT") {
		// Unit[id]��\��/��\������

		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->SetAppearFlag((bool)std::stoi(event_data[eventHead + 2]));

		eventHead += 3;
	}
	else if (event_name == "WARP_UNIT") {
		// Unit[id]���w��̍��W�ɑ����Ɉړ�������

		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x = boost::lexical_cast<int>(event_data[eventHead + 2]);
		unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y = boost::lexical_cast<int>(event_data[eventHead + 3]);

		eventHead += 4;
	}
	else if (event_name == "MOVE_UNIT") {
		// X���W
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x += 8;
		}
		else if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x -= 8;
		}
		// Y���W
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y < boost::lexical_cast<int>(event_data[eventHead + 3])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y += 8;
		}
		else if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y > boost::lexical_cast<int>(event_data[eventHead + 3])) {
			unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y -= 8;
		}

		// �I������
		if (unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x == boost::lexical_cast<int>(event_data[eventHead + 2])
			&& unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y == boost::lexical_cast<int>(event_data[eventHead + 3])) {
			eventHead += 4;
		}
	}
	else if (event_name == "WARP_CAM") {
		// �ړ�
		camera_pos_.set_pos(boost::lexical_cast<int>(event_data[eventHead + 1]), boost::lexical_cast<int>(event_data[eventHead + 2]));
		eventHead += 3;
	}
	else if (event_name == "MOVE_CAM") {
		isFollow = false;

		// X���W
		if (camera_pos_.X < boost::lexical_cast<int>(event_data[eventHead + 1])) {
			camera_pos_.X += 4;
		}
		else if (camera_pos_.X > boost::lexical_cast<int>(event_data[eventHead + 1])) {
			camera_pos_.X -= 4;
		}
		// Y���W
		if (camera_pos_.Y < boost::lexical_cast<int>(event_data[eventHead + 2])) {
			camera_pos_.Y += 4;
		}
		else if (camera_pos_.Y > boost::lexical_cast<int>(event_data[eventHead + 2])) {
			camera_pos_.Y -= 4;
		}

		// �I������
		if ((camera_pos_.X == boost::lexical_cast<int>(event_data[eventHead + 1]))
			&& (camera_pos_.Y == boost::lexical_cast<int>(event_data[eventHead + 2]))) {
			eventHead += 3;
		}
	}
	else if (event_name == "WAIT_CAM") {
		isFollow = false;
		if (boost::lexical_cast<int>(event_data[eventHead + 1]) <= wait_cam_time_) {
			eventHead += 2;

			wait_cam_time_ = 0;
		}
		else {
			wait_cam_time_++;
		}
	}
	else if (event_name == "EMOTE") {
		// �G���[�g

		Emote::SetEmote(
			boost::lexical_cast<int>(event_data[eventHead + 1]),
			boost::lexical_cast<int>(event_data[eventHead + 2]),
			boost::lexical_cast<int>(event_data[eventHead + 3])
			);

		eventHead += 4;
	}
	else if (event_name == "JOIN_CHARACTER") {
		// �L�����N�^�[���̃L�����N�^�[���Z�[�u�f�[�^����ǂݍ����/�ǂݍ��܂��Ƀp�[�e�B�[�ɉ�����

		string name = event_data[eventHead + 1];	// �G�C���A�X

		if (name == "tee") {
			// �e�B�[

			Player::character.push_back(new Tee(name, (bool)(std::stoi(event_data[eventHead + 2]))));
			Player::bag.resize(Player::character.size());	// �o�b�O����

			if (!(bool)(std::stoi(event_data[eventHead + 2]))) {
				// ���߂ăp�[�e�B�[�ɉ������
				Player::character.back()->Lv1_Init();			// Lv.1��Ԃւ̏�����

				int back = Player::character.size() - 1;

				// �A�C�e������������
				Player::InsertItem(back, 10);	// �}�W�b�N�����h
				Player::InsertItem(back, 4);	// �z�̕�
				Player::InsertItem(back, 5);	// �����
				// �����𑕔�
				for (int i = 0;i < 3;++i) {
					Player::bag[back][i]->Effect(Player::character[back]);	// ����
					Player::bag[back][i]->isEquipment = true;
				}
			}

			// ���j���[�{�b�N�X�ɒǉ�
			Player::charaMenu.itemName.push_back(name);
			Player::targetMenu.itemName.push_back(name);
			Player::targetMenu.height += 32;
			Shop::charaMenu.itemName.push_back(name);
			Shop::charaMenu.height += 32;
		}

		eventHead += 3;
	}

	// �I������
	else if (event_name == "END") {
		// ��x����̃C�x���g

		eventHead = 1;	// �����ς�
		Player::isFinish[Player::stage][id] = true;
		now_event_id_ = 0;
		isEvent = false;

		isFollow = true;
	}
	else if (event_name == "CONTINUE") {
		// ���x�ł��N����C�x���g

		eventHead = 1;	// �����ς�
		now_event_id_ = 0;
		isEvent = false;

		isFollow = true;
	}
	else {
		// ��������ċA����

		bool check_check_check = true;

		if (event_name == "FIRST") {
			// ���߂ĖK�ꂽ�Ƃ�

			eventHead += 1;
			EventProcess(id);
		}
		else if (event_name == "TALK") {
			// ����̃��j�b�g�ɘb����������

			if (Player::talkFlag.flag) {
				if (Player::talkFlag.pos_.X == unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->x
					&& Player::talkFlag.pos_.Y == unitData[boost::lexical_cast<int>(event_data[eventHead + 1])]->y) {
					eventHead += 2;
					EventProcess(id);
					check_check_check = false;
				}
			}
		}
		else if (event_name == "CATCH_EMOTE") {
			// ����̃G���[�g���g�p���ꂽ�Ƃ�

			if (Player::emote_lock_ > 0) {
				// �v���C���[���G���[�g���Ă���

				for (int j = 0, m = Emote::GetEmoteSize();j < m;++j) {
					if (std::stoi(event_data[eventHead + 2]) == Emote::GetEmoteId(j)) {
						// �w��̃G���[�g���g�p���ꂽ

						eventHead += 2;
						EventProcess(id);	// �C�x���g�i�s
						check_check_check = false;
					}
				}
			}
		}
		else if (event_name == "FINISH") {
			// �ق��̃C�x���g���I��������ǂ����Ď�����

			bool check_check_check_check_check = true;
			
			for (eventHead = eventHead + 1;event_data[eventHead] != "FINISH_END";eventHead += 2) {
				if (Player::isFinish[std::stoi(event_data[eventHead])][std::stoi(event_data[eventHead + 1])]) {

				}
				else {
					eventHead = 1;							// ���d�v������
					check_check_check_check_check = false;	// �C�x���g���I����ĂȂ�
					break;
				}
			}

			if (check_check_check_check_check) {
				// �C�x���g�N��

				eventHead += 1;
				EventProcess(id);	// �C�x���g�i�s
				check_check_check = false;
			}
		}

		if (check_check_check) {
			eventHead = 1;
			isEvent = false;
			isFollow = true;
		}
	}
}