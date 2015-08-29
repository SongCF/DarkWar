#pragma once

#include "SegmentedLayer.h"
#include <vector>
#include "ListViewLayer.h"

/**
 *	选择英雄的界面
 */
class ChooseHerosLayer : public SegmentedLayer
{
private:
	void itemClicked(CCObject* sender);

	unsigned int mSelectedHeroId;
	unsigned int mDefautHeroId;

	BaseSprite* photo;

public:
	ChooseHerosLayer(unsigned int defaultId);

	// 选择哪个
	void selectHeroById(unsigned int heroId);

	~ChooseHerosLayer(void);
};


class HeroListView : public ListViewLayer,ListViewCreator
{
private:
	ChooseHerosLayer* mLayer;

	int selectedIndex;
	vector<HeroInfo> mDataVec;

	// 创建第i个元素
	virtual BaseSprite* getItemContentByIndex(unsigned int index);
	// 刷新第i个元素，也可以简单的用getItemContentByIndex，再创建一个就是了
	virtual void refreshItem(CCNode* item,int index);

	virtual void listItemTouched(CCNode* listItem,int index);

public:
	HeroListView(ChooseHerosLayer* layer,unsigned int selectedHeroId);
};

