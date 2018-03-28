#include "graphicdata.h"

GraphicData::GraphicData() {}

GraphicData::~GraphicData() {}

Direct3D		GraphicData::direct3d;
DirectXText		GraphicData::directxtext;
vector<Texture>	GraphicData::tex;
vector<Sprite>	GraphicData::sprite;
map<int, int>	GraphicData::graphicmanager;

void GraphicData::InitRender()
{
	// アルファ・ブレンディングを行う
	direct3d.pDevice3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// 透過処理を行う
	direct3d.pDevice3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 半透明処理
	direct3d.pDevice3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
}

int GraphicData::LoadGraph(TCHAR* filename)
{
	// 同期用
	int num;

	// 画像情報構造体
	D3DXIMAGE_INFO info;
	// 画像情報取得
	D3DXGetImageInfoFromFile(filename, &info);

	// テクスチャが一個もない場合
	if (tex.empty())
	{
		tex.emplace_back();
		// テクスチャ生成
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// ファイルが存在しない場合
			return 0;
		}
		// メモリ確保
		sprite.emplace_back();
		// スプライトの大きさ初期化
		sprite.back().SetWidth(info.Width, info.Height);

		// 同期用
		num = 0;
	}
	// 既にメモリがある場合
	else
	{
		tex.emplace_back();

		// テクスチャ生成
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// ファイルが存在しない場合
			tex.erase(tex.begin() + (tex.size() - 1));
			return 0;
		}
		// メモリ確保
		sprite.emplace_back();
		// スプライトの大きさ初期化
		sprite.back().SetWidth(info.Width, info.Height);

		// 同期用
		num = tex.size() - 1;
	}

	random_device				rnd;							// 非決定的な乱数生成器を生成
	mt19937						mt(rnd());						//  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<>	rand_integer(0, 1000000000);	// 乱数範囲指定
	int							r	= rand_integer(mt);			// 保存
	auto						itr = graphicmanager.find(r);	// イテレータ生成

	// かぶり検索
	while (itr != graphicmanager.end())
	{
		r	= rand_integer(mt);			// 乱数生成
		itr = graphicmanager.find(r);	// イテレータ
	}

	// 管理番号確保
	graphicmanager.insert(make_pair(r, num));

	// 管理番号を返す
	return r;
}
int GraphicData::LoadGraph(TCHAR* filename, int divU, int divV)
{
	// 同期用
	int num;

	// 画像情報構造体
	D3DXIMAGE_INFO info;
	// 画像情報取得
	D3DXGetImageInfoFromFile(filename, &info);

	// テクスチャが一個もない場合
	if (tex.empty())
	{
		tex.emplace_back();
		// テクスチャ生成
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// ファイルが存在しない場合
			return 0;
		}
		// メモリ確保
		sprite.emplace_back();
		// スプライトの大きさ初期化
		sprite.back().SetDivide(divU, divV);
		sprite.back().SetWidth(info.Width / sprite.back().divU, info.Height / sprite.back().divV);

		// 同期用
		num = 0;
	}
	// 既にメモリがある場合
	else
	{
		tex.emplace_back();

		// テクスチャ生成
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// ファイルが存在しない場合
			tex.erase(tex.begin() + (tex.size() - 1));
			return 0;
		}
		// メモリ確保
		sprite.emplace_back();
		// スプライトの大きさ初期化
		sprite.back().SetDivide(divU, divV);
		sprite.back().SetWidth(info.Width / sprite.back().divU, info.Height / sprite.back().divV);

		// 同期用
		num = tex.size() - 1;
	}

	random_device rnd;	// 非決定的な乱数生成器を生成
	mt19937 mt(rnd());	//  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<> rand_integer(0, 1000000000);	// 乱数範囲指定
	int r = rand_integer(mt);	// 保存
	auto itr = graphicmanager.find(r);	// イテレータ生成

	// かぶり検索
	while (itr != graphicmanager.end())
	{
		r = rand_integer(mt);	// 乱数生成
		itr = graphicmanager.find(r);	// イテレータ
	}

	// 管理番号確保
	graphicmanager.insert(make_pair(r, num));

	// 管理番号を返す
	return r;
}

int GraphicData::LoadDivGraph(TCHAR* filename, int Allnum, int Xnum, int Ynum, int Xsize, int Ysize)
{
	// 同期用
	int num;

	// テクスチャが一個もない場合
	if (tex.empty())
	{
		tex.emplace_back();
		// テクスチャ生成
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// ファイルが存在しない場合
			return 0;
		}
		// メモリ確保
		sprite.emplace_back();
		// スプライトの大きさ初期化
		sprite.back().SetDivide(Xnum, Ynum);
		sprite.back().SetWidth(Xsize, Ysize);

		// 同期用
		num = 0;
	}
	// 既にメモリがある場合
	else
	{
		bool isVoid = false;

		for (int i = 0, n = tex.size(); i < n; i++)
		{
			// 空きがあったら
			if (tex[i].pTexture == NULL)
			{
				// テクスチャ生成
				if (!(tex[i].Load(direct3d.pDevice3D, filename)))
				{
					// ファイルが存在しない場合
					return 0;
				}
				// スプライトの大きさ初期化
				sprite.back().SetDivide(Xnum, Ynum);
				sprite.back().SetWidth(Xsize, Ysize);

				// 同期用
				num = i;

				isVoid = true;
				break;
			}
		}

		if (!isVoid)
		{
			tex.emplace_back();
			// テクスチャ生成
			if (!(tex.back().Load(direct3d.pDevice3D, filename)))
			{
				// ファイルが存在しない場合
				return 0;
			}
			// メモリ確保
			sprite.emplace_back();
			// スプライトの大きさ初期化
			sprite.back().SetDivide(Xnum, Ynum);
			sprite.back().SetWidth(Xsize, Ysize);

			// 同期用
			num = tex.size() - 1;
		}
	}

	random_device rnd;	// 非決定的な乱数生成器を生成
	mt19937 mt(rnd());	//  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	uniform_int_distribution<> rand_integer(0, 1000000000);		// 乱数範囲指定
	int r = rand_integer(mt);			// 保存
	auto itr = graphicmanager.find(r);	// イテレータ生成

	// かぶり検索
	while (itr != graphicmanager.end())
	{
		r = rand_integer(mt);			// 乱数生成
		itr = graphicmanager.find(r);	// イテレータ
	}

	// 管理番号確保
	graphicmanager.insert(make_pair(r, num));

	// 管理番号を返す
	return r;
}

void GraphicData::DeleteGraph(int handle)
{
	// テクスチャ削除
	tex[graphicmanager[handle]].Delete();
	tex[graphicmanager[handle]].pTexture = NULL;

	// 探索
	auto itr = graphicmanager.find(handle);
	if (itr != graphicmanager.end())
	{
		graphicmanager.erase(itr);
	}
}

void GraphicData::AllDeleteGraph()
{
	vector<int> no;
	for (auto itr = graphicmanager.begin(); itr != graphicmanager.end(); itr++) {
		no.push_back(itr->second);
	}
	for (int i = 0, n = no.size();i < n;++i) {
		DeleteGraph(no[i]);
	}

	tex.clear();
	tex.shrink_to_fit();
	sprite.clear();
	sprite.shrink_to_fit();
	graphicmanager.clear();
}

void GraphicData::DrawGraph(int x, int y, int handle)
{
	sprite[graphicmanager[handle]].Draw(x, y, direct3d.pDevice3D, tex[graphicmanager[handle]].pTexture);
}
void GraphicData::DrawGraph(int x, int y, int handle, int numU, int numV)
{
	sprite[graphicmanager[handle]].SetUVNum(numU, numV);
	sprite[graphicmanager[handle]].Draw(x, y, direct3d.pDevice3D, tex[graphicmanager[handle]].pTexture, false);
}
void GraphicData::DrawStretchGraph(int x, int y, int handle, int width, int height)
{
	if (width < 0 || height < 0)
	{
		return;
	}

	int preWidth = sprite[graphicmanager[handle]].width;
	int preHeight = sprite[graphicmanager[handle]].height;

	sprite[graphicmanager[handle]].SetWidth(width, height);
	sprite[graphicmanager[handle]].Draw(x, y, direct3d.pDevice3D, tex[graphicmanager[handle]].pTexture);
	sprite[graphicmanager[handle]].SetWidth(preWidth, preHeight);
}
void GraphicData::DrawStretchGraph(int x, int y, int handle, int numU, int numV, int width, int height)
{
	if (width < 0 || height < 0)
	{
		return;
	}
	
	int preWidth = sprite[graphicmanager[handle]].width;
	int preHeight = sprite[graphicmanager[handle]].height;

	sprite[graphicmanager[handle]].SetWidth(width, height);
	sprite[graphicmanager[handle]].SetUVNum(numU, numV);
	sprite[graphicmanager[handle]].Draw(x, y, direct3d.pDevice3D, tex[graphicmanager[handle]].pTexture, false);
	sprite[graphicmanager[handle]].SetWidth(preWidth, preHeight);
}
void GraphicData::DrawDeformationGraph(int handle, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
	sprite[graphicmanager[handle]].Draw(direct3d.pDevice3D, tex[graphicmanager[handle]].pTexture, false, x0, y0, x1, y1, x2, y2, x3, y3);
}