#pragma once
#include "Character.h"
#include "ModeTerm.h"
#include "sounddata.h"

// ���S�����G��ID�ƍ��W
struct tEnemyGrave {
	int id;
	int x;
	int y;
};

class Enemy : public Character {
private:
	int gr;		// �摜
	int id;		// �GID
	int width;	// ����
	int height;	// �c��
	int x, y;	// ���W

	static int gr_HP;	// HP�o�[

	vector<ModeTerm*> modeTerm;		// �ۗL�X�L��
	unsigned int nowMode;			// ���݂̃��[�h


	// �y�험�i�z
	unsigned int exp;	// �o���l
	unsigned int money;	// ��
	int dropItem;	// �h���b�v�A�C�e��ID
	__int8 dropRate;	// �h���b�v��

public:
	Enemy(int x, int y, string name);
	Enemy(int x, int y, int id, unsigned int& turnNum);	// �R���X�g���N�^
	~Enemy();
	
	static void InitBattle();	// ����������
	static void FinaBattle();	// �I������

	void LoadData(const unsigned int& turnNum, const std::vector<Enemy*>& enemy);

	static vector<unsigned int> deathList;	// �G���S���X�g

	void Draw(int fieldX, int fieldY, int cellsize);	// �`��

	void DecisionSkill(vector<int> playerHate, vector<Enemy*> enemy);	// �����X�L���̌���

	void ModeChange(int modeNum);	// ���[�h�ύX

	void SetColision(vector<vector<Character*>>& collField);	// �����蔻��̍쐬
	void DeleteColision(vector<vector<Character*>>& collField);	// ���S����
	void Release();		// �I������

	bool GetCharType() { return false; }	// �L�����N�^�[�̃^�C�v�擾	�v���C���[�Ftrue	�G�Ffalse
	void GetEffectText(vector<string>& message);	// �X�L���̃��b�Z�[�W�擾
	int GetID();	// ID�擾
	int GetWidth();		// ����
	int GetHeight();	// �c��
	int GetX();		// X���W
	int GetY();		// Y���W
	

	// �험�i
	unsigned int GetEXP();		// �o���l
	unsigned int GetMoney();	// ��
	int GetDropItem();			// �h���b�v�A�C�e��
	__int8 GetDropRate();		// �h���b�v��

	void TestIni(string name);
};