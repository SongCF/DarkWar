#include "BaseSprite.h"
#include "ImageTypeController.h"

BaseSprite::BaseSprite(void)
{
	mResMgr = ResourcePathManager::getManager();
	init();
}

BaseSprite* BaseSprite::create(string fileName)
{
	BaseSprite* s = new BaseSprite();
	s->initWithFile(fileName.c_str());
	s->autorelease();
	return s;
}
BaseSprite* BaseSprite::create()
{
	BaseSprite* s = new BaseSprite();
	s->init();
	s->autorelease();
	return s;
}
bool BaseSprite::initWithFile(const char *pszFilename)
{
	mFileName = ImageTypeController::getControler()->getImageFile(pszFilename);
	return CCSprite::initWithFile(mFileName.c_str());
}
bool BaseSprite::initWithFile(string fileName)
{
	return initWithFile(fileName.c_str());
}
bool BaseSprite::initWithFileDelay(string fileName,float delay)
{
	if (delay < 0)
	{
		return initWithFile(fileName);
	}else
	{
		mDelayFileName = fileName;
		runAction(CCSequence::create(
			CCDelayTime::create(delay),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::timeToDelayInit)),
			NULL
			));
	}
	return true;
}

void BaseSprite::timeToDelayInit(CCNode* mySelf)
{
	initWithFile(mDelayFileName);
	mDelayFileName = "";
	setScale(1.0f);
}

void BaseSprite::removeNode(CCNode* node)
{
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}
BaseSprite::~BaseSprite(void)
{
}
