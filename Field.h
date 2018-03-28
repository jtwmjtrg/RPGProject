#pragma once
#include "Game.h"
#include "Shop.h"

// ステージ名の名前付け
typedef enum eStageName
{
	WORLD,OZGAUTSS,TEEG,VISTE 
}StageName;

// 宣言順の関係
class EventManager;
class MapUnit;
class Game;

class Field :public Game{
private:
	// 画像
	static map<int, int> mapChip;	// マップチップ
	bool isFollow;					// カメラの追従
	Position2D camera_pos_;			// カメラ座標
	int performanceCount;			// 演出のための時間をカウントする
	int performanceSpeed;			// 演出のスピード
	int gr_Operation;				// フィールドでの操作説明
	vector<int> waitBattleGraph;	// 戦闘シーン直前

	// 音

	// 【エンカウント処理用】
	static bool			encFlag;		// エンカウントフラグ
	static vector<int>	encList;		// エンカウントリスト
	int encCount;		// エンカウントまでの残り歩数
	int encMinCount;	// 最小エンカウント歩数
	int encMaxCount;	// 最大エンカウント歩数

	Shop* shop;	// ショップの内容の保存先を示すポインタ

	int	 waitBattleCount;	// 戦闘待機時間

	// debug
	int wait_cam_time_;	// イベント用のカウンター

public:
	// コンストラクタ＆デストラクタ
	Field();
	~Field();

	void Process();	// 処理
	void Draw();	// 描写

	void LoadMapData();			// マップデータを読み込む

	void CameraProcess();					// カメラ移動についての処理
	void MapEffect(int player_position);	// マップデータの効果を記述し、実行する関数
	int PlayerPosition();					// プレイヤーの座標から踏んでいるマップチップの数値を返す
	int PlayerPosition(int i);				// 左上右下の2角
	int PlayerPosition(int i, int j);		// 4角
	bool CollisionChack(int x, int y);		// マップとの衝突判定
	void EventProcess(int id);				// イベント処理
};