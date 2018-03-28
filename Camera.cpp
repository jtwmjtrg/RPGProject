#include "camera.h"

Camera::Camera() :
	m_Length(0.0f),
	m_Yaw(0.0), m_Pitch(0.0)
{
}
Camera::~Camera(){
}

Camera* Camera::instance = NULL;	// ����

Camera* Camera::GetInstance() {
	if (instance == NULL) {
		instance = new Camera();	// ���̍쐬
	}
	return instance;
}
void Camera::Release() {
	if (instance) {
		delete instance;	// ���̍폜
		instance = NULL;
	}
}


//----------------------------------//
//	���_�ʒu�Ǝ���Ώۈʒu�Z�b�g	//
//									//
//	����	Eye	����ʒu			//
//			At	����Ώۂ̈ʒu		//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::SetEyeAtPos(const D3DXVECTOR3 &Eye, const D3DXVECTOR3 &At){
	if (Eye == At)
		return;

	m_Eye = Eye;
	m_At = At;

	D3DXVECTOR3 dist = m_At - m_Eye;
	m_Length = D3DXVec3Length(&dist);
	D3DXVECTOR3 distcos(dist.x, 0.0f, dist.z);

	m_Pitch = -atan2(dist.y, D3DXVec3Length(&distcos));	// �s�b�`�����߂�
	m_Yaw = -atan2(dist.z, dist.x);		// ���[�����߂�

	if (m_Pitch < D3DX_PI * 0.5 && m_Pitch > -D3DX_PI * 0.5)
		m_Up = D3DXVECTOR3(0, 1, 0);

	if (m_Pitch < D3DX_PI * 1.5 && m_Pitch > D3DX_PI * 0.5)
		m_Up = D3DXVECTOR3(0, -1, 0);

	if (m_Pitch == D3DX_PI * 0.5 || m_Pitch == D3DX_PI * 1.5 ||
		m_Pitch == D3DX_PI * 0.5 || m_Pitch == D3DX_PI * 1.5)
	{
		D3DXVECTOR3 temp((float)cos(m_Yaw), 0, 0);
		D3DXVec3Normalize(&m_Up, &temp);
	}
}

//----------------------------------//
//	�p�x�̃Z�b�g					//
//									//
//	����	yaw		���[��]�p�x	//
//			pitch	�s�b�`��]�p�x	//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::SetRad(const double yaw, const double pitch){
	m_Yaw = yaw;
	m_Pitch = pitch;
}

//----------------------------------//
//	�����̃Z�b�g					//
//									//
//	����	length ����				//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::SetLength(const double length){
	if (length < 0)
		return;

	m_Length = length;
}

//----------------------------------//
//	����Ώۈʒu�X�V				//
//									//
//	����	MoveCam	�X�V���		//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::UpdateAt(const D3DXVECTOR3 &MoveAt){
	if (m_Eye == (m_At + MoveAt))
		return;

	m_At += MoveAt;
}

//----------------------------------//
//	�p�x�̍X�V						//
//									//
//	����	yaw		���[��]�p�x	//
//			pitch	�s�b�`��]�p�x	//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::UpdateRad(const float yaw, const float pitch){
	m_Yaw += yaw;
	m_Pitch += pitch;
}

//----------------------------------//
//	�����̍X�V						//
//									//
//	����	length ����				//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::UpdateLength(float length){
	if (m_Length - length < 0.0f) {
		D3DXVECTOR3 temp;
		D3DXVec3Normalize(&temp, &(m_At - m_Eye));
		m_At += temp * length;
		return;
	}

	m_Length -= length;
}

//------------------------------------------//
//  �ۂߏ����֐�
//  ���������_�����w�肳�ꂽ���x�Ŋۂ߂��l��Ԃ�
//
//	����	val	       �ۂ߂����l
//        	precision  ���x
// 
//	�߂�l	�w�萸�x�Ŋۂ߂��l
//------------------------------------------//
double Round(double val, int precision){
	/* ***** �����ϐ���` ***** */
	double		ret;
	char		buf[256] = {};
	char		*p;

	/* ***** �ۂ߂��s��������ɕϊ� ***** */
	sprintf_s(buf, 256 * sizeof(char), "%.*f", precision, val);
	/* ***** �����񂩂琔�l�ɍĕϊ� ***** */
	ret = strtod(buf, &p);

	return ret;
}

//----------------------------------//
//	�J�����̍X�V					//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�Ȃ�					//
//----------------------------------//
void Camera::Update(){
	m_Pitch = Round(m_Pitch, 6);	// ���x�����_��6��
	m_Yaw = Round(m_Yaw, 6);	// ���x�����_��6��

	if (m_Pitch > D3DX_PI * 1.5)
		m_Pitch -= D3DX_PI * 2.0;

	if (m_Pitch < -D3DX_PI * 0.5)
		m_Pitch += D3DX_PI * 2.0;

	if (m_Pitch < D3DX_PI * 0.5 && m_Pitch > -D3DX_PI * 0.5)
		m_Up = D3DXVECTOR3(0, 1, 0);

	if (m_Pitch < D3DX_PI * 1.5 && m_Pitch > D3DX_PI * 0.5)
		m_Up = D3DXVECTOR3(0, -1, 0);

	if (m_Pitch == D3DX_PI * 0.5 || m_Pitch == D3DX_PI * 1.5 ||
		m_Pitch == D3DX_PI * 0.5 || m_Pitch == D3DX_PI * 1.5)
	{
		D3DXVECTOR3 temp((float)cos(m_Yaw), 0, 0);
		D3DXVec3Normalize(&m_Up, &temp);
	}

	m_Eye = D3DXVECTOR3(static_cast<float>(m_Length * cos(m_Pitch) * cos(m_Yaw)),
		static_cast<float>(m_Length * sin(m_Pitch)),
		static_cast<float>(m_Length * cos(m_Pitch) * sin(m_Yaw)));

	//�^�[�Q�b�g�̈ʒu�܂ŕ��s�ړ�
	D3DXMATRIX mtx;
	D3DXMatrixIdentity(&mtx);
	D3DXMatrixTranslation(&mtx, m_At.x, m_At.y, m_At.z);
	D3DXVec3TransformCoord(&m_Eye, &m_Eye, &mtx);

	// �r���[�ϊ��s��쐬
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_Eye, &m_At, &m_Up);


}

//----------------------------------//
//	�ˉe�ϊ��s��̍쐬
//
//	����:	Aspect   �A�X�y�N�g��	
//			Far		 �t�@�[�N���b�v
//			Near	 �j�A�N���b�v
//			Fovy	 ����p�i�S�T�x���ʏ�j
//
//	�߂�l�F�Ȃ�
//----------------------------------//
void Camera::SetProjMatrix(float Aspect, float Far, float Near, float Fovy){
	// �ˉe�ϊ��s��̍쐬
	D3DXMatrixPerspectiveFovLH(&m_ProjMtx, Fovy, Aspect, Near, Far);
}

//----------------------------------//
//	���ˉe�ϊ��s��̍쐬
//
//	����:	W		�r���[��
//			H		�r���[����
//			Far		�t�@�[�N���b�v
//			Near	�j�A�N���b�v
//
//	�߂�l�F�Ȃ�
//----------------------------------//
void Camera::SetOrthoMatrix(float W, float H, float Far, float Near){
	// ���ˉe�ϊ��s��̍쐬
	D3DXMatrixOrthoLH(&m_ProjMtx, W, H, Far, Near);
}

//----------------------------------//
//	�r���[�ϊ��s��擾				//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�r���[�ϊ��s��			//
//----------------------------------//
D3DXMATRIX Camera::GetViewMatrix(){
	return m_ViewMtx;
}

//----------------------------------//
//	�ˉe�ϊ��s��擾				//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�ˉe�ϊ��s��			//
//----------------------------------//
D3DXMATRIX Camera::GetProjMatrix(){
	return m_ProjMtx;
}

//----------------------------------//
//	���_�擾						//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	���_					//
//----------------------------------//
D3DXVECTOR3 Camera::GetEye(){
	return m_Eye;
}

//----------------------------------//
//	�����_�擾						//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�����_					//
//----------------------------------//
D3DXVECTOR3 Camera::GetAt(){
	return m_At;
}

//----------------------------------//
//	�A�b�v�x�N�g���擾				//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�A�b�v�x�N�g��			//
//----------------------------------//
D3DXVECTOR3 Camera::GetUp(){
	return m_Up;
}

//----------------------------------//
//	���[�擾						//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	���[��]				//
//----------------------------------//
float Camera::GetYaw(){
	return static_cast<float>(m_Yaw);
}

//----------------------------------//
//	�s�b�`�擾						//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�s�b�`��]				//
//----------------------------------//
float Camera::GetPitch(){
	return static_cast<float>(m_Pitch);
}

//----------------------------------//
//	�J�������ʕ����i���K���ς݁j�擾//
//									//
//	����	�Ȃ�					//
//									//
//	�߂�l	�J�������ʕ����x�N�g��	//
//----------------------------------//
D3DXVECTOR3 Camera::GetDirection(){
	D3DXVECTOR3 Dir;
	Dir = m_At - m_Eye;
	D3DXVec3Normalize(&Dir, &Dir);
	return Dir;
}