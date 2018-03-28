#pragma once
#include <vector>
#include <map>
#include <random>
#include <time.h>

#include "texture.h"
#include "sprite.h"
#include "directxtext.h"

using namespace std;

// define
#define InitRender				GraphicData::InitRender
#define LoadGraph				GraphicData::LoadGraph
#define LoadDivGraph			GraphicData::LoadDivGraph
#define direct3d				GraphicData::direct3d
#define directxtext				GraphicData::directxtext
#define DeleteGraph				GraphicData::DeleteGraph
#define DrawGraph				GraphicData::DrawGraph
#define DrawStretchGraph		GraphicData::DrawStretchGraph
#define DrawDeformationGraph	GraphicData::DrawDeformationGraph
#define AllDeleteGraph			GraphicData::AllDeleteGraph
#define DrawString				directxtext.Draw

class GraphicData {
private:
	static vector<Texture>	tex;
	static vector<Sprite>	sprite;
	static map<int, int>	graphicmanager;
public:
	static Direct3D		direct3d;
	static DirectXText	directxtext;

	GraphicData();
	~GraphicData();

	// 透過処理
	static void InitRender();

	// 画像を読み込む関数
	static int LoadGraph(TCHAR* filename);		// 画像をそのまま読み込む
	static int LoadGraph(TCHAR* filename, int numU, int numV);	// 画像分割読み込み
	static int LoadDivGraph(TCHAR* filename, int Allnum, int Xnum, int Ynum, int Xsize, int Ysize);

	// 読み込んだ画像データを削除する関数
	static void DeleteGraph(int handle);

	// 読み込んだ画像データを全て削除する関数
	static void AllDeleteGraph();

	// 画像を描写する関数
	static void DrawGraph(int x, int y, int handle);	// 通常描写
	static void DrawGraph(int x, int y, int handle, int numU, int numV);			// アニメーション描画
	static void DrawStretchGraph(int x, int y, int handle, int width, int height);	// 拡大縮小描写
	static void DrawStretchGraph(int x, int y, int handle, int numU, int numV, int width, int height);	// 拡大縮小描写（アニメーション）
	static void DrawDeformationGraph(int handle, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
};