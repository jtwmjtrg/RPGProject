#include "camera.h"

Camera::Camera() :
	m_Length(0.0f),
	m_Yaw(0.0), m_Pitch(0.0)
{
}
Camera::~Camera(){
}

Camera* Camera::instance = NULL;	// 実体

Camera* Camera::GetInstance() {
	if (instance == NULL) {
		instance = new Camera();	// 実体作成
	}
	return instance;
}
void Camera::Release() {
	if (instance) {
		delete instance;	// 実体削除
		instance = NULL;
	}
}


//----------------------------------//
//	視点位置と視る対象位置セット	//
//									//
//	引数	Eye	視る位置			//
//			At	視る対象の位置		//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::SetEyeAtPos(const D3DXVECTOR3 &Eye, const D3DXVECTOR3 &At){
	if (Eye == At)
		return;

	m_Eye = Eye;
	m_At = At;

	D3DXVECTOR3 dist = m_At - m_Eye;
	m_Length = D3DXVec3Length(&dist);
	D3DXVECTOR3 distcos(dist.x, 0.0f, dist.z);

	m_Pitch = -atan2(dist.y, D3DXVec3Length(&distcos));	// ピッチを求める
	m_Yaw = -atan2(dist.z, dist.x);		// ヨーを求める

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
//	角度のセット					//
//									//
//	引数	yaw		ヨー回転角度	//
//			pitch	ピッチ回転角度	//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::SetRad(const double yaw, const double pitch){
	m_Yaw = yaw;
	m_Pitch = pitch;
}

//----------------------------------//
//	長さのセット					//
//									//
//	引数	length 長さ				//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::SetLength(const double length){
	if (length < 0)
		return;

	m_Length = length;
}

//----------------------------------//
//	視る対象位置更新				//
//									//
//	引数	MoveCam	更新情報		//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::UpdateAt(const D3DXVECTOR3 &MoveAt){
	if (m_Eye == (m_At + MoveAt))
		return;

	m_At += MoveAt;
}

//----------------------------------//
//	角度の更新						//
//									//
//	引数	yaw		ヨー回転角度	//
//			pitch	ピッチ回転角度	//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::UpdateRad(const float yaw, const float pitch){
	m_Yaw += yaw;
	m_Pitch += pitch;
}

//----------------------------------//
//	長さの更新						//
//									//
//	引数	length 長さ				//
//									//
//	戻り値	なし					//
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
//  丸め処理関数
//  浮動小数点数を指定された精度で丸めた値を返す
//
//	引数	val	       丸めたい値
//        	precision  精度
// 
//	戻り値	指定精度で丸めた値
//------------------------------------------//
double Round(double val, int precision){
	/* ***** 内部変数定義 ***** */
	double		ret;
	char		buf[256] = {};
	char		*p;

	/* ***** 丸めを行い文字列に変換 ***** */
	sprintf_s(buf, 256 * sizeof(char), "%.*f", precision, val);
	/* ***** 文字列から数値に再変換 ***** */
	ret = strtod(buf, &p);

	return ret;
}

//----------------------------------//
//	カメラの更新					//
//									//
//	引数	なし					//
//									//
//	戻り値	なし					//
//----------------------------------//
void Camera::Update(){
	m_Pitch = Round(m_Pitch, 6);	// 精度小数点第6位
	m_Yaw = Round(m_Yaw, 6);	// 精度小数点第6位

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

	//ターゲットの位置まで平行移動
	D3DXMATRIX mtx;
	D3DXMatrixIdentity(&mtx);
	D3DXMatrixTranslation(&mtx, m_At.x, m_At.y, m_At.z);
	D3DXVec3TransformCoord(&m_Eye, &m_Eye, &mtx);

	// ビュー変換行列作成
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_Eye, &m_At, &m_Up);


}

//----------------------------------//
//	射影変換行列の作成
//
//	引数:	Aspect   アスペクト比	
//			Far		 ファークリップ
//			Near	 ニアクリップ
//			Fovy	 視野角（４５度が通常）
//
//	戻り値：なし
//----------------------------------//
void Camera::SetProjMatrix(float Aspect, float Far, float Near, float Fovy){
	// 射影変換行列の作成
	D3DXMatrixPerspectiveFovLH(&m_ProjMtx, Fovy, Aspect, Near, Far);
}

//----------------------------------//
//	正射影変換行列の作成
//
//	引数:	W		ビュー幅
//			H		ビュー高さ
//			Far		ファークリップ
//			Near	ニアクリップ
//
//	戻り値：なし
//----------------------------------//
void Camera::SetOrthoMatrix(float W, float H, float Far, float Near){
	// 正射影変換行列の作成
	D3DXMatrixOrthoLH(&m_ProjMtx, W, H, Far, Near);
}

//----------------------------------//
//	ビュー変換行列取得				//
//									//
//	引数	なし					//
//									//
//	戻り値	ビュー変換行列			//
//----------------------------------//
D3DXMATRIX Camera::GetViewMatrix(){
	return m_ViewMtx;
}

//----------------------------------//
//	射影変換行列取得				//
//									//
//	引数	なし					//
//									//
//	戻り値	射影変換行列			//
//----------------------------------//
D3DXMATRIX Camera::GetProjMatrix(){
	return m_ProjMtx;
}

//----------------------------------//
//	視点取得						//
//									//
//	引数	なし					//
//									//
//	戻り値	視点					//
//----------------------------------//
D3DXVECTOR3 Camera::GetEye(){
	return m_Eye;
}

//----------------------------------//
//	注視点取得						//
//									//
//	引数	なし					//
//									//
//	戻り値	注視点					//
//----------------------------------//
D3DXVECTOR3 Camera::GetAt(){
	return m_At;
}

//----------------------------------//
//	アップベクトル取得				//
//									//
//	引数	なし					//
//									//
//	戻り値	アップベクトル			//
//----------------------------------//
D3DXVECTOR3 Camera::GetUp(){
	return m_Up;
}

//----------------------------------//
//	ヨー取得						//
//									//
//	引数	なし					//
//									//
//	戻り値	ヨー回転				//
//----------------------------------//
float Camera::GetYaw(){
	return static_cast<float>(m_Yaw);
}

//----------------------------------//
//	ピッチ取得						//
//									//
//	引数	なし					//
//									//
//	戻り値	ピッチ回転				//
//----------------------------------//
float Camera::GetPitch(){
	return static_cast<float>(m_Pitch);
}

//----------------------------------//
//	カメラ正面方向（正規化済み）取得//
//									//
//	引数	なし					//
//									//
//	戻り値	カメラ正面方向ベクトル	//
//----------------------------------//
D3DXVECTOR3 Camera::GetDirection(){
	D3DXVECTOR3 Dir;
	Dir = m_At - m_Eye;
	D3DXVec3Normalize(&Dir, &Dir);
	return Dir;
}