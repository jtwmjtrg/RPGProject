#include "Title.h"

Title::Title(){
	// �摜
	backgroundGraph = LoadGraph("img\\title\\background.png");	// �w�i
	
	select = 0;
	selectNum = 3;
}
Title::~Title(){
	// �摜�폜
	DeleteGraph(backgroundGraph);	// �w�i
}

void Title::Process(){
	// �J�[�\�������
	if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1) {
		select = (select + (selectNum - 1)) % this->selectNum;
	}
	// �J�[�\��������
	if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1) {
		select = (select + 1) % selectNum;
	}
	// ����
	if (Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) {
		switch (select) {
		case 0:// �y��������z
			Player::InitPlayer(true);		// �v���C���[�̏�����
			nowScene = GameScene::STAGE;	// �Q�[���V�[����FIELD��
			break;
		case 1:// �y�͂��߂���z
			Player::InitPlayer(false);		// �v���C���[�̏�����
			InitUnitMode();					// �}�b�v���j�b�g�̃��[�h�̏�����
			nowScene = GameScene::STAGE;	// �Q�[���V�[����FIELD��
			break;
		case 2:// �y��߂�z
			nowScene = GameScene::END;		// �Q�[���I��
			break;
		default:
			break;
		}
	}
}

void Title::Draw(){
	DrawGraph(0, 0, backgroundGraph);		// �w�i
		
	DrawString(WHITE, 250, 700 + select * 64, "��");
	DrawString(WHITE, 300, 700, "��������");
	DrawString(WHITE, 300, 764, "�͂��߂���");
	DrawString(WHITE, 300, 828, "�Q�[������߂�");
}

// �}�b�v���j�b�g�̃��[�h�̏�����
void Title::InitUnitMode() {
	HANDLE hFind;	// �n���h��	
	WIN32_FIND_DATA fileData;// �t�@�C�����̍\����(����Ƀt�H���_��������)
	std::vector<std::string> folders;	// �t�H���_���X�gfolder

	//��������f�B���N�g���̎w��
	std::string search_name = "unitdata\\*";

	hFind = FindFirstFile(search_name.c_str(), &fileData);	// �����n���h���̎擾

	// �����ł��Ȃ���΃G���[��Ԃ�
	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
	}

	do {
		// �t�@�C������"."��".."�łȂ���Ίi�[
		folders.push_back(fileData.cFileName);	// �t�@�C�����i�[
		if (folders.back() == "." || folders.back() == "..") {
			folders.pop_back();	// �폜
		}
	} while (FindNextFile(hFind, &fileData));	// ���̃t�@�C��������

	FindClose(hFind);	// �����I��

	std::map<int, int> unitMode;	// �t�@�C���̃f�[�^�iID,�������[�h�j

	// ���j�b�g�̃��[�h�̃Z�[�u�f�[�^��S�ď���������
	for (int i = 0, n = folders.size(); i < n; ++i) {
		// ���j�b�g���[�h�������t�@�C�����J��
		std::ifstream lFile("unitdata\\" + folders[i] + "\\InitMode.csv");
		if (lFile.fail()) {
			// �ǂݍ��ݎ��s
		}
		else {
			// �ǂݍ��ݐ���

			// �������f�[�^�̓ǂݍ���
			unitMode.clear();
			std::string line;
			std::string token;
			int id = 0;	// ID
			while (getline(lFile, line)) {	// ��s���ǂݍ���
				std::istringstream stream(line);
				getline(stream, token, ',');
				id = boost::lexical_cast<int>(token);
				getline(stream, token, ',');
				unitMode[id] = boost::lexical_cast<int>(token);
			}
			
			// ���j�b�g���[�h�̃Z�[�u�t�@�C�����㏑��
			ofstream ofs("unitdata\\" + folders[i] + "\\SaveMode.csv");
			for (auto itr = unitMode.begin(), end = unitMode.end(); itr != end; ++itr) {
				ofs << itr->first << ',' << itr->second << endl;		// ID, ���[�h
			}
			ofs.close();
		}
		// �t�@�C������� 
		lFile.close();
	}

}