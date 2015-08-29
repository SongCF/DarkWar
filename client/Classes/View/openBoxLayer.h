
#pragma once
#include "cocos2d.h"
#include "protocol.h"
#include "vector"
using std::vector;

using namespace cocos2d;

#define  TAG_OpenBox_Layer  4643+1000


enum OpenBoxFrom
{
	  fromJingJiChang = 0
	, fromTuanDui = 1
};

class OpenBoxLayer : public CCLayerColor
{
public:
	OpenBoxLayer(OpenBoxFrom from, int openBonNum, vector<GiftData>& gettedData, vector<GiftData>& missedData);
	~OpenBoxLayer();

	static OpenBoxLayer* create(OpenBoxFrom from, int openBonNum, vector<GiftData> gettedData, vector<GiftData> missedData);
	bool init();

	void getGiftRsp(CCObject* msg);
	//对应位置
	/*  678
	**  345
	**  012  */	  
	void openBox(int idx);
	void displayAward(CCNode* node);
	void refreshLabel();

	void menuItemOkClicked(CCObject* pSender);

	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);


protected: 


	bool mOpenning;  //正在开第一个时，不能开第二个

	int mNum;          //只能打开这么多个
	int mNowOpenedNum;  //当前开到第几个
	bool mOpened[9];
	int mNowOpenedBoxIdx;
	vector<GiftData> mGettedData;
	vector<GiftData> mMissedData;



	OpenBoxFrom mFrom;
};