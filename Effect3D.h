#pragma once
#include "direct3d.h"
#include "d3dx9effect.h"
#include <map>
#include <vector>

// 3D�G�t�F�N�g
class Effect3D{
private:
	ID3DXEffect*											pEffect;			// �G�t�F�N�g�|�C���^
	std::map<std::basic_string<TCHAR>, D3DXHANDLE>			hTechniques;		// �e�N�j�b�N�n���h��
	std::map<std::basic_string<TCHAR>, D3DXHANDLE>			hParams;			// �V�F�[�_�O���[�o���ϐ��n���h��
	std::map<std::basic_string<TCHAR>, D3DXTECHNIQUE_DESC>	Techniques;		// �e�N�j�b�N
	std::map<std::basic_string<TCHAR>, D3DXPARAMETER_DESC>	Params;			// �V�F�[�_�O���[�o���ϐ�
	std::basic_string<TCHAR>								Name;				// �G�t�F�N�g��
	int														ID;				// �G�t�F�N�g���\�[�XID
	D3DXHANDLE												hCurrentTechnique;// ���݂̃e�N�j�b�N
	UINT													CurrentPass;		// ���݂̃p�X
	
	// �e�N�j�b�N���擾
	void GetTechniques(); 
	// �V�F�[�_�ϐ����擾
	void GetParams();
public:
	Effect3D();
	~Effect3D();
	
	// �t�@�C������G�t�F�N�g�쐬
	bool Create(IDirect3DDevice9* pD3DDevice, const TCHAR* FXFileName);
	// ���\�[�X����G�t�F�N�g�쐬
	bool Create(IDirect3DDevice9* pD3DDevice, int ResourceID);

	// �G�t�F�N�g�|�C���^�擾
	ID3DXEffect* GetEffect();

	// �V�F�[�_�O���[�o���ϐ����`�F�b�N
	bool IsParamName(const TCHAR* ParamName);
	// �s��Z�b�g
	bool SetMatrix(const TCHAR* ParamName, const D3DXMATRIX* pMtx);
	// �s��p���b�g�Z�b�g
	bool SetMatrixArray(const TCHAR* ParamName, const D3DXMATRIX* pMtxArray, int Count);
	// 4�����x�N�g���Z�b�g
	bool SetVector4(const TCHAR* ParamName, const D3DXVECTOR4* pVec4);
	// 3�����x�N�g���Z�b�g
	bool SetVector3(const TCHAR* ParamName, const D3DXVECTOR3* pVec3);
	// ���������Z�b�g
	bool SetFloat(const TCHAR* ParamName, float Param);
	// �����̃Z�b�g
	bool SetInt(const TCHAR* ParamName, int Param);
	// �e�N�X�`���Z�b�g
	bool SetTexture(const TCHAR* TextureName, IDirect3DBaseTexture9* pTexture);

	// �e�N�j�b�N�̃Z�b�g
	bool SetTechnique(const TCHAR* TechniqueName, UINT Pass = 0);
	// �G�t�F�N�g�J�n
	bool Begin();
	// �G�t�F�N�g�I��
	bool End();
};


// �R�����I�u�W�F�N�g�N���X
class Object3D
{
protected:
	// �I�u�W�F�N�g�̐��ʕ����i�����Ă�����j
	D3DXVECTOR3 m_Dir;
	// �X�P�[���A���s�ړ�
	D3DXVECTOR3 m_Scale, m_Translate;
	// ��]�s��
	D3DXMATRIX	m_RotateMtx;
	// ���[���h�ϊ��s��
	D3DXMATRIX	m_WorldMtx;
	// �r���[�ϊ��s��
	static D3DXMATRIX m_ViewMtx;
	// �ˉe�ϊ��s��
	static D3DXMATRIX m_ProjMtx;
public:

	//-------------------------//
	// �R���X�g���N�^
	//-------------------------//
	Object3D();
	//-------------------------//
	// �f�X�g���N�^
	//-------------------------//
	virtual ~Object3D();

	//-------------------------------------------//
	//	���[���h�ϊ��s����Z�b�g
	//
	//	�����F	WorldMtx	���[���h�ϊ��s��
	//-------------------------------------------//
	void SetWorldMatrix(const D3DXMATRIX& WorldMtx);
	//--------------------------------------------//
	//	���[���h�ϊ��s����擾����
	//
	//	�߂�l�F���[���h�ϊ��s��
	//--------------------------------------------//
	const D3DXMATRIX& GetWorldMatrix() const;
	//--------------------------------------------//
	//	���[���h�ϊ��s����쐬����
	//--------------------------------------------//
	virtual void CreateWorldMatrix();

	//-------------------------------------------//
	//	�r���[�ϊ��s����Z�b�g
	//
	//	�����FViewMtx �r���[�ϊ��s��
	//-------------------------------------------//
	static void SetViewMtx(const D3DXMATRIX& ViewMtx);
	//--------------------------------------------//
	//	�r���[�ϊ��s����擾����
	//
	//	�߂�l�F�r���[�ϊ��s��
	//--------------------------------------------//
	static const D3DXMATRIX& GetViewMatrix();

	//-------------------------------------------//
	//	�ˉe�ϊ��s����Z�b�g
	//
	//	�����FProjMtx �ˉe�ϊ��s��
	//-------------------------------------------//
	static void SetProjMtx(const D3DXMATRIX& ProjMtx);
	//--------------------------------------------//
	//	�ˉe�ϊ��s����擾����
	//
	//	�߂�l�F�ˉe�ϊ��s��
	//--------------------------------------------//
	static const D3DXMATRIX& GetProjMatrix();

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	�X�P�[�����O���Z�b�g����
	//
	//	�����FScale	�X�P�[�����O
	//-----------------------------------//
	void SetScale(const D3DXVECTOR3& Scale);
	//-----------------------------------//
	//	�X�P�[�����O���Z�b�g����
	//
	//	�����Fx,y,z	�e���ɑ΂���X�P�[�����O
	//-----------------------------------//
	void SetScale(float x, float y, float z);
	//-----------------------------------//
	//	��]���Z�b�g����
	//
	//	�����FRotate ��]
	//-----------------------------------//
	void SetRotate(const D3DXVECTOR3& Rotate);
	//-----------------------------------//
	//	��]���Z�b�g����
	//
	//	�����Fx,y,z	�e���ɑ΂����]
	//-----------------------------------//
	void SetRotate(float x, float y, float z);
	//-----------------------------------//
	//	���s�ړ����Z�b�g����
	//
	//	�����FTranslate ���s�ړ�
	//-----------------------------------//
	void SetTranslate(const D3DXVECTOR3& Translate);
	//-----------------------------------//
	//	���s�ړ����Z�b�g����
	//
	//	�����Fx,y,z	�e���ɑ΂��镽�s�ړ�
	//-----------------------------------//
	void SetTranslate(float x, float y, float z);

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	�X�P�[�����O���X�V����
	//
	//	�����FScale	�X�P�[�����O
	//-----------------------------------//
	void UpdateScale(const D3DXVECTOR3& Scale);
	//-----------------------------------//
	//	�X�P�[�����O���X�V����
	//
	//	�����Fx,y,z	�e���ɑ΂���X�P�[�����O
	//-----------------------------------//
	void UpdateScale(float x, float y, float z);
	//-----------------------------------//
	//	��]���X�V����
	//
	//	�����FRotate ��]
	//-----------------------------------//
	void UpdateRotate(const D3DXVECTOR3& Rotate);
	//-----------------------------------//
	//	��]���X�V����
	//
	//	�����Fx,y,z	�e���ɑ΂����]
	//-----------------------------------//
	void UpdateRotate(float x, float y, float z);
	//-----------------------------------//
	//	���s�ړ����X�V����
	//
	//	�����FTranslate ���s�ړ�
	//-----------------------------------//
	void UpdateTranslate(const D3DXVECTOR3& Translate);
	//-----------------------------------//
	//	���s�ړ����X�V����
	//
	//	�����Fx,y,z	�e���ɑ΂��镽�s�ړ�
	//-----------------------------------//
	void UpdateTranslate(float x, float y, float z);

	//////////////////////////////////////////////////////////////////

	//-----------------------------------//
	//	�X�P�[�����O���擾����
	//
	//	�߂�l�F�X�P�[�����O
	//-----------------------------------//
	const D3DXVECTOR3& GetScale() const;
	//-----------------------------------//
	//	��]���擾����
	//
	//	�߂�l�F��]�s��
	//-----------------------------------//
	const D3DXMATRIX& GetRotateMatrix() const;
	//-----------------------------------//
	//	���s�ړ����擾����
	//
	//	�߂�l�F���s�ړ�
	//-----------------------------------//
	const D3DXVECTOR3& GetTranslate() const;

	//////////////////////////////////////////////////////////////////

	//--------------------------------------------//
	//	���f���̐��ʌ����Z�b�g
	//
	//	�����FDir ���ʌ���
	//--------------------------------------------//
	void SetDir(const D3DXVECTOR3& Dir);

	//--------------------------------------------//
	//	���f���̐��ʌ����擾
	//
	//	�߂�l�F���ʌ���
	//--------------------------------------------//
	D3DXVECTOR3 GetDir();

};



// �􉽃I�u�W�F�N�g�N���X
class Geometory
{
private:
	// 2�������_�\����
	struct Vtx2D {
		float	x, y, z;
		float	rhw;
		int		color;
	};
	// 3�������_�\����
	struct Vtx3D {
		float	x, y, z;
		int		color;
	};

	// �`��p�X
	enum {
		PASS_DRAW2D,
		PASS_DRAW3D
	};


	static Effect3D						m_Effect;		// �􉽃I�u�W�F�N�g�G�t�F�N�g
	static IDirect3DVertexDeclaration9*	m_pVertexDecl2D;// 2�����􉽃I�u�W�F�N�g���_�\���̐錾
	static IDirect3DVertexDeclaration9*	m_pVertexDecl3D;// 3�����􉽃I�u�W�F�N�g���_�\���̐錾

														// �R���X�g���N�^
	Geometory() {}
	// �R�s�[�R���X�g���N�^
	Geometory(const Geometory& obj) {}
	// ������Z�q
	Geometory& operator=(const Geometory& obj) {}

public:
	// ������
	static bool Init(IDirect3DDevice9* pD3DDevice);
	// ���
	static void Release();

	//////////////////////////////////////////////////////////////////////////////////////////
	//								2�����I�u�W�F�N�g�`��										//
	//////////////////////////////////////////////////////////////////////////////////////////


	//-------------------------------------------//
	//	�����`��
	//
	//	�����F	Start		�n�_
	//			End			�I�_
	//			Color		�F
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//-------------------------------------------//
	static bool DrawLine(const D3DXVECTOR2& Start, const D3DXVECTOR2& End, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	�ې��`��
	//
	//	�����F	R		���a
	//			Pos		�ʒu
	//			Color	�F
	//			
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//-------------------------------------------//
	static bool DrawCircle(float R, const D3DXVECTOR2& Pos, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	�ە`��
	//
	//	�����F	R		���a
	//			Pos		�ʒu
	//			Color	�F
	//			
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//-------------------------------------------//
	static bool DrawFillCircle(float R, const D3DXVECTOR2& Pos, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	��`���`��
	//
	//	�����F	Rect		��`
	//			Color		�F
	//
	//	�߂�l�Ftrue:	�`�搬��
	//			false:	�`�掸�s
	//-------------------------------------------//
	static bool DrawRect(const RECT& Rect, DWORD Color = 0xffff0000);

	//-------------------------------------------//
	//	��`�`��
	//
	//	�����F	Rect		��`
	//			Color		�F
	//
	//	�߂�l�Ftrue:	�`�搬��
	//			false:	�`�掸�s
	//-------------------------------------------//
	static bool DrawFillRect(const RECT& Rect, DWORD Color = 0xffff0000);

	//////////////////////////////////////////////////////////////////////////////////////////
	//								3�����I�u�W�F�N�g�`��										//
	//////////////////////////////////////////////////////////////////////////////////////////

private:
	//------------------------------------------//
	//	�G�t�F�N�g�ɕϊ��s��Z�b�g
	//	
	//	����	�F	pWorldMtx	���[���h�ϊ��s��
	//
	//	�߂�l�F	true:	�Z�b�g����
	//			false:	�Z�b�g���s
	//------------------------------------------//
	static bool SetEffectParam(const D3DXMATRIX* pWorldMtx);

public:

	//-----------------------------------------------
	//	�O���b�h�`��
	//
	//	�����F	Wide		��
	//			Divide		������
	//			pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawGrid(float Wide, int Divide = 5, const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffffffff);

	//-----------------------------------------------
	//	3���`��
	//
	//	�����F	Length		���̒���
	//			pWorldMtx	���[���h�ϊ��s��
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawAxis(float Length, const D3DXMATRIX* pWorldMtx = NULL);

	//-----------------------------------------------
	//	�����`��
	//
	//	�����F	Start		�n�_
	//			End			�I�_
	//			pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//
	//	�߂�l�Ftrue:	�`�搬��
	//			false:	�`�掸�s
	//-----------------------------------------------
	static bool DrawLine(const D3DXVECTOR3& Start, const D3DXVECTOR3& End, const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);

	//--------------------------------------------
	//	�O�p�`����`��
	//
	//	�����F	Vtx			�O�p�`�̒��_
	//			pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);

	//--------------------------------------------
	//	�O�p�`��`��
	//
	//	�����F	Vtx			�O�p�`�̒��_
	//			pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//
	//	�߂�l�Ftrue:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawFillTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000);


	//--------------------------------------------
	//	�����̐���`��
	//
	//	�����F	pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//			Min			�����̂̑Ίp�ŏ��_
	//			Max			�����̂̑Ίp�ő�_
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawCube(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, const D3DXVECTOR3 &Min = D3DXVECTOR3(-0.5f, -0.5f, -0.5f), const D3DXVECTOR3 &Max = D3DXVECTOR3(0.5f, 0.5f, 0.5f));

	//-----------------------------------------------
	//	������`��
	//
	//	�����F	pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//			Pos			���̒��S���W
	//			R			���̔��a			
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawSphere(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, const D3DXVECTOR3 &Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f), float R = 1.0f);

	//---------------------------------------------
	//	�J�v�Z���`��
	//
	//	�����F	pWorldMtx	���[���h�ϊ��s��
	//			Color		�F
	//			R			���̔��a			
	//			Height		�~���̍���
	//
	//	�߂�l�F	true:	�`�搬��
	//			false:	�`�掸�s
	//--------------------------------------------
	static bool DrawCapsule(const D3DXMATRIX* pWorldMtx = NULL, DWORD Color = 0xffff0000, float R = 1.0f, float Height = 2.0f);
};


