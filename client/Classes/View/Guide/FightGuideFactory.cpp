#include "FightGuideFactory.h"
#include "BaseLayer.h"
#include "LightMaskFactory.h"
#include "LFUtile.h"
#include "History.h"
#include "FightGuideLayers.h"


FightGuideFactory* FightGuideFactory::factory;

FightGuideFactory::FightGuideFactory(void)
{
}

FightGuideFactory* FightGuideFactory::getFactory()
{
	if (factory == NULL)
	{
		factory = new FightGuideFactory();
	}
	return factory;
}

string FightGuideFactory::getFightGuideHistoryKey(GuideId id)
{
	char temp[128];
	sprintf(temp,"%s%d_showed",HISTORY_KEY_PREFIX,id);
	return temp;
}

// 是否某个guidelayer显示过了
bool FightGuideFactory::isFightGuideShowed(GuideId id)
{
	string key = getFightGuideHistoryKey(id);
	bool showed = false;
	History::getHistory()->getBoolFromCurrentHero(key,showed);
	return showed;
}
void FightGuideFactory::setFightGuideShowed(GuideId id,bool showed /*= true*/)
{
	string key = getFightGuideHistoryKey(id);
	//History::getHistory()->putInCurrentUser(key,showed);
	History::getHistory()->putInCurrentHero(key,showed);
}

CCLayer* FightGuideFactory::showBloodGuideLayer(CCNode* toAdd,CCSize bloodSize,CCPoint worldGlPos)
{
	CCLayer* ret = BloodGuideLayer::create(bloodSize,worldGlPos);
	toAdd->addChild(ret,2049);
	return ret;
}

CCLayer* FightGuideFactory::showFirstGetEquipGuideLayer(CCNode* toAdd,CCSize equipSize,CCPoint worldGlPos)
{
	CCLayer* ret = FirstGetEquipGuideLayer::create(equipSize,worldGlPos);
	toAdd->addChild(ret,2049);
	return ret;
}

CCLayer* FightGuideFactory::showFirstGetSkillBookGuideLayer(CCNode* toAdd,int skillBookType,CCSize bookSize,CCPoint worldGlPos)
{
	CCLayer* ret = FirstGetSkillBookGuideLayer::create(bookSize,worldGlPos,skillBookType);
	toAdd->addChild(ret,2049);
	return ret;
}

CCLayer* FightGuideFactory::showInitGuideLayer(InitGuideConfig config)
{
	CCAssert(config.toAdd != NULL,__FUNCTION__);
	CCLayer* ret = JuQingInFightGuideLayer::create(config);
	config.toAdd->addChild(ret,2049);
	return ret;
}


FightGuideFactory::~FightGuideFactory(void)
{
}
