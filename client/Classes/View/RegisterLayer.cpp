#include "RegisterLayer.h"
#include "protocol.h"
#include "LoginLayer.h"
#include "CreateHeroLayer.h"
#include "GameDirector.h"
#include "GameInforEditer.h"
#include "LFToast.h"

#define  TAG_Edite_Name  12332
#define  TAG_Edite_PSW1  12333
#define  TAG_Edite_PSW2  12334

RegisterLayer::RegisterLayer(void)
{
	BaseSprite* bg = BaseSprite::create("main_bg.png");
	addChild(bg,-1);
	bg->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	BaseSprite* biaoZhi = BaseSprite::create("main_biaozhi.png");
	addChild(biaoZhi,0);
	biaoZhi->setPosition(ccp(mWinSize.width/2,mWinSize.height * 0.8f));

	//
	editGroup = BaseSprite::create("main_zhuce_kuang.png");
	addChild(editGroup);
	editGroup->setPosition(ccp(mWinSize.width/2,460));


	CCSize editSize = CCSizeMake(320,47);
	float editPosX = 315;

	CCScale9Sprite* editItemBg = CCScale9Sprite::create("green_edit.png");
	editItemBg->setOpacity(0);

	CCEditBox* cutomNameBox = CCEditBox::create(editSize,editItemBg);
	editGroup->addChild(cutomNameBox,0,TAG_Edite_Name);
	cutomNameBox->setPosition(ccp(editPosX,139));
	cutomNameBox->setMaxLength(ClientAccountMaxLen);
	cutomNameBox->setDelegate(this);

	editItemBg = CCScale9Sprite::create("green_edit.png");
	editItemBg->setOpacity(0);
	CCEditBox* cutomPSW1 = CCEditBox::create(editSize,editItemBg);
	editGroup->addChild(cutomPSW1,0,TAG_Edite_PSW1);
	cutomPSW1->setPosition(ccp(editPosX,89));
	cutomPSW1->setMaxLength(ClientPswMaxLen);
	cutomPSW1->setInputFlag(kEditBoxInputFlagPassword);
	cutomPSW1->setDelegate(this);

	editItemBg = CCScale9Sprite::create("green_edit.png");
	editItemBg->setOpacity(0);
	CCEditBox* cutomPSW2 = CCEditBox::create(editSize,editItemBg);
	editGroup->addChild(cutomPSW2,0,TAG_Edite_PSW2);
	cutomPSW2->setPosition(ccp(editPosX,34));
	cutomPSW2->setMaxLength(cutomPSW1->getMaxLength());
	cutomPSW2->setInputFlag(kEditBoxInputFlagPassword);
	cutomPSW2->setDelegate(this);


	//264
	CCMenuItemImage* itemReg = CCMenuItemImage::create(
		"main_zhuce.png",
		"main_zhuce_select.png",
		this,SEL_MenuHandler(&RegisterLayer::registClicked)
		);

	CCMenuItemImage* itemRet = CCMenuItemImage::create(
		"main_zhuce_fanhui.png",
		"main_zhuce_fanhui_select.png",
		this,SEL_MenuHandler(&RegisterLayer::returnClicked)
		);

	CCMenu* menu = CCMenu::create(itemReg,itemRet,NULL);
	addChild(menu);
	menu->setPosition(CCPointZero);
	itemReg->setPosition(ccp(mWinSize.width/2,263));
	itemRet->setPosition(ccp(mWinSize.width/2,190));
}

void RegisterLayer::registClicked(CCObject* sender)
{
	CCEditBox* nameBox = (CCEditBox*)editGroup->getChildByTag(TAG_Edite_Name);
	CCEditBox* psw1Box = (CCEditBox*)editGroup->getChildByTag(TAG_Edite_PSW1);
	CCEditBox* psw2Box = (CCEditBox*)editGroup->getChildByTag(TAG_Edite_PSW2);

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
	addObserver(SEL_CallFuncO(&RegisterLayer::registCallback),MSG_clientRegistRsp);
	mCmdHlp->cmdRegist(name.c_str(),psw1.c_str());
}

void RegisterLayer::returnClicked(CCObject* sender)
{
	LoginLayer* layer = new LoginLayer();
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);
}

void RegisterLayer::registCallback(CCObject* msg_rsp)
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


	{
		CCEditBox* nameBox = (CCEditBox*)editGroup->getChildByTag(TAG_Edite_Name);
		CCEditBox* psw1Box = (CCEditBox*)editGroup->getChildByTag(TAG_Edite_PSW1);
		GameInforEditer::getGameInfor()->setLastUsedNameAndPsw_save(nameBox->getText(),psw1Box->getText());
	}

	CreateHeroLayer* layer = new CreateHeroLayer(CreateHero_Dest_UseThisToMain);
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->pushScene(scene);
}


void RegisterLayer::editBoxTextChanged(CCEditBox* editBox, const std::string& tex)
{
	string input_display = tex;
	CCLog("text-->[%s]",input_display.c_str());
	string trimed = LF::trim(input_display);
	CCLog("trimed-->[%s]",trimed.c_str());

	input_display = trimed;
	editBox->setText(input_display.c_str());

	if (editBox->getTag() == TAG_Edite_Name)
	{
		if (input_display.length() > ClientAccountMaxLen)
		{
			editBox->setText(input_display.substr(0,ClientAccountMaxLen+1).c_str());
			CCLog("name too long! cut it!"); 
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("mingZiTaiChang").c_str(),ClientAccountMaxLen)->getCString());
		}
	}
	else if (editBox->getTag() == TAG_Edite_PSW2 || editBox->getTag() == TAG_Edite_PSW1)
	{
		if (input_display.length() > ClientPswMaxLen)
		{
			editBox->setText(input_display.substr(0,ClientPswMaxLen+1).c_str());
			CCLog("password too long! cut it!");//
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("miMaTaiChang").c_str(),ClientPswMaxLen)->getCString());
		}
	}
}

void RegisterLayer::editBoxReturn(CCEditBox* editBox)
{
}

RegisterLayer::~RegisterLayer(void)
{
}
