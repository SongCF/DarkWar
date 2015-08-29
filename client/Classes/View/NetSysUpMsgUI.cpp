#include "NetSysUpMsgUI.h"
#include "NetTextListShowUI.h"
#include "NetLabelTTF.h"
#include "font.h"
NetSysUpMsgUI::NetSysUpMsgUI()
	:CCLayer()
	,m_nUpdataCount(0)
{
}
NetSysUpMsgUI::~NetSysUpMsgUI()
{
}
CCScene* NetSysUpMsgUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	NetSysUpMsgUI *layer = NetSysUpMsgUI::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool NetSysUpMsgUI::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	
	for (int i = 0 ; i < 0;i++)
	{
		UpdataMsgInfo info;
		info.isImportant = false;
		info.info = CCString::createWithFormat("%d:  %s",i,"this is a message!")->getCString();
		m_mUpdataMsg.infoList.push_back(info);
	}
	
	
	return true;
}
void NetSysUpMsgUI::setUpdataMsg(const vector<string> str,string title)
{
	m_mUpdataMsg.updataMsgTitle = title;

	for (int i = 0 ; i < str.size();i++)
	{
		UpdataMsgInfo info;
		info.isImportant = false;
		info.info = CCString::createWithFormat("%s\n",str.at(i).c_str())->getCString();
		m_mUpdataMsg.infoList.push_back(info);
	}
}
void NetSysUpMsgUI::setActivityMsg(vector<Flat_SystempNotifyInfoRsp::strACTIVITY> activityVec)
{
	for (int i = 0 ; i < activityVec.size();i++)
	{
		HuoDongMsg msg;
		msg.updataMsgTitle = activityVec.at(i).title;
		msg.info = activityVec.at(i).content;
		msg.level = 1;
		if (activityVec.at(i).type == NoticeActivityType_SilverStar)
		{
			msg.level = 1;
		}
		else
		{
			msg.level = 3;
		}
		
		msg.timeStr = activityVec.at(i).time;
		m_mHuoDongMsg.push_back(msg);
	}
}
void NetSysUpMsgUI::onEnter()
{
	CCLayer::onEnter();
	
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-130,true);

	/*CCLayerColor *colorLayer = CCLayerColor::create(ccc4BFromccc4F(ccc4f(255,255,255,100)));
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
	addChild(colorLayer,-1);*/

	CCSprite *mBgSpr = CCSprite::create("background1.jpg");
	addChild(mBgSpr,-1);
	mBgSpr->setPosition(ccp(mBgSpr->getContentSize().width/2, mBgSpr->getContentSize().height/2));


	CCSprite *bgSpr = CCSprite::create("zhuye_gonggao_bg.png");
	bgSpr->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2,CCDirector::sharedDirector()->getWinSize().height / 2));
	mBgSpr->addChild(bgSpr);
	m_sBackSpr = bgSpr;
	
	m_mMenu = CCMenu::create();
	//m_mMenu->ignoreAnchorPointForPosition(false);
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	m_mMenu->setContentSize(bgSpr->getContentSize());
	bgSpr->addChild(m_mMenu,2);
	m_mMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(NetSysUpMsgUI::changeMenuPriority_callfuncND), (void*)(-131)));
	CCSprite *titleSpr = CCSprite::create("gonggao_biaoti.png");
	titleSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - 20));
	bgSpr->addChild(titleSpr);

	int side = 0;
	CCPoint pos = ccp(titleSpr->getPositionX(),titleSpr->getPositionY() - titleSpr->getContentSize().height / 2 - side);
	if (m_mUpdataMsg.infoList.size() > 0)
	{
		CCSprite *upBgTitleSpr = CCSprite::create("gonggao_biaoti_banbengongxini.png");
		upBgTitleSpr->setPosition(ccp(pos.x,pos.y - upBgTitleSpr->getContentSize().height / 2));
		bgSpr->addChild(upBgTitleSpr);

		CCLabelTTF *ttf = CCLabelTTF::create(m_mUpdataMsg.updataMsgTitle.c_str(),fontStr_kaiti,24.0f);
		ttf->setColor(ccWHITE);
		ttf->setPosition(ccp(upBgTitleSpr->getContentSize().width / 2, upBgTitleSpr->getContentSize().height / 2));
		upBgTitleSpr->addChild(ttf);
		string inputStr = "";
		NetTextListShowUI *listShow = NetTextListShowUI::create();
		for (vector<UpdataMsgInfo>::iterator iter = m_mUpdataMsg.infoList.begin(); iter != m_mUpdataMsg.infoList.end() ; iter ++)
		{
			if (iter->isImportant)
			{
				inputStr += listShow->getConversionMsgStr(iter->info,ccGREEN);
			}
			else
			{
				inputStr += listShow->getConversionMsgStr(iter->info,ccWHITE);
			}
		}
		CCSprite *infoBgSpr = CCSprite::create("gonggao_neirong_kuang.png");

		infoBgSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,upBgTitleSpr->getPositionY() - upBgTitleSpr->getContentSize().height / 2 - infoBgSpr->getContentSize().height / 2 - side));
		bgSpr->addChild(infoBgSpr);

		CCSprite *infoBgSpr2 = CCSprite::create("gonggao_neirong_kuang_2.png");

		/*infoBgSpr2->setPosition(ccp(bgSpr->getContentSize().width / 2,infoBgSpr->getPositionY() - infoBgSpr->getContentSize().height / 2 - infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2 - side));
		bgSpr->addChild(infoBgSpr2);*/


		CCSprite *infoBgSpr3 = CCSprite::createWithTexture(infoBgSpr->getTexture());

		infoBgSpr3->setRotationX(180);

		listShow->setAnchorPoint(ccp(0,1));
		listShow->setTextSize(20);
		listShow->setTextWidth(infoBgSpr->getContentSize().width - 20);
		listShow->addMessage(inputStr);
		
		NetLabelTTF *ttf2 = NetLabelTTF::create(listShow->getMsgString().c_str(),fontStr_kaiti,20.0f);
		infoBgSpr2->setScaleY(ttf2->getContentSize().height / infoBgSpr2->getContentSize().height);
		infoBgSpr2->setPosition(ccp(bgSpr->getContentSize().width / 2,infoBgSpr->getPositionY() - infoBgSpr->getContentSize().height / 2 - infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2));
		bgSpr->addChild(infoBgSpr2);
		infoBgSpr3->setPosition(ccp(bgSpr->getContentSize().width / 2,infoBgSpr2->getPositionY() - infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2 + infoBgSpr3->getContentSize().height / 2));
		bgSpr->addChild(infoBgSpr3);



		ttf2->setHorizontalAlignment(kCCTextAlignmentLeft);
		ttf2->setHeight(listShow->getHeight());
		ttf2->setMsgColorTexCoords(listShow->getMsgColorTexCoordsList());
		ttf2->setPosition(ccp(ttf2->getContentSize().width / 2 + 60,infoBgSpr->getPositionY() + infoBgSpr->getContentSize().height * infoBgSpr->getScaleY() / 2 - ttf2->getContentSize().height / 2));
		bgSpr->addChild(ttf2,2);

		pos = infoBgSpr3->getPosition();
		pos.y -= infoBgSpr3->getContentSize().height/ 2;
	}
	int _width = 555;
	int _height = pos.y - 105;
	NetSysUpMsgList *listUI = NetSysUpMsgList::create();
	listUI->setEnveroment(_width - 10,10);
	for (vector<HuoDongMsg>::iterator iter = m_mHuoDongMsg.begin(); iter != m_mHuoDongMsg.end();iter ++)
	{
		listUI->addSysMsg(*iter);
	}
	int height = listUI->getHeight();
	listUI->ignoreAnchorPointForPosition(false);
	listUI->setAnchorPoint(CCPointZero);
	//listUI->setContentSize(CCSizeMake(_width,height));
	
	CCScrollView *m_cScroll = CCScrollView::create(CCSizeMake(_width,_height),listUI);
	m_cScroll->setDirection(kCCScrollViewDirectionVertical);
	m_cScroll->setContentSize(CCSizeMake(_width,height));

	m_cScroll->setContentOffset(ccp(0,-(height - _height)));
	//listUI->setEnveroment(height,side);
	m_cScroll->setAnchorPoint(CCPointZero);

	if (height < _height)
	{
		m_cScroll->setPosition(ccp(40,105 + _height - height));
	}
	else
	{
		m_cScroll->setPosition(ccp(40,105));
	}
	//m_cAuctionList->setMenuHandler(this,menu_selector(CardAuctionUI::onRandSelectCheck),menu_selector(CardAuctionUI::onRandSelectBuy));
	bgSpr->addChild(m_cScroll,10);
	m_cScroll->setTouchPriority(-132);

	
	
	/*{//关闭
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuangbei_xiangxi_guanbi.png","zhuangbei_xiangxi_guanbi_select.png","zhuangbei_xiangxi_guanbi_select.png",this,SEL_MenuHandler(&NetSysUpMsgUI::closeThisLayer));
		closeItem->setPosition(ccp(bgSpr->getContentSize().width - closeItem->getContentSize().width / 3,bgSpr->getContentSize().height - closeItem->getContentSize().height / 3));
		m_mMenu->addChild(closeItem);
	}*/
	{//开始游戏
		CCMenuItemImage *closeItem = CCMenuItemImage::create("gonggao_anniu_kaishiyouxi.png","gonggao_anniu_kaishiyouxi_select.png","gonggao_anniu_kaishiyouxi_select.png",this,SEL_MenuHandler(&NetSysUpMsgUI::startGameClick));
		closeItem->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - 687));
		m_mMenu->addChild(closeItem);
	}
}
void NetSysUpMsgUI::onExit() {
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

bool NetSysUpMsgUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void NetSysUpMsgUI::startGameClick(CCObject *obj)
{
	
	if (m_oTarger)
	{
		CCNode *node = CCNode::create();
		node->setTag(1);
		(m_oTarger->*m_hCallBackHandler)(node);
	}
}
string NetSysUpMsgUI::getMsgStr(string msg,ccColor3B color)
{
	string inputStr = "";
	inputStr += "[\n";
	CCString *str = CCString::createWithFormat("%d",0);
	inputStr += str->getCString();
	inputStr += "\n";
	inputStr += msg;
	inputStr += "|\n";
	str = CCString::createWithFormat("%d,%d,%d",color.r,color.g,color.b);
	inputStr += str->getCString();
	inputStr += "\n";
	str = CCString::createWithFormat("%d",0);
	inputStr += str->getCString();
	inputStr += "\n";
	str = CCString::createWithFormat("%d",0);
	inputStr += str->getCString();
	inputStr += "\n";
	inputStr += "]\n";
	return inputStr;
}