#pragma once
#include"Player.h"
#include "Shop.h"
#include <cstdlib>
#include <boost/algorithm/string/split.hpp>		// �X�v���b�g
#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string.hpp>

enum class GameScene : __int8;
class MapUnit_Mode;

// �����̏���
enum class eUnit_If {
	Talk,	// �b��������
	Touch,	// �G���
	Message,// ���b�Z�[�W����M����
	Always,// �펞
	Distance,	// ��l���Ƃ̋�������
};

// ����
enum class eUnit_Act {
	Warp,		// �]��
	Talk,		// �Z���t
	Battle,		// �퓬
	Damage,		// �_���[�W
	Recovery,	// ��
	Item,		// �A�C�e���擾
	Shop,		// �X
	Existence,	// ����-�o��
	ChangeMode,	// ���[�h�ύX
	Message,	// ���b�Z�[�W���M
	SelectBranch,// �I�𕪊�
	End,		// �����I��
	Goto,		// �����ԍ��̃W�����v
	GameOver,	// �����Q�[���I�[�o�[
	GameClear,	// �Q�[���N���A
	PayMoney,	// ���𕥂�
};

enum class eUnitStatus {
	Apear,		// �o��
	Disapear,	// ����
	SetColl,	// �����蔻���t����
	DeletColl	// �����蔻�������
};

/*-------------------------------------------------���j�b�g�̋����f�[�^--------------------------------------------*/

enum class eMUAReturn : __int8 {
	Next = 0,	// �����Ɏ��̋����ɂ���
	Stop = 1,	// �ꎞ��~
	End = -1,	// �����I��
};

// ���j�b�g�̋���
class MapUnit_Action {	// ���ۃN���X
protected:
	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	MapUnit_Action() {}

	int* actionNum;		// ���s���̍s���̗v�f�ԍ�

public:
	// �f�X�g���N�^
	virtual ~MapUnit_Action() {}

	// �����̎��s
	virtual eMUAReturn Execution() = 0;
	void SetActionNum(int* actionNum);		// ���s���̍s���̗v�f�ԍ��̃Z�b�^�[
};

// �y�]�ځz
class MapUnit_Action_Warp : public MapUnit_Action {
private:
	// �X�e�[�^�X
	int mapId;	// �]�ڐ�̃}�b�vID
	int x;		// �]�ڐ�̍��W
	int y;
	int effect;	// �]�ڃG�t�F�N�g�̎��

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Warp(string line);
	~MapUnit_Action_Warp();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�Z���t�z
class MapUnit_Action_Talk : public MapUnit_Action {
private:
	// �X�e�[�^�X
	std::vector<string> str;	// �Z���t���e

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Talk(string line);
	~MapUnit_Action_Talk();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�퓬�z
// �퓬�I���̏��
enum class eEndStatus : __int8 {
	Win,	// �G��S�ł�����
	Lose,	// �v���C���[���S�ł���
	Escape,	// ��������
};
class MapUnit_Action_Battle : public MapUnit_Action {
private:
	int encID;			// �G���J�E���gID
	int backID;			// �w�iID
	bool escapeFlag;	// �����\�t���O
	bool loseEventFlag;	// �����C�x�t���O
	std::array<int, 3> branch;	// �퓬�I���̏�Ԃŕ���i�����ԍ��̈ړ����j

	bool startFlag;		// �J�n�����t���O
	bool battleFlag;	// �퓬���t���O
	eEndStatus battleEndType;	// �퓬�I���̏��

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Battle(string line);
	~MapUnit_Action_Battle();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�_���[�W�z
class MapUnit_Action_Damage : public MapUnit_Action {
private:
	int damage;	// �_���[�W�ʁi�Œ�_���[�W�j
	int target;	// �Ώ�	0:�S��	1:�擪	2:�����_��

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Damage(string line);
	~MapUnit_Action_Damage();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�񕜁z
class MapUnit_Action_Recovery : public MapUnit_Action {
private:
	int heal_HP;		// HP�񕜗ʁi�Œ�ʁj	-1:�S��
	int heal_MP;		// MP�񕜗ʁi�Œ�ʁj	-1:�S��
	bool resurrection;	// �h���t���O
	int target;			// �Ώ�	0:�S��	1:�擪	2:�����_��

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Recovery(string line);
	~MapUnit_Action_Recovery();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�A�C�e���擾�z
class MapUnit_Action_Item : public MapUnit_Action {
private:
	int itemID;		// �擾�A�C�e����ID
	int jump_True;	// �A�C�e������ɓ��ꂽ�ꍇ�̃W�����v
	int jump_False;	// �������������ς��������ꍇ�̃W�����v

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Item(string line);
	~MapUnit_Action_Item();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�X�z
class MapUnit_Action_Shop : public MapUnit_Action {
private:
	string shopID;		// �XID 

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Shop(string line);
	~MapUnit_Action_Shop();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y����-�o���z
class MapUnit_Action_Existence : public MapUnit_Action {
private:
	bool flag;		// true::�o��	false:����
	int staging;	// ���o
	bool& appearFlag;	// �o���t���O

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Existence(string line, bool& appearFlag);
	~MapUnit_Action_Existence();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y���[�h�ύX�z
class MapUnit_Action_ChangeMode : public MapUnit_Action {
private:
	int modeNum;	// �ύX��̃��[�h��
	int& nowMode;	// �o���t���O
	const vector<MapUnit_Mode*>& mode;

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_ChangeMode(string line, int& nowMode, const vector<MapUnit_Mode*>& mode);
	~MapUnit_Action_ChangeMode();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y���b�Z�[�W���M�z
class MapUnit_Action_Message : public MapUnit_Action {
private:
	int messageNum;	// ���b�Z�[�W�ԍ�

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Message(string line);
	~MapUnit_Action_Message();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�I�𕪊�z
enum class eSelectBranch : __int8 {
	question,	// ����
	wait,		// �𓚑҂�
};
class MapUnit_Action_SelectBranch : public MapUnit_Action {
private:
	string question;	// ���╶
	std::vector<tSelectBranch> choices;	// �I����
	bool selectFlag;	// �I���t���O
	int select;			// �I��ԍ�
	eSelectBranch step;	// �i�K

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_SelectBranch(string line);
	~MapUnit_Action_SelectBranch();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�����I���z
class MapUnit_Action_End : public MapUnit_Action {
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_End();
	~MapUnit_Action_End();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�����ԍ��̃W�����v�z
class MapUnit_Action_Goto : public MapUnit_Action {
private:
	int moveNum;	// �ړ���

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_Goto(string line);
	~MapUnit_Action_Goto();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�����Q�[���I�[�o�[�z
class MapUnit_Action_GameOver : public MapUnit_Action {
private:
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_GameOver();
	~MapUnit_Action_GameOver();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};

// �y�Q�[���N���A�z
class MapUnit_Action_GameClear : public MapUnit_Action {
private:
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_GameClear();
	~MapUnit_Action_GameClear();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};
// �y���𕥂��z
class MapUnit_Action_PayMoney : public MapUnit_Action {
private:
	int price;		// �������z
	int jump_True;	// �������肽�ꍇ�̃W�����v
	int jump_False;	// ��������Ȃ������ꍇ�̃W�����v
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Action_PayMoney(string line);
	~MapUnit_Action_PayMoney();

	// �I�[�o�[���C�h
	eMUAReturn Execution();	// �����̎��s
};
/*-------------------------------------------------���j�b�g�̋��������f�[�^--------------------------------------------*/

// ���j�b�g�̋����̏���
class MapUnit_If {	// ���ۃN���X

protected:
	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	MapUnit_If() {}

	// �X�e�[�^�X
	vector<MapUnit_Action*> action;	// �����f�[�^
	int actionNum;		// ���s���̍s���̗v�f�ԍ�

public:
	// �f�X�g���N�^
	virtual ~MapUnit_If() {}

	bool ActionExecution();	// �����̎��s

	// ����n
	eUnit_If ifType;	// �����̎��
	void AddAction(MapUnit_Action* action);	// �����ǉ�
	virtual void CheckMessage(int num) {}	// ���b�Z�[�W����
	void EndProcess();						// �I������

	virtual bool ChackIf() = 0;	// ��������
};

// �y��l�������̃��j�b�g�ɘb��������z
class MapUnit_If_Talk : public MapUnit_If {
private:
	const int& x;
	const int& y;
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_If_Talk(string line, const int& x, const int& y);
	~MapUnit_If_Talk();

	// �I�[�o�[���C�h
	bool ChackIf();	// ��������
};

// �y��l�������̃��j�b�g�ɐG���z
class MapUnit_If_Touch : public MapUnit_If {
private:
	const int& x;
	const int& y;
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_If_Touch(string line, const int& x, const int& y);
	~MapUnit_If_Touch();

	// �I�[�o�[���C�h
	bool ChackIf();	// ��������
};

// �y���b�Z�[�W��M�z
class MapUnit_If_Message : public MapUnit_If {
private:
	int messageNum;	// ���b�Z�[�W�ԍ�
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_If_Message(string line);
	~MapUnit_If_Message();

	// �I�[�o�[���C�h
	bool ChackIf();	// ��������
};

// �y�펞�z
class MapUnit_If_Always : public MapUnit_If {
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_If_Always();
	~MapUnit_If_Always();

	// �I�[�o�[���C�h
	bool ChackIf();	// ��������
};

// �y�v���C���[�Ƃ̋�������z
class MapUnit_If_Distance : public MapUnit_If {
private:
	const int& x;
	const int& y;

	int distance;	// �����i���a�j
	bool In_Out;	// true:�߂Â�	false:�����
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_If_Distance(string line, const int& x, const int& y);
	~MapUnit_If_Distance();

	// �I�[�o�[���C�h
	bool ChackIf();	// ��������
};
/*-------------------------------------------------���j�b�g�̈ړ��f�[�^--------------------------------------------*/

// ���j�b�g�̈ړ�
class MapUnit_Move {	// ���ۃN���X
protected:
	int preX;	// �P�t���[���O�̍��W
	int preY;

	int& x;
	int& y;
	int speed;		// �ړ����x
	int delay;		// �P�}�X�ړ����邲�Ƃ�nF�~�܂�
	Direction& dir;	// �ړ�����
	int moveRange;	// �ړ�����
	bool moveFlag;	// �ړ����t���O

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Move(int& x, int& y, Direction& dir);
	virtual ~MapUnit_Move() {}

	virtual void Move() = 0;	// �ړ�
	void PosCorrection();		// ���W�C��
};

// �y�����Ȃ��z
class MapUnit_Move_None : public MapUnit_Move {
public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Move_None(int& x, int& y, Direction& dir);
	~MapUnit_Move_None();
	void Move();
};

// �y�����_���ړ��z
class MapUnit_Move_Random: public MapUnit_Move{
private:
	int iniX;		// �������W
	int iniY;
	int count;		// �t���[���J�E���g
	int moveCount;	// �ړ����i�R�����������W�̕����ɓ����j

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit_Move_Random(string line, int& x, int& y, Direction& dir);
	~MapUnit_Move_Random();
	void Move();
};


/*-------------------------------------------------���j�b�g�̃��[�h--------------------------------------------*/

enum class eUnitMode {
	None,		// �����Ȃ�
};

// �y���[�h�̈ڍs�����X�[�p�[�N���X�z
class MapUnit_Mode {
protected:
	vector<MapUnit_If*> terms;	// �����̎��s����
	int termNum;				// ���s���̏����̗v�f�ԍ�
	MapUnit_Move* move;			// �ړ��N���X

	MapUnit_Mode();
public:
	virtual ~MapUnit_Mode() {}
	eUnitMode type;		// �ڍs�����̎��

	virtual bool ModeCheck() = 0;	// ���[�h�̈ڍs��������
	bool TermsCheck();				// �����̎��s��������

	bool ActionExecution();		// �s�����s
	void Move();				// �ړ�
	void PosCorrection();		// ���W�C��

	void AddTerm(MapUnit_If* term);	// �����̎��s������ǉ�
	void AddAction(MapUnit_Action* term);	// �����ǉ�
	void SetMove(MapUnit_Move* data);		// �ړ��N���X�̃Z�b�^�[

	void EndProcess();	// �I������
};

// �y�����Ȃ��z
class MapUnit_Mode_None : public MapUnit_Mode {
private:
	const int modeNum;		// �����̃��[�h�ԍ�
	const int& nowMode;		// ���݂̃��[�h
public:
	MapUnit_Mode_None(int modeNum, int& nowMode);
	~MapUnit_Mode_None();

	bool ModeCheck();	// ���[�h�̈ڍs��������
};
// �y�X�g�[���[�i�s�x�z
class MapUnit_Mode_Chapter : public MapUnit_Mode {
private:
	int chapter;	// ��̐i�s�x
	int term;		// �����i0:�̂Ƃ�	1:�ȊO�̂Ƃ�	2:�ȑO		3:�ȍ~�j
	
public:
	MapUnit_Mode_Chapter(string line);
	~MapUnit_Mode_Chapter();


	bool ModeCheck();	// ���[�h�̈ڍs��������
};

/*-------------------------------------------------�}�b�v�I�u�W�F�N�g�X�[�p�[�N���X--------------------------------------------*/

// �}�b�v��̃I�u�W�F�N�g�X�[�p�[�N���X
class MapObject {	// ���ۃN���X
protected:
	// �A�N�Z�X�������ꂽ�R���X�g���N�^
	MapObject() {}

	// �z�u�f�[�^
	int		id;		// ID
	string	name;	// ���O
	int		initX;	// �������W
	int		initY;		
	int		imgID;	// �摜ID
	bool	collisionFlag;	// �����蔻��
	bool	appearFlag;		// �o���t���O
	Direction dir;	// �ړ�����

	int		img;	// �摜�f�[�^

	int count;
	
public:
	// �f�X�g���N�^
	virtual ~MapObject() {}

	int		x;		// ���W
	int		y;

	// ����n
	bool CheckColl(int x, int y);	// �v���C���[�Ƃ̓����蔻��

	// �Q�b�^�[
	int GetImg() { return img; }	// �摜�̊Ǘ��ԍ���Ԃ�
	int GetID() { return id; }		// ID��Ԃ�
	bool GetAppearFlag() { return appearFlag; }			// �o���t���O
	bool GetCollisionFlag(){ return collisionFlag; }	// �����蔻��

	// �Z�b�^�[
	void ChangeStatus(eUnitStatus type);	// �\����ԂƓ����蔻��̗L�������肷��
	void SetDir(Direction dir);		// �ړ�����)

	// ���b�Z�[�W�֌W
	static void MessageUpDate();					// ���b�Z�[�W�X�V����
	static vector<int> messageData;			// ���b�Z�[�W�̃f�[�^�i1�t���[���O�ɌĂ΂ꂽ���́j
	static vector<int> messageData_Stack;	// ���b�Z�[�W�̃f�[�^�i���t���[���ɌĂ΂ꂽ���́j

	void SetAppearFlag(bool isAppear) { appearFlag = isAppear; }	// �t���O�̂������[
};

/*-------------------------------------------------���j�b�g�f�[�^--------------------------------------------*/

class MapUnit : public MapObject {	// �S���j�b�g�̐e�N���X
private:
	// �X�e�[�^�X
	int nowMode;	// ���݂̃��[�h
	bool execFlag;	// �s�����s���t���O
	
	vector<MapUnit_Mode*> mode;		// ���[�h

	int areaX;		// �敪���̍��W
	int areaY;

	// �t�@�C������
	void LoadData();	// �����f�[�^�̓ǂݍ���

public:
	// �R���X�g���N�^���f�X�g���N�^
	MapUnit(string line);
	~MapUnit();

	// ���C�����[�v
	bool Process();	// ���t���[���̏���
	void Draw(int cameraX, int cameraY);	// �J�������l�������`��

	bool ActionExecution();	// �s�����s
	void PosCorrection();	// ���W�C��

	void SetMode(int modeNum);	// ���[�h�̃Z�b�^�[
	int GetMode();				// ���[�h�̃Q�b�^�[

	void SetArea(int areaX, int areaY);		// �敪�����W�̃Z�b�^�[

	void EndProcess();	// �I������
};