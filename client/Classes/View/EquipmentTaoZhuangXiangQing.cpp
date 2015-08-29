#include "EquipmentTaoZhuangXiangQing.h"
#include "ImageManager.h"
#include "SuitPool.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "ZhuangBeiMianBanOther.h"
#include "GuideLayerBase.h"
#include "History.h"

using namespace FX_CommonFunc;


//只穿这一件equipedGroupIdVec.size == 1  和  没有穿它equipedGroupIdVec.size == 0
EquipmentTaoZhuangXiangQing::EquipmentTaoZhuangXiangQing(unsigned int curGroupId, string curName, vector<unsigned int> _equipedGroupIdVec,
														 TaoZhuangLayerInterface* callfunc)
{
	mCallfunc = callfunc;
	m_nShuxingHeight = 0;
	mBg = BaseSprite::create("zhuangbei_xiangxi_mianban_taozhuang.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

	mMenu = CCMenu::create();
	mBg->addChild(mMenu);
	mMenu->setPosition(ccp(0,0));

	mMenu->setTouchPriority(-129);//邮件弹窗需要提高一级
	//////////////////////////////////////////////////////////////////////////
	ccColor3B showColor = ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, false, true);


	//
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png",
		"shuxing_renwu_fanhui_select.png", 
		this, menu_selector(EquipmentTaoZhuangXiangQing::menuItemClicked_MainMenu));
	mMenu->addChild(fanhui, 0, 100);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
	//
	int teach_start_y = 0;
	int teach_end_y = 0;
	OneSuitInfo suitInfo;
	if(SuitPool::getPool()->getSuitInforByZBGroupId(curGroupId, suitInfo))
	{
		//获取信息成功

		CCSize boardSize = mBg->getContentSize();
		ccColor3B labelColor_huise = ccc3(127, 127, 127);
		int labelSize = 24;
		int start_x = 28;
		int start_y = 838- 140;
		int label_gap = 6;
		char buf[20];
		
		// 0 title (name)
		string strName = suitInfo.suit_name;
		CCLabelTTF *labelTitle = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, 30);
		mBg->addChild(labelTitle);
		labelTitle->setPosition(ccp(boardSize.width/2, 838-35));
		labelTitle->setColor(showColor);

		teach_start_y = labelTitle->getPositionY() + labelTitle->getContentSize().height / 2;
		// 1 name
		CCLabelTTF *labelName = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, 28);
		mBg->addChild(labelName);
		labelName->setAnchorPoint(ccp(0, 0.5));
		labelName->setPosition(ccp(start_x, 838-100));
		labelName->setColor(showColor);

		//当前详情界面的这件套装 有没有穿上、 没穿 则只有它的名字是白色、套装属性一个也没有
		//只穿这一件equipedGroupIdVec.size == 1  和  没有穿它equipedGroupIdVec.size == 0
		bool hasEquiped = false;   
		if (_equipedGroupIdVec.size() > 0)
			hasEquiped = true;
		else 
			hasEquiped = false;


		// 3 every one name
		int taozhuangNum = suitInfo.parts_of_suit.size(); //记录有多少件
		for (int i=0; i!=suitInfo.parts_of_suit.size(); ++i)
		{
			string strOneName = suitInfo.parts_of_suit.at(i).zhuangbei_name;
			string strTypeName = getPosName_ByZhuangBeiType(suitInfo.parts_of_suit.at(i).zhuanbei_type);
			CCLabelTTF *labelOneName = CCLabelTTF::create((strOneName + " (" + strTypeName + ")").c_str(), fontStr_kaiti, labelSize);
			mBg->addChild(labelOneName);
			labelOneName->setAnchorPoint(ccp(0, 0.5));
			labelOneName->setPosition(ccp(start_x, start_y));

			//如果没有穿上、 且不是当前详情展示的这件装备  则变灰
			if (! hasEquiped && strOneName != curName)
			{
				labelOneName->setColor(labelColor_huise);
				--taozhuangNum;
			}
			//如果穿上了、则要显示套装属性哦
			else if (hasEquiped)
			{
				bool curHas = false;
				for (int jNum=0,c=_equipedGroupIdVec.size(); jNum<c; ++jNum)
				{
					if (suitInfo.parts_of_suit.at(i).containsGroupId(_equipedGroupIdVec[jNum]))
					{
						curHas = true;
						break;
					}
				}
				if (! curHas)
				{
					labelOneName->setColor(labelColor_huise);
					--taozhuangNum;
				}
			}

			start_y -= (labelOneName->getContentSize().height + label_gap);
		}


		// 4 套装属性显示   (2)件
		for(int i=0; i<suitInfo.parts_of_suit.size()-1; ++i)
		{
			ccColor3B color;
			if(i+2 <= taozhuangNum)
				color = showColor;
			else 
				color = labelColor_huise;
			// 2、3、4、5件
			sprintf(buf, "(%d)", i+2);
			string strjian = buf + LFStrings::getValue("jian");
			CCLabelTTF *labelJian = CCLabelTTF::create(strjian.c_str(), fontStr_kaiti, labelSize);
			mBg->addChild(labelJian);
			labelJian->setAnchorPoint(ccp(0, 0.5));
			labelJian->setPosition(ccp(start_x, start_y));
			labelJian->setColor(color);

			start_y -= (labelJian->getContentSize().height + label_gap);
			
			//属性
			for (int j=0; j<=1; ++j)
			{
				if (suitInfo.suit_attr[i][j].attr_type == 0)
					continue;

				CCLog("%s ---> [i=%d][j=%d]", __FUNCTION__, i, j);
				BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
				mBg->addChild(biaoji);
				biaoji->setPosition(ccp(66, start_y));

				string strShuXing = GetMoFaShuXing_ByZBAttr(suitInfo.suit_attr[i][j]);
				CCLabelTTF *labelShuxing = CCLabelTTF::create(strShuXing.c_str(), fontStr_kaiti, labelSize);
				mBg->addChild(labelShuxing);
				labelShuxing->setAnchorPoint(ccp(0, 0.5));
				labelShuxing->setPosition(ccp(biaoji->getPositionX() + biaoji->getContentSize().width, start_y));
				labelShuxing->setColor(color);

				start_y -= (labelShuxing->getContentSize().height + label_gap);

				teach_end_y = labelShuxing->getPositionY() - labelShuxing->getContentSize().height / 2;
				//m_nShuxingHeight += labelShuxing->getContentSize().height + label_gap;
			}
			//m_nShuxingHeight += labelJian->getContentSize().height;
		}

	}
	m_nShuxingHeight = teach_start_y - teach_end_y;
}

void EquipmentTaoZhuangXiangQing::menuItemClicked_MainMenu(CCObject *pSender)
{
	if (mCallfunc != NULL)
		mCallfunc->callbackInterface_TaoZhuangLayer();
}

void EquipmentTaoZhuangXiangQing::onEnter()
{
	BaseLayer::onEnter();


	//教学
	bool firstDisplay_ChuanQiEquip = false;
	if (! History::getHistory()->getBoolFromCurrentHero("First_DisplayChuanQiEquip", firstDisplay_ChuanQiEquip))
	{
		firstDisplay_ChuanQiEquip = true;
		//升级界面再设置  
		//History::getHistory()->putInCurrentHero("First_DisplayChuanQiEquip", false);
	}
	if (firstDisplay_ChuanQiEquip && g_FirstTaoZhuangLook)
	{
		g_FirstTaoZhuangLook = false;

		GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
		CCMenuItem *fanhui = (CCMenuItem*)mMenu->getChildByTag(100);
		if (fanhui)
		{
			guideLayer2->init(
				fanhui->getContentSize(), 
				fanhui->getParent()->convertToWorldSpace(fanhui->getPosition()), 
				"", NULL, false);

			GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
			guideLayer1->init(
				CCSize(580, m_nShuxingHeight), 
				mBg->convertToWorldSpace(ccp(320, 758-460/2)), 
				LFStrings::getValue("chuanqi_ChuanQiTaoZhuangShuXing"), guideLayer2, true);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 1024);
		}
	}
}