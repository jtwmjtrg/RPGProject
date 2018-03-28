#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Item.h"
#include "Player.h"

class Shop {
private:
	static vector<Item*>	item;		// ���蕨
	static ItemBox			forSale;	// ���蕨
	static vector<BattleStatus>		bs;	// �X�e�[�^�X
	static vector<vector<BattleStatus> >	player_bs;	// �v���C���[�̑����̃o�g���X�e�[�^�X
	static string			greeting;		// ��������
	static string			confirmation;	// �m�F
	static string			who;			// �N�H

public:
	static ItemBox					decideMenu;	// �m�F���j���[
	static ItemBox					charaMenu;	// �L�����N�^�[���j���[

	// �R���X�g���N�^���f�X�g���N�^
	Shop();
	~Shop();

	static void InitShop(int id);	// ���i���̏�����
	static void InitPlayerBattleStatus();

	void Process();	// �����n
	void Draw();	// �`�ʌn
};