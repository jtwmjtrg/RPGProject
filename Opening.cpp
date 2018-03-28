#include "Opening.h"

Opening::Opening(){
	// �摜
	backgroundGraph = LoadGraph("img\\opening\\background.png");	// �w�i

	// ���o
	count = 0;		// �t���[�����̃J�E���g
	endCount = 60;	// ���o����
}
Opening::~Opening(){
	// �摜�폜
	DeleteGraph(backgroundGraph);	// �w�i
} 

void Opening::Process(){
	// endCount�܂ŉ��o
	if (count < endCount){
		count++;
	}
	else{
		nowScene = GameScene::TITLE;	// �Q�[���V�[����TITLE��
	}
}

void Opening::Draw(){
	DrawGraph(0, 0, backgroundGraph);	// �w�i
}