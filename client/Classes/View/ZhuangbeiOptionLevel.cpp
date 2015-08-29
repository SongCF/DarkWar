#include "ZhuangbeiOptionLevel.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
#include "CS_Common.h"
using namespace FX_CommonFunc;
ZhuangbeiOptionLevel::ZhuangbeiOptionLevel()
	:CCSprite()
{
}
ZhuangbeiOptionLevel::~ZhuangbeiOptionLevel()
{
}
ZhuangbeiOptionLevel *ZhuangbeiOptionLevel::createWithEquipment(Equipment *equipment)
{
	ZhuangbeiOptionLevel *layer = new ZhuangbeiOptionLevel();
	if(layer && layer->init(equipment))
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
bool ZhuangbeiOptionLevel::init(Equipment *equipment)
{
	if (!CCSprite::init())
	{
		return false;
	}
	int width = 0;
	int height = 0;
	mEquipment = equipment;
	vector<ZBAttr> data_list = mEquipment->getEquipData().purify_attr_vec;
	int max_count = CS::getPurifyMaxHoles(mEquipment->getEquipData().zhuangbei_type,mEquipment->getEquipColor(),mEquipment->getLevel());
	int count = data_list.size();
	int i = 0;
	int x = 0;

	string name_str[4] = {"gongjiang_qianghua_jieguo_tie.png","gongjiang_qianghua_jieguo_tong.png","gongjiang_qianghua_jieguo_yin.png","gongjiang_qianghua_jieguo_jin.png"};
	for (vector<ZBAttr>::iterator iter = data_list.begin();iter != data_list.end(); iter ++)
	{
		CCSprite *xingxingImg = CCSprite::create(name_str[ ( CS::getPurifyLevel(iter->attr_value) + 1 ) / 2 - 1].c_str());
		xingxingImg->setScale(0.3f);
		xingxingImg->setPosition(ccp(/*xingxingImg->getContentSize().width * xingxingImg->getScaleX() * 0.5f + */x,xingxingImg->getContentSize().height  * xingxingImg->getScaleY() / 2));
		addChild(xingxingImg);
		i++;
		height = xingxingImg->getContentSize().height * xingxingImg->getScaleY();
		width = i * xingxingImg->getContentSize().width * xingxingImg->getScaleX();
		x += xingxingImg->getContentSize().width * xingxingImg->getScaleX();
		//x -= 5;
	}
	for (int j = i ; j < max_count; j ++)
	{
		CCSprite *xingxingImg = CCSprite::create("gongjiang_qianghua_jieguo_kong.png");
		xingxingImg->setScale(0.3f);
		xingxingImg->setPosition(ccp(/*xingxingImg->getContentSize().width * xingxingImg->getScaleX() * 0.5f + */x,xingxingImg->getContentSize().height  * xingxingImg->getScaleY() / 2));
		addChild(xingxingImg);
		i++;
		height = xingxingImg->getContentSize().height * xingxingImg->getScaleY();
		width = i * xingxingImg->getContentSize().width * xingxingImg->getScaleX();
		x += xingxingImg->getContentSize().width * xingxingImg->getScaleX();
		//x -= 5;
	}
	this->setContentSize(CCSizeMake(width,height));
	return true;
}
void ZhuangbeiOptionLevel::onEnter()
{
	CCSprite::onEnter();

	
}