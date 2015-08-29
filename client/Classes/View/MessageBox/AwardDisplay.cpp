#include "AwardDisplay.h"
#include "font.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "XueZhanManager.h"
#include "SystemGiftBagManager.h"

AwardDisplay* AwardDisplay::create(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc)
{

	AwardDisplay* layer = new AwardDisplay(info, items, equips, callbackfunc);
	if(layer != NULL)
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}

AwardDisplay* AwardDisplay::create(CCString info, vector<ItemInfo> items,GiftBag* bag, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc)
{

	AwardDisplay* layer = new AwardDisplay(info, items,bag, equips, callbackfunc);
	if(layer != NULL)
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}


AwardDisplay* AwardDisplay::create(Flat_GetSysTemGiftBagRsp *data){


	AwardDisplay* layer = new AwardDisplay(data);

	if(layer != NULL)
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;

}

AwardDisplay::AwardDisplay(CCString info, vector<ItemInfo> items,GiftBag* bag, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc)
{
	CCAssert(items.size() > 0 || equips.size() > 0, "gift data size less than 0");

	mCallback = callbackfunc;
	int allNum = items.size() + equips.size();

	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));


	//label 提示
	{
		CCLabelTTF* label = CCLabelTTF::create(info.getCString(), 
			fontStr_kaiti, 24, CCSize(435, 0), kCCTextAlignmentCenter);
		if (allNum > 0)
		{
			label->setAnchorPoint(ccp(0.5, 1));
			label->setPosition(ccp(dialog->getContentSize().width/2, 250));
		}
		else //不允许这种情况  已经asset
		{
			label->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		}
		/*label->setColor(ccc3(0xff, 0xd7, 0x00));*/
		dialog->addChild(label, 1);
	}

	//材料
	int label_size = 20;
	ccColor3B label_color = ccWHITE;



	if(bag!=NULL&&bag->count>0){
	//libao img
	BaseSprite *LbImg = BaseSprite::create(ImageManager::getManager()->getGiftBagFileName(bag->id));
	dialog->addChild(LbImg,2);
	LbImg->setPosition(ccp((dialog->getContentSize().width-(items.size()+1)*LbImg->getContentSize().width)/2+LbImg->getContentSize().width/2,dialog->getContentSize().height/2 ));

	//礼包背景
	BaseSprite *LbImgBg = BaseSprite::create(ImageManager::getManager()->getGiftBagBoardName(bag->id));
	dialog->addChild(LbImgBg,1);
	LbImgBg->setPosition(ccp(LbImg->getPositionX(),LbImg->getPositionY()));

	//礼包数量
	char buf[20];
	sprintf(buf, "\nx%u", bag->count);
	string temp = LF::getString("LBSL");
	temp += buf;
	CCLabelTTF *labelShuliangbx = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			,CCSize(0,0), kCCTextAlignmentCenter);
	LbImg->addChild(labelShuliangbx);
	labelShuliangbx->setPosition(ccp(LbImg->getContentSize().width/2, -labelShuliangbx->getContentSize().height/2));
	labelShuliangbx->setColor(label_color);

	}

	int posIdx = 0;
	for (int i=0; i < items.size(); ++i, ++posIdx)
	{
		BaseSprite* cailiaoImg = BaseSprite::create("daoju_kuang_bg.png");
		dialog->addChild(cailiaoImg);

		int pos_x =0;
		 if(bag!=NULL&&bag->count>0){
			 BaseSprite *LbImg = BaseSprite::create(ImageManager::getManager()->getGiftBagFileName(bag->id));
			 pos_x =(dialog->getContentSize().width-(items.size()+1)*LbImg->getContentSize().width)/2+LbImg->getContentSize().width+10+LbImg->getContentSize().width/2-10;
		 }
		 else{
			 pos_x =dialog->getContentSize().width/2;
		 }

		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (cailiaoImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (cailiaoImg->getContentSize().width + 10);
		}
		cailiaoImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
		//item img
		BaseSprite * itemImg = BaseSprite::create(
			ImageManager::getManager()->getItemFilename(items.at(i).type)); // .count .type
		cailiaoImg->addChild(itemImg);
		itemImg->setPosition(ccp(cailiaoImg->getContentSize().width/2, cailiaoImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%u", items.at(i).count);
		string temp = CS::getItemName(items.at(i).type);
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		cailiaoImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(cailiaoImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}	





	for (int i=0; i<equips.size(); ++i, ++posIdx)
	{
		BaseSprite* diwenImg = BaseSprite::create(
			ImageManager::getManager()->getImageData_DiWen_WithBianKuang(equips.at(i).zhuangbei_colour)); // .count .type
		dialog->addChild(diwenImg);

		int pos_x = dialog->getContentSize().width/2;
		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (diwenImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (diwenImg->getContentSize().width + 10);
		}
		diwenImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
		//equip pic
		string equipfile = equips.at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *s = BaseSprite::create(equipfile);
		diwenImg->addChild(s);
		s->setScale(0.23f);
		s->setPosition(ccp(diwenImg->getContentSize().width/2,diwenImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%d", 1);
		string temp = equips.at(i).zhuangbei_name;
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		diwenImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(diwenImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}

	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&AwardDisplay::menuItemOkClicked));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}

	//
	{
		this->setTag(TAG_AwardDisplayer_Layer);
		this->setZOrder(1025);
		this->setBaseContainer(dialog,menu);
	}
}



AwardDisplay::AwardDisplay(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc)
{
	CCAssert(items.size() > 0 || equips.size() > 0, "gift data size less than 0");

	mCallback = callbackfunc;
	int allNum = items.size() + equips.size();

	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));


	//label 提示
	{
		CCLabelTTF* label = CCLabelTTF::create(info.getCString(), 
			fontStr_kaiti, 24, CCSize(435, 0), kCCTextAlignmentCenter);
		if (allNum > 0)
		{
			label->setAnchorPoint(ccp(0.5, 1));
			label->setPosition(ccp(dialog->getContentSize().width/2, 250));
		}
		else //不允许这种情况  已经asset
		{
			label->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		}
		/*label->setColor(ccc3(0xff, 0xd7, 0x00));*/
		dialog->addChild(label, 1);
	}

	//材料
	int label_size = 20;
	ccColor3B label_color = ccWHITE;


	
	int posIdx = 0;
	for (int i=0; i < items.size(); ++i, ++posIdx)
	{
		BaseSprite* cailiaoImg = BaseSprite::create("daoju_kuang_bg.png");
		dialog->addChild(cailiaoImg);

		int pos_x = pos_x =dialog->getContentSize().width/2;
		 

		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (cailiaoImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (cailiaoImg->getContentSize().width + 10);
		}
		cailiaoImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
		//item img
		BaseSprite * itemImg = BaseSprite::create(
			ImageManager::getManager()->getItemFilename(items.at(i).type)); // .count .type
		cailiaoImg->addChild(itemImg);
		itemImg->setPosition(ccp(cailiaoImg->getContentSize().width/2, cailiaoImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%u", items.at(i).count);
		string temp = CS::getItemName(items.at(i).type);
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		cailiaoImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(cailiaoImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}	





	for (int i=0; i<equips.size(); ++i, ++posIdx)
	{
		BaseSprite* diwenImg = BaseSprite::create(
			ImageManager::getManager()->getImageData_DiWen_WithBianKuang(equips.at(i).zhuangbei_colour)); // .count .type
		dialog->addChild(diwenImg);

		int pos_x = dialog->getContentSize().width/2;
		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (diwenImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (diwenImg->getContentSize().width + 10);
		}
		diwenImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
		//equip pic
		string equipfile = equips.at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *s = BaseSprite::create(equipfile);
		diwenImg->addChild(s);
		s->setScale(0.23f);
		s->setPosition(ccp(diwenImg->getContentSize().width/2,diwenImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%d", 1);
		string temp = equips.at(i).zhuangbei_name;
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		diwenImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(diwenImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}

	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&AwardDisplay::menuItemOkClicked));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}

	//
	{
		this->setTag(TAG_AwardDisplayer_Layer);
		this->setZOrder(1025);
		this->setBaseContainer(dialog,menu);
	}
}


int AwardDisplay::getLioBaoNum(Flat_GetSysTemGiftBagRsp *data){

	int num=0;
	Flat_GetSysTemGiftBagRsp Dt=*data;

	if(Dt.vitality>0)
		num++;
	num+=Dt.equipVec.size()+Dt.giftBagVec.size()+Dt.itemDebrisVec.size()
		 +Dt.itemVec.size()+Dt.shenbingDebrisVec.size();

	return num;
}


AwardDisplay::AwardDisplay(Flat_GetSysTemGiftBagRsp *data){

	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));

	
	Flat_GetSysTemGiftBagRsp Dt=*data;
	int The_first_few=0;
	float width=127.0;

	int num=getLioBaoNum(data);

	if(num>3)
		num=3;
	float statx=(dialog->getContentSize().width-num*width)/2+width/2-10;

	if(Dt.vitality>0){

		BaseSprite *Tinengbg=BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));
		Tinengbg->setPosition(ccp(statx,dialog->getContentSize().height/2));
		dialog->addChild(Tinengbg);

		BaseSprite *Tineng=BaseSprite::create("daoju_tili.png");
		Tineng->setPosition(ccp(statx,dialog->getContentSize().height/2));
		dialog->addChild(Tineng);

		CCString temp;
		temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), Dt.vitality);
		CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);
		nameLabel->setPosition(ccp(statx,dialog->getContentSize().height/2-Tineng->getContentSize().height/2-30));
		dialog->addChild(nameLabel);


		The_first_few++;
	}


	if(Dt.itemVec.size()>0){
		 
		for(int i=0;i<Dt.itemVec.size();i++){
		  unsigned int item_type = Dt.itemVec[i].type;

		  BaseSprite *Itemspritebg =BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(item_type));
		  float stx=statx+The_first_few*width+10*The_first_few;
		  if(The_first_few==0)
			   stx=statx;
		  Itemspritebg->setPosition(ccp(stx,dialog->getContentSize().height/2));
		  dialog->addChild(Itemspritebg);

		  BaseSprite *Itemsprite =BaseSprite::create(ImageManager::getManager()->getItemFilename(item_type));
		  Itemsprite->setPosition(ccp(stx,dialog->getContentSize().height/2));
		  dialog->addChild(Itemsprite);

		  CCString temp;
		  temp.initWithFormat("%s x%u", CS::getItemName(Dt.itemVec[i].type).c_str(), Dt.itemVec[i].count);
		  CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);

		  nameLabel->setPosition(ccp(stx,dialog->getContentSize().height/2-Itemsprite->getContentSize().height/2-30));
		  dialog->addChild(nameLabel);


		  The_first_few++;
		  if(The_first_few>=3)
			  break;
		}

	}

	if(The_first_few<3&&Dt.itemDebrisVec.size()>0){

		for(int i=0;i<Dt.itemDebrisVec.size();i++){

			unsigned int item_type = Dt.itemDebrisVec[i].item_id;
			BaseSprite *Itemspritebg =BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(item_type));
			  float stx=statx+The_first_few*width+10*The_first_few;
			  if(The_first_few==0)
				   stx=statx;
			  Itemspritebg->setPosition(ccp(stx,dialog->getContentSize().height/2));
			  dialog->addChild(Itemspritebg);

			BaseSprite *Itemsprite = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(Dt.itemDebrisVec[i].item_id));
			Itemsprite->setPosition(ccp(stx,dialog->getContentSize().height/2));
		    dialog->addChild(Itemsprite);

			string nameTemp = CS::getItemName(Dt.itemDebrisVec[i].item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + CCString::createWithFormat("%d",Dt.itemDebrisVec[i].suipian_id)->getCString();
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), Dt.itemDebrisVec[i].suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);
			nameLabel->setPosition(ccp(stx,dialog->getContentSize().height/2-Itemsprite->getContentSize().height/2-30));
		    dialog->addChild(nameLabel);

			The_first_few++;
			if(The_first_few>=3)
				break;

		}

	}

	//装备
	if(The_first_few<3&&Dt.equipVec.size()>0){

		for(int i=0;i<Dt.equipVec.size();i++){

			 float stx=statx+The_first_few*width+10*The_first_few;
			 if(The_first_few==0)
				   stx=statx;
			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(Dt.equipVec[i].zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(AwardDisplay::menuItemClicked_Equip));
			eqDiwenImg->setScale(0.8f);
			eqDiwenImg->setPosition(ccp(stx,dialog->getContentSize().height/2));
			dialog->addChild(eqDiwenImg);

			string equipfile = Dt.equipVec[i].equipPic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(stx,dialog->getContentSize().height/2));
			dialog->addChild(equipImg);

			string temp=Dt.equipVec[i].equipName;
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);
			nameLabel->setPosition(ccp(stx,dialog->getContentSize().height/2-eqDiwenImg->getContentSize().height/2-30));
		    dialog->addChild(nameLabel);


			The_first_few++;
			if(The_first_few>=3)
				break;

		}


	}

	// ShenbingSuipianInfo
	if(The_first_few<3&&Dt.shenbingDebrisVec.size()>0){

		for(int i=0;i<Dt.shenbingDebrisVec.size();i++){

		    unsigned int item_type = Dt.shenbingDebrisVec[i].zb_group_id;
			BaseSprite *Itemspritebg =BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(item_type));
			  float stx=statx+The_first_few*width+10*The_first_few;
			  if(The_first_few==0)
				   stx=statx;
			  Itemspritebg->setPosition(ccp(stx,dialog->getContentSize().height/2));
			  dialog->addChild(Itemspritebg);

			BaseSprite *Itemsprite = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(item_type));
			Itemsprite->setPosition(ccp(stx,dialog->getContentSize().height/2));
		    dialog->addChild(Itemsprite);

			string nameTemp = CS::getItemName(item_type) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + CCString::createWithFormat("%d",Dt.shenbingDebrisVec[i].suipian_id)->getCString();
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), Dt.shenbingDebrisVec[i].suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);
			nameLabel->setPosition(ccp(stx,dialog->getContentSize().height/2-Itemsprite->getContentSize().height/2-30));
		    dialog->addChild(nameLabel);

			The_first_few++;
			if(The_first_few>=3)
				break;

		}


	}


	if(The_first_few<3&&Dt.giftBagVec.size()>0){


		for(int i=0;i<Dt.giftBagVec.size();i++){

			BaseSprite *Itemspritebg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(Dt.giftBagVec[i].id));
			float stx=statx+The_first_few*width+10*The_first_few;
			if(The_first_few==0)
				  stx=statx;
		    Itemspritebg->setPosition(ccp(stx,dialog->getContentSize().height/2));
			dialog->addChild(Itemspritebg);

			BaseSprite *Itemsprite = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(Dt.giftBagVec.at(i).id));
			Itemsprite->setPosition(ccp(stx,dialog->getContentSize().height/2));
		    dialog->addChild(Itemsprite);

			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(Dt.giftBagVec.at(i).id).c_str(),Dt.giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(130, 0), kCCTextAlignmentCenter);
			nameLabel->setPosition(ccp(stx,dialog->getContentSize().height/2-width/2-30));
		    dialog->addChild(nameLabel);


			The_first_few++;
			if(The_first_few>=3)
				break;

		}


	}


	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&AwardDisplay::menuItemOkClicked1));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}
	

	{
		
		this->setZOrder(1025);
		this->setBaseContainer(dialog,menu);
	}
}


void AwardDisplay::menuItemClicked_Equip(CCObject *obj){

}

void AwardDisplay::menuItemOkClicked(CCObject* pSender)
{
	this->removeFromParent();
	if (mCallback) mCallback->awardDisplayCallback_func();
}

void AwardDisplay::menuItemOkClicked1(CCObject* pSender){

	this->removeFromParent();
}