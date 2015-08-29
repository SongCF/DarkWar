#include "GameScene.h"
//#include "DeviceAdapter.h"

/**
 *	其实应该就不需要了
 */
GameScene::GameScene(void)
{
	init();

	//DeviceAdapter* adapter = DeviceAdapter::getAdapter();
	////适配各种分辨率
	//setScale(adapter->getSceneScale());
	//CCPoint offset = adapter->getSceneOffset();
	//setPosition(offset);
	//CCPoint worldPoint = this->convertToWorldSpace(CCPointZero);
	//adapter->setWorldOffset(worldPoint);
}
GameScene* GameScene::createScene()
{
	GameScene* scene = new GameScene();
	scene->autorelease();
	return scene;
}

GameScene::~GameScene(void)
{
}
