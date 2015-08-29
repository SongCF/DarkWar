#pragma once

#include "BaseLayer.h"
#include <stack>
#include "MainHeaderLayer.h"
#include "MainFooterLayer.h"
using namespace std;

enum PopUpPos
{
	Pop_Pos_Center = 0,	// 居中，在footer和header之间居中
	Pop_Pos_FromFoot,	// 从footer开始对齐
	Pop_Pos_WondowDown, // 从屏幕最下方开始对其，隐藏footer
};

// 主界面下面的选择条
class MainLayerBase : public BaseLayer,FooterCallback
{
private:
	MainHeaderLayer* mHeader;
	MainFooterLayer* mFooter;

	BaseLayer* selectedContaint;

	stack<BaseLayer*> popLayerStack;

	void resetContentBodyPos(CCLayer* content,PopUpPos pos = Pop_Pos_Center);

	virtual void footerItemSelected(enum_MainLayerIndex index);

	// 创建第index页的内容
	BaseLayer* createContentAtIndex(enum_MainLayerIndex index);
protected:
	virtual void onEnter();
	virtual void onExit();
public:
	MainLayerBase(void);
	~MainLayerBase(void);

	// 获取当前的MainLayerBase实例
	// 如果当前内存没有这个实例，那么返回NULL
	static MainLayerBase* getCurrentMainBase();

	//// 添加一个根页面，会把当前刚才显示的跟页面推开，加入一个新页面
	//void addOneContentLayer(BaseLayer* content);
	//// 移除当前的跟页面，返回到上一个跟页面。
	//void removeCurrentContentLayer();

	// 设置当前页面的内容 body （修改根页面）
	void setContaintLayer(BaseLayer* content,bool removeHead = true);
	// 在当前页面浮出一个Layer
	void popUpContaintLayer(BaseLayer* content,PopUpPos pos = Pop_Pos_Center);
	// 在当前页面销毁浮出界面
	void dismmisPopUp();
	// 在当前页面移除所有pop出来的页面，只剩下根页面,并激活根页面
	void dismmisAllPop();
	// 在当前页面移除所有pop出来的页面，只剩下根页面
	void removeAllPop();
	// 获取当前页面的内容区大小
	CCSize getMaxContentBodySize();

	BaseLayer* getTopLayer(int backIndex = 0);
	BaseLayer* getContentLayer(){return selectedContaint;};

	MainFooterLayer* getFooter(){return mFooter;};
	MainHeaderLayer* getHeader(){return mHeader;};
};

