#pragma once

#include "LFAlert.h"
#include "cocos2d.h"
#include <string>

using std::string;

USING_NS_CC;


enum enumUnblockZone
{
	//主页上的按钮
	unblock_GongJiang,
	unblock_JiuGuan,
	unblock_JiaoTang,
	//
	unblock_YingXiongBang,
	unblock_ZaHuoPu,
	unblock_TuJian,

	//活动、副本、竞技
	unblock_DiYuFuBen,
	unblock_ChuanQiLu,
	unblock_JingJiChang,
	unblock_TuanDuiFuBen,
	unblock_XueSeJiangTang,
	unblock_JiXianShouCheng,
	unblock_DianFengDuiJue,
	unblock_DiaoLuoChaXun,

	//工匠系统
	unblock_GongJiang_QiangHuaHuiTui,
	unblock_GongJiang_FenJie,
	unblock_GongJiang_BaoShiXiangQian,
	unblock_GongJiang_BaoShiHeCheng,
	unblock_GongJiang_DuanZao,
	unblock_GongJiang_JingLianZhuanKe,
	unblock_GongJiang_CuiQuFuMo,
};
enum enumUnblockType
{
	unblockType_Switch,     // 直接开关控制 on off
	unblockType_Level = 1,  // 受等级控制
	unblockType_TaskProcess,// 受副本进度控制
	unblockType_Time,       // 受时间控制
};
struct UnblockInfo
{
	enumUnblockType type;
	enumUnblockZone zone;
	unsigned int value1;
	unsigned int value2;
	string description;

	UnblockInfo(){
		value1 = 0;  //
		value2 = 0;  //辅助使用，只有type为时间才会用到。
	}
};
class UnblockManager
{
public:
	static UnblockManager *getManager();
	UnblockInfo getBlockInfo(enumUnblockZone zone);
	bool isDeblocking(enumUnblockZone cond);//是否已经解锁（解锁条件 <= 当前）

	//升级 或者 通关触发教学 才可调用
	bool canOpen(enumUnblockZone cond);//是否现在刚解锁（解锁条件 == 当前）

private:
	UnblockManager(){}
	static UnblockManager* s_mManager;
};


class UnblockWarnDialog : public CCLayerColor
{

	enum _iTags
	{
		Tag_Label = 1,
	};

public:
	static UnblockWarnDialog* create(string infoStr, bool nowOpen=false, bool canNotGo=false,bool isGuide = false);
	UnblockWarnDialog(string infoStr, bool bOpen, bool canNotGo,bool isGuide);
	void setLabelDisplaySize(CCSize size);

public:
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();

	void updataTime(float dt);
	void displayEnd_RemoveThisDialog();
	void callback_removeFromParent();

	inline void setCallBack(CCObject *obj,SEL_CallFunc handler)
	{
		m_oTarger = obj;
		m_hRemoveOverCallBack = handler;
	}

protected:
	void menuItemClicked(CCObject *pSender);
	void menuItemClicked_Go(CCObject *pSender);
	
protected:
	float mHasDisplayTime;
	bool mClicked;
	bool mOpen;
	bool mCanNotGo;

	CCObject			*m_oTarger;
	SEL_CallFunc		m_hRemoveOverCallBack;

	int					m_nTagGoTo;

	bool				m_bIsGuide;
};