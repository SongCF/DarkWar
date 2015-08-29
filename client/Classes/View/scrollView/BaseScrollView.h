#ifndef __BaseScrollView_H__
#define __BaseScrollView_H__

#include "cocos2d.h"
#include "MyScrollView.h"
#include "BaseAdapter.h"
#include "ClickableItem.h"

class BaseScrollView  : public MyScrollView
{
public:
	BaseScrollView(BaseAdapter* adapter,CCSize viewPortSize);
	~BaseScrollView();
	void setAdapter(BaseAdapter* adapter);
private:
	BaseAdapter* adapter;
	int cal_currentShowedPageIndex;
	int cal_PageNum;
	float pageInval;
	float pageScrollTime;
	int itemNum;
	//���������ֻ�ǵ�����¼����ͻ�����������
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    
	virtual void intentToScrollToRightPage();//��ͼ�����ұ�ҳ
	virtual void intentToScrollToLeftPage();//��ͼ�������ҳ
	virtual void intentToScrollToCurrentPage();
	//��Grid�е�ĳ��Item�����֮��ͻ��������������ⷽ��ֱ�ӵ���Item��ClickMeȥʵ��
	virtual void itemClicked(ClickableItem* clickedObj,int index); 
};


#endif