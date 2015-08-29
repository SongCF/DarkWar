
#pragma once

#include "cocos2d.h"
#include <vector>
using namespace cocos2d;
using std::vector;

#define TAG_GetAward_Layer 4643+10006

//和 AwardDisplayerLayer  类似、 但是此layer有领取按钮 ，点击后才能领取

struct ItemInfo;
struct Flat_DuanzaoGroupZBInfo;

//simlarity lfalert
class GetAwardLayer : public CCLayerColor
{
public:
	static GetAwardLayer* create(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips = vector<Flat_DuanzaoGroupZBInfo>());
	GetAwardLayer(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips);
	void setMenuContainer(CCMenuItem* item);


	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected:
	CCSprite *mBoard;
	CCMenu *mMenu;
};
