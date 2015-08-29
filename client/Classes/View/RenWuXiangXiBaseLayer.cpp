#include "RenWuXiangXiBaseLayer.h"
#include "LFUtile.h"
#include "BaseActorPropertyParser.h"
#include "font.h"
#include "ImageManager.h"
#include "SkillTree.h"
#include "JiNeng_SelectLayer.h"
#include "SkillManager.h"
#include "CS_Common.h"
#include "GuideLayerBase.h"
#include "MyselfManager.h"

#define FONT_SIZE 24
#define SkillDiscribeLabel_Color ccc3(243, 184, 101)

extern bool g_firstLearnSkillBook;

enum 
{
	Tag_Label_SkillName = 50,
	Tag_Label_SkillPictrue,
	Tag_Label_SkillDescribe,
	Tag_Layer_ZhouQi,

	Tag_MenuItem_JiNeng,
	Tag_Layer_ZhouQi1,
};


RenWuXiangXiBaseLayer::RenWuXiangXiBaseLayer(BaseActor* actor,JiNengLayer_comeFrom fro /* = From_RenWuZhuangBei */,bool isYuanShen)
{

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	
	   adaptivex=(visibleSize.width+origin.x)/640;

	   adaptivey=(visibleSize.height+origin.y)/960;
	
	

	CalculateData data = actor->getCalculateData();
	//list<BuffData> buff_null;
	//data.allBuffs = buff_null;
	//int e_num = actor->getEquipments().size();
	//vector<EquipData> equip_vec;
	//for (int i=0;i<e_num;i++)
	//{
	//	equip_vec.push_back(actor->getEquipments()[i]->getEquipData());
	//}
	//data.allEquips = equip_vec;
	//BaseActorProperty base_pro = actor->getCurrentProperty();
	//data.mainPro = actor->getMainPropertyValue();				// 这种的需不要，如需要就成死循环了
	//data.basePro = &base_pro;
	initWithData(actor,data,false,fro, isYuanShen);
}

RenWuXiangXiBaseLayer::RenWuXiangXiBaseLayer(BaseActor* actor,CalculateData newData,JiNengLayer_comeFrom fro /* = From_RenWuZhuangBei */,bool isYuanShen)
{
	initWithData(actor,newData,true,fro, isYuanShen);
}

void RenWuXiangXiBaseLayer::initWithData(BaseActor* actor,CalculateData newData,bool useCmp,JiNengLayer_comeFrom fro,bool isYuanShen)
{


	
	mIsYuanShen = isYuanShen;
	mActor = actor;
	mFrom = fro;
	BaseSprite* bg = BaseSprite::create("zhuye_gonggao_bg.png");
	addChild(bg);
	bg->setPosition(ccp(320*adaptivex,480*adaptivey));

	

	BaseSprite* rwmsgtext = BaseSprite::create("shuxing_renwu_biaoti.png");
	addChild(rwmsgtext);
	rwmsgtext->setPosition(ccp(bg->getPosition().x,bg->getPosition().y+355));//835
	mBg = bg;
	mNewData = newData;

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(actor->getActorType());
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	heroImg->setScale(0.8);
	addChild(heroImg);
	heroImg->setPosition(ccp(mBg->getPosition().x-(207),mBg->getPosition().y+267));

	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImgBg->setScale(0.8);
	addChild(heroImgBg);
	//heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
	heroImgBg->setPosition(ccp(mBg->getPosition().x-(207),mBg->getPosition().y+267));
	createData(bg,mActor,useCmp);


	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);

	CCMenuItemImage* itemBack = CCMenuItemImage::create(
		"close.png",
		"close_select.png",
		this,SEL_MenuHandler(&RenWuXiangXiBaseLayer::menuItemClicked)
		);
	itemBack->setPosition(ccp((mBg->getContentSize().width-itemBack->getContentSize().width),(mBg->getContentSize().height-itemBack->getContentSize().height)));
	mMainMenu->addChild(itemBack, 1, Tag_Item_Back);

	if(actor->getCurrentUseSkill() != NULL)
	{
		SkillTree *tree = SkillTree::sharedSkillTree();
		SkillType type = (SkillType)actor->getCurrentUseSkill()->getInfor()->skill_type;

		
		
		//技能
		BaseSprite* jinengBoard = BaseSprite::create("jineng_icon_bg.png");
		jinengBoard->setScale(0.7);
		jinengBoard->setPosition(ccp(mBg->getPosition().x-(226),mBg->getPosition().y-210));
		addChild(jinengBoard, 1, Tag_Item_JiNengBoard);
		addBckgroud(jinengBoard->getPositionY()-(jinengBoard->getContentSize().height*0.7)/2,jinengBoard->getPositionY()+(jinengBoard->getContentSize().height*0.7-10)/2,jinengBoard->getPositionY());

		
		//pic
		BaseSprite *pic = BaseSprite::create(ImageManager::getManager()->getSkillPicFileName(type));
		pic->setScale(0.6);
		addChild(pic, 1, Tag_Label_SkillPictrue);
		pic->setPosition(ccp(mBg->getPosition().x-(226),mBg->getPosition().y-210));
		
		
		
		BaseSprite *xiangImg = BaseSprite::create("duiwu_yingxiong_anniu_xiangxi.png");
		//jinengBoard->addChild(xiangImg, 2);
		xiangImg->setPosition(ccp(93, 36));
		//name //168, 602 
		CCString jinengStr;
		SkillInfo *info = mActor->getCurrentUseSkill()->getInfor();
		if (info->skill_level >= 2)
			jinengStr.initWithFormat("%s+%d", tree->getSkillNameByType(type).c_str(), info->skill_level - 1);
		else 
			jinengStr = tree->getSkillNameByType(type);

		
		CCLabelTTF *nameLabel = CCLabelTTF::create(jinengStr.getCString(), fontStr_kaiti, 24);
		addChild(nameLabel, 1, Tag_Label_SkillName);
		nameLabel->setPosition(ccp((mBg->getPositionX()-110+nameLabel->getContentSize().width/2),mBg->getPositionY()-124));

		//describe
		//  128+dscLabel->getContentSize().width/2),190*adaptivey
		CCLabelTTF *dscLabel = CCLabelTTF::create(tree->getSkillDescribeByType(type).c_str(), fontStr_kaiti, 20, CCSize(600-146, 0), kCCTextAlignmentLeft);
		addChild(dscLabel, 1, Tag_Label_SkillDescribe);
		dscLabel->setColor(SkillDiscribeLabel_Color);
		dscLabel->setPosition(ccp(mBg->getPosition().x-(182-dscLabel->getContentSize().width/2),mBg->getPosition().y-190));
		//升级提示
		
		BaseSprite *levTishiLabel = BaseSprite::create("shuxing_renwu_tishishengji.png");
		//mBg->addChild(levTishiLabel);
		levTishiLabel->setPosition(ccp(266, 838-652));
		//触发周期
		BaseSprite *base = BaseSprite::create();
		base->ignoreAnchorPointForPosition(false);
		base->setAnchorPoint(ccp(0.5,0.5));
		//addChild(base, 1, Tag_Layer_ZhouQi);
		
		

		
		CCLabelTTF *chufaLabel = CCLabelTTF::create(LFStrings::getValue("chufalunci").c_str(), fontStr_kaiti, 20);
		//base->addChild(chufaLabel);
		chufaLabel->setColor(ccc3(127, 127, 127));
		chufaLabel->setAnchorPoint(ccp(0, 0.5));
		chufaLabel->setPosition(ccp(0, 15));
		

		int chuFaDian = mActor->getCurrentSkillTurn();
		int chuFaZhouQi = mActor->getCurrentSkillZhouQi();

		
		for (int i=0; i!=chuFaZhouQi; ++i)
		{
			BaseSprite *img = NULL;
			if(i == chuFaDian-1)
				img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
			else 
				img = BaseSprite::create("jineng_chufalunci_icon.png");
		    addChild(img,1,Tag_Layer_ZhouQi1+i);
			img->setPosition(ccp((mBg->getPosition().x+120+i*img->getContentSize().width),mBg->getPosition().y-124));
			img->setScaleX(adaptivex);
			img->setScaleY(adaptivey);
		}

		//jineng item
		CCSprite* touming1 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,620,140));
		CCSprite* touming2 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,620,140));
		CCMenuItemSprite* jinengItem = CCMenuItemSprite::create(touming1, touming2, this, menu_selector(RenWuXiangXiBaseLayer::menuItemClicked));
		jinengItem->setScaleX(adaptivex);
		jinengItem->setScaleY(adaptivey);

	
	
		mMainMenu->addChild(jinengItem,1, Tag_MenuItem_JiNeng);

		jinengItem->setPosition(ccp(mWinSize.width/2, jinengBoard->getPositionY()));
		jinengItem->setOpacity(0);
		if(mFrom == comefrom_xiangxiDuiBi || mFrom == comefrom_otherMianBan)
		{
			jinengItem->setEnabled(false);
		}

		//本版本不开启技能列表
		if (true)
		{
			xiangImg->setVisible(false);
			jinengItem->setEnabled(false);
			levTishiLabel->setVisible(false);
			dscLabel->setPositionY(dscLabel->getPositionY()-10);
			base->setPositionY(base->getPositionY()+35);
		}

	}
}

void RenWuXiangXiBaseLayer::menuItemClicked(CCObject* sender)
{
	//子类重载
	removeFromParent();
}

void RenWuXiangXiBaseLayer::createCmpareItem(BaseSprite* contaner,int curData,int newData,CCPoint fontLeftPos,float fontSize,string fontName,bool percent /* = false */)
{
	// 百分号
	string end = percent?"%":"";
	const char* endChar = end.c_str();
	if (curData != newData)
	{
		CCLabelTTF* lable = LF::lable(this,CCString::createWithFormat("%d%s",curData,endChar),fontLeftPos,fontSize,fontName);
		lable->setScaleX(adaptivex);
		lable->setScaleY(adaptivey);
		float width = lable->getContentSize().width;
		if (newData > curData)
		{
			LF::lable(
				this,
				CCString::createWithFormat("+%d%s",newData-curData,endChar),
				ccp(fontLeftPos.x + width,fontLeftPos.y - lable->getContentSize().height*0.1f),
				fontSize*0.8f,fontName)->setColor(ccGREEN);
		}else
		{
			LF::lable(
				this,
				CCString::createWithFormat("-%d%s",curData-newData,endChar),
				ccp(fontLeftPos.x + width,fontLeftPos.y - lable->getContentSize().height*0.1f),
				fontSize*0.8f,fontName)->setColor(ccRED);
		}
	}else
	{
		LF::lable(this,CCString::createWithFormat("%d%s",curData,endChar),fontLeftPos,fontSize,fontName);
	}


}

void RenWuXiangXiBaseLayer::createData(BaseSprite* bg,BaseActor* actor,bool useCmp /* = false */)
{
// 	BaseSprite* content = BaseSprite::create("renwu_shuxing_xiangxi.png");
// 	bg->addChild(content);
// 	content->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	BaseSprite* content = bg;

#define GET_POS(X,Y) ccp(X+17,Y-94)
	float fontSize = 24;
	PropertyCalculator calculate;
	int jiyunlv=0;
	int bgHeight = mBg->getContentSize().height;

	//名字
	string zhiYeName = BaseActorPropertyParser::getParser()->getExtraProerty(actor->getActorType()).nameInChinese;
	if(zhiYeName.compare(LF::getString("mishu1"))==0){
		jiyunlv=10;
	}
	CCLabelTTF *zhiyeLabel = CCLabelTTF::create(zhiYeName.c_str(), fontStr_kaiti, fontSize);
	addChild(zhiyeLabel);
	zhiyeLabel->setPosition(ccp(mBg->getPositionX()-(150-zhiyeLabel->getContentSize().width/2),mBg->getPositionY()+292));


	BaseActor *yusnShen = mActor->getMyYuanShen();


	int rwdj;
	//人物等级
	if(mIsYuanShen){
	   rwdj=actor->getMyZhenShen()->getCurrentProperty().level;
	}else{
	   rwdj=actor->getCurrentProperty().level;
	}
	std::string strrwdj=CCString::createWithFormat("%d",rwdj)->getCString();
	strrwdj=LFStrings::getValue("renwudengji")+strrwdj;
	CCLabelTTF *rwdjLabel = CCLabelTTF::create(strrwdj.c_str(), fontStr_kaiti, fontSize);
	addChild(rwdjLabel);
	rwdjLabel->setPosition(ccp(zhiyeLabel->getPositionX()-zhiyeLabel->getContentSize().width/2+rwdjLabel->getContentSize().width/2, mBg->getPositionY()+264));

	
	

	
	//元神等级
	int yuanshenlevel=0;
	//元神等级
	
	if (yusnShen){
	    yuanshenlevel=yusnShen->getLevel();
	}

	if (mIsYuanShen){
		yuanshenlevel=mActor->getLevel();
	}
	std::string yslevel=CCString::createWithFormat("%d",yuanshenlevel)->getCString();
	yslevel=LFStrings::getValue("yuanshengdengji")+yslevel;

	CCLabelTTF *ysleveLabel = CCLabelTTF::create(yslevel.c_str(), fontStr_kaiti, fontSize);
	addChild(ysleveLabel);
	ysleveLabel->setPosition(ccp(rwdjLabel->getPositionX()-rwdjLabel->getContentSize().width/2+ysleveLabel->getContentSize().width/2, mBg->getPositionY()+237));


	if(mIsYuanShen){

	   createCCProgressTimer(0,actor,rwdjLabel,ysleveLabel);
	}else{
       createCCProgressTimer(1,actor,rwdjLabel,ysleveLabel);
	}
	//属性
	BaseSprite *sx_rw_nr_bt = BaseSprite::create("shuxing_renwu_neirong_biaoti.png");
	sx_rw_nr_bt->setPosition(ccp(mBg->getPositionX()-15,mBg->getPositionY()+174));
	addChild(sx_rw_nr_bt);

	//
	BaseSprite *sx_rw_nr_sx = BaseSprite::create("shuxing_renwu_neirong_biaoti_shuxing.png");
	sx_rw_nr_sx->setPosition(ccp(mBg->getPositionX()-213,mBg->getPositionY()+172));
	addChild(sx_rw_nr_sx);
	//职业属性
	CCLabelTTF *rwtypeLabel = CCLabelTTF::create("", fontStr_kaiti, fontSize);
	rwtypeLabel->setPosition(ccp(mBg->getPositionX()-83,mBg->getPositionY()+172));
	addChild(rwtypeLabel);
	
	//体能
	BaseSprite *sx_rw_nr_tn = BaseSprite::create("shuxing_renwu_tineng.png");
	sx_rw_nr_tn->setPosition(ccp(mBg->getPositionX()+65,mBg->getPositionY()+172));
	addChild(sx_rw_nr_tn);

	
	
	

    //属性详细说明
	BaseSprite *sx_rw_nr_sxxx = BaseSprite::create("shuxing_renwu_neirong_xiangxi.png");
	sx_rw_nr_sxxx->setPosition(ccp(mBg->getPositionX()-52,mBg->getPositionY()+26));
	addChild(sx_rw_nr_sxxx,1);

	addBckgroud(sx_rw_nr_sxxx->getPositionY()-(sx_rw_nr_sxxx->getContentSize().height+10)/2,sx_rw_nr_sxxx->getPositionY()+(sx_rw_nr_sxxx->getContentSize().height)/2,sx_rw_nr_sxxx->getPositionY());

	
	//技能属性
	BaseSprite *sx_rw_nr_btjn0 = BaseSprite::create("shuxing_renwu_neirong_biaoti.png");
	BaseSprite *sx_rw_nr_btjn1 = BaseSprite::create("shuxing_renwu_neirong_biaoti.png");
	CCMenuItemSprite* sx_rw_nr_btjn = CCMenuItemSprite::create(sx_rw_nr_btjn0,sx_rw_nr_btjn1,this,menu_selector(RenWuXiangXiBaseLayer::menuItemClicked));
	sx_rw_nr_btjn->setPosition(ccp(mBg->getPositionX()-15,mBg->getPositionY()-123));


	CCMenu *jnmenu=CCMenu::create();
	jnmenu->addChild(sx_rw_nr_btjn);
	jnmenu->setPosition(CCPointZero);
	addChild(jnmenu);
	
	//技能
	BaseSprite *sx_rw_nr_jnsprite = BaseSprite::create("shuxing_renwu_neirong_biaoti_jineng.png");
	sx_rw_nr_jnsprite->setPosition(ccp(mBg->getPositionX()-213,mBg->getPositionY()-124));

	addChild(sx_rw_nr_jnsprite);
    //技能名字
	CCLabelTTF *jnnameLabel = CCLabelTTF::create("", fontStr_kaiti, fontSize);
	
	jnnameLabel->setPosition(ccp(mBg->getPositionX()-83+jnnameLabel->getContentSize().width/2,mBg->getPositionY()-134));
	addChild(jnnameLabel);
	
    //触发伦次
	BaseSprite *sx_rw_nr_cslc = BaseSprite::create("shuxing_renwu_chufalunci.png");
	sx_rw_nr_cslc->setPosition(ccp(mBg->getPositionX()+55,mBg->getPositionY()-124));
	addChild(sx_rw_nr_cslc);

	


    
	/*
	//元神
	if (mIsYuanShen)
	{
		CCLabelTTF *yuansLabel = CCLabelTTF::create(("("+LFStrings::getValue("YuanShen")+")").c_str(), fontStr_kaiti, fontSize);
		//content->addChild(yuansLabel);
		addChild(yuansLabel);
		yuansLabel->setColor(ccRED);
		//yuansLabel->setPosition(ccp(zhiyeLabel->getPositionX()+zhiyeLabel->getContentSize().width+yuansLabel->getContentSize().width/2,zhiyeLabel->getPositionY()));
		yuansLabel->setPosition(ccp(mBg->getPositionX()-100,mBg->getPositionY()+237));
	}
	*/

	//主属性
	BaseSprite *zsxImg = NULL; 
	switch (mActor->getCurrentProperty().profession)
	{
	case Profession_Yemanren:
	case Profession_WuSeng:
	case Profession_Qishi:
		zsxImg = BaseSprite::create("shuxing_renwu_liliang.png");
		//createCmpareItem(content,actor->getLiliang(),calculate.calculateLiliang(mNewData),ccp(100, bgHeight-336),fontSize,fontStr_kaiti);
		break;
	case Profession_Chike:
	case Profession_ShenJianShou:
	case Profession_YouXia:
	case Profession_LieShou:
		zsxImg = BaseSprite::create("shuxing_renwu_minjie.png");
		//createCmpareItem(content,actor->getMinjie(),calculate.calculateMinJie(mNewData),ccp(100, mBgHeight-336),fontSize,fontStr_kaiti);
		break;
	case Profession_Fashi:
	case Profession_MiShu:
	case Profession_WuShi:
		zsxImg = BaseSprite::create("shuxing_renwu_zhili.png");
		//createCmpareItem(content,actor->getZhili(),calculate.calculateZhili(mNewData),ccp(100, bgHeight-336),fontSize,fontStr_kaiti);
		break;
	default:
		zsxImg = BaseSprite::create();
		CCLog("Warrior: ---> %s", __FUNCTION__);
		break;
	}
	addChild(zsxImg);
	zsxImg->setPosition(ccp(mBg->getPositionX()-83,mBg->getPositionY()+172));

	int newGongjili = calculate.calculateGongJiLi_MacroValue(mNewData,Range_Average);
	createCmpareItem(content,actor->getGongji_MacroValue(),newGongjili,ccp(mBg->getPositionX()-85,mBg->getPositionY()+115),fontSize,fontStr_kaiti);
	
	createCmpareItem(content,actor->getBaoji(),calculate.calculateBaoJiLv(mNewData),ccp(mBg->getPositionX()-85,mBg->getPositionY()+72),fontSize,fontStr_kaiti,true);
	createCmpareItem(content,actor->getBaoshang(),calculate.calculateBaoJiShangHaiYinzi(mNewData),ccp(mBg->getPositionX()-85,mBg->getPositionY()+29),fontSize,fontStr_kaiti,true);
	

	createCmpareItem(content,jiyunlv,jiyunlv,ccp(mBg->getPositionX()-85,mBg->getPositionY()-14),fontSize,fontStr_kaiti,true);
	createCmpareItem(content,0,0,ccp(mBg->getPositionX()-85,mBg->getPositionY()-57),fontSize,fontStr_kaiti,true);

	createCmpareItem(content,actor->getHujia(),calculate.calculateHuJiaZhi(mNewData),ccp(mBg->getPositionX()+185,mBg->getPositionY()+112),fontSize,fontStr_kaiti);

	createCmpareItem(content,0,0,ccp(mBg->getPositionX()+185,mBg->getPositionY()+72),fontSize,fontStr_kaiti,true);
	createCmpareItem(content,0,0,ccp(mBg->getPositionX()+185,mBg->getPositionY()+29),fontSize,fontStr_kaiti,true);
	createCmpareItem(content,0,0,ccp(mBg->getPositionX()+185,mBg->getPositionY()-14),fontSize,fontStr_kaiti,true);
	
	createCmpareItem(content,actor->getShanbiLv(),calculate.calculateShanBiLv(mNewData),ccp(mBg->getPositionX()+185,mBg->getPositionY()-57),fontSize,fontStr_kaiti,true);
	
	//体能
	createCmpareItem(content,actor->getTineng(),calculate.calculateTineng(mNewData),ccp(mBg->getPositionX()+120,mBg->getPositionY()+172),fontSize,fontStr_kaiti);

}


RenWuXiangXiBaseLayer::~RenWuXiangXiBaseLayer(void)
{
}

// 禁用触摸
void RenWuXiangXiBaseLayer::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
}
// 启用触摸

void RenWuXiangXiBaseLayer::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	refreshSkill();
}

void RenWuXiangXiBaseLayer::refreshSkill()
{

	CCLOG("refesh--->");

	
	BaseSprite *jinengBoard = (BaseSprite*)mBg->getChildByTag(Tag_Item_JiNengBoard);
	SkillTree *tree = SkillTree::sharedSkillTree();
	SkillType type = (SkillType)mActor->getCurrentUseSkill()->getInfor()->skill_type;
	//pic
	//BaseSprite *pic = (BaseSprite*)jinengBoard->getChildByTag(Tag_Label_SkillPictrue);
	BaseSprite *pic = (BaseSprite*)mBg->getChildByTag(Tag_Label_SkillPictrue);
	if (pic) pic->initWithFile(ImageManager::getManager()->getSkillPicFileName(type));
	//name //168, 602
	CCString jinengStr;
	SkillInfo *info = mActor->getCurrentUseSkill()->getInfor();
	if (info->skill_level >= 2)
		jinengStr.initWithFormat("%s+%d", tree->getSkillNameByType(type).c_str(), int(info->skill_level - 1));
	else 
		jinengStr = tree->getSkillNameByType(type);
	CCLabelTTF *nameLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_SkillName);
	if (nameLabel) nameLabel->setString(jinengStr.getCString());
// 	nameLabel->setAnchorPoint(ccp(0, 0.5));
// 	nameLabel->setPosition(ccp(174, 838-602));
	//describe

	CCLabelTTF *dscLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_SkillDescribe);
	if (dscLabel) dscLabel->setString(tree->getSkillDescribeByType(type).c_str());
// 	dscLabel->setColor(SkillDiscribeLabel_Color);
// 	dscLabel->setAnchorPoint(ccp(0, 1));
// 	dscLabel->setPosition(ccp(174, nameLabel->getPositionY()-nameLabel->getContentSize().height));
	//升级tishi
	//触发周期
	//BaseSprite *base = (BaseSprite*)this->getChildByTag(Tag_Layer_ZhouQi);
// 	base->setAnchorPoint(ccp(0, 0.5));
// 	base->setPosition(ccp(505,296));
	//base->removeAllChildren();


	//
	CCLabelTTF *chufaLabel = CCLabelTTF::create(LFStrings::getValue("chufalunci").c_str(), fontStr_kaiti, 20);
	//base->addChild(chufaLabel);
	chufaLabel->setColor(ccc3(127, 127, 127));
	chufaLabel->setAnchorPoint(ccp(0, 0.5));
	chufaLabel->setPosition(ccp(0, 15));

	int chuFaDian = mActor->getCurrentSkillTurn();
	int chuFaZhouQi = mActor->getCurrentSkillZhouQi();
	for (int i=0; i!=chuFaZhouQi; ++i)
	{
		this->removeChildByTag(Tag_Layer_ZhouQi1+i);
		BaseSprite *img = NULL;
		if(i == chuFaDian-1)
			img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
		else 
			img = BaseSprite::create("jineng_chufalunci_icon.png");
		addChild(img,1,Tag_Layer_ZhouQi1+i);
		img->setPosition(ccp((mBg->getPosition().x+120+i*img->getContentSize().width),mBg->getPosition().y-124));
	}
	
}

string RenWuXiangXiBaseLayer::getZhuShuXingStrByProfession(Profession profId)
{
	string str;

	switch (profId)
	{
	case Profession_Yemanren:
		str = LFStrings::getValue("liliang");//
		break;
	case Profession_Chike:
		str = LFStrings::getValue("minjie");//
		break;
	case Profession_Fashi:
		str = LFStrings::getValue("zhili");
		break;
	case Profession_Qishi:
		str = LFStrings::getValue("liliang");//
		break;
	case Profession_ShenJianShou:
		str = LFStrings::getValue("minjie");//
		break;
	case Profession_MiShu:
		str = LFStrings::getValue("zhili");
		break;
	case Profession_WuSeng:
		str = LFStrings::getValue("liliang");//
		break;
	case Profession_YouXia:
		str = LFStrings::getValue("minjie");//
		break;
	case Profession_LieShou:
		str = LFStrings::getValue("minjie");//
		break;
	case Profession_WuShi:
		str = LFStrings::getValue("zhili");
		break;
	default:
		break;
	}

	return (" (" + str + ")");
}

void RenWuXiangXiBaseLayer::onEnter()
{
	BaseLayer::onEnter();


	if (isUI_Guide_LearnSkill && g_firstLearnSkillBook)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(mMainMenu->getChildByTag(Tag_MenuItem_JiNeng)->getContentSize(), 
			mMainMenu->convertToWorldSpace(mMainMenu->getChildByTag(Tag_MenuItem_JiNeng)->getPosition()), LFStrings::getValue(""), NULL, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
}


void  RenWuXiangXiBaseLayer::createCCProgressTimer(int isZhensen,BaseActor* actor,CCLabelTTF *p1,CCLabelTTF *p2){

	   unsigned int zscurrent=0;
	   unsigned int zsneed=0;
	   unsigned int yuansc=0;
	   unsigned int yuannd=0;
	  
	   if(isZhensen==1){

		   zscurrent=actor->getCurrentProperty().exp_current;
		   zsneed=actor->getCurrentProperty().exp_need;
		   BaseActor *yS = mActor->getMyYuanShen();
		   if(yS!=NULL){
		    yuansc=yS->getCurrentProperty().exp_current;
	        yuannd=yS->getCurrentProperty().exp_need;
		   }
	   }else{
	
		   BaseActor *zs=actor->getMyZhenShen();
		   zscurrent=zs->getCurrentProperty().exp_current;
		   zsneed=zs->getCurrentProperty().exp_need;
		   yuansc=actor->getCurrentProperty().exp_current;
	       yuannd=actor->getCurrentProperty().exp_need;
	   }
	 

	   if(zsneed>0){

		BaseSprite* tiLiBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
		addChild(tiLiBarBoarder,1, 10000);
		tiLiBarBoarder->setPosition(ccp(p1->getPositionX()+p1->getContentSize().width/2+tiLiBarBoarder->getContentSize().width/2+10, p1->getPositionY()));

	
		CCProgressTimer* tiLiBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
		tiLiBarBoarder->addChild(tiLiBar, 2, 101);
		tiLiBar->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
		tiLiBar->setPercentage((((float)zscurrent/(float)zsneed))* 100.0f);
		float cs=(float)zscurrent/(float)zsneed;
	   

		CCString temp;
		temp.initWithFormat("%u/%u", zscurrent, zsneed);
		CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
		tiliLabel->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
		tiLiBarBoarder->addChild(tiliLabel, 2, 102);

	  }else{

		CCLabelTTF *tiliLabel = CCLabelTTF::create(LF::getString("dengjiyiman").c_str(), fontStr_BookAntiqua, 15);
		tiliLabel->setPosition(ccp(p1->getPositionX()+p1->getContentSize().width/2+tiliLabel->getContentSize().width/2+10, p1->getPositionY()));
		tiliLabel->setColor(ccc3(245,0,0));
		addChild(tiliLabel);
	  }



	  if(isZhensen==0||(isZhensen==1&&mActor->getMyYuanShen()!=NULL)){
	   
		  if(yuannd>0){

			BaseSprite* tiLiBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
			addChild(tiLiBarBoarder,1, 10000);
			tiLiBarBoarder->setPosition(ccp(p1->getPositionX()+p1->getContentSize().width/2+tiLiBarBoarder->getContentSize().width/2+10, p2->getPositionY()));

	
			CCProgressTimer* tiLiBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
			tiLiBarBoarder->addChild(tiLiBar, 2, 101);
			tiLiBar->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
			tiLiBar->setPercentage(((float)yuansc/(float)yuannd) * 100.0f);
	        float cs=(float)yuansc/(float)yuannd;
	        

			CCString temp;
			temp.initWithFormat("%u/%u", yuansc, yuannd);
			CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
			tiliLabel->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
			tiLiBarBoarder->addChild(tiliLabel, 2, 102);

		  }else{

			CCLabelTTF *tiliLabel = CCLabelTTF::create(LF::getString("dengjiyiman").c_str(), fontStr_BookAntiqua, 15);
			tiliLabel->setPosition(ccp(p1->getPositionX()+p1->getContentSize().width/2+tiliLabel->getContentSize().width/2+10, p2->getPositionY()));
			tiliLabel->setColor(ccc3(245,0,0));
			addChild(tiliLabel);
		  }
	  
	  }

}

void RenWuXiangXiBaseLayer::addBckgroud(float start,float end,float hight){


	    {
			BaseSprite *sx_rw_nr_sxxxk = BaseSprite::create("gonggao_neirong_kuang.png");
			sx_rw_nr_sxxxk->setPosition(ccp(mBg->getPosition().x,start));
			addChild(sx_rw_nr_sxxxk);
		}


		BaseSprite *back=BaseSprite::create("gonggao_neirong_kuang_2.png");
		back->setPosition(ccp(mBg->getPosition().x,hight));
		back->setScaleY((end-start-10)/back->getContentSize().height);
		addChild(back,0);
		{
			BaseSprite *sx_rw_nr_sxxxk = BaseSprite::create("gonggao_neirong_kuang.png");
			sx_rw_nr_sxxxk->setPosition(ccp(mBg->getPosition().x,end));
			addChild(sx_rw_nr_sxxxk);
		}

}