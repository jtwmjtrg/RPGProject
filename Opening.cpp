#include "Opening.h"

Opening::Opening(){
	// ζ
	backgroundGraph = LoadGraph("img\\opening\\background.png");	// wi

	// o
	count = 0;		// t[ΜJEg
	endCount = 60;	// oΤ
}
Opening::~Opening(){
	// ζν
	DeleteGraph(backgroundGraph);	// wi
} 

void Opening::Process(){
	// endCountάΕo
	if (count < endCount){
		count++;
	}
	else{
		nowScene = GameScene::TITLE;	// Q[V[πTITLEΦ
	}
}

void Opening::Draw(){
	DrawGraph(0, 0, backgroundGraph);	// wi
}