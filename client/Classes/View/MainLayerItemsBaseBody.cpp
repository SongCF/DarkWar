
#include "MainLayerItemsBaseBody.h"
#include "LFUtile.h"
#include "BaseActorPropertyParser.h"
#include "CS_Common.h"

#include "font.h"

#define LABEL_SIZE 20

// 06-15 11:43:50.179: D/cocos2d-x debug info(1716): Get data from file(tabulation_armor_helmet_1.png) failed!
MainLayerItemsBaseBody::MainLayerItemsBaseBody(void)
{
	mTableView = CCTableView::create(this, CCSizeMake(613,681));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(34,40));
	mTableView->setDelegate(this);
	addChild(mTableView);
	mFooterForPop = NULL;
}


MainLayerItemsBaseBody::~MainLayerItemsBaseBody(void)
{
}


void MainLayerItemsBaseBody::scrollViewDidScroll(CCScrollView* view) 
{
}
void MainLayerItemsBaseBody::scrollViewDidZoom(CCScrollView* view) 
{
}

void MainLayerItemsBaseBody::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
}
void MainLayerItemsBaseBody::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
	//CCLog("tableCellWillRecycle");
}
void MainLayerItemsBaseBody::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
}

void MainLayerItemsBaseBody::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
}

CCSize MainLayerItemsBaseBody::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(622, 148);
}


void MainLayerItemsBaseBody::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
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
}
void MainLayerItemsBaseBody::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
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
}

CCTableViewCell* MainLayerItemsBaseBody::tableCellAtIndex(CCTableView *table, unsigned int idx)
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

	cell->addChild(getItemContentByIndex(idx),0,TAG_BaseBody_TABLE_ITEM);


	return cell;
}

BaseSprite* MainLayerItemsBaseBody::getItemContentByIndex(unsigned int index)
{

	BaseSprite* bg = BaseSprite::create("shangdian_shangpin_bg.png");
	//CCPoint picPos = ccp(72, bg->getContentSize().height/2);
	unsigned int item_type = mItemsVec[index].type;
	//borad
	{
		BaseSprite *sprite = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mItemsVec.at(index).type));
		sprite->setPosition(ccp(72, bg->getContentSize().height/2));
		bg->addChild(sprite);
	}

	//item图片
	{
		BaseSprite *sprite = BaseSprite::create(ImageManager::getManager()->getItemFilename(item_type));
		sprite->setPosition(ccp(72, bg->getContentSize().height/2));
		bg->addChild(sprite,0,TAG_BaseBody_TABLE_ITEM_Image);
		
	}

	// 名字
	{
		CCLabelTTF* lable = LF::lable(bg,CS::getItemName(item_type),ccp(142,112),LABEL_SIZE,fontStr_kaiti);
		lable->setTag(TAG_BaseBody_LABLE_NAME);		
	}

	//属性
	{
		//CCLabelTTF* lable = CCLabelTTF::create(CS::getItemShuxing(item_type).c_str(), fontStr_kaiti,20,CCSize(300, 54), kCCTextAlignmentLeft);
		CCLabelTTF* lable = CCLabelTTF::create(CS::getItemShuxing(item_type).c_str(), fontStr_kaiti,LABEL_SIZE,CCSize(300, 0), kCCTextAlignmentLeft);
		bg->addChild(lable);
		lable->setAnchorPoint(ccp(0, 0.5));
		lable->setPosition(ccp(142, 70));
		lable->setTag(TAG_BaseBody_LABLE_SHUXING);
		//CCSize size = lable->getContentSize();
		ccColor3B color;
		color.r = 201;
		color.g = 156;
		color.b = 18;
		lable->setColor(color);

		
	}
	
	//数量
	{
		CCLabelTTF* lable = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,36),LABEL_SIZE,fontStr_kaiti);
		lable->setTag(TAG_BaseBody_LABLE_COUNT);
		ccColor3B color;
		color.r = 89;
		color.g = 111;
		color.b = 183;
		lable->setColor(color);
	}

	//数量数值
	{
		char str[32];
		sprintf(str, "%d",mItemsVec[index].count);
		CCLabelTTF* lable = LF::lable(bg, str,ccp(202,36),LABEL_SIZE,fontStr);
		lable->setTag(TAG_BaseBody_LABLE_COUNT_NUM);
	}
	
	bg->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	return bg;
}

unsigned int MainLayerItemsBaseBody::numberOfCellsInTableView(CCTableView *table)
{
	
	return  mItemsVec.size();
}
