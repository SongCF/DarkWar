//@ npc body layer
//@ used to "冒险" item clicked by player
//@ by _Richard


#pragma once
#include "ListViewLayer.h"
#include "IMainLayerBody.h"
#include "BaseLayer.h"

// foot选项卡的NPC内容
class NPCBodyLayer : public ListViewLayer,ListViewCreator,public IMainLayerBody
{
private:
	BaseLayer* mContentLayer;

	int mFlag;
protected:

//	void menuItemClicked(CCObject *pSender);
	void setContentLayer(int tag);

	void queryMainTaskReward_Rsp(CCObject *f_msg);

public:
	MainLayerBase* getFooter(){ return mFooterLayer;}


	virtual void initFoot(MainLayerBase* footerLayer);
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSize(640,839);}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(320, 419);}
public:
	NPCBodyLayer(int flag = 0);
	~NPCBodyLayer(void);
};

