#ifndef __ClickableItem_H__
#define __ClickableItem_H__

#include "cocos2d.h"
#include "IBorder.h"
using namespace cocos2d;
//һ������ʰȡ�����Ԫ�صĳ�����
class ClickableItem  : public CCSprite,IBorder
{
public:
	static const int ENABLE = 0x0001;//ĩλ��1����ô��Ч��ĩλ��0����ô��Ч
	static const int CLICKABLE = 0x0010;//�����ڶ�λ��1��Ч��0��Ч

	ClickableItem()
	{
		//��ʼ��Ϊ��Ч���ɵ��;
		myState = ENABLE | CLICKABLE;
		initScale = -1;
	}

	//��������λ�����ҵ�λ�����棬��Ӧ�ã��ᣩ�ص��������;
	//������δ����Ϳ��û���Ҫ���û����ݵ�ǰ��״̬ȥ����Item����ʾ��������;
	virtual void clickedMe()
	{
        leaveMe();
	}
    //���������ң���ôĬ���ǷŴ�
    virtual void touchMe()
    {
        anchorPointBk = getAnchorPoint();
        isIgnoreAnchorBk = isIgnoreAnchorPointForPosition();
		initScale = getScale();
        setScale(1.2f*initScale);
    }
    //��ָ�뿪���ң�Ĭ���Ǳ��ԭ��С
    virtual void leaveMe()
    {
		if (initScale != -1)
		{
			setScale(1.0f*initScale);
		}
    }
    
	//������Լ��Ŀ���ʰȡ�û�����ķ�Χ,Ĭ�Ͼͷ����ҵı߿�
	virtual std::list<CCRect> getBounds()
	{
		std::list<CCRect> rects;
		CCRect r = boundingBox();
		rects.insert(rects.end(),r);
		return rects; 
	}

	void setEnable(bool enbale)
	{
		if (enbale)
		{
			myState = myState | ENABLE;
		}else
		{
			myState = myState & ~ENABLE;
		}
	}
	void setClickable(bool clickable)
	{
		if (clickable)
		{
			myState = myState | CLICKABLE;
		}else
		{
			myState = myState & ~CLICKABLE;
		}
	}

	//��Ԫ���Ƿ�Ϊ��Ч����Ч�Ļ����Ƕ��ڻ�ɫ��ͼƬ
	//Ԫ����Ч�Ļ���ô�϶����ɵ��;��Ч�Ļ����������Ƿ���Ե����
	//�о���4��״̬��û��������Ч��û��������Ч����������Ч����������Ч
	bool isEnable()
	{
		int temp = myState & ~ENABLE;
		if ( temp == ENABLE)
		{
			return true;
		}else
		{
			return false;
		}
	}

	//Ԫ���Ƿ���Ե����һ�����Ƿ��м��������ݣ�Ҳ����Ԫ����Ч������һ�����Ե��;
	//Ԫ�ؿ��Ե����һ������Ч��;
	bool isClickable()
	{
		int temp = myState & ~CLICKABLE;
		if ( temp == CLICKABLE )
		{
			return true;
		}else
		{
			return false;
		}
	}
	//�õ�״̬:
	//
	int getState()
	{
		return myState;
	}
private:
	int myState;
	float initScale;
protected:
    CCPoint anchorPointBk;
    bool isIgnoreAnchorBk;
};




#endif