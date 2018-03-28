#include "input.h"

// 【乱数生成器】
std::random_device RandClass::rnd;

void RandClass::Ini() {
	std::mt19937 mt;            // メルセンヌ・ツイスタの32ビット版
	// 非決定的な乱数生成器
	std::random_device rnd;     // 非決定的な乱数生成器
	mt.seed(rnd());            // シード指定
}

int RandClass::GetRand() {
	return rnd();
}
int RandClass::GetRand(int end) {
	if (end == -1) return 0;	// %0はエラー吐くから
	return rnd() % (end + 1);
}
int RandClass::GetRand(int start, int end) {
	if (end == -1) return 0;	// %0はエラー吐くから
	return start + rnd() % ((end + 1) - start);
}

// キーボード

int Input::key[256];
SHORT Input::tmpKey[256];

//int Input::pad[4][65536];
int Input::pad[4][16];
XINPUT_STATE Input::state;
int Input::joypadNum = 0;

Input::Input() {}
Input::~Input() {}

void Input::JoypadInitialize()
{
	// ジョイパッド
	DWORD dwResult;
	for (DWORD i = 0; i < 4; i++)
	{
		memset(&state, 0, sizeof(XINPUT_STATE));
		dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS)
		{
			joypadNum++;
		}
		else
		{
			break;
		}
	}
}

void Input::UpDate()
{
	// キーボード ---------------------------------------
	for (int i = 0;i < 256;i++)
	{
		// [i]キーの入力状態を取得
		tmpKey[i] = GetKeyState(i);

		// 保存
		if (tmpKey[i] >= 0)
		{
			if (key[i] > 0)
			{
				key[i] = (-1);
			}
			else
			{
				key[i] = 0;
			}
		}
		else
		{
			key[i]++;
		}
	}
	// キーボード ---------------------------------------

	// ジョイパッド--------------------------------------
	/*for (int i = 0;i < joypadNum;i++)
	{
		XInputGetState(i, &state);

		for (int j = 0;j < 65536;j++)
		{
			if (state.Gamepad.wButtons&j)
			{
				pad[i][j]++;
			}
			else
			{
				if (pad[i][j] > 0)
				{
					pad[i][j] = (-1);
				}
				else
				{
					pad[i][j] = 0;
				}
			}
		}

		DrawString(0xffffffff, 100, 100, "上%d", Input::Get(0, 0));
		DrawString(0xffffffff, 100, 125, "下%d", Input::Get(0, 1));
		DrawString(0xffffffff, 100, 150, "左%d", Input::Get(0, 2));
		DrawString(0xffffffff, 100, 175, "右%d", Input::Get(0, 3));
		DrawString(0xffffffff, 100, 200, "START%d", Input::Get(0, 4));
		DrawString(0xffffffff, 100, 225, "BACK%d", Input::Get(0, 5));
		DrawString(0xffffffff, 100, 250, "L3D%d", Input::Get(0, 6));
		DrawString(0xffffffff, 100, 275, "R3D%d", Input::Get(0, 7));
		DrawString(0xffffffff, 100, 300, "LT%d", Input::Get(0, 8));
		DrawString(0xffffffff, 100, 325, "RT%d", Input::Get(0, 9));
		DrawString(0xffffffff, 100, 350, "A%d", Input::Get(0, 12));
		DrawString(0xffffffff, 100, 375, "B%d", Input::Get(0, 13));
		DrawString(0xffffffff, 100, 400, "X%d", Input::Get(0, 14));
		DrawString(0xffffffff, 100, 425, "Y%d", Input::Get(0, ));
		DrawString(0xffffffff, 100, 450, "%d", Input::state.Gamepad.wButtons);

	}*/
	for (int i = 0;i < joypadNum;i++)
	{
		XInputGetState(i, &state);

		for (int j = 0;j < 16;j++)
		{
			if ((state.Gamepad.wButtons & 1 << j) != 0)
			{
				pad[i][j]++;
			}
			else
			{
				if (pad[i][j] > 0)
				{
					pad[i][j] = (-1);
				}
				else
				{
					pad[i][j] = 0;
				}
			}
		}
	}
	// ジョイパッド--------------------------------------
}

int Input::Get(int vkey)
{
	return key[vkey];
}
int Input::Get(int player, int vbutton)
{
	return pad[player][(int)log2(vbutton)];
}