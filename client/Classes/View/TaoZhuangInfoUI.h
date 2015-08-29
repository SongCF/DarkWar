#ifndef __TaoZhuangInfoUI_H__
#define __TaoZhuangInfoUI_H__

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
class TaoZhuangInfoUI : public CCLayer
{
public:
	TaoZhuangInfoUI();
	virtual ~TaoZhuangInfoUI();


	virtual bool init();
	CREATE_FUNC(TaoZhuangInfoUI);
	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

	inline void setProfessionId(Profession id)
	{
		m_sProfessionid = id;
	}
private:
	Profession getZhuShouId(int index);
	BaseActor *getZhuShouActor(Profession id);
	void clickTaoZhuang(CCObject *obj,TouchEventType type);
	void clickOk(CCObject *obj,TouchEventType type);

	string getNpcName(Profession id);

	string getNpcInfo(Profession id);
	
	string getNpcMiaoShu(Profession id);

	unsigned int getZhuangbeiGroupID(Profession id);
private:
	Profession				m_sProfessionid;
	vector<Button*>			m_lButtonList;

	//Button					*m_iCloseItem;
};

#endif // __MaskLayerUI_H__