#include "CreateHeroLayer.h"
#include "BaseActor.h"
#include "ImageManager.h"
#include "GameDirector.h"
#include "DefaultMainHeroLayer.h"
#include "ChooseHerosLayer.h"
#include "LFToast.h"
#include "LFMessageBox.h"
#include "PlayerNameFactory.h"
#include "MyselfManager.h"
#include "font.h"
#include "MainHeroManager.h"

static int actorType[] = {Profession_Yemanren,Profession_Chike,Profession_Fashi};
//static int actorType[] = {Profession_Yemanren};
static string menuItemPic = "zhiye_xuanze_";
static string jieShaoPic = "chuangjian_renwu_xinxi_";


#define TAG_Name_Edit_Box    1234

#define TAG_ITEM_Yemanren	11111
#define TAG_ITEM_Cike		11112
#define TAG_ITEM_FaShi		11113
#define TAG_ITEM_Cancel		11114
#define TAG_ITEM_OK			11115
#define TAG_ITEM_SUIJIM		11117

CreateHeroLayer::CreateHeroLayer(CreateHero_Dest dest /* = CreateHero_Dest_UseThisToMain */)
{
	//随机名字工厂
	PlayerNameFactory::getFactory(); // 在这里init， 在析构函数 delete

	// 分三段
	setHeadTopMost();
	setFootButtomMost();
	setBodyCenterMost();

	mDest = dest;

	CCSprite* bg = CCSprite::create("main_bg.png");
	addChild(bg,-1);
	CCLOG("width--->%f,hight---->%f",mWinSize.width,mWinSize.height);
	bg->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));
	currentSelected = 0;
    sex=true;

    headBiaoJi = BaseSprite::create("jiaose_icon_barbarian.png");
	headBiaoJi->setPosition(ccp(mWinSize.width/2,mWinSize.height- headBiaoJi->getContentSize().height/2-10));
	addChild(headBiaoJi);



	jueSheItems = CCArray::createWithCapacity(sizeof(actorType)/sizeof(int));
	jueSheItems->retain();

	/*
	BaseSprite* diZuo = BaseSprite::create("chuangjian_renwu_dizuo.png");
	addChildToBody(diZuo);
	diZuo->setPosition(ccp(mWinSize.width/2,250));
	*/

	
	// 英雄的滑动界面
	{
		// 构造时默认会移动到屏幕中心
		// 所有如果你不想他移动，就在构造完毕之后设置一次坐标即可
		heroGallery = new GridScrollView(
			CCSizeMake(640,960),
			sizeof(actorType)/sizeof(int),	// 元素的个数
			1,								// 每页横向显示几个
			1,								// 每页纵向显示几个
			10.0f,
			CCPointZero,
			20.0f);

		addChildToBody(heroGallery);
		heroGallery->release();
		heroGallery->setClickableCreater(this);
		// 可以显示了
		heroGallery->prepareToDisplay();
		heroGallery->setPosition(ccp(0,10));
	}

	

	BaseSprite* jieshaobg=BaseSprite::create("chuangjian_renwu_xinxi_bg.png");
	addChildToBody(jieshaobg);
	jieshaobg->setPosition(ccp(100,503));


	jieshaotype=BaseSprite::create("kaishi_jiaose_xinxi_barbarian.png");
	addChildToBody(jieshaotype);
	jieshaotype->setPosition(ccp(jieshaobg->getPositionX(),702));

	zhiyetext=LF::getString("ymrindroduce");
	zhiyejiesha=CCLabelTTF::create(zhiyetext.c_str(), fontStr_kaiti, 22, CCSize(130, 0), kCCTextAlignmentLeft);
	zhiyejiesha->setColor(fonColor_FaGuang);
	zhiyejiesha->setPosition(ccp(jieshaobg->getPositionX()+10,jieshaobg->getPositionY()+15));
	addChildToBody(zhiyejiesha);


	// 左边的按钮
	{
		BaseSprite* xuanZheZhiYe = BaseSprite::create("zhiye_xuanze_biaoti.png");
		addChildToBody(xuanZheZhiYe);
		xuanZheZhiYe->setPosition(ccp(541,702));

		int pos_x = 541;
		int pos_y = 622;
		CCMenuItemImage* yeManRen = CCMenuItemImage::create(
			"zhiye_xuanze_barbarian_select.png",		// 默认选中野蛮人
			"zhiye_xuanze_barbarian_anxia.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
		CCMenuItemImage* ciKe = CCMenuItemImage::create(
			"zhiye_xuanze_hunter_dengdai.png",
			"zhiye_xuanze_hunter_anxia.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
		CCMenuItemImage* faShi = CCMenuItemImage::create(
			"zhiye_xuanze_wizard_dengdai.png",
			"zhiye_xuanze_wizard_anxia.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));

		CCMenuItemImage* cancel = CCMenuItemImage::create(
			"chuangjian_quxiao.png",
			"chuangjian_quxiao_select.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
		if(MainHeroManager::getManager()->getAllMainHero().size()==0)
			cancel->setVisible(false);

		yeManRen->setPosition(ccp(pos_x,pos_y));
		yeManRen->setTag(TAG_ITEM_Yemanren); 
		ciKe->setPosition(ccp(pos_x,pos_y-100));
		ciKe->setTag(TAG_ITEM_Cike); 
		faShi->setPosition(ccp(pos_x,pos_y-198));



		faShi->setTag(TAG_ITEM_FaShi); 
		cancel->setPosition(ccp(pos_x,pos_y-298));
		cancel->setTag(TAG_ITEM_Cancel);

		jueSheItems->addObject(yeManRen);
		jueSheItems->addObject(ciKe);
		jueSheItems->addObject(faShi);
		CCMenu* menu = CCMenu::create(yeManRen,ciKe,faShi,cancel,NULL);
		addChildToBody(menu);
		menu->setPosition(CCPointZero);
	}
	

	// 名字输入框
	{


		s_bg = BaseSprite::create("chuangjian_youximing.png");
		addChildToFoot(s_bg);
		s_bg->setPosition(ccp(mWinSize.width/2,142));

		//输入提示
		string ts=LF::getString("srts");
		CCLabelTTF *LbmsLabel = CCLabelTTF::create(ts.c_str(), fontStr_kaiti, 20, CCSize(330, 0), kCCTextAlignmentLeft);
		LbmsLabel->setColor(ccc3(203,26,0));
		LbmsLabel->setPosition(ccp(s_bg->getPositionX()+50,s_bg->getPositionY()+s_bg->getContentSize().height/2+LbmsLabel->getContentSize().height/2));
		addChildToFoot(LbmsLabel);


		CCSize editSize = CCSizeMake(218,45);
		cutomNameBox = CCEditBox::create(editSize, CCScale9Sprite::create("green_edit.png"));
		addChildToFoot(cutomNameBox,0,TAG_Name_Edit_Box);
		
		cutomNameBox->setText(PlayerNameFactory::getFactory()->getRandomName(sex).c_str());
		cutomNameBox->setOpacity(0);
		cutomNameBox->setFontColor(fonColor_FaGuang);
		cutomNameBox->setFontSize(30);


		CCLabelTTF *temp = CCLabelTTF::create(cutomNameBox->getText(), fontStr_kaiti, 30);
		float moveJuli=(s_bg->getContentSize().width-60-temp->getContentSize().width)/2;
		float cutbox=s_bg->getPositionX();
		float cutboy=s_bg->getPositionY();
		cutbox=cutbox+moveJuli;
		cutomNameBox->setPosition(ccp(cutbox,cutboy));

		//cutomNameBox->setMaxLength(HeroNameMaxLen); // 不做这个限制，输入可以输入，最后不让玩家提交就是了
		cutomNameBox->setDelegate(this);
		

	//随机名字按钮
	

		suijimz=CCMenuItemImage::create(
			"main_touzi.png",
			"main_touzi_select.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
		suijimz->setTag(TAG_ITEM_SUIJIM);
		suijimz->setPosition(ccp(s_bg->getPositionX()+s_bg->getContentSize().width/2+suijimz->getContentSize().width/2,s_bg->getPositionY()));
		
	}


	// 确认按钮
	{
		ok = CCMenuItemImage::create(
			"chuangjian_queren.png",
			"chuangjian_queren_select.png",
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
		ok->setPosition(ccp(mWinSize.width/2,63));
		ok->setTag(TAG_ITEM_OK);
	}

	CCMenu* menu = CCMenu::create(ok,suijimz,NULL);
	addChildToFoot(menu);
	menu->setPosition(CCPointZero);
}

void CreateHeroLayer::itemClicked(CCObject* sender)
{
	CCMenuItem* item = (CCMenuItem*)sender;
	if (item->getTag() == TAG_ITEM_OK)
	{
		CCEditBox* box = (CCEditBox*)getChildByTagFromFoot(TAG_Name_Edit_Box);
		CCSize s = box->getContentSize();
		string name = box->getText();
		if (name.length() <= 0)
		{
			LFToast::makeToast(LF::getString("shurujuesemin"));
			return;
		}
		else if (name.length() > HeroNameMaxLen || name.length() <= 3)
		{
			LFToast::makeToast(
				CCString::createWithFormat(LF::getString("jueSheMingziChangDu").c_str(),3,HeroNameMaxLen)->getCString());
			return;
		}
		CCLOG("isok------>");
		createHeroWithTypeId(actorType[currentSelected],name);
	}
	else if (item->getTag() == TAG_ITEM_Yemanren)
	{
		heroGallery->scrollToPageSimple(0);
	}
	else if (item->getTag() == TAG_ITEM_Cike)
	{
		heroGallery->scrollToPageSimple(1);
	}
	else if (item->getTag() == TAG_ITEM_FaShi)
	{
		heroGallery->scrollToPageSimple(2);
	}
	else if (item->getTag() == TAG_ITEM_Cancel)
	{
		CCDirector::sharedDirector()->popScene();
	}else if(item->getTag() == TAG_ITEM_SUIJIM){


		cutomNameBox->setText(PlayerNameFactory::getFactory()->getRandomName(sex).c_str());
		CCLabelTTF *temp = CCLabelTTF::create(cutomNameBox->getText(), fontStr_kaiti, 30);
		float moveJuli=(s_bg->getContentSize().width-60-temp->getContentSize().width)/2;
		float cutbox=s_bg->getPositionX();
		float cutboy=s_bg->getPositionY();
		
	    cutbox=cutbox+moveJuli;
	
		cutomNameBox->setPosition(ccp(cutbox,cutboy));
		
	}
	
}
void CreateHeroLayer::createHeroWithTypeId(int typeId,string custName)
{
	addObserver(SEL_MenuHandler(&CreateHeroLayer::createHeroOver),MSG_createHeroRsp);
	mCmdHlp->cmdCreateHero(typeId,custName);
}
void CreateHeroLayer::createHeroOver(CCObject* msg_rsp)
{
	removeObserver(MSG_createHeroRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_CreateHeroRsp* rsp = (SPCmd_CreateHeroRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code == Success)
	{
		DefaultMainHeroLayer* layerDefault = NULL;
		ChooseHerosLayer* layerChoose = NULL;
		CCScene* scene = NULL;
		switch (mDest)
		{
		case CreateHero_Dest_None:

			LFMessageBox::alert("","Create OK");
			break;
		case CreateHero_Dest_DefaultMainHeroLayer:
			GameDirector::getDirector()->goToDefaultMainHeroLayer();
			break;
		case CreateHero_Dest_UseThisToMain:
			// 直接就用选择英雄的代码进去
			layerDefault = new DefaultMainHeroLayer(INVALID_ID);
			addChild(layerDefault,1);
			layerDefault->release();
			// 直接选这个英雄
			layerDefault->chooseHero(rsp->hero_id);
			break;
		case CreateHero_Dest_ChooseHerosLayer:

			/*
			CCLOG("choose------>page  %d",rsp->hero_id);
			layerChoose = new ChooseHerosLayer(rsp->hero_id);
			scene = CCScene::create();
			scene->addChild(layerChoose);
			layerChoose->release();
			CCDirector::sharedDirector()->replaceScene(scene);
			*/
		    GameDirector::getDirector()->goToDefaultMainHeroLayer(rsp->hero_id);

			break;
		default:
			CCAssert(false,"CreateHeroLayer::createHeroOver--->不支持的目的地");
			break;
		}
	}else
	{
		LFMessageBox::alert("","createHero Error!");
	}

}

ClickableItem* CreateHeroLayer::createGridItem(int pageIndex,int xIndex,int yIndex)
{
	ImageData data = ImageManager::getManager()->getImageData_JueSheQuanShen((Profession)actorType[pageIndex]);
	
	ClickableItemActor* item = new ClickableItemActor();
	item->initWithFile(data.getCurrentImageFile().c_str());
	item->setScale(data.scale);

	return item;
}


void CreateHeroLayer::pageSelectedEnd(int pageIndex)
{
	
	currentSelected = pageIndex;
	int num = sizeof(actorType)/sizeof(int);

	ok->setEnabled(true);

	//// 测试用，限制只能选择野蛮人
	//if (currentSelected != 0)
	//{
	//	LFToast::makeToast(LF::getString("buNengChuangJianGaiJueShe"));
	//	ok->setEnabled(false);
	//}

	// 禁用
	for (int i=0;i<num;i++)
	{

		//string fileName =menuItemPic + ImageManager::getManager()->getBaseImageName((Profession)actorType[i]) + ".png";
		string fileName="";
		string fileName1 ="";
		switch (i)
		{
			  case 0:
				  fileName="zhiye_xuanze_barbarian_dengdai.png";
				  fileName1 ="zhiye_xuanze_barbarian_anxia.png";
				break;
			  case 1:
				  fileName="zhiye_xuanze_hunter_dengdai.png";
				  fileName1 ="zhiye_xuanze_hunter_anxia.png";
				break;
			  case 2:
				  fileName="zhiye_xuanze_wizard_dengdai.png";
				  fileName1 ="zhiye_xuanze_wizard_anxia.png";
				break;
		}

		CCMenuItemImage* item = (CCMenuItemImage*)jueSheItems->objectAtIndex(i);
		item->initWithNormalImage( 
			(fileName).c_str(), 
			(fileName1).c_str(),
			(fileName1).c_str(),
			this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));
	}

	// 启用
	string fileNameEnable ="";// menuItemPic + ImageManager::getManager()->getBaseImageName((Profession)actorType[pageIndex]) + "_select.png";
	string fileNameEnable1 ="";
	switch (pageIndex)
		{
			  case 0:
				  fileNameEnable="zhiye_xuanze_barbarian_select.png";
				  fileNameEnable1="zhiye_xuanze_barbarian_anxia.png";
				break;
			  case 1:
				  fileNameEnable="zhiye_xuanze_hunter_select.png";
				  fileNameEnable1="zhiye_xuanze_hunter_anxia.png";
				break;
			  case 2:
				  fileNameEnable="zhiye_xuanze_wizard_select.png";
				  fileNameEnable1="zhiye_xuanze_wizard_anxia.png";
				break;
		}


	CCMenuItemImage* item = (CCMenuItemImage*)jueSheItems->objectAtIndex(pageIndex);
	item->initWithNormalImage( 
		(fileNameEnable).c_str(), 
		(fileNameEnable1).c_str(),
		(fileNameEnable1).c_str(),
		this,SEL_MenuHandler(&CreateHeroLayer::itemClicked));

	string jieShaoFileName = jieShaoPic + ImageManager::getManager()->getBaseImageName((Profession)actorType[pageIndex]) + ".png";
	CCLOG("------------->%s",jieShaoFileName.c_str());
	//jieShao->initWithFile(jieShaoFileName);

	{
	    string typepng="";
		string hendpepng="";
		switch (pageIndex)
		{
		   case 0:
			   sex=true;
			   typepng="kaishi_jiaose_xinxi_barbarian.png";
			   hendpepng="jiaose_icon_barbarian.png";
			   zhiyetext=LF::getString("ymrindroduce");
			 break;
		   case 1:
			   sex=true;
			   typepng="kaishi_jiaose_xinxi_hunter.png";
			    hendpepng="jiaose_icon_hunter.png";
				zhiyetext=LF::getString("ckindroduce");
			 break;
		   case 2:
			   typepng="kaishi_jiaose_xinxi_wizard.png";
			    hendpepng="jiaose_icon_wizard.png";
				zhiyetext=LF::getString("fsindroduce");
				sex=false;

			 break;
		}
		
	     jieshaotype->initWithFile(typepng);
		 headBiaoJi->initWithFile(hendpepng);
		 zhiyejiesha->setString(zhiyetext.c_str());
		 
	}


}

vector<string> CreateHeroLayer::parse(string sin){

	    int l = sin.length();
		vector<string> ret;
		ret.clear();
		for(int p = 0; p < l; ) {
		int size, n = l - p;
		unsigned char c = sin[p], cc = sin[p + 1];
		if(c < 0x80) {
		size = 1;
		} else if(c < 0xc2) {
		return ret;
		} else if(c < 0xe0) {
		if(n < 2) {
		return ret;
		}
		if(!((sin[p + 1] ^ 0x80) < 0x40)) {
		return ret;
		}
		size = 2;
		} else if(c < 0xf0) {
		if(n < 3) {
		return ret;
		}
		if(!((sin[p + 1] ^ 0x80) < 0x40 && 
		(sin[p + 2] ^ 0x80) < 0x40 &&
		(c >= 0xe1 || cc >= 0xa0))) {
		return ret;
		}
		size = 3;
		} else if(c < 0xf8) {
		if(n < 4) {
		return ret;
		}
		if(!((sin[p + 1] ^ 0x80) < 0x40 &&
		(sin[p + 2] ^ 0x80) < 0x40 &&
		(sin[p + 3] ^ 0x80) < 0x40 &&
		(c >= 0xf1 || cc >= 0x90))) {
		return ret;
		}
		size = 4;
		} else if (c < 0xfc) {
		if(n < 5) {
		return ret;
		}
		if(!((sin[p + 1] ^ 0x80) < 0x40 &&
		(sin[p + 2] ^ 0x80) < 0x40 &&
		(sin[p + 3] ^ 0x80) < 0x40 && 
		(sin[p + 4] ^ 0x80) < 0x40 &&
		(c >= 0xfd || cc >= 0x88))) {
		return ret;
		}
		size = 5;
		} else if (c < 0xfe) {
		if(n < 6) {
		return ret;
		}
		if(!((sin[p + 1] ^ 0x80) < 0x40 &&
		(sin[p + 2] ^ 0x80) < 0x40 &&
		(sin[p + 3] ^ 0x80) < 0x40 && 
		(sin[p + 4] ^ 0x80) < 0x40 &&
		(sin[p + 5] ^ 0x80) < 0x40 &&
		(c >= 0xfd || cc >= 0x84))) {
		return ret;
		}
		size = 6;
		} else {
		return ret;
		}
		string temp = "";
		temp = sin.substr(p, size);
		ret.push_back(temp);
		p += size;
		}
		return ret;
}

CCLabelTTF* CreateHeroLayer::getxyLength(string text){

	   CCLabelTTF *t;
	   CCLabelTTF *temp = CCLabelTTF::create(text.c_str(), fontStr_kaiti, 30);
	   if(temp->getContentSize().width>CCLabelTTF::create(LF::getString("length").c_str(), fontStr_kaiti, 30)->getContentSize().width){
		 
		   string msgtext="";
		   vector<string> strv;
		   strv=parse(text);
		   for(int i=0;i<strv.size()-1;i++){
			   msgtext+=strv[i];
		   }
		   t= getxyLength(msgtext);
	   }else{
		  
		   t=temp;

	   }
	return t;
}

void CreateHeroLayer::editBoxTextChanged(CCEditBox* editBox, const std::string& tex)
{
	string input_display = tex;
	CCLog("text-->[%s]",input_display.c_str());
	string trimed = LF::trim(input_display);
	CCLog("trimed-->[%s]",trimed.c_str());
    if (trimed != input_display) {
        editBox->setText(input_display.c_str());
    }

	string text=cutomNameBox->getText();
	CCLabelTTF *temp =getxyLength(text); //CCLabelTTF::create(cutomNameBox->getText(), fontStr_kaiti, 30);
	CCLOG("text--------->%s-------->%f",temp->getString(),temp->getContentSize().width);
	cutomNameBox->setText(temp->getString());
	float moveJuli=(s_bg->getContentSize().width-60-temp->getContentSize().width)/2;
	float cutbox=s_bg->getPositionX();
	float cutboy=s_bg->getPositionY();
	cutbox=cutbox+moveJuli;
	cutomNameBox->setPosition(ccp(cutbox,cutboy));
    
	// 输入过程中不做限制
	//input_display = trimed;
	//if (input_display.length() > HeroNameMaxLen)
	//{
	//	editBox->setText(input_display.substr(0,HeroNameMaxLen).c_str());
	//	CCLog("name too long! cut it!");
	//	LFToast::makeToast(
	//		CCString::createWithFormat(LF::getString("mingZiTaiChang").c_str(),HeroNameMaxLen)->getCString());
	//}
}
    
void CreateHeroLayer::editBoxReturn(CCEditBox* editBox)
{
}


CreateHeroLayer::~CreateHeroLayer(void)
{
	PlayerNameFactory::getFactory()->destroyFactory();

	jueSheItems->release();
}
