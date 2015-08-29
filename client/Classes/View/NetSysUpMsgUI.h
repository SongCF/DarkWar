#ifndef __NetSysUpMsgUI_H__
#define __NetSysUpMsgUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "NetSysUpMsgList.h"
#include "flat.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;
struct UpdataMsgInfo
{
	bool isImportant;//是否重要
	string info;//内容
};
struct UpdataMsg
{
	string updataMsgTitle;//更新的标题
	vector<UpdataMsgInfo> infoList;//内容列表
};
class NetSysUpMsgUI : public CCLayer
{
public:


	NetSysUpMsgUI();
	virtual ~NetSysUpMsgUI();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(NetSysUpMsgUI);
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void setUpdataMsg(const vector<string> str,string title);
	void setActivityMsg(vector<Flat_SystempNotifyInfoRsp::strACTIVITY> activityVec);

	inline void setCallBackHandler(CCObject *obj,SEL_CallFuncN handler)
	{
		m_oTarger = obj;
		m_hCallBackHandler = handler;
	}
private:
	inline void closeThisLayer(CCObject *obj)
	{
		CCMenuItem *item = (CCMenuItem *)obj;
		m_sBackSpr->runAction(CCSequence::create(CCSpawn::create(CCRotateBy::create(2.0f,1800.0f),CCScaleTo::create(1.8f,0.2f),NULL),CCCallFunc::create(this,SEL_CallFunc(&NetSysUpMsgUI::closeCallBack)),NULL));
		if (m_oTarger)
		{
			CCNode *node = CCNode::create();
			node->setTag(0);
			(m_oTarger->*m_hCallBackHandler)(node);
		}
	}
	inline void closeCallBack()
	{
		this->removeFromParent();
	}
	inline void closeCallBack(CCNode *node)
	{
		node->removeFromParent();
	}
	void startGameClick(CCObject *obj);

	inline void changeMenuPriority_callfuncND(CCNode* node, void *priority)
	{
		CCMenu *menu = dynamic_cast<CCMenu*>(node);
		if (menu)
		{
			menu->setHandlerPriority((int)priority);
		}
	}

	string getMsgStr(string msg,ccColor3B color);

	inline void textUpdata(float dt)
	{

		CCLog("interval : %.3f\n",dt);

		cc_timeval tl_begin , tl_end;
		CCTime::gettimeofdayCocos2d(&tl_end,NULL);
		CCLog("end : %ld  -  %ld\n",tl_end.tv_sec,tl_end.tv_usec);

		if (m_nUpdataCount >= 3)
		{
			unschedule(SEL_SCHEDULE(&NetSysUpMsgUI::textUpdata));
			return;
		}
		m_nUpdataCount++;
	}
private:
private:

	CCMenu						*m_mMenu;
	CCSprite					*m_sBackSpr;
	//CCSprite					*m_sBackSpr;

	UpdataMsg					m_mUpdataMsg;
	vector<HuoDongMsg>			m_mHuoDongMsg;

	unsigned int				m_nUpdataCount;

	CCObject					*m_oTarger;
	SEL_CallFuncN				m_hCallBackHandler;
};

#endif // __NetGameSetUI_H__