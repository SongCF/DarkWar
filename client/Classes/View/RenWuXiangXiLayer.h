#pragma once
#include "BaseLayer.h"
#include "BaseActor.h"
#include "IMainLayerBody.h"
#include "RenWuXiangXiBaseLayer.h"

class RenWuXiangXiLayer : public RenWuXiangXiBaseLayer, public IMainLayerBody
{
private:
	

	virtual void menuItemClicked(CCObject* sender);

	void initWithData(BaseActor* actor,CalculateData newData,bool useCmp = false,JiNengLayer_comeFrom fro = comefrom_JiuGuan);
protected:
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return RenWuXiangXiBaseLayer::getBodySize();};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return RenWuXiangXiBaseLayer::getBodyCenterPos();}; 
public:
	RenWuXiangXiLayer(BaseActor* actor,JiNengLayer_comeFrom fro,bool isYuanShen=false)
		:RenWuXiangXiBaseLayer(actor, fro, isYuanShen)
	{
	};
	RenWuXiangXiLayer(BaseActor* actor,CalculateData newData,JiNengLayer_comeFrom fro,bool isYuanShen=false)
		:RenWuXiangXiBaseLayer(actor, newData, fro, isYuanShen)
	{

	};

};

