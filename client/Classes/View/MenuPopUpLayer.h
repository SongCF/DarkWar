#ifndef __MenuPopUpLayer_H__
#define __MenuPopUpLayer_H__

#include "cocos2d.h"
#include "cocos-ext.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;
class MenuPopUpLayer : public CCLayer
{
public:
	MenuPopUpLayer();
	virtual ~MenuPopUpLayer();


	virtual bool init();
	CREATE_FUNC(MenuPopUpLayer);
	virtual void onEnter();
	virtual void onExit();
	void UnLockFenJie();//开放分解的动画
	void UnLockJiuGuan();//开放分解的动画
	inline void setCallBack(CCObject *obj,SEL_CallFuncO callback)
	{
		m_oTarger = obj;
		m_hCallBackNode = callback;
	}
	inline CCMenuItem* getJiuGuanItem()
	{
		CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(1);
		return item;
	}

	inline void setTanchu(bool isCan)
	{
		m_bIsTanchu = isCan;
		if (m_mMenu)
		{
			if (m_bIsTanchu)
			{
				((CCMenuItem *)m_mMenu->getChildByTag(999))->setVisible(false);
				((CCMenuItem *)m_mMenu->getChildByTag(999))->setEnabled(false);
				((CCMenuItem *)m_mMenu->getChildByTag(0))->setVisible(true);
				((CCMenuItem *)m_mMenu->getChildByTag(0))->setEnabled(true);
				for (int i = 1 ; i < 6 ; i ++)
				{
					m_lMenuState[i] = 1;
				}

				for (int i = 0 ; i < 3 ; i ++)
				{
					CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
					item->setPositionY(item->getPositionY() + (3 - i) * 90);
				}

				for (int i = 0 ; i < 2 ; i ++)
				{
					CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
					item->setPositionX(item->getPositionX() + (i + 1) * 82);
				}
			}
			else
			{
			
				((CCMenuItem *)m_mMenu->getChildByTag(999))->setVisible(true);
				((CCMenuItem *)m_mMenu->getChildByTag(999))->setEnabled(true);
				((CCMenuItem *)m_mMenu->getChildByTag(0))->setVisible(false);
				((CCMenuItem *)m_mMenu->getChildByTag(0))->setEnabled(false);
				m_bMenuState = false;
				for (int i = 1 ; i < 6 ; i ++)
				{
					m_lMenuState[i] = 0;
				}

				for (int i = 0 ; i < 3 ; i ++)
				{
					CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 1);
					item->setVisible(false);
					item->setEnabled(false);
				}

				for (int i = 0 ; i < 2 ; i ++)
				{
					CCMenuItem *item = (CCMenuItem *)m_mMenu->getChildByTag(i + 4);
					item->setVisible(false);
					item->setEnabled(false);
				}
			}
		}
	}

	void setFenJiePlayCallBack(CCObject *obj,SEL_CallFunc handler)
	{
		m_oPlayTarger = obj;
		m_hPlayCallBackHandler = handler;
	}
private:
	void clickShouqi(CCObject *obj);
	void clickMore(CCObject *obj);
	void clickOther(CCObject *obj);

	void moveOverCallBack(CCNode *node);
	void shouqiMoveOverCallBack(CCNode *node);
	inline void playOver(CCNode *obj)
	{
		CCMenuItem *item = (CCMenuItem *)obj;
		item->setEnabled(true);

		if (m_oPlayTarger)
		{
			(m_oPlayTarger->*m_hPlayCallBackHandler)();
		}
	}
private:
	CCObject				*m_oPlayTarger;
	SEL_CallFunc			m_hPlayCallBackHandler;
	CCMenu					*m_mMenu;

	vector<CCMenuItem *>	m_lMenuList;

	int						m_lMenuState[6];

	bool					m_bMenuState;//false是收回的true是弹出的

	CCObject				*m_oTarger;
	SEL_CallFuncO			m_hCallBackNode;

	bool					m_bIsTanchu;
};

#endif // __MenuPopUpLayer_H__