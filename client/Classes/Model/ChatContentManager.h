#pragma once

#include "cocos2d.h"
#include <vector>
#include <algorithm>
#include <string>
#include "include_json.h"

USING_NS_CC;
using namespace std;


typedef struct
{
	string name;
	string chatStr;
	string musicFile;
	float  deltaTime;
}ChatItem;
 
class ChatContentManager : public CCObject
{
public:
	~ChatContentManager(){}
	static ChatContentManager* sharedChatContentManager();

	//1. param1 id 任务id     
	//2. param2 idx 任务共分为5个对话部分 （1.接受任务 2.刚开始进入任务时 3.BOSS关卡开始战斗前 4.BOSS关卡通过时 5.完成任务）
	//3. return  pair(speaker, sentence)
	vector<ChatItem> getMainTaskChatContentById(int id, int idx);
	//教学关卡对话  guideTaskIdx = 1, 2.     section = 2, 4
	vector<ChatItem> getGuideTaskChatContent(int guideTaskIdx, int section);
	vector<ChatItem> getGuideChatContent(string status);

	string getMainTaskNameById(int id);

	//use to chat layer
	string getNpcFileNameByNpcName(string name);
private:
	string getPinyinName(int id);
protected:
	ChatContentManager();


	static ChatContentManager* s_manager;



	Json::Value   mJson_Root;
};