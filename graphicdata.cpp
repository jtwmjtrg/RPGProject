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
	// �A���t�@�E�u�����f�B���O���s��
	direct3d.pDevice3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// ���ߏ������s��
	direct3d.pDevice3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// ����������
	direct3d.pDevice3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
}

int GraphicData::LoadGraph(TCHAR* filename)
{
	// �����p
	int num;

	// �摜���\����
	D3DXIMAGE_INFO info;
	// �摜���擾
	D3DXGetImageInfoFromFile(filename, &info);

	// �e�N�X�`��������Ȃ��ꍇ
	if (tex.empty())
	{
		tex.emplace_back();
		// �e�N�X�`������
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// �t�@�C�������݂��Ȃ��ꍇ
			return 0;
		}
		// �������m��
		sprite.emplace_back();
		// �X�v���C�g�̑傫��������
		sprite.back().SetWidth(info.Width, info.Height);

		// �����p
		num = 0;
	}
	// ���Ƀ�����������ꍇ
	else
	{
		tex.emplace_back();

		// �e�N�X�`������
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// �t�@�C�������݂��Ȃ��ꍇ
			tex.erase(tex.begin() + (tex.size() - 1));
			return 0;
		}
		// �������m��
		sprite.emplace_back();
		// �X�v���C�g�̑傫��������
		sprite.back().SetWidth(info.Width, info.Height);

		// �����p
		num = tex.size() - 1;
	}

	random_device				rnd;							// �񌈒�I�ȗ���������𐶐�
	mt19937						mt(rnd());						//  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	uniform_int_distribution<>	rand_integer(0, 1000000000);	// �����͈͎w��
	int							r	= rand_integer(mt);			// �ۑ�
	auto						itr = graphicmanager.find(r);	// �C�e���[�^����

	// ���Ԃ茟��
	while (itr != graphicmanager.end())
	{
		r	= rand_integer(mt);			// ��������
		itr = graphicmanager.find(r);	// �C�e���[�^
	}

	// �Ǘ��ԍ��m��
	graphicmanager.insert(make_pair(r, num));

	// �Ǘ��ԍ���Ԃ�
	return r;
}
int GraphicData::LoadGraph(TCHAR* filename, int divU, int divV)
{
	// �����p
	int num;

	// �摜���\����
	D3DXIMAGE_INFO info;
	// �摜���擾
	D3DXGetImageInfoFromFile(filename, &info);

	// �e�N�X�`��������Ȃ��ꍇ
	if (tex.empty())
	{
		tex.emplace_back();
		// �e�N�X�`������
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// �t�@�C�������݂��Ȃ��ꍇ
			return 0;
		}
		// �������m��
		sprite.emplace_back();
		// �X�v���C�g�̑傫��������
		sprite.back().SetDivide(divU, divV);
		sprite.back().SetWidth(info.Width / sprite.back().divU, info.Height / sprite.back().divV);

		// �����p
		num = 0;
	}
	// ���Ƀ�����������ꍇ
	else
	{
		tex.emplace_back();

		// �e�N�X�`������
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// �t�@�C�������݂��Ȃ��ꍇ
			tex.erase(tex.begin() + (tex.size() - 1));
			return 0;
		}
		// �������m��
		sprite.emplace_back();
		// �X�v���C�g�̑傫��������
		sprite.back().SetDivide(divU, divV);
		sprite.back().SetWidth(info.Width / sprite.back().divU, info.Height / sprite.back().divV);

		// �����p
		num = tex.size() - 1;
	}

	random_device rnd;	// �񌈒�I�ȗ���������𐶐�
	mt19937 mt(rnd());	//  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	uniform_int_distribution<> rand_integer(0, 1000000000);	// �����͈͎w��
	int r = rand_integer(mt);	// �ۑ�
	auto itr = graphicmanager.find(r);	// �C�e���[�^����

	// ���Ԃ茟��
	while (itr != graphicmanager.end())
	{
		r = rand_integer(mt);	// ��������
		itr = graphicmanager.find(r);	// �C�e���[�^
	}

	// �Ǘ��ԍ��m��
	graphicmanager.insert(make_pair(r, num));

	// �Ǘ��ԍ���Ԃ�
	return r;
}

int GraphicData::LoadDivGraph(TCHAR* filename, int Allnum, int Xnum, int Ynum, int Xsize, int Ysize)
{
	// �����p
	int num;

	// �e�N�X�`��������Ȃ��ꍇ
	if (tex.empty())
	{
		tex.emplace_back();
		// �e�N�X�`������
		if (!(tex.back().Load(direct3d.pDevice3D, filename)))
		{
			// �t�@�C�������݂��Ȃ��ꍇ
			return 0;
		}
		// �������m��
		sprite.emplace_back();
		// �X�v���C�g�̑傫��������
		sprite.back().SetDivide(Xnum, Ynum);
		sprite.back().SetWidth(Xsize, Ysize);

		// �����p
		num = 0;
	}
	// ���Ƀ�����������ꍇ
	else
	{
		bool isVoid = false;

		for (int i = 0, n = tex.size(); i < n; i++)
		{
			// �󂫂���������
			if (tex[i].pTexture == NULL)
			{
				// �e�N�X�`������
				if (!(tex[i].Load(direct3d.pDevice3D, filename)))
				{
					// �t�@�C�������݂��Ȃ��ꍇ
					return 0;
				}
				// �X�v���C�g�̑傫��������
				sprite.back().SetDivide(Xnum, Ynum);
				sprite.back().SetWidth(Xsize, Ysize);

				// �����p
				num = i;

				isVoid = true;
				break;
			}
		}

		if (!isVoid)
		{
			tex.emplace_back();
			// �e�N�X�`������
			if (!(tex.back().Load(direct3d.pDevice3D, filename)))
			{
				// �t�@�C�������݂��Ȃ��ꍇ
				return 0;
			}
			// �������m��
			sprite.emplace_back();
			// �X�v���C�g�̑傫��������
			sprite.back().SetDivide(Xnum, Ynum);
			sprite.back().SetWidth(Xsize, Ysize);

			// �����p
			num = tex.size() - 1;
		}
	}

	random_device rnd;	// �񌈒�I�ȗ���������𐶐�
	mt19937 mt(rnd());	//  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
	uniform_int_distribution<> rand_integer(0, 1000000000);		// �����͈͎w��
	int r = rand_integer(mt);			// �ۑ�
	auto itr = graphicmanager.find(r);	// �C�e���[�^����

	// ���Ԃ茟��
	while (itr != graphicmanager.end())
	{
		r = rand_integer(mt);			// ��������
		itr = graphicmanager.find(r);	// �C�e���[�^
	}

	// �Ǘ��ԍ��m��
	graphicmanager.insert(make_pair(r, num));

	// �Ǘ��ԍ���Ԃ�
	return r;
}

void GraphicData::DeleteGraph(int handle)
{
	// �e�N�X�`���폜
	tex[graphicmanager[handle]].Delete();
	tex[graphicmanager[handle]].pTexture = NULL;

	// �T��
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