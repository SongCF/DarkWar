#ifndef __ZhuangbeiOptionLevel_H__
#define __ZhuangbeiOptionLevel_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Equipment.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class ZhuangbeiOptionLevel : public CCSprite
{
public:
	ZhuangbeiOptionLevel();
	virtual ~ZhuangbeiOptionLevel();

	virtual bool init(Equipment *equipment);
	//CREATE_FUNC(ZhuangbeiOptionLevel);
	static ZhuangbeiOptionLevel *createWithEquipment(Equipment *equipment);
	virtual void onEnter();
public:

private:
	
private:
	Equipment					*mEquipment;
};

#endif // __ZhuangbeiOptionLevel_H__