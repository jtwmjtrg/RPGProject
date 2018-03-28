#include "TextBox.h"

// �摜
int TextBox::gr_box;	// ���w�i
int TextBox::line;		// �g��

// �X�e�[�^�X�n
int TextBox::line_width;	// �g���̑���
deque<DisplayText*> TextBox::strLine;	// �`�悷�镶����
bool TextBox::textBoxFlag;	// �e�L�X�g�t���O

// ����n
int TextBox::maxLine;	// �P�y�[�W�ɍő剽�s�̕������`�ʂ��邩
int TextBox::countText;	// ��������J�E���g
int TextBox::countLine;	// �������s�J�E���g
int TextBox::lineNum;	// �\�����̍s��

// �y�e�L�X�g�N���X�z
TextLine::TextLine(vector<string> str, int speed, int delayLine, int delayPage) :
	str(str),
	speed(speed),
	delayLine(delayLine),
	delayPage(delayPage)
{
	type = eDisplayType::textLine;
}

// �y�I�𕪊�z
SelectText::SelectText(vector<string> str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine):
	str(str),
	choices(choices),
	selectFlag(selectFlag),
	select(select),
	speed(speed),
	delayLine(delayLine)
{
	type = eDisplayType::selectText;
	strEndFlag = false;
}


void TextBox::InitTextBox(){
	// �摜
	gr_box = LoadGraph("img\\textbox\\box.png");	// ���w�i
	line = LoadGraph("img\\textbox\\line.png");	// �g��

	// ����n
	maxLine	= 5;	// �P�y�[�W�ɍő剽�s�̕������`�ʂ��邩
	countText = 0;	// ��������J�E���g
	countLine = 0;	// �������s�J�E���g
	lineNum	= 0;	// �\�����̍s��

	// �X�e�[�^�X�n
	textBoxFlag = false;	// �e�L�X�g�t���O
	line_width = 5;			// �g���̑���
}
void TextBox::Destroy(){
	// �摜�폜
	DeleteGraph(gr_box);	// ���w�i
	DeleteGraph(line);	// �g��
	Reset();
}

void TextBox::DrawBox(int X, int Y, int width, int height, bool fill){
	// �h��Ԃ�
	if (fill){
		DrawStretchGraph(X, Y, gr_box, width, height);
	}
	// �g��
	DrawStretchGraph(X, Y, line, width, line_width);						// ��
	DrawStretchGraph(X, Y + height - line_width, line, width, line_width);	// ��
	DrawStretchGraph(X, Y, line, line_width, height);						// ��
	DrawStretchGraph(X + width - line_width, Y, line, line_width, height);	// �E
}
void TextBox::DrawTextBox(int X, int Y, string text, int width, int height){
	// �h��Ԃ�
	DrawStretchGraph(X, Y, gr_box, width, height);
	// �g��
	DrawStretchGraph(X, Y, line, width, line_width);						// ��
	DrawStretchGraph(X, Y + height - line_width, line, width, line_width);	// ��
	DrawStretchGraph(X, Y, line, line_width, height);						// ��
	DrawStretchGraph(X + width - line_width, Y, line, line_width, height);	// �E
	// ������`��
	DrawString(WHITE, X + 16, Y + 16, text.c_str());
}

void TextBox::Process() {
	// �\�����ׂ������񂪂Ȃ�
	if (strLine.empty()) {
		textBoxFlag = false;	// �t���O��܂�
	}
	// ����ꍇ
	else {
		textBoxFlag = true;		// �t���O�𗧂Ă�

		switch (strLine.front()->type){
		case eDisplayType::textLine: // �y�ʏ�e�L�X�g�z
		{
			// �e�L�X�g�������蒆 || ���̍s������
			if ((unsigned int)(countText / strLine.front()->GetTextLine()->speed) <= strLine.front()->GetTextLine()->str[lineNum].size() || (unsigned)lineNum < strLine.front()->GetTextLine()->str.size() - 1) {

				// ���̍s�̍Ō�܂œǂ� && ���̍s������
				if (((unsigned int)(countText / strLine.front()->GetTextLine()->speed) > strLine.front()->GetTextLine()->str[lineNum].size()) && ((unsigned)lineNum < strLine.front()->GetTextLine()->str.size() - 1)) {
					// �{�^�� or �����Ŏ��̍s�Ɉڂ�
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) || countLine == strLine.front()->GetTextLine()->delayLine) {
						countText = 0;	// ��������J�E���g���Z�b�g
						countLine = 0;	// �������s�J�E���g
						lineNum++;		// ���̍s��\�����n�߂�
					}
					else {
						countLine++;	// �������s�J�E���g�A�b�v
						// �e�L�X�g��S���\��
						if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
							lineNum = strLine.front()->GetTextLine()->str.size() - 1;
							countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// ��������J�E���g
							countLine = 0;	// �������s�J�E���g
						}
					}
				}
				else {
					countText++;	// ��������J�E���g�A�b�v
					// �e�L�X�g��S���\��
					if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
						lineNum = strLine.front()->GetTextLine()->str.size() - 1;
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// ��������J�E���g
					}
					// �e�L�X�g���s�̍Ō�܂ŕ\��
					else if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;
					}
				}
			}
			else {
				// �S���\����
				if ((Input::Get(VK_Z) == 2 || Input::Get(VK_V) == 2 || Input::Get(0, XINPUT_GAMEPAD_A) == 2 || Input::Get(0, XINPUT_GAMEPAD_Y) == 2 || Input::Get(0, XINPUT_GAMEPAD_B) == 2) || strLine.front()->GetTextLine()->delayPage == 0) {
					delete strLine.front();	// �f�[�^�폜
					strLine.pop_front();// ���̃e�L�X�g�ɂ���
					countText = 0;		// �J�E���g���Z�b�g
					lineNum = 0;		// ���̃y�[�W�̈�s�ڂ���\�����n�߂�
					// ���̃y�[�W���������
					if (strLine.empty()) {
						textBoxFlag = false;	// �t���O��܂�
					}
				}
				else {
					strLine.front()->GetTextLine()->delayPage--;	// �����y�[�W����J�E���g�_�E��
				}
			}
			break;
		}
		case eDisplayType::selectText: // �y�I�𕪊�z
		{
			// �e�L�X�g�������蒆 || ���̍s������
			if ((unsigned int)(countText / strLine.front()->GetSelectText()->speed) <= strLine.front()->GetSelectText()->str[lineNum].size() || (unsigned)lineNum < strLine.front()->GetSelectText()->str.size() - 1) {

				// ���̍s�̍Ō�܂œǂ� && ���̍s������
				if (((unsigned int)(countText / strLine.front()->GetSelectText()->speed) > strLine.front()->GetSelectText()->str[lineNum].size()) && ((unsigned)lineNum < strLine.front()->GetSelectText()->str.size() - 1)) {
					// �{�^�� or �����Ŏ��̍s�Ɉڂ�
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) || countLine == strLine.front()->GetSelectText()->delayLine) {
						countText = 0;	// ��������J�E���g���Z�b�g
						countLine = 0;	// �������s�J�E���g
						lineNum++;		// ���̍s��\�����n�߂�
					}
					else {
						countLine++;	// �������s�J�E���g�A�b�v
					}
				}
				else {
					countText++;	// ��������J�E���g�A�b�v
					// �e�L�X�g��S���\��
					if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
						lineNum = strLine.front()->GetTextLine()->str.size() - 1;
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// ��������J�E���g
					}
					// �e�L�X�g���s�̍Ō�܂ŕ\��
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
						countText = strLine.front()->GetSelectText()->str[lineNum].size() * strLine.front()->GetSelectText()->speed - 1;
					}
				}
			}
			else {
				strLine.front()->GetSelectText()->strEndFlag = true;
				// �J�[�\���ړ�
				if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
					|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
					if (strLine.front()->GetSelectText()->select == -1) {
						// �A�ł��ĊԈ���đI�����Ȃ��悤��
						strLine.front()->GetSelectText()->select = 0;
					}
					else {
						strLine.front()->GetSelectText()->select = (strLine.front()->GetSelectText()->select + (strLine.front()->GetSelectText()->choices.size() - 1)) % strLine.front()->GetSelectText()->choices.size();
					}
				}
				if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
					|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
					if (strLine.front()->GetSelectText()->select == -1) {
						// �A�ł��ĊԈ���đI�����Ȃ��悤��
						strLine.front()->GetSelectText()->select = 0;
					}
					else {
						strLine.front()->GetSelectText()->select = (strLine.front()->GetSelectText()->select + 1) % strLine.front()->GetSelectText()->choices.size();
					}
				}
				// ����
				if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1) &&
					strLine.front()->GetSelectText()->select != -1) {	// �A�ł��ĊԈ���đI�����Ȃ��悤��

					strLine.front()->GetSelectText()->selectFlag = true;	// �I���t���O�𗧂Ă�

					delete strLine.front();	// �f�[�^�폜
					strLine.pop_front();// ���̃e�L�X�g�ɂ���
					countText = 0;		// �J�E���g���Z�b�g
					lineNum = 0;		// ���̃y�[�W�̈�s�ڂ���\�����n�߂�
										// ���̃y�[�W���������
					if (strLine.empty()) {
						textBoxFlag = false;	// �t���O��܂�
					}
				}
			}
			break;
		}
		default: 
			delete strLine.front();
			strLine.pop_front();// ���̃e�L�X�g�ɂ���
			break;
		}

		
	}
}

void TextBox::Draw(){
	// �h��Ԃ�
	DrawStretchGraph(0, WINDOW_HEIGHT - 300, gr_box, WINDOW_WIDTH, WINDOW_HEIGHT);
	// �g��
	DrawStretchGraph(0,	WINDOW_HEIGHT - 300, line, WINDOW_WIDTH, line_width);							// ��
	DrawStretchGraph(0, WINDOW_HEIGHT - line_width, line, WINDOW_WIDTH, line_width);					// ��
	DrawStretchGraph(0, WINDOW_HEIGHT - 300, line, line_width, 1080);									// ��
	DrawStretchGraph(WINDOW_WIDTH - line_width, WINDOW_HEIGHT - 300, line, line_width, WINDOW_HEIGHT);	// �E

	// �e�L�X�g����Ȃ�\�����Ȃ�
	if (!textBoxFlag) return;

	switch (strLine.front()->type)
	{
	case eDisplayType::textLine:// �y�ʏ�e�L�X�g�z
	{
		// �\�����Ă��镶���񂪕`�ʂł���ő吔�𒴂��Ă��邩�ǂ���
		if (lineNum < maxLine) {
			// ���ɕ\�����I����Ă���s��\������
			for (int i = 0; i < lineNum; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[i].c_str());
			}
			// �ŐV�̍s
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (lineNum * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		else {
			// ���ɕ\�����I����Ă���s��\������
			for (int i = 0; i < maxLine - 1; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[lineNum - (maxLine - 1) + i].c_str());
			}
			// �ŐV�̍s
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + ((maxLine - 1) * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		break;
	}
	case eDisplayType::selectText:// �y�I�𕪊�z
	{
		// �\�����Ă��镶���񂪕`�ʂł���ő吔�𒴂��Ă��邩�ǂ���
		if (lineNum < maxLine) {
			// ���ɕ\�����I����Ă���s��\������
			for (int i = 0; i < lineNum; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[i].c_str());
			}
			// �ŐV�̍s
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (lineNum * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		else {
			// ���ɕ\�����I����Ă���s��\������
			for (int i = 0; i < maxLine - 1; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[lineNum - (maxLine - 1) + i].c_str());
			}
			// �ŐV�̍s
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + ((maxLine - 1) * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}

		// ����S���\���ςȂ�I���{�b�N�X��\��
		if (strLine.front()->GetSelectText()->strEndFlag) {
			// �c��
			int selectHeight = strLine.front()->GetSelectText()->choices.size() * 48 + 32;
			// ����
			int selectWidth = strLine.front()->GetSelectText()->choices[0].answer.size() * 16 + 96;
			for (int i = 1, n = strLine.front()->GetSelectText()->choices.size(); i < n; ++i) {
				if (selectWidth < (int)strLine.front()->GetSelectText()->choices[i].answer.size() * 16 + 96) {
					selectWidth = strLine.front()->GetSelectText()->choices[i].answer.size() * 16 + 96;
				}
			}
			// �h��Ԃ�
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, gr_box, selectWidth, selectHeight);
			// �g��
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, line, selectWidth, line_width);	// ��
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - line_width, line, selectWidth, line_width);		// ��
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, line, line_width, selectHeight);	// ��
			DrawStretchGraph(320 + selectWidth - line_width, WINDOW_HEIGHT - 320 - selectHeight, line, line_width, selectHeight);	// �E
			// �I����
			for (int i = 0, n = strLine.front()->GetSelectText()->choices.size(); i < n; ++i) {
				DrawString(WHITE, 384, WINDOW_HEIGHT - 300 - selectHeight + i * 48, strLine.front()->GetSelectText()->choices[i].answer.c_str());
			}
			// �J�[�\��
			if (strLine.front()->GetSelectText()->select != -1) {
				DrawString(WHITE, 340, WINDOW_HEIGHT - 300 - selectHeight + strLine.front()->GetSelectText()->select * 48, "��");
			}
		}
		break;
	}
	default:
		break;
	}
}

void TextBox::AddLine(string str, int speed, int delayLine, int delayPage) {
	vector<string> vectStr;		// �L���X�g�p
	vectStr.push_back(str);		// ���
	if (speed == 0)	speed = -1;		// �X�s�[�h0�̓G���[�f��

	strLine.push_back(new TextLine(vectStr, speed, delayLine, delayPage));		// ��Ԍ��̃y�[�W�Ƃ��ēo�^����

	textBoxFlag = true;		// �e�L�X�g�t���O�𗧂Ă�
}

void TextBox::AddLine(vector<string> str, int speed, int delayLine, int delayPage) {
	if (str.empty()) return;		// ��̃��F�N�^�[�͓���Ȃ�
	if (speed == 0)	speed = -1;		// �X�s�[�h0�̓G���[�f��

	strLine.push_back(new TextLine(str , speed , delayLine ,delayPage));		// ��Ԍ��̃y�[�W�Ƃ��ēo�^����

	textBoxFlag = true;		// �e�L�X�g�t���O�𗧂Ă�
}

void TextBox::AddSelect(string str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine) {
	vector<string> vectStr;		// �L���X�g�p
	vectStr.push_back(str);		// ���
	if (speed == 0)	speed = -1;	// �X�s�[�h0�̓G���[�f��

	strLine.push_back(new SelectText(vectStr, choices, selectFlag, select, speed, delayLine));

	textBoxFlag = true;		// �e�L�X�g�t���O�𗧂Ă�
}

// �������
void TextBox::Reset() {
	for (int i = 0, n = (int)strLine.size(); i < n; ++i) {
		delete strLine[i];
	}
	strLine.clear();
	strLine.shrink_to_fit();

	textBoxFlag = false;
	lineNum = 0;
	countLine = 0;
}