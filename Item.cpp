#include "Item.h"

// �A�C�e��
map<int, int>			Item::arrayid;
vector<vector<string> > Item::data;

Item::Item(int id){
	this->id = id;	// ���ʔԍ�
	// �f�[�^�x�[�X���Q�Ƃ��ď���������
	name	= data[arrayid[id]][1];								// ���O
	info	= data[arrayid[id]][2];								// ������
	price	= boost::lexical_cast<int>(data[arrayid[id]][3]);	// �l�i

	equipmentType	= EquipmentType::NOT_EQUIPMENT;		// �����A�C�e���ł͂Ȃ�
	isEquipment		= false;							// �������Ă��Ȃ�
	isArea			= false;							// ���ʔ͈͂��P�̂��S�̂�
	if (boost::lexical_cast<int>(data[arrayid[id]].back()) == 0) {
		// �X�L���Ȃ�
		skill = nullptr;
	}
	else {
		// �퓬���Ɏg�p�����ۂ̌���
		skill = new Skill(boost::lexical_cast<int>(data[arrayid[id]].back()));
	}

	isConsumption	= true;	// ����A�C�e�����ǂ���
}
Item::~Item(){
	delete skill;
}

void Item::InitItem() {
	ifstream ifs("item\\data.csv");
	if (!ifs) {
		return;	// �G���[
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
	Item* ip = nullptr;	// �����񃊃^�[�������̂����

	if ((int)data.size() <= id) {
		// ���݂��Ȃ��A�C�e��

		return ip;
	}

	if (data[arrayid[id]][4] == "important") {
		// �d�v�A�C�e��
		ip = new ImportantItem(id);
		ip->type = ItemType::IMPORTANT;
	}
	else if (data[arrayid[id]][4] == "recovery") {
		// �񕜃A�C�e��
		ip			= new RecoveryItem(id);
		ip->type	= ItemType::RECOVERY;
	}
	else if (data[arrayid[id]][4] == "damage") {
		// �U���A�C�e��
		ip			= new DamageItem(id);
		ip->type	= ItemType::DAMAGE;
	}
	else if (data[arrayid[id]][4] == "equipment") {
		// �����A�C�e��
		ip			= new CallEquipmentItem(id);
		ip->type	= ItemType::EQUIPMENT;
	}
	else if (data[arrayid[id]][4] == "lifesteal") {
		// ���C�t�X�e�B�[���A�C�e��
		ip			= new LifeStealItem(id);
		ip->type	= ItemType::LIFESTEAL;
	}
	else {
		// �G���[
	}

	return ip;
}

ImportantItem::ImportantItem(int id) :Item(id) {
	// myData = 0;	// ����Ȃ�
}
ImportantItem::~ImportantItem() {

}
vector<int>	ImportantItem::GetMyData() {
	return myData;
}

RecoveryItem::RecoveryItem(int id) :Item(id) {
	// �f�[�^�x�[�X���Q�Ƃ��ď���������
	recovery_value	= boost::lexical_cast<int>(data[arrayid[id]][5]);						// ���ʗ�
	isArea			= (boost::lexical_cast<int>(data[arrayid[id]][6]) == 1) ? true : false;	// ���ʂ��S�̂��P�̂�

	myData.push_back(recovery_value);
	myData.push_back(isArea);
}
RecoveryItem::~RecoveryItem() {
	// ���ɂȂ�
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
	// �f�[�^�x�[�X���Q�Ƃ��ď���������
	damage_value = boost::lexical_cast<int>(data[arrayid[ID]][5]);	// ���ʗ�

	myData.push_back(damage_value);
}
DamageItem::~DamageItem(){
	// ���ɂȂ�
}
void DamageItem::Effect(){
	// �g��Ȃ�
}
void DamageItem::Effect(Character* character){
	character->Damage(damage_value);
}
vector<int>	DamageItem::GetMyData() {
	return myData;
}

CallEquipmentItem::CallEquipmentItem(int id) :Item(id) {
	callID = boost::lexical_cast<int>(data[arrayid[id]][5]);	// �Ăяo��������ID
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
		// �G���[
	}
	isConsumption = false;	// ����A�C�e���łȂ�

	myData.push_back(callID);
	myData.push_back((int)equipmentType);
}
CallEquipmentItem::~CallEquipmentItem() {
	// ���ɂȂ�
}
void CallEquipmentItem::Effect() {
	// ���ɂȂ�
}
void CallEquipmentItem::Effect(Character* pc) {
	if (isEquipment) {
		// �������O��
		switch (equipmentType) {
		case EquipmentType::NOT_EQUIPMENT:
			// �����łȂ�
			break;

		case EquipmentType::WEAPON:
			// ����
			pc->EquipmentWeapon(new EquipmentItem(0));	// ��������
			break;

		case EquipmentType::ARMOR:
			// �h��
			pc->EquipmentArmor(new EquipmentItem(0));	// ��������
			break;

		case EquipmentType::AMULET:
			// ���̑�
			pc->EquipmentAmulet(new EquipmentItem(0));	// ��������
			break;

		default:
			// �e�N���X�ŏ��������Ă��邽�߂��肦�Ȃ�
			break;
		}
	}
	else {
		// ��������
		switch (equipmentType) {
		case EquipmentType::NOT_EQUIPMENT:
			// �����łȂ�
			break;

		case EquipmentType::WEAPON:
			// ����
			pc->EquipmentWeapon(new EquipmentItem(callID));	// ��������
			break;

		case EquipmentType::ARMOR:
			// �h��
			pc->EquipmentArmor(new EquipmentItem(callID));	// ��������
			break;

		case EquipmentType::AMULET:
			// ���̑�
			pc->EquipmentAmulet(new EquipmentItem(callID));	// ��������
			break;

		default:
			// �e�N���X�ŏ��������Ă��邽�߂��肦�Ȃ�
			break;
		}
	}
		
	isEquipment = !isEquipment;	// �t���O���]
}
vector<int>	CallEquipmentItem::GetMyData() {
	return myData;
}

LifeStealItem::LifeStealItem(int id) :Item(id) {
	damage	= boost::lexical_cast<int>(data[arrayid[id]][5]);	// �_���[�W��
	recovery= boost::lexical_cast<int>(data[arrayid[id]][6]);	// �񕜗�

	myData.push_back(damage);
	myData.push_back(recovery);
}
LifeStealItem::~LifeStealItem() {
	// ���ɂȂ�
}
void LifeStealItem::Effect(Character* target,Character* user) {
	target->Damage(damage);		// HP�����炷
	user->Recovery(recovery);	// �񕜂���
}
vector<int>	LifeStealItem::GetMyData() {
	return myData;
}