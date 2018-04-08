#include "Game.h"

StageMode				Game::mode = StageMode::FIELD;
std::map<int, MapUnit*> Game::unitData;			// �e���j�b�g�̃|�C���^
vector<MapUnit*>		Game::exec_Unit;		// ���ݍs�����s���̃��j�b�g�̃|�C���^
vector<vector<string> > Game::eventData;		// �C�x���g�f�[�^
int                     Game::eventNum;			// �C�x���g�̐�
int                     Game::eventHead;		// �ǂ��܂ŃC�x���g��ǂ񂾂�
bool                    Game::isEvent;			// �C�x���g�����ǂ���
int						Game::now_event_id_;	// ���ݏ������̃C�x���g��id
tEncounter				Game::encounter;		// �G���J�E���g�f�[�^
MapInfo					Game::info;				// �}�b�v�̏��
std::map<int, int>		Game::chipToBack;		// �}�b�v�`�b�v���ƂɃo�g����ʂ̔w�i���w��
bool					Game::waitBattle;		// �퓬�ҋ@��ԃt���O
int						Game::fieldBGM;			// �t�B�[���hBGM

std::vector<std::vector< std::list<MapUnit*> > >	Game::unitArea;	// ���j�b�g�̋�敪��

Game::Game()
{
	
}
Game::~Game() {
}

// �}�b�v�`�b�v�ƃo�g����ʂ̔w�i�̘A�z�z����쐬
void Game::LaodChipToBack() {
	ifstream ifs("Encounterdata\\chipToBack.csv", ios::in);
	if (!ifs) {
		return;
	}
	string str;
	string token;

	int chipNum = 0;
	int backNum = 0;

	while (getline(ifs, str)) {	// ��s���ǂݍ���
		// �ǂݍ��񂾍s���R�s�[
		std::istringstream stream(str);
		// 1�y�}�b�v�`�b�v�z
		getline(stream, token, ',');
		chipNum = boost::lexical_cast<int>(token);
		// 2�y�w�i�z
		getline(stream, token, ',');
		backNum = boost::lexical_cast<int>(token);
		chipToBack[chipNum] = backNum;
	}

	ifs.close();	// �t�@�C���N���[�Y
}

// ���j�b�g�̔z�u�f�[�^��ǂݍ���
void Game::LoadUnitData() {
	//SaveUnitMode(playerPreStage);	// ���O�ɂ����}�b�v�̃��j�b�g�̃��[�h�̃Z�[�u
	if(!unitData.empty()) Player::TmpSave_UnitMode(playerPreStage, unitData);
	//Player::Save();					// �v���C�f�[�^�̃I�[�g�Z�[�u
	// ���������
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; itr++) {
		itr->second->EndProcess();			// �I������
		DeleteGraph(itr->second->GetImg());	// �摜�폜
		delete itr->second;					// ���������
	}
	unitData.clear();
	exec_Unit.clear();
	exec_Unit.shrink_to_fit();

	ifstream ifs("unitdata\\" + to_string(Player::stage) + "\\PutUnit.csv", ios::in);
	if (!ifs) {
		// �ǂݍ��ݎ��s
		return;
	}
	else {
		// �ǂݍ��ݐ���
		string line;
		string token;

		while (getline(ifs, line)) {
			std::istringstream stream(line);
			getline(stream, token, ',');
			unitData[boost::lexical_cast<int>(token)] = new MapUnit(line);
		}
	}	

	ifs.close();	// �t�@�C���N���[�Y

	// ���j�b�g�̃��[�h�f�[�^�̃��[�h
	Player::SetUnitMode_TmpSave(unitData);

	SetUnitArea();	// ���j�b�g�̋�敪��
}
// ���j�b�g�̃��[�h�f�[�^�̃��[�h
/*
void Game::LoadUnitMode() {
	
	ifstream ifs("unitdata\\" + to_string(Player::stage) + "\\SaveMode.csv", ios::in);
	
	if (ifs.fail()) {	// �t�@�C���ǂݍ���
		// �ǂݍ��ݎ��s
	}
	else {
		string line;
		string token;

		int id = 0;			// ID
		auto itr = unitData.begin();// ���j�b�g�̃C�e���[�^
		auto end = unitData.end();	// ���j�b�g��end�C�e���[�^

		while (getline(ifs, line)) {
			// 1�yid�z
			std::istringstream stream(line);
			getline(stream, token, ',');
			id = boost::lexical_cast<int>(token);
			itr = unitData.find(id);
			// ID��������΃X�L�b�v
			if (itr != end) {
				// 2�y���[�h�ԍ��z
				getline(stream, token, ',');
				itr->second->SetMode(boost::lexical_cast<int>(token));
			}
		}
	}
	ifs.close();
	
}
*/
// ���j�b�g�̃��[�h�f�[�^�̃Z�[�u
/*
void Game::SaveUnitMode(int stage) {
	
	// �t�@�C���I�[�v��
	ofstream ofs("unitdata\\" + to_string(stage) + "\\SaveMode.csv");
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		ofs << itr->first << ',' << itr->second->GetMode() << endl;		// ID, ���[�h
	}
	ofs.close();
	
}
*/
// ���j�b�g�̋�敪��
void Game::SetUnitArea() {
	for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		unitArea[i].clear();
		unitArea[i].shrink_to_fit();
	}
	unitArea.clear();
	unitArea.shrink_to_fit();
	

	// �敪������
	unitArea.resize(mapData.size() / AREASIZE + 1);
	for (int i = 0, n = (int)unitArea.size(); i < n; ++i) {
		unitArea[i].resize(mapData[0].size() / AREASIZE + 1);
	}

	int tmpAreaX = 0;
	int tmpAreaY = 0;
	for (auto itr = unitData.begin(), end = unitData.end(); itr != end; ++itr) {
		tmpAreaX = itr->second->x / CHIPSIZE / AREASIZE;
		tmpAreaY = itr->second->y / CHIPSIZE / AREASIZE;
		unitArea[tmpAreaY][tmpAreaX].push_back(itr->second);		// ���ɓ����
		itr->second->SetArea(tmpAreaX, tmpAreaY);
	}

}

void Game::InfomationUpdate() {
	info.id				= Player::stage;
	info.isEncount		= true;
	info.magicAverage	= 1000;
}

StageMode Game::GetStageMode()
{
	return mode;
}

// �ʏ�퓬�J�n
void Game::Encounter(int encounterID, int back, int battleBGM) {
	encounter.id = encounterID;			// �G���J�E���gID
	encounter.back = back;				// �w�iID
	encounter.escapeFlag = true;		// �����\�t���O
	encounter.loseEventFlag = false;	// �����C�x�t���O
	encounter.battleFlag = NULL;		// �퓬���t���O�i�󂯓n���p�j
	encounter.battleEndType = NULL;		// �퓬�I���̏�ԁi�󂯓n���p�j
	encounter.battleBGM = battleBGM;	// �퓬BGM

	// ��
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);

	PlaySoundMem(battleBGM, true);	// �Đ�

	waitBattle = true;
}
// �C�x���g�퓬�J�n
void Game::Encounter(int encounterID, int back, int battleBGM, bool escapeFlag, bool loseEventFlag, bool* battleFlag, eEndStatus* battleEndType) {
	encounter.id = encounterID;					// �G���J�E���gID
	encounter.back = back;						// �w�iID
	encounter.escapeFlag = escapeFlag;			// �����\�t���O
	encounter.loseEventFlag = loseEventFlag;	// �����C�x�t���O
	encounter.battleFlag = battleFlag;			// �퓬���t���O�i�󂯓n���p�j
	encounter.battleEndType = battleEndType;	// �퓬�I���̏�ԁi�󂯓n���p�j
	encounter.battleBGM = battleBGM;			// �퓬BGM

	// ��
	StopSoundMem(fieldBGM);
	DeleteSoundMem(fieldBGM);

	PlaySoundMem(battleBGM, true);	// �Đ�

	waitBattle = true;
}

void Game::EncounterReset() {
	encounter.id = -1;					// �G���J�E���gID
	encounter.back = 0;					// �w�iID
	encounter.escapeFlag = true;		// �����\�t���O
	encounter.loseEventFlag = false;	// �����C�x�t���O
	encounter.battleFlag = NULL;		// �퓬���t���O�i�󂯓n���p�j
	encounter.battleEndType = NULL;		// �퓬�I���̏�ԁi�󂯓n���p�j
}

void Game::TmpSave_UnitMode(){
	Player::TmpSave_UnitMode(Player::stage, unitData);
}
