#include "MapsLayer_DiaoLuoChaXun.h"
#include "font.h"
#include "ChatContentManager.h"
#include "MapsLayer.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ChatContentManager.h"
#include "ImageManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "SkillTree.h"
#include "CS_Common.h"
#include "EquipmentManager.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;


enum 
{
	Tag_Label_ZhangJie = 1,

	Tag_MainMenu_FanHui,
	Tag_MainMenu_XiaYiGe,
	Tag_MainMenu_ShangYiGe,

	Tag_Label_TaskName,
	Tag_Label_Zhuangtai,
	Tag_Label_Tishi,
	Tag_ZheGai_img,

	Tag_ChaXunMenu_1,

	Tag_Board_ZB_Display = 50,
	Tag_BoardMenu,
	Tag_Board_zb_Or_dj_Menu,
	Tag_MenuItem_Close,
	Tag_MenuItem_ZhuangBei,
	Tag_MenuItem_DaoJu,

	Tag_Board_EquipDetail = 60,
	Tag_Menu_EquipDetail,
	Tag_MenuItem_EquipDetail_Close,
	Tag_MenuItem_TaoZhuang,

	Tag_Board_TaoZhuangShuxing,
	Tag_MenuItem_TaoZhuang_Fanhui,
};


MapsLayer_DiaoLuoChaXun* MapsLayer_DiaoLuoChaXun::create(int curZhangJie, MapsLayer *callbackLayer, int curDifficulty)
{
	MapsLayer_DiaoLuoChaXun *layer = new MapsLayer_DiaoLuoChaXun;
	if(layer && layer->init(curZhangJie, callbackLayer, curDifficulty))
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

bool MapsLayer_DiaoLuoChaXun::init(int zhangjie, MapsLayer *callbackLayer, int curDifficulty)
{
	mCurDifficulty = curDifficulty;
	mCallbackLayer = callbackLayer;
	mCurZhangJie = zhangjie;

	mBg = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);

	mChaXunMenu = CCMenu::create();
	mBg->addChild(mChaXunMenu);
	mChaXunMenu->setPosition(CCPointZero);

	//main menu
	{
		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_MainMenu));
		mMainMenu->addChild(fanhui, 1, Tag_MainMenu_FanHui);
		fanhui->setPosition(ccp(mBg->getContentSize().width/2, 723-660));
		CCMenuItemImage *xiayige = CCMenuItemImage::create(
			"zhandou_ditu_diaoluochaxun_you.png"
			, "zhandou_ditu_diaoluochaxun_you_select.png"
			, "zhandou_ditu_diaoluochaxun_you_select.png"
			, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_MainMenu));
		mMainMenu->addChild(xiayige, 1, Tag_MainMenu_XiaYiGe);
		xiayige->setPosition(ccp(551, 723-113));
		CCMenuItemImage *shangyige = CCMenuItemImage::create(
			"zhandou_ditu_diaoluochaxun_zuo.png"
			, "zhandou_ditu_diaoluochaxun_zuo_select.png"
			, "zhandou_ditu_diaoluochaxun_you_select.png"
			, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_MainMenu));
		mMainMenu->addChild(shangyige, 1, Tag_MainMenu_ShangYiGe);
		shangyige->setPosition(ccp(58, 723-113));
	}
	//chaxun menu
	{
		float start_x = 306, start_y = 552;
		float delta = 45.5;
		for(int i=0; i!=10; ++i)
		{
			CCMenuItemImage *item = CCMenuItemImage::create(
				"zhandou_ditu_diaoluochaxun_xuanzerenwu_yitongguan.png"
				, "zhandou_ditu_diaoluochaxun_xuanzerenwu_jinxingzhong.png"
				, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_XuanZeMenu));
			int taskId = 1;
			if (mCurDifficulty == difficult_normal) taskId = mCurZhangJie*10-9 + i;
			else if (mCurDifficulty ==difficult_hard) taskId = mCurZhangJie*10-9 + i + HellTaskId_Min - 1;
			mChaXunMenu->addChild(item, 0, Tag_ChaXunMenu_1 + i);
			item->setPosition(ccp(start_x, start_y - delta*i));
			item->setUserData((void*)taskId);

			//label   task name
			string taskName = ChatContentManager::sharedChatContentManager()->getMainTaskNameById(taskId);
			CCLabelTTF *labelName = CCLabelTTF::create(taskName.c_str(), fontStr_kaiti, 20);
			item->addChild(labelName, 5, Tag_Label_TaskName);
			labelName->setAnchorPoint(ccp(0, 0.5));
			labelName->setPosition(ccp(32, item->getContentSize().height/2));
			labelName->setColor(ccc3(195, 145, 96));
			//label   zhuangtai
			CCLabelTTF *zhuangtaiLabel = CCLabelTTF::create(LFStrings::getValue("yitongguan").c_str(), fontStr_kaiti, 20);
			item->addChild(zhuangtaiLabel, 5, Tag_Label_Zhuangtai);
			zhuangtaiLabel->setPosition(ccp(341, item->getContentSize().height/2));
			zhuangtaiLabel->setColor(ccc3(195, 145, 96));
			// tishi
			CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("dianjichaxun").c_str(), fontStr_kaiti, 20);
			item->addChild(tishiLabel, 5, Tag_Label_Tishi);
			tishiLabel->setAnchorPoint(ccp(1, 0.5));
			tishiLabel->setPosition(ccp(532, item->getContentSize().height/2));
			tishiLabel->setColor(ccc3(195, 145, 96));

			int curId = mCallbackLayer->getMainTaskId();
			if(curId == taskId)
			{
				labelName->setColor(ccWHITE);
				zhuangtaiLabel->setString(LFStrings::getValue("jinxingzhong").c_str());
				zhuangtaiLabel->setColor(ccWHITE);
				tishiLabel->setColor(ccWHITE);
			}
			else if(taskId > curId)
			{
				labelName->setColor(ccWHITE);
				zhuangtaiLabel->setString(LFStrings::getValue("weijiesuo").c_str());
				zhuangtaiLabel->setColor(ccWHITE);
				tishiLabel->setColor(ccWHITE);

				BaseSprite *img = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_select.png");
				item->addChild(img, 10, Tag_ZheGai_img);
				img->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
			}
		}
	}

	//
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangJie);
	string title = LFStrings::getValue(buf);
	sprintf(buf, "zhangjieName%d", mCurZhangJie);
	title = title + "  " + LFStrings::getValue(buf);
	CCLabelTTF *titleLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 24);
	mBg->addChild(titleLabel, 1, Tag_Label_ZhangJie);
	titleLabel->setColor(ccc3(195, 145, 96));
	titleLabel->setPosition(ccp(mBg->getContentSize().width/2, 723-115));


// 	mTableView = CCTableView::create(this, CCSizeMake(564, 590));
// 	mTableView->setDirection(kCCScrollViewDirectionVertical);
// 	mTableView->setPosition(ccp(22, 18));
// 	mTableView->setDelegate(this);
// 	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
// 	mBg->addChild(mTableView);

	return true;
}
/*
void MapsLayer_DiaoLuoChaXun::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{

}

CCTableViewCell* MapsLayer_DiaoLuoChaXun::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();

	cell = new CCTableViewCell();
	cell->autorelease();
	//
	CCMenu *menu = CCMenu::create();
	cell->addChild(menu);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *item = CCMenuItemImage::create(
		"zhandou_ditu_diaoluochaxun_xuanzerenwu_yitongguan.png"
		, "zhandou_ditu_diaoluochaxun_xuanzerenwu_jinxingzhong.png"
		, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_XuanZeMenu));
	int taskId = mCurZhangJie*10-9 + idx;
	menu->addChild(item, 0, taskId);
	CCSize size = tableCellSizeForIndex(table, idx);
	item->setPosition(ccp(size.width/2, size.height/2));

	//label   task name
	string taskName = ChatContentManager::sharedChatContentManager()->getMainTaskNameById(taskId);
	CCLabelTTF *labelName = CCLabelTTF::create(taskName.c_str(), fontStr_kaiti, 20);
	cell->addChild(labelName, 5);
	labelName->setAnchorPoint(ccp(0, 0.5));
	labelName->setPosition(ccp(32, size.height/2));
	labelName->setColor(ccc3(195, 145, 96));
	//label   zhuangtai
	CCLabelTTF *zhuangtaiLabel = CCLabelTTF::create(LFStrings::getValue("yitongguan").c_str(), fontStr_kaiti, 20);
	cell->addChild(zhuangtaiLabel, 5);
	zhuangtaiLabel->setPosition(ccp(341, size.height/2));
	zhuangtaiLabel->setColor(ccc3(195, 145, 96));
	// tishi
	CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("dianjichaxun").c_str(), fontStr_kaiti, 20);
	cell->addChild(tishiLabel, 5);
	tishiLabel->setAnchorPoint(ccp(1, 0.5));
	tishiLabel->setPosition(ccp(532, size.height/2));
	tishiLabel->setColor(ccc3(195, 145, 96));

	int curId = mCallbackLayer->getMainTaskId();
	if(curId == idx)
	{
		labelName->setColor(ccWHITE);
		zhuangtaiLabel->setString(LFStrings::getValue("jinxingzhong").c_str());
		zhuangtaiLabel->setColor(ccWHITE);
		tishiLabel->setColor(ccWHITE);
	}
	else if(idx > curId)
	{
		labelName->setColor(ccWHITE);
		zhuangtaiLabel->setString(LFStrings::getValue("weijiesuo").c_str());
		zhuangtaiLabel->setColor(ccWHITE);
		tishiLabel->setColor(ccWHITE);

		BaseSprite *img = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_select.png");
		cell->addChild(img, 10);
		img->setPosition(ccp(size.width/2, size.height/2));
	}

	return cell;
}
*/

void MapsLayer_DiaoLuoChaXun::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MainMenu_FanHui)
	{
		mCallbackLayer->disableAllTouchEnd();
		this->removeFromParent();
		return;
	}
	else if(tag == Tag_MainMenu_ShangYiGe)
	{
		if(mCurZhangJie <= 1)  return;
		--mCurZhangJie;
	}
	else if(tag == Tag_MainMenu_XiaYiGe)
	{
		if(mCurZhangJie >= 4) return;
		++mCurZhangJie;
	}

	//label zhangjie
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangJie);
	string title = LFStrings::getValue(buf);
	sprintf(buf, "zhangjieName%d", mCurZhangJie);
	title = title + "  " + LFStrings::getValue(buf);
	CCLabelTTF *titleLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_ZhangJie);
	titleLabel->setString(title.c_str());
//	titleLabel->setColor(ccc3(195, 145, 96));
//	titleLabel->setPosition(ccp(mBg->getContentSize().width/2, 723-115));
	// item
	refreshChaXunItem();
}

void MapsLayer_DiaoLuoChaXun::refreshChaXunItem()
{
	for (int i=0; i!=10; ++i)
	{
		CCMenuItemImage *item = (CCMenuItemImage*)mChaXunMenu->getChildByTag(Tag_ChaXunMenu_1 + i);
		int taskId = mCurZhangJie*10-9 + i;
		item->setUserData((void*)taskId);
//		item->removeAllChildren();
		CCLabelTTF *labelName = (CCLabelTTF*)item->getChildByTag(Tag_Label_TaskName);
		CCLabelTTF *zhuangtaiLabel = (CCLabelTTF*)item->getChildByTag(Tag_Label_Zhuangtai);
		CCLabelTTF *tishiLabel = (CCLabelTTF*)item->getChildByTag(Tag_Label_Tishi);
		item->removeChildByTag(Tag_ZheGai_img);

		//label   task name
		string taskName = ChatContentManager::sharedChatContentManager()->getMainTaskNameById(taskId);
// 		CCLabelTTF *labelName = CCLabelTTF::create(taskName.c_str(), fontStr_kaiti, 20);
// 		item->addChild(labelName, 5, Tag_Label_TaskName);
// 		labelName->setAnchorPoint(ccp(0, 0.5));
// 		labelName->setPosition(ccp(32, item->getContentSize().height/2));
		labelName->setString(taskName.c_str());
		labelName->setColor(ccc3(195, 145, 96));
		//label   zhuangtai
// 		CCLabelTTF *zhuangtaiLabel = CCLabelTTF::create(LFStrings::getValue("yitongguan").c_str(), fontStr_kaiti, 20);
// 		item->addChild(zhuangtaiLabel, 5, Tag_Label_Zhuangtai);
// 		zhuangtaiLabel->setPosition(ccp(341, item->getContentSize().height/2));
		zhuangtaiLabel->setString(LFStrings::getValue("yitongguan").c_str());
		zhuangtaiLabel->setColor(ccc3(195, 145, 96));
		// tishi
// 		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("dianjichaxun").c_str(), fontStr_kaiti, 20);
// 		item->addChild(tishiLabel, 5, Tag_Label_Tishi);
// 		tishiLabel->setAnchorPoint(ccp(1, 0.5));
// 		tishiLabel->setPosition(ccp(532, item->getContentSize().height/2));
		tishiLabel->setString(LFStrings::getValue("dianjichaxun").c_str());
		tishiLabel->setColor(ccc3(195, 145, 96));

		int curId = mCallbackLayer->getMainTaskId();
		if(curId == taskId)
		{
			labelName->setColor(ccWHITE);
			zhuangtaiLabel->setString(LFStrings::getValue("jinxingzhong").c_str());
			zhuangtaiLabel->setColor(ccWHITE);
			tishiLabel->setColor(ccWHITE);
		}
		else if(taskId > curId)
		{
			labelName->setColor(ccWHITE);
			zhuangtaiLabel->setString(LFStrings::getValue("weijiesuo").c_str());
			zhuangtaiLabel->setColor(ccWHITE);
			tishiLabel->setColor(ccWHITE);

			BaseSprite *img = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_select.png");
			item->addChild(img, 10, Tag_ZheGai_img);
			img->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
		}

	}
}

void MapsLayer_DiaoLuoChaXun::menuItemClicked_XuanZeMenu(CCObject *pSender)
{
//	int tag = ((CCMenuItemImage*)pSender)->getTag();
	int taskId = (int)((CCMenuItemImage*)pSender)->getUserData();

	mCurTask = taskId;

	addObserver(callfuncO_selector(MapsLayer_DiaoLuoChaXun::diaoluChaXun_Rsp), MSG_QueryFubenDiaoluoRsp_F);
	mCmdHlp->cmdQueryFubenDiaoluo(taskId);
}

void MapsLayer_DiaoLuoChaXun::diaoluChaXun_Rsp(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_QueryFubenDiaoluoRsp_F);

	Flat_QueryFubenDiaoluoRsp* data = (Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	mCurTaskData = *data;
	{
		this->disableAllTouchBegin();
		//display window
		BaseSprite *board = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_bg.png");
		mBg->addChild(board, 20, Tag_Board_ZB_Display);
		board->setPosition(ccp(306, 723-343));

		//main menu
		CCMenu *menu = CCMenu::create();
		board->addChild(menu, 1, Tag_BoardMenu);
		menu->setPosition(CCPointZero);
		//关闭按钮
		CCMenuItemImage* closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		closeItem->setPosition(ccp(board->getContentSize().width-closeItem->getContentSize().width*0.8
			, board->getContentSize().height-closeItem->getContentSize().height*0.66));
		menu->addChild(closeItem, 1, Tag_MenuItem_Close);
		//task name
		string title = LFStrings::getValue("renwuChaxun") + ChatContentManager::sharedChatContentManager()->getMainTaskNameById(mCurTask);
		CCLabelTTF *labelTaskName = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 23);
		board->addChild(labelTaskName);
		labelTaskName->setPosition(ccp(board->getContentSize().width/2, 490));
		labelTaskName->setColor(ccc3(195, 145, 96));

		////////////////////////////////////////////////////////////////////////////////////
		int Label_Size = 20;
		ccColor3B Label_Color = ccc3(195, 145, 96);
		// 装备 道具 menu
		CCMenu *zb_djMenu = CCMenu::create();
		board->addChild(zb_djMenu, 1, Tag_Board_zb_Or_dj_Menu);
		zb_djMenu->setPosition(CCPointZero);
		//1. 装备
		if(mCurTaskData.diaoluo_vec.size() > 0)
		{
			int start_Y = 524-196;
			int start_X = 60;
			int delta_y = 205;
			int delta_x = 114;
			float scale = 0.23f;
			int num = mCurTaskData.diaoluo_vec.size();

			for(int i=0; i!=num; ++i)
			{
				//diwen
				string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
					(ZhuangbeiColour)mCurTaskData.diaoluo_vec.at(i).zhuangbei_colour, 
					false,//ChuanqiPool::getPool()->isShenBing(mCurTaskData.diaoluo_vec.at(i).group_id), 
					SuitPool::getPool()->isSuit(mCurTaskData.diaoluo_vec.at(i).group_id));
				//			BaseSprite *diwen = BaseSprite::create(cardFile);
				CCMenuItemImage *diwen = CCMenuItemImage::create(
					cardFile.c_str(),
					cardFile.c_str(),
					this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_zbClicked));
				diwen->setUserData((void*)i);

				zb_djMenu->addChild(diwen);
				int posX = start_X + 2*delta_x;
				if(i < (num+1)/2)
				{// 第一排
					int firstCount = (num+1)/2;
					posX += (i - firstCount/2)*delta_x;
					if(firstCount % 2 == 0)
						posX += delta_x / 2;
				}
				else
				{// 第二排
					int secondCount = num - (num+1)/2;
					posX += (i-(num+1)/2-secondCount/2)*delta_x;
					if(secondCount % 2 == 0)
						posX += delta_x / 2;
				}
				diwen->setPositionX(posX);
				diwen->setPositionY(start_Y - delta_y*(i/((num+1)/2)) );
				//equip
				string equipfile = mCurTaskData.diaoluo_vec.at(i).zhuangbei_pic;
				equipfile = "minute_" + equipfile + ".png";
				BaseSprite *equipImg = BaseSprite::create(equipfile);
				diwen->addChild(equipImg, 10);
				equipImg->setScale(scale);
				equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
				//name label
				string name = mCurTaskData.diaoluo_vec.at(i).zhuangbei_name;
				CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size, CCSize(96, 0), kCCTextAlignmentCenter);
				diwen->addChild(labelName, 10);
				labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
				ccColor3B namecolor = ImageManager::getManager()->getShowColor((ZhuangbeiColour)mCurTaskData.diaoluo_vec.at(i).zhuangbei_colour
					, false
					, SuitPool::getPool()->isSuit(mCurTaskData.diaoluo_vec.at(i).group_id));
				labelName->setColor(namecolor);
			}
		}
		//2. 道具
		if(mCurTaskData.items.size() > 0)
		{
			int start_Y = -42;
			int start_X = 60;
			int delta_x = 114;
			float scale = 0.7f;
			int num = mCurTaskData.items.size();

			for(int i=0; i!=num; ++i)
			{
				//board
				CCMenuItemImage *diwen = CCMenuItemImage::create(
					"daoju_kuang_bg.png",
					"daoju_kuang_bg.png",
					this, NULL);
				diwen->setUserData((void*)i);

				zb_djMenu->addChild(diwen);
				int posX = start_X + 2*delta_x;
				{
					posX += (i - num/2)*delta_x;
					if(num % 2 == 0)
						posX += delta_x / 2;
				}
				diwen->setPositionX(posX);
				diwen->setScale(scale);
				diwen->setPositionY(start_Y);
				//daoju
				string djFile = ImageManager::getManager()->getItemFilename(mCurTaskData.items.at(i));
				BaseSprite *djImg = BaseSprite::create(djFile);
				diwen->addChild(djImg, 10);
				djImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
				//name label
				string name;
				if(mCurTaskData.items.at(i) > ItemsType_SkillBook_MinId && mCurTaskData.items.at(i) < ItemsType_SkillBook_MaxId)
					name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(mCurTaskData.items.at(i) - ItemsType_SkillBook_MinId));
				else 
					name = CS::getItemName(mCurTaskData.items.at(i));
				CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size+3, CCSize(0, 0), kCCTextAlignmentCenter);
				diwen->addChild(labelName, 10);
				labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
				labelName->setColor(ccc3(127,127,127));
			}
		}
	}
}

void MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_Close)
	{
		this->disableAllTouchEnd();
		mBg->getChildByTag(Tag_Board_ZB_Display)->removeFromParent();
		return;
	}
}

void MapsLayer_DiaoLuoChaXun::refresh_ChaXunZB_ZhuangBei(BaseSprite* board, vector<Flat_FubenDiaoluoZBInfo>& diaoluoVec)
{
}

void MapsLayer_DiaoLuoChaXun::refresh_ChaXunZB_DaoJu(BaseSprite* board, vector<ItemsType>& daojuVec)
{
}

/*
void MapsLayer_DiaoLuoChaXun::diaoluChaXun_Rsp(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_QueryFubenDiaoluoRsp_F);

	mCurTaskData = *(Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(mCurTaskData.err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create();
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	{
		this->disableAllTouchBegin();
		//display window
		BaseSprite *board = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_bg.png");
		mBg->addChild(board, 20, Tag_Board_ZB_Display);
		board->setPosition(ccp(306, 723-343));

		//main menu
		CCMenu *menu = CCMenu::create();
		board->addChild(menu, 1, Tag_BoardMenu);
		menu->setPosition(CCPointZero);
		//关闭按钮
		CCMenuItemImage* closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		closeItem->setPosition(ccp(board->getContentSize().width-closeItem->getContentSize().width*0.8
			, board->getContentSize().height-closeItem->getContentSize().height*0.66));
		menu->addChild(closeItem, 1, Tag_MenuItem_Close);
		//装备
		CCMenuItemImage* zbItem = CCMenuItemImage::create(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei_select.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei.png",
			this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		zbItem->setPosition(ccp(149, 523-88));
		menu->addChild(zbItem, 1, Tag_MenuItem_ZhuangBei);
		//道具
		CCMenuItemImage* djItem = CCMenuItemImage::create(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju_select.png",
			this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		djItem->setPosition(ccp(431, 523-88));
		menu->addChild(djItem, 1, Tag_MenuItem_DaoJu);

		//task name
		string title = LFStrings::getValue("renwuChaxun") + ChatContentManager::sharedChatContentManager()->getMainTaskNameById(mCurTask);
		CCLabelTTF *labelTaskName = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 23);
		board->addChild(labelTaskName);
		labelTaskName->setPosition(ccp(board->getContentSize().width/2, 490));
		labelTaskName->setColor(ccc3(195, 145, 96));

		// 装备 道具 menu
		CCMenu *zb_djMenu = CCMenu::create();
		board->addChild(zb_djMenu, 1, Tag_Board_zb_Or_dj_Menu);
		zb_djMenu->setPosition(CCPointZero);

		menuItemClicked_ChaXunZB(zbItem);
	}
}

void MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_Close)
	{
		this->disableAllTouchEnd();
		mBg->getChildByTag(Tag_Board_ZB_Display)->removeFromParent();
		return;
	}
		

	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZB_Display);
	CCMenu *menu = (CCMenu*)board->getChildByTag(Tag_BoardMenu);
	CCMenuItemImage *djItem = (CCMenuItemImage *)menu->getChildByTag(Tag_MenuItem_DaoJu);
	CCMenuItemImage *zbItem = (CCMenuItemImage *)menu->getChildByTag(Tag_MenuItem_ZhuangBei);
	((CCMenu*)board->getChildByTag(Tag_Board_zb_Or_dj_Menu))->removeAllChildren();
	if(tag == Tag_MenuItem_ZhuangBei)
	{
		djItem->initWithNormalImage(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju_select.png",
			NULL,this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		zbItem->initWithNormalImage(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei_select.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei.png",
			NULL, this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));

		refresh_ChaXunZB_ZhuangBei(board, mCurTaskData.diaoluo_vec);
	}
	else if(tag == Tag_MenuItem_DaoJu)
	{
		djItem->initWithNormalImage(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju_select.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_daoju.png",
			NULL,this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));
		zbItem->initWithNormalImage(
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei.png",
			"zhandou_ditu_diaoluochaxun_xuanzerenwu_chakandiaoluo_anniu_zhuangbei_select.png",
			NULL, this,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_ChaXunZB));

		refresh_ChaXunZB_DaoJu((BaseSprite*)mBg->getChildByTag(Tag_Board_ZB_Display), mCurTaskData.items);
	}
}

void MapsLayer_DiaoLuoChaXun::refresh_ChaXunZB_ZhuangBei(BaseSprite* board, vector<Flat_FubenDiaoluoZBInfo>& diaoluoVec)
{
	CCMenu *zbMenu = (CCMenu*)board->getChildByTag(Tag_Board_zb_Or_dj_Menu);

	int Label_Size = 20;
	ccColor3B Label_Color = ccc3(195, 145, 96);
	
	if(diaoluoVec.size() > 0)
	{
		int start_Y = 524-196;
		int start_X = 60;
		int delta_y = 205;
		int delta_x = 114;
		float scale = 0.23;
		int num = diaoluoVec.size();

		for(int i=0; i!=num; ++i)
		{
			//diwen
			string cardFile = "";
			if (SuitPool::getPool()->isSuit(diaoluoVec.at(i).group_id))
			{
				cardFile = "zhandou_jiesuan_zhuangbei_chuanqi_taozhuang.png";
			}else
			{
				switch (diaoluoVec.at(i).zhuangbei_colour)
				{
				case ZhuangbeiColour_Baizhuang:
					cardFile = "zhandou_jiesuan_zhuangbei_putong.png";
					break;
				case ZhuangbeiColour_Fumo:
					cardFile = "zhandou_jiesuan_zhuangbei_mofa.png";
					break;
				case ZhuangbeiColour_Xiyou:
					cardFile = "zhandou_jiesuan_zhuangbei_xiyou.png";
					break;
				case ZhuangbeiColour_Chuanqi:
					cardFile = "zhandou_jiesuan_zhuangbei_chuanqi.png";
					break;
				default:
					CCAssert(false,"");
					break;
				}
			}
//			BaseSprite *diwen = BaseSprite::create(cardFile);
			CCMenuItemImage *diwen = CCMenuItemImage::create(
				cardFile.c_str(),
				cardFile.c_str(),
				this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_zbClicked));
			diwen->setUserData((void*)i);
			
			zbMenu->addChild(diwen);
			int posX = start_X + 2*delta_x;
			if(i < (num+1)/2)
			{// 第一排
				int firstCount = (num+1)/2;
				posX += (i - firstCount/2)*delta_x;
				if(firstCount % 2 == 0)
					posX += delta_x / 2;
			}
			else
			{// 第二排
				int secondCount = num - (num+1)/2;
				posX += (i-(num+1)/2-secondCount/2)*delta_x;
				if(secondCount % 2 == 0)
					posX += delta_x / 2;
			}
			diwen->setPositionX(posX);
			diwen->setPositionY(start_Y - delta_y*(i/((num+1)/2)) );
			//equip
			string equipfile = diaoluoVec.at(i).zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			diwen->addChild(equipImg, 10);
			equipImg->setScale(scale);
			equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
			//name label
			string name = diaoluoVec.at(i).zhuangbei_name;
			CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size, CCSize(96, 0), kCCTextAlignmentCenter);
			diwen->addChild(labelName, 10);
			labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
			ccColor3B namecolor = ImageManager::getManager()->getShowColor((ZhuangbeiColour)diaoluoVec.at(i).zhuangbei_colour
				, SuitPool::getPool()->isSuit(diaoluoVec.at(i).group_id));
			labelName->setColor(namecolor);
		}
	}
}

void MapsLayer_DiaoLuoChaXun::refresh_ChaXunZB_DaoJu(BaseSprite* board, vector<ItemsType>& daojuVec)
{
	CCMenu *djMenu = (CCMenu*)board->getChildByTag(Tag_Board_zb_Or_dj_Menu);

	int Label_Size = 20;
	ccColor3B Label_Color = ccc3(195, 145, 96);

	if(daojuVec.size() > 0)
	{
		int start_Y = 524-196;
		int start_X = 60;
		int delta_y = 205;
		int delta_x = 114;
		float scale = 0.23;
		int num = daojuVec.size();

		for(int i=0; i!=num; ++i)
		{
			//board
			CCMenuItemImage *diwen = CCMenuItemImage::create(
				"daoju_kuang_bg.png",
				"daoju_kuang_bg.png",
				this, NULL);
			diwen->setUserData((void*)i);

			djMenu->addChild(diwen);
			int posX = start_X + 2*delta_x;
			if(i < (num+1)/2)
			{// 第一排
				int firstCount = (num+1)/2;
				posX += (i - firstCount/2)*delta_x;
				if(firstCount % 2 == 0)
					posX += delta_x / 2;
			}
			else
			{// 第二排
				int secondCount = num - (num+1)/2;
				posX += (i-(num+1)/2-secondCount/2)*delta_x;
				if(secondCount % 2 == 0)
					posX += delta_x / 2;
			}
			diwen->setPositionX(posX);
			diwen->setPositionY(start_Y - delta_y*(i/((num+1)/2)) );
			//daoju
			string djFile = ImageManager::getManager()->getItemFilename(daojuVec.at(i));
			BaseSprite *djImg = BaseSprite::create(djFile);
			diwen->addChild(djImg, 10);
//			djImg->setScale(scale);
			djImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
			//name label
			string name;
			if(daojuVec.at(i) > ItemsType_SkillBook_MinId && daojuVec.at(i) < ItemsType_SkillBook_MaxId)
				name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(daojuVec.at(i) - ItemsType_SkillBook_MinId));
			else 
				name = CS::getItemName(daojuVec.at(i));
			CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size, CCSize(96, 0), kCCTextAlignmentCenter);
			diwen->addChild(labelName, 10);
			labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
			labelName->setColor(ccc3(127,127,127));
		}
	}
}
*/

void MapsLayer_DiaoLuoChaXun::menuItemClicked_zbClicked(CCObject *pSender)
{
	//
	BaseSprite *zbDisplayerBg = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZB_Display);
	zbDisplayerBg->getChildByTag(Tag_BoardMenu)->setVisible(false);
	zbDisplayerBg->getChildByTag(Tag_Board_zb_Or_dj_Menu)->setVisible(false);

	CCMenuItemImage *eqItem = (CCMenuItemImage*)pSender;
	int idx = (int)eqItem->getUserData();
	//
	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	mBg->addChild(board, 30, Tag_Board_EquipDetail);
	board->setPosition(ccp(mBg->getContentSize().width/2+4, mBg->getContentSize().height/2-58));
	{
		Flat_DuanzaoGroupZBInfo info;
		convertFubenDiaoluoZBInfo2DuanzaoGroupZBInfo(mCurTaskData.diaoluo_vec.at(idx), info);
		refreshEquipBoard(board, info);
	}


	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_equipDetailMenu));
	menu->addChild(item, 0, Tag_MenuItem_EquipDetail_Close);
	item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
	//底纹
	ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		(ZhuangbeiColour)mCurTaskData.diaoluo_vec.at(idx).zhuangbei_colour
		, false//, ChuanqiPool::getPool()->isShenBing(mCurTaskData.diaoluo_vec.at(idx).group_id)
		, SuitPool::getPool()->isSuit(mCurTaskData.diaoluo_vec.at(idx).group_id));
	BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
	diWen->setScale(0.7f);
	board->addChild(diWen);
	diWen->setPosition(ccp(123, 838-240+122));
	// 装备图片
	string equipfile = mCurTaskData.diaoluo_vec.at(idx).zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *s = BaseSprite::create(equipfile);
	diWen->addChild(s);
	s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	//套装按钮
	if(SuitPool::getPool()->isSuit(mCurTaskData.diaoluo_vec.at(idx).group_id))
	{
		string norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang.png";
		string selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png";
// 		if (ChuanqiPool::getPool()->isShenBing(mCurTaskData.diaoluo_vec.at(idx).group_id))
// 		{
// 			norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png";
// 			selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png";
// 		} 
		CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
			norStr.c_str()
			, selStr.c_str()
			, this ,menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		taozhuangItem->setEnabled(false);

		mCurTaoZhuang_Info = mCurTaskData.diaoluo_vec.at(idx);

	}
}


void MapsLayer_DiaoLuoChaXun::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_EquipDetail_Close)
	{
		//
		BaseSprite *zbDisplayerBg = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZB_Display);
		zbDisplayerBg->getChildByTag(Tag_BoardMenu)->setVisible(true);
		zbDisplayerBg->getChildByTag(Tag_Board_zb_Or_dj_Menu)->setVisible(true);
		//
		mBg->removeChildByTag(Tag_Board_EquipDetail);
	}
	else
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
			mCurTaoZhuang_Info.group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
		mBg->addChild(layer, 40, Tag_Board_TaoZhuangShuxing);
		layer->release();

		/*
		BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban_taozhuang.png");
		mBg->addChild(board, 40, Tag_Board_TaoZhuangShuxing);
		board->setPosition(ccp(mWinSize.width/2-12, mBg->getContentSize().height/2-54));

		/////////////////////////////////////////////
		CCMenu *menu = CCMenu::create();
		board->addChild(menu);
		menu->setPosition(CCPointZero);
		//
		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(MapsLayer_DiaoLuoChaXun::menuItemClicked_taoZhuangFanhui));
		menu->addChild(fanhui);
		fanhui->setPosition(ccp(board->getContentSize().width/2, 838-784 + 80));
		//

		OneSuitInfo suitInfo;
		if(SuitPool::getPool()->getSuitInforByZBGroupId(mCurTaoZhuang_Info.group_id, suitInfo))
		{//获取信息成功
			CCSize boardSize = board->getContentSize();
			ccColor3B labelColor_huise = ccc3(127, 127, 127);
			int labelSize = 24;
			int start_x = 28;
			int start_y = 838- 140;
			int label_gap = 6;
			char buf[20];
			// 0 title (name)
			string strName = suitInfo.suit_name;
			CCLabelTTF *labelTitle = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, 30);
			board->addChild(labelTitle);
			labelTitle->setPosition(ccp(boardSize.width/2, 838-35));
			labelTitle->setColor(ccGREEN);
			// 1 name
			CCLabelTTF *labelName = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, 28);
			board->addChild(labelName);
			labelName->setAnchorPoint(ccp(0, 0.5));
			labelName->setPosition(ccp(start_x, 838-100));
			labelName->setColor(ccGREEN);
			// 2 level
			sprintf(buf, "%d", mCurTaoZhuang_Info.level);
			CCLabelTTF *labelLevel = CCLabelTTF::create((LFStrings::getValue("dengji")+buf).c_str(), fontStr_kaiti, labelSize);
			board->addChild(labelLevel);
			labelLevel->setAnchorPoint(ccp(1, 0.5));
			labelLevel->setPosition(ccp(boardSize.width-start_x, 838-100));
			labelLevel->setColor(labelColor_huise);
			// 3 every one name
			int taozhuangNum = suitInfo.parts_of_suit.size(); //记录有多少件
			for (int i=0; i!=suitInfo.parts_of_suit.size(); ++i)
			{
				string strOneName = suitInfo.parts_of_suit.at(i).zhuangbei_name;
				string strTypeName = getPosName_ByZhuangBeiType(suitInfo.parts_of_suit.at(i).zhuanbei_type);
				CCLabelTTF *labelOneName = CCLabelTTF::create((strOneName + " (" + strTypeName + ")").c_str(), fontStr_kaiti, labelSize);
				board->addChild(labelOneName);
				labelOneName->setAnchorPoint(ccp(0, 0.5));
				labelOneName->setPosition(ccp(start_x, start_y));

				if (mCurTaoZhuang_Info.zhuangbei_name != strOneName)
				{
					labelOneName->setColor(labelColor_huise);
					--taozhuangNum;
				}

				start_y -= (labelOneName->getContentSize().height + label_gap);
			}
			// 4 (2)件
			for(int i=0; i<=suitInfo.parts_of_suit.size()-2; ++i)
			{
				ccColor3B color;
				if(i+2 <= taozhuangNum)
					color = ccGREEN;
				else 
					color = labelColor_huise;
				// 2、3、4、5件
				sprintf(buf, "(%d)", i+2);
				string strjian = buf + LFStrings::getValue("jian");
				CCLabelTTF *labelJian = CCLabelTTF::create(strjian.c_str(), fontStr_kaiti, labelSize);
				board->addChild(labelJian);
				labelJian->setAnchorPoint(ccp(0, 0.5));
				labelJian->setPosition(ccp(start_x, start_y));
				labelJian->setColor(color);

				start_y -= (labelJian->getContentSize().height + label_gap);
				//属性
				for (int j=0; j<=1; ++j)
				{
					if (suitInfo.suit_attr[i][j].attr_type == 0)
						continue;

					CCLog("EquipmentXiangQingBody::create_TaoZhuangShuXing_Item [i=%d][j=%d]", i, j);
					BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
					board->addChild(biaoji);
					biaoji->setPosition(ccp(66, start_y));

					string strShuXing = GetMoFaShuXing_ByZBAttr(suitInfo.suit_attr[i][j]);
					CCLabelTTF *labelShuxing = CCLabelTTF::create(strShuXing.c_str(), fontStr_kaiti, labelSize);
					board->addChild(labelShuxing);
					labelShuxing->setAnchorPoint(ccp(0, 0.5));
					labelShuxing->setPosition(ccp(biaoji->getPositionX() + biaoji->getContentSize().width, start_y));
					labelShuxing->setColor(color);

					start_y -= (labelShuxing->getContentSize().height + label_gap);
				}
			}

		}*/
	}
}
// 
// void MapsLayer_DiaoLuoChaXun::menuItemClicked_taoZhuangFanhui(CCObject *pSender)
// {
// 	((CCMenu*)mBg->getChildByTag(Tag_Board_EquipDetail)->getChildByTag(Tag_Menu_EquipDetail))->setEnabled(true);
// 
// 	mBg->removeChildByTag(Tag_Board_TaoZhuangShuxing);
// }

void MapsLayer_DiaoLuoChaXun::callbackInterface_TaoZhuangLayer()
{
	((CCMenu*)mBg->getChildByTag(Tag_Board_EquipDetail)->getChildByTag(Tag_Menu_EquipDetail))->setEnabled(true);

	mBg->removeChildByTag(Tag_Board_TaoZhuangShuxing);
}