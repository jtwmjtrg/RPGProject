#pragma once
#include "sounddata.h"
#include <vector>
#include <map>
#include "MapObject.h"
#include <list>
#include "Emote.h"

#define AREASIZE 15

using namespace std;

typedef struct _MapInfo {
	int  id;			// 管理番号
	bool isEncount;		// 敵が出るのか
	int  magicAverage;	// 平均的なマソ量
}MapInfo;

// フィールド画面かバトル画面か
typedef enum class eStageMode {
	FIELD, BATTLE, SKILLTREE, GAMEOVER, NONE
}StageMode;

// エンカウントデータ
struct tEncounter {
	int id;		// エンカウントID
	int back;	// 背景ID
	bool escapeFlag;	// 逃走可能フラグ
	bool loseEventFlag;	// 負けイベフラグ
	bool* battleFlag;	// 戦闘中フラグ
	eEndStatus* battleEndType;	// 戦闘終了の状態
	int	battleBGM;	// 戦闘BGM
};

class Game {
protected:
	static StageMode				mode;		// フィールド画面かバトル画面か
	static vector<vector<int> >		mapData;	// マップデータ
	static std::map<int, MapUnit*>	unitData;	// 各ユニットのポインタ
	static vector<MapUnit*>			exec_Unit;	// 現在行動実行中のユニットのポインタ
	static vector<vector<string> >	eventData;	// イベントデータ
	static int						eventNum;	// イベントの数
	static int						eventHead;	// どこまでイベントを読んだか
	static bool						isEvent;	// イベント中かどうか
	static int						now_event_id_;	// 処理中のイベントのid
	static tEncounter				encounter;	// エンカウントデータ
	static MapInfo					info;		// このステージの情報
	static std::map<int,int>		chipToBack;	// マップチップごとにバトル画面の背景を指定
	static bool						waitBattle;	// 戦闘待機状態フラグ
	static int 						fieldBGM;	// フィールドBGM

	static std::vector<std::vector< std::list<MapUnit*> > >	unitArea;	// ユニットの区画分け

	void InfomationUpdate();				// マップの情報の更新

	int playerPreStage;	// 直前のステージ

	Game();	// アクセス制限されたコンストラクタ

public:
	// 継承前提の定義
	virtual ~Game();
	virtual void Process() = 0;
	virtual void Draw() = 0;

	void LoadUnitData();			// ユニットの配置データを読み込む
	//void LoadUnitMode();			// ユニットのモードデータのロード
	//void SaveUnitMode(int stage);	// ユニットのモードデータのセーブ
	void SetUnitArea();				// ユニットの区画分け
	void LaodChipToBack();			// マップチップとバトル画面の背景の連想配列を作成
	static void Encounter(int encounterID, int back, int battleBGM);	// 通常戦闘開始
	static void Encounter(int encounterID, int back, int battleBGM, bool escapeFlag, bool loseEventFlag, bool* battleFlag, eEndStatus* battleEndType);	// イベント戦闘開始
	static void EncounterReset();	// エンカウンターのリセット
	static void TmpSave_UnitMode();	// ユニットのモードの一時セーブ

	static StageMode GetStageMode();		// 現在のモードを返す
};