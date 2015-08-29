#include "ZhuangbeiXilianOverOption.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
using namespace FX_CommonFunc;
ZhuangbeiXilianOverOption::ZhuangbeiXilianOverOption()
	:CCSprite(),
	m_bIsFirst(false),
	m_bIsBiger(false)
{
}
ZhuangbeiXilianOverOption::~ZhuangbeiXilianOverOption()
{
}
bool ZhuangbeiXilianOverOption::init()
{
	if (!CCSprite::init())
	{
		return false;
	}

	return true;
}
void ZhuangbeiXilianOverOption::onEnter()
{
	CCSprite::onEnter();

	if (m_nLevel != -1)
	{
		BaseSprite *bgSpr = BaseSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");
		addChild(bgSpr);

		this->setContentSize(bgSpr->getContentSize());
		BaseSprite *xingxingImg = BaseSprite::create(CCString::createWithFormat("gongming_anniu_%d.png",m_nLevel)->getCString());
		bgSpr->addChild(xingxingImg);
		xingxingImg->setPosition(ccp(40, bgSpr->getContentSize().height / 2));
		xingxingImg->setScale(0.58f);

		ZBAttr zb_attr;
		zb_attr.attr_type = m_nAtter_type_index;
		zb_attr.attr_value = m_nPercent;
		string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
		CCLabelTTF *label = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, 20);
		bgSpr->addChild(label);
		label->setColor(ccc3(255,255,255));
		label->setPosition(ccp(bgSpr->getContentSize().width / 2, bgSpr->getContentSize().height / 2));

		bgSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height / 2));

		if (m_bIsBiger)
		{
			CCSprite *spr = CCSprite::create("renxing_zhuangbeitisheng.png");
			spr->setPosition(ccp(230,this->getContentSize().height / 2));
			bgSpr->addChild(spr);
		}
		if (m_bIsFirst)
		{
			CCSprite *spr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing_xiaoguo.png");
			spr->setPosition(ccp(this->getContentSize().width / 2,this->getContentSize().height / 2));
			bgSpr->addChild(spr);

			float time = 0;
			float minTime = 0.5f;
			float maxTime = 1.2f;
			time = CCRANDOM_0_1()*(maxTime - minTime) + minTime;
			Blink(spr,time);
		}
	}
	else
	{
		BaseSprite *xingxingImg = BaseSprite::create("zhuangbei_xiangxi_xilian_shuxing_suoding.png");
		addChild(xingxingImg);
		xingxingImg->setPosition(ccp(xingxingImg->getContentSize().width / 2,xingxingImg->getContentSize().height / 2));
		this->setContentSize(xingxingImg->getContentSize());
	}
}
void ZhuangbeiXilianOverOption::Blink(CCNode *node,float dur)
{
	node->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(dur / 2),CCFadeOut::create(dur / 2),NULL)));
}