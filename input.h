#pragma once
#include "Windows.h"
#include "XInput.h"
#pragma comment (lib, "xinput.lib")
#include<cmath>
#include<random>

#define GetRand RandClass::GetRand

// ����������
class RandClass {
private:
	static std::random_device rnd;
	RandClass() {}
	virtual ~RandClass() {}
public:
	static void Ini();		// ����������
	// �����擾
	static int GetRand();
	static int GetRand(int end);
	static int GetRand(int start, int end);
};


class Input
{
private:
	static int key[256];		// �L�[���͏�ԕۑ�
	static SHORT tmpKey[256];	// ���݂̓��͏�Ԃ�\��

	static int pad[4][16];	// �W���C�p�b�h

public:
	// �W���C�p�b�h
	static XINPUT_STATE state;	// �W���C�p�b�h�̓��͂��Ǘ�����
	static int joypadNum;		// �q����Ă���W���C�p�b�h�̐�

	Input();
	~Input();

	static void JoypadInitialize();	// �W���C�p�b�h�̏�����

	static void UpDate();						// ���͏�ԍX�V
	static int Get(int vkey);					// �L�[�̓��͏�Ԃ𓾂�
	static int Get(int player, int vbutton);	// �Q�[���p�b�h�̓��͏�Ԃ𓾂�
};