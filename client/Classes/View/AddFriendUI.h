#ifndef __AddFriendUI_H__
#define __AddFriendUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "LFStrings.h"
#include "font.h"
#include "ImageManager.h"
#include "MyselfManager.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace cocos2d::ui;
USING_NS_CC;
class AddFriendUI : public CCLayer
{
public:
	AddFriendUI();
	virtual ~AddFriendUI();


	virtual bool init();
	CREATE_FUNC(AddFriendUI);
	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

	inline void setFriendID(int index)
	{
		m_sFriendIndex = index;
		m_bIsGuide = true;
	}
	inline void setCallBack(CCObject *obj,SEL_CallFuncN handler)
	{
		m_oTarger = obj;
		m_nHandler = handler;
	}
	inline void setMenuShow(bool isCloseShow = false,bool isAddShow = false)
	{
		m_bCloseItem = isCloseShow;
		m_bAddTeamItem = isAddShow;
	}

	inline void setProfessionId(Profession id)
	{
		m_pProfessionId = id;
		m_bIsGuide = false;
	}
private:
	Profession getZhuShouId(int index);
	BaseActor *getZhuShouActor(Profession id);
	void clickTaoZhuang(CCObject *obj,TouchEventType type);
	void clickOk(CCObject *obj,TouchEventType type);

	string getNpcName(Profession id);

	string getNpcInfo(Profession id);
private:
	int			m_sFriendIndex;
	vector<Button*>			m_lButtonList;

	CCObject			*m_oTarger;
	SEL_CallFuncN		m_nHandler;

	bool				m_bCloseItem;
	bool				m_bAddTeamItem;
	bool				m_bIsGuide;

	Profession			m_pProfessionId;
};

#endif // __MaskLayerUI_H__