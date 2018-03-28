#include "Manager.h"

Manager* Manager::Instance(){
	// ��x����Manager�𐶐�����
	if (_instance == NULL){
		_instance = new Manager();	// �B���Manager
	}

	return _instance;	// ��ɗB���Manager��Ԃ�
}
void Manager::Destroy(){
	delete _instance;	// ���������
}

Manager::Manager(){
	// �A�v���P�[�V�����J�n����
	scene = new Opening();		// �I�[�v�j���O����X�^�[�g 
	TextBox::InitTextBox();		// �e�L�X�g�{�b�N�X�̏�����
	Item::InitItem();			// �A�C�e���̃f�[�^��ǂݍ���
	EquipmentItem::InitEquipmentItem();	// �����̃f�[�^��ǂݍ���
	Emote::Create();			// �G���[�g�̓ǂݍ���
}
Manager::~Manager(){
	// �A�v���P�[�V�����I������
	TextBox::Destroy();			// �e�L�X�g�{�b�N�X�̊��p
	Emote::Destroy();			// �G���[�g�̊��p
	if(scene) delete scene;		// ���������
}

void Manager::UpDate(){
	// ���O�̃Q�[���V�[����ۑ�
	preScene = Scene::GetNowScene();

	// �e�L�X�g�{�b�N�X�X�V
	TextBox::Process();

	// �e�V�[���̏���
	scene->Process();
	scene->Draw();

	// �Q�[���V�[���̕ύX���������ꍇ
	if (preScene != Scene::GetNowScene()){
		ChangeScene();	// �Q�[���V�[���ύX����
	}
}

void Manager::ChangeScene(){
	// ���������
	delete scene;

	// �ύX��̃R���X�g���N�^�Ń������m��
	switch (Scene::GetNowScene()){
	case GameScene::OPENING:
		scene = new Opening();
		break;

	case GameScene::TITLE:
		scene = new Title();
		break;

	case GameScene::STAGE:
		scene = new Stage();
		break;

	case GameScene::GAMEOVER:
		scene = new GameOver();
		break;

	case GameScene::GAMECLEAR:
		scene = new GameClear();
		break;

	case GameScene::END:
		Scene::end = true;		// �Q�[���I��
		scene = nullptr;
		break;

	default:
		//�Q�[���V�[���s��
		break;
	}
}