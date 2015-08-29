
#include "ChangeZhuangBeiBody.h"
#include "MyselfManager.h"
#include "MsgDefine.h"
#include "GameDirector.h"
#include "LFMessageBox.h"
#include "LFAlert.h"
#include "font.h"
#include "GuideLayerBase.h"
#include "GongJiangJianDing.h"

#define TAG_BG_MASK_PIC 22221
#define TAG_CLOSE_ITEM  22222
#define TAG_YES_ITEM	22223

enum 
{
	Tag_displayer_Dialog = 100,
	Tag_MenuItem_ok,
	Tag_MenuItem_cancel,
	Tag_MenuItem_JianDing,
	Tag_MenuItem_JianDing_Erci_QueDing,

	Tag_GengXinTishi,    //好装备提示
};

extern bool g_firstTaskEnd_to_changeEquip;

// 选择装备的列表，点击了列表的项，就触发到目的装备详情界面
ChangeZhuangBeiBody::ChangeZhuangBeiBody(int actorId,EquipRequirePlace place,Equipment* equip /* = NULL */)
	: MainLayerZhuangBeiBaseBody(From_ChangeZhuangBei)
	,m_iCurCellIndex(0)
{
	mActorId = actorId;
	//mType = type;
	//mEquipTypeId = equipTypeId;
	mPlace = place;
	mNeedReplacedEquip = equip;

	board = BaseSprite::create("mianban_zhuangbei.png");
	addChild(board);
	board->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	selectedIndex = -1;

	mTableView->retain();
	mTableView->removeFromParent();
	board->addChild(mTableView);
	mTableView->release();
	mTableView->initWithViewSize(CCSize(624,678+63));
	mTableView->setPosition(ccp(8,74-63));
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

	BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
	vector<Equipment*> temp = MyselfManager::getManager()->getAllEquipments(Filter_All);
	vector<Equipment*> filted;
	int num = temp.size();
	for (int i=0;i<num;i++)
	{
		if((temp[i]->getEquipData().profession_id != 0 && temp[i]->getEquipData().profession_id != actor->getCurrentProperty_R()->profession))
			continue;
		if (//temp[i]->getRequiredPlace() == place &&		// 位置正确
			temp[i]->canEquipToPlace(place) &&		// 位置正确
			temp[i] != mNeedReplacedEquip &&			// 已经穿在身上的不显示
			temp[i]->getActorId() != mActorId &&
			EquipmentManager::getManager()->canUseEquip(temp[i]->getType(),actor->getActorType()))// 可以装备这种类型
			//!temp[i]->isInFengYin())//封印
			
		{
			filted.push_back(temp[i]);
		}
	}

	setEquipmentData(filted, SORT_IN_HuanZhuang);
	//提示更新装备 排在前面、、、、
	{
		//获取后保存、 之后不再获取
		BaseActor *curActor = MyselfManager::getManager()->getMyTeamActor(mActorId);
		mCurActorReport = ZhuangbeiScanner::getScanner()->getTestReportByActor(curActor);
		if (mCurActorReport.reportList.size() > 0 && mCurActorReport.reportList.at(0).actor == curActor
			&& ! ZhuangbeiScanner::getScanner()->isIgnoreHint(curActor, mPlace))
		{
			vector<Equipment*> repEqVec = mCurActorReport.reportList.at(0).betterEquipVec;
			int iSwapNum = 0;
			//遍历当前装备列表的装备
			for (int mi=0; mi<mEquipmentVec.size(); ++mi)
			{
				//如果它在更好的提示装备列表中  则网前移动
				for (int iidx=0; iidx<repEqVec.size(); ++iidx)
				{
					if (repEqVec.at(iidx)->getEquipId() == mEquipmentVec.at(mi)->getEquipId())
					{
						swap(mEquipmentVec.at(mi), mEquipmentVec.at(iSwapNum++));
						//移到前面
						break;
					}
				}
			}
		}
	}
	mTableView->reloadData();

	CCMenuItemImage* closeItem = CCMenuItemImage::create(
		"close.png",
		"close_select.png",
		this,menu_selector(ChangeZhuangBeiBody::menuItemClickHandler)    
		);
	closeItem->setTag(TAG_CLOSE_ITEM);

	menu = CCMenu::create(closeItem,NULL);
	board->addChild(menu);
	menu->setPosition(CCPointZero);
	closeItem->setPosition(ccp(board->getContentSize().width - closeItem->getContentSize().width,board->getContentSize().height - closeItem->getContentSize().height+8));
}



void ChangeZhuangBeiBody::menuItemClickHandler(CCObject* sender)
{
	//////////////////////////////////////////////////////////////////////////////////
	//点击装备后，该槽位的装备提示就取消了
	ZhuangbeiScanner::getScanner()->setIgnoreHint(true, MyselfManager::getManager()->getMyTeamActor(mActorId), mPlace);
	/////////////////////////////////////////////////////////////////////////////////////

	CCMenuItemImage* item = (CCMenuItemImage*)sender;
	int tag = item->getTag();

	if (tag == TAG_YES_ITEM)
	{

	}
	else if(tag == TAG_CLOSE_ITEM)
	{
		MainLayerBase* footer = mFooterLayer;
		footer->dismmisPopUp();
		BaseLayer* topLayer = footer->getTopLayer();
		if(topLayer != NULL)
		{
			topLayer->refreshLayer();
		}
		else
		{
			footer->getContentLayer()->refreshLayer();
		}
	}

}
void ChangeZhuangBeiBody::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	if (mFooterForPop == NULL)
	{
		CCLog("please call setFooter!");
		CCAssert(false,"");
		return;
	}
	m_iCurCellIndex = cell->getIdx();
	if(mEquipmentVec.at(cell->getIdx())->getActorId() != INVALID_ID)
	{
		this->addDialog(LFStrings::getValue("gaizhuangbeibeiqitayingxiongshiyong"), Tag_MenuItem_ok, Tag_MenuItem_cancel);

//		//弹框背景
// 		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
// 		dialog->setPosition(ccp(320, mTableView->getViewSize().height/2 + 50));
// 		dialog->setTag(Tag_displayer_Dialog);
// 		//提示
// 		{
// 			CCLabelTTF* lable = CCLabelTTF::create(LFStrings::getValue("gaizhuangbeibeiqitayingxiongshiyong").c_str()
// 				, fontStr_kaiti,30,CCSize(340, 0), kCCTextAlignmentLeft);
// 			dialog->addChild(lable);
// 			lable->setPosition(ccp(0,0));
// 			lable->setPosition(ccp(dialog->getContentSize().width/2, 228));
// 
// 			ccColor3B color;
//			color.r = 201;
//			color.g = 156;
//			color.b = 18;
//			lable->setColor(color);
// 		}
// 
// 		//按键
// 		CCMenu* menu;
// 		{
// 			CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
// 				"shangpin_goumai_queren.png",
// 				"shangpin_goumai_queren_select.png",
// 				this,SEL_MenuHandler(&ChangeZhuangBeiBody::menuItemClicked_displayDialog));
// 			CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
// 				"shangpin_goumai_quxiao.png",
// 				"shangpin_goumai_quxiao_select.png",
// 				this,SEL_MenuHandler(&ChangeZhuangBeiBody::menuItemClicked_displayDialog));
// 
// 			menu = CCMenu::create(menuItemMakeSure, menuItemCancel, NULL);
// 			menuItemMakeSure->setTag(Tag_MenuItem_ok);
// 			menuItemCancel->setTag(Tag_MenuItem_cancel);
// 			menuItemMakeSure->setUserData(mEquipmentVec.at(cell->getIdx()));
// 
// 			menuItemMakeSure->setPosition(ccp(120,70));
// 			menuItemCancel->setPosition(ccp(368,70));
// 		}
// 		//弹框
// 		{
// 
// 			LFAlert* lfAlert = new LFAlert();
// 			lfAlert->setBaseContainer(dialog,menu);
// 			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_displayer_Dialog);
// 			lfAlert->release();
// 		}
	}
	else if (mEquipmentVec.at(cell->getIdx())->isInFengYin())
	{
		//要鉴定的装备的等级比当前角色的等级高的话，弹框提示
		Equipment* pEquipInfo = mEquipmentVec.at(cell->getIdx());
		unsigned int id =  pEquipInfo->getActorId();
		if (id != INVALID_ID)
		{
			if (pEquipInfo->getEquipData().zhuangbei_level > MyselfManager::getManager()->getMyTeamActor(id)->getCurrentProperty().level)
			{
				LFAlert* pDig = dynamic_cast<LFAlert*>(CCDirector::sharedDirector()->getRunningScene()->getChildByTag(Tag_displayer_Dialog));
				if (pDig)
				{
					pDig->removeFromParent();
				}

				this->addDialog(LFStrings::getValue("JianDian_Zhuangbei_DengJi_Taigao"), Tag_MenuItem_JianDing_Erci_QueDing, Tag_MenuItem_cancel, true);
			}
			else
			{
				this->addDialog(LFStrings::getValue("JianDian_ChuanDai"), Tag_MenuItem_JianDing, Tag_MenuItem_cancel);
			}
		}
		else
		{
			this->addDialog(LFStrings::getValue("JianDian_ChuanDai"), Tag_MenuItem_JianDing, Tag_MenuItem_cancel);
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////////
		//点击装备后，该槽位的装备提示就取消了
		ZhuangbeiScanner::getScanner()->setIgnoreHint(true, MyselfManager::getManager()->getMyTeamActor(mActorId), mPlace);
		/////////////////////////////////////////////////////////////////////////////////////

		EquipmentXiangQingBody* xiangQing = 
			new EquipmentXiangQingBody(mEquipmentVec[cell->getIdx()], mActorId,From_ChangeZhuangBei);
		mFooterForPop->popUpContaintLayer(xiangQing, Pop_Pos_WondowDown);
		xiangQing->release();
	}
}

void ChangeZhuangBeiBody::itemImageClicked(CCObject* obj)
{
	if (mFooterForPop == NULL)
	{
		CCLog("please call setFooter!");
		CCAssert(false,"");
		return;
	}

	CCMenuItemSprite* item = (CCMenuItemSprite*)obj;
	CCLog("Image Clicked! index = %d",item->getTag());

	tableCellTouched(mTableView, mTableView->cellAtIndex(item->getTag()));
}

BaseSprite* ChangeZhuangBeiBody::getItemContentByIndex(unsigned int index)
{
	BaseSprite* temp = MainLayerZhuangBeiBaseBody::getItemContentByIndex(index);
	if (index != selectedIndex)
	{
		temp->removeChildByTag(TAG_BG_MASK_PIC);
	}else
	{
		if (!temp->getChildByTag(TAG_BG_MASK_PIC))
		{
			BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
			temp->addChild(addEff,10,TAG_BG_MASK_PIC);
			addEff->setPosition(ccp(temp->getContentSize().width/2,temp->getContentSize().height/2));
		}
	}

	BaseActor *curActor = MyselfManager::getManager()->getMyTeamActor(mActorId);
	if (curActor && !curActor->isYuanShen() && curActor->getCurrentProperty().level < mEquipmentVec.at(index)->getLevel())
	//等级不足 盖一张图片
	{
// 		CCSprite *diwen = (CCSprite*)temp->getChildByTag(TAG_BaseBody_TABLE_ITEM_DiWen);
// 		if (diwen)
// 		{
// 			BaseSprite *img = BaseSprite::create("zhuangbei_xiangxi_diwen_dengjibuzu.png");
// 			temp->addChild(img, diwen->getZOrder()+1);
// 			img->setPosition(diwen->getPosition());
// 			img->setScale(diwen->getScale());
// 		}

		CCSprite *diwen = (CCSprite*)temp->getChildByTag(TAG_BaseBody_TABLE_ITEM_DiWen);
		if (diwen)
		{
			BaseSprite *img = BaseSprite::create("zhuangbei_xiangxi_diwen_dengjibuzu_new.png");
			temp->addChild(img, 1);
			img->setPosition(diwen->getPosition());
			img->setScale(diwen->getScale());
		}

		CCLabelTTF *owner = (CCLabelTTF*)temp->getChildByTag(TAG_BaseBody_LABLE_OWNTO);
		if (owner)
		{
			owner->setColor(fonColor_JingGao);
			owner->setString(LFStrings::getValue("DengJiBuZu").c_str());
		}
		CCLabelTTF *lv = (CCLabelTTF*)temp->getChildByTag(TAG_BaseBody_LABLE_Level);
		if (lv)
		{
			lv->setColor(fonColor_JingGao);
		}
	}

	//是否有装备更新提示
	bool bMarked = false;
	//构造函数已保存
// 	mCurActorReport = ZhuangbeiScanner::getScanner()->getTestReportByActor(curActor);
	if (mCurActorReport.reportList.size() > 0 && mCurActorReport.reportList.at(0).actor == curActor
		&& ! ZhuangbeiScanner::getScanner()->isIgnoreHint(curActor, mPlace))
	{
		vector<Equipment*> repEqVec = mCurActorReport.reportList.at(0).betterEquipVec;
		for (int iidx=0; iidx<repEqVec.size(); ++iidx)
		{
			if (bMarked) break;
			if (repEqVec.at(iidx)->getEquipId() == mEquipmentVec[index]->getEquipId())
			{
				CCSprite *tishiImg = CCSprite::create("zhuye_gongjitisheng.png");
				temp->addChild(tishiImg, 100, Tag_GengXinTishi);
				tishiImg->setPosition(ccp(temp->getContentSize().width-tishiImg->getContentSize().width/2, tishiImg->getContentSize().height/2));
				//
				bMarked = true;
				ZhuangbeiScanner::getScanner()->setMarkRead(curActor, repEqVec.at(iidx));
				break;
			}
		}
	}

	return temp;
}

void ChangeZhuangBeiBody::disableAllTouchBegin()
{
	MainLayerZhuangBeiBaseBody::disableAllTouchBegin();

	menu->setEnabled(false);
	menu->setVisible(false);
	mTableView->setTouchEnabled(false);
}
void ChangeZhuangBeiBody::disableAllTouchEnd()
{
	MainLayerZhuangBeiBaseBody::disableAllTouchEnd();

	menu->setEnabled(true);
	menu->setVisible(true);
	mTableView->setTouchEnabled(true);
}

ChangeZhuangBeiBody::~ChangeZhuangBeiBody()
{

}

void ChangeZhuangBeiBody::menuItemClicked_displayDialog(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_displayer_Dialog);
	int iTag = ((CCMenuItemImage*)pSender)->getTag();
	if(iTag == Tag_MenuItem_ok)
	{
		EquipmentXiangQingBody* xiangQing = 
			new EquipmentXiangQingBody((Equipment*)((CCMenuItemImage*)pSender)->getUserData(), mActorId,From_ChangeZhuangBei);
		mFooterForPop->popUpContaintLayer(xiangQing, Pop_Pos_WondowDown);
		xiangQing->release();
	}
	else if (iTag == Tag_MenuItem_JianDing)
	{
		GongJiangXiYouZhuangBeiJianDing* player = GongJiangXiYouZhuangBeiJianDing::create(gongjiang_jianding, (Equipment*)((CCMenuItemImage*)pSender)->getUserData());
		mFooterForPop->popUpContaintLayer(player);
	}
// 	else if (iTag == Tag_MenuItem_JianDing_Erci_QueDing)
// 	{
// 		GongJiangXiYouZhuangBeiJianDing* player = GongJiangXiYouZhuangBeiJianDing::create(gongjiang_jianding, (Equipment*)((CCMenuItemImage*)pSender)->getUserData());
// 		mFooterForPop->popUpContaintLayer(player);
// 	}
}

void ChangeZhuangBeiBody::onEnter()
{
	MainLayerZhuangBeiBaseBody::onEnter();
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_ChangeZhuangBei))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_ChangeZhuangBei);
		// 置false  与 目的装 详情里
		//g_firstTaskEnd_to_changeEquip = false;
		//
		CCTableViewCell* cell = mTableView->cellAtIndex(0);
		if (cell)
		{
			//GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_ChangeZhuangBei)
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCSprite *board = (CCSprite*)mTableView->cellAtIndex(0)->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);
			guideLayer->init(board->getContentSize(), board->getParent()->convertToWorldSpace(board->getPosition())
				, LFStrings::getValue("guide_XuanZeZhuangBei"), NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}
}

void ChangeZhuangBeiBody::addDialog(string contextStr, int iTag_OK, int iTag_Cancel, bool bOffset /*= false*/)
{
	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(320, mTableView->getViewSize().height/2 + 50));
	dialog->setTag(Tag_displayer_Dialog);
	//提示
	{
		CCLabelTTF* lable = CCLabelTTF::create(contextStr.c_str()
			, fontStr_kaiti,30,CCSize(340, 0), kCCTextAlignmentLeft);
		dialog->addChild(lable);
		lable->setPosition(ccp(0,0));
		lable->setPosition(ccp(dialog->getContentSize().width/2, 228));

		ccColor3B color;
		color.r = 201;
		color.g = 156;
		color.b = 18;
		lable->setColor(color);
	}

	//按键
	CCMenu* menu;
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&ChangeZhuangBeiBody::menuItemClicked_displayDialog));
		CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&ChangeZhuangBeiBody::menuItemClicked_displayDialog));

		menu = CCMenu::create(menuItemMakeSure, menuItemCancel, NULL);
		menuItemMakeSure->setTag(iTag_OK);
		menuItemCancel->setTag(iTag_Cancel);
		menuItemMakeSure->setUserData(mEquipmentVec.at(m_iCurCellIndex));

		menuItemCancel->setPosition(ccp(120,70));
		menuItemMakeSure->setPosition(ccp(368,70));

		if (bOffset)
		{
			menuItemCancel->setPosition(ccp(dialog->getContentSize().width/2, 70));
			menuItemMakeSure->setVisible(false);
		}
	}
	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_displayer_Dialog);
		lfAlert->release();
	}
}