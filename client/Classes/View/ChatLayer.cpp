
#include "ChatLayer.h"
#include "font.h"
#include "Music.h"
#include "MyselfManager.h"


enum MyEnum
{
	TAG_Board =1,
	TAG_ActorA ,
	TAG_ActorB ,
	Tag_Label_Name,
	TAG_label ,
	Tag_ImgNext,
};



#define TOUCH_ENABLE true
#define delta_time 0.75f

ChatLayer* ChatLayer::create(vector<ChatItem> chatContext, ChatLayerCallBack* callBackInterface)
{
	if(chatContext.size() > 0)
	{
		ChatLayer* layer = new ChatLayer(chatContext, callBackInterface);
		if(layer && layer->init())
		{
			layer->autorelease();
		}
		else 
		{
			CC_SAFE_DELETE(layer);
			layer = NULL;
		}

		return layer;
	}
	else 
	{
		callBackInterface->chatEndCallBack();
		return NULL;
	}
}

ChatLayer::ChatLayer(vector<ChatItem>& chatContext, ChatLayerCallBack* callBackInterface)
{
	m_chatContext = chatContext;
	m_callbackClass = callBackInterface;

	mLayerEnd = false;
	mCurMusicId = 0;

	m_index = 0;
	mTimeEnd = true;
	mLabelFadeInEnd = true;
}

ChatLayer::~ChatLayer()
{

}


bool ChatLayer::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(! CCLayerColor::initWithColor(ccc4(0,0,0,160)) );

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//board
		CCSprite* board = CCSprite::create("juqing_duihuakuang.png");
		addChild(board, 2, TAG_Board);
		board->setAnchorPoint(ccp(0.5, 0));
		board->setPosition(ccp(winSize.width/2, 0));
		board->setOpacity(0);
		//next img
		BaseSprite *nextImg = BaseSprite::create("sphc_anniu_fanye.png");
		board->addChild(nextImg, 0, Tag_ImgNext);
		nextImg->setPosition(ccp(597,37));
		nextImg->setOpacity(0);
		nextImg->runAction(CCSequence::create(
			CCFadeIn::create(0.15f),
			CCRepeat::create(CCSequence::create(CCFadeOut::create(0.75f), CCFadeIn::create(0.75f), NULL), -1),
			NULL));

		//npc
		string npcFileName = getFileByName(m_chatContext.at(0).name).second;
		////////////////////////////////////////////
		if(npcFileName.empty())
		{
			CCLog("Error: %s [can't found npc imgine]", __FUNCTION__);
			npcFileName = "juqing_duihua_NPC_shenmiren.png";
		}
		////////////////////////////////////////////
		CCSprite* actorA = CCSprite::create(npcFileName.c_str());
		addChild(actorA, 1, TAG_ActorA);
		actorA->setAnchorPoint(ccp(0,0));
		actorA->setPosition(ccp(0, board->getContentSize().height-14));
		actorA->setOpacity(0);
		//actor  //yemanren   cike   fashi
		string actorFileName = getFileByName("zhujue").second;
		CCSprite* actorB = CCSprite::create(actorFileName.c_str());
		addChild(actorB, 1, TAG_ActorB);
		actorB->setAnchorPoint(ccp(1,0));
		actorB->setPosition(ccp(winSize.width, board->getContentSize().height-14));
		//		actorB->setScale(0.9);
		actorB->setOpacity(0);

		//label name
		CCLabelTTF *namelabel = CCLabelTTF::create("", fontStr_kaiti, 24);
		addChild(namelabel, 5, Tag_Label_Name);
		namelabel->setAnchorPoint(ccp(0, 0.5));
		namelabel->setPosition(ccp(20, board->getContentSize().height-42));
		namelabel->setColor(fonColor_PuTong);
		namelabel->setOpacity(0);
		//label content
		CCLabelTTF* label = CCLabelTTF::create("", fontStr_kaiti, 24, CCSize(610, 0), kCCTextAlignmentLeft);
		addChild(label, 5, TAG_label);
		label->setAnchorPoint(ccp(0,1));
		label->setColor(fonColor_FaGuang);
		label->setPosition(ccp(20, board->getContentSize().height-76));
		label->setOpacity(0);




		////////////start chat
		if(m_chatContext.at(0).name == "zhujue")
			actorB->runAction(CCFadeIn::create(0.1f));
		else
			actorA->runAction(CCFadeIn::create(0.1f));
		board->runAction(CCFadeIn::create(0.1f));
		label->runAction(CCFadeIn::create(0.1f));
		namelabel->runAction(CCFadeIn::create(0.1f));
		nextImg->runAction(CCFadeIn::create(0.15f));
		scheduleOnce(schedule_selector(ChatLayer::showChatContext), 0.05f);
		{
			mTimeEnd = false;
			mLabelFadeInEnd = false;
			++m_index;
		}

		bRet = true;
	}while(0);

	return bRet;
}

void ChatLayer::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1024, true);
}

void ChatLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool ChatLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (   (mLabelFadeInEnd && mTimeEnd)
		|| TOUCH_ENABLE)
	{
		if (TOUCH_ENABLE)
		{
			//
			unschedule(schedule_selector(ChatLayer::showChatContext));
			unschedule(schedule_selector(ChatLayer::saidChatContextEnd));
			if (mCurMusicId != 0)
				Music::sharedMusicPlayer()->stopEffectSound(mCurMusicId);
			mCurMusicId = 0;
		}
	}
	else 
	{
		return true;
	}


	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCSprite *board = (CCSprite*)getChildByTag(TAG_Board);
	CCSprite *nextImg = (CCSprite*)board->getChildByTag(Tag_ImgNext);
	CCSprite *actorA = (CCSprite*)getChildByTag(TAG_ActorA);
	CCSprite *actorB = (CCSprite*)getChildByTag(TAG_ActorB);
	CCLabelTTF *label = (CCLabelTTF*)getChildByTag(TAG_label);
	CCLabelTTF *nameLabel = (CCLabelTTF*)getChildByTag(Tag_Label_Name);


	if(m_index >= m_chatContext.size())
	{
		if ( mLayerEnd) //避免渐渐消失的时候动作重复闪速
			return true;

		mLayerEnd = true;

		this->runAction(CCSequence::create(
			CCFadeTo::create(0.5f, 0)
			, CCCallFunc::create(this, SEL_CallFunc(&ChatLayer::callbackRemove))
			, NULL));

		board->runAction(CCFadeTo::create(0.25f, 0));
		actorA->runAction(CCFadeTo::create(0.25f, 0));
		actorB->runAction(CCFadeTo::create(0.25f, 0));
		label->runAction(CCFadeTo::create(0.25f, 0));
		nameLabel->runAction(CCFadeTo::create(0.25f, 0));

		nextImg->stopAllActions();
		nextImg->runAction(CCFadeTo::create(0.25f, 0));
	}
	else
	{
		mTimeEnd = false;
		mLabelFadeInEnd = false;

		//label
		label->setString("");

		//下一句对话的 Speaker是谁
		string heroName = m_chatContext.at(m_index).name;
		if(heroName == "zhujue")
		{
			actorB->runAction(CCFadeTo::create(0.1f, 255));
			actorA->runAction(CCFadeTo::create(0.1f, 0));
		} 
		else
		{
			actorA->initWithFile(getFileByName(heroName).second.c_str());
			actorA->setAnchorPoint(ccp(0,0));
			actorA->setOpacity(0);
			actorA->runAction(CCFadeTo::create(0.1f, 255));
			actorB->runAction(CCFadeTo::create(0.1f, 0));
		}

		scheduleOnce(schedule_selector(ChatLayer::showChatContext), 0.05f);
		++m_index;
	}

	return true;
}

void ChatLayer::showChatContext(float delta)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	int idx = m_index-1;

	string speakStr = getFileByName(m_chatContext.at(idx).name).first + ":";
	string chatStr(m_chatContext.at(idx).chatStr);

	CCLabelTTF *nameLabel = ((CCLabelTTF*)getChildByTag(Tag_Label_Name));
	CCLabelTTF * label = ((CCLabelTTF*)getChildByTag(TAG_label));
	nameLabel->setString(speakStr.c_str());
	label->setString(chatStr.c_str());
	nameLabel->setOpacity(0);
	label->setOpacity(0);
	nameLabel->runAction(CCSequence::create(
		CCFadeTo::create(0.25f, 255)
		, NULL));
	label->runAction(CCSequence::create(
		CCFadeTo::create(0.25f, 255)
		, CCCallFunc::create(this, callfunc_selector(ChatLayer::labelFadeInEnd))
		, NULL));

	if (m_chatContext.at(idx).deltaTime > 0.1f)
	{
		mCurMusicId = Music::sharedMusicPlayer()->playEffectSound(m_chatContext.at(idx).musicFile);
		scheduleOnce(schedule_selector(ChatLayer::saidChatContextEnd), m_chatContext.at(idx).deltaTime + delta_time);//有的时间太短。、、
	}
}

void ChatLayer::labelFadeInEnd()
{
	mLabelFadeInEnd = true;

	if (mLabelFadeInEnd && mTimeEnd)
		ccTouchBegan(0, 0);
}

void ChatLayer::saidChatContextEnd(float dt)
{
	mTimeEnd = true;
	Music::sharedMusicPlayer()->unLoadEffectSound(m_chatContext.at(m_index-1).musicFile);

	if (mLabelFadeInEnd && mTimeEnd)
		ccTouchBegan(0, 0);
}

void ChatLayer::callbackRemove()
{
	this->removeFromParent();
	m_callbackClass->chatEndCallBack();
}

pair<string, string> ChatLayer::getFileByName(string name)
{
	string actorName, fileName;
	if(name == "zhujue")
	{
		actorName = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().customName;

		Profession proId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
		if(proId == Profession_Yemanren)
			fileName = "yemanren";
		else if(proId == Profession_Chike)
			fileName = "cike";
		else if(proId == Profession_Fashi)
			fileName = "fashi";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "mishushi")
	{
		actorName = LFStrings::getValue("mishu");
		fileName = "mishushi";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "shenjianshou")
	{
		actorName = LFStrings::getValue("shenjianshou");
		fileName = "shenjianshou";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "qishi")
	{
		actorName = LFStrings::getValue("qiShi");
		fileName = "qishi";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "wuseng")
	{
		actorName = LFStrings::getValue("wuseng");
		fileName = "wuseng";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "youxia")
	{
		actorName = LFStrings::getValue("youxia");
		fileName = "youxia";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "lieren")
	{
		actorName = LFStrings::getValue("lieshou");
		fileName = "lieren";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else if (name == "wushi")
	{
		actorName = LFStrings::getValue("wushi");
		fileName = "wushi";
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);
	}
	else
	{
		actorName = name;
		fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(name);
	}

	return make_pair(actorName, fileName);
}


// 
// bool ChatLayer::init()
// {
// 	bool bRet = false;
// 	do 
// 	{
// 		CC_BREAK_IF(! CCLayerColor::initWithColor(ccc4(0,0,0,60)) );
// 
// 		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
// 
// 		//board
// 		CCSprite* board = CCSprite::create("juqing_duihuakuang.png");
// 		addChild(board, 2, TAG_BoardA);
// 		board->setAnchorPoint(ccp(0.5, 0));
// 		board->setPosition(ccp(winSize.width/2, 0));
// 		board->setOpacity(0);
// 
// 		//npc
// 		string npcFileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(
// 			m_chatContext.at(0).first);
// 		////////////////////////////////////////////
// 		if(npcFileName.empty())
// 		{
// 			npcFileName = "juqing_duihua_NPC_shenmiren.png";
// 		}
// 		////////////////////////////////////////////
// 		CCSprite* actorA = CCSprite::create(npcFileName.c_str());
// 		addChild(actorA, 1, TAG_ActorA);
// 		actorA->setAnchorPoint(ccp(0,0));
// 		actorA->setPosition(ccp(0, board->getContentSize().height));
// 		actorA->setOpacity(0);
// 		//actor  //yemanren   cike   fashi
// 		string actorFileName;
// 		Profession proId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
// 		if(proId == Profession_Yemanren)
// 			actorFileName = "yemanren";
// 		else if(proId == Profession_Chike)
// 			actorFileName = "cike";
// 		else if(proId == Profession_Fashi)
// 			actorFileName = "fashi";
// 		actorFileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(actorFileName);
// 		CCSprite* actorB = CCSprite::create(actorFileName.c_str());
// 		addChild(actorB, 1, TAG_ActorB);
// 		actorB->setAnchorPoint(ccp(1,0));
// 		actorB->setPosition(ccp(winSize.width, board->getContentSize().height));
// 		//		actorB->setScale(0.9);
// 		actorB->setOpacity(0);
// 
// 		//label
// 		CCLabelTTF* label = CCLabelTTF::create("", fontStr_kaiti, 22, CCSize(620, 0), kCCTextAlignmentLeft);
// 		addChild(label, 5, TAG_label);
// 		label->setAnchorPoint(ccp(0,1));
// //		label->setColor(ccc3(0xee, 0xc9, 0x00));
// 		label->setPosition(ccp(10, board->getContentSize().height-40));
// 		label->setOpacity(0);
// 
// 		//menu
// 		CCSprite* item = CCSprite::create("juqing_duihua_xiayiye_dizuo.png");
// 		addChild(item, 5, TAG_Next+1);
// 		item->setPosition(ccp(winSize.width-40, 20));
// 
// 		CCSprite* next = CCSprite::create("juqing_duihua_xiayiye.png");
// 		addChild(next, 5, TAG_Next);
// 		next->setPosition(ccp(winSize.width-40, 40));
// 		next->runAction(CCRepeatForever::create(CCSequence::create(
// 			CCMoveBy::create(0.5, ccp(0, 20))
// 			, CCMoveBy::create(0.5, ccp(0, -20))
// 			, NULL)));
// 
// 
// 		////////////start chat
// 		if(m_chatContext.at(0).first == "zhujue")
// 			actorB->runAction(CCFadeIn::create(0.1));
// 		else
// 			actorA->runAction(CCFadeIn::create(0.1));
// 		board->runAction(CCFadeIn::create(0.1));
// 		label->runAction(CCFadeIn::create(0.1));
// 		schedule(schedule_selector(ChatLayer::updateContext));
// 		bUpdateContext = true;
// 		++m_index;
// 
// 		bRet = true;
// 	}while(0);
// 
// 	return bRet;
// }
// 
// void ChatLayer::onEnter()
// {
// 	CCLayerColor::onEnter();
// 	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1024, true);
// }
// 
// void ChatLayer::onExit()
// {
// 	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
// 	CCLayerColor::onExit();
// }
// 
// bool ChatLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
// {
// 	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
// 
// 
// 	if(bUpdateContext)
// 	{
//  		unschedule(schedule_selector(ChatLayer::updateContext));
// 		string tempStr = m_chatContext.at(m_index-1).first;
// 		if(tempStr == "zhujue")
// 			tempStr = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().customName;
// 		tempStr += ":\n   " + m_chatContext.at(m_index-1).second;
// 		((CCLabelTTF*)getChildByTag(TAG_label))->setString(tempStr.c_str());
// 		mChatNum = 0;
//  		bUpdateContext = false;
// 		return true;
// 	}
// 
// 	if(m_index >= m_chatContext.size())
// 	{
// 		this->runAction(CCSequence::create(
// 			CCFadeOut::create(0.5)
// 			, CCCallFunc::create(this, SEL_CallFunc(&ChatLayer::callbackRemove))
// 			, NULL));
// 
// 		getChildByTag(TAG_Next)->runAction(CCFadeTo::create(0.25, 0));
// 		getChildByTag(TAG_Next+1)->runAction(CCFadeTo::create(0.25, 0));
// 		getChildByTag(TAG_BoardA)->runAction(CCFadeTo::create(0.25, 0));
// 		getChildByTag(TAG_ActorA)->runAction(CCFadeTo::create(0.25, 0));
// 		getChildByTag(TAG_ActorB)->runAction(CCFadeTo::create(0.25, 0));
// 		getChildByTag(TAG_label)->runAction(CCFadeTo::create(0.25, 0));
// 
// 	}
// 	else
// 	{
// 	
// 		//label
// 		CCLabelTTF* label = ((CCLabelTTF*)getChildByTag(TAG_label));
// 		label->setString("");
// 
// 		//下一句对话的 Speaker是谁
// 		string heroName = m_chatContext.at(m_index).first;
// 		if(heroName == "zhujue")
// 		{
// 			((CCSprite*)getChildByTag(TAG_ActorB))->runAction(CCFadeTo::create(0.1, 255));
// 			((CCSprite*)getChildByTag(TAG_ActorA))->runAction(CCFadeTo::create(0.1, 0));
// 		} 
// 		else
// 		{
// 			CCSprite * npc = (CCSprite*)getChildByTag(TAG_ActorA);
// 			npc->initWithFile(ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(heroName).c_str());
// 			npc->setAnchorPoint(ccp(0,0));
// 			npc->setOpacity(0);
// 			npc->runAction(CCFadeTo::create(0.1, 255));
// 			((CCSprite*)getChildByTag(TAG_ActorB))->runAction(CCFadeTo::create(0.1, 0));
// 		}
// 
// 		schedule(schedule_selector(ChatLayer::updateContext));
// 		bUpdateContext = true;
// 		++m_index;
// 	}
// 
// 	return true;
// }
// 
// void ChatLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
// {
// 	
// }
// 
// void ChatLayer::updateContext(float delta)
// {
// 	bUpdateContext = true;
// 	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
// 
// 	int idx = m_index-1;
// 
// 	bool flag = false;
// 	mChatNum += 4;
// 	if(mChatNum >= m_chatContext.at(idx).second.size())
// 	{
// 		flag = true;
// 		mChatNum = m_chatContext.at(idx).second.size();
// 	}
// 	string tempStr(m_chatContext.at(idx).second.begin(), m_chatContext.at(idx).second.begin()+mChatNum);
// 
// 	string speakStr = m_chatContext.at(idx).first;
// 	if(speakStr == "zhujue")
// 		speakStr = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().customName;
// 	speakStr += ":\n   " + tempStr;
// 	((CCLabelTTF*)getChildByTag(TAG_label))->setString(speakStr.c_str());
// 
// 	if(flag)
// 	{
// 		mChatNum = 0;
// 		unschedule(schedule_selector(ChatLayer::updateContext));
// 		bUpdateContext = false;
// 	}
// }
// 
// void ChatLayer::callbackRemove()
// {
// 	this->removeFromParent();
// 	m_callbackClass->chatEndCallBack();
// }
