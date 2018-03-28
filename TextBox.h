#pragma once
#include "graphicdata.h"
#include <deque>
#include "input.h"
#include "Define.h"

// 名前付け
#define DrawBox					TextBox::DrawBox
#define DrawTextBox				TextBox::DrawTextBox
#define DrawConversationBox		TextBox::DrawConversationBox

// 色
#define WHITE	0xffffffff	// 白
#define BLACK	0xff000000	// 黒
#define RED		0xffff4040	// 赤
#define GREEN	0xff00ff00	// 緑
#define BLUE	0xff0000ff	// 青
#define YELLOW	0xffffff00	// 黄

// 表示テキストの種類
enum eDisplayType {
	textLine,		// 普通のテキスト
	selectText,		// 選択分岐
};

// 選択分岐データ
struct tSelectBranch {
	string answer;		// 選択肢文
	int destination;	// 分岐先（挙動番号をn進める）
};

class TextLine;
class SelectText;

// 表示テキストスーパークラス
class DisplayText {
public:
	DisplayText() {};
	~DisplayText() {};
	eDisplayType type;	// 表示テキストの種類

	TextLine* GetTextLine() { return (TextLine*)this; }
	SelectText* GetSelectText() { return (SelectText*)this; }
};
// 普通のテキスト
class TextLine : public DisplayText {
public:
	const vector<string> str;	// 描画する文字列
	const int speed;		// 文字送りスピード		-1は文字送りなし（一瞬で全表示）
	int delayLine;	// 自動改行のフレーム数			-1は自動なし
	int delayPage;	// 自動ページ送りのフレーム数	-1は自動なし

	TextLine(vector<string> str, int speed, int delayLine, int delayPage);
	~TextLine(){}
};

// 選択分岐
class SelectText : public DisplayText {
public:
	const vector<string> str;	// 描画する文字列
	const int speed;			// 文字送りスピード			-1は文字送りなし（一瞬で全表示）
	int delayLine;				// 自動改行のフレーム数		-1は自動なし
	std::vector<tSelectBranch> choices;	// 選択肢
	bool& selectFlag;			// 選択フラグ
	int& select;				// 選択番号
	bool strEndFlag;			// 全部表示したかのフラグ

	SelectText(vector<string> str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine);
	~SelectText() {}
};


class TextBox	// 静的クラス
{
private:
	// 画像
	static int gr_box;		// 黒背景
	static int line;	// 白線

	// 制御系
	static int maxLine;		// １ページに何行はいるか
	static int countText;	// 文字送りカウント
	static int countLine;	// 自動改行カウント
	static int lineNum;		// 表示中の行数

	// ステータス系
	static int			line_width;	// 白線の幅
	static deque<DisplayText*>	strLine;	// 描画する文字列

public:
	// 初期化系
	static void InitTextBox();	// 初期化
	static void Destroy();		// 終了処理

	// メインループ
	static void Process();	// 毎フレーム処理
	static void Draw();	// 描画

	// ステータス系
	static bool textBoxFlag;	// テキストフラグ

	// セッター
	static void AddLine(string str, int speed = 2, int delayLine = 0, int delayPage = -1);
	static void AddLine(std::vector<string> str, int speed = 2, int delayLine = 0, int delayPage = -1);	// 描画する文字列追加

	static void AddSelect(string str,std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed = 2, int delayLine = 0);

	// 基本関数
	static void DrawBox(int X, int Y, int width, int height, bool fill);		// 矩形描写
	static void DrawTextBox(int X, int Y, string text, int width, int height);	// 大きさ指定描画

	static void Reset();	// 解放処理
};