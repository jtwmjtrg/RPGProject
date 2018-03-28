#pragma once
#include <fstream>
#include <map>
#include <sstream>
#include "Character.h"
#include "Skill.h"

using namespace std;

class Character;

// �A�C�e���^�C�v
typedef enum class eItemType {
	IMPORTANT, RECOVERY, DAMAGE, EQUIPMENT, LIFESTEAL
}ItemType;
typedef enum class eEquipmentType {
	NOT_EQUIPMENT, WEAPON, ARMOR, AMULET
}EquipmentType;

class Item	// ���ۃN���X
{
protected:
	// �X�e�[�^�X
	int		id;				// ���ʔԍ�
	string	name;			// ���O
	string	info;			// ����
	int		price;			// �l�i
	bool	isArea;			// �Ώۂ��S�̂��ǂ���
	Skill*	skill;			// �퓬���Ɏg�p����ۂ̌���
	bool	isConsumption;	// ����A�C�e�����ǂ���
	vector<int>	myData;		// ���̃A�C�e���̃f�[�^

	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	Item(int id);

	static map<int, int>			arrayid;	// id->�z��ԍ�
	static vector<vector<string> >	data;		// �A�C�e���̃f�[�^�x�[�X
public:
	ItemType		type;			// ���A�C�e����
	EquipmentType	equipmentType;	// �����̑���
	bool			isEquipment;	// ���������ǂ���

	static void		InitItem();		// �f�[�^�x�[�X�փA�C�e���f�[�^����荞��
	virtual			~Item();		// �f�X�g���N�^

	// �t�B�[���h�p
	virtual void	Effect() {};									// �Ώۂ����Ȃ�����
	virtual void	Effect(Character* character) {}					// �Ώۂ�������
	virtual void	Effect(vector<Character*> character) {}			// �S�̌���
	virtual void	Effect(Character* target, Character* user) {}	// �Ώۂ����A�����ɂ����ʂ�����A�C�e��
	// �o�g���p
	virtual void	BattleEffect() {};										// �Ώۂ����Ȃ�����
	virtual void	BattleEffect(Character* character) {};					// �Ώۂ�������
	virtual void	BattleEffect(vector<Character*> character) {};			// �S�̌���
	virtual void	BattleEffect(Character* target, Character* user) {};	// �Ώۂ����A�����ɂ����ʂ�����A�C�e��

	// �Q�b�^�[
	int		 GetID();
	string	GetName();
	static string GetName(int itemID);
	string	GetInfo();
	int		GetPrice();
	bool	GetIsArea();
	Skill*	GetSkill();
	bool	GetIsCunsumption();
	virtual vector<int>	GetMyData() = 0;

	// �t�@�N�g��
	static Item* Factory(int id);
};
class ImportantItem :public Item {
public:
	ImportantItem(int id);
	~ImportantItem();

	vector<int> GetMyData();
};
class RecoveryItem :public Item	// �񕜃A�C�e���N���X
{
private:
	// �X�e�[�^�X
	int  recovery_value;	// �񕜗�

public:
	// �R���X�g���N�^���f�X�g���N�^
	RecoveryItem(int id);
	~RecoveryItem();

	// ���ʂ��L�q����镔��
	// �t�B�[���h�p
	void Effect(Character* character);			// �Ώۂ�������
	void Effect(vector<Character*> character);	// �S�̌���
	// �o�g���p
	void BattleEffect(Character* character);			// �Ώۂ�������
	void BattleEffect(vector<Character*> character);	// �S�̌���

	vector<int> GetMyData();
};
class DamageItem :public Item	// �_���[�W�\�[�X�A�C�e���N���X
{
private:
	// �X�e�[�^�X
	int damage_value;		// �_���[�W��

public:
	// �R���X�g���N�^���f�X�g���N�^
	DamageItem(int id);
	~DamageItem();

	// ���ʂ��L�q����镔��
	void Effect();
	void Effect(Character* charactor);

	vector<int> GetMyData();

};
class CallEquipmentItem :public Item {
private:
	int  callID;		// �����A�C�e���N���X��id

public:
	// �R���X�g���N�^���f�X�g���N�^
	CallEquipmentItem(int id);
	~CallEquipmentItem();

	void Effect();
	void Effect(Character* pc);	// ����������

	vector<int> GetMyData();
};
class LifeStealItem :public Item {
private:
	int damage;
	int recovery;

public:
	// �R���X�g���N�^���f�X�g���N�^
	LifeStealItem(int id);
	~LifeStealItem();

	void Effect(Character* target, Character* user);	// �Ώۂ����A�����ɂ����ʂ�����A�C�e��

	vector<int> GetMyData();
};