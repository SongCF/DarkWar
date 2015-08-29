#pragma once
#include "SkillTree.h"
#include "BaseActorPropertyParser.h"

// 血瓶的教学
class BloodGuideLayer : public FightGuideLayerBase
{
private:
	BloodGuideLayer(CCSize bloodSize,CCPoint worldGlPos)
	{
		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(bloodSize,worldGlPos));
		showBuildSprite(guide);
		showClickScreenToContinue();
		// 边框
		addChild(createLightFrame(bloodSize,worldGlPos));

		// 箭头
		CCSprite* arraw = createDownArrow();
		addChild(arraw);
		CCNode* lable = createLable(LF::getString("add_blood_auto"),ccWHITE);
		addChild(lable);

		autoVerticalLayoutGuideInfor(lable,arraw,worldGlPos,bloodSize);

		//arraw->setPosition(ccp(worldGlPos.x,worldGlPos.y + bloodSize.height/2 + arraw->getContentSize().height/2 + 10));
		//float posy = arraw->getPositionY() + arraw->getContentSize().height/2 + lable->getContentSize().height/2 + 5;
		//setLableNodePos(lable,ccp(worldGlPos.x,posy));

	}

	virtual void userClicked()
	{
		guideEnd();
	}

	virtual GuideId getId()
	{
		return GID_BloodGuide;
	}
public:
	static BloodGuideLayer* create(CCSize bloodSize,CCPoint worldGlPos)
	{
		BloodGuideLayer* layer = new BloodGuideLayer(bloodSize,worldGlPos);
		layer->autorelease();
		return layer;
	}

};


// 第一次得到装备
class FirstGetEquipGuideLayer : public FightGuideLayerBase
{
private:
	FirstGetEquipGuideLayer(CCSize equipSize,CCPoint worldGlPos)
	{
		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(equipSize,worldGlPos));
		showBuildSprite(guide);
		showClickScreenToContinue();
		// 边框
		addChild(createLightFrame(equipSize,worldGlPos));

		// 箭头
		CCSprite* arraw = createDownArrow();
		addChild(arraw);
		// 描述
		CCNode* lable = createLable(LF::getString("first_get_equip_guide"),ccWHITE);
		addChild(lable);

		autoVerticalLayoutGuideInfor(lable,arraw,worldGlPos,equipSize);

	}

	virtual void userClicked()
	{
		guideEnd();
	}

	virtual GuideId getId()
	{
		return GID_FirstGetEquip;
	}
public:
	static FirstGetEquipGuideLayer* create(CCSize equipSize,CCPoint worldGlPos)
	{
		FirstGetEquipGuideLayer* layer = new FirstGetEquipGuideLayer(equipSize,worldGlPos);
		layer->autorelease();
		return layer;
	}

};



// 第一次得到技能书
class FirstGetSkillBookGuideLayer : public FightGuideLayerBase
{
private:
	FirstGetSkillBookGuideLayer(CCSize bookSize,CCPoint worldGlPos,int skillType)
	{
		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(bookSize,worldGlPos));
		showBuildSprite(guide);
		showClickScreenToContinue();

		// 边框
		addChild(createLightFrame(bookSize,worldGlPos));

		// 箭头
		CCSprite* arraw = createDownArrow();
		addChild(arraw);
		// 描述
		SkillItem skill = SkillTree::sharedSkillTree()->getSkillInfor((SkillType)skillType);
		string profeName = BaseActorPropertyParser::getParser()->getExtraProerty(skill.actor_profession).nameInChinese;
		string skillName = skill.name;
		CCNode* lable = createLable(
			CCString::createWithFormat(
				LF::getString("first_get_skillbook_guide").c_str(),
				profeName.c_str(),
				skillName.c_str())->getCString(),
			ccWHITE);
		addChild(lable);

		autoVerticalLayoutGuideInfor(lable,arraw,worldGlPos,bookSize);
	}

	virtual void userClicked()
	{
		guideEnd();
	}

	virtual GuideId getId()
	{
		return GID_FirstGetSkillBook;
	}
public:
	static FirstGetSkillBookGuideLayer* create(CCSize bookSize,CCPoint worldGlPos,int skillType)
	{
		FirstGetSkillBookGuideLayer* layer = new FirstGetSkillBookGuideLayer(bookSize,worldGlPos,skillType);
		layer->autorelease();
		return layer;
	}

};

// 剧情初始引导里面的战斗介绍
class JuQingInFightGuideLayer: public FightGuideLayerBase
{
private:
	int guide_index;
	FightGuideFactory::InitGuideConfig mConfig;

	void showGuaiwuPosGuide()
	{
		removeAllChildren();

		CCPoint glPos = ccp(mConfig.rectGlPosForGuaiwu.getMidX(),mConfig.rectGlPosForGuaiwu.getMidY());
		CCSize size = mConfig.rectGlPosForGuaiwu.size;
		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(size,glPos));
		showBuildSprite(guide);
		showClickScreenToContinue();

		// 边框
		addChild(createLightFrame(size,glPos));

		// 箭头
		CCSprite* arraw = createDownArrow();
		addChild(arraw);
		CCNode* lable = createLable(LF::getString("shangfangweiguaiwuzhanwei"),ccWHITE);
		addChild(lable);

		autoVerticalLayoutGuideInfor(lable,arraw,glPos,size);
	}

	void showHeroPosGuide()
	{
		removeAllChildren();
		CCPoint glPos = ccp(mConfig.rectGlPosForHero.getMidX(),mConfig.rectGlPosForHero.getMidY());
		CCSize size = mConfig.rectGlPosForHero.size;

		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(size,glPos));
		showBuildSprite(guide);
		showClickScreenToContinue();

		// 边框
		addChild(createLightFrame(size,glPos));

		// 箭头
		CCSprite* arraw = createDownArrow();
		addChild(arraw);
		CCNode* lable = createLable(LF::getString("xiafangweizijizhanwei"),ccWHITE);
		addChild(lable);

		autoVerticalLayoutGuideInfor(lable,arraw,glPos,size);
	}

	void showFightIsAutoGuide()
	{
		removeAllChildren();
		CCPoint glPos = ccp(0,0);
		CCSize size = CCSizeMake(0,0);

		CCSprite* guide = LightMaskFactory::getCurrentFactory()->buildSprite(mMask,createOneLightNode(size,glPos));
		showBuildSprite(guide);
		showClickScreenToContinue();

		CCNode* lable = createLable(LF::getString("zhandouzijiyunxing"),ccWHITE);
		addChild(lable);

		lable->setPosition(ccp(mWinSize.width/2,mWinSize.height/2+lable->getContentSize().height));
	}

	JuQingInFightGuideLayer(FightGuideFactory::InitGuideConfig config)
	{
		mConfig = config;
		guide_index = 0;
		showGuaiwuPosGuide();
	}

	virtual void userClicked()
	{
		if (guide_index == 0)
		{
			showHeroPosGuide();
			guide_index ++;
		}
		else if(guide_index == 1)
		{
			showFightIsAutoGuide();
			guide_index ++;
		}
		else
		{
			guideEnd();
		}
	}

	virtual GuideId getId()
	{
		return GID_INIT;
	}
public:
	static JuQingInFightGuideLayer* create(FightGuideFactory::InitGuideConfig config)
	{
		JuQingInFightGuideLayer* layer = new JuQingInFightGuideLayer(config);
		layer->autorelease();
		return layer;
	}
};