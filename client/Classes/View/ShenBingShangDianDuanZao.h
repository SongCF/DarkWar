#pragma once
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "flat.h"
#include "CommonDefine.h"

class ShenBingShangDianDuanZao : public BaseLayer
{
public:
	ShenBingShangDianDuanZao(vector<EquipData> equips);

	//全局函数
// 	//zOrder    -1详情边框光效  -2不动光   -3缓慢转动光   -4忽明忽暗光   不是-1都要变大一倍
// 	BaseSprite* getDuanZaoChengGong_EffectSprite(int color, int idx, float actionDelayTime);
	void effectEndCallback(CCNode *node, void *data);
	void display10Equip();

	void menuItemClicked_EquipImg(CCObject *pSender);

	///////////////////////////////////////////////////////////////////////////////////////////
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
	//////////////////////////////////////////////////////////////////////////////////////////

private:
	BaseSprite *mBg;

	int mCount;
	vector<EquipData> mEquips;
};
