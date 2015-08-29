#ifndef __GRID_SCROLLVIEW_H__
#define __GRID_SCROLLVIEW_H__

#include "cocos2d.h"
#include "MyScrollView.h"
#include "ClickableItem.h"
#include "ClickableItemCreater.h"
using namespace cocos2d;
//һ�����񻯵�ScrollView����������ң����ϵ��µĴ��;
class GridScrollView  : public MyScrollView
{
protected:
	CCArray* gridItemArray;//�����д�ŵ�Ԫ�ص�����,������CCSprite*����;

	float paddingLeft;//������������ľ��ο�������ScrollPort���������
	float paddingRight;//������������ľ��ο�������ScrollPort������Ҽ��
	float paddingTop;//������������ľ��ο�������ScrollPort������ϼ��
	float paddingButtom;//������������ľ��ο�������ScrollPort������¼��

	CCSize mScrollViewPortSize;//ScrollPort����Ĵ�С
	//CCSize gridSize;//������������ľ��ο��С��mScrollViewPortSize����һ��gridSize
	CCSize winSize;

	CCPoint scrollViewPortOffset;//ScrollPort�������ԭ���ƫ��

	int gridNumInX;//������X�����ϵĸ��Ӹ���
	int gridNumInY;//������Y�����ϵĸ��Ӹ���
	int gridNum;//����ĸ��Ӹ���,Ҳ����ScrollView�У�һ��Ҫ���Item�ĸ���
	float pageInval;//ҳ��֮��ļ���Ƕ���
	float pageScrollTime;

	int cal_PageNum;//����ʱ�������ҳ�����;
	int cal_gridNumInOnePage;//����ʱ�������ÿһҳ�ж��ٸ�����
	int cal_currentShowedPageIndex;//��ǰ��ʾ��������һҳ;

	//float pageSizeX;//�����Ӿ���СmScrollViewPortSize.width
	CCSize cal_containerSize;
	CCSprite* container;//������CCSprite

	//���������ֻ�ǵ�����¼����ͻ�����������
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    
	virtual void intentToScrollToRightPage();//��ͼ�����ұ�ҳ
	virtual void intentToScrollToLeftPage();//��ͼ�������ҳ
	virtual void intentToScrollToCurrentPage();
	//��Grid�е�ĳ��Item�����֮��ͻ�����������������û���Ҫ������ô������д�������,Ĭ���ǽ���Clickable
	//һ�㲻����д
	virtual void itemClicked(ClickableItem* clickedObj); 
	////���λ������ͼ�ķ�Χ��
	//virtual void clickedOutOffViewPort();

	virtual CCSprite* createContainner();//��������,�ڲ���������Զ�������С
	
	// ���ĳһҳѡ�еĻ���containerӦ�õ�Offset��
	float getPageOffsetX(int pageIndex);

	/**
	 *	��������ĳ��ҳ�棬�ͻ���ã���һ���ص��ӿ�
	 *	�е����ҳ�����ͼ��Ҳ�ᷢ�͵ģ�Ȼ��Ϳ�ʼ�����ƶ���ȥ
	 *	����ֱ�ӵ�����ȥ
	 */
	virtual void pageSelected(int pageIndex);
	/**
	 *	���������
	 */
	virtual void pageSelectedEnd(int pageIndex);

	//����itemToStore������������������뵽itemToStore��
	//���Item��ScrollVIew�е����񲼾֣�һ�㲻��Ҫ�޸�;
	virtual void initGridItem(CCArray* itemToStore,int gridNumInX,int gridNumInY);
public:
	//����һ�����񣬿ɷ�ҳ��ScrollView����Ҫָ����������Ŀ��������С�������������У����ӵĸ���������ĺ��������ĸ��Ӹ���
	//Ĭ�����������ӿڵ����ģ��ӿ�Ҳ������Ļ������;
	//gridPadding ����һҳ�е�Grid��ռ�õ�ҳ���padding
	GridScrollView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding = 10.0f,CCPoint scrollViewPortOffset = CCPointZero,float pageInval = 40.0f);
	~GridScrollView();

	//׼����Ͼ͵����������ȥ��ʾ
	//�������֮����������޸�pading֮��ģ�����ҪrefactorGridSize
	void prepareToDisplay();
	//����Item��������
	void setClickableCreater(ClickableItemCreater* creater);
	void setPadding(float padding,bool refactorGridSize = true);		//���� ������������ľ��ο�������ScrollPort��������ܼ��
	void setPaddingRight(float padding,bool refactorGridSize = true);	//���� ������������ľ��ο�������ScrollPort������Ҽ��
	void setPaddingLeft(float padding,bool refactorGridSize = true);	//���� ������������ľ��ο�������ScrollPort���������
	void setPaddingTop(float padding,bool refactorGridSize = true);		//���� ������������ľ��ο�������ScrollPort������ϼ��
	void setPaddingButtom(float padding,bool refactorGridSize = true);	//���� ������������ľ��ο�������ScrollPort������¼��
	void setGridNumInX(int gridNumInX);									//��������ĺ�����Ӹ���
	void setGridNumInY(int gridNumInY);									//���������������Ӹ���

	void setPageScrollDur(float dur);									//���÷�ҳ��ʱ

	//�򵥵ķ�ҳ�����Ǹ��ݵ�ǰ��ҳ���С��ҳ������ҳ�������scrollToPage�ǿ��Զ�̬��ָ��ҳ���С��ҳ��������ҳ
	// 
	void scrollToPageSimple(int pageIndex,float time);

	// ��ҳ��ʹ��Ĭ�ϵ��ٶ�
	void scrollToPageSimple(int pageIndex);

	//����gridIndex���ҵ��ð�ť���ڵ�ҳ��,ע��index��Щ���Ǵ�0��ʼ�ģ�
	int getPageIndex(int gridIndex);
	// ��ø�Ԫ����Grid�е�ҳ��λ��,-1����û���ҵ�
	int getPageIndex(CCSprite* item);
	//����ĳҳ��Ԫ�ص�͸����(ֻ��GirdItem��͸����)
	void setPageItemOpacity(int pageIndex,GLubyte opacity);
	//����page��ĳһ��Ԫ�ص�͸����
	void setGridItemOpacity(CCSprite* item,GLubyte opacity);

	CCArray* getPageItems(int pageIndex);			// ���ĳҳ������Item

private:
	//���¹����Լ���������padding���������Եȶ���Ҫ��������������Ż���Ч
	void reBuildSelf();
	ClickableItemCreater* itemCreator;
};

#endif