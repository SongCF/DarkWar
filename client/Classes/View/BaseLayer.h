#pragma once
#include "LFLayer.h"
#include "ResourcePathManager.h"
#include "BaseSprite.h"
#include "CmdHelper.h"
//class CmdHelper;


class BaseLayer : public LFLayer
{
protected:
	CCSize mWinSize;
	CCSize mVisibleSize;
	CCPoint mVisibleOrigin;

	CCPoint mCenterPoint;
	ResourcePathManager* mResMgr;
	CmdHelper* mCmdHlp;

	vector<string> msgVec;

	// 添加消息监听
	void addObserver(SEL_CallFuncO selector,const char *name);
	// 移除消息监听
	void removeObserver(const char *name);

	virtual void onExit();
public:
	// 禁用触摸
	virtual void disableAllTouchBegin();
	// 启用触摸
	virtual void disableAllTouchEnd();

	//刷新本页面
	virtual void refreshLayer(){};

	// 移除一个Node
	void removeNode(CCNode* toRemove);

	// 修改menu的优先级，upOrdown为true就升高
	void changeMenuHandlerPriority(CCMenu* menu,bool upOrDown);

	// 降低menu的优先级
	void jiangDiMenuHandlerPriority(CCNode* menu);

	// 升高menu的优先级
	void shengGaoMenuHandlerPriority(CCNode* menu);

	// runaction  迟一帧调用！ add by scf
	//Menu->runAction(CCCallFuncND::create(this, callfuncND_selector(BaseLayer::changeMenuPriority_callfuncND), (void*)(-129)));
	void changeMenuPriority_callfuncND(CCNode* node, void *priority)
	{
		CCMenu *menu = dynamic_cast<CCMenu*>(node);
		if (menu)
		{
			menu->setHandlerPriority((int)priority);
		}
	}


	BaseLayer(void);
	~BaseLayer(void);
};
