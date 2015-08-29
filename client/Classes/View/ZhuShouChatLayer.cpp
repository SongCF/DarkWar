
#include "ZhuShouChatLayer.h"
#include "font.h"
#include "Music.h"
#include "MyselfManager.h"
#include "ChatContentManager.h"
#include "NetLabelTTF.h"
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

ZhuShouChatLayer* ZhuShouChatLayer::create(vector<ShowMsg> &msg,int zhushou_id, ZhuShouChatLayerCallBack* callBackInterface)
{
	if(msg.size() > 0)
	{
		ZhuShouChatLayer* layer = new ZhuShouChatLayer(msg,zhushou_id, callBackInterface);
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
		callBackInterface->ZhuShouchatEndCallBack();
		return NULL;
	}
}

ZhuShouChatLayer::ZhuShouChatLayer(vector<ShowMsg> &msg,int zhushou_id, ZhuShouChatLayerCallBack* callBackInterface)
{
	m_nChildCount = 0;
	m_sChatMsg = msg;
	m_callbackClass = callBackInterface;

	mLayerEnd = false;
	mCurMusicId = 0;

	m_index = 0;
	mTimeEnd = true;
	mLabelFadeInEnd = true;
	m_nZhuaShouId = zhushou_id;
}

ZhuShouChatLayer::~ZhuShouChatLayer()
{

}


bool ZhuShouChatLayer::init()
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
		board->setScaleY(0.7f);
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
		string npcFileName = getFileByName(getPinyinName(m_nZhuaShouId)).second;
		////////////////////////////////////////////
		if(npcFileName.empty())
		{
			CCLog("Error: %s [can't found npc imgine]", __FUNCTION__);
			npcFileName = "juqing_duihua_NPC_shenmiren.png";
		}
		////////////////////////////////////////////
		int x_side = 20;
		CCSprite* actorA = CCSprite::create(npcFileName.c_str());
		addChild(actorA, 3, TAG_ActorA);
		actorA->setAnchorPoint(ccp(0,0));
		actorA->setPosition(ccp(x_side,x_side));
		actorA->setOpacity(0);

		int side = 10;
		//label name
		CCLabelTTF *namelabel = CCLabelTTF::create("", fontStr_kaiti, 24);
		addChild(namelabel, 5, Tag_Label_Name);
		namelabel->setAnchorPoint(ccp(0, 0.5));
		namelabel->setPosition(ccp(20, board->getContentSize().height-42));
		namelabel->setColor(fonColor_PuTong);
		namelabel->setOpacity(0);
		//label content
		//CCLabelTTF* label = CCLabelTTF::create("", fontStr_kaiti, 24, CCSize(board->getContentSize().width - actorA->getContentSize().width - side, 0), kCCTextAlignmentLeft);
		m_nTextWidth = board->getContentSize().width - actorA->getContentSize().width - side;
		NetLabelTTF* label = NetLabelTTF::create("", fontStr_kaiti, 24);
		addChild(label, 5, TAG_label);
		label->setHorizontalAlignment(kCCTextAlignmentLeft);
		label->setAnchorPoint(ccp(0,1));
		label->setColor(fonColor_FaGuang);
		label->setPosition(ccp(actorA->getContentSize().width + side, board->getContentSize().height-145));
		label->setOpacity(0);

		actorA->runAction(CCFadeIn::create(0.0f));
		board->runAction(CCFadeIn::create(0.0f));
		label->runAction(CCFadeIn::create(0.0f));
		namelabel->runAction(CCFadeIn::create(0.0f));
		nextImg->runAction(CCFadeIn::create(0.0f));
		scheduleOnce(schedule_selector(ZhuShouChatLayer::showChatContext), 0.05f);
		{
			mTimeEnd = false;
			mLabelFadeInEnd = false;
			++m_index;
		}

		bRet = true;
	}while(0);
	
	return bRet;
}

void ZhuShouChatLayer::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1024, true);

	this->setPositionY(200);
}

void ZhuShouChatLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool ZhuShouChatLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (   (mLabelFadeInEnd && mTimeEnd)
		|| TOUCH_ENABLE)
	{
		if (TOUCH_ENABLE)
		{
			//
			unschedule(schedule_selector(ZhuShouChatLayer::showChatContext));
			unschedule(schedule_selector(ZhuShouChatLayer::saidChatContextEnd));
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



	if ( mLayerEnd) //避免渐渐消失的时候动作重复闪速
		return true;

	mLayerEnd = true;
	nodeAndAllChildRunAction(this,CCFadeOut::create(0.25f),CCSequence::create(
		CCFadeOut::create(0.25f)
		, CCCallFunc::create(this, SEL_CallFunc(&ZhuShouChatLayer::callbackRemove))
		, NULL));
	

	//board->runAction(CCFadeTo::create(0.0f, 0));
	//actorA->runAction(CCFadeTo::create(0.0f, 0));
	//actorB->runAction(CCFadeTo::create(0.25f, 0));
	//label->runAction(CCFadeTo::create(0.0f, 0));
	//nameLabel->runAction(CCFadeTo::create(0.0f, 0));

	//nextImg->stopAllActions();
	//nextImg->runAction(CCFadeTo::create(0.0f, 0));
	
	return true;
}

void ZhuShouChatLayer::showChatContext(float delta)
{
	
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	int idx = m_index-1;

	string speakStr = "";

	NetTextListShowUI* pText = NetTextListShowUI::create();


	std::string fullStr = "";
	for (vector<ShowMsg>::iterator iter = m_sChatMsg.begin(); iter != m_sChatMsg.end() ; iter++)
	{
		fullStr += pText->getConversionMsgStr(iter->msg, iter->color);
		
	}
	NetLabelTTF * label = ((NetLabelTTF*)getChildByTag(TAG_label));
	pText->setTextSize(label->getFontSize());
	pText->setTextWidth(m_nTextWidth);
	pText->addMessage(fullStr);
	
	label->setString(pText->getMsgString().c_str());
	label->setMsgColorTexCoords(pText->getMsgColorTexCoordsList());
	label->setHeight(pText->getHeight());


	//string chatStr(m_sChatMsg);

	CCLabelTTF *nameLabel = ((CCLabelTTF*)getChildByTag(Tag_Label_Name));
	
	nameLabel->setString(speakStr.c_str());
	//label->setString(chatStr.c_str());
	nameLabel->setOpacity(0);
	label->setOpacity(0);
	nameLabel->runAction(CCSequence::create(
		CCFadeTo::create(0.0f, 255)
		, NULL));
	label->runAction(CCSequence::create(
		CCFadeTo::create(0.0f, 255)
		, CCCallFunc::create(this, callfunc_selector(ZhuShouChatLayer::labelFadeInEnd))
		, NULL));
}

void ZhuShouChatLayer::labelFadeInEnd()
{
	mLabelFadeInEnd = true;

	if (mLabelFadeInEnd && mTimeEnd)
		ccTouchBegan(0, 0);
}

void ZhuShouChatLayer::saidChatContextEnd(float dt)
{
	mTimeEnd = true;
	//Music::sharedMusicPlayer()->unLoadEffectSound(m_chatContext.at(m_index-1).musicFile);

	if (mLabelFadeInEnd && mTimeEnd)
		ccTouchBegan(0, 0);
}

void ZhuShouChatLayer::callbackRemove()
{
	this->removeFromParent();
	//nodeAndAllChildRunAction(this,CCRemoveSelf::create());
	m_callbackClass->ZhuShouchatEndCallBack();
}
void ZhuShouChatLayer::nodeAndAllChildRunAction(CCNode* node, CCAction* action,CCAction *actionOver)
{
	if(node == NULL || action == NULL)
		return;
	m_nChildCount ++;
	//childe
	CCArray* childs = node->getChildren();
	if (childs != NULL)
	{
		for (int i=0; i!=childs->count(); ++i)
		{
			nodeAndAllChildRunAction((CCNode*)childs->objectAtIndex(i), (CCAction*)action->copy()->autorelease(),NULL);
		}
	}
	m_nChildCount--;
	if (m_nChildCount == 0)
	{
		node->runAction(actionOver);
	}
	//自己
	
}

pair<string, string> ZhuShouChatLayer::getFileByName(string name)
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
string ZhuShouChatLayer::getPinyinName(int id)
{
	string name[10] = {"remanren","cike","fashi","qishi","shenjianshou","mishushi","wuseng","youxia","lieren", "wushi"};
	return name[getZhuShouId(id) - 1];
}
int ZhuShouChatLayer::getZhuShouId(int index)
{
	Profession id = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;

	switch (id)
	{
	case Profession_Yemanren:
		{
			if (index == 1)
			{
				return Profession_MiShu;
			}
			else if (index == 2)
			{
				return Profession_YouXia;
			}
			else if (index == 3)
			{
				return Profession_LieShou;
			}
		}
		break;
	case Profession_Chike:
		{
			if (index == 1)
			{
				return Profession_LieShou;
			}
			else if (index == 2)
			{
				return Profession_MiShu;
			}
			else if (index == 3)
			{
				return Profession_Qishi;
			}
		}
		break;
	case Profession_Fashi:
		{
			if (index == 1)
			{
				return Profession_YouXia;
			}
			else if (index == 2)
			{
				return Profession_Qishi;
			}
			else if (index == 3)
			{
				return Profession_LieShou;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}