#pragma once
#include <fstream>
#include <map>
#include <sstream>
#include "Character.h"
#include "Skill.h"

using namespace std;

class Character;

// アイテムタイプ
typedef enum class eItemType {
	IMPORTANT, RECOVERY, DAMAGE, EQUIPMENT, LIFESTEAL
}ItemType;
typedef enum class eEquipmentType {
	NOT_EQUIPMENT, WEAPON, ARMOR, AMULET
}EquipmentType;

class Item	// 抽象クラス
{
protected:
	// ステータス
	int		id;				// 識別番号
	string	name;			// 名前
	string	info;			// 説明
	int		price;			// 値段
	bool	isArea;			// 対象が全体かどうか
	Skill*	skill;			// 戦闘中に使用する際の効果
	bool	isConsumption;	// 消費アイテムかどうか
	vector<int>	myData;		// このアイテムのデータ

	// アクセス制限されたコンストラクタ
	Item(int id);

	static map<int, int>			arrayid;	// id->配列番号
	static vector<vector<string> >	data;		// アイテムのデータベース
public:
	ItemType		type;			// 何アイテムか
	EquipmentType	equipmentType;	// 装備の属性
	bool			isEquipment;	// 装備中かどうか

	static void		InitItem();		// データベースへアイテムデータを取り込む
	virtual			~Item();		// デストラクタ

	// フィールド用
	virtual void	Effect() {};									// 対象を取らない効果
	virtual void	Effect(Character* character) {}					// 対象を取る効果
	virtual void	Effect(vector<Character*> character) {}			// 全体効果
	virtual void	Effect(Character* target, Character* user) {}	// 対象を取り、自分にも効果があるアイテム
	// バトル用
	virtual void	BattleEffect() {};										// 対象を取らない効果
	virtual void	BattleEffect(Character* character) {};					// 対象を取る効果
	virtual void	BattleEffect(vector<Character*> character) {};			// 全体効果
	virtual void	BattleEffect(Character* target, Character* user) {};	// 対象を取り、自分にも効果があるアイテム

	// ゲッター
	int		 GetID();
	string	GetName();
	static string GetName(int itemID);
	string	GetInfo();
	int		GetPrice();
	bool	GetIsArea();
	Skill*	GetSkill();
	bool	GetIsCunsumption();
	virtual vector<int>	GetMyData() = 0;

	// ファクトリ
	static Item* Factory(int id);
};
class ImportantItem :public Item {
public:
	ImportantItem(int id);
	~ImportantItem();

	vector<int> GetMyData();
};
class RecoveryItem :public Item	// 回復アイテムクラス
{
private:
	// ステータス
	int  recovery_value;	// 回復量

public:
	// コンストラクタ＆デストラクタ
	RecoveryItem(int id);
	~RecoveryItem();

	// 効果が記述される部分
	// フィールド用
	void Effect(Character* character);			// 対象を取る効果
	void Effect(vector<Character*> character);	// 全体効果
	// バトル用
	void BattleEffect(Character* character);			// 対象を取る効果
	void BattleEffect(vector<Character*> character);	// 全体効果

	vector<int> GetMyData();
};
class DamageItem :public Item	// ダメージソースアイテムクラス
{
private:
	// ステータス
	int damage_value;		// ダメージ量

public:
	// コンストラクタ＆デストラクタ
	DamageItem(int id);
	~DamageItem();

	// 効果が記述される部分
	void Effect();
	void Effect(Character* charactor);

	vector<int> GetMyData();

};
class CallEquipmentItem :public Item {
private:
	int  callID;		// 装備アイテムクラスのid

public:
	// コンストラクタ＆デストラクタ
	CallEquipmentItem(int id);
	~CallEquipmentItem();

	void Effect();
	void Effect(Character* pc);	// 装備させる

	vector<int> GetMyData();
};
class LifeStealItem :public Item {
private:
	int damage;
	int recovery;

public:
	// コンストラクタ＆デストラクタ
	LifeStealItem(int id);
	~LifeStealItem();

	void Effect(Character* target, Character* user);	// 対象を取り、自分にも効果があるアイテム

	vector<int> GetMyData();
};