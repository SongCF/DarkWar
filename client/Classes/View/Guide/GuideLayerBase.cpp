#include "cocos2d.h"
#include "GuideLayerBase.h"
#include "GameDirector.h"
#include "CmdHelper.h"
#include "Debug.h"

USING_NS_CC;


bool isUI_Guide_MainLine = true;
bool isUI_Guide_LearnSkill = false;
bool isUI_Guide_GongJiang = true;
// bool isUI_Guide_MainLine = false;
// bool isUI_Guide_LearnSkill = false;
// bool isUI_Guide_GongJiang = false;




//教学时使用，新任务或新关卡完成后，置为true（可能是通关后的fightSetlementLayer、也可能是LevelUpLayer，亦或是第一次看到什么后触发）
//，在unblockDialog中方便判断，再又各个教程过程中置为false、
bool g_gongjiang_QhHt_Opend = false;
bool g_gongjiang_FenJie_Guide = false;
bool g_gongjiang_DuanZao_Opend = false;
bool g_gongjiang_BsXq_Opend = false;
bool g_gongjiang_BsHc_Opend = false;
//
bool g_ChuanQiFuBen_Opend = false;  bool g_ChuanQiFuBen_Guide = false;
bool g_ChuanQiLu_Opend = false;     bool g_ChuanQiLu_Guide = false;
bool g_ChuanQiSuiPianHeCheng_Guide = false; bool g_ChuanQiSuiPianHeCheng_GuideEnd = false;
bool g_FirstChuanQiEquipLook = false; 
bool g_FirstTaoZhuangLook = false;
bool g_YingXiongBang_Opend = false;
bool g_JingJiChang_Opend = false;
bool g_XueSeJiaoTang_Opend = false;
bool g_JiuGuan_Opend = false;
bool g_JiXianShouCheng_Opend = false;
bool g_DianFengDuiJue_Opend = false;
//第一次通关，主页配装教程
bool g_firstTaskEnd_to_changeEquip = false;
//学技能书教学
bool g_firstLearnSkillBook = false;





GameGuideManager * GameGuideManager::_guideManager = 0;
GameGuideManager::GameGuideManager()
{
	_guideProTemp = _guideProcess = 0xffffffffffffffff;
	_currentGuideState = 0;
}
GameGuideManager * GameGuideManager::getManager()
{
	if (! _guideManager) _guideManager = new GameGuideManager;
	return _guideManager;
}
void GameGuideManager::setGuideProcess(unsigned long long process)
{
	_guideProTemp = _guideProcess = process;

#if DOT_SHOW_GUIDE
	_guideProTemp = _guideProcess = 0xffffffffffffffff;
#endif

}
unsigned long long GameGuideManager::getGuideProcess()
{
	return _guideProcess;
}

void GameGuideManager::setOneGuideStart(GuideProcess idx)
{
	if (isAlreadyGuide(idx)){
		return;
		//CCAssert(false, "已经教学过了");
	}
	else{
		_guideProTemp |= (unsigned long long)1<<idx;
		_currentGuideState = 1; //恒定开始第一步为1
	}
}
void GameGuideManager::setOneGuideEnd(GuideProcess idx)
{
	_guideProcess |= (unsigned long long)1<<idx;
	if (_guideProcess != _guideProTemp) CCAssert(false, "怎么会不相等");

	CmdHelper::getHelper()->cmdSetGuideState(_guideProcess);
}
bool GameGuideManager::isGuideNow(GuideProcess idx)
{
	if (((_guideProcess & (unsigned long long)1<<idx) == 0)
		&& ((_guideProTemp & (unsigned long long)1<<idx) >= 1)
		&& (_currentGuideState > 0))
		return true;
	else 
		return false;
}
bool GameGuideManager::isAlreadyGuide(GuideProcess idx)
{
	if (_guideProcess & (unsigned long long)1<<idx) return true;
	else return false;
}

int GameGuideManager::getCurGuideState()
{
	return _currentGuideState;
}
void GameGuideManager::setCurGuideState(int state)
{
	_currentGuideState = state;
}







void initGuideData() // 必须在进入主页前调用
{
	//test
	return;


	//
	//是否开启教学
	//
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	if (level > 1)
	{
		isUI_Guide_MainLine = false;
	}
	if (level > 12)
	{
		isUI_Guide_LearnSkill = false;
	}
	if (level > 15)
	{
		isUI_Guide_GongJiang = false;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//guide layer base

void GuideLayerBase::init(CCSize size, CCPoint pos, string infoStr, GuideLayerBase* nextLayer, bool swallowAllTouches)
{
	mSwallowAllTouches = swallowAllTouches;
	vector<LightMaskPos> lightPos;
	LightMaskPos temp;
	temp.size = size;
	temp.pos = pos;
	temp.info = infoStr;
	lightPos.push_back(temp);
	init(lightPos, nextLayer, swallowAllTouches);
}

CCSprite* GuideLayerBase::createOneLightNode(CCSize size,CCPoint pos)
{
	CCSprite* node = CCSprite::create("lightNode.png");

	float scalex = size.width / node->getContentSize().width;
	float scaley = size.height / node->getContentSize().height;

	node->setScaleX(scalex);
	node->setScaleY(scaley);

	node->setPosition(pos);

	return node;
}

CCSprite* GuideLayerBase::createLightBg(vector<LightMaskPos> lightPos)
{
	vector<CCSprite*> vec;
	for (int i=0; i!=lightPos.size(); ++i)
	{
		vec.push_back(createOneLightNode(lightPos.at(i).size, lightPos.at(i).pos));
	}

	return LightMaskFactory::getCurrentFactory()->buildSprite(mMask, vec);
}

// 创建一个亮的边框
CCNode* GuideLayerBase::createLightFrame(CCSize size,CCPoint worldGlPos)
{
	CCScale9Sprite* frame = CCScale9Sprite::create("jiaoxue_tishikuang.png");
	frame->setPosition(worldGlPos);
	CCSize conSize = size;
	conSize.width += 20;
	conSize.height += 20;
	frame->setContentSize(conSize);
	return frame;
}

CCNode* GuideLayerBase::createLabel(string text, float labelWidth, ccColor3B color, float fSize)
{
	CCLabelTTF* lable = CCLabelTTF::create(
		text.c_str(),
		fontStr_kaiti,
		fSize,
		CCSizeMake(0,0),kCCTextAlignmentLeft);

	float lableMaxWidth = mVisibleSize.width - LABLE_GAP_WITH_SCREEN * 4;
	if (lable->getContentSize().width > labelWidth)
	{
		lable->setDimensions(CCSizeMake(labelWidth,0));
	}
	if (lable->getContentSize().width > lableMaxWidth)
	{
		lable->setDimensions(CCSizeMake(lableMaxWidth,0));
	}
	lable->setAnchorPoint(ccp(0.5f,0.5f));
	CCScale9Sprite* base = CCScale9Sprite::create("jiaoxue_duihuaqipao.png");
	base->addChild(lable);

	float baseHight = lable->getContentSize().height + LABLE_GAP_WITH_SCREEN*4;

	base->setContentSize(CCSizeMake(
		lable->getContentSize().width + LABLE_GAP_WITH_SCREEN*2,
		baseHight
		));
	lable->setPosition(ccp(base->getContentSize().width/2,base->getContentSize().height/2));
	return base;
}

void GuideLayerBase::autoVerticalLayoutGuideInfor(CCNode* lable,CCNode* downArrow,CCPoint lightNodePos,CCSize lightNodeSize)
{

	float arrowAndLableGap = 10;
	float lightNodeAndArrowGap = 10;

	float lable_posy = 0;
	float arrow_posy = 0;
	float arrow_angle = 0;
	// 寻找合适的上下地方显示描述信息
	{
		if (lightNodePos.y < mVisibleSize.height/2)
		{// 如果描述等放在上面的话，如果在可接受的可见范围，那么就使用“上置”

			arrow_posy = lightNodePos.y + lightNodeSize.height/2 + lightNodeAndArrowGap + downArrow->getContentSize().height/2;
			lable_posy = arrow_posy + downArrow->getContentSize().height/2 + arrowAndLableGap + lable->getContentSize().height/2;
			arrow_angle = 0;
		}
		else
		{// 就放下面
			arrow_posy = lightNodePos.y - lightNodeSize.height/2 - lightNodeAndArrowGap - downArrow->getContentSize().height/2;
			lable_posy = arrow_posy - downArrow->getContentSize().height/2 - arrowAndLableGap - lable->getContentSize().height/2;
			arrow_angle = 180;
		}
	}

	float arrow_posx = lightNodePos.x;
	float lable_posx = lightNodePos.x;
	if (lable_posx + lable->getContentSize().width/2 - mVisibleSize.width > -LABLE_GAP_WITH_SCREEN)
	{
		lable_posx = mVisibleSize.width - LABLE_GAP_WITH_SCREEN - lable->getContentSize().width/2;
	}
	if (lable_posx - lable->getContentSize().width/2 < LABLE_GAP_WITH_SCREEN)
	{
		lable_posx = LABLE_GAP_WITH_SCREEN + lable->getContentSize().width/2;
	}

	lable->setPosition(ccp(lable_posx,lable_posy));
	downArrow->setPosition(ccp(arrow_posx,arrow_posy));
	downArrow->setRotation(arrow_angle);
}

CCSprite* GuideLayerBase::createDownArrow()
{
	return BaseSprite::create("juqing_duihua_xiayiye.png");
}

void GuideLayerBase::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -200, true);

	CCArray* childs = this->getChildren();
	if(childs != NULL)
	{
		for (int i=0; i!=childs->count(); ++i)
		{
			if(((CCNode*)childs->objectAtIndex(i))->getTag() == Tag_LightBg)
			{
				((CCSprite*)childs->objectAtIndex(i))->setOpacity(0);
				((CCSprite*)childs->objectAtIndex(i))->runAction(CCFadeTo::create(1, LightMask_Opacity));
			}
			else
			{
				nodeAndAllChildRunAction((CCNode*)childs->objectAtIndex(i), CCFadeIn::create(1));
			}
		}
	}

}

void GuideLayerBase::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}





/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//GuideLayer_SimpleLayer

void GuideLayer_SimpleLayer::init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches)
{

	mSwallowAllTouches = swallowAllTouches;
	mNextCallback = nextLayer;
	if(mNextCallback != NULL)
		mNextCallback->retain();

	CCSprite *bg = createLightBg(lightPos);
	addChild(bg, -1, Tag_LightBg);
	bg->setPosition(ccp(mVisibleSize.width/2, bg->getContentSize().height/2));
	bg->setOpacity(LightMask_Opacity);

	//////////////////////////////////////////////////////////////////////
	CCSprite* arraw = createDownArrow();
	addChild(arraw, 1);
	CCNode *labelBoard = createLabel(lightPos.at(0).info);
	addChild(labelBoard, 1);
	autoVerticalLayoutGuideInfor(labelBoard, arraw, lightPos.at(0).pos, lightPos.at(0).size);

	CCNode *lightBoard = createLightFrame(lightPos.at(0).size, lightPos.at(0).pos);
	addChild(lightBoard, 1);
	if(! mSwallowAllTouches)
	{
		arraw->setPositionY(arraw->getPositionY() - 10);
		arraw->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5, ccp(0,20)), CCMoveBy::create(0.5, ccp(0,-20)), NULL)));
		lightBoard->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1), CCFadeOut::create(1), NULL)));
	}

	if(lightPos.at(0).info.empty())
		labelBoard->removeFromParent();

	mClickedPos = lightPos.at(0).pos;
	mClickedSize = lightPos.at(0).size;
}

bool GuideLayer_SimpleLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (m_pTouch == pTouch)
	{
		return false;
	}

	return true;
}


void GuideLayer_SimpleLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	float deltaX = mClickedSize.width * 0.1;
	float deltaY = mClickedSize.height* 0.1;

	CCPoint worldPos = pTouch->getLocation();

	if (mSwallowAllTouches)  //吞噬  不下传
	{
		if(mNextCallback != NULL)
		{
			this->getParent()->addChild(mNextCallback, this->getZOrder());
			mNextCallback->setPosition(this->getPosition());
		}

		//this->removeFromParent();
		if (m_oTarger)
		{
			(m_oTarger->*m_hHandler)();
		}
		this->runAction(CCRemoveSelf::create(true));
		return;
	}
	else if(! mSwallowAllTouches   //需要下传
		&& worldPos.x >= mClickedPos.x-mClickedSize.width/2 + deltaX
		&& worldPos.x <= mClickedPos.x+mClickedSize.width/2 - deltaX
		&& worldPos.y >= mClickedPos.y-mClickedSize.height/2 + deltaY
		&& worldPos.y <= mClickedPos.y+mClickedSize.height/2 - deltaY)
	{
		if(mNextCallback != NULL)
		{
			this->getParent()->addChild(mNextCallback, this->getZOrder());
			mNextCallback->setPosition(this->getPosition());
		}
		if (m_oTarger)
		{
			(m_oTarger->*m_hHandler)();
		}

		//然后重新派发出一个 触摸消息给低优先级的（该touch已被吞噬）
		m_pTouch = pTouch;
		m_pTouch->retain();
		scheduleOnce(schedule_selector(GuideLayer_SimpleLayer::removeSelfAndDispatchTouchEnd), 0);

		return;
	}
}

void GuideLayer_SimpleLayer::removeSelfAndDispatchTouchEnd(float dt)
{
	//when self touch end
	CCPoint _pos = CCDirector::sharedDirector()->convertToUI(mClickedPos);
	m_pTouch->setTouchInfo(m_pTouch->getID(), _pos.x, _pos.y); // 矫正到正中心
	CCSet* _set = CCSet::create();
	_set->addObject(m_pTouch);
	m_pTouch->release();
	CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(_set, NULL);
	CCDirector::sharedDirector()->getTouchDispatcher()->touchesEnded(_set, NULL);

	this->removeFromParent();
}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//GuideLayer_SimpleJumpLayer

bool GuideLayer_SimpleJumpLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//GuideLayer_JiuGongGeSpecial

void GuideLayer_JiuGongGeSpecial::init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches)
{
	mSwallowAllTouches = swallowAllTouches;
	mNextCallback = nextLayer;
	if(mNextCallback != NULL)
		mNextCallback->retain();

	CCSprite *bg = createLightBg(lightPos);
	addChild(bg, -1, Tag_LightBg);
	bg->setPosition(ccp(mVisibleSize.width/2, bg->getContentSize().height/2));
	bg->setOpacity(LightMask_Opacity);

	//////////////////////////////////////////////////////////////////////
	CCNode *labelBoard = createLabel(lightPos.at(0).info);
	addChild(labelBoard, 1);
	labelBoard->setPosition(ccp(mWinSize.width/2, (lightPos.at(0).pos.y+lightPos.at(1).pos.y)/2));

	CCSprite* arraw1 = createDownArrow();
	addChild(arraw1, 1);
	arraw1->setRotation(90);
	arraw1->setPosition(ccp(lightPos.at(1).pos.x+lightPos.at(1).size.width, lightPos.at(1).pos.y));
	CCSprite* arraw2 = createDownArrow();
	addChild(arraw2, 1);
	arraw2->setRotation(-90);
	arraw2->setPosition(ccp(lightPos.at(0).pos.x-lightPos.at(0).size.width, lightPos.at(0).pos.y));

	CCNode *node1 = createLightFrame(lightPos.at(0).size, lightPos.at(0).pos);
	addChild(node1, 1);
	CCNode *node2 = createLightFrame(lightPos.at(1).size, lightPos.at(1).pos);
	addChild(node2, 1);

	if(! swallowAllTouches)
	{
		arraw1->setPositionX(arraw1->getPositionX() - 10);
		arraw2->setPositionX(arraw2->getPositionX() + 10);
		arraw1->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5, ccp(20,0)), CCMoveBy::create(0.5, ccp(-20,0)), NULL)));
		arraw2->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5, ccp(-20,0)), CCMoveBy::create(0.5, ccp(20,0)), NULL)));
		node1->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1), CCFadeOut::create(1), NULL)));
		node2->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1), CCFadeOut::create(1), NULL)));
	}

	mClickedPos[0] = lightPos.at(0).pos;
	mClickedSize[0] = lightPos.at(0).size;
	mClickedPos[1] = lightPos.at(1).pos;
	mClickedSize[1] = lightPos.at(1).size;
}

bool GuideLayer_JiuGongGeSpecial::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	float deltaX = mClickedSize[0].width * 0.1;
	float deltaY = mClickedSize[0].height* 0.1;

	CCPoint worldPos = pTouch->getLocation();
	if (mSwallowAllTouches)
	{
		if(mNextCallback != NULL)
		{
			this->getParent()->addChild(mNextCallback, this->getZOrder());
			mNextCallback->setPosition(this->getPosition());
		}
		this->removeFromParent();
		return true;
	}
	else if(! mSwallowAllTouches
		&& worldPos.x >= mClickedPos[0].x-mClickedSize[0].width/2 + deltaX
		&& worldPos.x <= mClickedPos[0].x+mClickedSize[0].width/2 - deltaX
		&& worldPos.y >= mClickedPos[0].y-mClickedSize[0].height/2 + deltaY
		&& worldPos.y <= mClickedPos[0].y+mClickedSize[0].height/2 - deltaY)
	{
		this->removeFromParent();
		return false;
	}
	else if(! mSwallowAllTouches
		&& worldPos.x >= mClickedPos[1].x-mClickedSize[1].width/2 + deltaX
		&& worldPos.x <= mClickedPos[1].x+mClickedSize[1].width/2 - deltaX
		&& worldPos.y >= mClickedPos[1].y-mClickedSize[1].height/2 + deltaY
		&& worldPos.y <= mClickedPos[1].y+mClickedSize[1].height/2 - deltaY)
	{
		this->removeFromParent();
		return false;
	}


	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//GuideLayer_RenXingMianBan_Special

void GuideLayer_RenXingMianBan_Special::init(CCSize size, CCPoint pos, vector<string> infoStrs, GuideLayerBase* nextLayer, bool swallowAllTouches)
{
	mSwallowAllTouches = swallowAllTouches;
	mNextCallback = nextLayer;
	if(mNextCallback != NULL)
		mNextCallback->retain();

	vector<LightMaskPos> vec;
	LightMaskPos templight;
	templight.size = size;
	templight.pos  = pos;
	vec.push_back(templight);
	CCSprite *bg = createLightBg(vec);
	addChild(bg, -1, Tag_LightBg);
	bg->setPosition(ccp(mVisibleSize.width/2, bg->getContentSize().height/2));
	bg->setOpacity(LightMask_Opacity);

	//////////////////////////////////////////////////////////////////////
	for (int i=0; i!=infoStrs.size(); ++i)
	{
		CCSprite* arraw = createDownArrow();
		addChild(arraw, 1);
		CCNode *labelBoard = createLabel(infoStrs.at(i));
		addChild(labelBoard, 1);
		autoVerticalLayoutGuideInfor(labelBoard, arraw, ccp(90+i*150, pos.y), size);
	}

	CCNode *lightBoard = createLightFrame(size, pos);
	addChild(lightBoard, 1);

}

bool GuideLayer_RenXingMianBan_Special::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	float deltaX = mClickedSize.width * 0.1;
	float deltaY = mClickedSize.height* 0.1;

	CCPoint worldPos = pTouch->getLocation();
	if (mSwallowAllTouches)
	{
		if(mNextCallback != NULL)
		{
			this->getParent()->addChild(mNextCallback, this->getZOrder());
			mNextCallback->setPosition(this->getPosition());
		}
		this->removeFromParent();
		return true;
	}

	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//GuideLayer_MultLightAndLabel
void GuideLayer_MultLightAndLabel::init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches)
{
	mSwallowAllTouches = swallowAllTouches;
	mNextCallback = nextLayer;
	if(mNextCallback != NULL)
		mNextCallback->retain();

	CCSprite *bg = createLightBg(lightPos);
	addChild(bg, -1, Tag_LightBg);
	bg->setPosition(ccp(mVisibleSize.width/2, bg->getContentSize().height/2));
	bg->setOpacity(LightMask_Opacity);

	//////////////////////////////////////////////////////////////////////
	for (int i=0; i!=lightPos.size(); ++i)
	{
		CCSprite* arraw = createDownArrow();
		addChild(arraw, 1);
		CCNode *labelBoard = createLabel(lightPos.at(i).info, 250);
		addChild(labelBoard, 1);
		CCNode *lightNode = createLightFrame(lightPos.at(i).size, lightPos.at(i).pos);
		addChild(lightNode, 1);
		if (! swallowAllTouches)
		{
			arraw->setPositionX(arraw->getPositionX() + 10);
			arraw->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5, ccp(-20,0)), CCMoveBy::create(0.5, ccp(20,0)), NULL)));
			lightNode->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1), CCFadeOut::create(1), NULL)));
		}
		autoCenterLayoutGuideInfor(labelBoard, arraw, lightPos.at(i).pos, lightPos.at(i).size);
	}

	mLightPos = lightPos;
}

bool GuideLayer_MultLightAndLabel::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint worldPos = pTouch->getLocation();
	if (mSwallowAllTouches)
	{
		if(mNextCallback != NULL)
		{
			this->getParent()->addChild(mNextCallback, this->getZOrder());
			mNextCallback->setPosition(this->getPosition());
		}
		this->removeFromParent();
		return true;
	}
	else if(! mSwallowAllTouches)
	{
		//bug：位置和触摸点都会向下偏移 footer的高度

		for (int i=0; i!=mLightPos.size(); ++i)
		{
			if(! mSwallowAllTouches
				&& worldPos.x >= mLightPos.at(i).pos.x-mLightPos.at(i).size.width/2 + mLightPos.at(i).size.width*0.1
				&& worldPos.x <= mLightPos.at(i).pos.x+mLightPos.at(i).size.width/2 - mLightPos.at(i).size.width*0.1
				&& worldPos.y >= mLightPos.at(i).pos.y-mLightPos.at(i).size.height/2 + mLightPos.at(i).size.height*0.1
				&& worldPos.y <= mLightPos.at(i).pos.y+mLightPos.at(i).size.height/2 - mLightPos.at(i).size.height*0.1)
			{
				this->removeFromParent();
				return false;
			}
		}
	}


	return true;
}

void GuideLayer_MultLightAndLabel::autoCenterLayoutGuideInfor(CCNode* lable,CCNode* downArrow,CCPoint lightNodePos,CCSize lightNodeSize)
{
	float lightNodeAndArrowGap = 10;
	float arrowAndLableGap = 10;

	float lable_posx = 0;
	float arrow_posx = 0;
	float arrow_angle = 0;
	// 如果在左右两边，则autoCenter   否则调用 autoVe
	if((lightNodePos.x < 130 || lightNodePos.x > 510)
		&& (lightNodePos.y < mVisibleSize.height-75 || lightNodePos.y > 60))
	{
		if (lightNodePos.x < 130)
		{// light在左  描述放在右边
			arrow_posx = lightNodePos.x + lightNodeSize.width/2 + lightNodeAndArrowGap + downArrow->getContentSize().width/2;
			lable_posx = arrow_posx + downArrow->getContentSize().width/2 + arrowAndLableGap + lable->getContentSize().width/2;
			arrow_angle = 90;
		}
		else
		{// 
			arrow_posx = lightNodePos.x - lightNodeSize.width/2 - lightNodeAndArrowGap - downArrow->getContentSize().width/2;
			lable_posx = arrow_posx - downArrow->getContentSize().width/2 - arrowAndLableGap - lable->getContentSize().width/2;
			arrow_angle = -90;
		} 
	}
	else
	{
		autoVerticalLayoutGuideInfor(lable,  downArrow, lightNodePos, lightNodeSize);
	}

	float arrow_posy = lightNodePos.y;
	float lable_posy = lightNodePos.y;
	if (lable_posy + lable->getContentSize().height/2 - mVisibleSize.height > -LABLE_GAP_WITH_SCREEN)
	{
		lable_posy = mVisibleSize.height - lable->getContentSize().height/2 - LABLE_GAP_WITH_SCREEN;
	}
	if (lable_posy - lable->getContentSize().height/2 < LABLE_GAP_WITH_SCREEN)
	{
		lable_posy = LABLE_GAP_WITH_SCREEN + lable->getContentSize().height/2;
	}

	lable->setPosition(ccp(lable_posx,lable_posy));
	downArrow->setPosition(ccp(arrow_posx,arrow_posy));
	downArrow->setRotation(arrow_angle);
}