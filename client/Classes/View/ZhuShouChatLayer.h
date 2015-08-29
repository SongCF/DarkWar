
#pragma  once

#include "cocos2d.h"
#include <string>
#include <algorithm>
#include <vector>
using std::vector;
using std::string;
using std::pair;
USING_NS_CC;

const int TAG_ZhuShouChatLayer = 4644+60;
struct ShowMsg
{
	string msg;
	ccColor3B color;
};

// interface
class ZhuShouChatLayerCallBack
{
public:
	virtual void ZhuShouchatEndCallBack() = 0;
};


class ZhuShouChatLayer : public CCLayerColor
{
public:
	static ZhuShouChatLayer* create(vector<ShowMsg> &msg,int zhushou_id, ZhuShouChatLayerCallBack* callBackInterface);
	ZhuShouChatLayer(vector<ShowMsg> &msg,int zhushou_id, ZhuShouChatLayerCallBack* callBackInterface);
	virtual ~ZhuShouChatLayer();

	bool init();

	pair<string, string> getFileByName(string name);  // name , filename

	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);


	void showChatContext(float delta);
	void labelFadeInEnd();
	void saidChatContextEnd(float dt);


	void callbackRemove();


protected:
	ZhuShouChatLayerCallBack* m_callbackClass;
	int						  m_nZhuaShouId;
	vector<ShowMsg> 		  m_sChatMsg;


	int m_index;			//标记是第几个对话

	bool mTimeEnd;
	bool mLabelFadeInEnd;

	bool mLayerEnd;   //对话结束，置为true，该layer渐渐消失，


	unsigned int mCurMusicId; // touch后 stop声音时用
private:

	void nodeAndAllChildRunAction(CCNode* node, CCAction* action,CCAction *actionOver);
private:
	string getPinyinName(int id);
	int    getZhuShouId(int index);

	int					m_nTextWidth;

	int					m_nChildCount;
	
};