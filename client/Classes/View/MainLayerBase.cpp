#include "MainLayerBase.h"
#include "MainLayerRenWuBody.h"
#include "MainLayerZhuangBeiBody.h"
#include "MainLayerStoreFrame.h"
//#include "ChooseFightBody.h"
//#include "NPCBodyLayer.h"
#include "Music.h"
#include "StateBar.h"
#include "MyselfManager.h"
#include "WuPinManager.h"
#include "HeroRankList.h"
#include "MainLayerJingJi.h"
#include "font.h"
#include "NpcFuBenLayer.h"
#include "MapsLayer.h"

static MainLayerBase* s_MainLayerBase = NULL;

MainLayerBase::MainLayerBase(void)
{
// 	BaseSprite* container = BaseSprite::create("function_frame.png");
// 	addChild(container,-1);
// 	CCPoint originPos = CCDirector::sharedDirector()->getVisibleOrigin();
// 	container->setPosition(ccp(mWinSize.width/2,originPos.y + container->getContentSize().height/2));

	s_MainLayerBase = this;

	// 头部
	mHeader = new MainHeaderLayer();
	addChild(mHeader,2);
	mHeader->release();

	mFooter = new MainFooterLayer();
	addChild(mFooter,2);
	mFooter->release();
	selectedContaint = NULL;

	mFooter->setCallBack(this);

	Music::sharedMusicPlayer()->playMainMusic();
}


void MainLayerBase::onEnter()
{
	BaseLayer::onEnter();
	//if (s_MainLayerBase != NULL)
	//{
	//	CCAssert(false,"这是个异常，或者内存中已经有一个MainLayerBase在渲染了");
	//}
	s_MainLayerBase = this;
	CCLog("MainLayerBase::onEnter");
}
void MainLayerBase::onExit()
{
	BaseLayer::onExit();
	s_MainLayerBase = NULL;
	CCLog("MainLayerBase::onExit"); 
}
MainLayerBase* MainLayerBase::getCurrentMainBase()
{
	return s_MainLayerBase;
}

void MainLayerBase::footerItemSelected(enum_MainLayerIndex index)
{
	BaseLayer* layer = createContentAtIndex(index);
	setContaintLayer(layer);

	if (index == mainlayer_ZhuYe)
	{
		mHeader->setContent(StateBar::createBar());
		resetContentBodyPos(selectedContaint);

		// 主页特殊处理
		IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(layer);

		if (body)
		{
			layer->setPosition(ccp(layer->getPositionX(), body->getBodyCenterPos().y - body->getBodySize().height/2));
		}

	}
	else if(index == mainlayer_ShangDian || index == mainlayer_ChongZhi)
	{
		BaseSprite* head = BaseSprite::create("zhuangtailan_jinqian.png");
		//金币银币图标
		CCSprite* pJinbi = CCSprite::create("jinbi_tubiao.png");
		head->addChild(pJinbi, 1);
		pJinbi->setPosition(ccp(253, 36));

		CCSprite* pYinbi = CCSprite::create("yinbi_tubiao.png");
		head->addChild(pYinbi, 1);
		pYinbi->setPosition(ccp(450, pJinbi->getPositionY()));

		BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
		BaseActorProperty pro = actor->getCurrentProperty();
		CCLabelTTF* name = LF::lable(head, pro.customName, ccp(33, head->getContentSize().height/2), 25, fontStr_kaiti);
		name->setTag(STORE_FRAM_TAG_HEAD_NAME);

		WuPin* gold = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		WuPin* yingBi = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		int goldNum = gold!=NULL?gold->getCount():0;
		int yinBiNum = yingBi!=NULL?yingBi->getCount():0;

		LF::lable(head,goldNum,ccp(282,head->getContentSize().height/2),25,fontStr_BookAntiqua)->setTag(STORE_FRAM_TAG_HEAD_JINBI);
		LF::lable(head,yinBiNum,ccp(485,head->getContentSize().height/2),25,fontStr_BookAntiqua)->setTag(STORE_FRAM_TAG_HEAD_YINBI);

		//
		mHeader->setContent(head);
		resetContentBodyPos(selectedContaint);
	}
	
}
void MainLayerBase::setContaintLayer(BaseLayer* content,bool removeHead /* = true */)
{
	// 必须是IMainLayerBody类型
	CCAssert(dynamic_cast<IMainLayerBody*>(content)!=NULL,"");

	if(selectedContaint)
	{
		selectedContaint->removeFromParent();
	}

	// 移除所有
	// bug修正
	// 需要在initFoot之前
	// 因为如果在initFoot之后，那么在initFoot中pop的layer就移除了
	removeAllPop();

	// 这句代码一定要在initFoot之前
	// 因为会有如下一个bug：
	// 如果在initFoot之后
	// 那么可能会有layer在initFoot时刻popUpLayer一下
	// 那么就pop到了上一个content里面了，于是就看不到了
	selectedContaint = content;


	// 注入layer,需要提前注入
	IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(content);
	if (body)
	{
		body->initFoot(this);
	}else
	{
		CCAssert(false,"异常！");
	}

	// 要在设置位置之前移除头部
	if (removeHead)
	{
		mHeader->setContent(NULL);
	}

	// 提前做坐标偏移
	resetContentBodyPos(content);
	CCPoint posTemp = content->getPosition();
	addChild(content);
	CCPoint posTemp2 = content->getPosition();

	// layer在onEnter之类的方法把坐标偷偷换了，Assert
	if (posTemp.x != posTemp2.x || posTemp.y != posTemp2.y)
	{
		CCAssert(false,"是不是不小心自己设置了layer的位置？？？？");
	}

	// bug,或者是不好的做法，应该直接remove就可以了
	// 如果激活的话，那么可能很多东西都还没初始化
	// dismmisAllPop();

	//// BUG：注入layer,需要提前注入
	//IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(content);
	//if (body)
	//{
	//	body->initFoot(this);
	//}else
	//{
	//	CCAssert(false,"异常！");
	//}

	// bug :
	// 不应该在最后做位置偏移，而是应该在addChild之前
	// 不然layer里面做的坐标转换就会出问题，因为之前坐标转换是layer在A点
	// 但是后来被我们偷偷改成了B点，就出问题了
	//resetContentBodyPos(content);
}
void MainLayerBase::removeAllPop()
{
	while (!popLayerStack.empty())
	{
		BaseLayer* layer = popLayerStack.top();
		popLayerStack.pop();
		layer->removeFromParent();
	}
	mFooter->setVisible(true);
}
void MainLayerBase::dismmisAllPop()
{
	removeAllPop();
	if (selectedContaint!=NULL)
	{
		selectedContaint->setVisible(true);  //add by scf
		selectedContaint->disableAllTouchEnd();
		mFooter->setVisible(true);//
	}
}

BaseLayer* MainLayerBase::getTopLayer(int backIndex /* = 0 */)
{
	if (popLayerStack.empty()) 
	{
		CCLog("Waring : %s---> popLayerStack is empty",__FUNCTION__);
		return NULL;
	}

	if (backIndex > 0)
	{
		CCLog("Waring : %s---> backIndex need <= 0",__FUNCTION__);
		return NULL;
	}
	if (abs(backIndex) > popLayerStack.size()-1)
	{
		CCLog("Waring : %s---> backIndex need <= size - 1",__FUNCTION__);
		return NULL;
	}

	stack<BaseLayer*>tempBuff;
	while(backIndex < 0)
	{
		tempBuff.push(popLayerStack.top());
		popLayerStack.pop();
		backIndex ++;
	}
	BaseLayer* ret = popLayerStack.top();

	// 恢复
	while (!tempBuff.empty())
	{
		popLayerStack.push(tempBuff.top());
		tempBuff.pop();
	}

	return ret;
}

void MainLayerBase::dismmisPopUp()
{
	if (!popLayerStack.empty())
	{
		BaseLayer* layer = popLayerStack.top();
		popLayerStack.pop();
		layer->removeFromParent();
	}
	// 恢复新的先处理的layer可用
	if ( !popLayerStack.empty() )  
	{
		popLayerStack.top()->setVisible(true);  // add by scf
		popLayerStack.top()->disableAllTouchEnd();

		IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(popLayerStack.top());
		if (body->getPopPos() == Pop_Pos_WondowDown) mFooter->setVisible(false);
		else mFooter->setVisible(true);
	}
	else // 没有数据了，那么就把根，恢复了
	{
		if (selectedContaint!=NULL)
		{
			selectedContaint->setVisible(true); // add by scf
			selectedContaint->disableAllTouchEnd();
			mFooter->setVisible(true);//
		}
	}
}

void MainLayerBase::popUpContaintLayer(BaseLayer* content,PopUpPos pos /* = Pop_Pos_Center */)
{
	// 必须是IMainLayerBody类型
	CCAssert(dynamic_cast<IMainLayerBody*>(content)!=NULL,"");

	if (!popLayerStack.empty())
	{
		popLayerStack.top()->disableAllTouchBegin();
		if (dynamic_cast<IMainLayerBody*>(popLayerStack.top())->getBodySize().height <= dynamic_cast<IMainLayerBody*>(content)->getBodySize().height+10)
			popLayerStack.top()->setVisible(false);   //add by scf
	}else
	{
		// 栈顶本来就没有数据，那么就直接把根禁用了
		if (selectedContaint!=NULL)
		{
			selectedContaint->disableAllTouchBegin();

			if (dynamic_cast<IMainLayerBody*>(selectedContaint)->getBodySize().height <= dynamic_cast<IMainLayerBody*>(content)->getBodySize().height+10)
				selectedContaint->setVisible(false); // add by scf
		}
	}

	popLayerStack.push(content);

	// 先注入layer
	IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(content);
	if (body)
	{
		body->initFoot(this);
	}else
	{
		CCAssert(false,"异常！");
	}
	body->setPopPos(pos);
	resetContentBodyPos(content,pos);
	CCPoint posTemp = content->getPosition();
	addChild(content,1);
	CCPoint posTemp2 = content->getPosition();
	// layer在onEnter之类的方法把坐标偷偷换了，Assert
	if (posTemp.x != posTemp2.x || posTemp.y != posTemp2.y)
	{
		CCAssert(false,"是不是不小心自己设置了layer的位置？？？？");
	}
}

void MainLayerBase::resetContentBodyPos(CCLayer* content,PopUpPos pos /* = Pop_Pos_Center */)
{
	// 剩余空间居中
	IMainLayerBody* body = dynamic_cast<IMainLayerBody*>(content);
	if (body)
	{
		//// 注入layer ---> 移动到了popUpContaintLayer和setContaintLayer里面了
		//body->initFoot(this);
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		float headerHight =  mHeader->getHeaderSize().height;
		float footerHeight = mFooter->getFooterSize().height;
		// 计算出该在Y方向上做的偏移
        float posyOffset = 0;
		//pos = Pop_Pos_FromFoot;
		switch (pos)
		{
		case Pop_Pos_Center:
			posyOffset = -body->getBodyCenterPos().y +visibleSize.height/2 - headerHight/2 +footerHeight/2;
			mFooter->setVisible(true);
			break;
		case Pop_Pos_FromFoot:
			posyOffset = -body->getBodyCenterPos().y + body->getBodySize().height/2 + footerHeight;
			mFooter->setVisible(true);
			break;
		case Pop_Pos_WondowDown:
			posyOffset = -body->getBodyCenterPos().y + body->getBodySize().height/2;
			mFooter->setVisible(false);
			break;
		default:
			CCLog("Error:%s:%d --> pos unkown",__FILE__,__LINE__);
			CCAssert(false,"未知参数");
			break;
		}
		content->setPosition(ccp(0,0 + posyOffset));
	}else
	{
		CCAssert(false,"");
	}
}

BaseLayer* MainLayerBase::createContentAtIndex(enum_MainLayerIndex index)
{
	BaseLayer* ret = NULL;
	switch (index)
	{
	case mainlayer_ZhuYe:
		ret = new MainLayerRenWuBody();
		ret->autorelease();
		break;
	case mainlayer_MaoXian:// 冒险
		//ret = new ChooseFightBody();
		ret = new MapsLayer;
		ret->autorelease();
		break;
	case mainlayer_JingJiChang_challenge:// 竞技
		//ret = new ChooseFightBody();
		ret = MainLayerJingJi::create(1);
		break;
	case mainlayer_JingJiChang_random:// 竞技
		//ret = new ChooseFightBody();
		ret = MainLayerJingJi::create(2);
		break;
	case mainlayer_HuoDong:// 活动
		//ret = new ChooseFightBody();
		ret = NpcFuBenLayer::create();
		break;
	case mainlayer_ShangDian:
		ret = new MainLayerStoreFrame(store_ShenBing);
		ret->autorelease();
		break;
	case mainlayer_ChongZhi:
		ret = new MainLayerStoreFrame(store_ChongZhi);
		ret->autorelease();
		break;
	case mainlayer_ZhuangBei:
		ret = new MainLayerZhuangBeiBody();
		ret->autorelease();
		break;
	case mainlayer_YingXiongBang:
		ret = new HeroRankList();
		ret->autorelease();
		break;
	default:
		ret =new TestLayer();
		ret->autorelease();

		CCLabelTTF *label = CCLabelTTF::create("Footer Not implement!", "Helvetica", 40.0);
		label->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));
		ret->addChild(label);
		return ret;
	}
	return ret;
}

CCSize MainLayerBase::getMaxContentBodySize()
{
	CCSize size = mVisibleSize;
	size.height -= mFooter->getFooterSize().height;
	size.height -= mHeader->getHeaderSize().height;

	return size;
}

MainLayerBase::~MainLayerBase(void)
{
}
