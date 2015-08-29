#include "MainHeaderLayer.h"
#include "LFToast.h"


MainHeaderLayer::MainHeaderLayer(void)
{
	mHeader = NULL;
}
void MainHeaderLayer::setContent(BaseSprite* header)
{
	if (mHeader)
	{
		mHeader->removeFromParentAndCleanup(true);
	}

	mHeader = header;

	// 没有头部，就添加默认的头部
	if (mHeader == NULL)
		mHeader = createDefaultHead();

	if (mHeader)
	{
		addChild(mHeader);
		CCPoint originPos = CCDirector::sharedDirector()->getVisibleOrigin();
		CCSize visbleSize = CCDirector::sharedDirector()->getVisibleSize();
		mHeader->setPosition(ccp(mWinSize.width/2,originPos.y +visbleSize.height - mHeader->getContentSize().height/2));

		//防止头部下边的按钮被点中！！  （装备列表）
		CCMenuItemImage *headItem = CCMenuItemImage::create(mHeader->getFileName().c_str(), mHeader->getFileName().c_str(), mHeader->getFileName().c_str());
		headItem->setOpacity(0);
		headItem->setPosition(mHeader->getPosition());
		CCMenu *menu = CCMenu::create(headItem, NULL);
//		addChild(menu);
		menu->setPosition(CCPointZero);
	}
}

BaseSprite* MainHeaderLayer::createDefaultHead()
{
#define		MIN_RATIO	1.5f	// 最小的支持的长宽比
#define		MAX_RATIO	1.8f	// 最大的支持的长宽比

	float _ratio = mVisibleSize.height / mVisibleSize.width;

	// 无法完美适配的提示
	if (_ratio < MIN_RATIO)
		LFToast::makeToast(LF::getString("tooShort"));
	else if (_ratio > MAX_RATIO)
		LFToast::makeToast(LF::getString("tooLong"));

	if (_ratio >= MAX_RATIO)	// 最大开发值,无机型
	{
		return BaseSprite::create("dingkuang_192.png");
	}
	else if (_ratio >= 1.775f)	// iPhone5+小米2+小米1+HTC one + Surface
	{
		return BaseSprite::create("dingkuang_176.png");
	}
	else if (_ratio >= 1.6f)	// MX2
	{
		return BaseSprite::create("dingkuang_64.png");
	}
	else if (_ratio >= 1.5f)	// 刚大于iPhone4点点
	{
		return NULL;
	}

	// 更短的
	return NULL;
}

CCSize MainHeaderLayer::getHeaderSize()
{
	if (mHeader)
	{
		return mHeader->getContentSize();
	}
	return CCSizeMake(0,0);
}

MainHeaderLayer::~MainHeaderLayer(void)
{
}
