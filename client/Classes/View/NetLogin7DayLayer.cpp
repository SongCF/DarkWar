#include "NetLogin7DayLayer.h"
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
#include "CmdHelper.h"
#include "NetBaseTouchLayer.h"
#include "StateBar.h"
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
static const ccColor3B ccChuanQi = {181,93,6};
NetLogin7DayLayer::NetLogin7DayLayer()
	:CCLayer()
	,m_nSelectJiangliIndex(0)
	,m_nWitchSelect(0)
	,m_oTarger(NULL)
{
}
NetLogin7DayLayer::~NetLogin7DayLayer()
{
	
}
CCScene* NetLogin7DayLayer::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	NetLogin7DayLayer *layer = NetLogin7DayLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool NetLogin7DayLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	return true;
}
void NetLogin7DayLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
	mGiftsLoginVec = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
	
	//mGiftsVec.insert(list.begin(),list.end());
	{
		sort(mGiftsLoginVec.begin(), mGiftsLoginVec.end(), compareByBonusId);
	}

	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu,11);
	menu->setTouchPriority(-130);
	m_mMenu = menu;

	CCLayerColor *colorLayer = CCLayerColor::create(ccc4(0,0,0,125));
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
	addChild(colorLayer,-1);

	CCSprite *bgSpr = CCSprite::create("zhuye_gonggao_bg.png");
	bgSpr->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2,CCDirector::sharedDirector()->getWinSize().height / 2));
	addChild(bgSpr,-1);
	m_sBackSpr = bgSpr;

	/*CCSprite *titleSpr = CCSprite::create("denglujiangli_wenzi_xitongjiangli.png");
	titleSpr->setPosition(ccp(headSpr->getContentSize().width / 2, headSpr->getContentSize().height / 2));
	headSpr->addChild(titleSpr);
	*/
	int side = 5;
	m_sBodySize = bgSpr->getContentSize();

	
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shangpin_goumai_guanbi.png"
		, "shangpin_goumai_guanbi_select.png"
		, this, menu_selector(NetLogin7DayLayer::clickCancel));
	menu->addChild(fanhui);
	fanhui->setPosition(ccp(bgSpr->getPositionX(),bgSpr->getPositionY() - bgSpr->getContentSize().height / 2 +  fanhui->getContentSize().height/2 + 30));

	fanhui->setVisible(false);
	fanhui->setEnabled(false);

	m_mCloseItem = fanhui;
	//fanhui->setPosition(ccp(100,40));
	/*CCPoint pos = addDengluJiangli(ccp(m_sBodySize.width/2, headSpr->getPositionY() - headSpr->getContentSize().height / 2));

	CCPoint pos2 = addDengjiJiangli(pos);

	CCPoint pos3 = addRiChangJiangli(pos2);
	CCPoint pos4 = addXuanShangJiangli(pos3);*/
	/*
	CCLabelTTF *titleTTF = CCLabelTTF::create(LFStrings::getValue("DayLoginJiangLi").c_str(),fontStr_kaiti,30.0f);
	titleTTF->setPosition(ccp(bgSpr->getContentSize().width / 2 ,bgSpr->getContentSize().height - titleTTF->getContentSize().height / 2 - 50.0f));
	bgSpr->addChild(titleTTF);
	titleTTF->setColor(ccChuanQi);
	*/
	CCSprite *titleSpr = CCSprite::create("lianxudenglu_biaoti.png");
	titleSpr->setPosition(ccp(bgSpr->getContentSize().width / 2 ,bgSpr->getContentSize().height - titleSpr->getContentSize().height / 2 - 50.0f));
	bgSpr->addChild(titleSpr);
	int tileHeight = 180;

	int layerHeight = tileHeight * 7;
	int scrollHeight = 596 - titleSpr->getContentSize().height;
	CCLayer *layer = CCLayer::create();
	layer->setContentSize(CCSizeMake(564,layerHeight));

	layer->setAnchorPoint(CCPointZero);

	m_cScroll = CCScrollView::create(CCSizeMake(564,scrollHeight),layer);
	m_cScroll->setDirection(kCCScrollViewDirectionVertical);
	//CCPoint pos = m_cScroll->getContentOffset();
	m_cScroll->setContentOffset(ccp(0,-(layerHeight - scrollHeight)));
	m_cScroll->setAnchorPoint(CCPointZero);
	m_cScroll->setPosition(ccp(40,105));
	//m_cAuctionList->setMenuHandler(this,menu_selector(CardAuctionUI::onRandSelectCheck),menu_selector(CardAuctionUI::onRandSelectBuy));
	m_sBackSpr->addChild(m_cScroll);

	//CCPoint pos = addDengluJiangli(layer,ccp(615.0f / 2.0f,224 * 4));
	CCPoint pos = ccp(564.0f / 2.0f,tileHeight * 7);
	for (int i = 0 ; i < 7 ; i ++)
	{
		pos = addDengluJiangli(layer,pos, i);
	}
	int offset = -(layerHeight - scrollHeight);
	offset += (tileHeight) * m_nWitchSelect;
	m_cScroll->setContentOffset(ccp(0,offset));
	//m_cScroll->setContentOffset(ccp(0,(tileHeight) * m_nWitchSelect));
	m_cScroll->setTouchPriority(-129);
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(layer,-129,true);
	m_lScrollLayer = layer;
	
}
void NetLogin7DayLayer::menuItemClicked_GetGift(CCObject *obj)
{
	int index = ((CCMenuItem*)obj)->getTag();
	m_nSelectJiangliIndex = index;
	addObserver(callfuncO_selector(NetLogin7DayLayer::rsp_getGift), MSG_getBonusRsp);
	unsigned int bonusId = (unsigned int) ((CCMenuItemImage*)obj)->getUserData();
	BonusType type = BonusType_DailyLogin;
	int id = 0;
	type = BonusType_DailyLogin;
	id = mGiftsLoginVec.at(m_nSelectJiangliIndex).bonus_id;
	
	CmdHelper::getHelper()->cmdGetBonus(type,id);
}
void NetLogin7DayLayer::rsp_getGift(CCObject *msg_f)
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
			MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
		return;
	}


	//弹出奖励的框
	{
		CCString infoStr = LFStrings::getValue("HuoDe") + LFStrings::getValue("jiangli");
		GiftDisplay *layer = GiftDisplay::create(infoStr,mGiftsLoginVec.at(m_nSelectJiangliIndex));
		//AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsLoginVec.at(0).itemVec, mGiftsLoginVec.at(0).equipVec);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

		m_sLingQuOverSpr->setVisible(true);
		m_mLingQuItem->setVisible(false);
		m_mLingQuItem->setEnabled(false);
		//m_lYiLingQuSprList.at(0)->setVisible(true);
		//m_lLingQuItemList.at(0)->setVisible(false);
		//m_lLingQuItemList.at(0)->setEnabled(false);

		m_mCloseItem->setVisible(true);
		m_mCloseItem->setEnabled(true);
	}

	StateBar * bar_ = (StateBar*)MainLayerBase::getCurrentMainBase()->getHeader()->getHeaderContent();
	if (bar_) bar_->refreshGoldAndSilver();
	vector<ItemInfo> list = mGiftsLoginVec.at(m_nSelectJiangliIndex).itemVec;
	for (vector<ItemInfo>::iterator iter = list.begin() ; iter != list.end() ; iter++)
	{
		if (iter->type == ItemsType_PresentVIP1)
		{
			bar_->refreshVipMig(1);
		}
	}
}
void NetLogin7DayLayer::menuItemClicked_Equip(CCObject *obj)
{
	mCurTaoZhuang_Info = *(Flat_DuanzaoGroupZBInfo*)((CCMenuItemImage*)obj)->getUserData();

	this->disableAllTouchBegin();
	NetBaseTouchLayer *layer = NetBaseTouchLayer::create();
	
	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	
	refreshEquipBoard(board, mCurTaoZhuang_Info);
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(ccp(0.5,0.5));
	layer->setPosition(ccp(m_sBodySize.width/2, this->getContentSize().height/2));// + 106));

	layer->setContentSize(board->getContentSize());
	layer->addChild(board);
	this->addChild(layer, 200, Tag_Board_EquipDetail);
	board->setPosition(ccp(board->getContentSize().width / 2, board->getContentSize().height/2));
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(layer,-129,true);


	CCMenu *menu = CCMenu::create();
	layer->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
	menu->setTouchPriority(-130);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, 
		menu_selector(NetLogin7DayLayer::menuItemClicked_equipDetailMenu));
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
			, this ,menu_selector(NetLogin7DayLayer::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		taozhuangItem->setEnabled(false);
	}
}

void NetLogin7DayLayer::clickCancel(CCObject *pSender)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	this->removeFromParent();
	if (m_oTarger)
	{
		(m_oTarger->*m_hCallBackHandler)();
	}
}
void NetLogin7DayLayer::displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo)
{
}
void NetLogin7DayLayer::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_Close)
	{
		CCLayer *layer = (CCLayer *)this->getChildByTag(Tag_Board_EquipDetail);
		if (layer)
		{
			CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(layer);
			layer->removeFromParent();
			MainLayerBase::getCurrentMainBase()->getFooter()->setVisible(true);
		}
		
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

void NetLogin7DayLayer::callbackInterface_TaoZhuangLayer()
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

CCPoint NetLogin7DayLayer::addDengluJiangli(CCLayer *layer,CCPoint pos,int index)
{
	CCPoint point = CCPointZero;
	CCSprite *jinriBoard = CCSprite::create("denglujiangli_jinri_bg.png");
	//if (mGiftsLoginVec.at(index).is_get || mGiftsLoginVec.at(index).enable)  //已领取过的、 和今日的
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setTouchPriority(-132);
		boardMenu->setPosition(CCPointZero);
		jinriBoard->addChild(boardMenu,1);
		layer->addChild(jinriBoard, 1);
		jinriBoard->setPosition(ccp(pos.x,pos.y - jinriBoard->getContentSize().height * 0.90 / 2));
		jinriBoard->setScale(0.9);
		CCSprite *yiLingquSpr = CCSprite::create("denglujiangli_anniu_yilingqu.png");
		jinriBoard->addChild(yiLingquSpr, 0);
		yiLingquSpr->setPosition(ccp(550, 189-103));
		CCMenuItemImage *lingQuItem = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(NetLogin7DayLayer::menuItemClicked_GetGift));
		boardMenu->addChild(lingQuItem, 0);
		lingQuItem->setPosition(ccp(550, 189-103));
		lingQuItem->setUserData((void*)mGiftsLoginVec.at(index).bonus_id);
		lingQuItem->setTag(index);

		if (mGiftsLoginVec.at(index).enable && ! mGiftsLoginVec.at(index).is_get)
		{
			m_sLingQuOverSpr = yiLingquSpr;
			m_mLingQuItem = lingQuItem;
			yiLingquSpr->setVisible(false);
			m_nWitchSelect = index;
			//m_lItemList.push_back(lingQuItem);
		}
		else if (!mGiftsLoginVec.at(index).enable && ! mGiftsLoginVec.at(index).is_get)
		{
			yiLingquSpr->setVisible(false);
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		else
		{
			lingQuItem->setEnabled(false);
			lingQuItem->setVisible(false);
		}
		CCSprite *titleBarSpr = CCSprite::create("denglujiangli_hongtiao.png");
		titleBarSpr->setPosition(ccp(jinriBoard->getContentSize().width/2, 189-30));
		jinriBoard->addChild(titleBarSpr);

		/*CCSprite *titleSpr = CCSprite::create("denglujiangli_biaoti_meiridenglu.png");
		titleSpr->setPosition(ccp(titleSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleSpr);*/

		string tileTitle = "";
		switch (index)
		{
		case 0:
			{
				tileTitle = LFStrings::getValue("FirstDay") + LFStrings::getValue("DayLogin");
			}
			break;
		case 1:
			{
				tileTitle = LFStrings::getValue("SecondDay") + LFStrings::getValue("DayLogin");
			}
			break;
		default:
			{
				tileTitle = CCString::createWithFormat("%d%s",index + 1,LFStrings::getValue("DayLogin").c_str())->getCString();
			}
			break;
		}
		CCLabelTTF *titleTTF = CCLabelTTF::create(tileTitle.c_str(),fontStr_kaiti,25.0f);
		titleTTF->setPosition(ccp(titleBarSpr->getContentSize().width/2, titleBarSpr->getContentSize().height / 2));
		titleBarSpr->addChild(titleTTF);

		titleTTF->setColor(ccChuanQi);
		//

		if (mGiftsLoginVec.at(index).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsLoginVec.at(index).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}

		int itemNum = mGiftsLoginVec.at(index).itemVec.size();
		sort(mGiftsLoginVec.at(index).itemVec.begin(), mGiftsLoginVec.at(index).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(index).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsLoginVec.at(index).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsLoginVec.at(index).itemVec.at(i).type).c_str(), mGiftsLoginVec.at(index).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84+i*130, 189-103));
		}

		int itemNum2 = mGiftsLoginVec.at(index).itemDebrisVec.size();
		//sort(mGiftsLoginVec.at(index).itemDebrisVec.begin(), mGiftsLoginVec.at(index).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(index).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(index).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(index).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLoginVec.at(index).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(index).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsLoginVec.at(index).equipDebrisVec.size();
		//sort(mGiftsLoginVec.at(index).equipDebrisVec.begin(), mGiftsLoginVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(index).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsLoginVec.at(index).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsLoginVec.at(index).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsLoginVec.at(index).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsLoginVec.at(index).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			jinriBoard->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130, 189-103));
		}
		int itemNum4 = mGiftsLoginVec.at(index).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsLoginVec.at(index).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsLoginVec.at(index).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsLoginVec.at(index).giftBagVec.at(i).id).c_str(),mGiftsLoginVec.at(index).giftBagVec.at(i).count);
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
		if (mGiftsLoginVec.at(index).ran_zb_count > 0)
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
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),mGiftsLoginVec.at(index).ran_zb_count);
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

		for (int i=0, count=mGiftsLoginVec.at(index).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsLoginVec.at(index).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(NetLogin7DayLayer::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);

			m_lItemList.push_back(eqDiwenImg);

			eqDiwenImg->setUserData(&mGiftsLoginVec.at(index).equipVec.at(i));
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4 + itemNum5)*130, 189-112));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}
	}

	point = jinriBoard->getPosition();
	point.y -= jinriBoard->getContentSize().height / 2;
	return point;
}
void NetLogin7DayLayer::addObserver(SEL_CallFuncO selector,const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
}
// 移除消息监听
void NetLogin7DayLayer::removeObserver(const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
}
void NetLogin7DayLayer::disableAllTouchBegin()
{
	m_mMenu->setEnabled(false);
	m_lScrollLayer->setTouchEnabled(false);
	for (vector<CCMenuItem *>::iterator iter = m_lItemList.begin(); iter != m_lItemList.end() ; iter ++)
	{
		(*iter)->setEnabled(false);
	}
	//m_cScroll->setTouchPriority(0);
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

void NetLogin7DayLayer::disableAllTouchEnd()
{
	m_mMenu->setEnabled(true);
	m_lScrollLayer->setTouchEnabled(true);
	for (vector<CCMenuItem *>::iterator iter = m_lItemList.begin(); iter != m_lItemList.end() ; iter ++)
	{
		(*iter)->setEnabled(true);
	}
	//m_cScroll->setTouchPriority(-130);
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
}
bool NetLogin7DayLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}