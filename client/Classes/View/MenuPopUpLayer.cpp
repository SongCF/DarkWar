#include "MenuPopUpLayer.h"
#include "History.h"
#include "GuideLayerBase.h"
enum 
{
	Tag_ActorFullPhoto_1 = 1,

	Tag_Img_GongJiang_Xin = 10,

	Tag_MessageInfoBar = 101,

	TAG_ITEM_JiuGuan  = 111,
	TAG_ITEM_JiaoTang  ,
	TAG_ITEM_GongJiang  ,

	TAG_ITEM_BeiBao  ,
	TAG_ITEM_ZhuangBei ,
	TAG_ITEM_ZaHuoPu   ,
	TAG_ITEM_YingXiongBang ,

	TAG_ITEM_HeCheng ,
	TAG_ITEM_ChuanQiLu,
	TAG_ITEM_RenWu ,
	TAG_ITEM_TuJian ,
	TAG_ITEM_TanSuo,

	TAG_ITEM_XiTongJiangLi ,
	TAG_ITEM_DengLuJiangLi ,
	TAG_ITEM_TiLiZengSong ,
	TAG_ITEM_VIP_ShuoMing ,
	TAG_ITEM_ZhuYinXiTong , //铸银系统

	Tag_Window_GiftTiLi = 676,
	Tag_Window_ZhuYin,

	TAG_ITEM_Feijie,
	TAG_ITEM_Youjian,
	TAG_ITEM_Shouqi,
	TAG_ITEM_Tanchu,
};
MenuPopUpLayer::MenuPopUpLayer()
	:CCLayer(),
	m_bMenuState(true),
	m_mMenu(NULL)
{
}
MenuPopUpLayer::~MenuPopUpLayer()
{
}
bool MenuPopUpLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	return true;
}
void MenuPopUpLayer::onEnter()
{
	CCLayer::onEnter();
	m_mMenu = CCMenu::create();
	m_mMenu->setPosition(CCPointZero);
	addChild(m_mMenu);

	CCMenuItemImage *moreItem = CCMenuItemImage::create(
		"zhuye_anniu_gengduo.png"
		, "zhuye_anniu_gengduo_select.png"
		, this, menu_selector(MenuPopUpLayer::clickMore));
	m_mMenu->addChild(moreItem, 1,999);
	moreItem->setPosition(ccp(moreItem->getContentSize().width / 2, moreItem->getContentSize().height / 2));
	moreItem->setVisible(false);
	moreItem->setEnabled(false);

	CCMenuItemImage *shouqiItem = CCMenuItemImage::create(
		"zhuye_anniu_shouqi.png"
		,"zhuye_anniu_shouqi_select.png"
		, this, menu_selector(MenuPopUpLayer::clickShouqi));
	m_mMenu->addChild(shouqiItem, 1,0);
	shouqiItem->setPosition(moreItem->getPosition());


	CCMenuItemImage *jiuguanItem = CCMenuItemImage::create(
		"zhuye_anniu_zahuopu.png"
		,"zhuye_anniu_zahuopu_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(jiuguanItem, 0,1);
	jiuguanItem->setPosition(moreItem->getPosition());
	//tujianItem->setVisible(false);
	//tujianItem->setEnabled(false);

	//分解
	CCMenuItemImage *haoyouItem = CCMenuItemImage::create(
		"zhuye_anniu_fenjie.png"
		,"zhuye_anniu_fenjie_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(haoyouItem, 0,2);
	haoyouItem->setPosition(moreItem->getPosition());
  

	CCMenuItemImage *tujianItem = CCMenuItemImage::create(
		"zhuye_anniu_tujian.png"
		,"zhuye_anniu_tujian_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(tujianItem,0,3);
	tujianItem->setPosition(moreItem->getPosition());
	//zhuangbeiItem->setVisible(false);
	//zhuangbeiItem->setEnabled(false);

	


	CCMenuItemImage *beibaoItem = CCMenuItemImage::create(
		"zhuye_anniu_beibao.png"
		,"zhuye_anniu_beibao_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(beibaoItem, 0,4);
	beibaoItem->setPosition(moreItem->getPosition());

	
	

	

	//jiuguanItem->setVisible(false);
	//jiuguanItem->setEnabled(false);

	
	/*
	CCMenuItemImage *haoyouItem = CCMenuItemImage::create(
		"zhuye_anniu_haoyou.png"
		,"zhuye_anniu_haoyou_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(haoyouItem, 0,5);
	haoyouItem->setPosition(moreItem->getPosition());
	*/
	//haoyouItem->setVisible(false);
	//haoyouItem->setEnabled(false);

	CCMenuItemImage *zhuangbeiItem = CCMenuItemImage::create(
		"zhuye_anniu_zhuangbei.png"
		,"zhuye_anniu_zhuangbei_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(zhuangbeiItem, 0,5);
	zhuangbeiItem->setPosition(moreItem->getPosition());

	/*
	CCMenuItemImage *youjianItem = CCMenuItemImage::create(
		"zhuye_anniu_youjian.png"
		,"zhuye_anniu_youjian_select.png"
		, this, menu_selector(MenuPopUpLayer::clickOther));
	m_mMenu->addChild(youjianItem, 0,5);
	youjianItem->setPosition(moreItem->getPosition());*/
	//youjianItem->setVisible(false);
	//youjianItem->setEnabled(false);

	if (m_bIsTanchu)
	{
		for (int i = 1 ; i < 6 ; i ++)
		{
			m_lMenuState[i] = 1;
		}

		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
			item->setPositionY(item->getPositionY() + (3 - i) * 90);
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
			item->setPositionX(item->getPositionX() + (i + 1) * 82);
		}
	}
	else
	{
		moreItem->setVisible(true);
		moreItem->setEnabled(true);
		shouqiItem->setVisible(false);
		shouqiItem->setEnabled(false);
		m_bMenuState = false;
		for (int i = 1 ; i < 6 ; i ++)
		{
			m_lMenuState[i] = 0;
		}

		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
			item->setVisible(false);
			item->setEnabled(false);
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
			item->setVisible(false);
			item->setEnabled(false);
		}
	}

	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FenJieUnLock))
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(2);
		item->setVisible(false);
		item->setEnabled(false);
	}
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanPlay))
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(1);
		item->setVisible(false);
		item->setEnabled(false);
	}
}

void MenuPopUpLayer::onExit()
{
	CCLayer::onExit();
}
void MenuPopUpLayer::clickMore(CCObject *obj)
{
	History::getHistory()->putInCurrentHero(Key_ZhuyeMenuTanchu,true);
	History::getHistory()->save();
	if (!m_bMenuState)
	{
		m_bMenuState = true;
		m_mMenu->setEnabled(false);
		CCMenuItem *moreItem = (CCMenuItem*)obj;
		moreItem->setVisible(false);
		moreItem->setEnabled(false);
		float dt = 0.2f;
		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create(i * dt),
				CCMoveBy::create(dt * (3 - i),ccp(0,(3 - i) * 90)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create((2 - i) * dt),
				CCMoveBy::create(dt * (i + 1),ccp((i + 1) * 82,0)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}
		CCMenuItem *shouqiItem = (CCMenuItem *)m_mMenu->getChildByTag(0);
		shouqiItem->setVisible(true);
		shouqiItem->setEnabled(false);
	}
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FenJieUnLock))
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(2);
		item->setVisible(false);
		item->setEnabled(false);
	}
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanPlay))
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(1);
		item->setVisible(false);
		item->setEnabled(false);
	}
	CCMenuItem *node = CCMenuItem::create();
	node->setTag(TAG_ITEM_Tanchu);
	(m_oTarger->*m_hCallBackNode)(node);

}
void MenuPopUpLayer::clickShouqi(CCObject *obj)
{
	History::getHistory()->putInCurrentHero(Key_ZhuyeMenuTanchu,false);
	History::getHistory()->save();
	if (m_bMenuState)
	{
		m_bMenuState = false;
		m_mMenu->setEnabled(false);
		CCMenuItem *shouqiItem = (CCMenuItem*)obj;
		shouqiItem->setVisible(false);
		//shouqiItem->setEnabled(false);
		float dt = 0.2f;
		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem*)m_mMenu->getChildByTag(i + 1);
			item->setEnabled(false);
			item->runAction(CCSequence::create(//CCDelayTime::create(i * dt),
				CCMoveTo::create(dt * (3 - i),shouqiItem->getPosition()),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::shouqiMoveOverCallBack)),
				NULL));
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem*)m_mMenu->getChildByTag(i + 4);
			item->setEnabled(false);
			item->runAction(CCSequence::create(//CCDelayTime::create((2 - i) * dt),
				CCMoveTo::create(dt * (i + 1),shouqiItem->getPosition()),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::shouqiMoveOverCallBack)),
				NULL));
		}
		CCMenuItem *moreItem = (CCMenuItem *)m_mMenu->getChildByTag(999);
		moreItem->setVisible(true);
		//moreItem->setEnabled(false);
	}

	CCMenuItem *node = CCMenuItem::create();
	node->setTag(TAG_ITEM_Shouqi);
	(m_oTarger->*m_hCallBackNode)(node);
}
void MenuPopUpLayer::clickOther(CCObject *obj)
{
	CCMenuItem *item = (CCMenuItem *)obj;

	CCMenuItem *node = CCMenuItem::create();
	switch (item->getTag())
	{
	case 1:
		{
			
			node->setTag(TAG_ITEM_JiuGuan);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
	case 2:
		{
			node->setTag(TAG_ITEM_Feijie);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
	case 3:
		{
			
			node->setTag(TAG_ITEM_TuJian);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
	case 4:
		{
			
			node->setTag(TAG_ITEM_BeiBao);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
		/*
	case 5:
		{
			node->setTag(TAG_ITEM_Haoyou);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
		*/
	case 5:
		{
			node->setTag(TAG_ITEM_ZhuangBei);
			(m_oTarger->*m_hCallBackNode)(node);
		}
		break;
	default:
		break;
	}
}
void MenuPopUpLayer::moveOverCallBack(CCNode *node)
{
	CCMenuItem *item = (CCMenuItem*)node;
	item->setEnabled(true);

	
	m_lMenuState[node->getTag()] = 1;

	for (int i = 1 ; i < 6 ; i ++)
	{
		if (m_lMenuState[i] == 0)
		{
			return;
		}
	}
	CCMenuItem *shouqiItem = (CCMenuItem *)m_mMenu->getChildByTag(0);
	//shouqiItem->setVisible(true);
	shouqiItem->setEnabled(true);
	m_mMenu->setEnabled(true);
}
void MenuPopUpLayer::shouqiMoveOverCallBack(CCNode *node)
{
	CCMenuItem *item = (CCMenuItem*)node;
	item->setVisible(false);

	m_lMenuState[node->getTag()] = 0;

	for (int i = 1 ; i < 6 ; i ++)
	{
		if (m_lMenuState[i] == 1)
		{
			return;
		}
	}

	CCMenuItem *moreItem = (CCMenuItem *)m_mMenu->getChildByTag(999);
	//moreItem->setVisible(true);
	moreItem->setEnabled(true);
	m_mMenu->setEnabled(true);
}
void MenuPopUpLayer::UnLockFenJie()
{
	CCMenuItem *moreItem = (CCMenuItem *)m_mMenu->getChildByTag(999);
	if (!m_bMenuState)
	{
		History::getHistory()->putInCurrentHero(Key_ZhuyeMenuTanchu,true);
		History::getHistory()->save();
	
		m_bMenuState = true;
		m_mMenu->setEnabled(false);
		
		moreItem->setVisible(false);
		moreItem->setEnabled(false);
		float dt = 0.2f;
		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create(i * dt),
				CCMoveBy::create(dt * (3 - i),ccp(0,(3 - i) * 90)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create((2 - i) * dt),
				CCMoveBy::create(dt * (i + 1),ccp((i + 1) * 82,0)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}
		CCMenuItem *shouqiItem = (CCMenuItem *)m_mMenu->getChildByTag(0);
		shouqiItem->setVisible(true);
		shouqiItem->setEnabled(false);
	}
	
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(1);
		item->setVisible(false);
		item->setEnabled(false);
	}
	CCMenuItem *node = CCMenuItem::create();
	node->setTag(TAG_ITEM_Tanchu);
	(m_oTarger->*m_hCallBackNode)(node);
	CCMenuItem *fenjieItem = (CCMenuItem *)m_mMenu->getChildByTag(2);
	fenjieItem->setVisible(true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	fenjieItem->setPosition(ccp(size.width / 2, size.height / 2 - 100));
	fenjieItem->runAction(CCSequence::create(CCScaleTo::create(0.5f,1.2f),CCScaleTo::create(0.5f,1.0f),CCScaleTo::create(0.5f,1.2f),CCMoveTo::create(1.0f,ccp(moreItem->getPositionX(),moreItem->getPositionY() + 180)),CCScaleTo::create(0.5f,1.0f),CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::playOver)),NULL));
}
void MenuPopUpLayer::UnLockJiuGuan()
{
	CCMenuItem *moreItem = (CCMenuItem *)m_mMenu->getChildByTag(999);
	if (!m_bMenuState)
	{
		History::getHistory()->putInCurrentHero(Key_ZhuyeMenuTanchu,true);
		History::getHistory()->save();

		m_bMenuState = true;
		m_mMenu->setEnabled(false);

		moreItem->setVisible(false);
		moreItem->setEnabled(false);
		float dt = 0.2f;
		for (int i = 0 ; i < 3 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create(i * dt),
				CCMoveBy::create(dt * (3 - i),ccp(0,(3 - i) * 90)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}

		for (int i = 0 ; i < 2 ; i ++)
		{
			CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
			item->setVisible(true);
			item->setEnabled(false);
			item->runAction(CCSequence::create(CCDelayTime::create((2 - i) * dt),
				CCMoveBy::create(dt * (i + 1),ccp((i + 1) * 82,0)),
				CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::moveOverCallBack)),
				NULL));
		}
		CCMenuItem *shouqiItem = (CCMenuItem *)m_mMenu->getChildByTag(0);
		shouqiItem->setVisible(true);
		shouqiItem->setEnabled(false);
	}

	CCMenuItem *node = CCMenuItem::create();
	node->setTag(TAG_ITEM_Tanchu);
	(m_oTarger->*m_hCallBackNode)(node);
	CCMenuItem *jiuGuanItem = (CCMenuItem *)m_mMenu->getChildByTag(1);
	jiuGuanItem->setVisible(true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	jiuGuanItem->setPosition(ccp(size.width / 2, size.height / 2 - 100));
	jiuGuanItem->runAction(CCSequence::create(CCScaleTo::create(0.5f,1.2f),CCScaleTo::create(0.5f,1.0f),CCScaleTo::create(0.5f,1.2f),CCMoveTo::create(1.0f,ccp(moreItem->getPositionX(),moreItem->getPositionY() + 270)),CCScaleTo::create(0.5f,1.0f),CCCallFuncN::create(this,SEL_CallFuncN(&MenuPopUpLayer::playOver)),NULL));
}