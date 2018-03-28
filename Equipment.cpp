#include "Equipment.h"

// 装備
vector<vector<string> > EquipmentItem::data;
map<int, int>			EquipmentItem::arrayid;

void EquipmentItem::InitEquipmentItem() {
	ifstream ifs("equipment\\data.csv");
	if (!ifs) {
		return;	// エラー
	}

	int i = 0;
	string str;
	while (getline(ifs, str)) {
		data.resize(i + 1);
		istringstream stream(str);
		string token;
		while (getline(stream, token, ',')) {
			data[i].push_back(token);
		}
		arrayid.insert(make_pair(boost::lexical_cast<int>(data.back()[0]), i));

		++i;
	}

	ifs.close();
}
EquipmentItem::EquipmentItem(int id) {
	this->id				= id;
	name					= data[arrayid[id]][1];
	correction.maxHP		= boost::lexical_cast<int>(data[arrayid[id]][2]);
	correction.maxMP		= boost::lexical_cast<int>(data[arrayid[id]][3]);
	correction.STR			= boost::lexical_cast<int>(data[arrayid[id]][4]);
	correction.DEX			= boost::lexical_cast<int>(data[arrayid[id]][5]);
	correction.AGI			= boost::lexical_cast<int>(data[arrayid[id]][6]);
	correction.LUC			= boost::lexical_cast<int>(data[arrayid[id]][7]);
	correction.DEF_PHYS		= boost::lexical_cast<int>(data[arrayid[id]][8]);
	correction.DEF_HEAT		= boost::lexical_cast<int>(data[arrayid[id]][9]);
	correction.DEF_COLD		= boost::lexical_cast<int>(data[arrayid[id]][10]);
	correction.DEF_ELEC		= boost::lexical_cast<int>(data[arrayid[id]][11]);
	type					= (WeaponType)boost::lexical_cast<int>(data[arrayid[id]][12]);
}
EquipmentItem::~EquipmentItem() {
	// 特になし
}
int EquipmentItem::GetID() {
	return id;
}
WeaponType EquipmentItem::GetType() {
	return type;
}
string EquipmentItem::GetName() {
	return name;
}
BattleStatus EquipmentItem::GetCorrection() {
	return correction;
}

BattleStatus EquipmentItem::GetCorrection(int id) {
	// データベース参照用
	BattleStatus b;
	b.maxHP		= boost::lexical_cast<int>(data[arrayid[id]][2]);
	b.maxMP		= boost::lexical_cast<int>(data[arrayid[id]][3]);
	b.STR		= boost::lexical_cast<int>(data[arrayid[id]][4]);
	b.DEX		= boost::lexical_cast<int>(data[arrayid[id]][5]);
	b.AGI		= boost::lexical_cast<int>(data[arrayid[id]][6]);
	b.LUC		= boost::lexical_cast<int>(data[arrayid[id]][7]);
	b.DEF_PHYS	= boost::lexical_cast<int>(data[arrayid[id]][8]);
	b.DEF_HEAT	= boost::lexical_cast<int>(data[arrayid[id]][9]);
	b.DEF_COLD	= boost::lexical_cast<int>(data[arrayid[id]][10]);
	b.DEF_ELEC	= boost::lexical_cast<int>(data[arrayid[id]][11]);

	return b;
}