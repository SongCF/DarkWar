#pragma once

#include "BaseLayer.h"
#include "Equipment.h"
#include "cocos-ext.h"
#include "GalleryLayer.h"
#include "EquipmentXiangQingBaseBody.h"
#include "EquipmentTaoZhuangXiangQing.h"

using namespace cocos2d::extension;

class BaseActor;
class RenWuXiangXiBaseLayer;



// 装备详情界面,应该选择压入的方式，他只是一个查看页面，不需要替换原有的，而是直接
// push进来
class EquipmentXiangQingBody : public EquipmentXiangQingBaseBody, public TaoZhuangLayerInterface
{
protected:
	virtual void popOneChildLayer(RenWuXiangXiBaseLayer* layer);
	virtual void dismissMyselfLayer(bool someThingChanged=false);
	virtual void replaceMyselfLayer(BaseLayer* layer);

	virtual void menuItemClicked(CCObject* sender);
	virtual void menuItemGongJiang(CCObject* sender);

	void refresh_YuanShiBiaoJiao_Layer(BaseSprite* board);
	void create_YuanShiBijiao_Item(BaseSprite *base, Equipment *equip);
	void menuItemClicked_FanHui(CCObject *pSender);

	void menuItemClicked_JieFeng(CCObject *pSender);
	void rsp_shenBingJieFengYin(CCObject *msg);
	void menuItemClicked_JianDing(CCObject *pSender);
	void rsp_shenBingJianDing(CCObject *msg);
	
public:
	//教学
	virtual void onEnter();
	void teachGuide();
	void equipLevelUpGuide();

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	//套装界面回调
	virtual void callbackInterface_TaoZhuangLayer();
	
public:
	virtual void reportChangeZhuangBeiRsp(CCObject* msg_rsp);

	EquipmentXiangQingBody(Equipment* equip, unsigned int actorId,ZhuangbeiXiangQingFrom from)
		:EquipmentXiangQingBaseBody(equip, actorId, from)
	{
	};
	~EquipmentXiangQingBody(void){};

	// 得到body的大小
	virtual CCSize getBodySize(){return EquipmentXiangQingBaseBody::getBodySize();}
	//
	virtual CCPoint getBodyCenterPos(){return EquipmentXiangQingBaseBody::getBodyCenterPos();}

private:
	void chatEndCallBack();
};
