#include "ShenBingShangDian.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "BaseActorPropertyParser.h"
#include "font.h"
#include "GameDirector.h"
#include "flat.h"
#include "MessageDisplay.h"
#include "ShenBingShangDianDuanZao.h"
#include "WuPinManager.h"
#include "MainLayerStoreFrame.h"
#include "FXScrollMenu.h"
#include "ClientErrorCode.h"

#include "ChuanqiduanzaoLayerScrollView.h"

enum 
{
	Tag_Board = 10, 
	Tag_Menu,
	Tag_MenuItem_Yici,
	Tag_MenuItem_Shici,

	Tag_BtnMenu,
	Tag_ScorllView,
	Tag_LeftBtn,
	Tag_RightBtn,
};

bool sortActorByProfId(const BaseActor *a, const BaseActor *b)
{
	return a->getCurrentProperty().profession < b->getCurrentProperty().profession;
}

ShenBingShangDian::ShenBingShangDian(MainLayerBase* footerLayer, CCSize contentSize)
{
	mFooterLayer = footerLayer;
	mActors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	sort(mActors.begin(), mActors.end(), sortActorByProfId);
	
// 	mTableView = CCTableView::create(this, CCSizeMake(613, contentSize.height));
// 	addChild(mTableView);
// 	mTableView->setDirection(kCCScrollViewDirectionVertical);
// 	mTableView->setPosition(ccp(13.5,13));
// 	mTableView->setDelegate(this);
// 	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

	//初始化滚动界面
	CCSprite* pTemp = CCSprite::create("shangdian_chuanqi_bg.png");
	ChuanqiduanzaoLayerScrollView* pScorllView = ChuanqiduanzaoLayerScrollView::createView(CCSizeMake(pTemp->getContentSize().width, pTemp->getContentSize().height), mActors.size(), 1, 1);
	addChild(pScorllView);
	pScorllView->setPosition(ccp(13.5, this->getContentSize().height-370-pTemp->getContentSize().height));
	pScorllView->setTouchEnabled(true);
	pScorllView->setTouchPriority(kCCMenuHandlerPriority-1);
	pScorllView->setClickableCreater(pScorllView);
	pScorllView->prepareToDisplay();
	pScorllView->setTag(Tag_ScorllView);

	//初始化左右按钮
	CCMenu* pMenu = CCMenu::create();
	pMenu->setTouchPriority(kCCMenuHandlerPriority-200);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1, Tag_BtnMenu);

	//左边按钮（上一个）
	CCMenuItemImage* pLeft = CCMenuItemImage::create("tujian_anniu_zuo.png", "tujian_anniu_zuo_select.png", 
		this, menu_selector(ShenBingShangDian::changePageIndex));
	pLeft->setPosition(ccp(pLeft->getContentSize().width*2, pScorllView->getPositionY()+pScorllView->getContentSize().height/2-pLeft->getContentSize().height/2));
	pLeft->setTag(Tag_LeftBtn);
	pLeft->setVisible(false);

	//右边按钮（上一个）
	CCMenuItemImage* pRight = CCMenuItemImage::create("tujian_anniu_you.png", "tujian_anniu_you_select.png", 
		this, menu_selector(ShenBingShangDian::changePageIndex));
	pRight->setPosition(ccp(this->getContentSize().width - pRight->getContentSize().width*2, pLeft->getPositionY()));
	pRight->setTag(Tag_RightBtn);
	if (mActors.size() == 1)
	{
		pRight->setVisible(false);
	}
	

	pMenu->addChild(pLeft);
	pMenu->addChild(pRight);
}

void ShenBingShangDian::changePageIndexEnd(int iPageIndex)
{
	CCMenu* pMenu = (CCMenu*)this->getChildByTag(Tag_BtnMenu);
	if (!pMenu)
	{
		return;
	}
	CCNode* pNode = NULL;
	if (iPageIndex == mActors.size()-1)
	{
		pNode = pMenu->getChildByTag(Tag_RightBtn);
		pNode->setVisible(false);
		pNode = pMenu->getChildByTag(Tag_LeftBtn);
	}
	else if (iPageIndex == 0)
	{
		pNode = pMenu->getChildByTag(Tag_LeftBtn);
		pNode->setVisible(false);
		pNode = pMenu->getChildByTag(Tag_RightBtn);
	}
	else
	{
		pNode = pMenu->getChildByTag(Tag_LeftBtn);
		if (pNode && !pNode->isVisible())
		{
			pNode->setVisible(true);
		}
		pNode = pMenu->getChildByTag(Tag_RightBtn);
		if (pNode && !pNode->isVisible())
		{
			pNode->setVisible(true);
		}
		pNode = NULL;
	}
	if (pNode)
	{
		if (mActors.size() > 1)
		{
			pNode->setVisible(true);
		}
	}
}

void ShenBingShangDian::changePageIndex(CCObject* pObj)
{
	ChuanqiduanzaoLayerScrollView* pScorllView = (ChuanqiduanzaoLayerScrollView*)this->getChildByTag(Tag_ScorllView);
	if (!pScorllView)
	{
		return;
	}
	int index = pScorllView->getCurPageIndex();
	int willToPageIndex = -1;
	CCMenuItemImage* pItem = (CCMenuItemImage*)pObj;
	int iTag = pItem->getTag();
	if (iTag == Tag_LeftBtn)
	{
		if (index >= 1)
		{
			willToPageIndex = index - 1;
		}
	}
	else if (iTag == Tag_RightBtn)
	{
		if (index <= mActors.size()-1)
		{
			willToPageIndex = index + 1;
		}
	}
	if (willToPageIndex >= 0)
	{
		pScorllView->scrollToPageSimple(willToPageIndex);
	}
}

void ShenBingShangDian::onEnter()
{
	BaseLayer::onEnter();
	//mfooter 改变了this layer的posY  导致tableview位置改变  scrollmenu 会出现不准确
//	mTableView->reloadData();
}

CCSize ShenBingShangDian::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(622, 188);
}

unsigned int ShenBingShangDian::numberOfCellsInTableView(CCTableView *table)
{
//	return 8;

	//只显示主角 和 已招募的助手
	return mActors.size();
}

CCTableViewCell* ShenBingShangDian::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL)
	{
		cell = new CCTableViewCell;
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}
	
	if (table == mTableView)
	{
		BaseSprite *board = BaseSprite::create("shangdian_shenbin_shangpin_bg.png");
		cell->addChild(board, 0, Tag_Board);
		board->setPosition(ccp(622/2, 188/2));
		CCSize bgSize = board->getContentSize();

		int  profId = mActors.at(idx)->getCurrentProperty().profession;
		BaseSprite *headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead((Profession)profId).getCurrentImageFile());
		//1. 头像  一共8个， 不同的主角 看到的第一个不同
		board->addChild(headImg);
		headImg->setPosition(ccp(70, bgSize.height - 71));
		BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
		headImg->addChild(kuang);
		kuang->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
		// 2. 名字    野蛮人神兵锻造
		string name = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)profId).nameInChinese 
			+ LFStrings::getValue("shenbing") + LFStrings::getValue("DuanZao");
		CCLabelTTF *nameLabel = CCLabelTTF::create(name.c_str(), fontStr_kaiti, 25);
		board->addChild(nameLabel);
		nameLabel->setAnchorPoint(ccp(0, 0.5f));
		nameLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, false, false));
		nameLabel->setPosition(ccp(138, bgSize.height - 31));
		//3. 锻造提示
		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("ShenBingDuanZao_ShuoMingTiShi").c_str()
			, fontStr_kaiti, 20, CCSize(0,0), kCCTextAlignmentLeft);
		board->addChild(tishiLabel);
		tishiLabel->setAnchorPoint(ccp(0, 0.5f));
		tishiLabel->setPosition(ccp(130, bgSize.height-85));
		tishiLabel->setColor(ccc3(195, 145, 96));

		//menu
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		board->addChild(menu, 0, Tag_Menu);
		menu->setPosition(ccp(0,0));
		//
		CCMenuItemImage *yici = CCMenuItemImage::create(
			"shangdian_shenbin_anniu_duanzaoyici.png",
			"shangdian_shenbin_anniu_duanzaoyici_select.png",
			this, menu_selector(ShenBingShangDian::menuItemClicked_DuanZao));
		menu->addChild(yici, 0, Tag_MenuItem_Yici);
		yici->setPosition(ccp(225, bgSize.height-147));
		yici->setUserData((void*)profId);
		CCMenuItemImage *shici = CCMenuItemImage::create(
			"shangdian_shenbin_anniu_duanzaoshici.png",
			"shangdian_shenbin_anniu_duanzaoshici_select.png",
			this, menu_selector(ShenBingShangDian::menuItemClicked_DuanZao));
		menu->addChild(shici, 0, Tag_MenuItem_Shici);
		shici->setPosition(ccp(457, bgSize.height-147));
		shici->setUserData((void*)profId);

		char buf[20];
		//need gold
		BaseSprite *goldIcon1 = BaseSprite::create("jinbi_tubiao.png");
		board->addChild(goldIcon1);
		goldIcon1->setScale(0.5f);
		goldIcon1->setPosition(ccp(yici->getPositionX()+yici->getContentSize().width/2+goldIcon1->getContentSize().width/2*goldIcon1->getScaleX()
			, yici->getPositionY()));
		sprintf(buf, "%d", 400);
		CCLabelTTF *yiciLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		board->addChild(yiciLabel);
		yiciLabel->setPosition(ccp(goldIcon1->getPositionX()+goldIcon1->getContentSize().width/2*goldIcon1->getScaleX()+yiciLabel->getContentSize().width/2
			, goldIcon1->getPositionY()));
		//
		BaseSprite *goldIcon10 = BaseSprite::create("jinbi_tubiao.png");
		board->addChild(goldIcon10);
		goldIcon10->setScale(0.5f);
		goldIcon10->setPosition(ccp(shici->getPositionX()+shici->getContentSize().width/2+goldIcon10->getContentSize().width/2*goldIcon10->getScaleX()
			, shici->getPositionY()));
		sprintf(buf, "%d", 400*10);
		CCLabelTTF *yiciLabel0 = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		board->addChild(yiciLabel0);
		yiciLabel0->setPosition(ccp(goldIcon10->getPositionX()+goldIcon10->getContentSize().width/2*goldIcon10->getScaleX()+yiciLabel0->getContentSize().width/2
			, goldIcon10->getPositionY()));
	}

	return cell;
}

void ShenBingShangDian::disableAllTouchBegin()
{
//	mTableView->setTouchEnabled(false);
// 	for (int i=0; i<numberOfCellsInTableView(mTableView); ++i)
// 	{
// 		CCTableViewCell *cell = mTableView->cellAtIndex(i);
// 		if (cell)
// 		{
// 			CCNode *board = cell->getChildByTag(Tag_Board);
// 			if (board)
// 			{
// 				CCMenu* menu = dynamic_cast<CCMenu*>(board->getChildByTag(Tag_Menu));
// 				if (menu)
// 				{
// 					menu->setEnabled(false);
// 				}
// 			}
// 		}
// 	}
}

void ShenBingShangDian::disableAllTouchEnd()
{
// 	mTableView->setTouchEnabled(true);
// 	for (int i=0; i<numberOfCellsInTableView(mTableView); ++i)
// 	{
// 		CCTableViewCell *cell = mTableView->cellAtIndex(i);
// 		if (cell)
// 		{
// 			CCNode *board = cell->getChildByTag(Tag_Board);
// 			if (board)
// 			{
// 				CCMenu* menu = dynamic_cast<CCMenu*>(board->getChildByTag(Tag_Menu));
// 				if (menu)
// 				{
// 					menu->setEnabled(true);
// 				}
// 			}
// 		}
// 	}
}

void ShenBingShangDian::menuItemClicked_DuanZao(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	bool  bShici = false;
	if (tag == Tag_MenuItem_Yici)  bShici = false;
	else if(tag == Tag_MenuItem_Shici) bShici = true;
	else return;

	int profId = (int)((CCMenuItem*)pSender)->getUserData();

	addObserver(callfuncO_selector(ShenBingShangDian::rsp_DuanZao), MSG_DuanzaoRsp_F);
	mCmdHlp->cmdDuanzao(bShici, profId);
}

void ShenBingShangDian::rsp_DuanZao(CCObject *msg_f)
{
	removeObserver(MSG_DuanzaoRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_DuanzaoRsp *data = (Flat_DuanzaoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
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

	//刷新头部金币
	{
		int gold = 0;
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			gold = wp->getCount();
		}

		BaseSprite* header = mFooterLayer->getHeader()->getHeaderContent();

		CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_JINBI);
		char str[64];
		sprintf(str, "%d", gold);
		lable->setString(str);
	}

	ShenBingShangDianDuanZao *layer = new ShenBingShangDianDuanZao(data->equips);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000);
	layer->release();
}