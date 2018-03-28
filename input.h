#pragma once
#include "Windows.h"
#include "XInput.h"
#pragma comment (lib, "xinput.lib")
#include<cmath>
#include<random>

#define GetRand RandClass::GetRand

// 乱数生成器
class RandClass {
private:
	static std::random_device rnd;
	RandClass() {}
	virtual ~RandClass() {}
public:
	static void Ini();		// 初期化処理
	// 乱数取得
	static int GetRand();
	static int GetRand(int end);
	static int GetRand(int start, int end);
};


class Input
{
private:
	static int key[256];		// キー入力状態保存
	static SHORT tmpKey[256];	// 現在の入力状態を表す

	static int pad[4][16];	// ジョイパッド

public:
	// ジョイパッド
	static XINPUT_STATE state;	// ジョイパッドの入力を管理する
	static int joypadNum;		// 繋がれているジョイパッドの数

	Input();
	~Input();

	static void JoypadInitialize();	// ジョイパッドの初期化

	static void UpDate();						// 入力状態更新
	static int Get(int vkey);					// キーの入力状態を得る
	static int Get(int player, int vbutton);	// ゲームパッドの入力状態を得る
};