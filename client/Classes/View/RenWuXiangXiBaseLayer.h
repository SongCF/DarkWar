#pragma once
#include "BaseLayer.h"
#include "BaseActor.h"
#include "JiNeng_SelectLayer.h"

enum 
{
	Tag_Item_JiNengBoard = 1,
	Tag_Item_Back = 2,
};

class RenWuXiangXiBaseLayer : public BaseLayer
{
protected:
	BaseSprite* mBg;
	CCMenu *mMainMenu;
	BaseActor* mActor;
	CalculateData mNewData;
	JiNengLayer_comeFrom mFrom;
	bool mIsYuanShen;

	//
	void createCmpareItem(BaseSprite* contaner,int curData,int newData,CCPoint fontLeftPos,float fontSize,string fontName,bool percent = false);
	// 创建显示数据
	// 如果有基准值base就显示比较结果的上升，下降
	void createData(BaseSprite* bg,BaseActor* actor,bool useCmp = false);

	virtual void menuItemClicked(CCObject* sender);

	void initWithData(BaseActor* actor,CalculateData newData,bool useCmp = false, JiNengLayer_comeFrom fro = comefrom_JiuGuan,bool isYuanShen=false);
	void refreshSkill();
	string getZhuShuXingStrByProfession(Profession profId);

	virtual void onEnter();
protected:
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();};// 禁用触摸
	virtual void disableAllTouchBegin();
	// 启用触摸
	virtual void disableAllTouchEnd();
public:
	RenWuXiangXiBaseLayer(BaseActor* actor,JiNengLayer_comeFrom fro,bool isYuanShen=false);
	RenWuXiangXiBaseLayer(BaseActor* actor,CalculateData newData,JiNengLayer_comeFrom fro,bool isYuanShen=false);
	BaseSprite* getBaseNode(){return mBg;};
	//
	void  createCCProgressTimer(int isZhensen,BaseActor* actor,CCLabelTTF *p1,CCLabelTTF *p2);
	~RenWuXiangXiBaseLayer(void);

	BaseActor* getActor(){return mActor;};
	CalculateData getNewData(){return mNewData;};
	void addBckgroud(float start,float end,float hight);
	float adaptivex;
	float adaptivey;
};

