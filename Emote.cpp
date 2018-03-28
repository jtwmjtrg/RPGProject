#include "Emote.h"

bool Emote::isCreate;									// �������ꂽ���ǂ���
int Emote::emote_num_;									// �G���[�g�̑���
std::vector<int> Emote::time_;							// �o�ߎ���
std::vector<int> Emote::graph_handle_;					// �摜�n���h��
std::vector<Emote::EmoteInfo> Emote::playing_emote_;	// �i�s���̃G���[�g

Emote::Emote() {

}
Emote::~Emote() {

}

void Emote::Create() {
	// ������
	
	emote_num_ = 10;	// ���͓K��

	for (int i = 0;i < emote_num_;++i) {
		// �摜�ǂݍ���
		
		string str = "img\\emote\\" + std::to_string(i) + ".png";	// �A�h���X����
		char* cstr = new char[str.size() + 1];						// �������m��
		strcpy_s(cstr, str.size() + 1, str.c_str());				// �R�s�[
		graph_handle_.push_back(LoadGraph(cstr));					// �摜���[�h
		delete cstr;

		// ����

		time_.push_back(30);
	}

	isCreate = true;	// ��������
}
void Emote::Destroy() {
	// ���p

	for (int i = 0;i < emote_num_;++i) {
		// �摜�j��
		DeleteGraph(graph_handle_[i]);
	}

	// ���������
	graph_handle_.clear();
	graph_handle_.shrink_to_fit();
	time_.clear();
	time_.shrink_to_fit();
	playing_emote_.clear();
	playing_emote_.shrink_to_fit();
}

void Emote::Process() {
	// �X�V����

	if (!isCreate) {
		return;
	}

	for (int i = 0;i < playing_emote_.size();++i) {
		// �X�^�b�N�ɂ���G���[�g���X�V����

		if (playing_emote_[i].count_ + 1 >= time_[playing_emote_[i].id_]) {
			// �I��

			playing_emote_.erase(playing_emote_.begin() + i);	// �I�������G���[�g���X�^�b�N����폜����
			i--;
		}
		else {
			// �X�V

			playing_emote_[i].count_++;	// �J�E���g��i�߂�
		}
	}
}
void Emote::Draw() {
	// �`�揈��

	if (!isCreate) {
		return;
	}

	for (int i = 0, n = playing_emote_.size();i < n;++i) {
		// �X�^�b�N�ɂ���G���[�g��`��X�V����

		DrawGraph(playing_emote_[i].pos_.X, playing_emote_[i].pos_.Y, graph_handle_[playing_emote_[i].id_]);
	}
}

void Emote::SetEmote(int id, Position2D pos) {
	// �G���[�g�̗\��

	if (!isCreate) {
		return;
	}

	EmoteInfo emote;
	emote.id_ = id;
	emote.pos_ = pos;
	emote.count_ = 0;
	playing_emote_.push_back(emote);
}
void Emote::SetEmote(int id, int X, int Y) {
	// �G���[�g�̗\��

	if (!isCreate) {
		return;
	}

	EmoteInfo emote;
	emote.id_ = id;
	emote.pos_.X = X;
	emote.pos_.Y = Y;
	emote.count_ = 0;
	playing_emote_.push_back(emote);
}

int Emote::GetEmoteSize() {
	return playing_emote_.size();
}
int Emote::GetEmoteId(int i) {
	if (i >= playing_emote_.size()) {
		return -1;
	}

	return playing_emote_[i].id_;
}