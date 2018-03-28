#include "Effect3D.h"

// �e�N�j�b�N�n���h���擾
void Effect3D::GetTechniques()
{

	D3DXEFFECT_DESC Effect3DDesc = {};
	D3DXTECHNIQUE_DESC TechniqueDesc = {};
	D3DXHANDLE hTechnique = NULL;

	// �G�t�F�N�g���擾
	pEffect->GetDesc(&Effect3DDesc);

	// ���݂̃e�N�j�b�N�擾
	hCurrentTechnique = pEffect->GetCurrentTechnique();

	// �e�N�j�b�N�̎擾
	for (DWORD i = 0; i < Effect3DDesc.Techniques; i++)
	{
		// �e�N�j�b�N�n���h���擾
		hTechnique = pEffect->GetTechnique(i);
		// �e�N�j�b�N���擾
		pEffect->GetTechniqueDesc(hTechnique, &TechniqueDesc);

		TCHAR tTechniqueName[MAX_PATH] = {};
		strcpy(tTechniqueName, TechniqueDesc.Name);

		if (TechniqueDesc.Name != NULL)
		{
			hTechniques.insert(std::make_pair(tTechniqueName, hTechnique));
		}

	}
}
// �V�F�[�_�ϐ��n���h���擾
void Effect3D::GetParams()
{

	D3DXEFFECT_DESC Effect3DDesc = {};
	D3DXPARAMETER_DESC ParamDesc = {};
	D3DXHANDLE hParam = NULL;

	// �G�t�F�N�g���擾
	pEffect->GetDesc(&Effect3DDesc);

	// �V�F�[�_�O���[�o���ϐ��̎擾
	for (DWORD i = 0; i < Effect3DDesc.Parameters; i++)
	{
		// �V�F�[�_�O���[�o���ϐ��n���h���擾
		hParam = pEffect->GetParameter(NULL, i);
		// �V�F�[�_�O���[�o���ϐ��̏��擾
		pEffect->GetParameterDesc(hParam, &ParamDesc);

		TCHAR tParamName[MAX_PATH] = {};
		strcpy(tParamName, ParamDesc.Name);

		if (ParamDesc.Name != NULL)
		{
			hParams.insert(std::make_pair(tParamName, hParam));
		}

	}
}

// �R���X�g���N�^
Effect3D::Effect3D() :pEffect(NULL), ID(0), hCurrentTechnique(NULL), CurrentPass(0)
{}


// �f�X�g���N�^
Effect3D::~Effect3D()
{
	hParams.clear();
	hTechniques.clear();
	if (pEffect) {
		pEffect->Release();
		pEffect = nullptr;
	}
}


// �t�@�C������G�t�F�N�g�쐬
bool Effect3D::Create(IDirect3DDevice9* pD3DDevice, const TCHAR* FXFileName)
{
	// ���łɍ쐬����Ă����玸�s
	if (pEffect != NULL)
		return false;

	LPD3DXBUFFER pError = NULL;
	// �G�t�F�N�g�쐬
	HRESULT hr = D3DXCreateEffectFromFileEx(pD3DDevice, FXFileName, NULL, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &pEffect, &pError);

	// �쐬���s
	if (FAILED(hr))
	{
		::OutputDebugStringA(static_cast<char*>(pError->GetBufferPointer()));
		::OutputDebugString(_T("\n"));
		return false;
	}

	// �V�F�[�_���擾
	this->GetTechniques();
	this->GetParams();

	// �G�t�F�N�g��
	Name = FXFileName;

	return true;
}

// ���\�[�X����G�t�F�N�g�쐬
bool Effect3D::Create(IDirect3DDevice9* pD3DDevice, int ResourceID)
{
	// ���łɍ쐬����Ă����玸�s
	if (pEffect != NULL)
		return false;

	LPD3DXBUFFER pError = NULL;
	// �G�t�F�N�g�쐬
	HRESULT hr = D3DXCreateEffectFromResourceEx(pD3DDevice, NULL, MAKEINTRESOURCE(ResourceID), NULL, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &pEffect, &pError);

	// �쐬���s
	if (FAILED(hr))
	{
		::OutputDebugStringA(static_cast<char*>(pError->GetBufferPointer()));
		::OutputDebugString(_T("\n"));
		return false;
	}

	// �V�F�[�_���擾
	this->GetTechniques();
	this->GetParams();

	// �G�t�F�N�gID
	ID = ResourceID;

	return true;
}

// �G�t�F�N�g�|�C���^�擾
ID3DXEffect* Effect3D::GetEffect() {
	return pEffect;
}

// �V�F�[�_�O���[�o���ϐ����`�F�b�N
bool Effect3D::IsParamName(const TCHAR* ParamName) {
	if (hParams.find(ParamName) == hParams.end()) {
		return false;
	}

	return true;
}

// �s��Z�b�g
bool Effect3D::SetMatrix(const TCHAR* ParamName, const D3DXMATRIX* pMtx) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetMatrix(hParams[ParamName], pMtx))) {
			return false;
		}
	}

	return true;
}

// �s��p���b�g�Z�b�g
bool Effect3D::SetMatrixArray(const TCHAR* ParamName, const D3DXMATRIX* pMtxArray, int Count) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetMatrixArray(hParams[ParamName], pMtxArray, Count))) {
			return false;
		}
	}

	return true;
}

// 4�����x�N�g���Z�b�g
bool Effect3D::SetVector4(const TCHAR* ParamName, const D3DXVECTOR4* pVec4) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetVector(hParams[ParamName], pVec4))) {
			return false;
		}
	}

	return true;
}

// 3�����x�N�g���̃Z�b�g
bool Effect3D::SetVector3(const TCHAR* ParamName, const D3DXVECTOR3* pVec3)
{

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetFloatArray(hParams[ParamName], &pVec3->x, 3))) {
			return false;
		}
	}

	return true;
}

// ���������Z�b�g
bool Effect3D::SetFloat(const TCHAR* ParamName, float Param) {

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetFloat(hParams[ParamName], Param))) {
			return false;
		}
	}

	return true;
}

// �����̃Z�b�g
bool Effect3D::SetInt(const TCHAR* ParamName, int Param)
{

#ifdef _DEBUG
	if (this->IsParamName(ParamName))
#endif
	{
		if (FAILED(pEffect->SetInt(hParams[ParamName], Param))) {
			return false;
		}
	}

	return true;
}

// �e�N�X�`���Z�b�g
bool Effect3D::SetTexture(const TCHAR* TextureName, IDirect3DBaseTexture9* pTexture) {

#ifdef _DEBUG
	if (this->IsParamName(TextureName))
#endif
	{
		if (FAILED(pEffect->SetTexture(hParams[TextureName], pTexture))) {
			return false;
		}
	}

	return true;
}

// �e�N�j�b�N�̃Z�b�g
bool Effect3D::SetTechnique(const TCHAR* TechniqueName, UINT Pass)
{
	// �e�N�j�b�N�̕ύX���������ꍇ
	if (hTechniques[TechniqueName] != hCurrentTechnique) {
		// �e�N�j�b�N�̃Z�b�g
		if (FAILED(pEffect->SetTechnique(hTechniques[TechniqueName]))) {
			return false;
		}
		// ���݂̃e�N�j�b�N�擾
		hCurrentTechnique = pEffect->GetCurrentTechnique();
	}
	// �p�X�̃Z�b�g
	CurrentPass = Pass;

	return true;
}

// �G�t�F�N�g�J�n
bool Effect3D::Begin() {

#ifdef _DEBUG
	if (pEffect == NULL)
		return false;
#endif


	UINT PassNum;
	pEffect->Begin(&PassNum, 0);
	// �p�X�Z�b�g
	if (FAILED(pEffect->BeginPass(CurrentPass))) {
		return false;
	}
	// �V�F�[�_�ϐ��̒l�K��
	pEffect->CommitChanges();

	return true;
}


// �G�t�F�N�g�I��
bool Effect3D::End() {

#ifdef _DEBUG
	if (pEffect == NULL)
		return false;
#endif

	// �V�F�[�_�v���O�����̏I���錾
	pEffect->EndPass();
	pEffect->End();

	return true;
}

//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################

// �r���[�ϊ��s��
D3DXMATRIX Object3D::m_ViewMtx;
// �ˉe�ϊ��s��
D3DXMATRIX Object3D::m_ProjMtx;

//-------------------------//
// �R���X�g���N�^
//-------------------------//
Object3D::Object3D()
	:m_Dir(0.0f, 0.0f, -1.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Translate(0, 0, 0)
{
	D3DXMatrixIdentity(&m_RotateMtx);
	D3DXMatrixIdentity(&m_WorldMtx);
}

//-------------------------//
// �f�X�g���N�^
//-------------------------//
Object3D::~Object3D()
{}

//-------------------------------------------//
//	���[���h�ϊ��s����Z�b�g
//
//	�����F	WorldMtx	���[���h�ϊ��s��
//-------------------------------------------//
void Object3D::SetWorldMatrix(const D3DXMATRIX& WorldMtx)
{
	m_WorldMtx = WorldMtx;

	// �v�Z��Scale * Rotate * Translation�̏��ԂɂȂ��Ă���s�񂪑O��ł���
	// ����ȊO�̍s�񂩂�͌��̐������Z�o�ł��Ȃ�


	// �X�P�[�����O�����擾
	D3DXVECTOR3 Sx(WorldMtx._11, WorldMtx._12, WorldMtx._13);
	D3DXVECTOR3 Sy(WorldMtx._21, WorldMtx._22, WorldMtx._23);
	D3DXVECTOR3 Sz(WorldMtx._31, WorldMtx._32, WorldMtx._33);
	D3DXVECTOR3 Scale(D3DXVec3Length(&Sx), D3DXVec3Length(&Sy), D3DXVec3Length(&Sz));
	m_Scale = Scale;

	// ���s�ړ������擾
	m_Translate.x = WorldMtx._41;
	m_Translate.y = WorldMtx._42;
	m_Translate.z = WorldMtx._43;

	// ��]�s��擾
	m_RotateMtx._11 = WorldMtx._11 / Scale.x;
	m_RotateMtx._12 = WorldMtx._12 / Scale.x;
	m_RotateMtx._13 = WorldMtx._13 / Scale.x;
	m_RotateMtx._14 = 0.0f;
	m_RotateMtx._21 = WorldMtx._21 / Scale.y;
	m_RotateMtx._22 = WorldMtx._22 / Scale.y;
	m_RotateMtx._23 = WorldMtx._23 / Scale.y;
	m_RotateMtx._24 = 0.0f;
	m_RotateMtx._31 = WorldMtx._31 / Scale.z;
	m_RotateMtx._32 = WorldMtx._32 / Scale.z;
	m_RotateMtx._33 = WorldMtx._33 / Scale.z;
	m_RotateMtx._34 = 0.0f;
	m_RotateMtx._41 = 0.0f;
	m_RotateMtx._42 = 0.0f;
	m_RotateMtx._43 = 0.0f;
	m_RotateMtx._44 = 1.0f;

}

//--------------------------------------------//
//	���[���h�ϊ��s����擾����
//
//	�߂�l�F���[���h�ϊ��s��
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetWorldMatrix() const
{
	return m_WorldMtx;
}

//--------------------------------------------//
//	���[���h�ϊ��s����쐬����
//--------------------------------------------//
void Object3D::CreateWorldMatrix() {
	D3DXMATRIX ScaleMtx, RotateMtx, TranslateMtx;
	D3DXMatrixScaling(&ScaleMtx, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixTranslation(&TranslateMtx, m_Translate.x, m_Translate.y, m_Translate.z);
	m_WorldMtx = ScaleMtx * m_RotateMtx * TranslateMtx;
}



//-------------------------------------------//
//	�r���[�ϊ��s����Z�b�g
//
//	�����FViewMtx �r���[�ϊ��s��
//-------------------------------------------//
void Object3D::SetViewMtx(const D3DXMATRIX& ViewMtx)
{
	m_ViewMtx = ViewMtx;
}
//--------------------------------------------//
//	�r���[�ϊ��s����擾����
//
//	�߂�l�F�r���[�ϊ��s��
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetViewMatrix()
{
	return m_ViewMtx;
}


//-------------------------------------------//
//	�ˉe�ϊ��s����Z�b�g
//
//	�����FProjMtx �ˉe�ϊ��s��
//-------------------------------------------//
void Object3D::SetProjMtx(const D3DXMATRIX& ProjMtx)
{
	m_ProjMtx = ProjMtx;
}
//--------------------------------------------//
//	�ˉe�ϊ��s����擾����
//
//	�߂�l�F�ˉe�ϊ��s��
//--------------------------------------------//
const D3DXMATRIX& Object3D::GetProjMatrix()
{
	return m_ProjMtx;
}

//////////////////////////////////////////////////////////////////

//-----------------------------------//
//	�X�P�[�����O���Z�b�g����
//
//	�����FScale	�X�P�[�����O
//-----------------------------------//
void Object3D::SetScale(const D3DXVECTOR3& Scale) {
	m_Scale = Scale;
}
//-----------------------------------//
//	�X�P�[�����O���Z�b�g����
//
//	�����Fx,y,z	�e���ɑ΂���X�P�[�����O
//-----------------------------------//
void Object3D::SetScale(float x, float y, float z) {
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

//-----------------------------------//
//	��]���Z�b�g����
//
//	�����FRotate ��]
//-----------------------------------//
void Object3D::SetRotate(const D3DXVECTOR3& Rotate) {
	D3DXMatrixRotationYawPitchRoll(&m_RotateMtx, Rotate.y, Rotate.x, Rotate.z);
}
//-----------------------------------//
//	��]���Z�b�g����
//
//	�����Fx,y,z	�e���ɑ΂����]
//-----------------------------------//
void Object3D::SetRotate(float x, float y, float z) {
	D3DXMatrixRotationYawPitchRoll(&m_RotateMtx, y, x, z);
}
//-----------------------------------//
//	���s�ړ����Z�b�g����
//
//	�����FTranslate ���s�ړ�
//-----------------------------------//
void Object3D::SetTranslate(const D3DXVECTOR3& Translate) {
	m_Translate = Translate;
}
//-----------------------------------//
//	���s�ړ����Z�b�g����
//
//	�����Fx,y,z	�e���ɑ΂��镽�s�ړ�
//-----------------------------------//
void Object3D::SetTranslate(float x, float y, float z) {
	m_Translate.x = x;
	m_Translate.y = y;
	m_Translate.z = z;
}

//////////////////////////////////////////////////////////////////


//-----------------------------------//
//	�X�P�[�����O���X�V����
//
//	�����FScale	�X�P�[�����O
//-----------------------------------//
void Object3D::UpdateScale(const D3DXVECTOR3& Scale)
{
	m_Scale += Scale;
}
//-----------------------------------//
//	�X�P�[�����O���X�V����
//
//	�����Fx,y,z	�e���ɑ΂���X�P�[�����O
//-----------------------------------//
void Object3D::UpdateScale(float x, float y, float z)
{
	m_Scale.x += x;
	m_Scale.y += y;
	m_Scale.z += z;
}

//-----------------------------------//
//	��]���X�V����
//
//	�����FRotate ��]
//-----------------------------------//
void Object3D::UpdateRotate(const D3DXVECTOR3& Rotate)
{
	D3DXMATRIX RotateMtx;
	D3DXMatrixRotationYawPitchRoll(&RotateMtx, Rotate.y, Rotate.x, Rotate.z);
	m_RotateMtx = RotateMtx * m_RotateMtx;
}
//-----------------------------------//
//	��]���X�V����
//
//	�����Fx,y,z	�e���ɑ΂����]
//-----------------------------------//
void Object3D::UpdateRotate(float x, float y, float z)
{
	D3DXMATRIX RotateMtx;
	D3DXMatrixRotationYawPitchRoll(&RotateMtx, y, x, z);
	m_RotateMtx = RotateMtx * m_RotateMtx;
}

//-----------------------------------//
//	���s�ړ����X�V����
//
//	�����FTranslate ���s�ړ�
//-----------------------------------//
void Object3D::UpdateTranslate(const D3DXVECTOR3& Translate)
{
	m_Translate += Translate;
}
//-----------------------------------//
//	���s�ړ����X�V����
//
//	�����Fx,y,z	�e���ɑ΂��镽�s�ړ�
//-----------------------------------//
void Object3D::UpdateTranslate(float x, float y, float z)
{
	m_Translate.x += x;
	m_Translate.y += y;
	m_Translate.z += z;
}


//////////////////////////////////////////////////////////////////


//-----------------------------------//
//	�X�P�[�����O���擾����
//
//	�߂�l�F�X�P�[�����O
//-----------------------------------//
const D3DXVECTOR3& Object3D::GetScale() const {
	return m_Scale;
}
//-----------------------------------//
//	��]���擾����
//
//	�߂�l�F��]
//-----------------------------------//
const D3DXMATRIX& Object3D::GetRotateMatrix() const {
	return m_RotateMtx;
}
//-----------------------------------//
//	���s�ړ����擾����
//
//	�߂�l�F���s�ړ�
//-----------------------------------//
const D3DXVECTOR3& Object3D::GetTranslate() const {
	return m_Translate;
}

//////////////////////////////////////////////////////////////////

//--------------------------------------------//
//	���f���̐��ʌ����Z�b�g
//
//	�����FDir ���ʌ���
//--------------------------------------------//
void Object3D::SetDir(const D3DXVECTOR3& Dir)
{
	m_Dir = Dir;
}

//--------------------------------------------//
//	���f���̐��ʌ����擾
//
//	�߂�l�F���ʌ���
//--------------------------------------------//
D3DXVECTOR3 Object3D::GetDir()
{
	D3DXVECTOR3 Dir;
	D3DXVec3TransformCoord(&Dir, &m_Dir, &m_RotateMtx);
	return Dir;
}

//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################
//##########################################################################################################################################################################################################################################

Effect3D						Geometory::m_Effect;				// �􉽃I�u�W�F�N�g�G�t�F�N�g
IDirect3DVertexDeclaration9*	Geometory::m_pVertexDecl2D = NULL;	// 2�����􉽃I�u�W�F�N�g���_�\���̐錾
IDirect3DVertexDeclaration9*	Geometory::m_pVertexDecl3D = NULL;	// 3�����􉽃I�u�W�F�N�g���_�\���̐錾

// ������
bool Geometory::Init(IDirect3DDevice9* pD3DDevice)
{
	// �G�t�F�N�g�ǂݍ���
	if (!m_Effect.Create(pD3DDevice, _T("geometory.cfx")))
		return false;

	// 2�����􉽃I�u�W�F�N�g���_�\���̒�`
	D3DVERTEXELEMENT9 decl2d[] =
	{
		{ 0, 0,	 D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR	 , 0 },
		D3DDECL_END()
	};

	// 2�����􉽃I�u�W�F�N�g���_�\���̐錾�쐬
	if (FAILED(pD3DDevice->CreateVertexDeclaration(decl2d, &m_pVertexDecl2D)))
	{
		return false;
	}

	// 3�����􉽃I�u�W�F�N�g���_�\���̒�`
	D3DVERTEXELEMENT9 decl3d[] =
	{
		{ 0, 0,	 D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION , 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR	 , 0 },
		D3DDECL_END()
	};

	// 3�����􉽃I�u�W�F�N�g���_�\���̐錾�쐬
	if (FAILED(pD3DDevice->CreateVertexDeclaration(decl3d, &m_pVertexDecl3D)))
	{
		return false;
	}

	atexit(Geometory::Release);
	return true;
}

// ���
void Geometory::Release()
{
	// ���_�\���̐錾���
	if (m_pVertexDecl3D) {
		m_pVertexDecl3D->Release();
		m_pVertexDecl3D = NULL;
	}
	if (m_pVertexDecl2D) {
		m_pVertexDecl2D->Release();
		m_pVertexDecl2D = NULL;
	}
}

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
//	�߂�l�F true:	�`�搬��
//			false:	�`�掸�s
//-------------------------------------------//
bool Geometory::DrawLine(const D3DXVECTOR2& Start, const D3DXVECTOR2& End, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx2D v[2] =
	{
		{ Start.x,Start.y,0.0f,1.0f,Color },
		{ End.x,End.y,0.0f,1.0f,Color },
	};
	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(Vtx2D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawCircle(float R, const D3DXVECTOR2& Pos, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	const int div = 17;
	const float divide = (D3DX_PI * 2) / (div - 1);
	Vtx2D v[div];

	float phi = 0.0f;
	for (int i = 0; i < div; ++i) {
		v[i].x = R * cosf(phi) + Pos.x;
		v[i].y = R * sinf(phi) + Pos.y;
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
		v[i].color = Color;
		phi += divide;
	}

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div - 1, v, sizeof(Vtx2D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawFillCircle(float R, const D3DXVECTOR2& Pos, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	const int div = 17;
	const float divide = (D3DX_PI * 2) / (div - 1);
	Vtx2D v[div + 1];


	v[0].x = Pos.x;
	v[0].y = Pos.y;
	v[0].z = 0.0f;
	v[0].rhw = 1.0f;
	v[0].color = Color;

	float phi = 0.0f;
	for (int i = 1; i < div + 1; ++i) {
		v[i].x = R * cosf(phi) + Pos.x;
		v[i].y = R * sinf(phi) + Pos.y;
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
		v[i].color = Color;
		phi += divide;
	}

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, div - 1, v, sizeof(Vtx2D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	return true;
}

//-------------------------------------------//
//	��`���`��
//
//	�����F	Rect		��`
//			Color		�F
//
//	�߂�l�F	true:	�`�搬��
//			false:	�`�掸�s
//-------------------------------------------//
bool Geometory::DrawRect(const RECT& Rect, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	RECT rect = Rect;

	Vtx2D v[8] =
	{
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color },

		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color },
		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },

		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },
		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },

		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
	};

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 4, v, sizeof(Vtx2D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-------------------------------------------//
//	��`�`��
//
//	�����F	Rect		��`
//			Color		�F
//
//	�߂�l�Ftrue:	�`�搬��
//			false:	�`�掸�s
//-------------------------------------------//
bool Geometory::DrawFillRect(const RECT& Rect, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW2D);
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	RECT rect = Rect;

	Vtx2D v[4] =
	{
		// ����
		{ (float)rect.left,(float)rect.bottom,0.0f,1.0f,Color },
		// ����
		{ (float)rect.left,(float)rect.top,0.0f,1.0f,Color },
		// �E��
		{ (float)rect.right,(float)rect.bottom,0.0f,1.0f,Color },
		// �E��
		{ (float)rect.right,(float)rect.top,0.0f,1.0f,Color }

	};

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl2D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Vtx2D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//								3�����I�u�W�F�N�g�`��										//
//////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------//
//	�G�t�F�N�g�ɕϊ��s��Z�b�g
//	
//	����	�F	pWorldMtx	���[���h�ϊ��s��
//
//	�߂�l�F	true:	�Z�b�g����
//			false:	�Z�b�g���s
//------------------------------------------//
bool Geometory::SetEffectParam(const D3DXMATRIX* pWorldMtx)
{

	// ���[���h�ϊ��s��Z�b�g
	if (pWorldMtx != NULL) {
		if (!m_Effect.SetMatrix(_T("g_mWorld"), pWorldMtx))
			return false;
	}
	else {
		D3DXMATRIX	Mtx;
		D3DXMatrixIdentity(&Mtx);
		m_Effect.SetMatrix(_T("g_mWorld"), &Mtx);
	}

	// �r���[�ϊ��s��Z�b�g
	if (!m_Effect.SetMatrix(_T("g_mView"), &Object3D::GetViewMatrix()))
		return false;
	// �ˉe�ϊ��s��Z�b�g
	if (!m_Effect.SetMatrix(_T("g_mProj"), &Object3D::GetProjMatrix()))
		return false;

	return true;
}

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
bool Geometory::DrawGrid(float Wide, int Divide, const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �`���Ȃ�
	if (Divide < 1)
		return false;

	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	// ���̒��_�f�[�^�����(���v���Ƀf�[�^������Ă���)	
	std::vector<Vtx3D> v;
	v.resize(Divide * 4 + 8);

	//�c���C��
	for (unsigned int i = 0; i < v.size() / 2; i += 2)
	{
		v[i].x = -Wide / 2.0f + i / 2 * Wide / (Divide + 1);
		v[i].y = 0.0f;
		v[i].z = Wide / 2.0f;
		v[i].color = Color;

		v[i + 1].x = -Wide / 2.0f + i / 2 * Wide / (Divide + 1);
		v[i + 1].y = 0.0f;
		v[i + 1].z = -Wide / 2.0f;
		v[i + 1].color = Color;
	}

	//�����C��
	for (unsigned int i = v.size() / 2; i < v.size(); i += 2)
	{
		v[i].x = Wide / 2.0f;
		v[i].y = 0.0f;
		v[i].z = -Wide / 2.0f + (i - v.size() / 2) / 2 * Wide / (Divide + 1);
		v[i].color = Color;

		v[i + 1].x = -Wide / 2.0f;
		v[i + 1].y = 0.0f;
		v[i + 1].z = -Wide / 2.0f + (i - v.size() / 2) / 2 * Wide / (Divide + 1);
		v[i + 1].color = Color;
	}

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, v.size() / 2, &v[0], sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}


//-----------------------------------------------
//	3���`��
//
//	�����F	Length		���̒���
//			pWorldMtx	���[���h�ϊ��s��
//
//	�߂�l�F	true:	�`�搬��
//			false:	�`�掸�s
//--------------------------------------------
bool Geometory::DrawAxis(float Length, const D3DXMATRIX* pWorldMtx)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx3D v[3][2] = {
		{ { -Length / 2.0f,0.0f,0.0f,0xffff0000 },{ Length / 2.0f,0.0f,0.0f,0xffff0000 } },
		{ { 0.0f,-Length / 2.0f,0.0f,0xff00ff00 },{ 0.0f,Length / 2.0f,0.0f,0xff00ff00 } },
		{ { 0.0f,0.0f,-Length / 2.0f,0xff0000ff },{ 0.0f,0.0f,Length / 2.0f,0xff0000ff } },
	};
	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, v, sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//-----------------------------------------------
//	�����`��
//
//	�����F	Start		�n�_
//			End			�I�_
//			pWorldMtx	���[���h�ϊ��s��
//			Color		�F
//
//	�߂�l�F	true:	�`�搬��
//			false:	�`�掸�s
//-----------------------------------------------
bool Geometory::DrawLine(const D3DXVECTOR3& Start, const D3DXVECTOR3& End, const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx3D v[2] =
	{
		{ Start.x,Start.y,Start.z,Color },
		{ End.x,End.y,End.z,Color }
	};
	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx3D v[3][2] = {
		{ { Vtx[0].x,Vtx[0].y,Vtx[0].z,Color },{ Vtx[1].x,Vtx[1].y,Vtx[1].z,Color } },
		{ { Vtx[1].x,Vtx[1].y,Vtx[1].z,Color },{ Vtx[2].x,Vtx[2].y,Vtx[2].z,Color } },
		{ { Vtx[2].x,Vtx[2].y,Vtx[2].z,Color },{ Vtx[0].x,Vtx[0].y,Vtx[0].z,Color } }
	};

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, v, sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

//--------------------------------------------
//	�O�p�`��`��
//
//	�����F	Vtx			�O�p�`�̒��_
//			pWorldMtx	���[���h�ϊ��s��
//			Color		�F
//
//	�߂�l�F	true:	�`�搬��
//			false:	�`�掸�s
//--------------------------------------------
bool Geometory::DrawFillTriangle(const D3DXVECTOR3 Vtx[3], const D3DXMATRIX* pWorldMtx, DWORD Color)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx3D v[3] = {
		{ Vtx[0].x,Vtx[0].y,Vtx[0].z,Color },
		{ Vtx[1].x,Vtx[1].y,Vtx[1].z,Color },
		{ Vtx[2].x,Vtx[2].y,Vtx[2].z,Color }
	};

	// ���_�\���̐錾�Z�b�g	
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, v, sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawCube(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	Vtx3D	v[16];

	v[0].x = Min.x;	v[0].y = Min.y;	v[0].z = Min.z;
	v[1].x = Max.x;	v[1].y = Min.y;	v[1].z = Min.z;
	v[2].x = Max.x;	v[2].y = Max.y;	v[2].z = Min.z;
	v[3].x = Min.x;	v[3].y = Max.y;	v[3].z = Min.z;
	v[4].x = Min.x;	v[4].y = Min.y;	v[4].z = Min.z;
	v[5].x = Min.x;	v[5].y = Min.y;	v[5].z = Max.z;
	v[6].x = Min.x;	v[6].y = Max.y;	v[6].z = Max.z;
	v[7].x = Min.x;	v[7].y = Max.y;	v[7].z = Min.z;

	v[8].x = Max.x;	v[8].y = Min.y;	v[8].z = Max.z;
	v[9].x = Min.x;	v[9].y = Min.y;	v[9].z = Max.z;
	v[10].x = Min.x; v[10].y = Max.y; v[10].z = Max.z;
	v[11].x = Max.x; v[11].y = Max.y; v[11].z = Max.z;
	v[12].x = Max.x; v[12].y = Min.y; v[12].z = Max.z;
	v[13].x = Max.x; v[13].y = Min.y; v[13].z = Min.z;
	v[14].x = Max.x; v[14].y = Max.y; v[14].z = Min.z;
	v[15].x = Max.x; v[15].y = Max.y; v[15].z = Max.z;

	for (int i = 0; i < sizeof(v) / sizeof(v[0]); ++i)
		v[i].color = Color;

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;
	// �`��P
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, v, sizeof(Vtx3D))))
		return false;
	// �`��Q
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, &v[8], sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawSphere(const D3DXMATRIX* pWorldMtx, DWORD Color, const D3DXVECTOR3 &Pos, float R)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	// ���_�\���̐錾�Z�b�g
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;

	// x����]�s��
	D3DXMATRIX RotXMtx;
	D3DXMatrixIdentity(&RotXMtx);
	// y����]�s��
	D3DXMATRIX RotYMtx;
	D3DXMatrixIdentity(&RotYMtx);

	float divide = D3DX_PI / 8;
	D3DXVECTOR3 vec[17];
	Vtx3D v[17];

	D3DXVECTOR3 Translation = Pos;
	for (float phi = 0; phi < D3DX_PI; phi += divide)
	{
		// �~
		int count = 0;
		for (float theta = 0; theta <= D3DX_PI * 2 + divide; theta += divide)
		{
			vec[count].x = R * cosf(theta);
			vec[count].y = 0.0f;
			vec[count].z = R * sinf(theta);
			count++;
		}

		// ��]
		for (int i = 0; i < ((D3DX_PI * 2) / divide + 1); ++i)
		{
			D3DXVec3TransformCoord(&vec[i], &vec[i], &RotXMtx);
			v[i].x = vec[i].x + Translation.x;
			v[i].y = vec[i].y + Translation.y;
			v[i].z = vec[i].z + Translation.z;
			v[i].color = Color;
		}

		// �`��
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, v, sizeof(Vtx3D))))
			return false;

	}

	for (float phi = 0; phi < D3DX_PI; phi += divide)
	{
		//�~
		int count = 0;
		for (float theta = 0; theta <= D3DX_PI * 2 + divide; theta += divide)
		{
			vec[count].x = 0.0f;
			vec[count].y = R * sinf(theta);
			vec[count].z = R * cosf(theta);
			count++;
		}
		D3DXMatrixRotationY(&RotYMtx, phi);

		//��]
		for (int i = 0; i < ((D3DX_PI * 2) / divide + 1); ++i)
		{
			D3DXVec3TransformCoord(&vec[i], &vec[i], &RotYMtx);
			v[i].x = vec[i].x + Translation.x;
			v[i].y = vec[i].y + Translation.y;
			v[i].z = vec[i].z + Translation.z;
			v[i].color = Color;
		}

		//�`��
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, v, sizeof(Vtx3D))))
			return false;

	}


	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

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
bool Geometory::DrawCapsule(const D3DXMATRIX* pWorldMtx, DWORD Color, float R, float Height)
{
	if (m_Effect.GetEffect() == NULL) {
		return false;
	}
	// �f�o�C�X�擾
	IDirect3DDevice9* pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
	SetEffectParam(pWorldMtx);

	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	// ���_�\���̐錾�Z�b�g	
	if (FAILED(pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D)))
		return false;


	// y����]�s��
	D3DXMATRIX RotYMtx;
	D3DXMatrixIdentity(&RotYMtx);

	// ����2�`��
	const int div = 8;
	float divide = D3DX_PI / div;
	D3DXVECTOR3 vec[div + 1];
	Vtx3D v[div + 1];
	int turn = 1;
	float phi = 0.0f;

	for (int k = 0; k < 2; ++k)
	{
		phi = 0.0f;
		for (int j = 0; j <= div; ++j)
		{
			// ���~
			float theta = 0;
			for (int i = 0; i <= div; ++i)
			{
				vec[i].x = 0.0f;
				vec[i].y = R * sinf(theta * turn);
				vec[i].z = R * cosf(theta * turn);
				theta += divide;
			}
			D3DXMatrixRotationY(&RotYMtx, phi);
			phi += divide;

			// ��]
			for (int i = 0; i <= div; ++i)
			{
				D3DXVec3TransformCoord(&vec[i], &vec[i], &RotYMtx);
				v[i].x = vec[i].x;
				v[i].y = vec[i].y + Height / 2.0f * turn;
				v[i].z = vec[i].z;
				v[i].color = Color;
			}

			// �`��
			if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div, v, sizeof(Vtx3D))))
				return false;

		}
		turn *= -1;
	}


	// �~���`��

	// �~�`��
	Vtx3D circlev[div * 2 + 1];
	turn = 1;
	for (int k = 0; k < 2; ++k)
	{
		phi = 0.0f;
		for (int i = 0; i <= div * 2; ++i)
		{
			circlev[i].x = R * sinf(phi);
			circlev[i].y = Height / 2.0f * turn;
			circlev[i].z = R * cosf(phi);
			circlev[i].color = Color;
			phi += divide;
		}
		// �`��
		if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, div * 2, circlev, sizeof(Vtx3D))))
			return false;

		turn *= -1;
	}

	// ���`��
	Vtx3D polev[div * 4];
	phi = 0.0f;
	for (int i = 0; i < div * 4; i += 2)
	{
		polev[i].x = R * sinf(phi);
		polev[i].y = Height / 2.0f;
		polev[i].z = R * cosf(phi);
		polev[i].color = Color;

		polev[i + 1].x = R * sinf(phi);
		polev[i + 1].y = -Height / 2.0f;
		polev[i + 1].z = R * cosf(phi);
		polev[i + 1].color = Color;

		phi += divide;
	}
	// �`��
	if (FAILED(pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, div * 2, polev, sizeof(Vtx3D))))
		return false;

	// �G�t�F�N�g�I��
	m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	return true;
}

