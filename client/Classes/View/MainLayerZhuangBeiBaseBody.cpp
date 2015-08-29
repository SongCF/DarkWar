#include "MainLayerZhuangBeiBaseBody.h"
#include "LFUtile.h"
#include "BaseActorPropertyParser.h"
#include "EquipmentXiangQingBody.h"
#include "font.h"
#include "CS_Common.h"
#include "FXScrollMenu.h"
#include "GongJiangFenJie.h"
#include <algorithm>
#include "ZhuangbeiOptionLevel.h"
#include "UnblockManager.h"

const int TAG_BIAOJI = 60;

CCPoint lableNamePos = ccp(148,134);
CCPoint lableOwnToPos= ccp(281,28);
float lableNameSize = 30.0f;
float lableOwnToSize = 25.0f;

// 06-15 11:43:50.179: D/cocos2d-x debug info(1716): Get data from file(tabulation_armor_helmet_1.png) failed!
MainLayerZhuangBeiBaseBody::MainLayerZhuangBeiBaseBody(ZhuangbeiXiangQingFrom from)
{
	mFrom = from;

	mTableView = CCTableView::create(this, CCSizeMake(624,678));
	addChild(mTableView);
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mTableView->setPosition(ccp(8,74));
	mTableView->setDelegate(this);
	mFooterForPop = NULL;

	mClickedIdx = 0;


	BaseSprite *bar = BaseSprite::create("mianban_zhuangbeishuliang_bg.png");
	addChild(bar, 10);
	bar->setPosition(ccp(320, 43));
	CCString temp;
	temp.initWithFormat("%s: %d/%u", LFStrings::getValue("zhuangbei").c_str()
		, MyselfManager::getManager()->getAllEquipmentsWithOutEquipedOrChuanQi().size(), MAX_COUNTS_IN_KNAPSACK);
	m_zbTotalLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
	m_zbTotalLabel->setAnchorPoint(ccp(1, 0.5));
	m_zbTotalLabel->setPosition(ccp(bar->getContentSize().width-20, bar->getContentSize().height/2));
	bar->addChild(m_zbTotalLabel);
	//分解提示
	m_zbFenJieTiShi = CCLabelTTF::create(LFStrings::getValue("ZBCountMax_GanKuaiQuFenJieBa").c_str(), fontStr_kaiti, 20);
	bar->addChild(m_zbFenJieTiShi);
	m_zbFenJieTiShi->setPosition(ccp(bar->getContentSize().width/2, bar->getContentSize().height/2));
	m_zbFenJieTiShi->setColor(fonColor_JingGao);
	m_zbFenJieTiShi->setVisible(false);
	m_zbFenJieMenu = CCMenu::create();
	bar->addChild(m_zbFenJieMenu);
	m_zbFenJieMenu->setPosition(ccp(0,0));
	m_zbFenJieMenu->setVisible(false);
	CCMenuItemImage *fenjie = CCMenuItemImage::create(
		"mianban_zhuangbei_qufenjie.png",
		"mianban_zhuangbei_qufenjie_select.png",
		this, menu_selector(MainLayerZhuangBeiBaseBody::menuitemClicked_GotoFenJie));
	m_zbFenJieMenu->addChild(fenjie);
	fenjie->setPosition(ccp(m_zbTotalLabel->getPositionX()-fenjie->getContentSize().width/2, m_zbTotalLabel->getPositionY()));
	if (mFrom != From_ZhuangBeiList)
	{
		m_zbFenJieMenu->removeFromParent();
	}
}

void MainLayerZhuangBeiBaseBody::onEnter()
{
	BaseLayer::onEnter();
	mTableView->reloadData();
}

MainLayerZhuangBeiBaseBody::~MainLayerZhuangBeiBaseBody(void)
{
}


void MainLayerZhuangBeiBaseBody::scrollViewDidScroll(CCScrollView* view) 
{
}
void MainLayerZhuangBeiBaseBody::scrollViewDidZoom(CCScrollView* view) 
{
}

void MainLayerZhuangBeiBaseBody::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
}
void MainLayerZhuangBeiBaseBody::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
	//CCLog("tableCellWillRecycle");
}
void MainLayerZhuangBeiBaseBody::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
}

void MainLayerZhuangBeiBaseBody::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
}

CCSize MainLayerZhuangBeiBaseBody::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(624, 226);
}

void MainLayerZhuangBeiBaseBody::itemImageClicked(CCObject* obj)
{
	if (mFooterForPop == NULL)
	{
		CCLog("please call setFooter!");
		CCAssert(false,"");
		return;
	}

	CCMenuItemSprite* item = (CCMenuItemSprite*)obj;
//	CCLog("Image Clicked! index = %d",item->getTag());
	mClickedIdx = item->getTag();

	EquipmentXiangQingBody* xiangQing = new EquipmentXiangQingBody(mEquipmentVec[item->getTag()]
											, mEquipmentVec[item->getTag()]->getActorId(),mFrom);
	mFooterForPop->popUpContaintLayer(xiangQing, Pop_Pos_WondowDown);
	xiangQing->release();
}

void MainLayerZhuangBeiBaseBody::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU);
				if (menu)
				{
					menu->setEnabled(false);
				}
			}
		}
	}
	
	mOffset = mTableView->getContentOffset();
}
void MainLayerZhuangBeiBaseBody::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU);
				if (menu)
				{
					menu->setEnabled(true);
				}
			}
		}
	}

	CCPoint beg = mTableView->getContentOffset();
	if (mOffset.y < beg.y) mOffset.y = beg.y;
	if (mOffset.y > tableCellSizeForIndex(mTableView,0).height*3) mOffset.y = tableCellSizeForIndex(mTableView,0).height*3;
	mTableView->setContentOffset(mOffset);
}

CCTableViewCell* MainLayerZhuangBeiBaseBody::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL) 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	if (table != mTableView) return cell;
	
	cell->addChild(getItemContentByIndex(idx),0,TAG_EquipBaseBody_TABLE_ITEM);
	
	return cell;

}

BaseSprite* MainLayerZhuangBeiBaseBody::getItemContentByIndex(unsigned int index)
{
	BaseSprite* bg = BaseSprite::create("mianban_zhuangbei_xiangxi.png");

	CCString tempStr;

	CCPoint picPos = ccp(75.0f, bg->getContentSize().height/2);

	{
		ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_List(
			mEquipmentVec[index]->getEquipColor(),
			false,
			mEquipmentVec[index]->isPartOfSuit(),
			mEquipmentVec[index]->haveZhuanke());
		BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
		diWen->setScale(diWenData.scale);
		diWen->setPosition(picPos);
		bg->addChild(diWen,0,TAG_BaseBody_TABLE_ITEM_DiWen);

		{
			BaseSprite *sprite = ImageManager::getManager()->getSpriteOfZhuangbei(mEquipmentVec[index],true,true);
			//sprite->setScale(data.scale);
			sprite->setScale(0.43f);
			sprite->setPosition(picPos);
			bg->addChild(sprite,0,TAG_BaseBody_TABLE_ITEM_Image);
		}
		
		//封印
		if (mEquipmentVec[index]->isInFengYin())
		{
			BaseSprite *fyImg = BaseSprite::create("shenbing_fengyin_suo.png");
			bg->addChild(fyImg, 0, TAG_BaseBody_TABLE_FengYin_Img);
			fyImg->setPosition(diWen->getPosition());
			fyImg->setScale(diWen->getContentSize().width*diWen->getScale() / fyImg->getContentSize().width);
		}
		
	}

	// 添加图片识别区
	{
		ImageData selecedData = ImageManager::getManager()->getImageData_DiWen_Selected(
			mEquipmentVec[index]->getEquipColor(),
			false,//mEquipmentVec[index]->isShenBing(),
			mEquipmentVec[index]->isPartOfSuit());
		BaseSprite* menuItemSel = BaseSprite::create(selecedData.getCurrentImageFile());
		menuItemSel->setScale(selecedData.scale);
		CCSprite* menuRect = CCSprite::createWithTexture(menuItemSel->getTexture());
		menuRect->setScale(0.0f);

		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		bg->addChild(menu, 0, TAG_BaseBody_MENU);
		menu->setPosition(ccp(0,0));
		CCMenuItemSprite* t = CCMenuItemSprite::create(menuRect,menuItemSel,this,SEL_MenuHandler(&MainLayerZhuangBeiBaseBody::itemImageClicked));
		menu->addChild(t, 0, index);
		t->setPosition(picPos);
	}

	float left_x_start = 160 - 4;
	//名称
	{
		CCLabelTTF* label;
		char str[64];
		if (mEquipmentVec[index]->getEquipData().qianghua_data.qianghua_count != 0)
		{
			sprintf(str, "%s", mEquipmentVec[index]->getName().c_str());
		}
		else
		{
			sprintf(str, "%s", mEquipmentVec[index]->getName().c_str());
		}
		
		label = LF::lable(bg,str,ccp(left_x_start,190),24,fontStr_kaiti);
		label->setTag(TAG_BaseBody_LABLE_NAME);
		ccColor3B color = ImageManager::getManager()->getShowColor(mEquipmentVec[index]->getEquipColor(),false,mEquipmentVec[index]->isPartOfSuit());
		label->setColor(color);
	}

	//类型
	{
		std::string typeStr = 
			CS::getZhuangbeiTypeName(mEquipmentVec[index]->getType(), mEquipmentVec[index]->getEquipColor());

		CCLabelTTF* label;
		label = LF::lable(bg,typeStr,ccp(left_x_start,165),18,fontStr_kaiti);
		label->setTag(TAG_BaseBody_LABLE_TYPE);
		label->setColor(fonColor_CiYao);
	}

	//白字
	{
		CCLabelTTF* label_baizhi = NULL;
		bool hasBaiZhi = false;
		{
			float baizi = 0;
			switch(mEquipmentVec[index]->getType())
			{
			case ZhuangbeiType_DanshouJian:
			case ZhuangbeiType_ShuangshouBishou:
			case ZhuangbeiType_ShuangshouFu:
			case ZhuangbeiType_DanshouFazhang:
			case ZhuangbeiType_ShuangshouFazhang:
			case ZhuangbeiType_Gong:
				hasBaiZhi = true;
				baizi = mEquipmentVec[index]->getGongjiLiAfterQianghua(false);
				break;
			case ZhuangbeiType_Dunpai:
			case ZhuangbeiType_KuijiaZhong:
			case ZhuangbeiType_KuijiaQin:
				hasBaiZhi = true;
				baizi = mEquipmentVec[index]->getHuJiaZhiAfterQianghua();
				break;
			default:
				break;
			}
			if (hasBaiZhi)
			{
				char str[64];
				sprintf(str, "%.1f", baizi);

				label_baizhi = LF::lable(bg,str,ccp(left_x_start,126),60,fontStr_BookAntiqua);
				label_baizhi->setTag(TAG_BaseBody_LABLE_BAIZI);
			}
			else
			{
				label_baizhi = LF::lable(bg,"",ccp(left_x_start,126),60,fontStr_BookAntiqua);
				label_baizhi->setTag(TAG_BaseBody_LABLE_BAIZI);
			}
		}
		//junshang
		CCLabelTTF* label_junshang = LF::lable(bg,LFStrings::getValue("junshang"),ccp(0,0),20,fontStr_kaiti);
		label_junshang->setAnchorPoint(ccp(0, 0.5));
		label_junshang->setPosition(
			ccp(label_baizhi->getPositionX()+label_baizhi->getContentSize().width+10, 116));
		label_junshang->setTag(TAG_BaseBody_LABLE_JUNSHANG_TXT);
		if(hasBaiZhi)
		{
			label_junshang->setVisible(true);
			if(mEquipmentVec[index]->getEquipType() == EquipType_FangJu)
				label_junshang->setString(LFStrings::getValue("huJia").c_str());
		}
		else
		{
			label_junshang->setVisible(false);
		}
	}
	
	//伤害详细 
	if (mEquipmentVec[index]->getEquipType() == EquipType_WuQi)
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, "%u-%u ", mEquipmentVec[index]->getEquipData().gongji_min
			, mEquipmentVec[index]->getEquipData().gongji_max);

		label = LF::lable(bg,str,ccp(left_x_start,87),20,fontStr_BookAntiqua);
		label->setTag(TAG_BaseBody_LABLE_SHANGHAIXIANGXI_NUM);
		label->setColor(fonColor_CiYao);
		CCLabelTTF* label2;
		label2 = LF::lable(bg,LFStrings::getValue("dianShangHai")
			,ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()),20,fontStr_kaiti);
		label2->setTag(TAG_BaseBody_LABLE_SHANGHAIXIANGXI_TXT);
		label2->setColor(fonColor_CiYao);
	}
	else
	{
		CCLabelTTF* label;
		label = LF::lable(bg,"",ccp(left_x_start,87),20,fontStr_kaiti);
		label->setTag(TAG_BaseBody_LABLE_SHANGHAIXIANGXI_NUM);
		CCLabelTTF* label2;
		label2 = LF::lable(bg,""
			,ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()),20,fontStr_kaiti);
		label2->setTag(TAG_BaseBody_LABLE_SHANGHAIXIANGXI_TXT);
	}

	//等级
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, "%s %u ", LFStrings::getValue("dengji").c_str(), mEquipmentVec[index]->getEquipData().zhuangbei_level);
		label = LF::lable(bg,str,ccp(left_x_start,28),20,fontStr_kaiti);
		label->setColor(fonColor_FaGuang);
		label->setTag(TAG_BaseBody_LABLE_Level);
	}

	//人物
	{
		CCLabelTTF* label;
		int actorId = mEquipmentVec[index]->getActorId();
		if (actorId != INVALID_ID)
		{
			BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(actorId);
			CCAssert(actor!=NULL,"");
			BaseActorProperty prop = actor->getCurrentProperty();
			label = LF::lable(bg,LF::getString("zhuangBeiYu")+prop.nameInChinese,ccp(0, 0),20,fontStr_kaiti);
		}
		else
		{
			label = LF::lable(bg,LF::getString("meiYouZhuangBei"),ccp(0, 0),20,fontStr_kaiti);
		}
		label->setColor(fonColor_FaGuang);
		label->setAnchorPoint(ccp(1, 0.5));
		label->setPosition(ccp( 590, 200));
		label->setTag(TAG_BaseBody_LABLE_OWNTO);
		if (actorId == INVALID_ID && mEquipmentVec[index]->getEquipData().profession_id != 0)
		{
			CCString temp;
			temp.initWithFormat("%s(%s)", LFStrings::getValue("ZhiYeXianZhi").c_str(),
				BaseActorPropertyParser::getParser()->getExtraProerty((Profession)mEquipmentVec[index]->getEquipData().profession_id).nameInChinese.c_str());
			label->setString(temp.getCString());
			label->setColor(fonColor_JingGao);
		}
	}

	//魔法属性
	{
		int start_x = 400 + 38;
		Equipment* equip = mEquipmentVec[index];
		std::vector<std::string> vec = equip->getAllMofaShuxing(false);

		//大于5个  第五个就是 ......
		for(int i=0; i<vec.size(); ++i)
		{
			std::string txt = vec[i];

			BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
			bg->addChild(biaoji, 0, TAG_BIAOJI+i);
			biaoji->setPosition(ccp(start_x, 170-i*28));
			//biaoji->setOpacity(80);

			CCLabelTTF* label;
			label = LF::lable(bg,txt,ccp(0,0),20,fontStr_kaiti);
			label->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Fumo, false, false));
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(start_x+15, 170-i*28));
			label->setTag(i);

			if(vec.size()>6)
			{
				CCLog("Warrior:%s ---> equipment property > 6, current is [%d]", __FUNCTION__, vec.size());
			//	break;
			}
		}
		//洗练
		string xxx;
		CCSprite *diwen = (CCSprite*)bg->getChildByTag(TAG_BaseBody_TABLE_ITEM_DiWen);
		if (equip->canPurify(xxx,1) && diwen)
		{
			ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(equip);
			bg->addChild(layer,20);
			//layer->setScale(1/diwen->getScale());
			CCPoint pos = diwen->getPosition();
			pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
			pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

			int width = pos.x + layer->getContentSize().width / 2 + 12;
			int height = pos.y + layer->getContentSize().height / 2 + 4;
			layer->setPosition(ccp(width,height));
		}
	}

	bg->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	return bg;
}

unsigned int MainLayerZhuangBeiBaseBody::numberOfCellsInTableView(CCTableView *table)
{
	// 第一步
	return mEquipmentVec.size();
}

void MainLayerZhuangBeiBaseBody::scrollToIndex(int index)
{
	// index不对不操作
	if (index < 0 || index > numberOfCellsInTableView(mTableView) - 1)
		return;


	{
		// 保存所有的位置数据
		std::vector<float> m_vCellsPositions;
		{
			int cellNum = numberOfCellsInTableView(mTableView);
			 m_vCellsPositions.resize(cellNum + 1, 0.0);

			if (cellNum > 0)
			{
				float currentPos = 0;
				CCSize cellSize;
				for (int i=0; i < cellNum; i++)
				{
					m_vCellsPositions[i] = currentPos;
					cellSize = tableCellSizeForIndex(mTableView, i);
					switch (mTableView->getDirection())
					{
					case kCCScrollViewDirectionHorizontal:
						currentPos += cellSize.width;
						break;
					default:
						currentPos += cellSize.height;
						break;
					}
				}
				m_vCellsPositions[cellNum] = currentPos;//1 extra value allows us to get right/bottom of the last cell
			}
		}
		

		//  __offsetFromIndex
		CCPoint offset;
		{
			//CCSize  cellSize;
			switch (mTableView->getDirection())
			{
			case kCCScrollViewDirectionHorizontal:
				offset = ccp(m_vCellsPositions[index], 0.0f);
				break;
			default:
				offset = ccp(0.0f, m_vCellsPositions[index]);
				break;
			}
		}
		


		// _offsetFromIndex
		const CCSize curr_cellSize = tableCellSizeForIndex(mTableView, index);
		if (mTableView->getVerticalFillOrder() == kCCTableViewFillTopDown)
		{
			offset.y = mTableView->getContainer()->getContentSize().height - offset.y - curr_cellSize.height;
		}

		// 移动container，把cell放在最上面
		offset.y = -offset.y;		// cell在y处，那么container就去要移动-y才可以看得到
		// 设置位置,此时offset是刚好在最下面，需要移动到最上面
		offset.y = offset.y + mTableView->getViewSize().height - curr_cellSize.height;

		if (offset.y > mTableView->maxContainerOffset().y)
		{
			offset.y = mTableView->maxContainerOffset().y;
		}
		if (offset.y < mTableView->minContainerOffset().y)
		{
			offset.y = mTableView->minContainerOffset().y;
		}
		//
		//mTableView->setContentOffsetInDuration(ccp(offset.x,offset.y),0.1f);
		mTableView->setContentOffset(ccp(offset.x,offset.y));
	}


	//CCLog("MainLayerZhuangBeiBaseBody::scrollToIndex --> index = %d",index);

	//float min_offset = mTableView->minContainerOffset().y;		// -x
	//float max_offset = mTableView->maxContainerOffset().y;		// 0
	//float offset_y = -min_offset;

	//for (int i=0;i<index;i++)
	//{
	//	offset_y += tableCellSizeForIndex(mTableView,i).height;
	//}
	//mTableView->setContentOffsetInDuration(ccp(mTableView->getContentOffset().x,offset_y),10.1f);
}

void MainLayerZhuangBeiBaseBody::setEquipmentData(vector<Equipment*> equipmentVec, int flag, int specialEquip_GroupId)
{
	if (equipmentVec.size() <= 0)
	{
		string infoStr = LFStrings::getValue("Zhuangbeibao_Empty");
		UnblockWarnDialog* pDlg =UnblockWarnDialog::create(infoStr);
		CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
	}

	this->mEquipmentVec = equipmentVec; 
	sortEquipment(flag);
	mTableView->reloadData();

	{
		CCString temp;
		temp.initWithFormat("%s: %d/%u", LFStrings::getValue("Kongjian").c_str()
			, MyselfManager::getManager()->getAllEquipmentsWithOutEquipedOrChuanQi().size(), MAX_COUNTS_IN_KNAPSACK);
		if (m_zbTotalLabel) m_zbTotalLabel->setString(temp.getCString());

		if (MyselfManager::getManager()->getAllEquipmentsWithOutEquipedOrChuanQi().size() >= MAX_COUNTS_IN_KNAPSACK)
		{
			if (m_zbTotalLabel) m_zbTotalLabel->setVisible(false);
			if (m_zbFenJieTiShi) m_zbFenJieTiShi->setVisible(true);
			if (m_zbFenJieMenu) m_zbFenJieMenu->setVisible(true);
		}
		else
		{
			if (m_zbTotalLabel) m_zbTotalLabel->setVisible(true);
			if (m_zbFenJieTiShi) m_zbFenJieTiShi->setVisible(false);
			if (m_zbFenJieMenu) m_zbFenJieMenu->setVisible(false);
		}
	}
}

void MainLayerZhuangBeiBaseBody::menuitemClicked_GotoFenJie(CCObject *pSender)
{
	GongJiangFenJie *layer = GongJiangFenJie::create();
	mFooterForPop->popUpContaintLayer(layer);
}

void MainLayerZhuangBeiBaseBody::sortEquipment(int flag)
{
	//xuanze排序
	for(int i=0; i!=mEquipmentVec.size(); ++i)
	{
		for (int j=i+1; j!=mEquipmentVec.size(); ++j)
		{
			// 1. color
			if(mEquipmentVec.at(i)->getEquipColor() < mEquipmentVec.at(j)->getEquipColor())
			{
				swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
				continue;
			}
			else if(mEquipmentVec.at(i)->getEquipColor() > mEquipmentVec.at(j)->getEquipColor())
				continue;
			else
			{// color equil
				// 2. level
				if(mEquipmentVec.at(i)->getLevel() < mEquipmentVec.at(j)->getLevel())
				{
					swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
					continue;
				}
				else if(mEquipmentVec.at(i)->getLevel() > mEquipmentVec.at(j)->getLevel())
					continue;
				else
				{//level equil   
					// 3. type  	(EquipType_WuQi = 0, EquipType_FangJu, EquipType_ShiPin,)
// 					if(mEquipmentVec.at(i)->getEquipType() < mEquipmentVec.at(j)->getEquipType())
// 						continue;
// 					else if(mEquipmentVec.at(i)->getEquipType() > mEquipmentVec.at(j)->getEquipType())
// 					{
// 						swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
// 						continue;
// 					}
// 					else
					{
						// 4. _in type  ()
						if(mEquipmentVec.at(i)->getType() < mEquipmentVec.at(j)->getType())
							continue;
						else if(mEquipmentVec.at(i)->getType() > mEquipmentVec.at(j)->getType())
						{
							swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
							continue;
						}
						else
						{
							// 5  同类装备、  名字相同--
							if (strcmp(mEquipmentVec.at(i)->getName().c_str(), mEquipmentVec.at(j)->getName().c_str()) < 0)
								continue;
							else if (strcmp(mEquipmentVec.at(i)->getName().c_str(), mEquipmentVec.at(j)->getName().c_str()) > 0)
							{
								swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
								continue;
							}
							else 
							{
								// 6. 同等级， 同成色， 同类型    则按白字排序
								float iBaizi = 0;
								float jBaizi = 0;
								{
									switch(mEquipmentVec[i]->getType())
									{
									case ZhuangbeiType_DanshouJian:
									case ZhuangbeiType_ShuangshouBishou:
									case ZhuangbeiType_ShuangshouFu:
									case ZhuangbeiType_DanshouFazhang:
									case ZhuangbeiType_ShuangshouFazhang:
									case ZhuangbeiType_Gong:
										iBaizi = 
											(mEquipmentVec[i]->getEquipData().gongji_min+mEquipmentVec[i]->getEquipData().gongji_max) / 2.0;
										jBaizi =
											(mEquipmentVec[j]->getEquipData().gongji_min+mEquipmentVec[j]->getEquipData().gongji_max) / 2.0;
										break;
									case ZhuangbeiType_Dunpai:
										//case ZhuangbeiType_Toukui:
									case ZhuangbeiType_KuijiaZhong:
									case ZhuangbeiType_KuijiaQin:
										//case ZhuangbeiType_KuziZhong:
										//case ZhuangbeiType_KuziQin:
										//case ZhuangbeiType_Xuezi:
										iBaizi = mEquipmentVec[i]->getEquipData().hujia;
										jBaizi = mEquipmentVec[j]->getEquipData().hujia;
										break;
									case ZhuangbeiType_Jiezhi:
									case ZhuangbeiType_Hushenfu:
									case ZhuangbeiType_Faqi:
									default:
										break;
									}
								}
								if(iBaizi < jBaizi)
									swap(mEquipmentVec.at(i), mEquipmentVec.at(j));
							}// 6.
						}// 5.
					}// 4.
				}// 3.

			}// 2.

		}// 1.
	}// for loop end



	//分解装备 则是倒排序(由小到大) //分解 和萃取、、、
	if(flag == SORT_IN_FenJie)
	{	
		reverse(mEquipmentVec.begin(),mEquipmentVec.end());
		return;
	}

/*	//工匠界面 则将已经装备的放在前面
	if(flag == SORT_IN_GongJiang)
	{
		int num=0;
		for(int i=0; i!=mEquipmentVec.size(); ++i)
		{
			vector<Equipment*>::iterator it = mEquipmentVec.begin();
			int actorId = mEquipmentVec.at(i)->getActorId();
			if (actorId != INVALID_ID)
			{
				Equipment *eqiup = mEquipmentVec.at(i);	
				mEquipmentVec.erase(it+i);
				it = mEquipmentVec.begin();
				mEquipmentVec.insert(it+num, eqiup);
				++num;
			}
		}
	}
*/
}
