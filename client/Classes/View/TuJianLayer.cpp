#include "TuJianLayer.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ImageManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "FX_CommonFunc.h"
#include "BaseActorPropertyParser.h"
#include "MyselfManager.h"
#include "FXScrollMenu.h"
#include "GuideLayerBase.h"
#include "JiuGongGeLayer.h"
#include "TaskManager.h"
#include "BackgroudLayer.h"
#include "MapsLayer.h"
#include "GameScene.h"
#include "EquipFallManger.h"
using namespace FX_CommonFunc;

#define Label_Size_Min 20
#define Label_Size_Max 24

enum 
{
	Tag_Door1 = 1,
	Tag_Door2,

	Tag_MenuItem_HeroListBefore,
	Tag_MenuItem_HeroListNext,

	Tag_HeroListMenu,
	Tag_CellMenu,

	Tag_Board_ZBShengJi = 50,
	Tag_Menu_ZbShengji,
	Tag_MenuItem_Back,
	Tag_MenuItem_Zb_,

	Tag_Board_EquipDetail = 100,
	Tag_Menu_EquipDetail,
	Tag_MenuItem_Close,
	Tag_MenuItem_TaoZhuang,

	Tag_Board_TaoZhuangShuxing = 200,

	Tag_GoTo_Fighting_Btn,
	Tag_GoTo_ChuanQi_Btn,

};


extern bool sortEquipByZhuangBeiType(const Flat_DuanzaoGroupZBInfo &a, const Flat_DuanzaoGroupZBInfo &b);
extern void sortZBInfo(vector<Flat_DuanzaoGroupZBInfo> &vec);

TuJianLayer::~TuJianLayer()
{

}

TuJianLayer::TuJianLayer()
{
	m_bIsChatGuide = false;

	m_iGuideLayerDiwenItem = NULL;

	mComeIn = false;

	mBg = BaseSprite::create("tujian_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	//黑色边框框 ---> 盖在装备table之上， hero table之下
	BaseSprite *heiKuang = BaseSprite::create("tujian_neirongkuang.png");
	mBg->addChild(heiKuang, 10);
	heiKuang->setPosition(ccp(320, 838-456));
	//door
	BaseSprite *door1 = BaseSprite::create("tujian_damen.png");
	mBg->addChild(door1, 100, Tag_Door1);
	door1->setPosition(ccp(163, 838-455));
	BaseSprite *door2 = BaseSprite::create("tujian_damen.png");
	door2->setFlipX(true);
	mBg->addChild(door2, 100, Tag_Door2);
	door2->setPosition(ccp(477, 838-455));

	//hero list
	BaseSprite *heroListBar = BaseSprite::create("tujian_shuoming.png");
	mBg->addChild(heroListBar, 11);
	heroListBar->setPosition(ccp(320, mBg->getContentSize().height-172));
	//menu
	mHeroListMenu = CCMenu::create();
	heroListBar->addChild(mHeroListMenu);
	mHeroListMenu->setTouchPriority(-129);
	mHeroListMenu->setPosition(ccp(0,0));
	CCMenuItemImage *beforeItem = CCMenuItemImage::create(
		"tujian_anniu_zuo.png",
		"tujian_anniu_zuo_select.png",
		this, menu_selector(TuJianLayer::menuItemClicked_ListBeforeNext));
	mHeroListMenu->addChild(beforeItem, 0, Tag_MenuItem_HeroListBefore);
	beforeItem->setPosition(ccp(25, 190-81));
	CCMenuItemImage *nextItem = CCMenuItemImage::create(
		"tujian_anniu_you.png",
		"tujian_anniu_you_select.png",
		this, menu_selector(TuJianLayer::menuItemClicked_ListBeforeNext));
	mHeroListMenu->addChild(nextItem, 0, Tag_MenuItem_HeroListNext);
	nextItem->setPosition(ccp(607, 190-81));
	//tabel view
	mTableViewHeroList = CCTableView::create(this, CCSizeMake(540, 158));
	mTableViewHeroList->setDirection(kCCScrollViewDirectionHorizontal);
	mTableViewHeroList->setPosition(ccp(46, 16));
	mTableViewHeroList->setDelegate(this);
	heroListBar->addChild(mTableViewHeroList);
	mTableViewHeroList->reloadData();

	mTableView = CCTableView::create(this, CCSizeMake(600, 568));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(20, 10));
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mBg->addChild(mTableView, 8);
}

void TuJianLayer::onEnter()
{
	BaseLayer::onEnter();

	mComeIn = true;
	mOldCellIdx = 0;
	mCurrCellIdx = 0;
	menuItemClicked_HeroClicked((CCMenuItemImage*)mTableViewHeroList->cellAtIndex(
		mCurrCellIdx)->getChildByTag(Tag_HeroListMenu)->getChildByTag(100));

	//mfooter 重置了该layer的pos.y，FXScrollMenu之前穿进去的menuPosition已经不准确了，在这里刷新一下 
	if (mTableViewHeroList) mTableViewHeroList->reloadData();
	if (mTableView) mTableView->reloadData();
}

void TuJianLayer::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
	for(int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if(cell != NULL)
		{
			FXScrollMenu *menu = dynamic_cast<FXScrollMenu*>(cell->getChildByTag(Tag_CellMenu));
			if (menu) menu->setEnabled(false);
		}
	}

	mTableViewHeroList->setTouchEnabled(false);
	for(int i=0; i!=numberOfCellsInTableView(mTableViewHeroList); ++i)
	{
		CCTableViewCell *cell = mTableViewHeroList->cellAtIndex(i);
		if(cell != NULL)
		{
			FXScrollMenu *menu = dynamic_cast<FXScrollMenu*>(cell->getChildByTag(Tag_HeroListMenu));
			if (menu) menu->setEnabled(false);
		}
	}

	mHeroListMenu->setEnabled(false);
}

void TuJianLayer::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);
	for(int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if(cell != NULL)
		{
			FXScrollMenu *menu = dynamic_cast<FXScrollMenu*>(cell->getChildByTag(Tag_CellMenu));
			if (menu) menu->setEnabled(true);
		}
	}

	mTableViewHeroList->setTouchEnabled(true);
	for(int i=0; i!=numberOfCellsInTableView(mTableViewHeroList); ++i)
	{
		CCTableViewCell *cell = mTableViewHeroList->cellAtIndex(i);
		if(cell != NULL)
		{
			FXScrollMenu *menu = dynamic_cast<FXScrollMenu*>(cell->getChildByTag(Tag_HeroListMenu));
			if (menu) menu->setEnabled(true);
		}
	}

	mHeroListMenu->setEnabled(true);

	if (m_iGuideLayerDiwenItem && m_bIsChatGuide)
	{
		playGuideLayer(m_iGuideLayerDiwenItem);
	}
}

void TuJianLayer::rsp_getEquipAlbumData(CCObject *msg_f)
{
	removeObserver(MSG_getZhuangbeiAlbumRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_GetZhuangbeiAlbumRsp *data = (Flat_GetZhuangbeiAlbumRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}


// 	if (mTableViewHeroList->cellAtIndex(mOldCellIdx) != NULL)
// 	{
// 		CCMenu *oldMenu = (CCMenu*)mTableViewHeroList->cellAtIndex(mOldCellIdx);
// 		if (oldMenu != NULL)
// 		{
// 			CCMenuItemImage *oldMenuItem = (CCMenuItemImage*)mTableViewHeroList->cellAtIndex(
// 				mOldCellIdx)->getChildByTag(Tag_HeroListMenu)->getChildByTag(100);
// 			if (oldMenuItem != NULL)
// 				oldMenuItem->initWithNormalImage(
// 					"renxing_renwu_touxiang.png", 
// 					"renxing_renwu_touxiang.png",
// 					NULL, 
// 					this, menu_selector(TuJianLayer::menuItemClicked_HeroClicked));
// 		}
// 	}
// 	mOldCellIdx = ((CCTableViewCell*)mCurHeroMenuItem->getParent()->getParent())->getIdx();
// 	mCurHeroMenuItem->initWithNormalImage(
// 		"renxing_renwu_touxiang_select.png", 
// 		"renxing_renwu_touxiang_select.png",
// 		NULL,
// 		this, menu_selector(TuJianLayer::menuItemClicked_HeroClicked));


	mTableViewHeroList->updateCellAtIndex(mOldCellIdx);
	mTableViewHeroList->updateCellAtIndex(mCurrCellIdx);

	//init calss member
	//神兵套装、（神兵单间为空了、） 传奇套装、 传奇单件  
	mChuanqiSuitData = data->chuanqiSuitData;
	mXiYouVec        = data->xiYouVec;
	sortZBInfo(mChuanqiSuitData);
	sortZBInfo(mXiYouVec);
// 	sort(mChuanqiSuitData.begin(), mChuanqiSuitData.end(), sortEquipByZhuangBeiType);
// 	sort(mXiYouVec.begin(), mXiYouVec.end(), sortEquipByZhuangBeiType);
	//
	if (mComeIn)
	{
		mComeIn = false;
		disableAllTouchBegin();
		//action
		BaseSprite *door1 = (BaseSprite*)mBg->getChildByTag(Tag_Door1);
		BaseSprite *door2 = (BaseSprite*)mBg->getChildByTag(Tag_Door2);

		BaseSprite *bian1 = BaseSprite::create("tujian_damen_bian.png");
		door1->addChild(bian1);
		bian1->setPosition(ccp(door1->getContentSize().width, door1->getContentSize().height/2));
		BaseSprite *bian2 = BaseSprite::create("tujian_damen_bian.png");
		door2->addChild(bian2);
		bian2->setFlipX(true);
		bian2->setPosition(ccp(0, door2->getContentSize().height/2));

		door1->runAction(CCSequence::create(
			CCMoveBy::create(1.0f, ccp(-320, 0)),
			CCCallFuncN::create(this, callfuncN_selector(TuJianLayer::callfunc_removeFromParent)),
			NULL));
		door2->runAction(CCSequence::create(
			CCMoveBy::create(1.0f, ccp(320, 0)),
			CCCallFuncN::create(this, callfuncN_selector(TuJianLayer::callfunc_removeFromParent)),
			NULL));
	}

	mTableView->reloadData();
}

void TuJianLayer::callfunc_removeFromParent(CCNode *node)
{
	node->removeFromParent();
	disableAllTouchEnd();
}

CCSize TuJianLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	if (table == mTableViewHeroList)
	{
		return CCSize(139, 158);
	}
	else
	{
		if (idx == (mChuanqiSuitData.size()+3)/4)  //下一种层色的第一个cell上面放一个分隔条
			return CCSize(600, 270+28);
		else 
			return CCSize(600, 270);
	}
}

unsigned int TuJianLayer::numberOfCellsInTableView(CCTableView *table)
{
	if (table == mTableViewHeroList)
	{
		return 10;
	}
	else 
	{
		int chuanqiTaoCellNum  = (mChuanqiSuitData.size() + 3) / 4;
		int xiyouCellNum     = (mXiYouVec.size() + 3) / 4;
		return (unsigned int)(chuanqiTaoCellNum + xiyouCellNum);
	}
}

CCTableViewCell* TuJianLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell != NULL)
	{
		cell->removeAllChildren();
	}
	else 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}

	if (table == mTableViewHeroList)
	{
		CCSize cellSize = tableCellSizeForIndex(mTableViewHeroList, idx);

		Profession profId = (Profession)(idx + 1);

		//head  Profession 1-10
		BaseSprite *headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(profId).getCurrentImageFile());
		cell->addChild(headImg);
		headImg->setPosition(ccp(cellSize.width/2, 92));
		//menu
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableViewHeroList->getParent()->convertToWorldSpace(mTableViewHeroList->getPosition()), mTableViewHeroList->getViewSize());
		cell->addChild(menu, 1, Tag_HeroListMenu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = NULL;
		if (idx == mCurrCellIdx)
			item = CCMenuItemImage::create(
			"renxing_renwu_touxiang_select.png", 
			"renxing_renwu_touxiang_select.png",
			this, menu_selector(TuJianLayer::menuItemClicked_HeroClicked));
		else 
			item = CCMenuItemImage::create(
			"renxing_renwu_touxiang.png", 
			"renxing_renwu_touxiang.png",
			this, menu_selector(TuJianLayer::menuItemClicked_HeroClicked));
		menu->addChild(item, 0, 100);
		item->setPosition(headImg->getPosition());
		item->setUserData((void*)profId);
		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(BaseActorPropertyParser::getParser()->getExtraProerty(profId).nameInChinese.c_str()
			, fontStr_kaiti, 22);
		cell->addChild(nameLabel);
		nameLabel->setPosition(ccp(headImg->getPositionX()
			, headImg->getPositionY()-headImg->getContentSize().height/2-nameLabel->getContentSize().height/2));
		nameLabel->setColor(ccc3(195, 145, 96));
	}
	else if (table == mTableView)
	{
		// .menu
		FXScrollMenu *menu = FXScrollMenu::create(
			mBg->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize()); //触摸移动后， menu就不会响应了
		cell->addChild(menu, 1, Tag_CellMenu);
		menu->setPosition(ccp(0,0));
		menu->setUserData((void*)idx);

		for (int i=0; i<4; ++i)
		{
			Flat_DuanzaoGroupZBInfo zbInfo;
			//传奇套
			if (idx < (mChuanqiSuitData.size()+3)/4)
			{
				//当前这个cell不足4个时
				if (idx * 4 + i >= mChuanqiSuitData.size()) break;
				//
				zbInfo = mChuanqiSuitData.at(idx * 4 + i);
			}
			//稀有单件
			else 
			{
				//当前cell-传奇的cellNum = 稀有的cellNum，  
				if ((idx - (mChuanqiSuitData.size()+3)/4)*4 + i >= mXiYouVec.size()) break;
				//
				zbInfo = mXiYouVec.at((idx - (mChuanqiSuitData.size()+3)/4)*4 + i);

				if (mChuanqiSuitData.size() > 0 && idx == (mChuanqiSuitData.size()+3)/4)//加上分割线
				{
					BaseSprite *bar = BaseSprite::create("tujian_fengexian.png");
					cell->addChild(bar);
					bar->setPosition(ccp(tableCellSizeForIndex(mTableView, idx).width/2,
						tableCellSizeForIndex(mTableView, idx).height - bar->getContentSize().height/2));
				}
			}
			// 1. 底色

			//BaseSprite *diSe = BaseSprite::create("tujian_zhuangbei_touying.png");
			//cell->addChild(diSe);
			//diSe->setPosition(ccp(87 + 139 * i, 210 - diSe->getContentSize().height/2 - 10));
			BaseSprite *diSe = BaseSprite::create("tujian_zhuangbei_bg.png");
			cell->addChild(diSe);
			diSe->setPosition(ccp(87 + 139 * i, 270 - diSe->getContentSize().height / 2 - 10));
			
			// 2. equip
			//diwen
			string cardFile = ImageManager::getManager()->getImageData_DiWen_XiangQing(
				zbInfo.zhuangbei_colour,
				false,
				SuitPool::getPool()->isSuit(zbInfo.zb_group_id)).getCurrentImageFile();
			CCMenuItemImage *diwen = CCMenuItemImage::create(
				cardFile.c_str(),
				cardFile.c_str(),
				this, menu_selector(TuJianLayer::menuItemClicked_zbClicked));
			diwen->setUserData((void*)i);  //menu->setUserData((void*)idx);
			menu->addChild(diwen);
			diwen->setScale(0.32f);
			diwen->setPosition(ccp(diSe->getPosition().x,diSe->getPosition().y));

			if (m_bIsChatGuide && idx == 0 && i == 0)
			{
				m_iGuideLayerDiwenItem = diwen;
				m_tTableViewCell = cell;
				m_tTableViewTable = table;
				m_mGuideLayerMenu = menu;
			}
			
			//equip
			string equipfile = zbInfo.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			diwen->addChild(equipImg);
			equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
			//name label   
			string name = zbInfo.zhuangbei_name;
			CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size_Min, CCSize(128, 0), kCCTextAlignmentCenter);
			diwen->addChild(labelName);
			labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2 / diwen->getScale() - 50));
			labelName->setColor(ccBLACK);
			labelName->setScale(1 / diwen->getScale());


			string fullLevelStr = LFStrings::getValue("ChuanQiTaoZhuang");
			if (zbInfo.zhuangbei_colour != ZhuangbeiColour_Chuanqi)
			{
				fullLevelStr = LFStrings::getValue("dengji")+CCString::createWithFormat(" %d", zbInfo.level)->getCString();
			}
			CCLabelTTF *labeLevel = CCLabelTTF::create(fullLevelStr.c_str(), fontStr_kaiti, Label_Size_Min, CCSize(128, 0), kCCTextAlignmentCenter);
			diwen->addChild(labeLevel);
			labeLevel->setPosition(ccp(diwen->getContentSize().width/2,diwen->getContentSize().height+60));
			labeLevel->setColor(fonColor_PuTong);
			labeLevel->setScale(1 / diwen->getScale());
		}
	}

	return cell;
}

void TuJianLayer::menuItemClicked_zbClicked(CCObject *pSender)
{
	CCMenuItem *item = (CCMenuItem*)pSender;
	unsigned int cellIdx = (unsigned int)item->getParent()->getUserData();
	int ii = (int)item->getUserData();


	if (cellIdx * 4 + ii < mChuanqiSuitData.size())
	{
		displayTaoZhuang(mChuanqiSuitData.at(cellIdx*4+ii));
	}
	else
	{
		mCur_Info = mXiYouVec.at((cellIdx-(mChuanqiSuitData.size()+3)/4)*4+ii);
		addObserver(callfuncO_selector(TuJianLayer::QueryEquipFallRsp), MSG_QueryEquipFallRsp);
		CmdHelper::getHelper()->cmdQueryEquipFall();
//		displayXiangQing(mXiYouVec.at((cellIdx-(mChuanqiSuitData.size()+3)/4)*4+ii));
	}

}

void TuJianLayer::QueryEquipFallRsp(CCObject* msg_rsp)
{
	removeObserver(MSG_QueryEquipFallRsp);
	GameDirector::getDirector()->hideWaiting();
	Flat_QueryEquipFallRsp *data = (Flat_QueryEquipFallRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success)
	{
		displayXiangQing(mCur_Info);
	}
	else
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
	}
}
void TuJianLayer::displayTaoZhuang(Flat_DuanzaoGroupZBInfo zbInfo) // 能升级的传奇装备
{
	this->disableAllTouchBegin();

	BaseSprite *board = BaseSprite::create("tujian_bg.png");
	mBg->addChild(board, 15, Tag_Board_ZBShengJi);
	board->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_ZbShengji);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *backItem = CCMenuItemImage::create(
		"sphc_anniu_fanhui.png",
		"sphc_anniu_fanhui_select.png",
		this, menu_selector(TuJianLayer::menuItemClicked_DengJiBoardItem));
	menu->addChild(backItem, 0, Tag_MenuItem_Back);
	backItem->setPosition(ccp(516, board->getContentSize().height-748));

	//title
	BaseSprite *titleBar = BaseSprite::create("tujian_shuoming.png");
	board->addChild(titleBar);
	titleBar->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-titleBar->getContentSize().height/2));
	{
		ccColor3B color = {195, 145, 96};
		CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("TuJian_ZhuangBeiShuoMing_Chuanqi").c_str(), fontStr_kaiti, 24
			, CCSize(titleBar->getContentSize().width-26, 0), kCCTextAlignmentLeft);
		titleBar->addChild(title);
		title->setAnchorPoint(ccp(0, 1));
		title->setPosition(ccp(15, titleBar->getContentSize().height-27));
		//shengji
		BaseSprite *sjImg = BaseSprite::create("tujian_shuoming_jiantou.png");
		titleBar->addChild(sjImg);
		sjImg->setPosition(ccp(23, titleBar->getContentSize().height-94));
		CCLabelTTF *sjLabel = CCLabelTTF::create(LFStrings::getValue("TuJian_ZhuangBeiShuoMing_shengji").c_str(), fontStr_kaiti, 20);
		titleBar->addChild(sjLabel, 0);
		sjLabel->setAnchorPoint(ccp(0, 1));
		sjLabel->setPosition(ccp(34, titleBar->getContentSize().height-88));
		sjLabel->setColor(color);
		//qianghua
		BaseSprite *qhImg = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
		titleBar->addChild(qhImg);
		qhImg->setPosition(ccp(23, titleBar->getContentSize().height-124));
		qhImg->setScale(0.5f);
		CCLabelTTF *qhLabel = CCLabelTTF::create(LFStrings::getValue("TuJian_ZhuangBeiShuoMing_qianghua").c_str(), fontStr_kaiti, 20);
		titleBar->addChild(qhLabel, 0);
		qhLabel->setAnchorPoint(ccp(0, 1));
		qhLabel->setPosition(ccp(sjLabel->getPositionX(), sjLabel->getPositionY()-25));
		qhLabel->setColor(color);
		//jing lian
		BaseSprite *jlImg = BaseSprite::create("gongjiang_jinglian_tubiao.png");
		titleBar->addChild(jlImg);
		jlImg->setPosition(ccp(23, titleBar->getContentSize().height-148));
		jlImg->setScale(1.2f);
		CCLabelTTF *jlLabel = CCLabelTTF::create(LFStrings::getValue("TuJian_ZhuangBeiShuoMing_jinglian").c_str(), fontStr_kaiti, 20);
		titleBar->addChild(jlLabel);
		jlLabel->setAnchorPoint(ccp(0,1));
		jlLabel->setPosition(ccp(qhLabel->getPositionX(), qhLabel->getPositionY()-25));
		jlLabel->setColor(color);
	}

	//equip
	BaseSprite *layer = BaseSprite::create("tujian_zhuangbei_touying_2.png");
	board->addChild(layer);
	layer->setPosition(ccp(320, board->getContentSize().height-534));
	{
		string cardFile = ImageManager::getManager()->getImageData_DiWen_XiangQing(zbInfo.zhuangbei_colour).getCurrentImageFile();
		string equipfile = zbInfo.zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		mCurTaoZhuang_Info = zbInfo;  //保存一下  点击装备后 改变等级  再传给displayXiangQing

		for (int i=0; i<4; ++i)
		{
			int level = 0;
			float scale = ImageManager::getManager()->getImageData_DiWen_XiangQing(zbInfo.zhuangbei_colour).scale;
			CCMenuItemImage *item = CCMenuItemImage::create(cardFile.c_str(), cardFile.c_str(), this, 
				menu_selector(TuJianLayer::menuItemClicked_DengJiBoardItem));
			CCPoint pos;
			if (i == 1) 
			{
				level = 15;
				pos.x = 305;
				pos.y = board->getContentSize().height - 384;
				scale = scale * (102 / item->getContentSize().width);
			}
			else if (i == 2) 
			{
				level = 25;
				pos.x = 495;
				pos.y = board->getContentSize().height - 502;
				scale = scale * (118 / item->getContentSize().width);
			}
			else if (i == 3) 
			{
				level = 30;
				pos.x = 270;
				pos.y = board->getContentSize().height - 647;
				scale = scale * (148 / item->getContentSize().width);
			}
			else 
			{
				level = 5;
				pos.x = 131;
				pos.y = board->getContentSize().height - 383;
				scale = scale * (88 / item->getContentSize().width);
			}
			menu->addChild(item, 0, Tag_MenuItem_Zb_ + level);
			item->setPosition(pos);
			item->setScale(scale);
			BaseSprite *eq = BaseSprite::create(equipfile);
			item->addChild(eq, 0, 1);
			eq->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
			//name
			char buf[128];
			sprintf(buf, "%d%s%s", level, LFStrings::getValue("ji").c_str(), zbInfo.zhuangbei_name.c_str());
			CCLabelTTF *nameLabel = CCLabelTTF::create(buf, fontStr_kaiti, 20);
			item->addChild(nameLabel);
			nameLabel->setPosition(ccp(item->getContentSize().width/2, -nameLabel->getContentSize().height/2/item->getScale()));
			nameLabel->setScale(1 / item->getScale());
		}
	}
}

void TuJianLayer::displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo)
{
	this->disableAllTouchBegin();

	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	mBg->addChild(board, 20, Tag_Board_EquipDetail);
	refreshEquipBoard(board, zbInfo);
	board->setPosition(ccp(mBg->getContentSize().width/2, board->getContentSize().height/2-115));

	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
//	menu->setTouchPriority(-131);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, menu_selector(TuJianLayer::menuItemClicked_equipDetailMenu));
	menu->addChild(item, 0, Tag_MenuItem_Close);
	item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
	//底纹
	ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		zbInfo.zhuangbei_colour
		, false
		, SuitPool::getPool()->isSuit(zbInfo.zb_group_id));
	BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
	diWen->setScale(0.7f);
	board->addChild(diWen);
	diWen->setPosition(ccp(123, 838-240+122));
	// 装备图片
	string equipfile = zbInfo.zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *s = BaseSprite::create(equipfile);
	diWen->addChild(s);
	s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	//套装按钮
	if(SuitPool::getPool()->isSuit(zbInfo.zb_group_id))
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
			, this ,menu_selector(TuJianLayer::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		taozhuangItem->setEnabled(false);

		mCurTaoZhuang_Info = zbInfo;
	}

	//掉落说明
	string tipstr = "";

	int iXiYouType = 1;
	int iChuanqiType = 2;
	int iJuQingType = 3;

	int iType = 0;
	//装备掉落的关卡
	int iDiaoLuoLevelIndex = 0;
	//稀有装
	if (zbInfo.zhuangbei_colour == ZhuangbeiColour_Xiyou)
	{
		if (zbInfo.zb_group_id == 16 || zbInfo.zb_group_id == 17)
		{
			iType = iJuQingType;
			tipstr = LFStrings::getValue("Tujian_Diaoluo_Juqing_Tip");
		}
		else if (zbInfo.zhuangbei_type == ZhuangbeiType_Faqi || zbInfo.zhuangbei_type == ZhuangbeiType_DanshouFazhang 
			|| zbInfo.zhuangbei_type == ZhuangbeiType_ShuangshouBishou || zbInfo.zhuangbei_type == ZhuangbeiType_ShuangshouFu)
		{
			iType = iChuanqiType;
			tipstr = LFStrings::getValue("DiaoLuoTip_ChuanQi");
		}
		else
		{
			//装备掉落的关卡
			iDiaoLuoLevelIndex = EquipFallManager::getManager()->getAlbumEquipFallTaskId(zbInfo.zb_group_id);

			int iZhangjie = TaskManager::getManager()->getCurrentFightZhangJie();
			
			char buf[20] = {0};
			sprintf(buf, "zhangjie%d", iZhangjie);
			string LevelFullName = LFStrings::getValue(buf);
			LevelFullName += LFStrings::getValue("Dian");
			LevelFullName += ChatContentManager::sharedChatContentManager()->getMainTaskNameById(iDiaoLuoLevelIndex);

			iType = iXiYouType;
			tipstr = CCString::createWithFormat(LFStrings::getValue("Tujian_Diaoluo_Level_Tip").c_str(), LevelFullName.c_str())->getCString();
		}
	}
	else if (zbInfo.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
	{
		iType = iChuanqiType;
		tipstr = LFStrings::getValue("DiaoLuoTip_ChuanQi");
	}

	if (iType > 0)
	{
		CCLabelTTF* pDiaoLuo = CCLabelTTF::create(LFStrings::getValue("diaoluo").c_str(), fontStr_kaiti, 26);//, CCSizeMake(380, 0), kCCTextAlignmentLeft);
		pDiaoLuo->setColor(fonColor_PuTong);
 		pDiaoLuo->setHorizontalAlignment(kCCTextAlignmentLeft);
 		pDiaoLuo->setAnchorPoint(ccp(0.0f, 0.0f));
		board->addChild(pDiaoLuo);
		pDiaoLuo->setPosition(ccp(20, 50));

		CCLabelTTF* pDiaoLuoTip = CCLabelTTF::create(tipstr.c_str(), fontStr_kaiti, 26, CCSizeMake(380, 0), kCCTextAlignmentLeft);
		pDiaoLuoTip->setColor(fonColor_PuTong);
		pDiaoLuoTip->setHorizontalAlignment(kCCTextAlignmentLeft);
		pDiaoLuoTip->setAnchorPoint(ccp(0.0f, 1.0f));
		board->addChild(pDiaoLuoTip);
		pDiaoLuoTip->setPosition(ccp(pDiaoLuo->getPositionX()+pDiaoLuo->getContentSize().width+20, pDiaoLuo->getPositionY()+pDiaoLuo->getContentSize().height));

		//前往按钮
		CCMenuItemImage* pGoTo = CCMenuItemImage::create("xinguanqia_qianwang.png", "xinguanqia_qianwang_select.png",
			this, menu_selector(TuJianLayer::goToTheLayer));
		menu->addChild(pGoTo, 0);
		pGoTo->setPosition(ccp(board->getContentSize().width - pGoTo->getContentSize().width/2-20, 45));

		if (iType == iXiYouType)
		{
			pGoTo->setTag(Tag_GoTo_Fighting_Btn);
			//稀有装时，该关卡未解锁时不显示前往按钮
			if (TaskManager::getManager()->getLastActivedMainTaskId() < iDiaoLuoLevelIndex)
			{
				pGoTo->setVisible(false);
			}
		}
		else if (iType == iChuanqiType)
		{
			pGoTo->setTag(Tag_GoTo_ChuanQi_Btn);
			//为传奇装备时，小于5级不显示前往按钮
			if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < 5)
			{
				pGoTo->setVisible(false);
			}
		}
		else if (iType == iJuQingType)
		{
			pGoTo->setTag(Tag_GoTo_ChuanQi_Btn);
			pGoTo->setVisible(false);
		}
		else
		{
			pGoTo->setTag(Tag_GoTo_ChuanQi_Btn);
			pGoTo->setVisible(false);
		}
	}
}

void TuJianLayer::goToTheLayer(CCObject* pObj)
{
	int iTag = ((CCMenuItemImage*)pObj)->getTag();
	if (iTag == Tag_GoTo_ChuanQi_Btn)
	{
		GameDirector::getDirector()->mainScene(mainlayer_ShangDian);
	}
	else if (iTag == Tag_GoTo_Fighting_Btn)
	{
		MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();

		MapsLayer* mapslayer = new MapsLayer(GoTo_LevelTiaozhen_UI_Type);
		footer->setContaintLayer(mapslayer);
		//装备掉落的关卡
		int iDiaoLuoLevelIndex = EquipFallManager::getManager()->getAlbumEquipFallTaskId(mCur_Info.zb_group_id);
		mapslayer->setWillGoToLevel(iDiaoLuoLevelIndex);

		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);

		footer->release();
		bgLayer->release();
		mapslayer->release();

		CCDirector::sharedDirector()->replaceScene(scene);
	}
}

void TuJianLayer::rsp_Fight(CCObject* pObj)
{
	removeObserver(MSG_hellTaskBattleRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_HellTaskBattleRsp* data = (Flat_HellTaskBattleRsp*)((Flat_MSG_Rsp*)pObj)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	CCDirector::sharedDirector()->getRunningScene()->removeAllChildren();
	CCScene *scene = CCScene::create();
	JiuGongGeLayer *jggLayer = JiuGongGeLayer::create();
	scene->addChild(jggLayer, 0, TAG_JiuGongGe_Layer);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, scene));
}

void TuJianLayer::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_Close)
	{
		if (mBg->getChildByTag(Tag_Board_ZBShengJi) == NULL)
		{
			this->disableAllTouchEnd();
		}
		else 
		{
			CCMenu *menu = (CCMenu*)mBg->getChildByTag(Tag_Board_ZBShengJi)->getChildByTag(Tag_Menu_ZbShengji);
			if (menu) menu->setEnabled(true);
		}
		mBg->removeChildByTag(Tag_Board_EquipDetail);
		MainLayerBase::getCurrentMainBase()->getFooter()->setVisible(true);
	}
	else
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
			mCurTaoZhuang_Info.zb_group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
		mBg->addChild(layer, 20, Tag_Board_TaoZhuangShuxing);
		layer->release();
	}
}

void TuJianLayer::callbackInterface_TaoZhuangLayer()
{
	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_EquipDetail);
	if(board != NULL)
	{
		((CCMenu*)board->getChildByTag(Tag_Menu_EquipDetail))->setEnabled(true);
	}
	mBg->removeChildByTag(Tag_Board_TaoZhuangShuxing);
}

void TuJianLayer::menuItemClicked_DengJiBoardItem(CCObject *pSender)
{
	int  tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_Back)
	{
		mBg->removeChildByTag(Tag_Board_ZBShengJi);
		this->disableAllTouchEnd();
	}
	else
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		int level = tag - Tag_MenuItem_Zb_;
		mCurTaoZhuang_Info.level = level;
		if (ChuanqiPool::getPool()->getChuanQiData(mCurTaoZhuang_Info.zb_group_id,mCurTaoZhuang_Info,level))
		{
			mCur_Info = mCurTaoZhuang_Info;
			displayXiangQing(mCurTaoZhuang_Info);
		}
		else 
			CCLog("Error: %s chuanqiPool have no current equip info", __FUNCTION__);
	}
}


//

void TuJianLayer::menuItemClicked_ListBeforeNext(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	CCPoint offsetPos = mTableViewHeroList->getContentOffset();
	CCSize cellSize = tableCellSizeForIndex(mTableViewHeroList, 0);

	if (tag == Tag_MenuItem_HeroListNext)
	{
		if (offsetPos.x >= -cellSize.width*(numberOfCellsInTableView(mTableViewHeroList)-1-4))
			mTableViewHeroList->setContentOffsetInDuration(ccp(offsetPos.x-cellSize.width, offsetPos.y), 0.25f);
		else 
			mTableViewHeroList->setContentOffsetInDuration(ccp(-cellSize.width*(numberOfCellsInTableView(mTableViewHeroList)-4), offsetPos.y), 0.15f);
	}
	else if (tag == Tag_MenuItem_HeroListBefore)
	{
		if (offsetPos.x <= -cellSize.width)
			mTableViewHeroList->setContentOffsetInDuration(ccp(offsetPos.x+cellSize.width, offsetPos.y), 0.25f);
		else 
			mTableViewHeroList->setContentOffsetInDuration(ccp(0, offsetPos.y), 0.15f);
	}
}

void TuJianLayer::menuItemClicked_HeroClicked(CCObject *pSender)
{
	CCMenuItem* item = (CCMenuItem *)pSender;  //
	int profId = (int)item->getUserData();

	mOldCellIdx = mCurrCellIdx;
	mCurrCellIdx = ((CCTableViewCell*)item->getParent()->getParent())->getIdx();

	addObserver(callfuncO_selector(TuJianLayer::rsp_getEquipAlbumData), MSG_getZhuangbeiAlbumRsp_F);
	mCmdHlp->cmdGetZhuangbeiAlbum(profId);
}



void sortZBInfo(vector<Flat_DuanzaoGroupZBInfo> &vec)
{
	for (int i=0; i<vec.size(); ++i)
	{
		for (int j=i+1; j<vec.size(); ++j)
		{
			if (! sortEquipByZhuangBeiType(vec.at(i), vec.at(j)))
				swap(vec.at(i), vec.at(j));
		}
	}
}

bool sortEquipByZhuangBeiType(const Flat_DuanzaoGroupZBInfo &a, const Flat_DuanzaoGroupZBInfo &b)
{
//  神兵>套装>传奇
// 	同品级装备，等级优先
// 	同等级装备，武器类>护甲类>副手类>首饰类
// 	同类型装备，武器：主角职业优先单手
// 	首饰：护身符>戒指
	if (a.zhuangbei_colour != b.zhuangbei_colour)
	{
		return a.zhuangbei_colour > b.zhuangbei_colour;
	}
	else if (a.level != b.level)  
	{
		return a.level > b.level;
	}
	else
	{
		if (a.zhuangbei_type==ZhuangbeiType_ShuangshouBishou || 
			a.zhuangbei_type==ZhuangbeiType_ShuangshouFu || 
			a.zhuangbei_type==ZhuangbeiType_DanshouFazhang)
		{
			return true;
		}
		else if (a.zhuangbei_type==ZhuangbeiType_DanshouJian || 
			     a.zhuangbei_type==ZhuangbeiType_Gong || 
			     a.zhuangbei_type==ZhuangbeiType_ShuangshouFazhang)
		{
			if (b.zhuangbei_type==ZhuangbeiType_ShuangshouBishou || 
				b.zhuangbei_type==ZhuangbeiType_ShuangshouFu || 
				b.zhuangbei_type==ZhuangbeiType_DanshouFazhang)
				return false;
			else
				return true;
		}
		else if (a.zhuangbei_type==ZhuangbeiType_KuijiaZhong || a.zhuangbei_type==ZhuangbeiType_KuijiaQin)
		{
			if (b.zhuangbei_type==ZhuangbeiType_KuijiaZhong || 
				b.zhuangbei_type==ZhuangbeiType_KuijiaQin || 
				b.zhuangbei_type==ZhuangbeiType_Faqi || 
				b.zhuangbei_type==ZhuangbeiType_Dunpai || 
				b.zhuangbei_type==ZhuangbeiType_Hushenfu ||
				b.zhuangbei_type==ZhuangbeiType_Jiezhi)
				return true;
			else 
				return false;
		}
		else if (a.zhuangbei_type==ZhuangbeiType_Faqi || a.zhuangbei_type==ZhuangbeiType_Dunpai)
		{
			if (b.zhuangbei_type==ZhuangbeiType_Faqi || 
				b.zhuangbei_type==ZhuangbeiType_Dunpai || 
				b.zhuangbei_type==ZhuangbeiType_Hushenfu ||
				b.zhuangbei_type==ZhuangbeiType_Jiezhi)
				return true;
			else 
				return false;
		}
		else if (a.zhuangbei_type==ZhuangbeiType_Hushenfu)
		{
			if (b.zhuangbei_type==ZhuangbeiType_Hushenfu ||
				b.zhuangbei_type==ZhuangbeiType_Jiezhi)
				return true;
			else 
				return false;
		}
		else
		{
			if (b.zhuangbei_type == ZhuangbeiType_Jiezhi) return true;
			else return false;
		}
	}
}
void TuJianLayer::playGuideLayer(CCMenuItem *menuItem)
{
	GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
	CCPoint pos = mBg->convertToWorldSpace(m_tTableViewCell->convertToWorldSpace(m_mGuideLayerMenu->convertToWorldSpace(menuItem->getPosition())));
	pos.y -= 537;
	pos.x -= 20;
	guideLayer->init(CCSizeMake(menuItem->getContentSize().width * menuItem->getScale(),menuItem->getContentSize().height * menuItem->getScale()), 
		pos, LFStrings::getValue("TuJianGuide"), NULL, false);
	CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	m_bIsChatGuide = false;
}