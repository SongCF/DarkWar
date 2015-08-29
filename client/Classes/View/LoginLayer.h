#pragma once

#include "BaseLayer.h"

/**
 *	账号输入和登陆的界面
 */
class LoginLayer : public BaseLayer
{
private:

	CCNode* loginBoard;
	CCNode* registerBoard;
	CCNode* functionPanle;

	//bool logined;
	//bool registered;

	CCLabelTTF* logindName;

	CCLayer* boardLayer;

	void registClicked(CCObject* sender);
	void showLoginItemClicked(CCObject* sender);

	void restart(CCObject* sender);

public:
	LoginLayer(void);

	// 登陆成功，或者注册成功就调用
	// 用于关闭board，更新右上角的用户名
	void setLogined(string userName);
	void setRegistered(string userName);

	// 进入游戏
	void goIn();

	void setLoginBoardVisible(bool visible);
	void setRegisterVisible(bool visible);

	void boardScrollToLogin(bool animation = true);
	void boardScrollToRegister(bool animation = true);
	void scrollingEnd(cocos2d::CCNode *board);

	~LoginLayer(void);
};

