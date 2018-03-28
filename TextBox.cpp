#include "TextBox.h"

// 画像
int TextBox::gr_box;	// 黒背景
int TextBox::line;		// 枠線

// ステータス系
int TextBox::line_width;	// 枠線の太さ
deque<DisplayText*> TextBox::strLine;	// 描画する文字列
bool TextBox::textBoxFlag;	// テキストフラグ

// 制御系
int TextBox::maxLine;	// １ページに最大何行の文字列を描写するか
int TextBox::countText;	// 文字送りカウント
int TextBox::countLine;	// 自動改行カウント
int TextBox::lineNum;	// 表示中の行数

// 【テキストクラス】
TextLine::TextLine(vector<string> str, int speed, int delayLine, int delayPage) :
	str(str),
	speed(speed),
	delayLine(delayLine),
	delayPage(delayPage)
{
	type = eDisplayType::textLine;
}

// 【選択分岐】
SelectText::SelectText(vector<string> str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine):
	str(str),
	choices(choices),
	selectFlag(selectFlag),
	select(select),
	speed(speed),
	delayLine(delayLine)
{
	type = eDisplayType::selectText;
	strEndFlag = false;
}


void TextBox::InitTextBox(){
	// 画像
	gr_box = LoadGraph("img\\textbox\\box.png");	// 黒背景
	line = LoadGraph("img\\textbox\\line.png");	// 枠線

	// 制御系
	maxLine	= 5;	// １ページに最大何行の文字列を描写するか
	countText = 0;	// 文字送りカウント
	countLine = 0;	// 自動改行カウント
	lineNum	= 0;	// 表示中の行数

	// ステータス系
	textBoxFlag = false;	// テキストフラグ
	line_width = 5;			// 枠線の太さ
}
void TextBox::Destroy(){
	// 画像削除
	DeleteGraph(gr_box);	// 黒背景
	DeleteGraph(line);	// 枠線
	Reset();
}

void TextBox::DrawBox(int X, int Y, int width, int height, bool fill){
	// 塗りつぶし
	if (fill){
		DrawStretchGraph(X, Y, gr_box, width, height);
	}
	// 枠線
	DrawStretchGraph(X, Y, line, width, line_width);						// 上
	DrawStretchGraph(X, Y + height - line_width, line, width, line_width);	// 下
	DrawStretchGraph(X, Y, line, line_width, height);						// 左
	DrawStretchGraph(X + width - line_width, Y, line, line_width, height);	// 右
}
void TextBox::DrawTextBox(int X, int Y, string text, int width, int height){
	// 塗りつぶし
	DrawStretchGraph(X, Y, gr_box, width, height);
	// 枠線
	DrawStretchGraph(X, Y, line, width, line_width);						// 上
	DrawStretchGraph(X, Y + height - line_width, line, width, line_width);	// 下
	DrawStretchGraph(X, Y, line, line_width, height);						// 左
	DrawStretchGraph(X + width - line_width, Y, line, line_width, height);	// 右
	// 文字列描写
	DrawString(WHITE, X + 16, Y + 16, text.c_str());
}

void TextBox::Process() {
	// 表示すべき文字列がない
	if (strLine.empty()) {
		textBoxFlag = false;	// フラグを折る
	}
	// ある場合
	else {
		textBoxFlag = true;		// フラグを立てる

		switch (strLine.front()->type){
		case eDisplayType::textLine: // 【通常テキスト】
		{
			// テキスト文字送り中 || 次の行がある
			if ((unsigned int)(countText / strLine.front()->GetTextLine()->speed) <= strLine.front()->GetTextLine()->str[lineNum].size() || (unsigned)lineNum < strLine.front()->GetTextLine()->str.size() - 1) {

				// その行の最後まで読んだ && 次の行がある
				if (((unsigned int)(countText / strLine.front()->GetTextLine()->speed) > strLine.front()->GetTextLine()->str[lineNum].size()) && ((unsigned)lineNum < strLine.front()->GetTextLine()->str.size() - 1)) {
					// ボタン or 自動で次の行に移る
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) || countLine == strLine.front()->GetTextLine()->delayLine) {
						countText = 0;	// 文字送りカウントリセット
						countLine = 0;	// 自動改行カウント
						lineNum++;		// 次の行を表示し始める
					}
					else {
						countLine++;	// 自動改行カウントアップ
						// テキストを全部表示
						if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
							lineNum = strLine.front()->GetTextLine()->str.size() - 1;
							countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// 文字送りカウント
							countLine = 0;	// 自動改行カウント
						}
					}
				}
				else {
					countText++;	// 文字送りカウントアップ
					// テキストを全部表示
					if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
						lineNum = strLine.front()->GetTextLine()->str.size() - 1;
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// 文字送りカウント
					}
					// テキストを行の最後まで表示
					else if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;
					}
				}
			}
			else {
				// 全部表示済
				if ((Input::Get(VK_Z) == 2 || Input::Get(VK_V) == 2 || Input::Get(0, XINPUT_GAMEPAD_A) == 2 || Input::Get(0, XINPUT_GAMEPAD_Y) == 2 || Input::Get(0, XINPUT_GAMEPAD_B) == 2) || strLine.front()->GetTextLine()->delayPage == 0) {
					delete strLine.front();	// データ削除
					strLine.pop_front();// 次のテキストにうつる
					countText = 0;		// カウントリセット
					lineNum = 0;		// 次のページの一行目から表示し始める
					// 次のページが無ければ
					if (strLine.empty()) {
						textBoxFlag = false;	// フラグを折る
					}
				}
				else {
					strLine.front()->GetTextLine()->delayPage--;	// 自動ページ送りカウントダウン
				}
			}
			break;
		}
		case eDisplayType::selectText: // 【選択分岐】
		{
			// テキスト文字送り中 || 次の行がある
			if ((unsigned int)(countText / strLine.front()->GetSelectText()->speed) <= strLine.front()->GetSelectText()->str[lineNum].size() || (unsigned)lineNum < strLine.front()->GetSelectText()->str.size() - 1) {

				// その行の最後まで読んだ && 次の行がある
				if (((unsigned int)(countText / strLine.front()->GetSelectText()->speed) > strLine.front()->GetSelectText()->str[lineNum].size()) && ((unsigned)lineNum < strLine.front()->GetSelectText()->str.size() - 1)) {
					// ボタン or 自動で次の行に移る
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1 || Input::Get(0, XINPUT_GAMEPAD_Y) == 1) || countLine == strLine.front()->GetSelectText()->delayLine) {
						countText = 0;	// 文字送りカウントリセット
						countLine = 0;	// 自動改行カウント
						lineNum++;		// 次の行を表示し始める
					}
					else {
						countLine++;	// 自動改行カウントアップ
					}
				}
				else {
					countText++;	// 文字送りカウントアップ
					// テキストを全部表示
					if (Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
						lineNum = strLine.front()->GetTextLine()->str.size() - 1;
						countText = strLine.front()->GetTextLine()->str[lineNum].size() * strLine.front()->GetTextLine()->speed - 1;	// 文字送りカウント
					}
					// テキストを行の最後まで表示
					if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1)) {
						countText = strLine.front()->GetSelectText()->str[lineNum].size() * strLine.front()->GetSelectText()->speed - 1;
					}
				}
			}
			else {
				strLine.front()->GetSelectText()->strEndFlag = true;
				// カーソル移動
				if (Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
					|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) {
					if (strLine.front()->GetSelectText()->select == -1) {
						// 連打して間違って選択しないように
						strLine.front()->GetSelectText()->select = 0;
					}
					else {
						strLine.front()->GetSelectText()->select = (strLine.front()->GetSelectText()->select + (strLine.front()->GetSelectText()->choices.size() - 1)) % strLine.front()->GetSelectText()->choices.size();
					}
				}
				if (Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
					|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) {
					if (strLine.front()->GetSelectText()->select == -1) {
						// 連打して間違って選択しないように
						strLine.front()->GetSelectText()->select = 0;
					}
					else {
						strLine.front()->GetSelectText()->select = (strLine.front()->GetSelectText()->select + 1) % strLine.front()->GetSelectText()->choices.size();
					}
				}
				// 決定
				if ((Input::Get(VK_Z) == 1 || Input::Get(VK_V) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1|| Input::Get(0, XINPUT_GAMEPAD_Y) == 1) &&
					strLine.front()->GetSelectText()->select != -1) {	// 連打して間違って選択しないように

					strLine.front()->GetSelectText()->selectFlag = true;	// 選択フラグを立てる

					delete strLine.front();	// データ削除
					strLine.pop_front();// 次のテキストにうつる
					countText = 0;		// カウントリセット
					lineNum = 0;		// 次のページの一行目から表示し始める
										// 次のページが無ければ
					if (strLine.empty()) {
						textBoxFlag = false;	// フラグを折る
					}
				}
			}
			break;
		}
		default: 
			delete strLine.front();
			strLine.pop_front();// 次のテキストにうつる
			break;
		}

		
	}
}

void TextBox::Draw(){
	// 塗りつぶし
	DrawStretchGraph(0, WINDOW_HEIGHT - 300, gr_box, WINDOW_WIDTH, WINDOW_HEIGHT);
	// 枠線
	DrawStretchGraph(0,	WINDOW_HEIGHT - 300, line, WINDOW_WIDTH, line_width);							// 上
	DrawStretchGraph(0, WINDOW_HEIGHT - line_width, line, WINDOW_WIDTH, line_width);					// 下
	DrawStretchGraph(0, WINDOW_HEIGHT - 300, line, line_width, 1080);									// 左
	DrawStretchGraph(WINDOW_WIDTH - line_width, WINDOW_HEIGHT - 300, line, line_width, WINDOW_HEIGHT);	// 右

	// テキストが空なら表示しない
	if (!textBoxFlag) return;

	switch (strLine.front()->type)
	{
	case eDisplayType::textLine:// 【通常テキスト】
	{
		// 表示している文字列が描写できる最大数を超えているかどうか
		if (lineNum < maxLine) {
			// 既に表示が終わっている行を表示する
			for (int i = 0; i < lineNum; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[i].c_str());
			}
			// 最新の行
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (lineNum * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		else {
			// 既に表示が終わっている行を表示する
			for (int i = 0; i < maxLine - 1; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[lineNum - (maxLine - 1) + i].c_str());
			}
			// 最新の行
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + ((maxLine - 1) * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		break;
	}
	case eDisplayType::selectText:// 【選択分岐】
	{
		// 表示している文字列が描写できる最大数を超えているかどうか
		if (lineNum < maxLine) {
			// 既に表示が終わっている行を表示する
			for (int i = 0; i < lineNum; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[i].c_str());
			}
			// 最新の行
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (lineNum * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}
		else {
			// 既に表示が終わっている行を表示する
			for (int i = 0; i < maxLine - 1; i++) {
				DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + (i * 32), strLine.front()->GetTextLine()->str[lineNum - (maxLine - 1) + i].c_str());
			}
			// 最新の行
			DrawString(WHITE, 16, (WINDOW_HEIGHT - 300) + 16 + ((maxLine - 1) * 32), strLine.front()->GetTextLine()->str[lineNum].substr(0, (int)(countText / strLine.front()->GetTextLine()->speed)).c_str());
		}

		// 文を全部表示済なら選択ボックスを表示
		if (strLine.front()->GetSelectText()->strEndFlag) {
			// 縦幅
			int selectHeight = strLine.front()->GetSelectText()->choices.size() * 48 + 32;
			// 横幅
			int selectWidth = strLine.front()->GetSelectText()->choices[0].answer.size() * 16 + 96;
			for (int i = 1, n = strLine.front()->GetSelectText()->choices.size(); i < n; ++i) {
				if (selectWidth < (int)strLine.front()->GetSelectText()->choices[i].answer.size() * 16 + 96) {
					selectWidth = strLine.front()->GetSelectText()->choices[i].answer.size() * 16 + 96;
				}
			}
			// 塗りつぶし
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, gr_box, selectWidth, selectHeight);
			// 枠線
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, line, selectWidth, line_width);	// 上
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - line_width, line, selectWidth, line_width);		// 下
			DrawStretchGraph(320, WINDOW_HEIGHT - 320 - selectHeight, line, line_width, selectHeight);	// 左
			DrawStretchGraph(320 + selectWidth - line_width, WINDOW_HEIGHT - 320 - selectHeight, line, line_width, selectHeight);	// 右
			// 選択肢
			for (int i = 0, n = strLine.front()->GetSelectText()->choices.size(); i < n; ++i) {
				DrawString(WHITE, 384, WINDOW_HEIGHT - 300 - selectHeight + i * 48, strLine.front()->GetSelectText()->choices[i].answer.c_str());
			}
			// カーソル
			if (strLine.front()->GetSelectText()->select != -1) {
				DrawString(WHITE, 340, WINDOW_HEIGHT - 300 - selectHeight + strLine.front()->GetSelectText()->select * 48, "◆");
			}
		}
		break;
	}
	default:
		break;
	}
}

void TextBox::AddLine(string str, int speed, int delayLine, int delayPage) {
	vector<string> vectStr;		// キャスト用
	vectStr.push_back(str);		// 代入
	if (speed == 0)	speed = -1;		// スピード0はエラー吐く

	strLine.push_back(new TextLine(vectStr, speed, delayLine, delayPage));		// 一番後ろのページとして登録する

	textBoxFlag = true;		// テキストフラグを立てる
}

void TextBox::AddLine(vector<string> str, int speed, int delayLine, int delayPage) {
	if (str.empty()) return;		// 空のヴェクターは入れない
	if (speed == 0)	speed = -1;		// スピード0はエラー吐く

	strLine.push_back(new TextLine(str , speed , delayLine ,delayPage));		// 一番後ろのページとして登録する

	textBoxFlag = true;		// テキストフラグを立てる
}

void TextBox::AddSelect(string str, std::vector<tSelectBranch> choices, bool& selectFlag, int& select, int speed, int delayLine) {
	vector<string> vectStr;		// キャスト用
	vectStr.push_back(str);		// 代入
	if (speed == 0)	speed = -1;	// スピード0はエラー吐く

	strLine.push_back(new SelectText(vectStr, choices, selectFlag, select, speed, delayLine));

	textBoxFlag = true;		// テキストフラグを立てる
}

// 解放処理
void TextBox::Reset() {
	for (int i = 0, n = (int)strLine.size(); i < n; ++i) {
		delete strLine[i];
	}
	strLine.clear();
	strLine.shrink_to_fit();

	textBoxFlag = false;
	lineNum = 0;
	countLine = 0;
}