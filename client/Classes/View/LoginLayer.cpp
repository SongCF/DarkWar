#include "LoginLayer.h"
#include "GameInforEditer.h"
#include "GameDirector.h"
#include "CreateHeroLayer.h"
#include "NetworkWaitingUI.h"
#include "RegisterLayer.h"
#include "DefaultMainHeroLayer.h"
#include "MainHeroManager.h"
#include "LFToast.h"
#include "LFMessageBox.h"
#include "font.h"
#include "GameStoryLayer.h"

enum LoginLayerState
{
	LG_State_Fresh		= 0,	// 刚到这个页面，没有点击登陆，也没注册，一切都没操作
	LG_Regiter_Success	= 1,	// 发送注册，成功
	LG_Login_Success	= 5,	// 发送Login成功
};
// 默认是LG_State_Fresh，每次点击board的登陆或者注册，都会清空状态到默认状态
static LoginLayerState s_State = LG_State_Fresh;

class LoginBoard : public BaseSprite,CCEditBoxDelegate
{
	friend class LoginLayer;
public:
	LoginBoard(LoginLayer* par_layer)
	{
		initWithFile("main_mianban_denglu.png");
		this->par_layer = par_layer;
		{
			lable = CCLabelTTF::create(
				"",
				fontStr_kaiti,
				30,
				CCSize(getContentSize().width - 40,0), 
				kCCTextAlignmentCenter);

			addChild(lable);
			lable->setPosition(ccp(getContentSize().width/2,317));
		}

		{
			CCSize editSize = CCSizeMake(247,30);
			CCScale9Sprite* editItemBg = CCScale9Sprite::create("green_edit.png");
			editItemBg->setOpacity(0);

			cutomNameBox = CCEditBox::create(editSize,editItemBg);
			addChild(cutomNameBox);
			cutomNameBox->setPosition(ccp(276,246));
			cutomNameBox->setDelegate(this);
			//cutomNameBox->setMaxLength(ClientAccountMaxLen); // 输入不限制
			cutomNameBox->setFontColor(ccBLACK);

			editItemBg = CCScale9Sprite::create("green_edit.png");
			editItemBg->setOpacity(0);

			miMaBox = CCEditBox::create(editSize,editItemBg);
			addChild(miMaBox);
			miMaBox->setPosition(ccp(cutomNameBox->getPositionX(),175));
			miMaBox->setInputFlag(kEditBoxInputFlagPassword);
			miMaBox->setDelegate(this);
			miMaBox->setFontColor(ccBLACK);
			//miMaBox->setMaxLength(ClientPswMaxLen); // 输入不限制

			reloadLastUsedUserInfor();
		}
		itemDengLu = CCMenuItemImage::create(
			"main_anjian_denglu.png",
			"main_anjian_denglu_select.png",
			this,
			SEL_CallFuncO(&LoginBoard::loginClicked)
			);

		itemZhuCe = CCMenuItemImage::create(
			"main_anjian_zhuce.png",
			"main_anjian_zhuce_select.png",
			this,
			SEL_CallFuncO(&LoginBoard::goToRegisterClicked)
			);

		itemClose = CCMenuItemImage::create(
			"main_anjian_tuichu.png",
			"main_anjian_tuichu_select.png",
			this,
			SEL_CallFuncO(&LoginBoard::closeClicked)
			);

		itemClose->setPosition(ccp(432,384));

		itemDengLu->setPosition(ccp(96,75));
		itemZhuCe->setPosition(ccp(getContentSize().width - itemDengLu->getPositionX(),itemDengLu->getPositionY()));

		menu = CCMenu::create(itemDengLu,itemZhuCe,itemClose,NULL);
		addChild(menu);
		menu->setPosition(CCPointZero);
	}

	// 重新载入数据
	void reloadLastUsedUserInfor()
	{
		cutomNameBox->setText(GameInforEditer::getGameInfor()->getLastUsername().c_str());
		miMaBox->setText(GameInforEditer::getGameInfor()->getLastPasseord().c_str());
	}

	void setString(string msg)
	{
		lable->setString(msg.c_str());
	}

	~LoginBoard()
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(
			this,MSG_clientLoginRsp);
	}
private:
	CCLabelTTF* lable;
	CCEditBox* cutomNameBox;
	CCEditBox* miMaBox;
	CCMenu* menu;
	CCMenuItemImage* itemDengLu;
	CCMenuItemImage* itemZhuCe;
	CCMenuItemImage* itemClose;
	LoginLayer* par_layer;

	void loginClicked(CCObject* sender)
	{
		// 清空状态
		s_State = LG_State_Fresh;

		string name = cutomNameBox->getText();
		string mima = miMaBox->getText();

		if (name.length() < 3 || name.length() > ClientAccountMaxLen)
		{
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("yonghumingchangdu").c_str(),3,ClientAccountMaxLen)->getCString());
			return;
		}

		if (mima.length() < 3 || mima.length() > ClientPswMaxLen)
		{
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("mimachangdu").c_str(),3,ClientPswMaxLen)->getCString());
			return;
		}

		// 监听登陆消息
		CCNotificationCenter::sharedNotificationCenter()->addObserver(
			this,SEL_CallFuncO(&LoginBoard::loginRsp),MSG_clientLoginRsp,NULL);

		int currentId = GameInforEditer::getGameInfor()->getClientId();

		CmdHelper::getHelper()->cmdLogin(name.c_str(),mima.c_str());
	}

	void goToRegisterClicked(CCObject* sender)
	{
		par_layer->boardScrollToRegister();
	}

	void closeClicked(CCObject* sender)
	{
		par_layer->setLoginBoardVisible(false);
	}

	void loginRsp(CCObject* msg_rsp)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(
			this,MSG_clientLoginRsp);
		GameDirector::getDirector()->hideWaiting();

		// 登陆的返回
		SPCmd_ClientLoginRsp* rsp = (SPCmd_ClientLoginRsp*)((MSG_Rsp*)msg_rsp)->getData();

		if (rsp->err_code == AccountOrPswErr)
		{
			GameDirector::getDirector()->hideWaiting();
			LFToast::makeToast(LF::getString("yonghumingHuoMiMaCuoWu"));
			return;
		}

		if (rsp->err_code != Success)
		{
			GameDirector::getDirector()->hideWaiting();
			LFMessageBox::alert("","Login Error!");
			return;
		}

		s_State = LG_Login_Success;

		string name = cutomNameBox->getText();
		string mima = miMaBox->getText();

		// 保存
		{
			// 更新数据
			GameInforEditer* infor = GameInforEditer::getGameInfor();
			if (infor->getLastUsername() != name || infor->getLastPasseord() != mima)
			{
				GameInforEditer::getGameInfor()->setLastUsedNameAndPsw_save(name,mima);
			}
		}

		par_layer->setLogined(GameInforEditer::getGameInfor()->getLastUsername());
	}

	/**
     * This method is called when the edit box text was changed.
     * @param editBox The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text)
	{
		// 输入不限制
		//if (editBox == miMaBox)
		//{
		//	if (text.length() > ClientPswMaxLen)
		//	{
		//		editBox->setText(text.substr(0,ClientPswMaxLen).c_str());
		//		CCLog("name too long! cut it!"); 
		//		LFToast::makeToast(
		//			CCString::createWithFormat(LF::getString("miMaTaiChang").c_str(),ClientPswMaxLen)->getCString());
		//	}
		//}
		//else if (editBox == cutomNameBox)
		//{
		//	if (text.length() > ClientAccountMaxLen)
		//	{
		//		editBox->setText(text.substr(0,ClientAccountMaxLen).c_str());
		//		CCLog("password too long! cut it!");//
		//		LFToast::makeToast(
		//			CCString::createWithFormat(LF::getString("mingZiTaiChang").c_str(),ClientAccountMaxLen)->getCString());
		//	}
		//}
	}
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxReturn(CCEditBox* editBox)
	{


	}
};

#define  TAG_Edite_Name  12332
#define  TAG_Edite_PSW1  12333
#define  TAG_Edite_PSW2  12334
class RegisterBoard : public BaseSprite,CCEditBoxDelegate
{
	friend class LoginLayer;
public:
	RegisterBoard(LoginLayer* par_layer)
	{
		initWithFile("main_mianban_zhuce.png");
		this->par_layer = par_layer;

		lable = CCLabelTTF::create(
			"",
			fontStr_kaiti,
			30,
			CCSize(getContentSize().width - 40,0), 
			kCCTextAlignmentCenter);

		addChild(lable);
		lable->setPosition(ccp(getContentSize().width/2,333));


        reCreatEditBox();

		CCMenuItemImage* itemZhuce = CCMenuItemImage::create(
			"main_anjian_queren.png",
			"main_anjian_queren_select.png",
			this,
			SEL_CallFuncO(&RegisterBoard::registerClicked)
			);

		CCMenuItemImage* itemQuxiao = CCMenuItemImage::create(
			"main_anjian_fanhui.png",
			"main_anjian_fanhui_select.png",
			this,
			SEL_CallFuncO(&RegisterBoard::cancleClicked)
			);

		CCMenuItemImage* itemClose = CCMenuItemImage::create(
			"main_anjian_tuichu.png",
			"main_anjian_tuichu_select.png",
			this,
			SEL_CallFuncO(&RegisterBoard::closeClicked)
			);

		itemClose->setPosition(ccp(433,384));
		itemZhuce->setPosition(ccp(96,75));
		itemQuxiao->setPosition(ccp(getContentSize().width - itemZhuce->getPositionX(),itemZhuce->getPositionY()));

		menu = CCMenu::create(itemZhuce,itemQuxiao,itemClose,NULL);
		addChild(menu);
		menu->setPosition(CCPointZero);
	}

	~RegisterBoard()
	{
		removeObserver(MSG_clientRegistRsp);
	}

	void clearPanle()
	{
		CCEditBox* nameBox = (CCEditBox*)this->getChildByTag(TAG_Edite_Name);
		CCEditBox* psw1Box = (CCEditBox*)this->getChildByTag(TAG_Edite_PSW1);
		CCEditBox* psw2Box = (CCEditBox*)this->getChildByTag(TAG_Edite_PSW2);

		nameBox->setText("");
		psw1Box->setText("");
		psw2Box->setText("");
	}
    
    void reCreatEditBox()
    {
        // remove first
        removeChildByTag(TAG_Edite_Name);
        removeChildByTag(TAG_Edite_PSW1);
        removeChildByTag(TAG_Edite_PSW2);
        
        // create again
        {
			CCSize editSize = CCSizeMake(247,30);
			float editPosX = 283;
            
			CCScale9Sprite* editItemBg = CCScale9Sprite::create("green_edit.png");
			editItemBg->setOpacity(0);
            
			CCEditBox* cutomNameBox = CCEditBox::create(editSize,editItemBg);
			this->addChild(cutomNameBox,0,TAG_Edite_Name);
			cutomNameBox->setPosition(ccp(editPosX,280));
			cutomNameBox->setDelegate(this);
			cutomNameBox->setFontColor(ccBLACK);
            
            
			editItemBg = CCScale9Sprite::create("green_edit.png");
			editItemBg->setOpacity(0);
			CCEditBox* cutomPSW1 = CCEditBox::create(editSize,editItemBg);
			this->addChild(cutomPSW1,0,TAG_Edite_PSW1);
			cutomPSW1->setPosition(ccp(editPosX,211));
			cutomPSW1->setInputFlag(kEditBoxInputFlagPassword);
			cutomPSW1->setDelegate(this);
			cutomPSW1->setFontColor(ccBLACK);
            
            
			editItemBg = CCScale9Sprite::create("green_edit.png");
			editItemBg->setOpacity(0);
			CCEditBox* cutomPSW2 = CCEditBox::create(editSize,editItemBg);
			this->addChild(cutomPSW2,0,TAG_Edite_PSW2);
			cutomPSW2->setPosition(ccp(editPosX,142));
			cutomPSW2->setInputFlag(kEditBoxInputFlagPassword);
			cutomPSW2->setDelegate(this);
			cutomPSW2->setFontColor(ccBLACK);
            
		}
    }

	void setString(string msg)
	{
		lable->setString(msg.c_str());
	}

private:

	CCLabelTTF* lable;
	LoginLayer* par_layer;
	CCMenu* menu;

	void addObserver(SEL_CallFuncO selector,const char *name)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
	}
	// 移除消息监听
	void removeObserver(const char *name)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
	}

	void registerClicked(CCObject* sender)
	{
		// 清空状态
		s_State = LG_State_Fresh;

		CCEditBox* nameBox = (CCEditBox*)this->getChildByTag(TAG_Edite_Name);
		CCEditBox* psw1Box = (CCEditBox*)this->getChildByTag(TAG_Edite_PSW1);
		CCEditBox* psw2Box = (CCEditBox*)this->getChildByTag(TAG_Edite_PSW2);

		string name = nameBox->getText();
		string psw1 = psw1Box->getText();
		string psw2 = psw2Box->getText();

		if (name.length() < 3 || name.length() > ClientAccountMaxLen)
		{
			CCLog("name.length = %d",name.length());
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("yonghumingchangdu").c_str(),
				3,ClientAccountMaxLen)->getCString());
			return;
		}

		if (!LF::is_pws_valide(psw1) || !LF::is_pws_valide(psw2))
		{
			LFToast::makeToast(LF::getString("mimageshibudui"));
			return;
		}

		if (psw1.length() < 3 || psw1.length() > ClientPswMaxLen)
		{
			CCLog("name.length = %d",psw1.length());
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("mimachangdu").c_str(),3,ClientPswMaxLen)->getCString());
			return;
		}

		if (psw1 != psw2)
		{
			LFToast::makeToast(LF::getString("mimabutong"));
			return;
		}
		addObserver(SEL_CallFuncO(&RegisterBoard::registCallback),MSG_clientRegistRsp);
		CmdHelper::getHelper()->cmdRegist(name.c_str(),psw1.c_str());
	}

	void registCallback(CCObject* msg_rsp)
	{
		removeObserver(MSG_clientRegistRsp);

		GameDirector::getDirector()->hideWaiting();

		SPCmd_RegisterRsp* rsp = (SPCmd_RegisterRsp*)((MSG_Rsp*)msg_rsp)->getData();

		if (rsp->err_code == AccountAlreadyExist)
		{
			LFToast::makeToast(LF::getString("AccountAlreadyExist"));
			return;
		}
		if (rsp->err_code != Success)
		{
			LFToast::makeToast(LF::getString("zhuceShiBai"));
			return;
		}

		// 切换状态
		s_State = LG_Regiter_Success;

		// 保存
		{
			CCEditBox* nameBox = (CCEditBox*)this->getChildByTag(TAG_Edite_Name);
			CCEditBox* psw1Box = (CCEditBox*)this->getChildByTag(TAG_Edite_PSW1);
			GameInforEditer::getGameInfor()->setLastUsedNameAndPsw_save(nameBox->getText(),psw1Box->getText());
		}

		par_layer->setRegistered(GameInforEditer::getGameInfor()->getLastUsername());
	}


	void cancleClicked(CCObject* sender)
	{
		par_layer->boardScrollToLogin();
	}

	void closeClicked(CCObject* sender)
	{
		par_layer->setRegisterVisible(false);
	}

	void editBoxTextChanged(CCEditBox* editBox, const std::string& tex)
	{
		string input_display = tex;
		CCLog("text-->[%s]",input_display.c_str());
		string trimed = LF::trim(input_display);
		CCLog("trimed-->[%s]",trimed.c_str());

	 	if (trimed != input_display) {
            editBox->setText(input_display.c_str());
        }
	}

	void editBoxReturn(CCEditBox* editBox)
	{
	}
};

// 包括登陆按钮和点击选择区域的按钮
class FunctionPanle_Normal : public BaseSprite
{
public:
	FunctionPanle_Normal(LoginLayer* layer)
	{
		par_layer = layer;

		initWithFile("denglu_gongneng_normal.png");
		curServerItem = CCMenuItemImage::create(
			"main_fuwuqi.png",
			"main_fuwuqi.png",
			this,
			SEL_MenuHandler(&FunctionPanle_Normal::changeServerClicked)
			);

		{
			lable = CCLabelTTF::create(
				"",
				fontStr_kaiti,
				30,
				CCSize(getContentSize().width - 40,0), 
				kCCTextAlignmentCenter);
			lable->setColor(ccc3(217,131,55));

			curServerItem->addChild(lable);
			lable->setPosition(ccp(curServerItem->getContentSize().width/2,curServerItem->getContentSize().height/2));
		}

		loginItem = CCMenuItemImage::create(
			"main_jinru.png",
			"main_jinru.png",
			"main_jinru_no.png",
			this,
			SEL_MenuHandler(&FunctionPanle_Normal::loginClicked)
			);

		loginFlashMask = BaseSprite::create("main_jinru_ziti.png");
		loginItem->addChild(loginFlashMask);

		loginFlashMask->runAction(CCRepeat::create( CCSequence::create(
			CCFadeTo::create(0.4f,80),
			CCFadeTo::create(0.35f,255),
			NULL
			),-1));
		loginFlashMask->setPosition(ccp(loginItem->getContentSize().width/2,loginItem->getContentSize().height/2));

		CCMenu* meue = CCMenu::create(curServerItem,loginItem,NULL);
		addChild(meue);
		meue->setPosition(CCPointZero);

		curServerItem->setPosition(ccp(getContentSize().width/2,getContentSize().height - curServerItem->getContentSize().height/2));
		loginItem->setPosition(ccp(getContentSize().width/2,loginItem->getContentSize().height/2));
	}

	void setServer(string name)
	{
		lable->setString(name.c_str());
	}
	void setEnable(bool enable)
	{
		loginItem->setEnabled(enable);
		curServerItem->setEnabled(enable);
		loginFlashMask->setVisible(enable);

		if (enable)
		{
			lable->setColor(ccc3(217,131,55));
		}else
		{
			lable->setColor(ccGRAY);
		}
		

	}
private:
	CCLabelTTF* lable;
	CCMenuItemImage* loginItem;
	CCMenuItemImage* curServerItem;
	BaseSprite* loginFlashMask;
	LoginLayer* par_layer;

	void changeServerClicked(CCObject* obj)
	{

	}

	void loginClicked(CCObject* obj)
	{
		// 没有登陆，也没有注册
		// 那么就用保存的数据先登陆
		if (s_State != LG_Login_Success && s_State != LG_Regiter_Success)
		{
			// 监听登陆消息
			CCNotificationCenter::sharedNotificationCenter()->addObserver(
				this,SEL_CallFuncO(&FunctionPanle_Normal::loginRsp),MSG_clientLoginRsp,NULL);

			int currentId = GameInforEditer::getGameInfor()->getClientId();

			string name = GameInforEditer::getGameInfor()->getLastUsername();
			string pass = GameInforEditer::getGameInfor()->getLastPasseord();

			// 如果保存的数据不全，那么就显示登陆窗口
			if (name.length() <= 0 || pass.length() <=0)
			{
				par_layer->setLoginBoardVisible(true);
				par_layer->boardScrollToLogin(false);
				return;
			}
			
			CmdHelper::getHelper()->cmdLogin(name.c_str(),pass.c_str());
		}
		else
		{
			par_layer->goIn();
		}
	}

	void loginRsp(CCObject* msg_rsp)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(
			this,MSG_clientLoginRsp);
		GameDirector::getDirector()->hideWaiting();

		// 登陆的返回
		SPCmd_ClientLoginRsp* rsp = (SPCmd_ClientLoginRsp*)((MSG_Rsp*)msg_rsp)->getData();

		if (rsp->err_code == AccountOrPswErr)
		{
			GameDirector::getDirector()->hideWaiting();
			LFToast::makeToast(LF::getString("yonghumingHuoMiMaCuoWu"));
			return;
		}

		if (rsp->err_code != Success)
		{
			GameDirector::getDirector()->hideWaiting();
			LFMessageBox::alert("","Login Error!");
			return;
		}
		s_State = LG_Login_Success;
		par_layer->goIn();
	}
};
class FunctionPanle_ChooseServer : public BaseSprite
{

};

LoginLayer::LoginLayer(void)
{
	s_State = LG_State_Fresh;
	//
	BaseSprite* bg = BaseSprite::create("main_beijin.png");
	addChild(bg,-100);
	bg->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	BaseSprite* biaoZhi = BaseSprite::create("main_biaoti.png");
	addChild(biaoZhi,0);
	biaoZhi->setPosition(ccp(mWinSize.width/2,mWinSize.height*0.63f));

	loginBoard = new LoginBoard(this);
	registerBoard = new RegisterBoard(this);
	boardLayer = CCLayer::create();
	addChild(boardLayer);
	boardLayer->addChild(loginBoard);
	boardLayer->addChild(registerBoard);
	loginBoard->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height/2));
	registerBoard->setPosition(ccp(loginBoard->getPositionX() + mVisibleSize.width,loginBoard->getPositionY()));
	loginBoard->release();
	registerBoard->release();

	functionPanle = new FunctionPanle_Normal(this);
	addChild(functionPanle);
	functionPanle->setPosition(ccp(mVisibleSize.width/2,functionPanle->getContentSize().height/2 + 20));
	functionPanle->release();

	((FunctionPanle_Normal*)functionPanle)->setEnable(false);
	((FunctionPanle_Normal*)functionPanle)->setServer("alpha test server");

	//// 放到左右边，就看不到board了
	//boardLayer->setPosition(ccp(mVisibleSize.width,0));
	//// 滑到看得到登陆框的页面
	//boardLayer->runAction(CCMoveTo::create(0.3f,ccp(0,0)));

	((LoginBoard*)loginBoard)->setOpacity(0);
	((LoginBoard*)loginBoard)->itemZhuCe->setOpacity(0);
	((LoginBoard*)loginBoard)->itemDengLu->setOpacity(0);
	((LoginBoard*)loginBoard)->itemClose->setOpacity(0);
	CCAction* loginBaordAction = CCSequence::create(
		CCDelayTime::create(0.5f),
		CCFadeTo::create(0.2f,255),
		NULL
		);
	loginBoard->runAction((CCAction*)loginBaordAction->copy()->autorelease());
	((LoginBoard*)loginBoard)->itemZhuCe->runAction((CCAction*)loginBaordAction->copy()->autorelease());
	((LoginBoard*)loginBoard)->itemDengLu->runAction((CCAction*)loginBaordAction->copy()->autorelease());
	((LoginBoard*)loginBoard)->itemClose->runAction((CCAction*)loginBaordAction->copy()->autorelease());

	// 人物
	BaseSprite* renwu = BaseSprite::create("main_renwu.png");
	addChild(renwu,-1);
	renwu->setPosition(ccp(mVisibleSize.width/2.0f,-renwu->getContentSize().height/2));
	renwu->runAction(CCSequence::create(
		CCDelayTime::create(0.1f),
		CCMoveTo::create(0.3f,ccp(mVisibleSize.width/2.0f,renwu->getContentSize().height/2)),
		NULL
		));


	// 雨点
	BaseSprite* yu = BaseSprite::create("main_qianjin.png");
	addChild(yu,-1);
	yu->setPosition(ccp(mVisibleSize.width/2 + mVisibleSize.width,mVisibleSize.height/2));
	yu->runAction(CCSequence::create(
		CCDelayTime::create(0.2f),
		CCMoveTo::create(0.3f,ccp(mVisibleSize.width/2,mVisibleSize.height/2)),
		NULL
		));


	//partical
	//
	{
		CCParticleSystemQuad* par = CCParticleSystemQuad::create("partical_BiaoZhi.plist");
		par->setAutoRemoveOnFinish(true);
		par->setScale(1.0f);
		addChild(par,-1);
		par->setPosition(ccp(biaoZhi->getPositionX()+26, biaoZhi->getPositionY() + 100));
	}

	CCMenuItemImage* itemShowLoginBoard = CCMenuItemImage::create(
		"main_zhanghao.png",
		"main_zhanghao.png",
		this,SEL_MenuHandler(&LoginLayer::showLoginItemClicked)
		);
	CCMenuItemImage* restart = CCMenuItemImage::create(
		"restart.png",
		"restart_select.png",
		this,SEL_MenuHandler(&LoginLayer::restart)
		);

	CCMenu* menu = CCMenu::create(itemShowLoginBoard,restart,NULL);
	menu->setPosition(CCPointZero);
	itemShowLoginBoard->setPosition(ccp(
		mVisibleSize.width - itemShowLoginBoard->getContentSize().width/2 - 10,
		mVisibleSize.height - itemShowLoginBoard->getContentSize().height/2 - 10));
	restart->setPosition(ccp(
		mVisibleSize.width - restart->getContentSize().width/2 - 10,
		restart->getContentSize().height/2 + 10));
	addChild(menu);

	//登陆过的名字
	logindName = CCLabelTTF::create(
		"",
		fontStr_kaiti,
		25,
		CCSize(240 - 40,0), 
		kCCTextAlignmentLeft);
	logindName->setAnchorPoint(ccp(0,0.5f));
	itemShowLoginBoard->addChild(logindName);
	logindName->setColor(ccBLACK);
	logindName->setPosition(ccp(70,20));

	// 上次有登陆
	if (GameInforEditer::getGameInfor()->getLastUsername() != "" && GameInforEditer::getGameInfor()->getLastPasseord() != "")
	{
		logindName->setString(GameInforEditer::getGameInfor()->getLastUsername().c_str());
		setLoginBoardVisible(false);
	}
}

//
void LoginLayer::registClicked(CCObject* sender)
{
	RegisterLayer* layer = new RegisterLayer();
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);
}

void LoginLayer::showLoginItemClicked(CCObject* sender)
{
	setLoginBoardVisible(true);
	setRegisterVisible(true);

	boardScrollToLogin(false);
}


void LoginLayer::setLogined(string userName)
{
	setLoginBoardVisible(false);
	logindName->setString(userName.c_str());
}
void LoginLayer::setRegistered(string userName)
{
	setRegisterVisible(false);
	logindName->setString(userName.c_str());
}

void LoginLayer::goIn()
{
	if (s_State == LG_Login_Success || s_State == LG_Regiter_Success)
	{
		// 有英雄，就到选择英雄界面
		if (MainHeroManager::getManager()->getAllMainHero().size() > 0)
		{
			GameDirector::getDirector()->goToDefaultMainHeroLayer();
			CCLog("LoginLayer::loginCallBack----> ChooseHerosLayer");
		}
		// 没有英雄，就到创建英雄界面
		else
		{
			GameStoryLayer *layer = GameStoryLayer::create();
			CCScene* scene = CCScene::create();
			scene->addChild(layer);
			CCDirector::sharedDirector()->replaceScene(scene);
		}
	}
	else
	{
		// 上次有登陆,那么自动登陆
		if (GameInforEditer::getGameInfor()->getLastUsername() != "" && GameInforEditer::getGameInfor()->getLastPasseord() != "")
		{
			((LoginBoard*)loginBoard)->loginClicked(NULL);
			return;
		}
		else
		{
			showLoginItemClicked(NULL);
		}
	}
}

void LoginLayer::setLoginBoardVisible(bool visible)
{
	loginBoard->setVisible(visible);
	((LoginBoard*)loginBoard)->menu->setVisible(visible);
	((FunctionPanle_Normal*)functionPanle)->setEnable(!visible);

	if (visible)
	{
		// 刷新登陆框解界面
		((LoginBoard*)loginBoard)->reloadLastUsedUserInfor();
	}
}
void LoginLayer::setRegisterVisible(bool visible)
{
	registerBoard->setVisible(visible);
	((RegisterBoard*)registerBoard)->menu->setVisible(visible);
	((FunctionPanle_Normal*)functionPanle)->setEnable(!visible);


	if (visible)
	{
		// 刷新注册框解界面
		((RegisterBoard*)registerBoard)->clearPanle();
	}
}

void LoginLayer::boardScrollToLogin(bool animation/* = true*/)
{
	float pos_x_target = 0;
	boardLayer->stopAllActions();
	if(animation)
		boardLayer->runAction(CCMoveTo::create(0.3f,ccp(pos_x_target,0)));
	else
		boardLayer->setPosition(ccp(pos_x_target,0));
}
void LoginLayer::boardScrollToRegister(bool animation /*= true*/)
{
	float pos_x_target = -mVisibleSize.width;
	boardLayer->stopAllActions();
	if(animation)
		boardLayer->runAction(CCSequence::create(
                                                 CCMoveTo::create(0.3f,ccp(pos_x_target,0)),
                                                 CCCallFuncN::create(this, SEL_CallFuncN(&LoginLayer::scrollingEnd)),NULL));
	else
		boardLayer->setPosition(ccp(pos_x_target,0));
}

void LoginLayer::scrollingEnd(cocos2d::CCNode *board)
{
	// 因为iOS的bug，所以后创建的editView在到了屏幕可视范围内之后还得再重新创建一次
	// bug应该是这样的，如果创建的EditeBox初始位置不在屏幕内(可视范围内)，那么把他移动进来还是看不见到
	// 也许那个真正的view还是在外面
	// 所有需要重新创建一次
    if (registerBoard != NULL) {
        ((RegisterBoard*)registerBoard)->reCreatEditBox();
    }
}


//
void LoginLayer::restart(CCObject* sender)
{
	GameDirector::getDirector()->index();
}
//
LoginLayer::~LoginLayer(void)
{
}
