#pragma once
#include "LFLayer.h"
#include <string>
#include "LFUtile.h"
#include "LFToast.h"

using namespace std;
using namespace cocos2d;

#define MSG_ALERT_CLOSE  "MSG_ALERT_CLOSE"

class LFMessageBox : public LFLayer
{
private:
	LFMessageBox(string title,string msg);
	~LFMessageBox(void);
	// 按钮点击
	void itemClicked(CCObject* sender);
public:
	// 弹框
	static void alert(string title,string msg);
};

