#pragma once
#include <vector>
#include <string>
#include "Define.h"
#include "graphicdata.h"
#include "TextBox.h"

// �G���[�g
class Emote {
private:
	static bool isCreate;					// �������ꂽ���ǂ����̃t���O

	static int emote_num_;					// �G���[�g�̑���

	static std::vector<int> time_;			// graph_handle�ƓY���Ή��Btime_[x]��graph_handle_[x]�̃G���[�g���Ԃ�����
	static std::vector<int> graph_handle_;	// �摜�n���h��

	typedef struct _EmoteInfo {
		// �G���[�g�̏��

		int id_;			// id
		Position2D pos_;	// ���W
		int count_;			// �o�ߎ���
	}EmoteInfo;

	static std::vector<EmoteInfo> playing_emote_;	// �i�s���̃G���[�g

public:
	Emote();
	~Emote();

	// �����������p
	static void Create();
	static void Destroy();

	// �X�V�������`��
	static void Process();
	static void Draw();

	// �G���[�g�Z�b�g
	static void SetEmote(int id, Position2D pos);
	static void SetEmote(int id, int X, int Y);

	static int GetEmoteSize();
	static int GetEmoteId(int i);
};
