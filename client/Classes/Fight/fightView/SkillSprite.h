#pragma once
#include "BaseSprite.h"

#define		SKILL_PIC_SCALE		(1.0f/0.4f)

//针对使用plist的技能的一个缩放 处理

/**
 * 对初始scale做了透明
 */
class SkillSprite : public BaseSprite
{
private:
	float mInitScale;
public:
	static SkillSprite* create(string file,float initScale = SKILL_PIC_SCALE)
	{
		SkillSprite* ret = new SkillSprite();
		ret->mInitScale = initScale;
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(file.c_str());
		if (frame == NULL)
		{
			CCLog("Error : %s --> frame is NULL, key = %s",__FUNCTION__,file.c_str());
			CCAssert(false,file.c_str());
			ret->init();
		}
		else
		{
			ret->initWithSpriteFrame(frame);
		}
		
		ret->setScale(1.0f);
		ret->autorelease();
		return ret;
	}
	static SkillSprite* create(float initScale = SKILL_PIC_SCALE)
	{
		SkillSprite* ret = new SkillSprite();
		ret->mInitScale = initScale;
		ret->init();
		ret->setScale(1.0f);
		ret->autorelease();
		return ret;
	}
	// 测试模式的话，就不会用SpriteFrame
	// 而是直接用图片，并且也不考率基准缩放
	// 主要用于有缩放的图册和原图的单张图片的程序的透明转换
	static SkillSprite* create_test(string file)
	{
		SkillSprite* ret = new SkillSprite();
		ret->mInitScale = 1.0f;
		ret->initWithFile(file);
		ret->autorelease();
		return ret;
	}

	float getInitScale(){return mInitScale;};

	virtual float getScaleY()
	{
		return BaseSprite::getScaleY()/mInitScale;
	}
	virtual float getScaleX()
	{
		return BaseSprite::getScaleX()/mInitScale;
	}

	virtual float getScale()
	{
		return BaseSprite::getScale()/mInitScale;
	}
	virtual void setScale(float fScale)
	{
		BaseSprite::setScale(fScale*mInitScale);
	}
	virtual void setScaleX(float fScaleX)
	{
		BaseSprite::setScaleX(fScaleX*mInitScale);
	}
	virtual void setScaleY(float fScaleY)
	{
		BaseSprite::setScaleY(fScaleY*mInitScale);
	}
};
