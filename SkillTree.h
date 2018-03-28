#pragma once
#include "Game.h"
#include "Skill.h"
#include "Effect3D.h"
#include "Camera.h"

class PlayerChara;

struct tSkillCell {
	int x;
	int y;
	Skill* skill;		// �擾�X�L��
	int cost;			// �擾�R�X�g
	bool getFlag;		// �擾�t���O
	std::vector<string> premiseSkillID;	// �O��X�L��ID
};

class Effect_Cube : Object3D {
public:
	// 3�������_�\����
	struct Vtx3D {
		float	x, y, z;
		DWORD	color;
	};

	// �`��p�X
	enum {
		PASS_DRAW2D,
		PASS_DRAW3D
	};

	D3DVERTEXELEMENT9 decl3d[3];
	IDirect3DDevice9* pD3DDevice;
	Vtx3D	v[9][16];
	Vtx3D	v_ini[9][16];
	float thetaX[9];
	float thetaY[9];
	float thetaZ[9];
	float scale;
	int count;

	D3DXVECTOR3 core;	// ���S���W

	static Effect3D						m_Effect;		// �􉽃I�u�W�F�N�g�G�t�F�N�g
	static IDirect3DVertexDeclaration9*	m_pVertexDecl3D;// 3�����􉽃I�u�W�F�N�g���_�\���̐錾

	Effect_Cube(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max);
	~Effect_Cube();

	static void Release();
	static bool SetEffectParam(const D3DXMATRIX* pWorldMtx);

	void Draw();
	void InitStatus();
};

class Effect_SkillTree {
private:
	int count;

	Effect_Cube* effect;

public:
	bool flag;

	Effect_SkillTree();
	~Effect_SkillTree();

	void Draw();
	void Set();
};

// �y�X�L���c���[�N���X�z
class SkillTree : public Game {
private:
	// �y�摜�z
	int gr_back;		// �w�i
	int gr_select;		// �I���J�[�\��
	int gr_get;			// �擾�ς݃X�L��

	// �y���z
	int so_BGM;		// BGM
	int so_get;		// �X�L���擾

	static PlayerChara *chara;		// �L�����N�^
	std::map<std::string, tSkillCell> skills;
	std::vector<std::vector<std::string>> cell;

	int selectX;	// �I�����W
	int selectY;

	int scrollX;	// �X�N���[��
	int scrollY;

	// �y�G�t�F�N�g�z
	Effect_SkillTree* effect;
	
public:
	SkillTree();
	~SkillTree();

	void Process();
	void Draw();

	static void SetData(PlayerChara *chara);
	bool CheckPremiseSkill(tSkillCell* skill);		// �O��X�L���̃`�F�b�N
	
	bool LoadData();	// �X�L���c���[�̃��[�h
	void SaveData();	// �X�L���c���[�̃Z�[�u
};