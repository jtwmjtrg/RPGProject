#include "SkillTree.h"
#include "Character.h"
#include <assert.h>

PlayerChara* SkillTree::chara = nullptr;

SkillTree::SkillTree() {
	// �y�X�L���c���[�̃��[�h�z
	if (!LoadData()) {
		// ���[�h���s
		Game::mode = eStageMode::FIELD;	// �t�B�[���h��ʂֈڍs
	}

	// �y�摜�z
	gr_back = LoadGraph("img\\skilltree\\back.png");		// �w�i
	gr_select = LoadGraph("img\\skilltree\\select.png");	// �I���J�[�\��
	gr_get = LoadGraph("img\\skilltree\\get.png");			// �擾�ς݃X�L��

	// �y���z
	so_BGM = LoadSoundMem("sound\\skillTree.wav");	// BGM
	so_get = LoadSoundMem("sound\\getSkill.wav");	// �X�L���擾

	// �y�ϐ��������z
	selectX = 0;	// �I�����W
	selectY = 0;

	scrollX = 0;	// �X�N���[��
	scrollY = 0;

	// �y�G�t�F�N�g�z
	effect = new Effect_SkillTree();

	PlaySoundMem(so_BGM, true);
}

SkillTree::~SkillTree() {
	//SaveData();		// �X�L���c���[�̃Z�[�u

	chara = nullptr;	// �L�����N�^�̃|�C���^��؂�

	// �ydelete�z
	// �X�L��
	for (auto itr = skills.begin(), end = skills.end(); itr != end; ++itr) {
		itr->second.skill->Release();
		delete itr->second.skill;
		itr->second.skill = nullptr;
	}
	skills.clear();

	// �y�摜�z
	DeleteGraph(gr_select);		// �I���J�[�\��
	DeleteGraph(gr_get);		// �擾�ς݃X�L��

	// �y���z
	DeleteSoundMem(so_BGM);	// BGM
	DeleteSoundMem(so_get);	// �X�L���擾
}

// �X�L���c���[�̃��[�h
bool SkillTree::LoadData() {
	if (chara) {
		// �y�t�@�C���ǂݍ��ݗp�ϐ��z
		ifstream ifs("savedata\\" + chara->GetName() + "\\skilltree.csv");	// �ǂݍ��ރt�@�C��
		string line;				// �ǂݍ��񂾍s�i1�s�j
		string token;				// 1�s��,�ŕ�������ۂɎg��
		bool loadTypeFlag = true;	// true:�Z���̃��[�h	false:�X�L���f�[�^�̃��[�h

		if (ifs.fail()) {
			// �y�t�@�C���������z
			return false;	// ���[�h���s
		}
		else {
			std::string skillID;
			int countCellX = 0;
			int countCellY = 0;

			while (getline(ifs, line)) {	// ��s���ǂݍ���
				// �ǂݍ��񂾍s���R�s�[
				std::istringstream stream(line);
				// 1�s�ڔ���
				getline(stream, token, ',');
				if (token[0] == '#') {
					// �Z���̃��[�h
					if (token == "#cell")	loadTypeFlag = true;
					// �X�L���f�[�^�̃��[�h
					else if (token == "#skills")	loadTypeFlag = false;

					continue;
				}
				if (loadTypeFlag) {
					// �y�Z���̃��[�h�z
					cell.resize(countCellY + 1);	// �s�ǉ�
					boost::algorithm::split(cell.back(), line, boost::is_any_of(","));
					countCellX = 0;
					for (int i = 0, n = (int)cell.back().size(); i < n; ++i) {
						if (!cell.back()[i].empty() && cell.back()[i][0] != '%') {
							// �X�L�����X�g�̒ǉ�
							skills[cell.back()[i]] = { countCellX, countCellY };
						}
						++countCellX;
					}
					++countCellY;
				}
				else {
					// �y�X�L���f�[�^�̃��[�h�z
					// 1�yID�z
					skillID = token;
					// 2�y�R�X�g�z
					getline(stream, token, ',');
					skills[skillID].cost = boost::lexical_cast<int>(token);
					// 3�y�擾�t���O�z
					getline(stream, token, ',');
					skills[skillID].getFlag = token == "1";
					// 4�y�O��X�L���z
					getline(stream, token, ',');
					boost::algorithm::split(skills[skillID].premiseSkillID, token, boost::is_any_of("!"));
					// �y�X�L���f�[�^�z
					skills[skillID].skill = new Skill(boost::lexical_cast<int>(skillID));
				}
			}
		}
	}
	else {
		// �y�L�����N�^���Z�b�g����Ă��Ȃ��z
		return false;	// ���[�h���s
	}

	if (cell.empty() || cell.empty()) {
		// �y�X�L���c���[�������z
		return false;	// ���[�h���s
	}

	return true;
}

// �X�L���c���[�̃Z�[�u
void SkillTree::SaveData() {
	// �t�@�C���I�[�v��
	ofstream ofs("savedata\\" + chara->GetName() + "\\skilltree.csv");
	//1�y�Z���̃Z�[�u�z
	ofs << "#cell" <<  endl;
	for (int i = 0, n = (int)cell.size(); i < n; ++i) {
		for (int j = 0, m = (int)cell[i].size()-1; j < m; ++j) {
			ofs << cell[i][j] << ',';
		}
		ofs << cell[i].back();
		ofs << endl;
	}

	//2�y�X�L���z
	ofs << "#skills" << endl;
	for (auto itr = skills.begin(), end = skills.end(); itr != end; ++itr) {
		ofs << itr->first << ',';					// ID
		ofs << itr->second.cost << ',';				// �R�X�g
		ofs << (itr->second.getFlag ? "1," : "0,");	// �擾�t���O
		// �O��X�L��
		for (int i = 0, n = (int)itr->second.premiseSkillID.size() - 1; i < n; ++i) {
			ofs << itr->second.premiseSkillID[i] << '!';
		}
		ofs << itr->second.premiseSkillID.back() << endl;
	}
	ofs.close();

	// �L�����N�^�f�[�^�̃Z�[�u1
	Player::Save();
	chara->LoadSkillTree();		// �X�L����ǂݒ���
}

void SkillTree::Process() {
	// �e�L�X�g�{�b�N�X�̕\�����͖��t���[���������s��Ȃ�
	if (TextBox::textBoxFlag) return;
	
	// �y�J�[�\���ړ��z
	if ((Input::Get(VK_LEFT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) == 1
		|| (Input::Get(VK_LEFT) > 10 && Input::Get(VK_LEFT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_LEFT) % 5 == 0)) && !effect->flag) {	// ��
		int tmpX = selectX - 1;
		int tmpY = selectY;
		bool tmpSelectFlag = false;
		Direction tmpDir = LEFT;
		// �r���Ōq�����Ă���X�L����T��
		while (tmpX >= 0) {
			if (!cell[tmpY][tmpX].empty()) {
				if (cell[tmpY][tmpX][0] == '%') {
					// �y�r���z
					switch (cell[tmpY][tmpX][1]) {
					case '~':// �y���z
						++tmpY;
						tmpDir = DOWN;
						break;
					case '^':// �y���z
						--tmpY;
						tmpDir = UP;
						break;
					case '*':// �y���z
						--tmpX;
						break;
					case '-':// �y���z
						--tmpX;
						break;
					case '/':// �y���z
						--tmpX;
						break;
					case '+':// �y���z
						--tmpX;
						break;
					case '=':// �y���z
						--tmpX;
						break;
					case '$':// �y���z
						--tmpX;
						break;
					case ':':// �y���z
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
					// �X�L��
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
			tmpX = selectX - 2;	// ���W���Z�b�g
			tmpY = selectY;
			for (int i = 0; i < 2 && tmpX >= 0; ++i, tmpX -= 2) {
				// �㉺2�}�X�Ɖ�6�}�X��T��
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				if (tmpY >= 2 && !cell[tmpY - 2][tmpX].empty() && cell[tmpY - 2][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY - 2;
					break;
				}
				if (tmpY < (int)cell.size() - 2 && !cell[tmpY + 2][tmpX].empty() && cell[tmpY + 2][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY + 2;
					break;
				}
			}
		}
	}
	else if ((Input::Get(VK_RIGHT) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) == 1
		|| (Input::Get(VK_RIGHT) > 10 && Input::Get(VK_RIGHT) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_RIGHT) % 5 == 0)) && !effect->flag) {	// �E
		int tmpX = selectX + 1;
		int tmpY = selectY;
		bool tmpSelectFlag = false;
		Direction tmpDir = RIGHT;
		// �r���Ōq�����Ă���X�L����T��
		while (tmpX < (int)cell[tmpY].size()) {
			if (!cell[tmpY][tmpX].empty()) {
				if (cell[tmpY][tmpX][0] == '%') {
					// �y�r���z
					switch (cell[tmpY][tmpX][1]) {
					case '~':// �y���z
						++tmpX;
						break;
					case '^':// �y���z
						++tmpX;
						break;
					case '*':// �y���z
						++tmpX;
						break;
					case '-':// �y���z
						++tmpX;
						break;
					case '/':// �y���z
						++tmpX;
						break;
					case '+':// �y���z
						++tmpX;
						break;
					case '=':// �y���z
						--tmpY;
						tmpDir = UP;
						break;
					case '$':// �y���z
						++tmpY;
						tmpDir = DOWN;
						break;
					case ':':// �y���z
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
					// �X�L������
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
			tmpX = selectX + 2;	// ���W���Z�b�g
			tmpY = selectY;
			for (int i = 0; i < 2 && tmpX < (int)cell[tmpY].size(); ++i, tmpX += 2) {
				// �㉺2�}�X�Ɖ�6�}�X��T��
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				if (tmpY >= 2 && !cell[tmpY-2][tmpX].empty() && cell[tmpY-2][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY - 2;
					break;
				}
				if (tmpY < (int)cell.size() - 2  && !cell[tmpY+2][tmpX].empty() && cell[tmpY+2][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY + 2;
					break;
				}
			}
		}
	}
	else if ((Input::Get(VK_UP) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) == 1
		|| (Input::Get(VK_UP) > 10 && Input::Get(VK_UP) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_UP) % 5 == 0)) && !effect->flag) {	// ��
		int tmpX = selectX;
		int tmpY = selectY - 2;
		bool tmpSelectFlag = false;

		// ���E2�}�X�Ɖ�8�}�X��T��
		for (int i = 0; i < 3 && tmpY >= 0; ++i, tmpY -= 2) {
			if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX;
				selectY = tmpY;
				break;
			}
			if (tmpX >= 2 && !cell[tmpY][tmpX - 2].empty() && cell[tmpY][tmpX - 2][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX - 2;
				selectY = tmpY;
				break;
			}
			if (tmpX < (int)cell[tmpY].size() - 2 && !cell[tmpY][tmpX + 2].empty() && cell[tmpY][tmpX + 2][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX + 2;
				selectY = tmpY;
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX;	// ���W���Z�b�g
			tmpY = selectY;
			while (tmpY >= 0) {
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// �X�L��
					selectX = tmpX;
					selectY = tmpY;
					break;
				}
				--tmpY;
			}
		}
		
	}
	else if ((Input::Get(VK_DOWN) == 1 || Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) == 1
		|| (Input::Get(VK_DOWN) > 10 && Input::Get(VK_DOWN) % 5 == 0) || (Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) > 10 && Input::Get(0, XINPUT_GAMEPAD_DPAD_DOWN) % 5 == 0)) && !effect->flag) {	// ��
		int tmpX = selectX;
		int tmpY = selectY + 2;
		bool tmpSelectFlag = false;
		
		// ���E2�}�X�Ɖ�8�}�X��T��
		for (int i = 0; i < 3 && tmpY < (int)cell.size(); ++i, tmpY += 2) {
			if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX;
				selectY = tmpY;
				break;
			}
			if (tmpX >= 2 && !cell[tmpY][tmpX - 2].empty() && cell[tmpY][tmpX - 2][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX - 2;
				selectY = tmpY;
				break;
			}
			if (tmpX < (int)cell[tmpY].size() - 2 && !cell[tmpY][tmpX + 2].empty() && cell[tmpY][tmpX + 2][0] != '%') {
				// �X�L��
				tmpSelectFlag = true;
				selectX = tmpX + 2;
				selectY = tmpY;
				break;
			}
		}
		if (!tmpSelectFlag) {
			tmpX = selectX;	// ���W���Z�b�g
			tmpY = selectY;

			while (tmpY < (int)cell.size()) {
				if (!cell[tmpY][tmpX].empty() && cell[tmpY][tmpX][0] != '%') {
					// �X�L��
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

	// �y�X�L���̎擾�z
	if ((Input::Get(VK_Z) == 1 || Input::Get(0, XINPUT_GAMEPAD_A) == 1) && !effect->flag) {
		tSkillCell* tmpSkill = &skills[cell[selectY][selectX]];
		// ���擾�̏ꍇ
		if (!tmpSkill->getFlag) {
			// �O��X�L����S�Ď擾���Ă��邩����
			if (CheckPremiseSkill(tmpSkill)) {
				// �R�X�g��0�����łȂ��� �X�L���|�C���g������Ă��邩
				if (chara->DecSkillPoint(tmpSkill->cost)) {
					PlaySoundMem(so_get);
					effect->Set();	// �G�t�F�N�g
					tmpSkill->getFlag = true;	// �X�L�����擾��Ԃɂ���
				}
				else {
					TextBox::AddLine("�X�L���|�C���g������Ȃ�");
				}
			}
			else {
				TextBox::AddLine("���̃X�L���͂܂��擾�ł��Ȃ�");
			}
		}
	}
	// �y�t�B�[���h�ɖ߂�z
	if (Input::Get(VK_X) == 1 || Input::Get(0, XINPUT_GAMEPAD_B) == 1) {
		Game::mode = eStageMode::FIELD;	// �t�B�[���h��ʂֈڍs
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
					// �y�r���z
					switch (cell[i][j][1]) {
					case '~':
						tmp = "��";
						break;
					case '^':
						tmp = "��";
						break;
					case '*':
						tmp = "��";
						break;
					case '-':
						tmp = "��";
						break;
					case '/':
						tmp = "��";
						break;
					case '+':
						tmp = "��";
						break;
					case '=':
						tmp = "��";
						break;
					case '$':
						tmp = "��";
						break;
					case ':':
						tmp = "��";
						break;
					default:
						break;
					}
				}
				else {
					// �X�L��
					DrawBox(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - 10 - scrollX, 100 + i * 64 - 10 - scrollY, skllWid, 64, true);
					DrawBox(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) + 192 - 10 - scrollX, 100 + i * 64 - 10 - scrollY, 64, 64, true);
					if (skills[cell[i][j]].getFlag) {
						DrawStretchGraph(100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - 5 - scrollX, 100 + i * 64 - 5 - scrollY, gr_get, 190, 54);
					}
					// �O��X�L����S�Ď擾���Ă��邩����
					if (CheckPremiseSkill(&skills[cell[i][j]])) {
						tmp = skills[cell[i][j]].skill->name;
					}
					else {
						// �擾���Ă��Ȃ��ꍇ�͔�\���ɂ���
						tmp = "???????";
					}
					DrawString(WHITE, 100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) + 192 - scrollX, 100 + i * 64 - scrollY, "%d", skills[cell[i][j]].cost);
				}
				DrawString(WHITE, 100 + (j / 2 * 64 + (j + 1) / 2 * skllWid) - scrollX, 100 + i * 64 - scrollY, tmp.c_str());
			}
		}
	}
	DrawStretchGraph(100 + (selectX / 2 * 64 + (selectX + 1) / 2 * skllWid) - 10 - scrollX, 100 + selectY * 64 - 10 - scrollY, gr_select, skllWid, 64);
	
	// �e�L�X�g�{�b�N�X�\��
	//if (TextBox::textBoxFlag) {
		TextBox::Draw();
	//}
	effect->Draw();		// �G�t�F�N�g

	DrawGraph(0, 0, gr_back);	// �w�i
	DrawString(BLUE, 150, 130, chara->GetName().c_str());
	DrawString(BLUE, 630, 130, "%d", chara->GetSkillPoint());

	const int text_Top = WINDOW_HEIGHT - 300;	// �e�L�X�g�{�b�N�X�̏�[
	tSkillCell* tmpSkill = &skills[cell[selectY][selectX]];

	// �y�ʏ�z
	DrawString(WHITE, 784, text_Top + 20, "�y �� �� �z");
	// �g�pMP
	DrawString(RED, 960 + 96, text_Top + 20, "%d", tmpSkill->skill->useMP[0]);
	DrawString(BLUE, 960 + 192, text_Top + 20, "%d", tmpSkill->skill->useMP[1]);
	DrawString(YELLOW, 960 + 288, text_Top + 20, "%d", tmpSkill->skill->useMP[2]);
	// �X�L��������	
	for (int i = 0, n = tmpSkill->skill->explanatoryText.size(); i < n; ++i) {
		DrawString(WHITE, 784, text_Top + 72 + i * 32, tmpSkill->skill->explanatoryText[i].c_str());
	}
}
void SkillTree::SetData(PlayerChara *chara) {
	if (chara) {
		SkillTree::chara = chara;			// �L�����̃Z�b�g
		Game::mode = eStageMode::SKILLTREE;	// �X�L���c���[��ʂֈڍs

		// �t�B�[���h�̉����~�߂�
		StopSoundMem(fieldBGM);
		DeleteSoundMem(fieldBGM);
	}
}
// �O��X�L���̃`�F�b�N
bool SkillTree::CheckPremiseSkill(tSkillCell* skill) {
	// -1�͑O��X�L���Ȃ�
	if (skill->premiseSkillID.front() == "-1") {
		return true;
	}

	// �O��X�L����S�đ���
	for (int i = 0, n = (int)skill->premiseSkillID.size(); i < n; ++i) {
		if (!skills[skill->premiseSkillID[i]].getFlag) {
			// ��ł��擾���Ă��Ȃ��X�L��������Ȃ�false
			return false;
		}
	}

	// �S�Ď擾�ς݂Ȃ�true
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

	// �����̐��`��
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

Effect3D						Effect_Cube::m_Effect;		// �􉽃I�u�W�F�N�g�G�t�F�N�g
IDirect3DVertexDeclaration9*	Effect_Cube::m_pVertexDecl3D;// 3�����􉽃I�u�W�F�N�g���_�\���̐錾

Effect_Cube::Effect_Cube(const D3DXMATRIX* pWorldMtx, DWORD color, const D3DXVECTOR3 &Min, const D3DXVECTOR3 &Max) : Object3D() {
	// �G�t�F�N�g�ǂݍ���
	m_Effect.Create(direct3d.pDevice3D, _T("geometory.cfx"));
	
	// 3�����􉽃I�u�W�F�N�g���_�\���̒�`
	decl3d[0] = { 0, 0,	 D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION , 0 };
	decl3d[1] = { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 };
	decl3d[2] = D3DDECL_END();
	
	// 3�����􉽃I�u�W�F�N�g���_�\���̐錾�쐬
	direct3d.pDevice3D->CreateVertexDeclaration(decl3d, &m_pVertexDecl3D);

	atexit(Effect_Cube::Release);

	if (m_Effect.GetEffect() == NULL) {
		return;
	}
	// �f�o�C�X�擾
	pD3DDevice = NULL;
	m_Effect.GetEffect()->GetDevice(&pD3DDevice);

	// �e�N�j�b�N�Z�b�g
	m_Effect.SetTechnique(_T("T0"), PASS_DRAW3D);

	// �G�t�F�N�g�ɕϊ��s��Z�b�g
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
	// �G�t�F�N�g�I��
	//m_Effect.End();
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	Effect_Cube::Release();
}
// ���
void Effect_Cube::Release(){
	// ���_�\���̐錾���
	if (m_pVertexDecl3D) {
		m_pVertexDecl3D->Release();
		m_pVertexDecl3D = NULL;
	}
}
bool Effect_Cube::SetEffectParam(const D3DXMATRIX* pWorldMtx)
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

void Effect_Cube::Draw() {	
	// �G�t�F�N�g�J�n
	m_Effect.Begin();

	if (count > 25) {
		core.z += 20.0;
	}
	else {
		core.z += 1.0;
	}

	// ���_�\���̐錾�Z�b�g
	pD3DDevice->SetVertexDeclaration(m_pVertexDecl3D);
	for (int i = 0; i < 9; ++i) {
		// �`��P
		pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 7, v[i], sizeof(Vtx3D));
		// �`��Q
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

	// �G�t�F�N�g�I��
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
