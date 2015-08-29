#ifndef __ChuanqiduanzaoLayerScrollView_h_
#define __ChuanqiduanzaoLayerScrollView_h_

#include "cocos2d.h"
#include "ChuanqiduanzaoLayerScrollView.h"
#include "scrollView/GridScrollView.h"
#include "BaseActor.h"

class ChuanqiduanzaoLayerScrollView : public GridScrollView, public ClickableItemCreater
{
public:
	ChuanqiduanzaoLayerScrollView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding = 10.0f,CCPoint scrollViewPortOffset = CCPointZero,float pageInval = 40.0f);
	~ChuanqiduanzaoLayerScrollView(void);

	static ChuanqiduanzaoLayerScrollView* createView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding = 10.0f, CCPoint scrollViewPortOffset = CCPointZero,float pageInval = 40.0f);

	int getCurPageIndex(){return m_iCurPageIndex;};
protected:
	//重写父类的;
	//创建一个Item，根据传入的页面号，x(从左到右的号)，y(从上到下的号)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex);

	//重写，为了拦截滑动事件，以通知MaskLayer是否显示上一页，下一页的按钮
	virtual void pageSelected(int pageIndex){};
	virtual void pageSelectedEnd(int pageIndex);

private:
	int		m_iCurPageIndex;
};

class ChuanqiduanzaoItem : public ClickableItem
{
	enum 
	{
		Tag_Board = 10, 
		Tag_Menu,
		Tag_MenuItem_Yici,
		Tag_MenuItem_Shici,
	};


public:
	ChuanqiduanzaoItem();
	~ChuanqiduanzaoItem();

	static ChuanqiduanzaoItem* createChuanqiduanzaoItem(BaseActor* info);
	bool initItem(BaseActor* info);
protected:
	virtual void clickedMe()
	{
	}
	//触摸到了我，那么默认是放大
	virtual void touchMe()
	{
	}
	//手指离开了我，默认是变回原大小
	virtual void leaveMe()
	{
	}

private:
	string getIconFileNameByType(int iType);
	string getHeroTypeFileNameByType(int iType);
	void menuItemClicked_DuanZao(CCObject* pObj);
	void rsp_DuanZao(CCObject *msg_f);
	void menuItemClicked_Xiaoguo(CCObject* pObj);

	Profession m_Profession_id;
};

#endif // __ChuanqiduanzaoLayerScrollView_h_