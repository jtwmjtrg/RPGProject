#include "Event.h"

// �ً}�f�o�b�O
namespace emergency {
	vector<vector<string> > eventdata;	// �C�x���g�f�[�^
	int eventnum;			// �C�x���g�̐�
	bool isPerformance;		// 
	int head;				// �C�x���g���̐i�s�x
}
using namespace emergency;

EventManager::EventManager(){
	isPerformance = false;
}
EventManager::~EventManager(){
	
}

void EventManager::Process(){
	for (int i = stoi(eventdata[0][0]), j = 0;j < eventnum;++i, ++j) {
		if (Player::isFinish[i]) {
			// �����ς�
			head = 2;
			isPerformance = false;
		}
		else {
			// ���߂ĖK�ꂽ�Ƃ�
			if (eventdata[j][1] == "FIRST")
			{
				EventProcess(j);
				isPerformance = true;
				break;
			}
		}
	}
}
void EventManager::Draw(){
	
}

void EventManager::LoadEvent()
{
	// �t�@�C���I�[�v��
	ifstream ifs;
	ifs.open("eventdata\\" + to_string(Player::stage) + ".csv");
	// �t�@�C�����Ȃ��ꍇ
	if (!ifs){
		return;
	}

	int i = 0;	// ���[�v�񐔂��J�E���g
	string str;	// ���͂��ꂽ������(�s)��ۑ�

	// 1��擾
	while (getline(ifs, str)){
		eventdata.resize(i + 1);	// �������m��

		stringstream stream(str);	// str���X�g���[���Ƃ��Ď���
		string token;				// ���͂��ꂽ������(�J���}��؂�)��ۑ�

		// �J���}��؂�Ńx�N�^�[�֊i�[����
		while (getline(stream, token, ',')){
			eventdata[i].push_back(token);		// token�𐔒l�ɕϊ����Ė����ɑ}��
		}

		++i;		// �J�E���g��i�߂�
	}

	// �t�@�C���N���[�Y
	ifs.close();

	eventnum = eventdata.size();
	head = 2;
}

void EventManager::EventProcess(int id) {
	// �ړ�
	if (eventdata[id][head] == "WARP") {
		Player::X = stoi(eventdata[id][head + 1]);
		Player::Y = stoi(eventdata[id][head + 2]);
		head += 3;
	}
	else if (eventdata[id][head] == "MOVE") {
		// X���W
		if (Player::X < stoi(eventdata[id][head + 1])) {
			Player::X += 8;
		}
		else if (Player::X > stoi(eventdata[id][head + 1])) {
			Player::X -= 8;
		}
		// Y���W
		if (Player::Y < stoi(eventdata[id][head + 2])) {
			Player::Y += 8;
		}
		else // Y���W
		if (Player::Y > stoi(eventdata[id][head + 2])) {
			Player::Y -= 8;
		}

		if (Player::X == stoi(eventdata[id][head + 1]) && Player::Y == stoi(eventdata[id][head + 2])) {
			head += 3;
		}
	}
	else if (eventdata[id][head] == "TALK") {
		TextBox::AddLine(eventdata[id][head + 1]);
		head += 2;
	}

	if (head == eventdata[id].size()) {
		Player::isFinish[0] = true;
	}
}

bool EventManager::GetEvent() {
	return isPerformance;
}