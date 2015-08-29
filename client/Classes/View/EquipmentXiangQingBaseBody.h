#pragma once

#include "BaseLayer.h"
#include "Equipment.h"
#include "IMainLayerBody.h"
#include "cocos-ext.h"
#include "GalleryLayer.h"
using namespace cocos2d::extension;

class BaseActor;
class RenWuXiangXiBaseLayer;

enum ZhuangbeiXiangQingFrom
{
	From_RenXingMianBan = 0,	//“有持有人”状态 --- zhaungbeiMianBanBase 使用的   
	From_ZhuangBeiList,			//“无持有人”状态 --- 装备list里面点击图片跳向EquipmentXiangQingBody就是这个参数
	From_ChangeZhuangBei,		//“目的装备”状态 --- changeZhuangbeiBody使用的，同样，点击了图片一样是From_ChangeZhuangBei
	From_GongJiang,				//“工匠请求”状态 --- 其他的地方，比如人形界面


	From_OtherHeroDetail,          // 查看其他人的装备时
	From_SpecialScene,             // 不在GameScene框架下，特殊处理。 dismiss
	From_ZhuangBeiDuanZao,         // 不显示关闭按钮
};


class EquipmentXiangQingBaseBody : public BaseLayer, public IMainLayerBody, public GalleryCallBack
{
protected:
	unsigned int mActorId;
	BaseSprite* mBoard;
	ZhuangbeiXiangQingFrom mFrom;
	Equipment* mEquip;
	CCMenu* mMenu;
	CCMenu *mGongJiangMenu;
	float m_Label_y;

	virtual void menuItemClicked(CCObject* sender);
	virtual void menuItemGongJiang(CCObject *pSender){};
	void schedule_initLayer(float dt);
	virtual void menuItemXilian(CCObject *pSender){};

	bool isDisplay_YuanShiDuiBi_Item();

	virtual void popOneChildLayer(RenWuXiangXiBaseLayer* layer);
	virtual void dismissMyselfLayer(bool someThingChanged=false);
	virtual void replaceMyselfLayer(BaseLayer* layer);

	//void reportError(CCObject* msg_null);
public:
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	//得到body的大小
	virtual CCSize getBodySize(){return mBoard->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return mBoard->getPosition();}
	void createEquipPropertyUI(Equipment* equip,BaseSprite* base,CCPoint startPos);
	void createEquipPropertyUI(float dt);
	void createTaoZhuangShuXing(float dt);
	// 简单的创建一个比较单元，上升就有加好，下降就有减号
	void addCompareInfoLabel(CCNode* node, int curData,int newData,CCPoint fontLeftPos,float fontSize,string fontName);

public:
	virtual void reportChangeZhuangBeiRsp(CCObject* msg_rsp);


	EquipmentXiangQingBaseBody(Equipment* equip, unsigned int actorId,ZhuangbeiXiangQingFrom from);
	~EquipmentXiangQingBaseBody(void);
	bool init(Equipment* equip, unsigned int actorId,ZhuangbeiXiangQingFrom from);
	virtual void refreshLayer();


	Equipment* getEquip(){return mEquip;}
	unsigned int getActorId(){return mActorId;}
	ZhuangbeiXiangQingFrom getFrom(){return mFrom;}
};
