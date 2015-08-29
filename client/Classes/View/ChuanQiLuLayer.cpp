#include "ChuanQiLuLayer.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ImageManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "FX_CommonFunc.h"
#include "BaseActorPropertyParser.h"
#include "MyselfManager.h"
#include "FXScrollMenu.h"
#include "SuiPianManager.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "MapsManager.h"
#include "DebrisSynthesiseLayer.h"
#include "GuideLayerBase.h"

using namespace FX_CommonFunc;

#define Label_Size_Min 20
#define Label_Size_Max 24

enum 
{
	Tag_MenuItem_HeroListBefore,
	Tag_MenuItem_HeroListNext,

	Tag_HeroListMenu,

	Tag_MenuItem_Back,
	Tag_EquipMenu,

	Tag_Board_EquipDetail = 100,
	Tag_Menu_EquipDetail,
	Tag_MenuItem_Close,
	Tag_MenuItem_TaoZhuang,

	Tag_Board_TaoZhuangShuxing = 200,

};

Profession ChuanQiLuLayer::s_ProfId = Profession_GW_Start;

ChuanQiLuLayer::ChuanQiLuLayer()
	:mChallengeMenuItem_forGuide(NULL)
{
	mBg = BaseSprite::create("chuanqilu_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	//equip content
	mEqContentLayer = CCSprite::create();
	mBg->addChild(mEqContentLayer, 1);
	mEqContentLayer->setPosition(ccp(0,0));
	//黑色边框框 ---> 盖在装备table之上， hero table之下
	BaseSprite *heiKuang = BaseSprite::create("chuanqilu_zhuangbei_heikuang.png");
	mBg->addChild(heiKuang, 10);
	heiKuang->setPosition(ccp(320, 838-444));
	//timebar
	BaseSprite *timeBar = BaseSprite::create("chuanqilu_dibu_bg.png");
	mBg->addChild(timeBar, 11);
	timeBar->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-821));
	mTimeLabel = CCLabelTTF::create("", fontStr_BookAntiqua, Label_Size_Min);
	mBg->addChild(mTimeLabel, 12);
	mTimeLabel->setPosition(ccp(576, 839-823));

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
		"chuanqilu_anniu_zuo.png",
		"tujian_anniu_zuo_select.png",
		this, menu_selector(ChuanQiLuLayer::menuItemClicked_ListBeforeNext));
	mHeroListMenu->addChild(beforeItem, 0, Tag_MenuItem_HeroListBefore);
	beforeItem->setPosition(ccp(25, 190-81));
	CCMenuItemImage *nextItem = CCMenuItemImage::create(
		"chuanqilu_anniu_you.png",
		"tujian_anniu_you_select.png",
		this, menu_selector(ChuanQiLuLayer::menuItemClicked_ListBeforeNext));
	mHeroListMenu->addChild(nextItem, 0, Tag_MenuItem_HeroListNext);
	nextItem->setPosition(ccp(607, 190-81));

	//tabel view
	mTableViewHeroList = CCTableView::create(this, CCSizeMake(540, 158));
	mTableViewHeroList->setDirection(kCCScrollViewDirectionHorizontal);
	mTableViewHeroList->setPosition(ccp(46, 16));
	mTableViewHeroList->setDelegate(this);
	heroListBar->addChild(mTableViewHeroList);
//	mTableViewHeroList->reloadData();

	mHeros = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_UnLock);
}

void ChuanQiLuLayer::onEnter()
{
	BaseLayer::onEnter();

	if (s_ProfId >= Profession_GW_Start)//当前登录后 第一次进来  锁定主角
	{
		s_ProfId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
	}

	{
		for (int i=0; i<mHeros.size(); ++i)
		{
			if (mHeros.at(i)->getCurrentProperty().profession == s_ProfId)
			{
				mOldCellIdx = i;
				break;
			}
		}
	}
	mCurrCellIdx = mOldCellIdx;

	addObserver(callfuncO_selector(ChuanQiLuLayer::rsp_QueryChuanqiLu), MSG_queryLegendNoteRsp_F);
	mCmdHlp->cmdQueryLegendNote();

	//mfooter 重置了该layer的pos.y，FXScrollMenu之前穿进去的menuPosition已经不准确了，在这里刷新一下 
	if (mTableViewHeroList) mTableViewHeroList->reloadData();
}

void ChuanQiLuLayer::addDiaoLuo(vector<ShenbingSuipianInfo> vec)
{
	if (vec.size() <= 0) return;

	mVec = vec;
	scheduleOnce(schedule_selector(ChuanQiLuLayer::schedule_displayDiaoLuo), 0);
}

void ChuanQiLuLayer::schedule_displayDiaoLuo(float dt)
{
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(mBg->convertToNodeSpace(ccp(mWinSize.width/2, mWinSize.height/2)));

	//label 提示
	{
		string str = LFStrings::getValue("JiBaiDuiShou") + ", " + LFStrings::getValue("huode") + ":";
		CCLabelTTF* label = CCLabelTTF::create(str.c_str(), 
			fontStr_kaiti, 24, CCSize(435, 0), kCCTextAlignmentCenter);
		label->setAnchorPoint(ccp(0.5, 1));
		label->setPosition(ccp(dialog->getContentSize().width/2, 250));

		dialog->addChild(label, 1);
	}

	//材料
	int label_size = 20;
	ccColor3B label_color = ccWHITE;

	int posIdx = 0;
	int allNum = mVec.size();
	for (int i=0; i<mVec.size(); ++i, ++posIdx)
	{
		Flat_DuanzaoGroupZBInfo zbData;
		ChuanqiPool::getPool()->getChuanQiData(mVec.at(i).zb_group_id, zbData, 5);

		BaseSprite* diwenImg = BaseSprite::create(
			ImageManager::getManager()->getImageData_DiWen_WithBianKuang(ZhuangbeiColour_Chuanqi)); // .count .type
		dialog->addChild(diwenImg);

		int pos_x = dialog->getContentSize().width/2;
		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (diwenImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (diwenImg->getContentSize().width + 10);
		}
		diwenImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
		//equip pic
		string equipfile = zbData.zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *s = BaseSprite::create(equipfile);
		diwenImg->addChild(s);
		s->setScale(0.23f);
		s->setPosition(ccp(diwenImg->getContentSize().width/2,diwenImg->getContentSize().height/2));

		//数量
		string temp = zbData.zhuangbei_name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mVec.at(i).suipian_id); 
		char buf[20];
		sprintf(buf, "\nx%d", 1);
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		diwenImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(diwenImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}

	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);

	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		lfAlert->setAutoDismiss("");
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, 128);
		lfAlert->release();
	}
}

void ChuanQiLuLayer::rsp_QueryChuanqiLu(CCObject *msg_f)
{
	removeObserver(MSG_queryLegendNoteRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryLegendNoteRsp *data = (Flat_QueryLegendNoteRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mTimes = data->times;
	mEquipVec = data->all_profession_zhuangbeis;

	char buf[32];
	sprintf(buf, "%d", mTimes);
	mTimeLabel->setString(buf);

	refreshEquipContentLayer();

	teachGuide();
}

void ChuanQiLuLayer::disableAllTouchBegin()
{
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

	CCArray *chidren = mEqContentLayer->getChildren();
	for (int i=0; i<chidren->count(); ++i)
	{
		BaseSprite *board = dynamic_cast<BaseSprite*>(chidren->objectAtIndex(i));
		if (board)
		{
			CCMenu *menu = dynamic_cast<CCMenu*>(board->getChildByTag(Tag_EquipMenu));
			if (menu) menu->setEnabled(false);
		}
	}
}

void ChuanQiLuLayer::disableAllTouchEnd()
{
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

	CCArray *chidren = mEqContentLayer->getChildren();
	for (int i=0; i<chidren->count(); ++i)
	{
		BaseSprite *board = dynamic_cast<BaseSprite*>(chidren->objectAtIndex(i));
		if (board)
		{
			CCMenu *menu = dynamic_cast<CCMenu*>(board->getChildByTag(Tag_EquipMenu));
			if (menu) menu->setEnabled(true);
		}
	}
}

void ChuanQiLuLayer::callfunc_removeFromParent(CCNode *node)
{
	node->removeFromParent();
	disableAllTouchEnd();
}

CCSize ChuanQiLuLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(139, 158);
}

unsigned int ChuanQiLuLayer::numberOfCellsInTableView(CCTableView *table)
{

	return mHeros.size(); 
}

CCTableViewCell* ChuanQiLuLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
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

		Profession profId = mHeros.at(idx)->getCurrentProperty().profession;

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
			this, menu_selector(ChuanQiLuLayer::menuItemClicked_HeroClicked));
		else 
			item = CCMenuItemImage::create(
			"renxing_renwu_touxiang.png", 
			"renxing_renwu_touxiang.png",
			this, menu_selector(ChuanQiLuLayer::menuItemClicked_HeroClicked));
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

	return cell;
}

void ChuanQiLuLayer::menuItemClicked_zbClicked(CCObject *pSender)
{
	CCMenuItem *item = (CCMenuItem*)pSender;
	Flat_DuanzaoGroupZBInfo* zbInfo = (Flat_DuanzaoGroupZBInfo*)item->getUserData();

	displayXiangQing(*zbInfo);
}

void ChuanQiLuLayer::displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo)
{

	this->disableAllTouchBegin();

	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	mBg->addChild(board, 20, Tag_Board_EquipDetail);
	refreshEquipBoard(board, zbInfo);
	board->setPosition(ccp(mBg->getContentSize().width/2, board->getContentSize().height/2-106));


	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
//	menu->setTouchPriority(-131);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, menu_selector(ChuanQiLuLayer::menuItemClicked_equipDetailMenu));
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
			, this ,menu_selector(ChuanQiLuLayer::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		//taozhuangItem->setEnabled(false);

		mCurTaoZhuang_Info = zbInfo;
	}
}

void ChuanQiLuLayer::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_Close)
	{
		this->disableAllTouchEnd();

		mBg->removeChildByTag(Tag_Board_EquipDetail);
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

void ChuanQiLuLayer::callbackInterface_TaoZhuangLayer()
{
	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_EquipDetail);
	if(board != NULL)
	{
		((CCMenu*)board->getChildByTag(Tag_Menu_EquipDetail))->setEnabled(true);
	}
	mBg->removeChildByTag(Tag_Board_TaoZhuangShuxing);
}

//

void ChuanQiLuLayer::menuItemClicked_ListBeforeNext(CCObject *pSender)
{
	if (mHeros.size() < 4) return;

	int tag = ((CCMenuItem*)pSender)->getTag();

	CCPoint offsetPos = mTableViewHeroList->getContentOffset();
	CCSize cellSize = tableCellSizeForIndex(mTableViewHeroList, 0);

	if (tag == Tag_MenuItem_HeroListNext)
	{
		if (numberOfCellsInTableView(mTableViewHeroList) > 4
			&& offsetPos.x >= -cellSize.width*(numberOfCellsInTableView(mTableViewHeroList)-1-4))
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

void ChuanQiLuLayer::menuItemClicked_HeroClicked(CCObject *pSender)
{
	CCMenuItem* item = (CCMenuItem *)pSender;  //
	int profId = (int)item->getUserData();

	s_ProfId = (Profession)profId;
	mOldCellIdx = mCurrCellIdx;
	mCurrCellIdx = ((CCTableViewCell*)item->getParent()->getParent())->getIdx();

	mTableViewHeroList->updateCellAtIndex(mOldCellIdx);
	mTableViewHeroList->updateCellAtIndex(mCurrCellIdx);

	refreshEquipContentLayer();
}

int ChuanQiLuLayer::getCardPositionIndexByZhuangBeiType(ZhuangbeiType type,bool hasDunpai,bool isFirstJiezhi){
	switch (type)
	{
	case ZhuangbeiType_DanshouJian://单手剑
	case ZhuangbeiType_ShuangshouBishou://双手匕首
	case ZhuangbeiType_DanshouFazhang://单手法杖
	case ZhuangbeiType_Gong://弓
	case ZhuangbeiType_ShuangshouFazhang://双手法杖
	case ZhuangbeiType_ShuangshouFu://双手斧
		return 0;
	case ZhuangbeiType_KuijiaZhong://铠甲重
	case ZhuangbeiType_KuijiaQin://铠甲轻
		return 1;
	case ZhuangbeiType_Faqi://法器
	case ZhuangbeiType_Dunpai://盾牌
		return hasDunpai ? 2 : 1;
	case ZhuangbeiType_Hushenfu://附身符
		return hasDunpai ? 3 : 2;
	case ZhuangbeiType_Jiezhi://戒指
		if(isFirstJiezhi)
			return hasDunpai ? 4 : 3;
		return hasDunpai ? 5 : 4;
	}
	return 0;
}

void ChuanQiLuLayer::refreshEquipContentLayer()
{
	Profession prof = mHeros.at(mCurrCellIdx)->getCurrentProperty().profession;

	mEqContentLayer->removeAllChildren();

	int count = mEquipVec.at(prof).size();
	int i;
	bool isViewJiezhi = false;
	for (i=0; i<count; ++i)
	{
		ShenbingSuipianGroupData curEqSuiPian;
		SuiPianManager::getManager()->getShenBingSuiPianNumByGroupId(mEquipVec.at(prof).at(i).zb_group_id, curEqSuiPian);
		int hadNum = 0;
		for (int id=1; id!=MAX_SUIPIAN_ID_NUM+1; ++id)  // 1 - 5
		{
			if (curEqSuiPian.count_array[id] > 0)
				++ hadNum;
		}
		bool hasDun = false;
		int j;
		for (j=0; j<count; ++j){
			if(mEquipVec.at(prof).at(j).zhuangbei_type == ZhuangbeiType_Dunpai || mEquipVec.at(prof).at(j).zhuangbei_type == ZhuangbeiType_Faqi){
				hasDun = true;
				break;
			}
		}
		if(mEquipVec.at(prof).at(i).zhuangbei_type == ZhuangbeiType_Jiezhi)
			isViewJiezhi = !isViewJiezhi;

		BaseSprite *board = BaseSprite::create("chuanqilu_zhuangbei_bg.png");
		mEqContentLayer->addChild(board);
		board->setPosition(getPositionByIdx(getCardPositionIndexByZhuangBeiType(mEquipVec.at(prof).at(i).zhuangbei_type,hasDun,isViewJiezhi), 6));
		CCMenu *menu = CCMenu::create();
		board->addChild(menu, 1, Tag_EquipMenu);
		menu->setPosition(ccp(0,0));
		// diwen 
		string diwenStr = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			ZhuangbeiColour_Chuanqi, false, false).getCurrentImageFile();
		CCMenuItemImage *diwenItem = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str()
			, this, menu_selector(ChuanQiLuLayer::menuItemClicked_zbClicked));
		menu->addChild(diwenItem);
		diwenItem->setScale(0.32f);
		diwenItem->setUserData((void*)&mEquipVec.at(prof).at(i));
		diwenItem->setPosition(ccp(92, 270-116));
		//equip
		string equipfile = mEquipVec.at(prof).at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *eqImg = BaseSprite::create(equipfile);
		diwenItem->addChild(eqImg);
		eqImg->setPosition(ccp(diwenItem->getContentSize().width/2,diwenItem->getContentSize().height/2));
		//challenge
		CCMenuItemImage *challengeItem = NULL;
		if (hadNum < 5)
		{
			challengeItem = CCMenuItemImage::create(
				"chuanqilu_anniu_tiaozhan.png",
				"chuanqilu_anniu_tiaozhan_select.png",
				this, menu_selector(ChuanQiLuLayer::menuItemClicked_Challenge));
		}
		else
		{
			challengeItem = CCMenuItemImage::create(
				"chuanqilu_anniu_hecheng.png",
				"chuanqilu_anniu_hecheng_select.png",
				this, menu_selector(ChuanQiLuLayer::menuItemClicked_HeCheng));
		}

		//guide
		{
			if (g_ChuanQiLu_Opend && i == 0)
				mChallengeMenuItem_forGuide = challengeItem;
		}

		menu->addChild(challengeItem);
		challengeItem->setPosition(ccp(92, 270-214));
		challengeItem->setUserData((void*)&mEquipVec.at(prof).at(i));
		//name
		CCLabelTTF *namelabel = CCLabelTTF::create(mEquipVec.at(prof).at(i).zhuangbei_name.c_str(), fontStr_kaiti, 20);
		board->addChild(namelabel);
		namelabel->setColor(ccBLACK);
		namelabel->setPosition(ccp(92, 20));
		//num
		char buf[32];
		sprintf(buf, "%d/%d", hadNum, MAX_SUIPIAN_ID_NUM);
		CCLabelTTF *numLa = CCLabelTTF::create(buf, fontStr_BookAntiqua, Label_Size_Min);
		board->addChild(numLa);
		numLa->setPosition(ccp(92, 270-19));
	}
}

CCPoint ChuanQiLuLayer::getPositionByIdx(int idx, int allCount)
{
	CCPoint pos;

	if (allCount <= 3)
	{
		pos.y = 839-535;
		if (allCount == 3) 
		{
			if (idx == 0) pos.x = 320-197;
			else if (idx == 1) pos.x = 320;
			else if (idx == 2) pos.x = 320 + 197;
		}
		else if (allCount == 2) 
		{
			if (idx == 0) pos.x = 320-197/2;
			else if (idx == 1) pos.x = 320+197/2;
		}
		else if (allCount == 1)
		{
			pos.x = 320;
		}
	}
	else if (allCount <= 6)
	{
		if (idx < 3)//第一排
			pos.y = 839-400;
		else
			pos.y = 839 - 669;

		if (idx == 0 || idx == 3) pos.x = 320-197;
		else if (idx == 1 || idx == 4) pos.x = 320;
		else if (idx == 2 || idx == 5) pos.x = 320+197;
		//
		if (allCount == 5)
		{
			if (idx == 3) pos.x = 320 - 197/2;
			if (idx == 4) pos.x = 320 + 197/2;
		}
		else if (allCount == 4 && idx == 3) pos.x = 320;
	}
	else
	{
		CCAssert(false, "> 6");
	}

	return pos;
}

void ChuanQiLuLayer::menuItemClicked_HeCheng(CCObject *pSender)
{
	DebrisSynthesiseLayer *layer = DebrisSynthesiseLayer::create();
	mFooterLayer->setContaintLayer(layer);
}

void ChuanQiLuLayer::menuItemClicked_Challenge(CCObject *pSender)
{
	Flat_DuanzaoGroupZBInfo *zbInfo = (Flat_DuanzaoGroupZBInfo*)((CCMenuItem*)pSender)->getUserData();

	addObserver(callfuncO_selector(ChuanQiLuLayer::rsp_Challenge), MSG_challengeLegendNoteRsp_f);
	mCmdHlp->cmdChallengeLegendNote(zbInfo->zb_group_id);
}

void ChuanQiLuLayer::rsp_Challenge(CCObject *msg_f)
{
	removeObserver(MSG_challengeLegendNoteRsp_f);
	GameDirector::getDirector()->hideWaiting();

	Flat_ChallengeLegendNoteRsp *data = (Flat_ChallengeLegendNoteRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","ChuanQiLu");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		,MapsManager::getManager()->getTuanDuiFuBenMapImage(),bundle);
}

void ChuanQiLuLayer::teachGuide()
{
	if (g_ChuanQiLu_Opend)
	{
		ChatLayer* layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQiLu_ZhenZhang_ZhiYin"), this);
		if(layer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);
		}
	}
	else if (g_ChuanQiLu_Guide)
	{
//		scheduleOnce(schedule_selector(ChuanQiLuLayer::schedule_guide), 0);


		//暂时不要合成、 
		g_ChuanQiLu_Guide = false;
	}
}

void ChuanQiLuLayer::schedule_guide(float dt)
{
	ChatLayer* layer = ChatLayer::create(
		ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQiLu_ZhenZhang_suipian"), this);
	if(layer != NULL)
	{
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);
	}
}

void ChuanQiLuLayer::chatEndCallBack()
{
	if (g_ChuanQiLu_Opend)
	{
		g_ChuanQiLu_Opend = false;
		g_ChuanQiLu_Guide = true;
		CCMenuItem *item = mChallengeMenuItem_forGuide;
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(item->getContentSize(),
			item->getParent()->convertToWorldSpace(item->getPosition()),
			LFStrings::getValue("ChuanQiLu_challenge"), NULL, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
	else if (g_ChuanQiLu_Guide)
	{
		g_ChuanQiLu_Guide = false;
		g_ChuanQiSuiPianHeCheng_Guide = true;
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
}