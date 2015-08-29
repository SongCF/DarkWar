#ifndef __NetGameSetUI_H__
#define __NetGameSetUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "BaseLayer.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class NetGameSetUI : public BaseLayer
{
public:
	NetGameSetUI();
	virtual ~NetGameSetUI();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(NetGameSetUI);
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
private:
	void changePaiHangBang(CCObject *obj);
	inline void closeThisLayer(CCObject *obj)
	{
		CCMenuItem *item = (CCMenuItem *)obj;
		//m_sBackSpr->runAction(CCSequence::create(CCSpawn::create(CCRotateBy::create(2.0f,1800.0f),CCScaleTo::create(1.8f,0.2f),NULL),CCCallFunc::create(this,SEL_CallFunc(&NetGameSetUI::closeCallBack)),NULL));
		m_sBackSpr->runAction(CCSequence::create(CCFadeOut::create(0),CCCallFunc::create(this,SEL_CallFunc(&NetGameSetUI::closeCallBack)),NULL));
	}
	inline void closeCallBack()
	{
		this->removeFromParent();
	}
	void heroClick(CCObject *obj);
	void vipClick(CCObject *obj);
	void shizuClick(CCObject *obj);
	void shuruClick(CCObject *obj);

	void backgroundSound(CCObject *obj);
	void effctiveSound(CCObject *obj);

	void lockScreen(CCObject *obj);
	void serverMessage(CCObject *obj);

	inline void callMenuEnable(CCNode *node)
	{
		CCMenuItem *item = (CCMenuItem *)node;
		item->setEnabled(true);
		setMenuEnable(item);
	}
	inline void setMenuFalseEnable(CCMenuItem *item)
	{
		map<CCMenuItem *,int>::iterator iter = m_lMenuItemList.find(item);
		if (iter != m_lMenuItemList.end())
		{
			iter->first->setEnabled(false);
			m_lMenuItemList.erase(iter->first);
		}
	}
	inline void setMenuEnable(CCMenuItem *item)
	{
		item->setEnabled(true);
		m_lMenuItemList.insert(map<CCMenuItem *,int>::value_type(item,0));
	}

	inline void changeMenuPriority_callfuncND(CCNode* node, void *priority)
	{
		CCMenu *menu = dynamic_cast<CCMenu*>(node);
		if (menu)
		{
			menu->setHandlerPriority((int)priority);
		}
	}

	void changeServer(CCObject *obj);

	void clickChangeServerOK(CCObject *obj);
	void clickChangeServerCancel(CCObject *obj);

	inline void closeLayer(CCNode *node)
	{
		//CCNode *node = (CCNode *)obj;
		node->removeFromParent();
		//m_lTanChuLayer.removeFromParent();
	}
private:
private:
	CCLayer						*m_lTanChuLayer;
	CCMenu						*m_mMenu;

	map<CCMenuItem *,int>		m_lMenuItemList;

	CCSprite					*m_sBackSpr;

	vector<CCSprite *>			m_bBackgroundSoundOpenSpr;
	vector<CCSprite *>			m_bEffctiveSoundOpenSpr;
	vector<CCSprite *>			m_bLockScreenOpenSpr;
	vector<CCSprite *>			m_bServerMessageOpenSpr;

	vector<CCMenuItemImage *>	m_iBackgroundItemList;
	vector<CCMenuItemImage *>	m_iEffctiveSoundItemList;
	vector<CCMenuItemImage *>	m_iLockScreenItemList;
	vector<CCMenuItemImage *>	m_iServerMessageItemList;

	CCLabelTTF					*m_tYingXiongBangLabelTTF;
};

#endif // __NetGameSetUI_H__