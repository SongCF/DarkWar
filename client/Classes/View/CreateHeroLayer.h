#pragma once
#include "SegmentedLayer.h"
#include "scrollView/GridScrollView.h"

// 来自创建完毕之后的目的地
// 
enum CreateHero_Dest
{
	CreateHero_Dest_None = 0,	// 没有目的地
	CreateHero_Dest_DefaultMainHeroLayer = 1,
	CreateHero_Dest_UseThisToMain = 2,
	CreateHero_Dest_ChooseHerosLayer = 3,
};

/**
 *	创建英雄界面,请直接pushscene的方式调用，因为返回按钮就直接pop了。
 */
class CreateHeroLayer : public SegmentedLayer,ClickableItemCreater,CCEditBoxDelegate
{
private:

	int currentSelected;
	CreateHero_Dest mDest;
	CCArray* jueSheItems;
	GridScrollView* heroGallery;
	BaseSprite* jieShao;
	BaseSprite* jieshaotype;
	BaseSprite* headBiaoJi;
	CCMenuItemImage* ok;
	CCMenuItemImage* suijimz;
	CCEditBox* cutomNameBox;
	CCLabelTTF *zhiyejiesha;
	BaseSprite* s_bg;
	string zhiyetext;
	void itemClicked(CCObject* sender);
	bool sex;

	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex);
	// 页面选择了
	virtual void pageSelectedEnd(int pageIndex);

	/**
     * This method is called when the edit box text was changed.
     * @param editBox The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxReturn(CCEditBox* editBox);

private:
	void createHeroWithTypeId(int typeId,string custName);
	void createHeroOver(CCObject* msg_rsp);
	void setSelectByIndex(int index);
	vector<string> parse(string sin);
	CCLabelTTF* getxyLength(string text);
public:

	CreateHeroLayer(CreateHero_Dest dest);
	~CreateHeroLayer(void);
};


class ClickableItemActor : public ClickableItem
{
public:
	ClickableItemActor()
	{
	}

	virtual void touchMe()
	{
	}
};

