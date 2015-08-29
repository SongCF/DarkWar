#pragma once

#include "SegmentedLayer.h"
#include "protocol.h"
#include "LinkedNetworkRequest.h"
#include "BaseActorPropertyParser.h"
#include "ListViewLayer.h"
/**
 * 分段的layer，把一个layer分成了3段
 * 注意和MainLayerBase的区别，MainLayerBase是把一个界面分成了3个layer
 * 头部和尾部都是固话好的 分别置顶和置底。而且开发期间是直接给了3段的素材的。
 *
 * 这里的这个layer和MainLayerBase基本一样，但是没有自动排版，做规定等。
 * 只是简单的把东西分为3类：头，中，尾。我们可以分别控制头中尾的个性偏移
 *
 */
class DefaultMainHeroLayer : public SegmentedLayer,IRequest
{
private:
	
	CCMenu* menu;
	LinkedRequest* mRequest;

	// 元素点击
	void itemClicked(CCObject* sender);

	bool getActorsOver(CCObject* msg_rsp);
	bool getEquipsOver(CCObject* msg_rsp);
	// 物品
	bool getItemsOver(CCObject* msg_rsp);
	bool getSysGiftOver(CCObject* f_msg_rsp);
	// 套装
	bool getSuitOver(CCObject* msg_rsp);
	// 技能
	bool getSkillOver(CCObject* msg_rsp);
	// 魔法精华
	bool getMoFajinghuaOver(CCObject* msg_rsp);

	bool getShenBingPoolOver(CCObject* f_msg_rsp);
	bool getShenBingSuiPianOver(CCObject* msg_rsp);
	bool getItemSuiPianOver(CCObject* msg_rsp);
	bool getArenaTeanBasicInfoOver(CCObject* msg_rsp); //STEP_GetJiXianShouChengState
	//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	bool queryDailyLoginBonusOver(CCObject* f_msg_rsp);
// 	bool queryLevelBonusOver(CCObject* f_msg_rsp);
// 	bool querySystemBonusOver(CCObject* f_msg_rsp);
	bool queryBonusOver(CCObject* f_msg_rsp);

	bool queryVipOver(CCObject* msg_rsp);
	bool queryChargetParketOver(CCObject* f_msg_rsp);
	bool getTaskListOver(CCObject* msg_rsp);
	bool queryGiftActivityOver(CCObject* f_msg_rsp);
	bool queryKilledMonsterBonusOver(CCObject* f_msg_rsp);

	bool queryOver(CCObject* msg_rsp,string log);




	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 获取总共需要请求的个数
	virtual int getRequestNum();
	// 开始发第index个请求
	virtual void request(int index);
	// 获取当前需要监听的消息MSG_****
	virtual string getMessgaeToListening(int index);
	// 收到消息
	// msg_data : 就是MSG_Data或者是flat的，这个随具体的命令
	// index	: 当前消息是属于第几个的
	// 返回true就代表正常，可以继续，false就代表错误，终端
	virtual bool receivedData(CCObject* msg_data,int index);
	// 超时了
	virtual void timeout();
	// 全部完毕，结束了
	virtual void end();
private:
	void systemVersionCallBack(CCObject *obj);
	void UpMsgUICallBack(CCNode *node);
public:
	DefaultMainHeroLayer(unsigned int defaultId);
	// 选择哪个
	void selectHeroById(unsigned int heroId);
	// 选择英雄，获取英雄下面的武器
	void chooseHero(unsigned int heroId);
	unsigned int mSelectedHeroId;
	unsigned int mDefautHeroId;
	string gamename;
	string ganmelevel;
	CCLabelTTF* namelabel;
	CCLabelTTF* levelabel;
	BaseSprite* jueSheName;
	BaseSprite* jueShe;
	unsigned int currentHeroId;
	
	~DefaultMainHeroLayer(void);
};


class HeroListDefaltView : public ListViewLayer,ListViewCreator
{
private:
	

	int selectedIndex;
	vector<HeroInfo> mDataVec;

	// 创建第i个元素
	virtual BaseSprite* getItemContentByIndex(unsigned int index);
	// 刷新第i个元素，也可以简单的用getItemContentByIndex，再创建一个就是了
	virtual void refreshItem(CCNode* item,int index);

	virtual void listItemTouched(CCNode* listItem,int index);

public:
	ListViewDataSource data;
	DefaultMainHeroLayer* mLayer;
	HeroListDefaltView(DefaultMainHeroLayer* layer,unsigned int selectedHeroId);
	

	CCTableView* getTableView(){return mTableView;}
};

