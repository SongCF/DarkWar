#include "NetSystemRewardUI.h"
#include "ImageManager.h"
#include "font.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "ClientErrorCode.h"
#include "MessageDisplay.h"
#include "AwardDisplay.h"
#include "FX_CommonFunc.h"
#include "MyselfManager.h"
#include "SystemGiftBagManager.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "MapsLayer.h"
#include "TaskManager.h"
#include "TanSuoLayer.h"
#include "VarietyStoreLayer.h"
#include "GongJiangQianghua.h"
#include "HeroRankList.h"
#include "UnblockManager.h"
#include "SystemGiftLayer.h"
#include "XueSeJiaoTangLayer.h"
#include "GiftSysDisplay.h"
#include "SuitPool.h"
#include "GuideLayerBase.h"
using namespace FX_CommonFunc;

enum 
{
	Tag_Board,
	Tag_Menu_LingJiang,
	Tag_MenuItem_Fanhui,
	Tag_Menu_Equip,

	Tag_Board_EquipDetail,
	Tag_Menu_EquipDetail,
	Tag_MenuItem_Close,
	Tag_MenuItem_TaoZhuang,

	Tag_Board_TaoZhuangShuxing,

};
NetSystemRewardUI::NetSystemRewardUI()
	:BaseLayer()
	,m_nYiJingOverRenWu(0)
{
}
NetSystemRewardUI::~NetSystemRewardUI()
{
}
CCScene* NetSystemRewardUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	NetSystemRewardUI *layer = NetSystemRewardUI::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool NetSystemRewardUI::init()
{
	if (!BaseLayer::init())
	{
		return false;
	}
	return true;
}
void NetSystemRewardUI::onEnter()
{
	BaseLayer::onEnter();
	mGiftsLoginVec = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
	mGiftsLevelVec = BonusManager::getManager()->getBonus(BonusType_Level);
	mGiftsSysVec = BonusManager::getManager()->getBonus(BonusType_DayWelfare);
	mGiftsChooseVec = BonusManager::getManager()->getBonus(BonusType_Reward);
	//mGiftsVec.insert(list.begin(),list.end());
	{
		sort(mGiftsLoginVec.begin(), mGiftsLoginVec.end(), compareByBonusId);
		sort(mGiftsLevelVec.begin(), mGiftsLevelVec.end(), compareByBonusId);
		//sort(mGiftsSysVec.begin(), mGiftsSysVec.end(), compareByBonusId);
		sort(mGiftsChooseVec.begin(), mGiftsChooseVec.end(), compareByBonusId);
	}
	sortRiChangList();

	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu,11);

	CCSprite *headSpr = CCSprite::create("denglujiangli_up_bg.png");
	headSpr->setPosition(ccp(getBodySize().width / 2,getBodySize().height - headSpr->getContentSize().height / 2));
	addChild(headSpr,2);

	CCSprite *titleSpr = CCSprite::create("denglujiangli_wenzi_xitongjiangli.png");
	titleSpr->setPosition(ccp(headSpr->getContentSize().width / 2, headSpr->getContentSize().height / 2));
	headSpr->addChild(titleSpr);

	int side = 5;
	m_sBodySize = getBodySize();

	//底
	BaseSprite *di = BaseSprite::create("renwuliebiao_down_bg.png");
	this->addChild(di, 2);
	di->setPosition(ccp(getBodySize().width/2, di->getContentSize().height/2));
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(NetSystemRewardUI::clickCancel));
	menu->addChild(fanhui);
	fanhui->setPosition(ccp(di->getContentSize().width/2, di->getContentSize().height/2));
	/*CCPoint pos = addDengluJiangli(ccp(m_sBodySize.width/2, headSpr->getPositionY() - headSpr->getContentSize().height / 2));

	CCPoint pos2 = addDengjiJiangli(pos);

	CCPoint pos3 = addRiChangJiangli(pos2);
	CCPoint pos4 = addXuanShangJiangli(pos3);*/

	CCLayer *layer = CCLayer::create();
	layer->setContentSize(CCSizeMake(615, 224 * 4 * 1046.0 / (m_sBodySize.height)));
	layer->setAnchorPoint(CCPointZero);

	CCScrollView *m_cScroll = CCScrollView::create(CCSizeMake(615,860),layer);
	m_cScroll->setDirection(kCCScrollViewDirectionVertical);
	//CCPoint pos = m_cScroll->getContentOffset();
	m_cScroll->setContentOffset(ccp(0,-(224 * 4 * 1046.0 / (m_sBodySize.height) - 860)));
	m_cScroll->setAnchorPoint(CCPointZero);
	m_cScroll->setPosition(ccp(12.5,di->getContentSize().height));
	//m_cAuctionList->setMenuHandler(this,menu_selector(CardAuctionUI::onRandSelectCheck),menu_selector(CardAuctionUI::onRandSelectBuy));
	addChild(m_cScroll);

	if (mGiftsLoginVec.at(0).bonus_id != 1)
	{
		CCPoint pos = addDengluJiangli(layer,ccp(615.0f / 2.0f,224 * 4));

		CCPoint pos2 = addDengjiJiangli(layer,pos);

		CCPoint pos3 = addRiChangJiangli(layer,pos2);
		CCPoint pos4 = addXuanShangJiangli(layer,pos3);

		//m_lInfoPos.push_back(ccp(m_sBodySize.width/2, headSpr->getPositionY() - headSpr->getContentSize().height / 2));
		m_lInfoPos.push_back(ccp(615.0f / 2.0f,224 * 4));
		m_lInfoPos.push_back(pos);
		m_lInfoPos.push_back(pos2);
		m_lInfoPos.push_back(pos3);
	}
	else
	{
		CCPoint pos = ccp(615.0f / 2.0f,224 * 4);

		CCPoint pos2 = addDengjiJiangli(layer,pos);

		CCPoint pos3 = addRiChangJiangli(layer,pos2);
		CCPoint pos4 = addXuanShangJiangli(layer,pos3);

		m_lInfoPos.push_back(CCPointZero);
		m_lInfoPos.push_back(pos);
		m_lInfoPos.push_back(pos2);
		m_lInfoPos.push_back(pos3);
	}
	m_lScrollLayer = layer;
	
}
void NetSystemRewardUI::menuItemClicked_GetGift(CCObject *obj)
{
	int index = ((CCMenuItem*)obj)->getTag();
	m_nSelectJiangliIndex = index;
	addObserver(callfuncO_selector(NetSystemRewardUI::rsp_getGift), MSG_getBonusRsp);
	unsigned int bonusId = (unsigned int) ((CCMenuItemImage*)obj)->getUserData();
	BonusType type = BonusType_DailyLogin;
	int id = 0;
	switch (index)
	{
	case 1:
		{
			type = BonusType_DailyLogin;
			id = mGiftsLoginVec.at(0).bonus_id;
		}
		break;
	case 2:
		{
			type = BonusType_Level;
			id = mGiftsLevelVec.at(m_nSelectLevelIndex).bonus_id;
		}
		break;
	case 3:
		{
			type = BonusType_DayWelfare;
			id = mGiftsSysVec.at(0).bonus_id;
		}
		break;
	case 4:
		{
			type = BonusType_Reward;
			id = mGiftsChooseVec.at(0).bonus_id;
		}
		break;
	default:
		break;
	}
	mCmdHlp->cmdGetBonus(type,id);
}
void NetSystemRewardUI::rsp_getGift(CCObject *msg_f)
{
	removeObserver(MSG_getBonusRsp);
	GameDirector::getDirector()->hideWaiting();

	Flat_GetBonusRsp* data = (Flat_GetBonusRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code)
	{
		if (data->err_code == CEC_NoMoreFreeSpaceToStoreEquip)
		{
			EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
			layer->showGoToFenJieDialog();
		}
		else
		{
			MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
		return;
	}


	//弹出奖励的框
	{
		CCString infoStr = LFStrings::getValue("HuoDe") + LFStrings::getValue("jiangli");

		switch (m_nSelectJiangliIndex)
		{
		case 1:
			{
				GiftDisplay *layer = GiftDisplay::create(infoStr,mGiftsLoginVec.at(0));
				//AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsLoginVec.at(0).itemVec, mGiftsLoginVec.at(0).equipVec);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

				m_lYiLingQuSprList.at(0)->setVisible(true);
				m_lLingQuItemList.at(0)->setVisible(false);
				m_lLingQuItemList.at(0)->setEnabled(false);
			}
			break;
		case 2:
			{
				GiftDisplay *layer = GiftDisplay::create(infoStr,mGiftsLevelVec.at(0));
				//AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsLevelVec.at(0).itemVec, mGiftsLevelVec.at(0).equipVec);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				mGiftsLevelVec = BonusManager::getManager()->getBonus(BonusType_Level);

				map<short,CCSprite *>::iterator iter = m_lInfoBgSprList.find(3);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(3);
				}
				iter = m_lInfoBgSprList.find(4);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(4);
				}
				addDengjiJiangli(m_lScrollLayer,m_lInfoPos.at(1));
			}
			break;
		case 3:
			{
				GiftDisplay *layer = GiftDisplay::create(infoStr,mGiftsSysVec.at(0));
				//AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsSysVec.at(0).itemVec, mGiftsSysVec.at(0).equipVec);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

				mGiftsSysVec = BonusManager::getManager()->getBonus(BonusType_DayWelfare);

				map<short,CCSprite *>::iterator iter = m_lInfoBgSprList.find(5);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(5);
				}
				iter = m_lInfoBgSprList.find(6);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(6);
				}
				sortRiChangList();
				addRiChangJiangli(m_lScrollLayer,m_lInfoPos.at(2));
			}
			break;
		case 4:
			{
				GiftDisplay *layer = GiftDisplay::create(infoStr,mGiftsChooseVec.at(0));
				//AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsChooseVec.at(0).itemVec, mGiftsChooseVec.at(0).equipVec);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

				map<short,CCSprite *>::iterator iter = m_lInfoBgSprList.find(7);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(7);
				}
				iter = m_lInfoBgSprList.find(8);
				if (iter != m_lInfoBgSprList.end())
				{
					iter->second->removeFromParent();
					m_lInfoBgSprList.erase(8);
				}
				mGiftsChooseVec = BonusManager::getManager()->getBonus(BonusType_Reward);
				addXuanShangJiangli(m_lScrollLayer,m_lInfoPos.at(3));
			}
			break;
		default:
			break;
		}
	}
	
	//mGiftsVec = BonusManager::getManager()->getBonus(BT_DailyLogin);
	////排序
	//{
	//	sort(mGiftsVec.begin(), mGiftsVec.end(), compareByBonusId);
	//}
	//mTableView->reloadData();

}
void NetSystemRewardUI::resetDengLuJiangLi()
{
	m_lYiLingQuSprList.at(0)->setVisible(true);
	m_lLingQuItemList.at(0)->setVisible(false);
	m_lLingQuItemList.at(0)->setEnabled(false);
}
void NetSystemRewardUI::resetDengJiJiangLi()
{
	mGiftsLevelVec = BonusManager::getManager()->getBonus(BonusType_Level);

	map<short,CCSprite *>::iterator iter = m_lInfoBgSprList.find(3);
	if (iter != m_lInfoBgSprList.end())
	{
		iter->second->removeFromParent();
		m_lInfoBgSprList.erase(3);
	}
	iter = m_lInfoBgSprList.find(4);
	if (iter != m_lInfoBgSprList.end())
	{
		iter->second->removeFromParent();
		m_lInfoBgSprList.erase(4);
	}
	addDengjiJiangli(m_lScrollLayer,m_lInfoPos.at(1));
}
void NetSystemRewardUI::menuItemClicked_Equip(CCObject *obj)
{
	mCurTaoZhuang_Info = *(Flat_DuanzaoGroupZBInfo*)((CCMenuItemImage*)obj)->getUserData();

	this->disableAllTouchBegin();

	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	this->addChild(board, 10, Tag_Board_EquipDetail);
	refreshEquipBoard(board, mCurTaoZhuang_Info);
	board->setPosition(ccp(mWinSize.width/2, board->getContentSize().height/2));


	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
	//	menu->setTouchPriority(-131);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, 
		menu_selector(NetSystemRewardUI::menuItemClicked_equipDetailMenu));
	menu->addChild(item, 0, Tag_MenuItem_Close);
	item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
	//底纹
	ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		mCurTaoZhuang_Info.zhuangbei_colour);
	BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
	diWen->setScale(0.7f);
	board->addChild(diWen);
	diWen->setPosition(ccp(123, 838-240+122));
	// 装备图片
	string equipfile = mCurTaoZhuang_Info.zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *s = BaseSprite::create(equipfile);
	diWen->addChild(s);
	s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	//套装按钮
	if(SuitPool::getPool()->isSuit(mCurTaoZhuang_Info.zb_group_id))
	{
		string norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang.png";
		string selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png";
		// 		if (ChuanqiPool::getPool()->isShenBing(mEquipVec.at(idx).zb_group_id))
		// 		{
		// 			norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png";
		// 			selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png";
		// 		}  
		CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
			norStr.c_str()
			, selStr.c_str()
			, this ,menu_selector(NetSystemRewardUI::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		//taozhuangItem->setEnabled(false);
	}
}
CCPoint NetSystemRewardUI::addDengluJiangli(CCLayer *layer,CCPoint pos)
{
	CCPoint point = CCPointZero;
	CCSprite *jinriBoard = CCSprite::create("denglujiangli_jinri_bg.png");
	m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(1,jinriBoard));
	if (mGiftsLoginVec.at(0).is_get || mGiftsLoginVec.at(0).enable)  //已领取过的、 和今日的
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		jinriBoard->addChild(boardMenu,1);
		layer->addChild(jinriBoard, 1);
		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));

		CCSprite *yiLingquSpr = CCSprite::create("denglujiangli_anniu_yilingqu.png");
		jinriBoard->addChild(yiLingquSpr, 0);
		yiLingquSpr->setPosition(ccp(550, 189-103));
		CCMenuItemImage *lingQuItem = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_GetGift));
		boardMenu->addChild(lingQuItem, 0);
		lingQuItem->setPosition(ccp(550, 189-103));
		lingQuItem->setUserData((void*)mGiftsLoginVec.at(0).bonus_id);
		lingQuItem->setTag(1);

		if (mGiftsLoginVec.at(0).enable && ! mGiftsLoginVec.at(0).is_get)
		{
			
			yiLingquSpr->setVisible(false);
			
		}
		else
		{
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		m_lLingQuItemList.push_back(lingQuItem);
		m_lYiLingQuSprList.push_back(yiLingquSpr);

		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_langtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);

		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_meiridenglu.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		CCMenuItemImage *infoitem = CCMenuItemImage::create(
			"denglujiangli_anniu_xiangqing.png",
			"denglujiangli_anniu_xiangqing_select.png",
			this, menu_selector(NetSystemRewardUI::clickInfo));
		boardMenu->addChild(infoitem, 0);
		infoitem->setPosition(ccp(titleBarSpr->getContentSize().width - infoitem->getContentSize().width / 2, titleBarSpr->getPositionY()));
		infoitem->setTag(1);
		//

		if (mGiftsLoginVec.at(0).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsLoginVec.at(0).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}

		int itemNum = mGiftsLoginVec.at(0).itemVec.size();
		sort(mGiftsLoginVec.at(0).itemVec.begin(), mGiftsLoginVec.at(0).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(0).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsLoginVec.at(0).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsLoginVec.at(0).itemVec.at(i).type).c_str(), mGiftsLoginVec.at(0).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84+i*130, 189-103));
		}

		int itemNum2 = mGiftsLoginVec.at(0).itemDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).itemDebrisVec.begin(), mGiftsLoginVec.at(0).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(0).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(0).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(0).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLoginVec.at(0).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(0).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsLoginVec.at(0).equipDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).equipDebrisVec.begin(), mGiftsLoginVec.at(0).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(0).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(0).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(0).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLoginVec.at(0).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(0).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130, 189-103));
		}
		int itemNum4 = mGiftsLoginVec.at(0).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(0).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsLoginVec.at(0).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsLoginVec.at(0).giftBagVec.at(i).id).c_str(),mGiftsLoginVec.at(0).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 189-103));
		}
		int itemNum5 = 0;
		if (mGiftsLoginVec.at(0).ran_zb_count > 0)
		{
			itemNum5 = 1;
		}
		
		
		for (int i=0; i!=itemNum5; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getGiftBagBoardName(3));

			BaseSprite *itemImg = BaseSprite::create("daoju_libao_3.png");
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),mGiftsLoginVec.at(0).ran_zb_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 + itemNum4) *130, 189-103));
		}
		//装备

		for (int i=0, count=mGiftsLoginVec.at(0).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsLoginVec.at(0).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(NetSystemRewardUI::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsLoginVec.at(0).equipVec.at(i));
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4 + itemNum5)*130, 189-112));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}
		bool isMingtianChuanqi = false;
		for (int i=0, count=mGiftsLoginVec.at(1).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsLoginVec.at(1).equipVec.at(i);
			if (oneEq.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
			{
				//titleSpr
				isMingtianChuanqi = true;
				break;
			}
		}
		if(isMingtianChuanqi)
		{
			CCLabelTTF *nameLabel = CCLabelTTF::create(LFStrings::getValue("MingriJiangLiChuanqi").c_str(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width - infoitem->getContentSize().width * 2 - nameLabel->getContentSize().width / 2,titleSpr->getPositionY()));
			//nameLabel->setColor(ccc3(181,93,6));
			nameLabel->setColor(ccWHITE);
			nameLabel->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0.8f),CCFadeIn::create(0.8f),NULL)));
		}
	}
	if (mGiftsLoginVec.size() > 1)
	{
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr);
		m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(2,titleBarSpr));
		int side = 30;
		CCLabelTTF *mtTitleTTF = CCLabelTTF::create(LFStrings::getValue("MingriJiangLi").c_str(),fontStr_kaiti,20.0f);
		mtTitleTTF->setPosition(ccp(mtTitleTTF->getContentSize().width / 2,titleBarSpr->getContentSize().height / 2));

		point = titleBarSpr->getPosition();
		point.y -= titleBarSpr->getContentSize().height / 2;

		mtTitleTTF->setColor(ccc3(195,145,95));
		titleBarSpr->addChild(mtTitleTTF);
		CCString temp;
		int itemNum = mGiftsLoginVec.at(1).itemVec.size();
		//sort(mGiftsLoginVec.at(1).itemVec.begin(), mGiftsLoginVec.at(1).itemVec.end(), compareByItemId);

		CCPoint pos = ccp(mtTitleTTF->getPositionX() + mtTitleTTF->getContentSize().width / 2 + side, mtTitleTTF->getPositionY());
		for (int i=0; i!=itemNum; ++i)
		{
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsLoginVec.at(1).itemVec.at(i).type).c_str(), mGiftsLoginVec.at(1).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(pos.x + nameLabel->getContentSize().width / 2,pos.y));
			nameLabel->setColor(ccWHITE);
			pos.x = nameLabel->getPositionX() + nameLabel->getContentSize().width / 2 + side;
		}

		int itemNum2 = mGiftsLoginVec.at(1).itemDebrisVec.size();
		//sort(mGiftsLoginVec.at(1).itemDebrisVec.begin(), mGiftsLoginVec.at(1).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(1).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(1).itemDebrisVec.at(i).suipian_id);
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(1).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(pos.x + nameLabel->getContentSize().width / 2,pos.y));
			nameLabel->setColor(ccWHITE);
			pos.x = nameLabel->getPositionX() + nameLabel->getContentSize().width / 2 + side;
		}

		int itemNum3 = mGiftsLoginVec.at(1).equipDebrisVec.size();
		//sort(mGiftsLoginVec.at(1).equipDebrisVec.begin(), mGiftsLoginVec.at(1).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(1).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(1).equipDebrisVec.at(i).suipian_id);
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(1).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(pos.x + nameLabel->getContentSize().width / 2,pos.y));
			nameLabel->setColor(ccWHITE);
			pos.x = nameLabel->getPositionX() + nameLabel->getContentSize().width / 2 + side;
		}
		int itemNum4 = mGiftsLoginVec.at(1).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsLoginVec.at(1).giftBagVec.at(i).id).c_str(),mGiftsLoginVec.at(1).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(pos.x + nameLabel->getContentSize().width / 2,pos.y));
			nameLabel->setColor(ccWHITE);
			pos.x = nameLabel->getPositionX() + nameLabel->getContentSize().width / 2 + side;
		}
		int itemNum5 = 0;
		if (mGiftsLoginVec.at(1).ran_zb_count > 0)
		{
			itemNum5 = 1;
		}
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum5; ++i)
		{
			CCString temp;
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),mGiftsLoginVec.at(1).ran_zb_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			titleBarSpr->addChild(nameLabel);
			nameLabel->setPosition(ccp(pos.x + nameLabel->getContentSize().width / 2,pos.y));
			nameLabel->setColor(ccWHITE);
			pos.x = nameLabel->getPositionX() + nameLabel->getContentSize().width / 2 + side;
		}
		//装备

		for (int i=0, count=mGiftsLoginVec.at(1).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsLoginVec.at(1).equipVec.at(i);

			CCLabelTTF *nameTTF = CCLabelTTF::create(CCString::createWithFormat("%sx1",oneEq.zhuangbei_name.c_str())->getCString(),fontStr_kaiti,20.0f);
			nameTTF->setPosition(ccp(pos.x + nameTTF->getContentSize().width / 2,pos.y));
			titleBarSpr->addChild(nameTTF);
			if (oneEq.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
			{
				nameTTF->setColor(ccc3(181,93,6));
			}
			else
			{
				nameTTF->setColor(ccc3(253,180,40));
			}
		}
	}
	return point;
}
CCPoint NetSystemRewardUI::addDengjiJiangli(CCLayer *layer,CCPoint pos)
{
	m_nSelectLevelIndex = 0;
	CCPoint point = CCPointZero;
	CCSprite *jinriBoard = CCSprite::create("denglujiangli_jinri_bg.png");
	m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(3,jinriBoard));
	if (mGiftsLevelVec.size() <= 0)
	{
		return pos;
	}
	int index = 0;
	for (vector<Flat_BonusInfo>::iterator iter = mGiftsLevelVec.begin() ; iter != mGiftsLevelVec.end() ; iter ++)
	{
		if (iter->is_get == false)
		{
			m_nSelectLevelIndex = index;
			break;
		}
		index++;
	}

	if (index >= mGiftsLevelVec.size())
	{
		index = mGiftsLevelVec.size() - 1;
	}
	//if (mGiftsLevelVec.at(0).is_get || mGiftsLevelVec.at(0).enable)  //已领取过的、 和今日的
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		jinriBoard->addChild(boardMenu,1);
		layer->addChild(jinriBoard, 1);
		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));

		CCSprite *yiLingquSpr = CCSprite::create("a2.png");
		jinriBoard->addChild(yiLingquSpr, 0);
		yiLingquSpr->setPosition(ccp(550, 189-103));
		CCMenuItemImage *lingQuItem = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_GetGift));
		boardMenu->addChild(lingQuItem, 0);
		lingQuItem->setPosition(ccp(550, 189-103));
		lingQuItem->setUserData((void*)mGiftsLevelVec.at(index).bonus_id);
		lingQuItem->setTag(2);

		if (mGiftsLevelVec.at(index).enable && ! mGiftsLevelVec.at(index).is_get)
		{

			yiLingquSpr->setVisible(false);

		}
		else
		{
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		m_lLingQuItemList.push_back(lingQuItem);
		m_lYiLingQuSprList.push_back(yiLingquSpr);

		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);

		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_dengjijiangli.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		CCMenuItemImage *infoitem = CCMenuItemImage::create(
			"denglujiangli_anniu_xiangqing.png",
			"denglujiangli_anniu_xiangqing_select.png",
			this, menu_selector(NetSystemRewardUI::clickInfo));
		boardMenu->addChild(infoitem, 0);
		infoitem->setPosition(ccp(titleBarSpr->getContentSize().width - infoitem->getContentSize().width / 2, titleBarSpr->getPositionY()));
		infoitem->setTag(2);
		//

		if (mGiftsLevelVec.at(index).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsLevelVec.at(index).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}

		int itemNum = mGiftsLevelVec.at(index).itemVec.size();
		sort(mGiftsLevelVec.at(index).itemVec.begin(), mGiftsLevelVec.at(index).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLevelVec.at(index).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsLevelVec.at(index).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsLevelVec.at(index).itemVec.at(i).type).c_str(), mGiftsLevelVec.at(index).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84+i*130, 189-103));
		}

		int itemNum2 = mGiftsLevelVec.at(index).itemDebrisVec.size();
		//sort(mGiftsLevelVec.at(index).itemDebrisVec.begin(), mGiftsLevelVec.at(index).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLevelVec.at(index).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsLevelVec.at(index).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLevelVec.at(index).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLevelVec.at(index).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLevelVec.at(index).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsLevelVec.at(index).equipDebrisVec.size();
		//sort(mGiftsLevelVec.at(index).equipDebrisVec.begin(), mGiftsLevelVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLevelVec.at(index).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsLevelVec.at(index).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLevelVec.at(index).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLevelVec.at(index).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLevelVec.at(index).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130, 189-103));
		}
		int itemNum4 = mGiftsLevelVec.at(index).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLevelVec.at(index).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsLevelVec.at(index).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsLevelVec.at(index).giftBagVec.at(i).id).c_str(),mGiftsLevelVec.at(index).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 189-103));
		}
		//装备

		for (int i=0, count=mGiftsLevelVec.at(index).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsLevelVec.at(index).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(NetSystemRewardUI::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsLevelVec.at(index).equipVec.at(i));
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4)*130, 189-112));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}

		CCLabelTTF *nameLabel = CCLabelTTF::create(CCString::createWithFormat("%s%s%d%s",LFStrings::getValue("LingQuDengJi").c_str(),LFStrings::getValue("dakuohaoLeft").c_str(),mGiftsLevelVec.at(index).bonus_id,LFStrings::getValue("dakuohaoRight").c_str())->getCString(), fontStr_kaiti, 20);
		titleBarSpr->addChild(nameLabel);
		nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width / 2,titleSpr->getPositionY()));
		nameLabel->setColor(ccWHITE);
		
	}
	//if (mGiftsLevelVec.size() > 1)
	{
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr);
		m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(4,titleBarSpr));
		point = titleBarSpr->getPosition();
		point.y -= titleBarSpr->getContentSize().height / 2;

		int level = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
		if (mGiftsLevelVec.at(index).bonus_id > level)
		{
			CCLabelTTF *mtTitleTTF = CCLabelTTF::create(LFStrings::getValue("HaiCheNDengJi").c_str(),fontStr_kaiti,20.0f);
			mtTitleTTF->setPosition(ccp(mtTitleTTF->getContentSize().width / 2,titleBarSpr->getContentSize().height / 2));
			mtTitleTTF->setColor(ccc3(206,26,0));
			titleBarSpr->addChild(mtTitleTTF);

			CCLabelTTF *numTitleTTF = CCLabelTTF::create(CCString::createWithFormat("%d",mGiftsLevelVec.at(index).bonus_id - level)->getCString(),fontStr_kaiti,25.0f);
			numTitleTTF->setPosition(ccp(mtTitleTTF->getPositionX() - mtTitleTTF->getContentSize().width / 2 + 50,titleBarSpr->getContentSize().height / 2));
			numTitleTTF->setColor(ccc3(181,93,6));
			titleBarSpr->addChild(numTitleTTF);
		}
		else
		{
			CCLabelTTF *mtTitleTTF = CCLabelTTF::create(LFStrings::getValue("DengJiYiZu").c_str(),fontStr_kaiti,20.0f);
			mtTitleTTF->setPosition(ccp(mtTitleTTF->getContentSize().width / 2,titleBarSpr->getContentSize().height / 2));
			mtTitleTTF->setColor(ccc3(206,26,0));
			titleBarSpr->addChild(mtTitleTTF);
		}
	}
	return point;
}
void NetSystemRewardUI::clickInfo(CCObject *obj)
{
	CCMenuItem *item = (CCMenuItem*)obj;
	if (item->getTag() == 1)
	{
		LoginGiftLayer *layer = LoginGiftLayer::create();
		layer->setCallBakc(this,SEL_CallFunc(&NetSystemRewardUI::resetDengLuJiangLi));
		mFooterLayer->getHeader()->setContent(BaseSprite::create());
		mFooterLayer->popUpContaintLayer(layer);
	}
	else
	{
		SystemGiftLayer *layer = SystemGiftLayer::create();
		layer->setCallBakc(this,SEL_CallFunc(&NetSystemRewardUI::resetDengJiJiangLi));
		mFooterLayer->getHeader()->setContent(BaseSprite::create());
		mFooterLayer->popUpContaintLayer(layer);
	}
}
CCPoint NetSystemRewardUI::addRiChangJiangli(CCLayer *layer,CCPoint pos)
{
	CCPoint point = CCPointZero;
	CCSprite *jinriBoard = CCSprite::create("denglujiangli_jinri_bg.png");
	m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(5,jinriBoard));
	if (mGiftsSysVec.size() <= 0)
	{
		CCSprite *koSpr = CCSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
		koSpr->setPosition(ccp(jinriBoard->getContentSize().width / 2,20 + koSpr->getContentSize().height / 2));
		jinriBoard->addChild(koSpr);

		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);
		layer->addChild(jinriBoard);
		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_jinrifuli.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		CCLabelTTF *nameLabel = CCLabelTTF::create(LFStrings::getValue("JinRiRiChangYiWanCheng").c_str(), fontStr_kaiti, 20);
		titleBarSpr->addChild(nameLabel);
		nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width / 2,titleSpr->getPositionY()));
		nameLabel->setColor(ccWHITE);

		CCSprite *titleBarSpr2 = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr2->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr2);

		point = titleBarSpr2->getPosition();
		point.y -= titleBarSpr2->getContentSize().height / 2;
		return point;
	}
	int index = 0;
	//if (mGiftsSysVec.at(0).is_get || mGiftsSysVec.at(0).enable)  //已领取过的、 和今日的
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		jinriBoard->addChild(boardMenu,1);
		layer->addChild(jinriBoard, 1);
		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));

		CCMenuItemImage *qianWangItem = CCMenuItemImage::create(
			"xinguanqia_qianwang.png",
			"xinguanqia_qianwang_select.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_RiChangGoTo));
		boardMenu->addChild(qianWangItem, 0);
		qianWangItem->setTag(3);
		qianWangItem->setPosition(ccp(550, 189-103));
		CCMenuItemImage *lingQuItem = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_GetGift));
		boardMenu->addChild(lingQuItem, 0);
		lingQuItem->setPosition(ccp(550, 189-103));
		lingQuItem->setUserData((void*)mGiftsSysVec.at(index).bonus_id);
		lingQuItem->setTag(3);

		if (mGiftsSysVec.at(index).enable)
		{

			qianWangItem->setVisible(false);
			qianWangItem->setEnabled(false);

		}
		else
		{
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		m_lLingQuItemList.push_back(lingQuItem);
		m_lYiLingQuSprList.push_back(qianWangItem);

		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);

		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_jinrifuli.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		/*CCMenuItemImage *infoitem = CCMenuItemImage::create(
			"denglujiangli_anniu_xiangqing.png",
			"denglujiangli_anniu_xiangqing_select.png",
			this, menu_selector(NetSystemRewardUI::clickInfo));
		boardMenu->addChild(infoitem, 0);
		infoitem->setPosition(ccp(titleBarSpr->getContentSize().width - infoitem->getContentSize().width / 2, titleBarSpr->getPositionY()));
		infoitem->setTag(2);*/
		//

		if (mGiftsSysVec.at(index).vip_level > 0)
		{
			CCLabelTTF *ttf = CCLabelTTF::create(CCString::createWithFormat("VIP%d%s",mGiftsSysVec.at(index).vip_level,LFStrings::getValue("VIPFanBei").c_str())->getCString(),fontStr_kaiti,30.0f);
			ttf->setPosition(ccp(jinriBoard->getContentSize().width / 2,jinriBoard->getContentSize().height / 2));
			jinriBoard->addChild(ttf,10);
		}

		if (mGiftsSysVec.at(index).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsSysVec.at(index).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}

		int itemNum = mGiftsSysVec.at(index).itemVec.size();
		sort(mGiftsSysVec.at(index).itemVec.begin(), mGiftsSysVec.at(index).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsSysVec.at(index).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsSysVec.at(index).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsSysVec.at(index).itemVec.at(i).type).c_str(), mGiftsSysVec.at(index).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84+i*130, 189-103));
		}

		int itemNum2 = mGiftsSysVec.at(index).itemDebrisVec.size();
		//sort(mGiftsSysVec.at(index).itemDebrisVec.begin(), mGiftsSysVec.at(index).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsSysVec.at(index).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsSysVec.at(index).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsSysVec.at(index).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsSysVec.at(index).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsSysVec.at(index).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsSysVec.at(index).equipDebrisVec.size();
		//sort(mGiftsSysVec.at(index).equipDebrisVec.begin(), mGiftsSysVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsSysVec.at(index).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsSysVec.at(index).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsSysVec.at(index).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsSysVec.at(index).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsSysVec.at(index).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130, 189-103));
		}
		int itemNum4 = mGiftsSysVec.at(index).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsSysVec.at(index).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsSysVec.at(index).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsSysVec.at(index).giftBagVec.at(i).id).c_str(),mGiftsSysVec.at(index).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 189-103));
		}
		//装备

		for (int i=0, count=mGiftsSysVec.at(index).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsSysVec.at(index).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(NetSystemRewardUI::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsSysVec.at(index).equipVec.at(i));
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4)*130, 189-112));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}
		int list_size = mGiftsSysVec.size();
		CCLabelTTF *nameLabel = CCLabelTTF::create(CCString::createWithFormat("%s		(%d / %d)",LFStrings::getValue("RiChangRenWu").c_str(),m_nYiJingOverRenWu + 1 , (m_nYiJingOverRenWu + list_size ))->getCString(), fontStr_kaiti, 20);
		titleBarSpr->addChild(nameLabel);
		nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width / 2,titleSpr->getPositionY()));
		nameLabel->setColor(ccWHITE);

	}
	//if (mGiftsSysVec.size() > 1)
	{
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr);
	
		point = titleBarSpr->getPosition();
		point.y -= titleBarSpr->getContentSize().height / 2;

	
		CCLabelTTF *mtTitleTTF = CCLabelTTF::create(mGiftsSysVec.at(0).description.c_str(),fontStr_kaiti,20.0f);
		mtTitleTTF->setPosition(ccp(mtTitleTTF->getContentSize().width / 2,titleBarSpr->getContentSize().height / 2));
		mtTitleTTF->setColor(ccc3(206,26,0));
		titleBarSpr->addChild(mtTitleTTF);
		m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(6,titleBarSpr));
	}
	return point;
}
CCPoint NetSystemRewardUI::addXuanShangJiangli(CCLayer *layer,CCPoint pos)
{
	CCPoint point = CCPointZero;
	CCSprite *jinriBoard = CCSprite::create("denglujiangli_jinri_bg.png");
	m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(7,jinriBoard));
	if (mGiftsChooseVec.size() <= 0)
	{
		return pos;
	}
	if (mGiftsChooseVec.at(0).is_get)
	{
		CCSprite *koSpr = CCSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
		koSpr->setPosition(ccp(jinriBoard->getContentSize().width / 2,20 + koSpr->getContentSize().height / 2));
		jinriBoard->addChild(koSpr);

		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);
		layer->addChild(jinriBoard);
		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_xuanshangrenwu.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		CCLabelTTF *nameLabel = CCLabelTTF::create(LFStrings::getValue("JinRiZhouChangYiWanCheng").c_str(), fontStr_kaiti, 20);
		titleBarSpr->addChild(nameLabel);
		nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width / 2,titleSpr->getPositionY()));
		nameLabel->setColor(ccWHITE);

		CCSprite *titleBarSpr2 = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr2->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr2);

		point = titleBarSpr2->getPosition();
		point.y -= titleBarSpr2->getContentSize().height / 2;
		return point;
	}
	int index = 0;
	//if (mGiftsChooseVec.at(0).is_get || mGiftsChooseVec.at(0).enable)  //已领取过的、 和今日的
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		jinriBoard->addChild(boardMenu,1);
		layer->addChild(jinriBoard, 1);
		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height / 2));

		CCMenuItemImage *qianWangItem = CCMenuItemImage::create(
			"xinguanqia_qianwang.png",
			"xinguanqia_qianwang_select.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_XuanShangGoTo));
		boardMenu->addChild(qianWangItem, 0);
		qianWangItem->setTag(4);
		qianWangItem->setPosition(ccp(550, 189-103));
		CCMenuItemImage *lingQuItem = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(NetSystemRewardUI::menuItemClicked_GetGift));
		boardMenu->addChild(lingQuItem, 0);
		lingQuItem->setPosition(ccp(550, 189-103));
		lingQuItem->setUserData((void*)mGiftsChooseVec.at(index).bonus_id);
		lingQuItem->setTag(4);

		if (mGiftsChooseVec.at(index).enable)
		{
			qianWangItem->setVisible(false);
			qianWangItem->setVisible(false);
		}
		else
		{
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		m_lLingQuItemList.push_back(lingQuItem);
		m_lYiLingQuSprList.push_back(qianWangItem);

		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);

		CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_xuanshangrenwu.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);

		/*CCMenuItemImage *infoitem = CCMenuItemImage::create(
			"denglujiangli_anniu_xiangqing.png",
			"denglujiangli_anniu_xiangqing_select.png",
			this, menu_selector(NetSystemRewardUI::clickInfo));
		boardMenu->addChild(infoitem, 0);
		infoitem->setPosition(ccp(titleBarSpr->getContentSize().width - infoitem->getContentSize().width / 2, titleBarSpr->getPositionY()));
		infoitem->setTag(2);*/
		//
		if (mGiftsChooseVec.at(index).vip_level > 0)
		{
			CCSprite *vipSpr = getVipImg(mGiftsChooseVec.at(index).vip_level);
			vipSpr->setPosition(ccp(jinriBoard->getContentSize().width / 2,jinriBoard->getContentSize().height / 2 + vipSpr->getContentSize().height / 2));
			jinriBoard->addChild(vipSpr);
			CCLabelTTF *ttf = CCLabelTTF::create(LFStrings::getValue("VIPShuangBei").c_str(),fontStr_kaiti,24.0f);
			ttf->setPosition(ccp(jinriBoard->getContentSize().width / 2,jinriBoard->getContentSize().height / 2 - ttf->getContentSize().height));
			ttf->setColor(fonColor_FaGuang);
			jinriBoard->addChild(ttf,10);
		}

		if (mGiftsChooseVec.at(index).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsChooseVec.at(index).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}

		int itemNum = mGiftsChooseVec.at(index).itemVec.size();
		sort(mGiftsChooseVec.at(index).itemVec.begin(), mGiftsChooseVec.at(index).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsChooseVec.at(index).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsChooseVec.at(index).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsChooseVec.at(index).itemVec.at(i).type).c_str(), mGiftsChooseVec.at(index).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84+i*130, 189-103));
		}

		int itemNum2 = mGiftsChooseVec.at(index).itemDebrisVec.size();
		//sort(mGiftsChooseVec.at(index).itemDebrisVec.begin(), mGiftsChooseVec.at(index).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsChooseVec.at(index).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsChooseVec.at(index).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsChooseVec.at(index).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsChooseVec.at(index).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsChooseVec.at(index).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsChooseVec.at(index).equipDebrisVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsChooseVec.at(index).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsChooseVec.at(index).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsChooseVec.at(index).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsChooseVec.at(index).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsChooseVec.at(index).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130, 189-103));
		}
		int itemNum4 = mGiftsChooseVec.at(index).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsChooseVec.at(index).giftBagVec.at(i).id));
			
			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsChooseVec.at(index).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsChooseVec.at(index).giftBagVec.at(i).id).c_str(),mGiftsChooseVec.at(index).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 189-103));
		}

		//装备

		for (int i=0, count=mGiftsChooseVec.at(index).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsChooseVec.at(index).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(NetSystemRewardUI::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsChooseVec.at(index).equipVec.at(i));
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4)*130, 189-112));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}

		CCLabelTTF *nameLabel = CCLabelTTF::create(LFStrings::getValue("RiChangRenWu").c_str(), fontStr_kaiti, 20);
		titleBarSpr->addChild(nameLabel);
		nameLabel->setPosition(ccp(titleBarSpr->getContentSize().width / 2,titleSpr->getPositionY()));
		nameLabel->setColor(ccWHITE);

	}
	//if (mGiftsChooseVec.size() > 1)
	{
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_zitiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getPositionX(),jinriBoard->getPositionY() - jinriBoard->getContentSize().height / 2 - titleBarSpr->getContentSize().height / 2 + 10));
		layer->addChild(titleBarSpr);

		point = titleBarSpr->getPosition();
		point.y -= titleBarSpr->getContentSize().height / 2;

		CCLabelTTF *mtTitleTTF = CCLabelTTF::create(mGiftsChooseVec.at(0).description.c_str(),fontStr_kaiti,20.0f);
		mtTitleTTF->setPosition(ccp(mtTitleTTF->getContentSize().width / 2,titleBarSpr->getContentSize().height / 2));
		mtTitleTTF->setColor(ccc3(206,26,0));
		titleBarSpr->addChild(mtTitleTTF);
		m_lInfoBgSprList.insert(map<short,CCSprite*>::value_type(8,titleBarSpr));
	}
	return point;
}
void NetSystemRewardUI::menuItemClicked_RiChangGoTo(CCObject *obj)
{
	switch (mGiftsSysVec.at(0).bonus_id)
	{
	case BonusID_Killed_Boss:
		{
			//前往打BOSS
			MainLayerBase* footer = new MainLayerBase();
			BackgroudLayer* bgLayer = new BackgroudLayer();

			MapsLayer* mapslayer = new MapsLayer(GoTo_BossTiaozhen_UI_Type);
			footer->setContaintLayer(mapslayer);
			mapslayer->setWillGoToLevel(TaskManager::getManager()->getLastActivedMainTaskId());

			GameScene* scene = GameScene::createScene();
			scene->addChild(footer,1);
			scene->addChild(bgLayer,-1);

			footer->release();
			bgLayer->release();
			mapslayer->release();

			CCDirector::sharedDirector()->replaceScene(scene);
		}
		break;
	case BonusID_Random_Arena:
		{
			if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChangPlay))
			{
				GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_random);//前往随机竞技
			}
			else
			{
				string infoStr = LFStrings::getValue("ZhuJiaoJingJi");
				UnblockWarnDialog* pDlg = UnblockWarnDialog::create(infoStr);
				CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
				return;
			}
			
		}
		break;
	case BonusID_VarietyStore:
		{
			//if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChangPlay))
			{
				//GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_random);
			}
			//else
			{
				//VarietyStoreLayer *layer = VarietyStoreLayer::create();
				//mFooterLayer->setContaintLayer(layer);
			}
		}
		break;
	case BonusID_Intensify_GoldStar:
		{
			if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_QiangHuaHuiTui))
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("ZhuJiaoQiangHua"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				return;
			}
			else 
			{
				BaseLayer *layer = GongJiangQianghua::create(gongjiang_qianghua);
				mFooterLayer->setContaintLayer(layer);
			}
		}
		break;
	}

}
void NetSystemRewardUI::menuItemClicked_XuanShangGoTo(CCObject *obj)
{
	switch (mGiftsChooseVec.at(0).bonus_id)
	{
	case BonusID_Discover:
		{
			if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXunBaoTeachPlay))
			{
				TanSuoLayer *layer = new TanSuoLayer();
				mFooterLayer->setContaintLayer(layer);
				layer->release();
			}
			else
			{
				string infoStr = LFStrings::getValue("ZhuJiaoXunBao");
				UnblockWarnDialog* pDlg = UnblockWarnDialog::create(infoStr);
				CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
				return;
			}
			
		}
		break;
	case BonusID_TopHeroList:
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_YingXiongBang);

			if (bUnblock)
			{
				HeroRankList *layer = new HeroRankList();
				mFooterLayer->setContaintLayer(layer);
				layer->release();
			}
			else
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("ZhuJiaoYingXiongBang"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
		}
		break;
	case BonusID_SelfChallenge:
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_YingXiongBang);
			if (bUnblock)
			{
				MainLayerBase* footer = new MainLayerBase();
				BackgroudLayer* bgLayer = new BackgroudLayer();

				XueSeJiaoTangLayer* layer = XueSeJiaoTangLayer::create();
				footer->setContaintLayer(layer);

				GameScene* scene = GameScene::createScene();
				scene->addChild(footer,1);
				scene->addChild(bgLayer,-1);

				footer->release();
				bgLayer->release();

				CCDirector::sharedDirector()->replaceScene(scene);
			}
			else
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("ZhuJiaoXueSe"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
		}
		break;
	}
}
void NetSystemRewardUI::sortRiChangList()
{
	m_nYiJingOverRenWu = 0;
	vector<Flat_BonusInfo> enableNotGetList;//完成任务，没有领取的
	vector<Flat_BonusInfo> notEnableNotGetList;//没完成任务的

	for (vector<Flat_BonusInfo>::iterator iter = mGiftsSysVec.begin() ; iter != mGiftsSysVec.end() ; iter++)
	{
		if (iter->enable && (!iter->is_get))
		{
			enableNotGetList.push_back(*iter);
		}
		else if ((!iter->enable) && (!iter->is_get))
		{
			notEnableNotGetList.push_back(*iter);
		}
		else
		{
			m_nYiJingOverRenWu++;
		}
	}
	mGiftsSysVec.clear();
	for (vector<Flat_BonusInfo>::iterator iter = enableNotGetList.begin() ; iter != enableNotGetList.end() ; iter++)
	{
		mGiftsSysVec.push_back(*iter);
	}
	for (vector<Flat_BonusInfo>::iterator iter = notEnableNotGetList.begin() ; iter != notEnableNotGetList.end() ; iter++)
	{
		mGiftsSysVec.push_back(*iter);
	}
}
void NetSystemRewardUI::clickCancel(CCObject *pSender)
{
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}
void NetSystemRewardUI::displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo)
{
}
void NetSystemRewardUI::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_Close)
	{
		this->removeChildByTag(Tag_Board_EquipDetail);
		disableAllTouchEnd();
	}
	else
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
			mCurTaoZhuang_Info.zb_group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
		this->addChild(layer, 20, Tag_Board_TaoZhuangShuxing);
		layer->setPosition(ccp(0, 106));
		layer->release();
	}
}

void NetSystemRewardUI::callbackInterface_TaoZhuangLayer()
{
	CCSprite *board = (CCSprite*)this->getChildByTag(Tag_Board_EquipDetail);
	if (board)
	{
		CCMenu *menu = (CCMenu*)board->getChildByTag(Tag_Menu_EquipDetail);
		if (menu) menu->setEnabled(true);


		this->removeChildByTag(Tag_Board_TaoZhuangShuxing);
	}
	else
	{
		CCLog("Error: %s 没有找到详情面板。", __FUNCTION__);
	}
}