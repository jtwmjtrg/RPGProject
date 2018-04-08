#include "Item.h"

// アイテム
map<int, int>			Item::arrayid;
vector<vector<string> > Item::data;

Item::Item(int id){
	this->id = id;	// 識別番号
	// データベースを参照して初期化する
	name	= data[arrayid[id]][1];								// 名前
	info	= data[arrayid[id]][2];								// 説明文
	price	= boost::lexical_cast<int>(data[arrayid[id]][3]);	// 値段

	equipmentType	= EquipmentType::NOT_EQUIPMENT;		// 装備アイテムではない
	isEquipment		= false;							// 装備していない
	isArea			= false;							// 効果範囲が単体か全体か
	if (boost::lexical_cast<int>(data[arrayid[id]].back()) == 0) {
		// スキルなし
		skill = nullptr;
	}
	else {
		// 戦闘中に使用した際の効果
		skill = new Skill(boost::lexical_cast<int>(data[arrayid[id]].back()));
	}

	isConsumption	= true;	// 消費アイテムかどうか
}
Item::~Item(){
	delete skill;
}

void Item::InitItem() {
	ifstream ifs("item\\data.csv");
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

int		Item::GetID() {
	return id;
}
string	Item::GetName(){
	return name;
}
string	Item::GetName(int itemID) {
	return data[arrayid[itemID]][1];
}
string	Item::GetInfo(){
	return info;
}
int		Item::GetPrice() {
	return price;
}
bool	Item::GetIsArea() {
	return isArea;
}
Skill*	Item::GetSkill() {
	return skill;
}
bool	Item::GetIsCunsumption() {
	return isConsumption;
}

Item* Item::Factory(int id) {
	Item* ip = nullptr;	// 複数回リターン書くのを回避

	if ((int)data.size() <= id) {
		// 存在しないアイテム

		return ip;
	}

	if (data[arrayid[id]][4] == "important") {
		// 重要アイテム
		ip = new ImportantItem(id);
		ip->type = ItemType::IMPORTANT;
	}
	else if (data[arrayid[id]][4] == "recovery") {
		// 回復アイテム
		ip			= new RecoveryItem(id);
		ip->type	= ItemType::RECOVERY;
	}
	else if (data[arrayid[id]][4] == "damage") {
		// 攻撃アイテム
		ip			= new DamageItem(id);
		ip->type	= ItemType::DAMAGE;
	}
	else if (data[arrayid[id]][4] == "equipment") {
		// 装備アイテム
		ip			= new CallEquipmentItem(id);
		ip->type	= ItemType::EQUIPMENT;
	}
	else if (data[arrayid[id]][4] == "lifesteal") {
		// ライフスティールアイテム
		ip			= new LifeStealItem(id);
		ip->type	= ItemType::LIFESTEAL;
	}
	else {
		// エラー
	}

	return ip;
}

ImportantItem::ImportantItem(int id) :Item(id) {
	// myData = 0;	// 現状ない
}
ImportantItem::~ImportantItem() {

}
vector<int>	ImportantItem::GetMyData() {
	return myData;
}

RecoveryItem::RecoveryItem(int id) :Item(id) {
	// データベースを参照して初期化する
	recovery_value	= boost::lexical_cast<int>(data[arrayid[id]][5]);						// 効果量
	isArea			= (boost::lexical_cast<int>(data[arrayid[id]][6]) == 1) ? true : false;	// 効果が全体か単体か

	myData.push_back(recovery_value);
	myData.push_back(isArea);
}
RecoveryItem::~RecoveryItem() {
	// 特になし
}
void RecoveryItem::Effect(Character* character){
	character->Recovery(recovery_value);
}
void RecoveryItem::Effect(vector<Character*> character) {
	for (int i = 0, n = character.size();i < n;++i) {
		character[i]->Recovery(recovery_value);
	}
}
void RecoveryItem::BattleEffect(Character* character) {
	character->Recovery(recovery_value);
}
void RecoveryItem::BattleEffect(vector<Character*> character) {
	for (int i = 0, n = character.size(); i < n; ++i) {
		character[i]->Recovery(recovery_value);
	}
}
vector<int>	RecoveryItem::GetMyData() {
	return myData;
}

DamageItem::DamageItem(int ID) :Item(ID) {
	// データベースを参照して初期化する
	damage_value = boost::lexical_cast<int>(data[arrayid[ID]][5]);	// 効果量

	myData.push_back(damage_value);
}
DamageItem::~DamageItem(){
	// 特になし
}
void DamageItem::Effect(){
	// 使わない
}
void DamageItem::Effect(Character* character){
	character->Damage(damage_value);
}
vector<int>	DamageItem::GetMyData() {
	return myData;
}

CallEquipmentItem::CallEquipmentItem(int id) :Item(id) {
	callID = boost::lexical_cast<int>(data[arrayid[id]][5]);	// 呼び出す装備のID
	if (data[arrayid[id]][6] == "weapon") {
		equipmentType = EquipmentType::WEAPON;
	}
	else if (data[arrayid[id]][6] == "armor") {
		equipmentType = EquipmentType::ARMOR;
	}
	else if (data[arrayid[id]][6] == "amulet") {
		equipmentType = EquipmentType::AMULET;
	}
	else {
		// エラー
	}
	isConsumption = false;	// 消費アイテムでない

	myData.push_back(callID);
	myData.push_back((int)equipmentType);
}
CallEquipmentItem::~CallEquipmentItem() {
	// 特になし
}
void CallEquipmentItem::Effect() {
	// 特になし
}
void CallEquipmentItem::Effect(Character* pc) {
	if (isEquipment) {
		// 装備を外す
		switch (equipmentType) {
		case EquipmentType::NOT_EQUIPMENT:
			// 装備でない
			break;

		case EquipmentType::WEAPON:
			// 武器
			pc->EquipmentWeapon(new EquipmentItem(0));	// 装備処理
			break;

		case EquipmentType::ARMOR:
			// 防具
			pc->EquipmentArmor(new EquipmentItem(0));	// 装備処理
			break;

		case EquipmentType::AMULET:
			// その他
			pc->EquipmentAmulet(new EquipmentItem(0));	// 装備処理
			break;

		default:
			// 親クラスで初期化しているためありえない
			break;
		}
	}
	else {
		// 装備する
		switch (equipmentType) {
		case EquipmentType::NOT_EQUIPMENT:
			// 装備でない
			break;

		case EquipmentType::WEAPON:
			// 武器
			pc->EquipmentWeapon(new EquipmentItem(callID));	// 装備処理
			break;

		case EquipmentType::ARMOR:
			// 防具
			pc->EquipmentArmor(new EquipmentItem(callID));	// 装備処理
			break;

		case EquipmentType::AMULET:
			// その他
			pc->EquipmentAmulet(new EquipmentItem(callID));	// 装備処理
			break;

		default:
			// 親クラスで初期化しているためありえない
			break;
		}
	}
		
	isEquipment = !isEquipment;	// フラグ反転
}
vector<int>	CallEquipmentItem::GetMyData() {
	return myData;
}

LifeStealItem::LifeStealItem(int id) :Item(id) {
	damage	= boost::lexical_cast<int>(data[arrayid[id]][5]);	// ダメージ量
	recovery= boost::lexical_cast<int>(data[arrayid[id]][6]);	// 回復量

	myData.push_back(damage);
	myData.push_back(recovery);
}
LifeStealItem::~LifeStealItem() {
	// 特になし
}
void LifeStealItem::Effect(Character* target,Character* user) {
	target->Damage(damage);		// HPを減らす
	user->Recovery(recovery);	// 回復する
}
vector<int>	LifeStealItem::GetMyData() {
	return myData;
}