#pragma once
// debug
typedef struct _BattleStatus {
	int  maxHP;		// 最大HP
	int  maxMP;		// 最大MP
	int		HP;		// ヒットポイント
	int		MP;		// マジックパワー
	int		STR;	// 筋力
 // int		INT;	// かしこさ
	int		DEX;	// 器用さ、命中
	int		AGI;	// 素早さ、行動順、回避
	int		LUC;	// 幸運
	int DEF_PHYS;	// 物理防御
	int DEF_HEAT;	// 熱防御
	int DEF_COLD;	// 冷気防御
	int DEF_ELEC;	// 電気防御
	int MUE;		// 使用限界エネルギー量
}BattleStatus;