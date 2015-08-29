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
	//��д�����;
	//����һ��Item�����ݴ����ҳ��ţ�x(�����ҵĺ�)��y(���ϵ��µĺ�)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex);

	//��д��Ϊ�����ػ����¼�����֪ͨMaskLayer�Ƿ���ʾ��һҳ����һҳ�İ�ť
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
	//���������ң���ôĬ���ǷŴ�
	virtual void touchMe()
	{
	}
	//��ָ�뿪���ң�Ĭ���Ǳ��ԭ��С
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