#ifndef __MyScrollView_H__
#define __MyScrollView_H__

#include "cocos2d.h"
#include <vector>
#include "cocos-ext.h" 
USING_NS_CC; 
USING_NS_CC_EXT; 
using namespace std;
//
//һ������չ��CCScrollView�ࡣ����ʵ�ֶ�ֻ�����ͻ����ķֿ�
//���ȫ��GL����ת����ScrollView���ϵı���ֲ�������꣨ԭ�������½ǣ�
//
class MyScrollView: public CCScrollView
{
protected:
	CCPoint convertToScrollViewRelativePoint(CCPoint& glPoint);//����Ļ��GL���ת��������ڵ�ǰScrollView������
	bool clicked(CCSprite* sprite,CCPoint clickPos);//�����ж�clickPos�Ƿ���sprite���棨ע�⣬ֻ��ScrollView�ĺ���Sprite���ã�
	float getZeroY(){return zeroY;}//�õ������������½ǵ�Y����
	float getZeroX(){return zeroX;}//�õ������������½ǵ�X����

	/**
	 *	���ĳһҳ��Ӧ��Offset
	 */
	float getPageOffsetX(int pageIndex,float pageSizeX,float pageInval_X);
	
	/**
	 *	����ҳ��Ĺ�����true�����أ��Ͳ������
	 *	Ĭ�ϲ�����.
	 */
	virtual bool pageScrollIntercept(int pageIndex);
	/**
	 * ���ｲ����ScrollView��container���������ҳ��ҳ����ϣ�ҳ��֮����һ�����pageInval��
	 * ����ֻ��Ҫ��������Ľ����С��ҳ���ļ������ô�˺����ͻ���������ҳ��λ���ƶ�����Ļ�м�
	 * ��Ȼ��������м��ƶ�����ͼ�У������Ѿ������������ƶ�����ˣ���ô�ͻ�ֻ�ƶ����Ҳ����������ƶ���Χ����
	 * ������sizeX��
	 * 
	 * ע�������pageInval_X��ָ��ҳ֮��ľ��룬��ABC����ΪAB֮��BC֮��ľ��룬��A�����û�о���ģ�Ҫʵ��A��߻���C�ұ�Ҳ�о���
	 * ��ôҲ�ܼ򵥣�ֱ�Ӱ�ҳ�����ô������ˡ��������ڳ����ҳ�������Ű��ʱ�����¿ռ�����ˡ�
	 */
	void srollToPage(int pageIndex,float pageSizeX,float pageInval_X,float duration);			//������ĳҳ��,��0��ʼ��;
	//��������ÿһҳ��speed��ÿһ���������
	//��� speed<=0 ��ô���������������ҳ��
	void srollToPageBySpeed(int pageIndex,float pageSizeX,float pageInval_X,float speed);		//������ĳҳ��,��0��ʼ��;
	
	void scrollToChild(CCNode* child);


	/**
	 *	��������ĳ��ҳ�棬�ͻ���ã���һ���ص��ӿ�
	 *	�е����ҳ�����ͼ��Ҳ�ᷢ�͵ģ�Ȼ��Ϳ�ʼ�����ƶ���ȥ
	 *	����ֱ�ӵ�����ȥ
	 */
	virtual void pageSelected(int pageIndex){};
	/**
	 *	���������
	 */
	virtual void pageSelectedEnd(int pageIndex){};
public:
	~MyScrollView();
	MyScrollView(CCSize viewPortSize);
	//�����Ĵ���һ�㲻������д��
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);//�����Ĵ���һ�㲻������д��
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);//�����Ĵ���һ�㲻������д��
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);//�����Ĵ���һ�㲻������д��

	virtual void intentToScrollToRightPage(){}//��ͼ�����ұ�ҳ
	virtual void intentToScrollToLeftPage(){}//��ͼ�������ҳ
	virtual void intentToScrollToCurrentPage(){}

	
	//���������ֻ�ǵ�����¼����ͻ�����������,��̧����֮����õ�
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
	//���ʧ�ܣ�����յ��ʱ����������Լ�����ʶ��������˾ͻ���á�
	virtual void onTouchFalse(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    
    //����4��������ӦccTouch��4��������
	//����������ƶ��¼��ͻ�����������
	virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);

	//�����Ƿ���Թ�����true����Թ���
	void setEnableScroll(bool enbale);

private:
	CCSize winSize;
	CCSize scrollViewPortSize;
	float zeroX;
	float zeroY;
	CCTouch *m_pTouch;
	CCEvent *m_pEvent;
	CCPoint m_tBeginPos;
	bool moving;//�Ƿ���moving����
	bool canScroll;

	float  mInstantaneousVelocity;	// ˲ʱ�ٶȣ���̧����ָ֮ǰ��˲ʱ�ٶ�
	
	vector<CCPoint> movePointRecord;
	void addRecord(CCPoint record);
	float getInstantaneousVelocity();

	int mSelectedIndex;
	//
	void scrollActionEnd(CCNode* container);
};

#endif