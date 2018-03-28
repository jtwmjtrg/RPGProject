#include "Scene.h"

GameScene	Scene::nowScene = GameScene::OPENING;
bool		Scene::end		= false;

Scene::Scene() {}

GameScene Scene::GetNowScene(){
	return nowScene;
}
bool Scene::GetEnd(){
	return end;
}