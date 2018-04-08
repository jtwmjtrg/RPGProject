#include "SkillTree.h"
#include "Character.h"
#include <assert.h>

PlayerChara* SkillTree::chara = nullptr;

SkillTree::SkillTree() {
	// 【スキルツリーのロード】
	if (!LoadData()) {
		// ロード失敗
		Game::mode = eStageMode::FIELD;	// フィールド画面へ移行
	}

	// 【画像】
	gr_back = LoadGraph("img\\skilltree\\back.png");		// 背景
	gr_select = LoadGraph("img\\skilltree\\select.png");	// 選択カーソル
	gr_get = LoadGraph("img\\skilltree\\get.png");			// 取得済みスキル

	// 【音】
	so_BGM = LoadSoundMem("sound\\skillTree.wav");	// BGM
	so_get = LoadSoundMem("sound\\getSkill.wav");	// スキル取得

	// 【変数初期化】
	selectX = 0;	// 選択座標
	selectY = 0;

	scrollX = 0;	// スクロール
	scrollY = 0;

	// 【エフェクト】
	effect = new Effect_SkillTree();

	PlaySoundMem(so_BGM, true);
}

SkillTree::~SkillTree() {
	//SaveData();		// スキルツリーのセーブ

	chara = nullptr;	// キャラクタのポインタを切る

	// 【delete】
	// スキル
	for (auto itr = skills.begin(), end = skills.end(); itr != end; ++itr) {
		itr->second.skill->Release();
		delete itr->second.skill;
		itr->second.skill = nullptr;
	}
	skills.clear();

	// 【画像】
	DeleteGraph(gr_select);		// 選択カーソル
	DeleteGraph(gr_get);		// 取得済みスキル

	// 【音】
	DeleteSoundMem(so_BGM);	// BGM
	DeleteSoundMem(so_get);	// スキル取得
}

// スキルツリーのロード
bool SkillTree::LoadData() {
	if (chara) {
		// 【ファイル読み込み用変数】
		ifstream ifs("savedata\\" + chara->GetName() + "\\skilltree.csv");	// 読み込むファイル
		string line;				// 読み込んだ行（1行）
		string token;				// 1行を,で分割する際に使う
		bool loadTypeFlag = true;	// true:セルのロード	false:スキルデータのロード

		if (ifs.fail()) {
			// 【ファイルが無い】
			return false;	// ロード失敗
		}
		else {
			std::string skillID;
			int countCellX = 0;
			int countCellY = 0;

			while (getline(ifs, line)) {	// 一行ずつ読み込み
				// 読み込んだ行をコピー
				std::istringstream stream(line);
				// 1行目判定
				getline(stream, token, ',');
				if (token[0] == '#') {
					// セルのロード
					if (token == "#cell")	loadTypeFlag = true;
					// スキルデータのロード
					else if (token == "#skills")	loadTypeFlag = false;

					continue;
				}
				if (loadTypeFlag) {
					// 【セルのロード】
					cell.resize(countCellY + 1);	// 行追加
					boost::algorithm::split(cell.back(), line, boost::is_any_of(","));
					countCellX = 0;
					for (int i = 0, n = (int)cell.back().size(); i < n; ++i) {
						if (!cell.back()[i].empty() && cell.back()[i][0] != '%') {
							// スキルリストの追加
							skills[cell.back()[i]] = { countCellX, countCellY };
						}
						++countCellX;
					}
					++countCellY;
				}
				else {
					// 【スキルデータのロード】
					// 1【ID】
					skillID = token;
					// 2【コスト】
					getline(stream, token, ',');
					skills[skillID].cost = boost::lexical_cast<int>(token);
					// 3【取得フラグ】
					getline(stream, token, ',');
					skills[skillID].getFlag = token == "1";
					// 4【前提スキル】
					getline(stream, token, ',');
					boost::algorithm::split(skills[skillID].premiseSkillID, token, boost::is_any_of("!"));
					// 【スキルデータ】
					skills[skillID].skill = new Skill(boost::lexical_cast<int>(skillID));
				}
			}
		}
	}
	else {
		// 【キャラクタがセットされていない】
		return false;	// ロード失敗
	}

	if (cell.empty() || cell.empty()) {
		// 【スキルツリーが無い】
		return false;	// ロード失敗
	}

	return true;
}

// スキルツリーのセーブ
void SkillTree::SaveData() {
	// ファイルオープン
	ofstream ofs("savedata\\" + chara->GetName() + "\\skilltree.csv");
	//1【セルのセーブ】
	ofs << "#cell" <<  endl;
	for (int i = 0, n = (int)cell.size(); i < n; ++i) {
		for (int j = 0, m = (int)cell[i].size()-1; j < m; ++j) {
			ofs << cell[i][j] << ',';
		}
		ofs << cell[i].back();
		ofs << endl;
	}

	//2【スキル】
	ofs << "#skills" << endl;
	for (auto itr = skills.begin(), end = skills.end(); itr != end; ++itr) {
		ofs << itr->first << ',';					// ID
		ofs << itr->second.cost << ',';				// コスト
		ofs << (itr->second.getFlag ? "1," : "0,");	// 取得フラグ
		// 前提スキル
		for (int i = 0, n = (int)itr->second.premiseSkillID.size() - 1; i < n; ++i) {
			ofs << itr->second.premiseSkillID[i] << '!';
		}
		ofs << itr->second.premiseSkillID.back() << endl;
	}
	ofs.close();

	// キャラクタデータのセーブ1
	Player::Save();
	chara->LoadSkillTree();		// スキルを読み直す
}

void SkillTree::Process() {
	// テキストボックスの表示中は毎フレーム処理を行わない
	if (TextBox::textBoxFlag) return;
	
	// 【カーソル移動】
	if ((Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
		|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) && !effect->flag) {	// 左
		int tmpX = selectX - 1;
		int tmpY = selectY;
		bool tmpSelectFlag = false;
		Direction tmpDir = LEFT;
		// 罫線で繋がっているスキルを探索
		while (tmpX >= 0) {
			if (!cell[tmpY][tmpX].empty()) {
				if (cell[tmpY][tmpX][0] == '%') {
					// 【罫線】
					switch (cell[tmpY][tmpX][1]) {
					case '~':// 【┏】
						++tmpY;
						tmpDir = DOWN;
						break;
					case '^':// 【┗】
						--tmpY;
						tmpDir = UP;
						break;
					case '*':// 【┳】
						--tmpX;
						break;
					case '-':// 【━】
						--tmpX;
						break;
					case '/':// 【┻】
						--tmpX;
						break;
					case '+':// 【╋】
						--tmpX;
						break;
					case '=':// 【┛】
						--tmpX;
						break;
					case '$':// 【┓】
						--tmpX;
						break;
					case ':':// 【┃】
						switch (tmpDir) {
						case UP:
							--tmpY;
							break;
						case DOWN:
							++tmpY;
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}
				}
				else {
					// スキル
					tmpSelectFlag = true;
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
			}
			else {
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX - 2;	// 座標リセット
			tmpY = selectY;
			for (int i = 0; i < 2 && tmpX >= 0; ++i, tmpX -= 2) {
				// 上下2マスと奥6マスを探索
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				if (tmpY >= 2 && !cell[tmpY - 2][tmpX].empty() && cell[tmpY - 2][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY - 2;
					break;
				}
				if (tmpY < (int)cell.size() - 2 && !cell[tmpY + 2][tmpX].empty() && cell[tmpY + 2][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY + 2;
					break;
				}
			}
		}
	}
	else if ((Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
		|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) && !effect->flag) {	// 右
		int tmpX = selectX + 1;
		int tmpY = selectY;
		bool tmpSelectFlag = false;
		Direction tmpDir = RIGHT;
		// 罫線で繋がっているスキルを探索
		while (tmpX < (int)cell[tmpY].size()) {
			if (!cell[tmpY][tmpX].empty()) {
				if (cell[tmpY][tmpX][0] == '%') {
					// 【罫線】
					switch (cell[tmpY][tmpX][1]) {
					case '~':// 【┏】
						++tmpX;
						break;
					case '^':// 【┗】
						++tmpX;
						break;
					case '*':// 【┳】
						++tmpX;
						break;
					case '-':// 【━】
						++tmpX;
						break;
					case '/':// 【┻】
						++tmpX;
						break;
					case '+':// 【╋】
						++tmpX;
						break;
					case '=':// 【┛】
						--tmpY;
						tmpDir = UP;
						break;
					case '$':// 【┓】
						++tmpY;
						tmpDir = DOWN;
						break;
					case ':':// 【┃】
						switch (tmpDir)	{
						case UP:
							--tmpY;
							break;
						case DOWN:
							++tmpY;
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}
				}
				else {
					// スキル発見
					tmpSelectFlag = true;
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
			}
			else {
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX + 2;	// 座標リセット
			tmpY = selectY;
			for (int i = 0; i < 2 && tmpX < (int)cell[tmpY].size(); ++i, tmpX += 2) {
				// 上下2マスと奥6マスを探索
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				if (tmpY >= 2 && !cell[tmpY-2][tmpX].empty() && cell[tmpY-2][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY - 2;
					break;
				}
				if (tmpY < (int)cell.size() - 2  && !cell[tmpY+2][tmpX].empty() && cell[tmpY+2][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY + 2;
					break;
				}
			}
		}
	}
	else if ((Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
		|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) && !effect->flag) {	// 上
		int tmpX = selectX;
		int tmpY = selectY - 2;
		bool tmpSelectFlag = false;

		// 左右2マスと奥8マスを探索
		for (int i = 0; i < 3 && tmpY >= 0; ++i, tmpY -= 2) {
			if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX;
				selectY = tmpY;
				break;
			}
			if (tmpX >= 2 && !cell[tmpY][tmpX - 2].empty() && cell[tmpY][tmpX - 2][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX - 2;
				selectY = tmpY;
				break;
			}
			if (tmpX < (int)cell[tmpY].size() - 2 && !cell[tmpY][tmpX + 2].empty() && cell[tmpY][tmpX + 2][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX + 2;
				selectY = tmpY;
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX;	// 座標リセット
			tmpY = selectY;
			while (tmpY >= 0) {
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				--tmpY;
			}
		}
		
	}
	else if ((Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
		|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) && !effect->flag) {	// 下
		int tmpX = selectX;
		int tmpY = selectY + 2;
		bool tmpSelectFlag = false;
		
		// 左右2マスと奥8マスを探索
		for (int i = 0; i < 3 && tmpY < (int)cell.size(); ++i, tmpY += 2) {
			if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX;
				selectY = tmpY;
				break;
			}
			if (tmpX >= 2 && !cell[tmpY][tmpX - 2].empty() && cell[tmpY][tmpX - 2][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX - 2;
				selectY = tmpY;
				break;
			}
			if (tmpX < (int)cell[tmpY].size() - 2 && !cell[tmpY][tmpX + 2].empty() && cell[tmpY][tmpX + 2][0] != '%') {
				// スキル
				tmpSelectFlag = true;
				selectX = tmpX + 2;
				selectY = tmpY;
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX;	// 座標リセット
			tmpY = selectY;

			while (tmpY < (int)cell.size()) {
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// スキル
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				++tmpY;
			}
		}
	}
	
	//DrawStretchGraph(0, 0, gr_select, 256, 64);
	/*const int tmpX = selectX * 160 - scrollX;
	const int tmpY = selectY * 64 - scrollY;
	
	if (tmpX < 256) scrollX = selectX * 160 - 256;
	if (tmpX > 1024) scrollX = selectX * 160 - 1024;
	if (tmpY < 256) scrollY = selectY * 64 - 256;
	if (tmpY > 768) scrollY = selectY * 64 - 768;*/

	scrollX = selectX * 160 + 200 - 960;
	scrollY = selectY * 64 + 128 - 540;

	// 【スキルの取得】
	if ((Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) && !effect->flag) {
		tSkillCell* tmpSkill = &skills[cell[selectY][selectX]];
		// 未取得の場合
		if (!tmpSkill->getFlag) {
			// 前提スキルを全て取得しているか判定
			if (CheckPremiseSkill(tmpSkill)) {
				// コストは0未満でないか スキルポイントが足りているか
				if (chara->DecSkillPoint(tmpSkill->cost)) {
					PlaySoundMem(so_get);
					effect->Set();	// エフェクト
					tmpSkill->getFlag = true;	// スキルを取得状態にする
				}
				else {
					TextBox::AddLine("スキルポイントが足りない");
				}
			}
			else {
				TextBox::AddLine("そのスキルはまだ取得できない");
			}
		}
	}
	// 【フィールドに戻る】
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		Game::mode = eStageMode::FIELD;	// フィールド画面へ移行
	}

	// debug
}
void SkillTree::Draw() {
	string tmp;
	const int skllWid = 256;

	for (int i = 0, n = (int)cell.size(); i < n; ++i) {
		for (int j = 0, m = (int)cell[i].size(); j < m; ++j) {
			if (!cell[i][j].empty()) {
				if (cell[i][j][0] == '%') {
					// 【罫線】
					switch (cell[i][j][1]) {
					case '~':
						tmp = "┏";
						break;
					case '^':
						tmp = "┗";
						break;
					case '*':
						tmp = "┳";
						break;
					case '-':
						tmp = "━";
						break;
					case '/':
						tmp = "┻";
						break;
					case '+':
						tmp = "╋";
						break;
					case '=':
						tmp = "┛";
						break;
					case '$':
						tmp = "┓";
						break;
					case ':':
						tmp = "┃";
						break;
					default:
						break;
					}
				}
				else {
					// スキル
					DrawBox(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - 10 - scrollX, 100 + i * 64 - 10 - scrollY, skllWid, 64, true);
					DrawBox(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) + 192 - 10 - scrollX, 100 + i * 64 - 10 - scrollY, 64, 64, true);
					if (skills[cell[i][j]].getFlag) {
						DrawStretchGraph(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - 5 - scrollX, 100 + i * 64 - 5 - scrollY, gr_get, 190, 54);
					}
					// 前提スキルを全て取得しているか判定
					if (CheckPremiseSkill(&skills[cell[i][j]])) {
						tmp = skills[cell[i][j]].skill->name;
					}
					else {
						// 取得していない場合は非表示にする
						tmp = "???????";
					}
					DrawString(WHITE, 100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) + 192 - scrollX, 100 + i * 64 - scrollY, "%d", skills[cell[i][j]].cost);
				}
				DrawString(WHITE, 100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - scrollX, 100 + i * 64 - scrollY, tmp.c_str());
			}
		}
	}
	DrawStretchGraph(100 + (selectX / 2 * 64 + (selectX + 1) / 2 * skllWid) - 10 - scrollX, 100 + selectY * 64 - 10 - scrollY, gr_select, skllWid, 64);
	
	// テキストボックス表示
	//if (TextBox::textBoxFlag) {
		TextBox::Draw();
	//}
	effect->Draw();		// エフェクト

	DrawGraph(0, 0, gr_back);	// 背景
	DrawString(BLUE, 150, 130, chara->GetName().c_str());
	DrawString(BLUE, 630, 130, "%d", chara->GetSkillPoint());

	const int text_Top = WINDOW_HEIGHT - 300;	// テキストボックスの上端
	tSkillCell* tmpSkill = &skills[cell[selectY][selectX]];

	// 【通常】
	DrawString(WHITE, 784, text_Top + 20, "【 通 常 】");
	// 使用MP
	DrawString(RED, 960 + 96, text_Top + 20, "%d", tmpSkill->skill->useMP[0]);
	DrawString(BLUE, 960 + 192, text_Top + 20, "%d", tmpSkill->skill->useMP[1]);
	DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", tmpSkill->skill->useMP[2]);
	// スキル説明文	
	for (int i = 0, n = tmpSkill->skill->explanatoryText.size(); i < n; ++i) {
		DrawString(WHITE, 784, text_Top + 72 + i * 32, tmpSkill->skill->explanatoryText[i].c_str());
	}
}
void SkillTree::SetData(PlayerChara *chara) {
	if (chara) {
		SkillTree::chara = chara;			// キャラのセット
		Game::mode = eStageMode::SKILLTREE;	// スキルツリー画面へ移行

		// フィールドの音を止める
		StopSoundMem(fieldBGM);
		DeleteSoundMem(fieldBGM);
	}
}
// 前提スキルのチェック
bool SkillTree::CheckPremiseSkill(tSkillCell* skill) {
	// -1は前提スキルなし
	if (skill->premiseSkillID.front() == "-1") {
		return true;
	}

	// 前提スキルを全て走査
	for (int i = 0, n = (int)skill->premiseSkillID.size(); i < n; ++i) {
		if (!skills[skill->premiseSkillID[i]].getFlag) {
			// 一つでも取得していないスキルがあるならfalse
			return false;
		}
	}

	// 全て取得済みならtrue
	return true;
}

Effect_SkillTree::Effect_SkillTree(){
	flag = false;
	count = 0;
	effect = new Effect_Cube(NULL, 0xffffffff, { 10,10,10 }, { -10,-10,-10 });
}

Effect_SkillTree::~Effect_SkillTree() {
}

void Effect_SkillTree::Draw() {
	if (!flag) return;

	// 立方体線描画
	//color += 0x00080808;
	//Geometory::DrawCube(NULL, 0xff000000, { 10,10,10 }, { -10,-10,-10 });
	effect->Draw();

	if (--count < 0) 
		flag = false;
}

void Effect_SkillTree::Set(){
	count = 30;
	effect->InitStatus();
	flag = true;
}

Effect3D						Effect_Cube::m_Effect;		// 幾何オブジェクトエフェクト
IDirect3DVertexDeclaration9*	Effect_Cube::m_pVertexDecl3D;// 3次元幾何オブジェクト頂点構造体宣言

Effect_Cube::Effect_Cube(const D3DXMATRIX* pWorldMtx, DWORD color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max) : Object3D() {
	// エフェクト読み込み
	m_Effect.Create(direct3d.pDevice3D, _T("geometory.cfx"));
	
	// 3次元幾何オブジェクト頂点構造体定義
	decl3d[0] = { 0, 0,	 D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION , 0 };
	decl3d[1] = { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 };
	decl3d[2] = D3DDECL_END();
	
	// 3次元幾何オブジェクト頂点構造体宣言作成
	direct3d.pDevice3D->CreateVertexDeclaration(decl3d, &m_pVertexDecl3D);

	atexit(Effect_Cube::Release);

	if (m_Effect.GetEffect() == NULL) {
		return;
	}
	// デバイス取得
	pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// テクニックセット
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// エフェクトに変換行列セット
	SetEffectParam(pWorldMtx);

	D3DXVECTOR3 min[9] = {
		{ 10, 10, 10 },
		{ 9, 9, 9 },
		{ 8, 8, 8 },
		{ 9.9f, 9.9f, 9.9f },
		{ 8.9f, 8.9f, 8.9f },
		{ 7.9f, 7.9f, 7.9f },
		{ 10.1f, 10.1f, 10.1f },
		{ 9.1f, 9.1f, 9.1f },
		{ 8.1f, 8.1f, 8.1f }
	};
	D3DXVECTOR3 max[9] = {
		{ -10, -10, -10 },
		{ -9, -9, -9 },
		{ -8, -8, -8 },
		{ -9.9f, -9.9f, -9.9f },
		{ -8.9f, -8.9f, -8.9f },
		{ -7.9f, -7.9f, -7.9f },
		{ -10.1f, -10.1f, -10.1f },
		{ -9.1f, -9.1f, -9.1f },
		{ -8.1f, -8.1f, -8.1f }
	};
	
	for (int i = 0; i < 9; ++i) {
		v[i][0].x = min[i].x;	v[i][0].y = min[i].y;	v[i][0].z = min[i].z;
		v[i][1].x = max[i].x;	v[i][1].y = min[i].y;	v[i][1].z = min[i].z;
		v[i][2].x = max[i].x;	v[i][2].y = max[i].y;	v[i][2].z = min[i].z;
		v[i][3].x = min[i].x;	v[i][3].y = max[i].y;	v[i][3].z = min[i].z;
		v[i][4].x = min[i].x;	v[i][4].y = min[i].y;	v[i][4].z = min[i].z;
		v[i][5].x = min[i].x;	v[i][5].y = min[i].y;	v[i][5].z = max[i].z;
		v[i][6].x = min[i].x;	v[i][6].y = max[i].y;	v[i][6].z = max[i].z;
		v[i][7].x = min[i].x;	v[i][7].y = max[i].y;	v[i][7].z = min[i].z;

		v[i][8].x = max[i].x;	v[i][8].y = min[i].y;	v[i][8].z = max[i].z;
		v[i][9].x = min[i].x;	v[i][9].y = min[i].y;	v[i][9].z = max[i].z;
		v[i][10].x = min[i].x; v[i][10].y = max[i].y; v[i][10].z = max[i].z;
		v[i][11].x = max[i].x; v[i][11].y = max[i].y; v[i][11].z = max[i].z;
		v[i][12].x = max[i].x; v[i][12].y = min[i].y; v[i][12].z = max[i].z;
		v[i][13].x = max[i].x; v[i][13].y = min[i].y; v[i][13].z = min[i].z;
		v[i][14].x = max[i].x; v[i][14].y = max[i].y; v[i][14].z = min[i].z;
		v[i][15].x = max[i].x; v[i][15].y = max[i].y; v[i][15].z = max[i].z;

		for (int j = 0; j < sizeof(v[i]) / sizeof(v[i][0]); ++j) {
			v[i][j].color = color;
			v_ini[i][j] = v[i][j];
		}
	}
}
Effect_Cube::~Effect_Cube(){
	// エフェクト終了
	//m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	Effect_Cube::Release();
}
// 解放
void Effect_Cube::Release(){
	// 頂点構造体宣言解放
	if (m_pVertexDecl3D) {
		m_pVertexDecl3D->Release();
		m_pVertexDecl3D = NULL;
	}
}
bool Effect_Cube::SetEffectParam(const D3DXMATRIX* pWorldMtx)
{
	// ワールド変換行列セット
	if (pWorldMtx != NULL) {
		if (!m_Effect.SetMatrix(_T("g_mWorld"), pWorldMtx))
			return false;
	}
	else {
		D3DXMATRIX	Mtx;
		D3DXMatrixIdentity(&Mtx);
		m_Effect.SetMatrix(_T("g_mWorld"), &Mtx);
	}

	// ビュー変換行列セット
	if (!m_Effect.SetMatrix(_T("g_mView"), &Object3D::GetViewMatrix()))
		return false;
	// 射影変換行列セット
	if (!m_Effect.SetMatrix(_T("g_mProj"), &Object3D::GetProjMatrix()))
		return false;

	return true;
}

void Effect_Cube::Draw() {	
	// エフェクト開始
	m_Effect.Begin();

	if (count > 25) {
		core.z += 20.0;
	}
	else {
		core.z += 1.0;
	}

	// 頂点構造体宣言セット
	pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D);
	for (int i = 0; i < 9; ++i) {
		// 描画１
		pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, v[i], sizeof(Vtx3D));
		// 描画２
		pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, &v[i][8], sizeof(Vtx3D));

		
		for (int j = 0; j < sizeof(v[i]) / sizeof(v[i][0]); ++j) {
			v[i][j].color -= 0x08000000;
			v[i][j].color += 0x00080801;
			if (count > 25) {
				v[i][j].z += 20.0;
			}
			else {
				v[i][j].z += 1.0;
			}
			
			float tx = v[i][j].x - core.x;
			float ty = v[i][j].y - core.y;
			float sx2 = tx * cos(thetaX[i]) - ty * sin(thetaX[i]);
			float sy2 = tx * sin(thetaX[i]) + ty * cos(thetaX[i]);
			v[i][j].x = sx2 + core.x;
			v[i][j].y = sy2 + core.y;
			
			float tz = v[i][j].z - core.z;
			ty = v[i][j].y - core.y; 
			float sz2 = tz * cos(thetaY[i]) - ty * sin(thetaY[i]);
			sy2 = tz * sin(thetaY[i]) + ty * cos(thetaY[i]);
			v[i][j].z = sz2 + core.z;
			v[i][j].y = sy2 + core.y;

			tz = v[i][j].z - core.z;
			tx = v[i][j].x - core.x;
			sz2 = tz * cos(thetaZ[i]) - tx * sin(thetaZ[i]);
			sx2 = tz * sin(thetaZ[i]) + tx * cos(thetaZ[i]);
			v[i][j].z = sz2 + core.z;
			v[i][j].x = sx2 + core.x;
		}
	}

	// エフェクト終了
	m_Effect.End();	
	--count;
}

void Effect_Cube::InitStatus() {
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < sizeof(v[i]) / sizeof(v[i][0]); ++j) {
			v[i][j] = v_ini[i][j];
			if (i < 3) {
				//v[i][j].color = 0xff000000 | ((unsigned int)GetRand(128, 255) << 16) | ((unsigned int)GetRand(128, 255) << 8) | ((unsigned int)GetRand(128, 255));
				v[i][j].color = 0xff0000e0;
			}
			else {
				v[i][j].color = v[i % 3][j].color;
			}
		}
		if (i < 3) {
			thetaX[i] = GetRand(-3, 3) * M_PI / 180.0f;
			thetaY[i] = GetRand(-3, 3) * M_PI / 180.0f;
			thetaZ[i] = GetRand(-3, 3) * M_PI / 180.0f;
		}
		else {
			thetaX[i] = thetaX[i % 3];
			thetaY[i] = thetaY[i % 3];
			thetaZ[i] = thetaY[i % 3];
		}
	}
	count = 30;
	core.x = 0.0f;
	core.y = 0.0f;
	core.z = 0.0f;
}
