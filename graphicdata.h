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

	// ���ߏ���
	static void InitRender();

	// �摜��ǂݍ��ފ֐�
	static int LoadGraph(TCHAR* filename);		// �摜�����̂܂ܓǂݍ���
	static int LoadGraph(TCHAR* filename, int numU, int numV);	// �摜�����ǂݍ���
	static int LoadDivGraph(TCHAR* filename, int Allnum, int Xnum, int Ynum, int Xsize, int Ysize);

	// �ǂݍ��񂾉摜�f�[�^���폜����֐�
	static void DeleteGraph(int handle);

	// �ǂݍ��񂾉摜�f�[�^��S�č폜����֐�
	static void AllDeleteGraph();

	// �摜��`�ʂ���֐�
	static void DrawGraph(int x, int y, int handle);	// �ʏ�`��
	static void DrawGraph(int x, int y, int handle, int numU, int numV);			// �A�j���[�V�����`��
	static void DrawStretchGraph(int x, int y, int handle, int width, int height);	// �g��k���`��
	static void DrawStretchGraph(int x, int y, int handle, int numU, int numV, int width, int height);	// �g��k���`�ʁi�A�j���[�V�����j
	static void DrawDeformationGraph(int handle, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);
};