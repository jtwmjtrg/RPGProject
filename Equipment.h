#pragma once
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include "BattleStatus.h"
#include <boost/lexical_cast.hpp>

using namespace std;

typedef enum eWeaponType {
	LIMITLESS = -1,	// 制限なし
	FIST = 0,		// 拳
	SWORD,			// 剣
	SPEAR,			// 槍
	AX,				// 斧
	WAND,			// 杖
	ARMOR,			// 鎧
	AMULET			// 装飾品
}WeaponType;

class EquipmentItem {
private:
	static vector<vector<string> > data;	// データベース
	static map<int, int> arrayid;			// id->配列番号

	int			id;		// 管理番号
	WeaponType	type;	// 武器の種類
	string		name;	// 名前
	BattleStatus correction;	// 装備の補正値

public:
	static void InitEquipmentItem();	// 初期化

	// コンストラクタ＆デストラクタ
	EquipmentItem(int id);
	~EquipmentItem();

	int					GetID();
	string				GetName();
	BattleStatus		GetCorrection();
	static BattleStatus GetCorrection(int id);

	WeaponType GetType();
};