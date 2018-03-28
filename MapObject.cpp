#include "MapObject.h"
#include "Game.h"
#include "Scene.h"

/*-------------------------------------------------ユニットの挙動データ--------------------------------------------*/
void MapUnit_Action::SetActionNum(int* actionNum) {		// 実行中の行動の要素番号のセッター
	this->actionNum = actionNum;
}
// 【転移】
MapUnit_Action_Warp::MapUnit_Action_Warp(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【マップID】取得
	getline(L_stream, L_token, ',');
	this->mapId = boost::lexical_cast<int>(L_token);
	// 3【X座標】取得（マップ座標）
	getline(L_stream, L_token, ',');
	this->x = boost::lexical_cast<int>(L_token);
	// 4【Y座標】取得（マップ座標）
	getline(L_stream, L_token, ',');
	this->y = boost::lexical_cast<int>(L_token);
	// 5【転移演出】取得
	getline(L_stream, L_token, ',');
	this->effect = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Warp::~MapUnit_Action_Warp() {
	// 特になし
}
eMUAReturn MapUnit_Action_Warp::Execution() {
	// プレイヤーの移動が終わったら
	if (!Player::animation)
	{
		// プレイヤーの座標を修正して、違うマップへ飛ばす
		Player::pos_.set_pos(x * CHIPSIZE, y * CHIPSIZE);
		Player::stage = mapId;
	}
	// エフェクト処理
	// あれば↓に書く

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Stop;	// 中断
}

// 【セリフ】
MapUnit_Action_Talk::MapUnit_Action_Talk(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【セリフ内容】取得
	//getline(L_stream, L_token, ',');
	//boost::algorithm::split(str, L_token, boost::is_any_of("@"));
	while (getline(L_stream, L_token, ',') ){
		if (L_token == "") break;
		this->str.push_back(L_token);
	}

}
MapUnit_Action_Talk::~MapUnit_Action_Talk() {
	// 特になし
}
eMUAReturn MapUnit_Action_Talk::Execution() {
	TextBox::AddLine(str);	// 表示する文字列をTextBoxクラスに送る

	// エフェクト処理
	// あれば↓に書く

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Stop;	// 中断
}

// 【戦闘】
MapUnit_Action_Battle::MapUnit_Action_Battle(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【エンカウントID】取得
	getline(L_stream, L_token, ',');
	encID = boost::lexical_cast<int>(L_token);
	// 4【背景ID】取得
	getline(L_stream, L_token, ',');
	backID = boost::lexical_cast<int>(L_token);
	// 5【逃走可能フラグ】取得
	getline(L_stream, L_token, ',');
	escapeFlag = L_token == "1";
	// 6【負けイベフラグ】取得
	getline(L_stream, L_token, ',');
	loseEventFlag = L_token == "1";
	// 7【勝利時の分岐】取得
	getline(L_stream, L_token, ',');
	branch[0] = boost::lexical_cast<int>(L_token);
	// 8【全滅時の分岐】取得
	getline(L_stream, L_token, ',');
	branch[1] = boost::lexical_cast<int>(L_token);
	// 9【逃走時の分岐】取得
	getline(L_stream, L_token, ',');
	branch[2] = boost::lexical_cast<int>(L_token);

	// 初期化
	startFlag = true;		// 開始処理フラグ
	battleFlag = false;		// 戦闘中フラグ
	battleEndType = eEndStatus::Lose;	// 戦闘終了の状態
}
MapUnit_Action_Battle::~MapUnit_Action_Battle() {
	// 特になし
}
eMUAReturn MapUnit_Action_Battle::Execution() {
	//Game::Encounter(encID);	// 戦闘開始

	if (startFlag) {
		// 開始処理
		// 初期化
		battleFlag = true;		// 戦闘中フラグ
		battleEndType = eEndStatus::Lose;	// 戦闘終了の状態

		startFlag = false;		// 戦闘終了待ち状態へ
		Game::Encounter(encID, backID, LoadSoundMem("sound\\bossbattle.wav"), escapeFlag, loseEventFlag, &battleFlag, &battleEndType);		// 戦闘開始
		return eMUAReturn::Stop;	// 中断
	}
	else {
		// 戦闘終了待ち状態
		if (battleFlag) {
			// 戦闘中
			return eMUAReturn::Stop;	// 中断
		}

		// 戦闘終了
		(*actionNum) += branch[(int)battleEndType];

		// 負けイベなら全員HP1で生き返らせる
		if (loseEventFlag) {
			std::vector<std::string> str;
			for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
				Player::character[i]->Resurrection(0);
				Player::character[i]->GetEffectText(str);
			}
		}

		// 初期化
		startFlag = true;		// 開始処理フラグ
		battleFlag = false;		// 戦闘中フラグ
		battleEndType = eEndStatus::Lose;	// 戦闘終了の状態

		return eMUAReturn::Next;	// すぐに次の挙動にうつる
	}
}

// 【ダメージ】
MapUnit_Action_Damage::MapUnit_Action_Damage(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【対象】取得
	getline(L_stream, L_token, ',');
	target = boost::lexical_cast<int>(L_token);
	// 4【ダメージ量】取得
	getline(L_stream, L_token, ',');
	damage = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Damage::~MapUnit_Action_Damage() {
	// 特になし
}
eMUAReturn MapUnit_Action_Damage::Execution() {
	std::vector<string> message;

	switch (target){
	case 0:// 【全員】
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			Player::character[i]->ReceiveSkillIni();
			Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
			Player::character[i]->Damage(damage);	// ダメージを与える
			Player::character[i]->GetEffectText(message);
		}
		break;
	case 1:// 【先頭】
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			if (Player::character[i]->GetAlive() == eAlive::alive) {
				Player::character[i]->ReceiveSkillIni();
				Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
				Player::character[i]->Damage(damage);	// ダメージを与える
				Player::character[i]->GetEffectText(message);
				break;
			}
		}
		break;
	case 2:// 【ランダム】
	{
		int i = 0;
		do {
			i = GetRand(Player::character.size() - 1);
		} while (Player::character[i]->GetAlive() != eAlive::alive);
		Player::character[i]->ReceiveSkillIni();
		Player::character[i]->SetReceiveSkill(eSkillEffect::Attack);
		Player::character[i]->Damage(damage);	// ダメージを与える
		Player::character[i]->GetEffectText(message);
		break;
	}
	default:
		break;
	}
	if (!message.empty()) {
		TextBox::AddLine(message);
	}

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Stop;	// 中断
}

// 【回復】
MapUnit_Action_Recovery::MapUnit_Action_Recovery(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【対象】取得
	getline(L_stream, L_token, ',');
	target = boost::lexical_cast<int>(L_token);
	// 4【HP回復量】取得
	getline(L_stream, L_token, ',');
	heal_HP = boost::lexical_cast<int>(L_token);
	// 5【MP回復量】取得
	getline(L_stream, L_token, ',');
	heal_MP = boost::lexical_cast<int>(L_token);
	// 6【蘇生フラグ】取得
	getline(L_stream, L_token, ',');
	resurrection = L_token == "1";
}
MapUnit_Action_Recovery::~MapUnit_Action_Recovery() {
	// 特になし
}
eMUAReturn MapUnit_Action_Recovery::Execution() {
	std::vector<string> message;

	switch (target) {
	case 0:// 【全員】
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			// スキルフラグ初期化
			Player::character[i]->ReceiveSkillIni();
			// 蘇生
			if (resurrection) {
				Player::character[i]->FullResurrection();
			}
			// HP回復
			if (heal_HP == -1)	Player::character[i]->FullRecovery();
			else				Player::character[i]->Recovery(heal_HP);
			// MP回復
			if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
			else				Player::character[i]->Recovery_MP(heal_MP);
			// メッセージ取得
			Player::character[i]->GetEffectText(message);
		}
		break;
	case 1:// 【先頭】
		for (int i = 0, n = (int)Player::character.size(); i < n; ++i) {
			if (Player::character[i]->GetAlive() == eAlive::alive) {
				Player::character[i]->ReceiveSkillIni();
				if (heal_HP == -1)	Player::character[i]->FullRecovery();
				else				Player::character[i]->Recovery(heal_HP);
				if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
				else				Player::character[i]->Recovery_MP(heal_MP);
				Player::character[i]->GetEffectText(message);
				break;
			}
		}
		break;
	case 2:// 【ランダム】
	{
		int i = 0;
		do {
			i = GetRand(Player::character.size() - 1);
		} while (Player::character[i]->GetAlive() != eAlive::alive);
		Player::character[i]->ReceiveSkillIni();
		if (heal_HP == -1)	Player::character[i]->FullRecovery();
		else				Player::character[i]->Recovery(heal_HP);
		if (heal_MP == -1)	Player::character[i]->FullRecovery_MP();
		else				Player::character[i]->Recovery_MP(heal_MP);
		Player::character[i]->GetEffectText(message);
		break;
	}
	default:
		break;
	}
	if (!message.empty()) {
		TextBox::AddLine(message);
	}

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Stop;	// 中断
}

// 【アイテム取得】
MapUnit_Action_Item::MapUnit_Action_Item(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream stream(line);
	string token;

	// 1【データの種類】破棄
	getline(stream, token, ',');
	// 2 【挙動のタイプ】
	getline(stream, token, ',');
	// 3 【取得アイテムのID】取得
	getline(stream, token, ',');
	itemID = boost::lexical_cast<int>(token);
	// 4 【アイテムを手に入れた場合のジャンプ】取得
	getline(stream, token, ',');
	jump_True = boost::lexical_cast<int>(token);
	// 5【持ち物がいっぱいだった場合のジャンプ】取得
	getline(stream, token, ',');
	jump_False = boost::lexical_cast<int>(token);
}
MapUnit_Action_Item::~MapUnit_Action_Item() {
	// 特になし
}
eMUAReturn MapUnit_Action_Item::Execution() {
	if (Player::InsertItem(itemID)) {		// アイテムを入手
		TextBox::AddLine(Item::GetName(itemID) + " を手に入れた");
		(*actionNum) += jump_True;	// ジャンプ
		return eMUAReturn::Stop;	// 中断
	}
	else {
		TextBox::AddLine("持ち物がいっぱいで " + Item::GetName(itemID) + " を持てなかった");
		(*actionNum) += jump_False;	// ジャンプ
		return eMUAReturn::Stop;	// 中断
	}
}

// 【店】
MapUnit_Action_Shop::MapUnit_Action_Shop(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【店ID】取得
	getline(L_stream, L_token, ',');
	shopID = L_token;
}
MapUnit_Action_Shop::~MapUnit_Action_Shop() {
	// 特になし
}
eMUAReturn MapUnit_Action_Shop::Execution() {
	Shop::InitShop(stoi(shopID));	// 店のＩＤで商品を初期化する
	Player::flag_shop = true;		// 店フラグを立てる

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Stop;	// 中断
}

// 【消滅-出現】
MapUnit_Action_Existence::MapUnit_Action_Existence(string line, bool& appearFlag) :
	appearFlag(appearFlag)
{
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【フラグ】取得
	getline(L_stream, L_token, ',');
	flag = L_token == "1";
	// 4【演出】取得
	getline(L_stream, L_token, ',');
	staging = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Existence::~MapUnit_Action_Existence() {
	// 特になし
}
eMUAReturn MapUnit_Action_Existence::Execution() {
	appearFlag = flag;

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Next;	// すぐに次の挙動にうつる
}

// 【モード変更】
MapUnit_Action_ChangeMode::MapUnit_Action_ChangeMode(string line, int& nowMode, const vector<MapUnit_Mode*>& mode) :
	nowMode(nowMode),
	mode(mode)
{
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【モード数】取得
	getline(L_stream, L_token, ',');
	modeNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_ChangeMode::~MapUnit_Action_ChangeMode() {
	// 特になし
}
eMUAReturn MapUnit_Action_ChangeMode::Execution() {
	// モードを変更
	if (modeNum >= 0 && (unsigned)modeNum < mode.size()) {
		nowMode = modeNum;
	}

	++(*actionNum);		// 次の挙動へ
	return eMUAReturn::Next;	// すぐに次の挙動にうつる
}

// 【メッセージ発信】
MapUnit_Action_Message::MapUnit_Action_Message(string line) : MapUnit_Action() {
	// 【データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【メッセージ番号】取得
	getline(L_stream, L_token, ',');
	messageNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Message::~MapUnit_Action_Message() {
	// 特になし
}
eMUAReturn MapUnit_Action_Message::Execution() {
	MapObject::messageData_Stack.push_back(messageNum);		// メッセージスタックに追加
	return eMUAReturn::Next;	// すぐに次の挙動にうつる
}

// 【選択分岐】
MapUnit_Action_SelectBranch::MapUnit_Action_SelectBranch(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【質問文】取得
	getline(L_stream, L_token, ',');
	question = L_token;
	// 【選択肢】取得
	while (getline(L_stream, L_token, ',')) {
		if (L_token == "") break;
		// 【選択肢文】取得
		choices.push_back({ L_token, 1});
		// 【分岐先】取得
		getline(L_stream, L_token, ',');
		choices.back().destination = boost::lexical_cast<int>(L_token);
	}

	// 初期化
	step = eSelectBranch::question;
	select = -1;
	selectFlag = false;
}
MapUnit_Action_SelectBranch::~MapUnit_Action_SelectBranch() {
	// 特になし
}
eMUAReturn MapUnit_Action_SelectBranch::Execution() {
	switch (step)
	{
	case eSelectBranch::question:// 【質問】
		// 初期化
		select = -1;
		selectFlag = false;
		// 質問文表示
		TextBox::AddSelect(question, choices, selectFlag, select);
		step = eSelectBranch::wait;	// 解答待ち状態へ
		return eMUAReturn::Stop;	// 中断
	case eSelectBranch::wait:// 【回答待ち】
		if (!selectFlag) {
			return eMUAReturn::Stop;	// 中断
		}
		// 【解答受け取り】
		// 分岐処理
		if (select >= 0 && select < (int)choices.size()) {
			// アウトオブレンジ対策
			(*actionNum) += choices[select].destination;
		}
		else {
			++(*actionNum);
		}
		
		// 初期化
		step = eSelectBranch::question;
		select = -1;
		selectFlag = false;

		return eMUAReturn::Next;	// すぐに次の挙動にうつる
	default:
		// 初期化
		step = eSelectBranch::question;
		select = -1;
		selectFlag = false;
		++(*actionNum);		// 次の挙動へ
		return eMUAReturn::End;	// 強制終了
	}
}

// 【強制終了】
MapUnit_Action_End::MapUnit_Action_End(){

}
MapUnit_Action_End::~MapUnit_Action_End() {

}
eMUAReturn MapUnit_Action_End::Execution() {
	return eMUAReturn::End;	// 強制終了
}

// 【挙動番号の移動】
MapUnit_Action_Goto::MapUnit_Action_Goto(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【質問文】取得
	getline(L_stream, L_token, ',');
	moveNum = boost::lexical_cast<int>(L_token);
}
MapUnit_Action_Goto::~MapUnit_Action_Goto() {
	// 特になし
}
eMUAReturn MapUnit_Action_Goto::Execution() {
	(*actionNum) += moveNum;
	return eMUAReturn::Next;	// すぐに次の挙動にうつる
}

// 【強制ゲームオーバー】
MapUnit_Action_GameOver::MapUnit_Action_GameOver() {

}
MapUnit_Action_GameOver::~MapUnit_Action_GameOver() {

}
eMUAReturn MapUnit_Action_GameOver::Execution() {
	Scene::nowScene = GameScene::GAMEOVER;
	return eMUAReturn::End;	// 強制終了
}

// 【ゲームクリア】
MapUnit_Action_GameClear::MapUnit_Action_GameClear() {

}
MapUnit_Action_GameClear::~MapUnit_Action_GameClear() {

}
eMUAReturn MapUnit_Action_GameClear::Execution() {
	Scene::nowScene = GameScene::GAMECLEAR;
	return eMUAReturn::End;	// 強制終了
}

// 【金を払う】
MapUnit_Action_PayMoney::MapUnit_Action_PayMoney(string line) {
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream stream(line);
	string token;

	// 1【データの種類】破棄
	getline(stream, token, ',');
	// 2【挙動のタイプ】
	getline(stream, token, ',');
	// 3【払う金額】取得
	getline(stream, token, ',');
	price = boost::lexical_cast<int>(token);
	// 4【金が足りた場合のジャンプ】取得
	getline(stream, token, ',');
	jump_True = boost::lexical_cast<int>(token);
	// 5【金が足りなかった場合のジャンプ】取得
	getline(stream, token, ',');
	jump_False = boost::lexical_cast<int>(token);
}
MapUnit_Action_PayMoney::~MapUnit_Action_PayMoney() {
	// 特になし
}
eMUAReturn MapUnit_Action_PayMoney::Execution() {
	if (price <= Player::GetMoney()) {
		Player::PayMoney(price);
		(*actionNum) += jump_True;
	}
	else {
		(*actionNum) += jump_False;
	}
	
	return eMUAReturn::Next;	// すぐに次の挙動にうつる
}
/*-------------------------------------------------ユニットの挙動条件データ--------------------------------------------*/
// 挙動の実行
bool MapUnit_If::ActionExecution() {
	/*// 持っている条件の数だけ走査する
	for (int i = 0, n = action.size(); i < n; i++) {
		action[i]->Execution();	// 各条件の評価と実行
	}*/

	bool nextFlag = true;

	while (actionNum < (int)action.size() && nextFlag) {
		switch (action[actionNum]->Execution()){
		case eMUAReturn::Next:// 【次の挙動にうつる】

			break;
		case eMUAReturn::Stop:// 【中断】
			nextFlag = false;
			break;
		case eMUAReturn::End:// 【強制終了】
			return false;

		default:// 【エラー】
			return false;	// 終了
			break;
		}
	}

	/*if (action[actionNum]->Execution()) {	// 行動を一つ実行
		return false;	// 強制終了
	}*/

	return actionNum < (int)action.size();		// 次の行動があるかを返す
}
// 挙動追加
void MapUnit_If::AddAction(MapUnit_Action* action) {
	this->action.push_back(action);	// 挙動を追加する
	this->action.back()->SetActionNum(&actionNum);
}
// 終了処理
void MapUnit_If::EndProcess() {
	// 挙動クラス削除
	for (int i = 0, n = action.size(); i < n; i++) {
		delete action[i];
	}
}

// 【主人公がこのユニットに話しかける】
MapUnit_If_Talk::MapUnit_If_Talk(string line, const int& x, const int& y)
	:x(x),
	y(y) 
{
	ifType = eUnit_If::Talk;	// 条件の種類
}
MapUnit_If_Talk::~MapUnit_If_Talk() {
	// 特になし
}
bool MapUnit_If_Talk::ChackIf() {
	// プレイヤーが話しかける動作を行った場合
	if (Player::talkFlag.flag) {
		// 話しかけられたかどうかを判定する
		if (abs(x - Player::talkFlag.pos_.X) <= (CHIPSIZE - 1) && abs(y - Player::talkFlag.pos_.Y) <= (CHIPSIZE - 1)) {
			actionNum = 0;	// 実行中の行動の要素番号を０に戻す
			return ActionExecution();
		}
	}
	return false;
}

// 【主人公がこのユニットに触れる】
MapUnit_If_Touch::MapUnit_If_Touch(string line, const int& x, const int& y)
	:x(x),
	y(y)
{
	ifType = eUnit_If::Touch;	// 条件の種類
}
MapUnit_If_Touch::~MapUnit_If_Touch() {
	// 特になし
}
bool MapUnit_If_Touch::ChackIf() {
	// 触れたかどうかを判定する
	if (abs(x - Player::pos_.X) <= (CHIPSIZE - 1) && abs(y - Player::pos_.Y) <= (CHIPSIZE - 1)) {
		actionNum = 0;	// 実行中の行動の要素番号を０に戻す
		return ActionExecution();	// 一つ目の行動だけ実行、次の行動があればtrueを変えす
	}
	return false;
}

// 【メッセージ受信】
MapUnit_If_Message::MapUnit_If_Message(string line) {
	ifType = eUnit_If::Message;	// 条件の種類

	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【メッセージ番号】取得
	getline(L_stream, L_token, ',');
	messageNum = boost::lexical_cast<int>(L_token);
}
MapUnit_If_Message::~MapUnit_If_Message() {
	// 特になし
}
bool MapUnit_If_Message::ChackIf() {
	// メッセージを全判定
	for (int i = 0, n = (int)MapObject::messageData.size(); i < n; ++i) {
		if (messageNum == MapObject::messageData[i]) {
			actionNum = 0;	// 実行中の行動の要素番号を０に戻す
			return ActionExecution();	// 一つ目の行動だけ実行、次の行動があればtrueを変えす
		}
	}
	return false;
}

// 【常時】
MapUnit_If_Always::MapUnit_If_Always() {
	ifType = eUnit_If::Always;	// 条件の種類
}
MapUnit_If_Always::~MapUnit_If_Always() {
	// 特になし
}
bool MapUnit_If_Always::ChackIf() {
	actionNum = 0;	// 実行中の行動の要素番号を０に戻す
	return ActionExecution();	// 一つ目の行動だけ実行、次の行動があればtrueを変えす
}

// 【近づくor離れる】
MapUnit_If_Distance::MapUnit_If_Distance(string line, const int& x, const int& y)
	:x(x),
	y(y)
{
	ifType = eUnit_If::Distance;	// 条件の種類

	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【挙動のタイプ】
	getline(L_stream, L_token, ',');
	// 3【距離（半径）】取得
	getline(L_stream, L_token, ',');
	distance = boost::lexical_cast<int>(L_token);
	// 3【近づく/離れる】取得
	getline(L_stream, L_token, ',');
	In_Out = L_token == "1";
}
MapUnit_If_Distance::~MapUnit_If_Distance() {
	// 特になし
}
bool MapUnit_If_Distance::ChackIf() {
	if (abs(x - Player::pos_.X) <= (distance) && abs(y - Player::pos_.Y) <= (distance)) {
		actionNum = 0;	// 実行中の行動の要素番号を０に戻す
		return ActionExecution();	// 一つ目の行動だけ実行、次の行動があればtrueを変えす
	}
	return false;
}
/*-------------------------------------------------ユニットの移動データ--------------------------------------------*/
// 【スーパークラス】
MapUnit_Move::MapUnit_Move(int& x, int& y, Direction& dir) : x(x), y(y), dir(dir){
	preX = x;
	preY = y;
}
// 座標修正
void MapUnit_Move::PosCorrection() {
	x = preX;	// 座標を直前の座標に戻す
	y = preY;
	moveRange -= speed;	// 移動距離を戻す
	// 向きを逆にする
	switch (dir) {
	case UP:	dir = DOWN;
		break;
	case DOWN:	dir = UP;
		break;
	case LEFT:	dir = RIGHT;
		break;
	case RIGHT:	dir = LEFT;
		break;
	default:
		break;
	}
	moveRange = 64 - moveRange;
}

// 【動かない】
MapUnit_Move_None::MapUnit_Move_None(int& x, int& y , Direction& dir) :
	MapUnit_Move(x, y, dir) 
{
	// 特にロードなし
	moveFlag = false;
	moveRange = 0;
	speed = 0;
	delay = 0;
}
MapUnit_Move_None::~MapUnit_Move_None() {
	// 特になし
}
void MapUnit_Move_None::Move() {
	preX = x;
	preY = y;
}

// 【ランダム】
MapUnit_Move_Random::MapUnit_Move_Random(string line, int& x, int& y, Direction& dir) :
	MapUnit_Move(x, y, dir)
{
	// 【配置データ読み込み】
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【移動の種類】
	getline(L_stream, L_token, ',');
	// 3【移動速度】取得
	getline(L_stream, L_token, ',');
	speed = boost::lexical_cast<int>(L_token);
	// 4【移動ごとのdelay】取得
	getline(L_stream, L_token, ',');
	delay = boost::lexical_cast<int>(L_token);

	iniX = x;
	iniY = y;
	count = 0;
	moveFlag = false;
	moveRange = 0;
	moveCount = 0;

	// 移動速度が0以下にならないように
	if (speed <= 0) {
		speed = 1;
	}

}
MapUnit_Move_Random::~MapUnit_Move_Random() {

}
void MapUnit_Move_Random::Move() {
	if (!moveFlag) {
		if (count >= delay) {
			if (moveCount >= 2) {
				// ３歩ごとに初期座標の方向に動く
				moveCount = 0;
				if (abs(x - iniX) > abs(y - iniY)){
					if (x < iniX)	dir = RIGHT;	// 右
					else			dir = LEFT;		// 左
				}
				else {
					if (y < iniY)	dir = DOWN;		// 下
					else			dir = UP;		// 上
				}
			}
			else {
				dir = (Direction)GetRand(3);	// ランダムに移動方向決定
				++moveCount;
			}
			moveFlag = true;// 移動フラグを立てる
			moveRange = 0;	// 移動距離リセット
			count = 0;		// 移動カウントリセット
		}
		else {
			++count;	// 移動カウント+
		}
	}
	if (moveFlag) {
		preX = x;	// 移動前の座標
		preY = y;

		// 移動中
		switch (dir){
		case UP:	y -= speed;
			break;
		case DOWN:	y += speed;
			break;
		case LEFT:	x -= speed;
			break;
		case RIGHT:	x += speed;
			break;
		default:
			break;
		}
		moveRange += speed;	// 移動距離+
		if (moveRange > 64) {
			// 1マス移動したら移動をやめる
			moveFlag = false;	// 移動フラグを折る
			// 移動しすぎた分を戻す
			switch (dir) {
			case UP:	y += moveRange - 64;
				break;
			case DOWN:	y -= moveRange - 64;
				break;
			case LEFT:	x += moveRange - 64;
				break;
			case RIGHT:	x -= moveRange - 64;
				break;
			default:
				break;
			}

		}
	}
	

}

/*-------------------------------------------------ユニットのモード--------------------------------------------*/
// 【スーパークラス】
MapUnit_Mode::MapUnit_Mode() {
	move = nullptr;
	termNum = 0;
}
// 挙動の実行条件を追加
void MapUnit_Mode::AddTerm(MapUnit_If* term) {
	terms.push_back(term);
}
// 挙動追加
void MapUnit_Mode::AddAction(MapUnit_Action* action) {
	terms.back()->AddAction(action);
}
// 挙動の実行条件判定
bool MapUnit_Mode::TermsCheck() {
	// モードの条件全判定
	for (int i = 0, n = terms.size(); i < n; i++) {
		// 実行するものがあれば実行中に移行
		if (terms[i]->ChackIf()) {
			termNum = i;
			return true;
		}
	}
	return false;
}
// 行動実行
bool MapUnit_Mode::ActionExecution() {
	return terms[termNum]->ActionExecution();
}
// 【移動】
void MapUnit_Mode::Move() {
	if(move) move->Move();
}
// 【座標修正】
void MapUnit_Mode::PosCorrection() {
	if (move) move->PosCorrection();
}
// 【移動クラスのセッター】
void MapUnit_Mode::SetMove(MapUnit_Move* data) {
	if (move) {
		// すでに移動クラスがある場合はセットしない
		delete data;
	}
	else {
		move = data;
	}
}
// 【終了処理】
void MapUnit_Mode::EndProcess() {
	// 条件クラス削除
	for (int i = 0, n = terms.size(); i < n; i++) {
		terms[i]->EndProcess();
		delete terms[i];
	}
	// 移動クラス削除
	if(move) delete move;
}

// 【条件なし】
MapUnit_Mode_None::MapUnit_Mode_None(int modeNum, int& nowMode) :
	modeNum(modeNum),
	nowMode(nowMode),
	MapUnit_Mode()
{
	type = eUnitMode::None;
}
MapUnit_Mode_None::~MapUnit_Mode_None() {

}
bool MapUnit_Mode_None::ModeCheck() {
	return modeNum == nowMode;
}

// 【ストーリー進行度】
MapUnit_Mode_Chapter::MapUnit_Mode_Chapter(string line) {
	type = eUnitMode::None;
	//ロード用変数
	std::istringstream L_stream(line);
	string L_token;

	// 1【データの種類】破棄
	getline(L_stream, L_token, ',');
	// 2【モード数】破棄
	getline(L_stream, L_token, ',');
	// 3【移行条件】破棄
	getline(L_stream, L_token, ',');
	// 4【基準の進行度】
	getline(L_stream, L_token, ',');
	chapter = boost::lexical_cast<int>(L_token);
	// 5【条件】
	getline(L_stream, L_token, ',');
	term = boost::lexical_cast<int>(L_token);

}
MapUnit_Mode_Chapter::~MapUnit_Mode_Chapter() {

}
bool MapUnit_Mode_Chapter::ModeCheck() {
	switch (term) {
	case 0:// 【nのとき】
		return Player::chapter == chapter;
	case 1:// 【n以外のとき】
		return Player::chapter != chapter;
	case 2:// 【n以前】
		return Player::chapter <= chapter;
	case 3:// 【n以降】
		return Player::chapter >= chapter;
	default:
		return false;
		break;
	}
}
/*-------------------------------------------------ユニットデータ--------------------------------------------*/
// メッセージ関連
vector<int> MapObject::messageData;			// メッセージのデータ（1フレーム前に呼ばれたもの）
vector<int> MapObject::messageData_Stack;	// メッセージのデータ（現フレームに呼ばれたもの）

// メッセージ更新処理
void MapObject::MessageUpDate() {
	// 送られたメッセージはまずmessageData_Stackに入る
	// その後、次のフレームにmessageDataに移動し、全ユニットが参照する
	// つまり、送られたメッセージは、次のフレームに届く

	if (!messageData.empty()) {
		if (!messageData_Stack.empty()) {
			// 【スタックがある】
			// スタックの内容をメッセージデータにコピー
			messageData = messageData_Stack;
			// スタックを空にする
			messageData_Stack.clear();
			messageData_Stack.shrink_to_fit();
		}
		else {
			// 【スタックがない】
			// メッセージデータを空にする
			messageData.clear();
			messageData.shrink_to_fit();
		}
	}
	else {
		if (!messageData_Stack.empty()) {
			// 【スタックがある】
			// スタックの内容をメッセージデータにコピー
			messageData = messageData_Stack;
			// スタックを空にする
			messageData_Stack.clear();
			messageData_Stack.shrink_to_fit();
		}
	}
}

MapUnit::MapUnit(string line) {
	// 初期化
	collisionFlag	= true;
	appearFlag		= true;
	execFlag		= false;	// 行動実行中フラグ
	nowMode			= 0;
	areaX			= 0;
	areaY			= 0;
	count			= 0;
	dir				= DOWN;

	// 【配置データ読み込み】
	//ロード用変数
	istringstream L_stream(line);
	string L_token;
	
	// 1【ユニットID】
	getline(L_stream, L_token, ',');
	id = boost::lexical_cast<int>(L_token);
	// 2【名前】取得
	getline(L_stream, L_token, ',');
	name = L_token;
	// 3【X座標】取得（マップ座標）
	getline(L_stream, L_token, ',');
	initX = x = boost::lexical_cast<int>(L_token) * CHIPSIZE;
	// 4【Y座標】取得（マップ座標）
	getline(L_stream, L_token, ',');
	initY = y = boost::lexical_cast<int>(L_token) * CHIPSIZE;
	// 5【画像】取得
	getline(L_stream, L_token, ',');
	if (L_token == "0") {
		L_token = "null";
	}
	string str = "img\\unit\\" + L_token + ".png";	// アドレス生成
	char* cstr = new char[str.size() + 1];			// メモリ確保
	strcpy_s(cstr, str.size() + 1, str.c_str());	// コピー
	img = LoadGraph(cstr, 4, 4);
	delete cstr;	// メモリ解放	
	// 6【当たり判定】取得
	getline(L_stream, L_token, ',');
	collisionFlag = L_token == "1";
	// 7【出現フラグ】取得
	getline(L_stream, L_token, ',');
	appearFlag = L_token == "1";
	// 8【現在のモード数】取得
	getline(L_stream, L_token, ',');
	nowMode = boost::lexical_cast<int>(L_token);

	LoadData();		// データのロード
}

void MapUnit::LoadData() {
	// 【ファイル読み込み用変数】
	ifstream L_File;	// 読み込むファイル
	string L_Line;		// 読み込んだ行（1行）
	int L_Count = 0;	// ノード読み込み用（何行目を読み込んでいるか）
	string L_token;		// 1行を,で分割する際に使う

	int L_Mode;		// 読み込み時のモード指定

	// 読み込むファイルの指定
	L_File.open("unitdata\\" + to_string(Player::stage) + "\\data\\" + to_string(id) + ".csv", ios::in);	// ファイルオープン

	if (L_File.fail()) {	// ファイル読み込み
		name = "読み込めてないよ";
	}
	else {

		while (getline(L_File, L_Line)) {	// 一行ずつ読み込み
			// 読み込んだ行をコピー
			std::istringstream L_stream(L_Line);
			// 1列目をロードして判定
			getline(L_stream, L_token, ',');

			switch (boost::lexical_cast<int>(L_token)){
			case 0:	// 【モード】
			{
				// 2【モード数】
				getline(L_stream, L_token, ',');
				L_Mode = boost::lexical_cast<int>(L_token);
				// モード数が少なければ
				if ((unsigned)L_Mode >= mode.size()) {
					// モード数を増やす
					mode.resize(L_Mode + 1);
				}
				// 3【条件ID】判定
				getline(L_stream, L_token, ',');
				switch (static_cast<eUnitMode>(boost::lexical_cast<int>(L_token))){
				case eUnitMode::None:// 【条件なし】
					mode[L_Mode] = new MapUnit_Mode_None(L_Mode, nowMode);
					break;
				default:
					break;
				}
				break;
			}
			case 1:	// 【移動】
				// 2【移動タイプ】判定
				getline(L_stream, L_token, ',');
				switch (boost::lexical_cast<int>(L_token)){
				case 0:	// 【動かない】
					mode[L_Mode]->SetMove(new MapUnit_Move_None(x, y, dir));
					break;
				case 1:	// 【主人公を追うor逃げる】

					break;
				case 2:	// 【ループ移動】

					break;
				case 3:	// 【ランダム移動】
					mode[L_Mode]->SetMove(new MapUnit_Move_Random(L_Line, x, y, dir));
					break;
				}
				break;
			case 2:	// 【挙動の実行条件】
				// 2【条件タイプ】判定
				getline(L_stream, L_token, ',');
				switch ((eUnit_If)boost::lexical_cast<int>(L_token)){
				case eUnit_If::Talk:	// 【話しかける】
					mode[L_Mode]->AddTerm(new MapUnit_If_Talk(L_Line, x, y));
					break;
				case eUnit_If::Touch:	// 【触れる】
					mode[L_Mode]->AddTerm(new MapUnit_If_Touch(L_Line, x, y));
					break;
				case eUnit_If::Message:	// 【メッセージを受信する】
					mode[L_Mode]->AddTerm(new MapUnit_If_Message(L_Line));
					break;
				case eUnit_If::Always:	// 【常時】
					mode[L_Mode]->AddTerm(new MapUnit_If_Always());
					break;
				case eUnit_If::Distance:// 【主人公との距離判定】
					mode[L_Mode]->AddTerm(new MapUnit_If_Distance(L_Line, x, y));
					break;
				}
				break;
			case 3:	// 【挙動】
				// 2【挙動タイプ】判定
				getline(L_stream, L_token, ',');
				switch (static_cast<eUnit_Act>(boost::lexical_cast<int>(L_token))) {
				case eUnit_Act::Warp:// 【転移】
					mode[L_Mode]->AddAction(new MapUnit_Action_Warp(L_Line));
					break;
				case eUnit_Act::Talk:// 【セリフ】
					mode[L_Mode]->AddAction(new MapUnit_Action_Talk(L_Line));
					break;
				case eUnit_Act::Battle:// 【戦闘】
					mode[L_Mode]->AddAction(new MapUnit_Action_Battle(L_Line));
					break;
				case eUnit_Act::Damage:// 【ダメージ】
					mode[L_Mode]->AddAction(new MapUnit_Action_Damage(L_Line));
					break;
				case eUnit_Act::Recovery:// 【回復】
					mode[L_Mode]->AddAction(new MapUnit_Action_Recovery(L_Line));
					break;
				case eUnit_Act::Item:// 【アイテム取得】
					mode[L_Mode]->AddAction(new MapUnit_Action_Item(L_Line));
					break;
				case eUnit_Act::Shop:// 【店】
					mode[L_Mode]->AddAction(new MapUnit_Action_Shop(L_Line));
					break;
				case eUnit_Act::Existence:	// 【消滅-出現】
					mode[L_Mode]->AddAction(new MapUnit_Action_Existence(L_Line, appearFlag));
					break;
				case eUnit_Act::ChangeMode:// 【モード変更】
					mode[L_Mode]->AddAction(new MapUnit_Action_ChangeMode(L_Line, nowMode, mode));
					break;
				case eUnit_Act::Message:// 【メッセージ】
					mode[L_Mode]->AddAction(new MapUnit_Action_Message(L_Line));
					break;
				case eUnit_Act::SelectBranch:// 【選択分岐】
					mode[L_Mode]->AddAction(new MapUnit_Action_SelectBranch(L_Line));
					break;
				case eUnit_Act::End:// 【強制終了】
					mode[L_Mode]->AddAction(new MapUnit_Action_End());
					break;
				case eUnit_Act::Goto:// 【挙動番号の移動】
					mode[L_Mode]->AddAction(new MapUnit_Action_Goto(L_Line));
					break;
				case eUnit_Act::GameOver:// 【強制ゲームオーバー】
					mode[L_Mode]->AddAction(new MapUnit_Action_GameOver());
					break;
				case eUnit_Act::GameClear:// 【ゲームクリア】
					mode[L_Mode]->AddAction(new MapUnit_Action_GameClear());
					break;
				case eUnit_Act::PayMoney:// 【金を払う】
					mode[L_Mode]->AddAction(new MapUnit_Action_PayMoney(L_Line));
					break;
				default:
					break;
				}
				break;
			}
		}
	}

	// ファイルを閉じる
	L_File.close();

}
MapUnit::~MapUnit() {
	// 特になし
}

// 毎フレームの処理
bool MapUnit::Process() {
	// 出現時のみ
	if (appearFlag) {
		
		// モードの条件全判定
		for (int i = 0, n = mode.size(); i < n; ++i) {
			if (mode[i]->ModeCheck()) {
				nowMode = i;
				break;
			}
		}
		// 移動
		mode[nowMode]->Move();
		// 行動条件判定
		if (mode[nowMode]->TermsCheck()) {
			execFlag = true;
			return true;
		}
		
	}
	return false;
}

// 描画
void MapUnit::Draw(int cameraX, int cameraY) {
	if (!appearFlag) return;
	int drawX = x - cameraX;
	int drawY = y - cameraY;
	if (drawX >= -64 && drawX <= 1920 && drawY >= -64 && drawY <= 1920) {
		DrawGraph(drawX, drawY, img, count / 10, dir);
	}

	++count;	// 描画カウント
	if (count > 40) {
		count = 0;
	}
}

// 行動実行
bool MapUnit::ActionExecution() {
	return mode[nowMode]->ActionExecution();
}

// ユニットの状態の変更
void MapObject::ChangeStatus(eUnitStatus type) {
	switch (type){
	case eUnitStatus::Apear:	// 【出現】
		appearFlag = true;
		break;
	case eUnitStatus::Disapear:	// 【消滅】
		appearFlag = false;
		break;
	case eUnitStatus::SetColl:	// 【当たり判定を付ける】
		collisionFlag = true;
		break;
	case eUnitStatus::DeletColl:	// 【当たり判定を消す】
		collisionFlag = false;
		break;
	default:
		break;
	}
}
// 移動方向
void MapObject::SetDir(Direction dir) {
	this->dir = dir;
}
bool MapObject::CheckColl(int x, int y) {
	// 当たり判定あり & 出現中のみ
	if (appearFlag) {
		if (collisionFlag) {
			if (abs(this->x - x) <= (CHIPSIZE - 1) && abs(this->y - y) <= (CHIPSIZE - 1)) {
				return true;
			}
		}
	}

	return false;
}

void MapUnit::EndProcess() {
	// 条件クラス削除
	for (int i = 0, n = mode.size(); i < n; i++) {
		mode[i]->EndProcess();
		delete mode[i];
	}
}

// 座標修正
void MapUnit::PosCorrection() {
	mode[nowMode]->PosCorrection();
}

// モードのセッター
void MapUnit::SetMode(int modeNum) {
	
	// アウトオブレンジ対策
	if (modeNum < 0 || mode.size() <= (unsigned)modeNum) {
		nowMode = 0;
	}
	else {
		nowMode = modeNum;
	}
}
// モードのゲッター
int MapUnit::GetMode() {
	return nowMode;
}
// 区分け座標のセッター
void MapUnit::SetArea(int areaX, int areaY) {
	this->areaX = areaX;
	this->areaY = areaY;
}