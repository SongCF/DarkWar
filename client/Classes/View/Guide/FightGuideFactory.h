#pragma once
#include "cocos2d.h"
#include "BaseLayer.h"
#include "font.h"
using namespace cocos2d;

// 教学完毕
#define	MSG_FG_END	"Fight_Guide_End"

#define HISTORY_KEY_PREFIX	"FightGuide_"

enum GuideId
{
	GID_BloodGuide = 0,		// 第一次看到血瓶
	GID_FirstGetEquip,		// 第一次掉装备
	GID_FirstGetSkillBook,	// 第一次掉技能书
	GID_INIT,				// 第一次运行游戏的初始引导
};

class FightGuideFactory
{
private:
	static FightGuideFactory* factory;

	FightGuideFactory(void);

	string getFightGuideHistoryKey(GuideId id);
public:
	static FightGuideFactory* getFactory();
	// 是否某个guidelayer显示过了
	bool isFightGuideShowed(GuideId id);
	void setFightGuideShowed(GuideId id,bool showed = true);

	CCLayer* showBloodGuideLayer(CCNode* toAdd,CCSize bloodSize,CCPoint worldGlPos);
	CCLayer* showFirstGetEquipGuideLayer(CCNode* toAdd,CCSize equipSize,CCPoint worldGlPos);
	CCLayer* showFirstGetSkillBookGuideLayer(CCNode* toAdd,int skillBookType,CCSize bookSize,CCPoint worldGlPos);

	struct InitGuideConfig
	{
		CCNode* toAdd;
		CCRect rectGlPosForGuaiwu;	// 怪物的board
		CCRect rectGlPosForHero;	// hero的board
	};
	CCLayer* showInitGuideLayer(InitGuideConfig config);
	~FightGuideFactory(void);
};



#define	TAG_CONTINUE_LABLE			2431980
#define Guide_Priority				-1030	
#define LABLE_GAP_WITH_SCREEN		10
class FightGuideLayerBase : public BaseLayer
{
protected:
	BaseSprite* mMask;

	// 显示精灵，我们会以底部为对齐
	void showBuildSprite(CCSprite* s)
	{
		addChild(s,-1);
		s->setPosition(ccp(s->getContentSize().width/2,s->getContentSize().height/2));
	}

	// 显示点击继续
	void showClickScreenToContinue(float fSize = 30.0f)
	{
		showClickScreenToContinue(ccp(mVisibleSize.width/2,mVisibleSize.height/2),fSize);
	}
	// 显示点击继续
	void showClickScreenToContinue(CCPoint pos,float fSize = 30.0f)
	{
		removeChildByTag(TAG_CONTINUE_LABLE);

		CCLabelTTF* lable = CCLabelTTF::create(
			LF::getString("click_to_continue").c_str(),
			fontStr_kaiti,
			fSize,
			CCSizeMake(mVisibleSize.width * 0.8f,0),kCCTextAlignmentCenter);
		addChild(lable,5,TAG_CONTINUE_LABLE);
		lable->setColor(ccc3(255,255,255));

		lable->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height/2));

		lable->runAction(CCRepeat::create(
			CCSequence::create(
			CCFadeTo::create(0.5f,100),
			CCFadeTo::create(0.5f,255),NULL
			),-1));
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		userClicked();
		return true;	// 吞掉点击事件
	}

	virtual void userClicked(){}


	// 发送消息
	virtual void sendGuideEndMessage()
	{
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_FG_END,this);
	}

	// 结束guide
	virtual void guideEnd()
	{
		sendGuideEndMessage();
		// 设置显示过了
		FightGuideFactory::getFactory()->setFightGuideShowed(getId());
		removeFromParentAndCleanup(true);
	}

	virtual void onExit()
	{
		BaseLayer::onExit();
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	}
public:
	FightGuideLayerBase()
	{
		mMask = BaseSprite::create("zhandou_jiugongge_touming.png");
		mMask->retain();

		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,Guide_Priority,true);
	}

	CCSprite* createOneLightNode(CCSize size,CCPoint pos = CCPointZero)
	{
		CCSprite* node = CCSprite::create("lightNode.png");

		float scalex = size.width / node->getContentSize().width;
		float scaley = size.height / node->getContentSize().height;

		node->setScaleX(scalex);
		node->setScaleY(scaley);

		node->setPosition(pos);

		return node;
	}

	CCSprite* createDownArrow()
	{
		return BaseSprite::create("juqing_duihua_xiayiye.png");
	}

	// 创建一个亮的边框
	CCNode* createLightFrame(CCSize size,CCPoint worldGlPos)
	{
		CCScale9Sprite* frame = CCScale9Sprite::create("jiaoxue_tishikuang.png");
		frame->setPosition(worldGlPos);
		CCSize conSize = size;
		conSize.width += 15*2;
		conSize.height += 15*2;
		frame->setContentSize(conSize);
		return frame;
	}

	CCNode* createLable(string text,ccColor3B color,float fSize = 25.0f)
	{
		//CCLabelTTF* lable = CCLabelTTF::create(
		//	text.c_str(),
		//	fontStr_kaiti,
		//	fSize,
		//	CCSizeMake(mVisibleSize.width - LABLE_GAP_WITH_SCREEN * 4,0),kCCTextAlignmentLeft);
		CCLabelTTF* lable = CCLabelTTF::create(
			text.c_str(),
			fontStr_kaiti,
			fSize,
			CCSizeMake(0,0),kCCTextAlignmentLeft);

		float lableMaxWidth = mVisibleSize.width - LABLE_GAP_WITH_SCREEN * 4;
		if (lable->getContentSize().width > lableMaxWidth)
		{
			lable->setDimensions(CCSizeMake(lableMaxWidth,0));
		}
		lable->setAnchorPoint(ccp(0.5f,0.5f));
		CCScale9Sprite* base = CCScale9Sprite::create("jiaoxue_duihuaqipao.png");
		base->addChild(lable);

		float baseHight = lable->getContentSize().height + LABLE_GAP_WITH_SCREEN*4;
		//if (lable->getContentSize().height < 60.0f)
		//{
		//	baseHight = 60.0f + LABLE_GAP_WITH_SCREEN;
		//}

		base->setContentSize(CCSizeMake(
			lable->getContentSize().width + LABLE_GAP_WITH_SCREEN*2,
			baseHight
			));
		lable->setPosition(ccp(base->getContentSize().width/2,base->getContentSize().height/2));
		return base;
	}

	// 自动垂直布局引导信息
	// 把箭头，描述根据需要介绍的点，做成垂直布局
	void autoVerticalLayoutGuideInfor(CCNode* lable,CCNode* downArrow,CCPoint lightNodePos,CCSize lightNodeSize)
	{
		float lightNodeAndArrowGap = 10;
		float arrowAndLableGap = 10;

		float lable_posy = 0;
		float arrow_posy = 0;
		float arrow_angle = 0;
		// 寻找合适的上下地方显示描述信息
		{
			if (lightNodePos.y + lightNodeSize.height/2 + lightNodeAndArrowGap + downArrow->getContentSize().height + arrowAndLableGap + lable->getContentSize().height <= mVisibleSize.height - LABLE_GAP_WITH_SCREEN)
			{// 如果描述等放在上面的话，如果在可接受的可见范围，那么就使用“上置”

				arrow_posy = lightNodePos.y + lightNodeSize.height/2 + lightNodeAndArrowGap + downArrow->getContentSize().height/2;
				lable_posy = arrow_posy + downArrow->getContentSize().height/2 + arrowAndLableGap + lable->getContentSize().height/2;
				arrow_angle = 0;
			}
			else
			{// 就放下面
				arrow_posy = lightNodePos.y - lightNodeSize.height/2 - lightNodeAndArrowGap - downArrow->getContentSize().height/2;
				lable_posy = arrow_posy - downArrow->getContentSize().height/2 - arrowAndLableGap - lable->getContentSize().height/2;
				arrow_angle = 180;
			}
		}

		float arrow_posx = lightNodePos.x;
		float lable_posx = lightNodePos.x;
		if (lable_posx + lable->getContentSize().width/2 - mVisibleSize.width > -LABLE_GAP_WITH_SCREEN)
		{
			lable_posx = mVisibleSize.width - LABLE_GAP_WITH_SCREEN - lable->getContentSize().width/2;
		}
		if (lable_posx - lable->getContentSize().width/2 < LABLE_GAP_WITH_SCREEN)
		{
			lable_posx = LABLE_GAP_WITH_SCREEN + lable->getContentSize().width/2;
		}

		lable->setPosition(ccp(lable_posx,lable_posy));
		downArrow->setPosition(ccp(arrow_posx,arrow_posy));
		downArrow->setRotation(arrow_angle);
	}

	// 获得Id，是区分哪个Guide的途径
	virtual GuideId getId() = 0;

	~FightGuideLayerBase()
	{
		mMask->release();
	}
};