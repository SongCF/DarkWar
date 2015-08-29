
#pragma  once

#include "cocos2d.h"
#include "ChatContentManager.h"
#include <string>
#include <algorithm>
#include <vector>
using std::vector;
using std::string;
using std::pair;
USING_NS_CC;

const int TAG_ChatLayer = 4643+60;

// interface
class ChatLayerCallBack
{
public:
	virtual void chatEndCallBack() = 0;
};


class ChatLayer : public CCLayerColor
{
public:
	static ChatLayer* create(vector<ChatItem> chatContext, ChatLayerCallBack* callBackInterface);
	ChatLayer(vector<ChatItem>& chatContext, ChatLayerCallBack* callBackInterface);
	~ChatLayer();

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
	ChatLayerCallBack* m_callbackClass;

	vector<ChatItem> m_chatContext;


	int m_index;			//标记是第几个对话

	bool mTimeEnd;
	bool mLabelFadeInEnd;

	bool mLayerEnd;   //对话结束，置为true，该layer渐渐消失，


	unsigned int mCurMusicId; // touch后 stop声音时用
};