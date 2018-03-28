#pragma once
#include "graphicdata.h"
#include <deque>
#include "input.h"
#include "Define.h"

// ���O�t��
#define DrawBox					TextBox::DrawBox
#define DrawTextBox				TextBox::DrawTextBox
#define DrawConversationBox		TextBox::DrawConversationBox

// �F
#define WHITE	0xffffffff	// ��
#define BLACK	0xff000000	// ��
#define RED		0xffff4040	// ��
#define GREEN	0xff00ff00	// ��
#define BLUE	0xff0000ff	// ��
#define YELLOW	0xffffff00	// ��

// �\���e�L�X�g�̎��
enum eDisplayType {
	textLine,		// ���ʂ̃e�L�X�g
	selectText,		// �I�𕪊�
};

// �I�𕪊�f�[�^
struct tSelectBranch {
	string answer;		// �I������
	int destination;	// �����i�����ԍ���n�i�߂�j
};

class TextLine;
class SelectText;

// �\���e�L�X�g�X�[�p�[�N���X
class DisplayText {
public:
	DisplayText() {};
	~DisplayText() {};
	eDisplayType type;	// �\���e�L�X�g�̎��

	TextLine* GetTextLine() { return (TextLine*)this; }
	SelectText* GetSelectText() { return (SelectText*)this; }
};
// ���ʂ̃e�L�X�g
class TextLine : public DisplayText {
public:
	const vector<string> str;	// �`�悷�镶����
	const int speed;		// ��������X�s�[�h		-1�͕�������Ȃ��i��u�őS�\���j
	int delayLine;	// �������s�̃t���[����			-1�͎����Ȃ�
	int delayPage;	// �����y�[�W����̃t���[����	-1�͎����Ȃ�

	TextLine(vector<string> str, int speed, int delayLine, int delayPage);
	~TextLine(){}
};

// �I�𕪊�
class SelectText : public DisplayText {
public:
	const vector<string> str;	// �`�悷�镶����
	const int speed;			// ��������X�s�[�h			-1�͕�������Ȃ��i��u�őS�\���j
	int delayLine;				// �������s�̃t���[����		-1�͎����Ȃ�
	std::vector<tSelectBranch> choices;	// �I����
	bool& selectFlag;			// �I���t���O
	int& select;				// �I��ԍ�
	bool strEndFlag;			// �S���\���������̃t���O

	SelectText(vector<string> str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine);
	~SelectText() {}
};


class TextBox	// �ÓI�N���X
{
private:
	// �摜
	static int gr_box;		// ���w�i
	static int line;	// ����

	// ����n
	static int maxLine;		// �P�y�[�W�ɉ��s�͂��邩
	static int countText;	// ��������J�E���g
	static int countLine;	// �������s�J�E���g
	static int lineNum;		// �\�����̍s��

	// �X�e�[�^�X�n
	static int			line_width;	// �����̕�
	static deque<DisplayText*>	strLine;	// �`�悷�镶����

public:
	// �������n
	static void InitTextBox();	// ������
	static void Destroy();		// �I������

	// ���C�����[�v
	static void Process();	// ���t���[������
	static void Draw();	// �`��

	// �X�e�[�^�X�n
	static bool textBoxFlag;	// �e�L�X�g�t���O

	// �Z�b�^�[
	static void AddLine(string str, int speed = 2, int delayLine = 0, int delayPage = -1);
	static void AddLine(std::vector<string> str, int speed = 2, int delayLine = 0, int delayPage = -1);	// �`�悷�镶����ǉ�

	static void AddSelect(string str,std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed = 2, int delayLine = 0);

	// ��{�֐�
	static void DrawBox(int X, int Y, int width, int height, bool fill);		// ��`�`��
	static void DrawTextBox(int X, int Y, string text, int width, int height);	// �傫���w��`��

	static void Reset();	// �������
};