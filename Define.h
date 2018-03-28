#pragma once

// ゲームパッド
#define VK_B 0x42
#define VK_C 0x43
#define VK_E 0x45
#define VK_F 0x46
#define VK_Q 0x51
#define VK_V 0x56
#define VK_X 0x58
#define VK_Z 0x5A

// 基本情報
#define CHIPSIZE      64
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

// ２次元座標
typedef struct _Position2D {
	int X;
	int Y;

	void set_pos(int X, int Y) {
		this->X = X;
		this->Y = Y;
	}

}Position2D;