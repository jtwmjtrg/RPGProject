#pragma once
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include "BattleStatus.h"
#include <boost/lexical_cast.hpp>

using namespace std;

typedef enum eWeaponType {
	LIMITLESS = -1,	// �����Ȃ�
	FIST = 0,		// ��
	SWORD,			// ��
	SPEAR,			// ��
	AX,				// ��
	WAND,			// ��
	ARMOR,			// �Z
	AMULET			// �����i
}WeaponType;

class EquipmentItem {
private:
	static vector<vector<string> > data;	// �f�[�^�x�[�X
	static map<int, int> arrayid;			// id->�z��ԍ�

	int			id;		// �Ǘ��ԍ�
	WeaponType	type;	// ����̎��
	string		name;	// ���O
	BattleStatus correction;	// �����̕␳�l

public:
	static void InitEquipmentItem();	// ������

	// �R���X�g���N�^���f�X�g���N�^
	EquipmentItem(int id);
	~EquipmentItem();

	int					GetID();
	string				GetName();
	BattleStatus		GetCorrection();
	static BattleStatus GetCorrection(int id);

	WeaponType GetType();
};