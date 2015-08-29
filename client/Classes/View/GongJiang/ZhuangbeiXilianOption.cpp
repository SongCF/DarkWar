#include "ZhuangbeiXilianOption.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
using namespace FX_CommonFunc;
ZhuangbeiXilianOption::ZhuangbeiXilianOption()
	:CCSprite(),
	m_bIsSelect(false),
	m_bIsLevelSheng(false),
	m_bIsValueSheng(false)
{
}
ZhuangbeiXilianOption::~ZhuangbeiXilianOption()
{
	if(isAnimation==1)
	  CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
}
bool ZhuangbeiXilianOption::init()
{
	if (!CCSprite::init())
	{
		return false;
	}
	
	return true;
}
void ZhuangbeiXilianOption::onEnter()
{
	CCSprite::onEnter();
	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu,1);
	
	//m_bIsLevelSheng = true;
	
	if (m_nLevel != -1)
	{
		BaseSprite *bgSpr = BaseSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");
		addChild(bgSpr);

		this->setContentSize(bgSpr->getContentSize());
		BaseSprite *xingxingImg = BaseSprite::create(CCString::createWithFormat("gongming_anniu_%d.png",(m_nLevel + 1 ) / 2)->getCString());
		bgSpr->addChild(xingxingImg);
		xingxingImg->setPosition(ccp(40, bgSpr->getContentSize().height / 2));
		xingxingImg->setZOrder(1023);
		//xingxingImg->setScale(0.58f);
		
		if (m_bIsLevelSheng)
		{
			/*
			xingxingImg->setScale(2.5f);
			xingxingImg->runAction(CCScaleTo::create(0.3f,1.0f));
			*/
			int level=(m_nLevel + 1 ) / 2;
			Qianhuadh(xingxingImg,level);
		}
		ZBAttr zb_attr;
		zb_attr.attr_type = m_nAtter_type_index;
		zb_attr.attr_value = m_nPercent;
		string attrStr;

		if (m_nAtter_type_index == AttrType_skillresult)
		{
			attrStr = CCString::createWithFormat("+%d%%%s",m_nPercent,LFStrings::getValue("jiNengShanghai").c_str())->getCString();
		}
		else
		{
			attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
		}
		
		CCLabelTTF *label = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, 20);
		bgSpr->addChild(label);
		label->setColor(ccc3(255,255,255));
		label->setPosition(ccp(bgSpr->getContentSize().width / 2, bgSpr->getContentSize().height / 2));

		bgSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height / 2));

		m_iSelectMenuItem = CCMenuItemImage::create("duiwu_yingxiong_daixuan.png","duiwu_yingxiong_xuanzhong.png",this,SEL_MenuHandler(&ZhuangbeiXilianOption::clickSelect));
		menu->addChild(m_iSelectMenuItem);
		m_iSelectMenuItem->setPosition(ccp(230,bgSpr->getContentSize().height / 2));
		//m_iMenuItem = selectItem;
		
		m_sSelectedSpr = CCSprite::create("duiwu_yingxiong_xuanzhong.png");
		bgSpr->addChild(m_sSelectedSpr);
		m_sSelectedSpr->setPosition(ccp(230,this->getContentSize().height / 2));
		m_sSelectedSpr->setVisible(false);
		if (m_bIsValueSheng)
		{
			CCLabelTTF *actionTTF = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, 20);
			bgSpr->addChild(actionTTF);
			actionTTF->setColor(ccc3(255,255,255));
			actionTTF->setPosition(ccp(bgSpr->getContentSize().width / 2, bgSpr->getContentSize().height / 2));

			//actionTTF->runAction(CCSequence::create(CCSpawn::create(CCScaleTo::create(1.0f,10.0f),CCFadeOut::create(1.0f),NULL),
			//CCCallFuncN::create(this,SEL_CallFuncN(&ZhuangbeiXilianOption::runActionOver)),NULL));
		}


	}
	else
	{
		BaseSprite *xingxingImg = BaseSprite::create("zhuangbei_xiangxi_xilian_shuxing_suoding.png");
		addChild(xingxingImg);
		xingxingImg->setPosition(ccp(xingxingImg->getContentSize().width / 2,xingxingImg->getContentSize().height / 2));
		this->setContentSize(xingxingImg->getContentSize());
	}

	m_mMenu = menu;
}
void ZhuangbeiXilianOption::clickSelect(CCObject *obj)
{
	m_bIsSelect = true;
	CCMenuItemImage *item = (CCMenuItemImage *)obj;
	CCNode *node = CCNode::create();
	node->setTag(m_nIndex);
	(m_oTarger->*m_hCallBackHandler)(node);

	m_iSelectMenuItem->setEnabled(false);
	m_iSelectMenuItem->setVisible(false);
	m_sSelectedSpr->setVisible(true);

	
}
void ZhuangbeiXilianOption::selectOther(int index)
{
	if (m_nIndex != index)
	{
		m_iSelectMenuItem->setEnabled(true);
		m_iSelectMenuItem->setVisible(true);
		m_sSelectedSpr->setVisible(false);
	}
}
void ZhuangbeiXilianOption::selectSelf()
{
	m_bIsSelect = true;
	CCNode *node = CCNode::create();
	node->setTag(m_nIndex);
	(m_oTarger->*m_hCallBackHandler)(node);

	m_iSelectMenuItem->setEnabled(false);
	m_iSelectMenuItem->setVisible(false);

	m_sSelectedSpr->setVisible(true);
}
void ZhuangbeiXilianOption::Blink(CCNode *node,float dur)
{
	node->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(dur / 2),CCFadeOut::create(dur / 2),NULL)));
}
void ZhuangbeiXilianOption::addNewShuxing()
{
	CCSprite *spr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing_xiaoguo.png");
	spr->setPosition(ccp(this->getContentSize().width / 2,this->getContentSize().height / 2));
	this->addChild(spr);

	float time = 0;
	float minTime = 0.5f;
	float maxTime = 1.2f;
	time = CCRANDOM_0_1()*(maxTime - minTime) + minTime;
	Blink(spr,time);
}


void ZhuangbeiXilianOption::Qianhuadh(BaseSprite *bs,int level){


	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
	CCArmature *armature = CCArmature::create("gongjiang_xiaoguo");
	armature->setAnchorPoint(ccp(0.5,0.5));
	string name="";

	if(level==3){
		name="xilian_yinxing";
	}else if(level==4){
		name="xilian_jinxing";
	}else{
		name="xilian_putong";
	}
	armature->setZOrder(1024);
	armature->getAnimation()->play(name.c_str());
	armature->getAnimation()->setMovementEventCallFunc(this, 
		  SEL_MovementEventCallFunc(&ZhuangbeiXilianOption::callfunc_AnimateEnd));
	armature->setPosition(ccp(bs->getPositionX()+bs->getContentSize().width*2-5,bs->getPositionY()-bs->getContentSize().height-15));
	bs->addChild(armature);
}

void ZhuangbeiXilianOption::callfunc_AnimateEnd(CCArmature *ar2, MovementEventType, const char *){

	ar2->removeFromParent();
	isAnimation=1;

}

