#include "NetGameSetUI.h"
#include "BaseActor.h"
#include "MyselfManager.h"
#include "font.h"
#include "GameInforEditer.h"
#include "VipExplainLayer.h"
#include "IMainLayerBody.h"
#include "GameDirector.h"
#include "NetBaseTouchLayer.h"
NetGameSetUI::NetGameSetUI()
	:BaseLayer()
{
}
NetGameSetUI::~NetGameSetUI()
{
}
CCScene* NetGameSetUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	NetGameSetUI *layer = NetGameSetUI::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool NetGameSetUI::init()
{
	if (!BaseLayer::init())
	{
		return false;
	}
	return true;
}
void NetGameSetUI::onEnter()
{
	BaseLayer::onEnter();

	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-130,true);

	int teamNumber = MyselfManager::getManager()->getMyZhuShous_ZhenShen(Actor_All,LF_UnLock).size() + 1;
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	BaseActorProperty pro = actor->getCurrentProperty();
	string name = pro.customName;

	int dengJi = pro.level;

	CCString *imgName = CCString::createWithFormat("touxiang_%s_danshou.png",pro.baseName.c_str());
	int type = 0;
	if (pro.baseName == "barbarian")
	{
		type = 3;
	}
	else if (pro.baseName == "hunter")
	{
		type = 1;
	}
	else
	{
		type = 2;
	}

	CCLayerColor *colorLayer = CCLayerColor::create(ccc4(0,0,0,125));
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
	addChild(colorLayer,-1);

	CCSprite *bgSpr = CCSprite::create("zhuye_shezhi_bg.png");
	bgSpr->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2,CCDirector::sharedDirector()->getWinSize().height / 2));
	addChild(bgSpr,-1);
	m_sBackSpr = bgSpr;

	m_mMenu = CCMenu::create();
	//m_mMenu->ignoreAnchorPointForPosition(false);
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	m_mMenu->setContentSize(bgSpr->getContentSize());
	bgSpr->addChild(m_mMenu,2);
	m_mMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(NetGameSetUI::changeMenuPriority_callfuncND), (void*)(-131)));
	CCSprite *titleSpr = CCSprite::create("zhuye_shezhi_biaoti_shezhi.png");
	titleSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - 20));
	bgSpr->addChild(titleSpr);

	{//头像
		CCSprite *headSpr = CCSprite::create(imgName->getCString());
		headSpr->setPosition(ccp(110,623));
		bgSpr->addChild(headSpr);
		headSpr->setScale(0.8f);

		CCMenuItemImage *headItem = CCMenuItemImage::create("renxing_renwu_touxiang.png","renxing_renwu_touxiang_select.png","renxing_renwu_touxiang_select.png",this,SEL_MenuHandler(&NetGameSetUI::heroClick));
		headItem->setPosition(headSpr->getPosition());
		m_mMenu->addChild(headItem);
		//m_lMenuItemList.push_back(headItem);
		headItem->setScale(0.8f);

		CCMenuItemImage *changeHeadItem = CCMenuItemImage::create("zhuye_shezhi_anniu_genghuantouxiang.png","zhuye_shezhi_anniu_genghuantouxiang_select.png","zhuye_shezhi_anniu_genghuantouxiang_select.png",this,SEL_MenuHandler(&NetGameSetUI::heroClick));
		changeHeadItem->setPosition(ccp(headSpr->getPositionX(),headSpr->getPositionY() - headSpr->getContentSize().height/ 2 - changeHeadItem->getContentSize().height / 2 + 5));
		m_mMenu->addChild(changeHeadItem);
		//m_lMenuItemList.push_back(changeHeadItem);

		headItem->setEnabled(false);
		changeHeadItem->setEnabled(false);
	}
	{//名字什么的
		int side = 36;
		//名字
		CCLabelTTF *niChengTTF = CCLabelTTF::create(LFStrings::getValue("NiCheng").c_str(),fontStr_kaiti,24.0f);
		niChengTTF->setAnchorPoint(CCPointZero);
		niChengTTF->setPosition(ccp(180,bgSpr->getContentSize().height - 106));
		bgSpr->addChild(niChengTTF);
		niChengTTF->setColor(ccc3(79,110,217));

		CCLabelTTF *nameTTF = CCLabelTTF::create(name.c_str(),fontStr_kaiti,20.0f);
		nameTTF->setAnchorPoint(CCPointZero);
		nameTTF->setPosition(ccp(niChengTTF->getPositionX() + niChengTTF->getContentSize().width,niChengTTF->getPositionY() + 2));
		bgSpr->addChild(nameTTF);
		//角色等级
		CCLabelTTF *jueseLevelTTF = CCLabelTTF::create(LFStrings::getValue("JueseDengji").c_str(),fontStr_kaiti,24.0f);
		jueseLevelTTF->setAnchorPoint(CCPointZero);
		jueseLevelTTF->setPosition(ccp(180,niChengTTF->getPositionY() - side));
		bgSpr->addChild(jueseLevelTTF);
		jueseLevelTTF->setColor(ccc3(79,110,217));

		CCLabelTTF *levelTTF = CCLabelTTF::create(CCString::createWithFormat("%d",dengJi)->getCString(),fontStr_kaiti,20.0f);
		levelTTF->setAnchorPoint(CCPointZero);
		levelTTF->setPosition(ccp(jueseLevelTTF->getPositionX() + jueseLevelTTF->getContentSize().width,jueseLevelTTF->getPositionY() + 2));
		bgSpr->addChild(levelTTF);

		//角色数量
		CCLabelTTF *jueseNumTTF = CCLabelTTF::create(LFStrings::getValue("JueseShuliang").c_str(),fontStr_kaiti,24.0f);
		jueseNumTTF->setAnchorPoint(CCPointZero);
		jueseNumTTF->setPosition(ccp(180,jueseLevelTTF->getPositionY() - side));
		bgSpr->addChild(jueseNumTTF);
		jueseNumTTF->setColor(ccc3(79,110,217));

		CCLabelTTF *numberTTF = CCLabelTTF::create(CCString::createWithFormat("%d",teamNumber)->getCString(),fontStr_kaiti,20.0f);
		numberTTF->setAnchorPoint(CCPointZero);
		numberTTF->setPosition(ccp(jueseNumTTF->getPositionX() + jueseNumTTF->getContentSize().width,jueseNumTTF->getPositionY() + 2));
		bgSpr->addChild(numberTTF);

		//英雄榜排名
		CCLabelTTF *paimingTTF = CCLabelTTF::create(LFStrings::getValue("YingXiongBangPaiMing").c_str(),fontStr_kaiti,24.0f);
		paimingTTF->setAnchorPoint(CCPointZero);
		paimingTTF->setPosition(ccp(180,jueseNumTTF->getPositionY() - side));
		bgSpr->addChild(paimingTTF);
		paimingTTF->setColor(ccc3(79,110,217));

		CCLabelTTF *paimingNumberTTF = CCLabelTTF::create(CCString::createWithFormat("%d",1)->getCString(),fontStr_kaiti,20.0f);
		paimingNumberTTF->setAnchorPoint(CCPointZero);
		paimingNumberTTF->setPosition(ccp(paimingTTF->getPositionX() + paimingTTF->getContentSize().width,paimingTTF->getPositionY() + 2));
		bgSpr->addChild(paimingNumberTTF);
		m_tYingXiongBangLabelTTF = paimingNumberTTF;
	}
	{//VIP按钮
		CCMenuItemImage *vipItem = CCMenuItemImage::create(
			"zhuye_anniu_vip.png"
			, "zhuye_anniu_vip_select.png"
			, this, menu_selector(NetGameSetUI::vipClick));

		{
			CCAnimation *animation = new CCAnimation;
			animation->init();
			char buf[50];
			for (int i=1; i!=8; ++i)
			{
				sprintf(buf, "zhuye_anniu_vip_xiaoguo_%d.png", i);
				animation->addSpriteFrameWithFileName(buf);
			}
			animation->setDelayPerUnit(0.1f);
			//			animation->setRestoreOriginalFrame(true);

			BaseSprite *ef = BaseSprite::create("zhuye_anniu_vip_xiaoguo_1.png");
			ef->setOpacity(0);
			vipItem->addChild(ef, 1);
			ef->setPosition(ccp(vipItem->getContentSize().width/2, vipItem->getContentSize().height/2));
			ef->runAction(CCRepeatForever::create(CCSequence::create(
				CCDelayTime::create(1.0f),
				CCFadeTo::create(0.01f, 255),
				CCAnimate::create(animation),
				CCFadeTo::create(0.01f, 0),
				CCDelayTime::create(1.0f),
				NULL)));
			animation->release();
		}
		vipItem->setPosition(ccp(510,bgSpr->getContentSize().height - 161));
		m_mMenu->addChild(vipItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(vipItem,0));
	}
	{//氏族
		CCMenuItemImage *shizuItem = CCMenuItemImage::create("zhuye_shezhi_anniu_shizu.png","zhuye_shezhi_anniu_shizu_select.png","zhuye_shezhi_anniu_shizu_select.png",this,SEL_MenuHandler(&NetGameSetUI::shizuClick));
		shizuItem->setPosition(ccp(480,bgSpr->getContentSize().height - 310));
		m_mMenu->addChild(shizuItem);
		//m_lMenuItemList.push_back(shizuItem);
		shizuItem->setEnabled(false);

		CCLabelTTF *shizuNameTTF = CCLabelTTF::create(LFStrings::getValue("ZanWuShiZu").c_str(),fontStr_kaiti,20.0f);
		shizuNameTTF->setAnchorPoint(ccp(0,0.5f));
		shizuNameTTF->setPosition(ccp(85,shizuItem->getPositionY()));
		bgSpr->addChild(shizuNameTTF);
		shizuNameTTF->setColor(ccc3(127,127,127));
	}
	{//输入兑换码
		CCMenuItemImage *shuruItem = CCMenuItemImage::create("zhuye_shezhi_anniu_shuru.png","zhuye_shezhi_anniu_shuru_select.png","zhuye_shezhi_anniu_shuru_select.png",this,SEL_MenuHandler(&NetGameSetUI::shuruClick));
		shuruItem->setPosition(ccp(480,bgSpr->getContentSize().height - 405));
		m_mMenu->addChild(shuruItem);
		//m_lMenuItemList.push_back(shizuItem);
		shuruItem->setEnabled(false);
	}
	int side = 15;
	{//背景音乐
		bool isBackgroundSound = GameInforEditer::getGameInfor()->getBackgroundSoundOpen();
		CCSprite *openSpr = CCSprite::create("zhuye_shezhi_anniu_dakai.png");
		openSpr->setPosition(ccp(499,bgSpr->getContentSize().height - 505));
		bgSpr->addChild(openSpr);

		CCSprite *closeSpr = CCSprite::create("zhuye_shezhi_anniu_guanbi.png");
		closeSpr->setPosition(ccp(499,bgSpr->getContentSize().height - 505));
		bgSpr->addChild(closeSpr);

		m_bBackgroundSoundOpenSpr.push_back(openSpr);
		m_bBackgroundSoundOpenSpr.push_back(closeSpr);

		if (isBackgroundSound)
		{
			m_bBackgroundSoundOpenSpr[1]->setVisible(false);
		}
		else
		{
			m_bBackgroundSoundOpenSpr[0]->setVisible(false);
		}
		CCMenuItemImage *openItem = CCMenuItemImage::create("zhuye_shezhi_anniu_yinyue.png","zhuye_shezhi_anniu_yinyue.png","zhuye_shezhi_anniu_yinyue.png",this,SEL_MenuHandler(&NetGameSetUI::backgroundSound));
		openItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(openItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(openItem,0));
		m_iBackgroundItemList.push_back(openItem);
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuye_shezhi_anniu_yinyue_guanbi.png","zhuye_shezhi_anniu_yinyue_guanbi.png","zhuye_shezhi_anniu_yinyue_guanbi.png",this,SEL_MenuHandler(&NetGameSetUI::backgroundSound));
		closeItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(closeItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(closeItem,0));
		m_iBackgroundItemList.push_back(closeItem);
		if (isBackgroundSound)
		{
			setMenuFalseEnable(closeItem);
			m_iBackgroundItemList[1]->setOpacity(0);
			m_iBackgroundItemList[1]->setEnabled(false);
			m_iBackgroundItemList[0]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
			m_iBackgroundItemList[1]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
		}
		else
		{
			setMenuFalseEnable(openItem);
			m_iBackgroundItemList[0]->setOpacity(0);
			m_iBackgroundItemList[0]->setEnabled(false);
			m_iBackgroundItemList[0]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
			m_iBackgroundItemList[1]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
		}
	}
	{//音效
		bool effctiveSound = GameInforEditer::getGameInfor()->getEffctiveSoundOpen();
		CCSprite *openSpr = CCSprite::create("zhuye_shezhi_anniu_dakai.png");
		openSpr->setPosition(ccp(222,bgSpr->getContentSize().height - 505));
		bgSpr->addChild(openSpr);

		CCSprite *closeSpr = CCSprite::create("zhuye_shezhi_anniu_guanbi.png");
		closeSpr->setPosition(ccp(222,bgSpr->getContentSize().height - 505));
		bgSpr->addChild(closeSpr);

		m_bEffctiveSoundOpenSpr.push_back(openSpr);
		m_bEffctiveSoundOpenSpr.push_back(closeSpr);

		if (effctiveSound)
		{
			m_bEffctiveSoundOpenSpr[1]->setVisible(false);
		}
		else
		{
			m_bEffctiveSoundOpenSpr[0]->setVisible(false);
		}

		CCMenuItemImage *openItem = CCMenuItemImage::create("zhuye_shezhi_anniu_yinxiao.png","zhuye_shezhi_anniu_yinxiao.png","zhuye_shezhi_anniu_yinxiao.png",this,SEL_MenuHandler(&NetGameSetUI::effctiveSound));
		openItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(openItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(openItem,0));
		m_iEffctiveSoundItemList.push_back(openItem);
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuye_shezhi_anniu_yinxiao_guanbi.png","zhuye_shezhi_anniu_yinxiao_guanbi.png","zhuye_shezhi_anniu_yinxiao_guanbi.png",this,SEL_MenuHandler(&NetGameSetUI::effctiveSound));
		closeItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(closeItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(closeItem,0));
		m_iEffctiveSoundItemList.push_back(closeItem);
		if (effctiveSound)
		{
			setMenuFalseEnable(closeItem);
			m_iEffctiveSoundItemList[1]->setOpacity(0);
			m_iEffctiveSoundItemList[1]->setEnabled(false);
			m_iEffctiveSoundItemList[0]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
			m_iEffctiveSoundItemList[1]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
		}
		else
		{
			setMenuFalseEnable(openItem);
			m_iEffctiveSoundItemList[0]->setOpacity(0);
			m_iEffctiveSoundItemList[0]->setEnabled(false);
			m_iEffctiveSoundItemList[0]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
			m_iEffctiveSoundItemList[1]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
		}
	}
	{//锁屏
		bool lockScreen = GameInforEditer::getGameInfor()->getLockScreenOpen();
		CCSprite *openSpr = CCSprite::create("zhuye_shezhi_anniu_dakai.png");
		openSpr->setPosition(ccp(222,bgSpr->getContentSize().height - 595));
		bgSpr->addChild(openSpr);

		CCSprite *closeSpr = CCSprite::create("zhuye_shezhi_anniu_guanbi.png");
		closeSpr->setPosition(ccp(222,bgSpr->getContentSize().height - 595));
		bgSpr->addChild(closeSpr);

		m_bLockScreenOpenSpr.push_back(openSpr);
		m_bLockScreenOpenSpr.push_back(closeSpr);

		if (lockScreen)
		{
			m_bLockScreenOpenSpr[1]->setVisible(false);
		}
		else
		{
			m_bLockScreenOpenSpr[0]->setVisible(false);
		}

		CCMenuItemImage *openItem = CCMenuItemImage::create("zhuye_shezhi_anniu_pinsuo.png","zhuye_shezhi_anniu_pinsuo.png","zhuye_shezhi_anniu_pinsuo.png",this,SEL_MenuHandler(&NetGameSetUI::lockScreen));
		openItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(openItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(openItem,0));
		m_iLockScreenItemList.push_back(openItem);
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuye_shezhi_anniu_pinsuo_guanbi.png","zhuye_shezhi_anniu_pinsuo_guanbi.png","zhuye_shezhi_anniu_pinsuo_guanbi.png",this,SEL_MenuHandler(&NetGameSetUI::lockScreen));
		closeItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(closeItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(closeItem,0));
		m_iLockScreenItemList.push_back(closeItem);
		if (lockScreen)
		{
			setMenuFalseEnable(closeItem);
			m_iLockScreenItemList[1]->setOpacity(0);
			m_iLockScreenItemList[1]->setEnabled(false);
			m_iLockScreenItemList[0]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
			m_iLockScreenItemList[1]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
		}
		else
		{
			setMenuFalseEnable(openItem);
			m_iLockScreenItemList[0]->setOpacity(0);
			m_iLockScreenItemList[0]->setEnabled(false);
			m_iLockScreenItemList[0]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
			m_iLockScreenItemList[1]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
		}
	}
	{//消息通知
		bool isSendMessage = GameInforEditer::getGameInfor()->getServerMessageOpen();
		CCSprite *openSpr = CCSprite::create("zhuye_shezhi_anniu_dakai.png");
		openSpr->setPosition(ccp(499,bgSpr->getContentSize().height - 595));
		bgSpr->addChild(openSpr);

		CCSprite *closeSpr = CCSprite::create("zhuye_shezhi_anniu_guanbi.png");
		closeSpr->setPosition(ccp(499,bgSpr->getContentSize().height - 595));
		bgSpr->addChild(closeSpr);

		m_bServerMessageOpenSpr.push_back(openSpr);
		m_bServerMessageOpenSpr.push_back(closeSpr);

		if (isSendMessage)
		{
			m_bServerMessageOpenSpr[1]->setVisible(false);
		}
		else
		{
			m_bServerMessageOpenSpr[0]->setVisible(false);
		}
		CCMenuItemImage *openItem = CCMenuItemImage::create("zhuye_shezhi_anniu_tongzhi.png","zhuye_shezhi_anniu_tongzhi.png","zhuye_shezhi_anniu_tongzhi.png",this,SEL_MenuHandler(&NetGameSetUI::serverMessage));
		openItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(openItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(openItem,0));
		m_iServerMessageItemList.push_back(openItem);
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuye_shezhi_anniu_tongzhi_guanbi.png","zhuye_shezhi_anniu_tongzhi_guanbi.png","zhuye_shezhi_anniu_tongzhi_guanbi.png",this,SEL_MenuHandler(&NetGameSetUI::serverMessage));
		closeItem->setPosition(ccp(openSpr->getPositionX() - openSpr->getContentSize().width / 2 - side,openSpr->getPositionY()));
		m_mMenu->addChild(closeItem);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(closeItem,0));
		m_iServerMessageItemList.push_back(closeItem);
		if (isSendMessage)
		{
			setMenuFalseEnable(closeItem);
			m_iServerMessageItemList[1]->setOpacity(0);
			m_iServerMessageItemList[1]->setEnabled(false);
			m_iServerMessageItemList[0]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
			m_iServerMessageItemList[1]->setPositionX(openSpr->getPositionX() + openSpr->getContentSize().width / 2 - side);
		}
		else
		{
			setMenuFalseEnable(openItem);
			m_iServerMessageItemList[0]->setOpacity(0);
			m_iServerMessageItemList[0]->setEnabled(false);
			m_iServerMessageItemList[0]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
			m_iServerMessageItemList[1]->setPositionX(openSpr->getPositionX() - openSpr->getContentSize().width / 2 + side);
		}
	}
	{//关闭
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuangbei_xiangxi_guanbi.png","zhuangbei_xiangxi_guanbi_select.png","zhuangbei_xiangxi_guanbi_select.png",this,SEL_MenuHandler(&NetGameSetUI::closeThisLayer));
		closeItem->setPosition(ccp(bgSpr->getContentSize().width - closeItem->getContentSize().width / 2,bgSpr->getContentSize().height - closeItem->getContentSize().height / 2));
		m_mMenu->addChild(closeItem);
	}
	{//注销换区
		CCMenuItemImage *closeItem = CCMenuItemImage::create("zhuye_shezhi_anniu_zhuxiaohuanqu.png","zhuye_shezhi_anniu_zhuxiaohuanqu_select.png","zhuye_shezhi_anniu_zhuxiaohuanqu_select.png",this,SEL_MenuHandler(&NetGameSetUI::changeServer));
		closeItem->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - 687));
		m_mMenu->addChild(closeItem);

		//暂时不能点击
		//closeItem->setEnabled(false);
	}
	
	addObserver(SEL_CallFuncO(&NetGameSetUI::changePaiHangBang),MSG_QueryHeroOtherInfoRsp);
	CmdHelper::getHelper()->cmdQueryHeroOtherInfo();
}
void NetGameSetUI::onExit() {
	m_lMenuItemList.clear();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	BaseLayer::onExit();
}

bool NetGameSetUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
// 	CCPoint pTouchPos = pTouch->getLocation();
// 	for (map<CCMenuItem *,int>::iterator iter = m_lMenuItemList.begin() ; iter != m_lMenuItemList.end() ; iter ++)
// 	{
// 		CCPoint pos = m_sBackSpr->convertToWorldSpace(iter->first->getPosition());
// 		CCSize size = iter->first->getContentSize();
// 		if (pTouchPos.x >= pos.x - size.width / 2 && pTouchPos.x <= pos.x + size.width / 2)
// 		{
// 			if (pTouchPos.y >= pos.y - size.height / 2 && pTouchPos.y <= pos.y + size.height / 2)
// 			{
// 				return false;
// 			}
// 		}
// 	}
	return true;
}

void NetGameSetUI::heroClick(CCObject *obj)
{

}

void NetGameSetUI::vipClick(CCObject *obj)
{
	VipExplainLayer *layer = new VipExplainLayer;
	MainLayerBase::getCurrentMainBase()->getHeader()->setContent(BaseSprite::create());
	MainLayerBase::getCurrentMainBase()->setContaintLayer(layer,false);
	layer->release();

	this->removeFromParent();
}

void NetGameSetUI::shizuClick(CCObject *obj)
{

}

void NetGameSetUI::shuruClick(CCObject *obj)
{
	//GameDirector::getDirector()->goToDefaultMainHeroLayer(-1);
}

void NetGameSetUI::backgroundSound(CCObject *obj)
{
	int side = 30;
	int time_in = 0.0f;
	float time_in2 = 0.5f;
	CCMenuItem *item = (CCMenuItem *)obj;
	setMenuFalseEnable(item);
	
	item->setEnabled(false);
	bool isBackgroundSound = GameInforEditer::getGameInfor()->getBackgroundSoundOpen();
	if (isBackgroundSound)
	{
		m_iBackgroundItemList[0]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),CCFadeOut::create(time_in),NULL));
		m_iBackgroundItemList[1]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));
		m_bBackgroundSoundOpenSpr[1]->setVisible(true);
		m_bBackgroundSoundOpenSpr[0]->setVisible(false);
	}
	else
	{
		m_iBackgroundItemList[1]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),CCFadeOut::create(time_in),NULL));
		m_iBackgroundItemList[0]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));

		m_bBackgroundSoundOpenSpr[0]->setVisible(true);
		m_bBackgroundSoundOpenSpr[1]->setVisible(false);
	}
	GameInforEditer::getGameInfor()->setBackgroundSoundOpen(!isBackgroundSound);
}

void NetGameSetUI::effctiveSound(CCObject *obj)
{
	int side = 30;
	int time_in = 0.0f;
	float time_in2 = 0.5f;
	CCMenuItem *item = (CCMenuItem *)obj;
	setMenuFalseEnable(item);
	item->setEnabled(false);
	bool isEffects = GameInforEditer::getGameInfor()->getEffctiveSoundOpen();
	if (isEffects)
	{
		m_iEffctiveSoundItemList[0]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),CCFadeOut::create(time_in),NULL));
		m_iEffctiveSoundItemList[1]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));
		m_bEffctiveSoundOpenSpr[1]->setVisible(true);
		m_bEffctiveSoundOpenSpr[0]->setVisible(false);
	}
	else
	{
		m_iEffctiveSoundItemList[1]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),CCFadeOut::create(time_in),NULL));
		m_iEffctiveSoundItemList[0]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));

		m_bEffctiveSoundOpenSpr[0]->setVisible(true);
		m_bEffctiveSoundOpenSpr[1]->setVisible(false);
	}
	GameInforEditer::getGameInfor()->setEffctiveSoundOpen(!isEffects);
}

void NetGameSetUI::lockScreen(CCObject *obj)
{
	int side = 30;
	int time_in = 0.0f;
	float time_in2 = 0.5f;
	CCMenuItem *item = (CCMenuItem *)obj;
	setMenuFalseEnable(item);
	item->setEnabled(false);
	bool lock = GameInforEditer::getGameInfor()->getLockScreenOpen();
	if (lock)
	{
		m_iLockScreenItemList[0]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),CCFadeOut::create(time_in),NULL));
		m_iLockScreenItemList[1]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));
		m_bLockScreenOpenSpr[1]->setVisible(true);
		m_bLockScreenOpenSpr[0]->setVisible(false);
	}
	else
	{
		m_iLockScreenItemList[1]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),CCFadeOut::create(time_in),NULL));
		m_iLockScreenItemList[0]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));

		m_bLockScreenOpenSpr[0]->setVisible(true);
		m_bLockScreenOpenSpr[1]->setVisible(false);
	}
	GameInforEditer::getGameInfor()->setLockScreenOpen(!lock);
}

void NetGameSetUI::serverMessage(CCObject *obj)
{
	int side = 30;
	int time_in = 0.0f;
	float time_in2 = 0.5f;
	CCMenuItem *item = (CCMenuItem *)obj;
	setMenuFalseEnable(item);
	item->setEnabled(false);
	bool isSend = GameInforEditer::getGameInfor()->getServerMessageOpen();
	if (isSend)
	{
		m_iServerMessageItemList[0]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),CCFadeOut::create(time_in),NULL));
		m_iServerMessageItemList[1]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(0 - m_bBackgroundSoundOpenSpr[0]->getContentSize().width + side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));
		m_bServerMessageOpenSpr[1]->setVisible(true);
		m_bServerMessageOpenSpr[0]->setVisible(false);
	}
	else
	{
		m_iServerMessageItemList[1]->runAction(CCSequence::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),CCFadeOut::create(time_in),NULL));
		m_iServerMessageItemList[0]->runAction(CCSequence::create(CCSpawn::create(CCMoveBy::create(time_in2,ccp(m_bBackgroundSoundOpenSpr[0]->getContentSize().width - side,0)),NULL),CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::callMenuEnable)),CCFadeIn::create(time_in),NULL));

		m_bServerMessageOpenSpr[0]->setVisible(true);
		m_bServerMessageOpenSpr[1]->setVisible(false);
	}
	GameInforEditer::getGameInfor()->setServerMessageOpen(!isSend);
}
void NetGameSetUI::changeServer(CCObject *obj)
{
	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);

	NetBaseTouchLayer *layer = NetBaseTouchLayer::create();
	layer->setContentSize(this->getContentSize());
	//layer->setAnchorPoint(ccp(0.5,0.5));
	layer->setPosition(CCPointZero);
	addChild(layer,10);
	CCLayerColor *colorLayer = CCLayerColor::create(ccc4(0,0,0,125));
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
	layer->addChild(colorLayer,-1);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(layer,-132,true);
	CCSprite *bgSpr = CCSprite::create("shangpin_goumai_tishikuang.png");
	//layer->setContentSize(bgSpr->getContentSize());
	//bgSpr->setScale(0.5f);
	bgSpr->setPosition(ccp(layer->getContentSize().width / 2,layer->getContentSize().height / 2));
	layer->addChild(bgSpr);
	bgSpr->addChild(menu);
	//layer->setTouchPriority(this->getTouchPriority() - 1);
	//menu->setHandlerPriority(layer->getTouchPriority() - 1);
	menu->runAction(CCCallFuncND::create(this, callfuncND_selector(NetGameSetUI::changeMenuPriority_callfuncND), (void*)(-133)));
	CCLabelTTF *ttf = CCLabelTTF::create(LFStrings::getValue("ShiFouZhuXiao").c_str(),fontStr_kaiti,30.0f);
	ttf->setColor(ccRED);
	bgSpr->addChild(ttf);
	ttf->setPosition(ccp(bgSpr->getContentSize().width / 2, bgSpr->getContentSize().height / 2 + 50));

	CCMenuItemImage *headItem = CCMenuItemImage::create("zhuangbei_xiangxi_mianban_queding.png","zhuangbei_xiangxi_mianban_queding_select.png","zhuangbei_xiangxi_mianban_queding_select.png",this,SEL_MenuHandler(&NetGameSetUI::clickChangeServerOK));
	headItem->setPosition(ccp(bgSpr->getContentSize().width / 2 - 30 - headItem->getContentSize().width / 2 , headItem->getContentSize().height / 2 + 30));
	menu->addChild(headItem);

	CCMenuItemImage *headItemCancel = CCMenuItemImage::create("shangpin_goumai_quxiao.png","shangpin_goumai_quxiao_select.png","shangpin_goumai_quxiao_select.png",this,SEL_MenuHandler(&NetGameSetUI::clickChangeServerCancel));
	headItemCancel->setPosition(ccp(bgSpr->getContentSize().width / 2 + 30 + headItemCancel->getContentSize().width / 2 , headItemCancel->getContentSize().height / 2 + 30));
	menu->addChild(headItemCancel);
	m_lTanChuLayer = layer;
}
void NetGameSetUI::clickChangeServerOK(CCObject *obj)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(m_lTanChuLayer);
	mCmdHlp->logOff();
}
void NetGameSetUI::clickChangeServerCancel(CCObject *obj)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(m_lTanChuLayer);
	CCMenuItem *item = (CCMenuItem *)obj;
	//item->getParent()->getParent()->getParent()->runAction(CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::closeLayer)));
	m_lTanChuLayer->runAction(CCCallFuncN::create(this,SEL_CallFuncN(&NetGameSetUI::closeLayer)));
}
void NetGameSetUI::changePaiHangBang(CCObject *obj)
{
	removeObserver(MSG_QueryHeroOtherInfoRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_QueryHeroOtherInfoRsp* data = (SPCmd_QueryHeroOtherInfoRsp *)((Flat_MSG_Rsp*)obj)->getData();
	if (data->err_code)
	{
		return;
	}
	unsigned int number = data->toplist_order;
	if (number <= 10000)
	{
		m_tYingXiongBangLabelTTF->setString(CCString::createWithFormat("%u",number)->getCString());
	}
	else
	{
		m_tYingXiongBangLabelTTF->setString(CCString::createWithFormat("10000%s",LFStrings::getValue("KaiWai").c_str())->getCString());
	}
	
}