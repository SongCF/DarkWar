#include "FX_CommonFunc.h"

#include "WuPinManager.h"
#include "CS_Common.h"
#include "ImageManager.h"
#include "font.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "MyselfManager.h"
#include "ZhuangBeiMianBanOther.h"
#include "ZhuangBeiMianBanYuanShen.h"
#include "MainLayerBase.h"

#define YOFFSET			122
#define Label_Size_Min 21

#define LABEL_COLOR ccc3(79, 110, 217)
#define COLOR_Hui ccc3(127,127,127)
#define COLOR_Huang ccc3(195, 145, 96)

BaseSprite * FX_CommonFunc::getNeedMaterialBoard(ItemsType type, unsigned int needCount, bool& isHave)
{
	BaseSprite *board = BaseSprite::create("daoju_kuang_bg.png");
	CCSize size = board->getContentSize();
	CCString temp;

	BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(type));
	board->addChild(itemImg);
	itemImg->setPosition(ccp(size.width/2, size.height/2));

	CCLabelTTF *nameLabel = CCLabelTTF::create(CS::getItemName(type).c_str(), fontStr_kaiti, Label_Size_Min);
	board->addChild(nameLabel);
	nameLabel->setColor(COLOR_Huang);
	nameLabel->setPosition(ccp(size.width/2, -nameLabel->getContentSize().height/2));
	//need
	temp.initWithFormat("%u/", needCount);
	CCLabelTTF *needLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
	nameLabel->addChild(needLabel);
	needLabel->setAnchorPoint(ccp(0,0.5));
	//all
	int allCount = WuPinManager::getManager()->getWuPin(type)->getCount();
	temp.initWithFormat("%d", allCount);
	CCLabelTTF *allLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
	needLabel->addChild(allLabel);
	allLabel->setAnchorPoint(ccp(0, 0.5));
	allLabel->setPosition(ccp(needLabel->getContentSize().width, needLabel->getContentSize().height/2));
	needLabel->setPosition(ccp(nameLabel->getContentSize().width/2-(allLabel->getContentSize().width+needLabel->getContentSize().width)/2,
		-needLabel->getContentSize().height/2));
	if (needCount > allCount)
	{
		isHave = false;
		allLabel->setColor(ccRED);
	}

// 	float offset = needLabel->getContentSize().width + allLabel->getContentSize().width;
// 	nameLabel->setPosition(ccp(size.width/2-offset/2, -nameLabel->getContentSize().height/2-10));

	return board;
}


string FX_CommonFunc::getStrByInt(unsigned int inter)
{
	string str;
	switch (inter)
	{
	case 0:
		str = LFStrings::getValue("ling");
		break;
	case 1:
		str = LFStrings::getValue("yi");
		break;
	case 2:
		str = LFStrings::getValue("er");
		break;
	case 3:
		str = LFStrings::getValue("san");
		break;
	case 4:
		str = LFStrings::getValue("si");
		break;
	case 5:
		str = LFStrings::getValue("wu");
		break;
	case 6:
		str = LFStrings::getValue("liu");
		break;
	case 7:
		str = LFStrings::getValue("qi");
		break;
	case 8:
		str = LFStrings::getValue("ba");
		break;
	case 9:
		str = LFStrings::getValue("jiu");
		break;
	case 10:
		str = LFStrings::getValue("shi");
	default:
		break;
	}

	return str;
}

void FX_CommonFunc::convertFubenDiaoluoZBInfo2DuanzaoGroupZBInfo(Flat_FubenDiaoluoZBInfo &_in, Flat_DuanzaoGroupZBInfo &_out)
{

	_out.zb_group_id        = _in.group_id;
	_out.zhuangbei_type     = (ZhuangbeiType)_in.zhuangbei_type;
	_out.zhuangbei_name     = _in.zhuangbei_name;
	_out.zhuangbei_colour   = (ZhuangbeiColour)_in.zhuangbei_colour;
	_out.zhuangbei_pic      = _in.zhuangbei_pic;
	_out.level              = _in.level;
	_out.ratio              = 0;   //对于掉落查询 无用、
	_out.gongji_min_min     = _in.gongji_min_min;
	_out.gongji_min_max     = _in.gongji_min_max;
	_out.gongji_max_min     = _in.gongji_max_min;
	_out.gongji_max_max     = _in.gongji_max_max;
	_out.wufang_min         = _in.wufang_min;
	_out.wufang_max         = _in.wufang_max;
	_out.hole1_percent      = _in.hole1_percent;
	_out.hole2_percent      = _in.hole2_percent;
	_out.hole3_percent      = _in.hole3_percent;
	_out.itemVec            = _in.itemVec;
	_out.zhuangbei_desc     = _in.desc;
}
void FX_CommonFunc::refreshEquipBoard(BaseSprite* board, Flat_DuanzaoGroupZBInfo zbInfo)
{
	MainLayerBase::getCurrentMainBase()->getFooter()->setVisible(false);

	//name
	CCLabelTTF *labelName = CCLabelTTF::create(zbInfo.zhuangbei_name.c_str(), fontStr_kaiti, 28);
	board->addChild(labelName);
	ccColor3B namecolor = ImageManager::getManager()->getShowColor((ZhuangbeiColour)zbInfo.zhuangbei_colour
		, false
		, SuitPool::getPool()->isSuit(zbInfo.zb_group_id));
	labelName->setColor(namecolor);
	labelName->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-36));



	//info label
	BaseSprite *base  = BaseSprite::create();
	CCSize containerSize = CCSizeMake(640,838+YOFFSET);
	board->addChild(base,0);
	base->setPosition(ccp(0, 0));

#define LABEL_SIZE 26
	//类型
	float label_gap = 2;
	float label_y = containerSize.height - 78;
	float start_x = 250;
	{
		CCLabelTTF* label;
		string str = CCString::createWithFormat("%d", zbInfo.level)->getCString() + LFStrings::getValue("ZhuangbeiXiangqing_ZhuangbeiLeixing");
		label = LF::lable(base,str,ccp(0,0),24,fontStr_kaiti);
		label->setAnchorPoint(ccp(0, 0.0));
		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height));

		ccColor3B color = ImageManager::getManager()->getShowColor(zbInfo.zhuangbei_colour, false, false);

		label->setColor(color);

		label_y -= label_gap + label->getContentSize().height;
	}
	bool bWuqi = false;
	CCLabelTTF* label;
	//白字
	{
		
		float baizi_1 = 0, baizi_2 = 0;
		{
			switch(zbInfo.zhuangbei_type)
			{
			case ZhuangbeiType_DanshouJian:
			case ZhuangbeiType_ShuangshouBishou:
			case ZhuangbeiType_ShuangshouFu:
			case ZhuangbeiType_DanshouFazhang:
			case ZhuangbeiType_ShuangshouFazhang:
			case ZhuangbeiType_Gong:
				baizi_1 = (zbInfo.gongji_min_min + zbInfo.gongji_min_max) / 2.0;
				baizi_2 = (zbInfo.gongji_max_min + zbInfo.gongji_max_max) / 2.0;
				bWuqi = true;
				break;
			case ZhuangbeiType_Dunpai:
			case ZhuangbeiType_KuijiaZhong:
			case ZhuangbeiType_KuijiaQin:
				baizi_1 = zbInfo.wufang_min;
				baizi_2 = zbInfo.wufang_max;
				break;
			case ZhuangbeiType_Jiezhi:
			case ZhuangbeiType_Hushenfu:
			case ZhuangbeiType_Faqi:
			default:
				break;
			}
		}
		if(baizi_2 > 0.1) // 只有武器、 衣服、盾牌  有白质
		{
			char str[64];
			sprintf(str, "%.1f-%.1f", baizi_1, baizi_2);
			label = LF::lable(base,str,ccp(0,0),50,fontStr_BookAntiqua);
			label->setColor(ccWHITE);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

			label_y -= label_gap + label->getContentSize().height;
		}
	}
	//伤害详细
	if (bWuqi)
	{
// 		CCLabelTTF* label;
// 		char str[64];
// 		sprintf(str, "(%u-%u)-(%u-%u)", zbInfo.gongji_min_min, zbInfo.gongji_min_max, zbInfo.gongji_max_min, zbInfo.gongji_max_max);
// 		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr);
// 		label->setColor(ccWHITE);
// 		label->setAnchorPoint(ccp(0, 0.5));
// 		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		CCLabelTTF* label2;
		label2 = LF::lable(base,LFStrings::getValue("dianShangHai"),ccp(0,0),LABEL_SIZE,fontStr_kaiti);
// 		ccColor3B color;
// 		color.r = 100;
// 		color.g = 98;
// 		color.b = 97;
//		label2->setColor(color);
		label2->setAnchorPoint(ccp(0, 0.5));
		label2->setPosition(ccp(label->getPosition().x + label->getContentSize().width, 
			label->getPositionY()-label->getContentSize().height/2+label2->getContentSize().height/2));
//		label2->setPosition(ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()));
//		label_y -= label_gap + label->getContentSize().height;
	}

	//等级
	{
// 		CCLabelTTF* label;
// 		char str[64];
// 		sprintf(str, "%s %u ", LFStrings::getValue("dengji").c_str(), zbInfo.level);
// 		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr_kaiti);
// 		ccColor3B color;
// 		color.r = 100;
// 		color.g = 98;
// 		color.b = 97;
// 		label->setColor(color);
// 		label->setAnchorPoint(ccp(0, 0.5));
// 		label->setPosition(ccp(start_x, label_y - label_gap*3 - label->getContentSize().height/2));
// 
// 		label_y -= label_gap*6 + label->getContentSize().height;
	}
	//带孔概率
	{
		if(zbInfo.hole1_percent != 0)
		{
			char buf[20];
			sprintf(buf, "%d%%", zbInfo.hole1_percent);
			string temp = LFStrings::getValue("dai") + LFStrings::getValue("yi") + LFStrings::getValue("ge")
				+ LFStrings::getValue("kong") + LFStrings::getValue("gailv") + buf;
			CCLabelTTF *label = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
			base->addChild(label);
			label->setAnchorPoint(ccp(0, 0.5));
			//			label->setColor(ccc3(79, 110, 217));
			label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));
			label_y -= label_gap + label->getContentSize().height;
			if(zbInfo.hole2_percent != 0)
			{
				sprintf(buf, "%d%%", zbInfo.hole2_percent);
				string temp = LFStrings::getValue("dai") + LFStrings::getValue("er") + LFStrings::getValue("ge")
					+ LFStrings::getValue("kong") + LFStrings::getValue("gailv") + buf;
				CCLabelTTF *label = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
				base->addChild(label);
				label->setAnchorPoint(ccp(0, 0.5));
				//				label->setColor(ccc3(79, 110, 217));
				label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));
				label_y -= label_gap + label->getContentSize().height;
				if(zbInfo.hole3_percent != 0)
				{
					sprintf(buf, "%d%%", zbInfo.hole3_percent);
					string temp = LFStrings::getValue("dai") + LFStrings::getValue("san") + LFStrings::getValue("ge")
						+ LFStrings::getValue("kong") + LFStrings::getValue("gailv") + buf;
					CCLabelTTF *label = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
					base->addChild(label);
					label->setAnchorPoint(ccp(0, 0.5));
					//					label->setColor(ccc3(79, 110, 217));
					label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));
					label_y -= label_gap + label->getContentSize().height;
				}
			}
			label_y -= label_gap*3;
		}
	}
	//魔法属性
	{
		label_y -= label_gap;

		for(int i=0; i<zbInfo.itemVec.size(); ++i)
		{
			int allAttr = zbInfo.itemVec.at(i).getZbAttrsIgnoreGaiLvAndCount().size();

			char buf[20];
			sprintf(buf, "%d%%", zbInfo.itemVec.at(i).gailv);
			string temp = buf + LFStrings::getValue("fudaixiamianderenyi");
			sprintf(buf, "%d/%d", zbInfo.itemVec.at(i).count, allAttr);
			temp += buf + LFStrings::getValue("ge") + LFStrings::getValue("shuxing");
			CCLabelTTF *dscLabel = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
			base->addChild(dscLabel);
			dscLabel->setAnchorPoint(ccp(0, 0.5));
			dscLabel->setColor(ccc3(127, 127, 127));
			dscLabel->setPosition(ccp(start_x, label_y - label_gap - dscLabel->getContentSize().height/2));
			label_y = label_y - label_gap - dscLabel->getContentSize().height;

			int delta_x = 20;
			for (int num=0; num!=allAttr; ++num)
			{
				BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
				base->addChild(biaoji);
				CCLabelTTF* label;
				label = LF::lable(base,getAttrbuteItemString(zbInfo.itemVec.at(i), num),ccp(0,0),LABEL_SIZE,fontStr_kaiti);
				label->setColor(ccc3(79, 110, 217));
				label->setAnchorPoint(ccp(0, 0.5));
				label->setPosition(ccp(start_x+biaoji->getContentSize().width + 10 + delta_x, label_y - label_gap - label->getContentSize().height/2));
				//
				biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2 + delta_x, label->getPositionY()));

				label_y = label_y - label_gap - label->getContentSize().height;
			}
			label_y -= label_gap*3;
		}
	}


	//描述
//	{
		float Y = containerSize.height - 700 - YOFFSET+10;

		CCLabelTTF* label1 = CCLabelTTF::create(zbInfo.zhuangbei_desc.c_str(), fontStr_kaiti, LABEL_SIZE, CCSize(590, 60), kCCTextAlignmentLeft);
		base->addChild(label1);

		label1->setAnchorPoint(ccp(0, 1));
		label1->setPosition(ccp(25, Y));
		label1->setColor(fonColor_CiYao);
//	}


	//套装信息
	OneSuitInfo suitInfo;
	if(SuitPool::getPool()->getSuitInforByZBGroupId(zbInfo.zb_group_id, suitInfo))
	{

		//传奇套装
		BaseSprite* pTaozhuang = NULL;
		if (zbInfo.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
		{
			pTaozhuang = BaseSprite::create("zhuangbei_xiangxi_chuanqitaozhuang.png");
		}
		else if (zbInfo.zhuangbei_colour == ZhuangbeiColour_Xiyou)
		{
			pTaozhuang = BaseSprite::create("zhuangbei_xiangxi_chuanqitaozhuang.png");
			pTaozhuang->setVisible(false);
		}
		if (pTaozhuang)
		{
			base->addChild(pTaozhuang, 1);
			pTaozhuang->setPosition(ccp(123+110-pTaozhuang->getContentSize().width/2, 
				838+122-240-140-pTaozhuang->getContentSize().height));

			if (!pTaozhuang->isVisible())
			{
				pTaozhuang->setPositionY(838+122-240-140);
			}

			int start_y = pTaozhuang->getPositionY()-pTaozhuang->getContentSize().height/2 - 20;

//			ccColor3B showColor = ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, false, true);

			//套装名字
			string strName = suitInfo.suit_name;
			CCLabelTTF *labelName = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, LABEL_SIZE, CCSizeMake(203, 0), kCCTextAlignmentCenter);
			base->addChild(labelName);
			//labelName->setAnchorPoint(ccp(0.5f, 0.5));
			labelName->setHorizontalAlignment(kCCTextAlignmentCenter);
			labelName->setPosition(ccp(123, start_y));

			start_y -= (labelName->getContentSize().height + 2);

			// 3 every one name
	//		int taozhuangNum = suitInfo.parts_of_suit.size(); //记录有多少件
			for (int i=0; i!=suitInfo.parts_of_suit.size(); ++i)
			{
				string strOneName = suitInfo.parts_of_suit.at(i).zhuangbei_name;
				string strTypeName = getPosName_ByZhuangBeiType(suitInfo.parts_of_suit.at(i).zhuanbei_type);
				CCLabelTTF *labelOneName = CCLabelTTF::create((strOneName + " (" + strTypeName + ")").c_str(), fontStr_kaiti, LABEL_SIZE-2);
				board->addChild(labelOneName);
				labelOneName->setAnchorPoint(ccp(0, 0.5));
				labelOneName->setPosition(ccp(pTaozhuang->getPositionX()-pTaozhuang->getContentSize().width/2, start_y));
//				labelOneName->setColor(showColor);

				start_y -= (labelOneName->getContentSize().height + 2);
			}

			labelName->setString(CCString::createWithFormat("%s", labelName->getString())->getCString());
		}

		int iTotal = suitInfo.parts_of_suit.size();

		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-12, (iTotal*LABEL_SIZE+Y-iTotal*label_gap)+pFengexian->getContentSize().height+LABEL_SIZE+20));
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-12, (iTotal*LABEL_SIZE+Y)+pFengexian->getContentSize().height+LABEL_SIZE+20));
#endif

		//标记
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, 
			pFengexian->getPositionY()-pFengexian->getContentSize().height/2-biaoji->getContentSize().height/2 - 5));

		//套装效果
		CCLabelTTF* pTaozhuangxiaoguo = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_ZhuangbeiXiaoguo").c_str(), fontStr_kaiti, LABEL_SIZE);
		pTaozhuangxiaoguo->setColor(LABEL_COLOR);
		base->addChild(pTaozhuangxiaoguo);
		pTaozhuangxiaoguo->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pTaozhuangxiaoguo->getContentSize().width/2, 
			biaoji->getPositionY()));

		label_y =  pTaozhuangxiaoguo->getPositionY()-pFengexian->getContentSize().height/2 - 25;

		//ccColor3B cqColor = ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi,false,false);
//		ccColor3B cqColor = fonColor_ChengGong;
		//穿在身上的能触发套装属性的套装件数、 groupId
//		vector<unsigned int> _vec;

		//当前详情界面的这件套装 有没有穿上、 没穿 则只有它的名字是白色、套装属性一个也没有
		//只穿这一件equipedGroupIdVec.size == 1  和  没有穿它equipedGroupIdVec.size == 0
//		int taozhuangNum = _vec.size(); //记录有多少件
//		label_y = 838-706+146;
// 		// 1 name
// 		CCString tempStr;
// 		tempStr.initWithFormat("%s(%s)", zbInfo.zhuangbei_name.c_str(), LFStrings::getValue("TaoZhuang").c_str());
// 		CCLabelTTF *labelName = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, LABEL_SIZE);
// 		base->addChild(labelName);
// 		labelName->setAnchorPoint(ccp(0, 0.5));
// 		labelName->setPosition(ccp(start_x, label_y));
// 		labelName->setColor(cqColor);
// 		label_y -= (labelName->getContentSize().height + label_gap);

		// 2 套装属性显示   (2)件
		char buf[32];
		CCPoint temppoint;
		for(int i=0; i<iTotal-1; ++i)
		{
			ccColor3B color;
//			if(i+2 <= taozhuangNum)
//				color = cqColor;
//			else 
				color = fonColor_CiYao;
			// 2、3、4、5件
			sprintf(buf, "(%d)", i+2);
			string strjian = buf + LFStrings::getValue("jian");
			CCLabelTTF *labelJian = CCLabelTTF::create(strjian.c_str(), fontStr_kaiti, LABEL_SIZE-2);
			base->addChild(labelJian);
			labelJian->setAnchorPoint(ccp(0, 0.5));
			if (i == 0)
			{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
				labelJian->setPosition(ccp(start_x, Y+(iTotal)*(labelJian->getContentSize().height) - 15));
#else
                labelJian->setPosition(ccp(start_x, Y+(iTotal)*(labelJian->getContentSize().height)));
#endif
			}
			else
			{
				labelJian->setPosition(ccp(temppoint.x, temppoint.y - labelJian->getContentSize().height - label_gap));
			}
			temppoint = labelJian->getPosition();
			
			labelJian->setColor(color);
			//属性
			for (int j=0; j<=1; ++j)
			{
				if (suitInfo.suit_attr[i][j].attr_type == 0)
					continue;

				CCLog("%s ---> [i=%d][j=%d]", __FUNCTION__, i, j);
				string strShuXing = GetMoFaShuXing_ByZBAttr(suitInfo.suit_attr[i][j]);
				CCLabelTTF *labelShuxing = CCLabelTTF::create(strShuXing.c_str(), fontStr_kaiti, LABEL_SIZE-2);
				base->addChild(labelShuxing);
				labelShuxing->setAnchorPoint(ccp(0, 0.5));
				if (j > 0) label_y -= (labelShuxing->getContentSize().height + label_gap);
				labelShuxing->setPosition(ccp(labelJian->getPositionX() + labelJian->getContentSize().width + 6, labelJian->getPositionY()));
				labelShuxing->setColor(color);
			}

			label_y -= (labelJian->getContentSize().height + label_gap);
		}
	}

}

string FX_CommonFunc::getAttrbuteItemString(AttrbuteItem& attr, int need_idx)
{
	char str[128];
	int cur_idx = 0;

	if(attr.kangxing_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.kangxing_min == attr.kangxing_max)
				sprintf(str, "+%d %s", attr.kangxing_min, LFStrings::getValue("moKang").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.kangxing_min, attr.kangxing_max, LFStrings::getValue("moKang").c_str());
			return str;
		}
	}
	if(attr.mingjie_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.mingjie_min == attr.mingjie_max)
				sprintf(str, "+%d %s", attr.mingjie_min, LFStrings::getValue("minjie").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.mingjie_min, attr.mingjie_max, LFStrings::getValue("minjie").c_str());
			return str;
		}
	}
	if(attr.zhili_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.zhili_min == attr.zhili_max)
				sprintf(str, "+%d %s", attr.zhili_min, LFStrings::getValue("zhili").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.zhili_min, attr.zhili_max, LFStrings::getValue("zhili").c_str());
			return str;
		}
	}
	if(attr.liliang_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.liliang_min == attr.liliang_max)
				sprintf(str, "+%d %s", attr.liliang_min, LFStrings::getValue("liliang").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.liliang_min, attr.liliang_max, LFStrings::getValue("liliang").c_str());
			return str;
		}
	}
	if(attr.tineng_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.tineng_min == attr.tineng_max)
				sprintf(str, "+%d %s", attr.tineng_min, LFStrings::getValue("tineng").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.tineng_min, attr.tineng_max, LFStrings::getValue("tineng").c_str());
			return str;
		}
	}
	if(attr.baoji_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.baoji_min == attr.baoji_max)
				sprintf(str, "+%d%% %s", attr.baoji_min, LFStrings::getValue("baojilv").c_str());
			else 
				sprintf(str, "+(%d-%d)%% %s", attr.baoji_min, attr.baoji_max, LFStrings::getValue("baojilv").c_str());
			return str;
		}
	}
	if(attr.baoshang_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.baoshang_min == attr.baoshang_max)
				sprintf(str, "+%d %s", attr.baoshang_min, LFStrings::getValue("baojishanghai").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.baoshang_min, attr.baoshang_max, LFStrings::getValue("baojishanghai").c_str());
			return str;
		}
	}
	if(attr.hp_tation_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.hp_tation_min == attr.hp_tation_max)
				sprintf(str, "+%d%% %s", attr.hp_tation_min, LFStrings::getValue("shengmingjiacheng").c_str());
			else 
				sprintf(str, "+(%d-%d)%% %s", attr.hp_tation_min, attr.hp_tation_max, LFStrings::getValue("shengmingjiacheng").c_str());
			return str;
		}
	}
	if(attr.gedanglv_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.gedanglv_min == attr.gedanglv_max)
				sprintf(str, "+%d%% %s", attr.gedanglv_min, LFStrings::getValue("gedanglv").c_str());
			else 
				sprintf(str, "+(%d-%d)%% %s", attr.gedanglv_min, attr.gedanglv_max, LFStrings::getValue("gedanglv").c_str());
			return str;
		}
	}
	if(attr.gedangzhi_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.gedangzhi_min == attr.gedangzhi_max)
				sprintf(str, "+%d %s", attr.gedangzhi_min, LFStrings::getValue("gedangzhi").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.gedangzhi_min, attr.gedangzhi_max, LFStrings::getValue("gedangzhi").c_str());
			return str;
		}
	}
	if(attr.gongji_min_add_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.gongji_min_add_min == attr.gongji_min_add_max)
				sprintf(str, "+%d %s", attr.gongji_min_add_min, LFStrings::getValue("zuixiaoshanghai").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.gongji_min_add_min, attr.gongji_min_add_max, LFStrings::getValue("zuixiaoshanghai").c_str());
			return str;
		}
	}
	if(attr.gongji_max_add_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.gongji_max_add_min == attr.gongji_max_add_max)
				sprintf(str, "+%d %s", attr.gongji_max_add_min, LFStrings::getValue("zuidashanghai").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.gongji_max_add_min, attr.gongji_max_add_max, LFStrings::getValue("zuidashanghai").c_str());
			return str;
		}
	}
	if(attr.hujia_add_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.hujia_add_min == attr.hujia_add_max)
				sprintf(str, "+%d %s", attr.hujia_add_min, LFStrings::getValue("huJia").c_str());
			else 
				sprintf(str, "+(%d-%d) %s", attr.hujia_add_min, attr.hujia_add_max, LFStrings::getValue("huJia").c_str());
			return str;
		}
	}
	if(attr.skill_zhouqi_sub_min != 0)// 减少几个技能周期
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.skill_zhouqi_sub_min == attr.skill_zhouqi_sub_max)
				sprintf(str, "%s%d%s", LFStrings::getValue("jiNengZhouQi").c_str(), attr.skill_zhouqi_sub_min, LFStrings::getValue("lun").c_str());
			else 
				sprintf(str, "%s(%d-%d)%s", LFStrings::getValue("jiNengZhouQi").c_str(), attr.skill_zhouqi_sub_min, attr.skill_zhouqi_sub_max, LFStrings::getValue("lun").c_str());
			return str; 
		}
	}
	if(attr.skill_turn_sub_min != 0)// 提前几个触发点
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.skill_turn_sub_min == attr.skill_turn_sub_max)
				sprintf(str, "%s%d%s", LFStrings::getValue("jiNengChuFaDian").c_str(), attr.skill_turn_sub_min, LFStrings::getValue("lun").c_str());
			else 
				sprintf(str, "%s(%d-%d)%s", LFStrings::getValue("jiNengChuFaDian").c_str(), attr.skill_turn_sub_min, attr.skill_turn_sub_max, LFStrings::getValue("lun").c_str());
			return str; 
		}
	}
	if(attr.skill_result_add_percent_min != 0)
	{
		if(cur_idx != need_idx) ++cur_idx;
		else
		{
			if(attr.skill_result_add_percent_min == attr.skill_result_add_percent_max)
				sprintf(str, "+%d%% %s", attr.skill_result_add_percent_min, LFStrings::getValue("jiNengXiaoGuoJiaCheng").c_str());
			else 
				sprintf(str, "+(%d-%d)%% %s", attr.skill_result_add_percent_min, attr.skill_result_add_percent_max, LFStrings::getValue("jiNengXiaoGuoJiaCheng").c_str());
			return str;
		}
	}

	CCAssert(false, "");
	return str;
}

//shuzhiLeft 增加的数值在名字左边
string FX_CommonFunc::GetMoFaShuXing_ByZBAttr(ZBAttr attr, bool isPercent)
{
	//label shuxing
	string shuxingStr;
	char buf[10] = "";

	switch (attr.attr_type)
	{
	case AttrType_mingjie:
		shuxingStr = LFStrings::getValue("minjie");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_liliang:
		shuxingStr = LFStrings::getValue("liliang");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_zhili:
		shuxingStr = LFStrings::getValue("zhili");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_tineng:
		shuxingStr = LFStrings::getValue("tineng");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_gongji_min:
		shuxingStr = LFStrings::getValue("zuixiaoshanghai");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_gongji_max:
		shuxingStr = LFStrings::getValue("zuidashanghai");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_baoji:
		shuxingStr = LFStrings::getValue("baojilv");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	case AttrType_baoshang:
		shuxingStr = LFStrings::getValue("baojishanghai");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
		//	case AttrType_skillratio: 
		// 		shuxingStr = LFStrings::getValue("jinengchufalv");
		// 		sprintf(buf, "+%d%%", attr.attr_value);
		// 		break;
	case AttrType_hujia_add:
		shuxingStr = LFStrings::getValue("huJia");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_hp_percent:
		shuxingStr = LFStrings::getValue("shengmingjiacheng");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	case AttrType_kangxing:
		shuxingStr = LFStrings::getValue("moKang");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_gedanglv:
		shuxingStr = LFStrings::getValue("geDangLv");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	case AttrType_gedangzhi:
		shuxingStr = LFStrings::getValue("geDangZhi");
		if (isPercent) sprintf(buf, "+%d%%", attr.attr_value);
		else           sprintf(buf, "+%d", attr.attr_value);
		break;
	case AttrType_skillzhouqi:
		shuxingStr = LFStrings::getValue("jiNengZhouQi");
		sprintf(buf, "%d%s", attr.attr_value, LFStrings::getValue("lun").c_str());
		shuxingStr += buf;
		buf[0] = 0;
		break;
	case AttrType_skillturn:
		shuxingStr = LFStrings::getValue("jiNengChuFaDian");
		sprintf(buf, "%d%s", attr.attr_value, LFStrings::getValue("lun").c_str());
		shuxingStr += buf;
		buf[0] = 0;
		break;
	case AttrType_skillresult:
		shuxingStr = LFStrings::getValue("jiNengXiaoGuoJiaCheng");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	case AttrType_actor_gongji_addratio:// = 18,	// 角色整体攻击加成
		shuxingStr = LFStrings::getValue("gongJiJiaCheng");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	case AttrType_actor_hp_addratio:// = 19,	// 角色整体血量加成
		shuxingStr = LFStrings::getValue("xueLiangJiaCheng");
		sprintf(buf, "+%d%%", attr.attr_value);
		break;
	default:
		CCAssert(false, "default type");
		break;
	}

	shuxingStr = buf + shuxingStr;


	// 精炼百分比，如果是攻击，统一返回最小攻击
	if (isPercent && attr.attr_type == AttrType_gongji_min) 
	{
		shuxingStr = LFStrings::getValue("gongJi");
		sprintf(buf, "+%d%%", attr.attr_value);
		shuxingStr = buf + shuxingStr;
	}

	return shuxingStr;
}


string FX_CommonFunc::getPosName_ByZhuangBeiType(ZhuangbeiType type)
{
	string name;

	switch (type)
	{
	case ZhuangbeiType_DanshouJian:
	case ZhuangbeiType_ShuangshouBishou:
	case ZhuangbeiType_ShuangshouFu:
	case ZhuangbeiType_DanshouFazhang:
	case ZhuangbeiType_Gong:
	case ZhuangbeiType_ShuangshouFazhang:
		name = LFStrings::getValue("wuqi");
		break;
	case ZhuangbeiType_Faqi:
	case ZhuangbeiType_Dunpai:
		name = LFStrings::getValue("fushou");
		break;
	case ZhuangbeiType_Hushenfu:
		name = LFStrings::getValue("hushenfu");
		break;
	case ZhuangbeiType_KuijiaZhong:
	case ZhuangbeiType_KuijiaQin:
		name = LFStrings::getValue("xiongjia");
		break;
	case ZhuangbeiType_Jiezhi:
		name = LFStrings::getValue("jiezhi");
		break;
	default:
		break;
	}

	return name;
}

int FX_CommonFunc::getJiaoTangFileIdxByLevel(unsigned int lv)
{
	return (lv + 1)/2;
}

void FX_CommonFunc::sortQianghuaDataAtHuiTui(QianghuaData &qianghuaData)
{
	//QianghuaLevel_jin_special  == 5


	for(int i=0; i<qianghuaData.qianghua_count; ++i)
	{
		for(int j=i; j!=qianghuaData.qianghua_count; ++j)
		{
			if(  (qianghuaData.info_arr[i].level < qianghuaData.info_arr[j].level)   
				|| (qianghuaData.info_arr[i].level == qianghuaData.info_arr[j].level && 
				qianghuaData.info_arr[i].percent < qianghuaData.info_arr[j].percent)  )
			{
				swap(qianghuaData.info_arr[i], qianghuaData.info_arr[j]);
			}
		}
	}

/*	//把前4个是 QianghuaLevel_jin_special 的   换到后面去、
	int changePos = 4;
	for (int i=0; i<qianghuaData.qianghua_count && i<4 && changePos<qianghuaData.qianghua_count; ++i)
	{
		//前4个里面有 熔金石锻造出的金星
		if (qianghuaData.info_arr[i].level == QianghuaLevel_jin_special)
		{
			//从第五个位置 idx==4  开始找一个不是熔金石锻造过的 位置、 直到找到或越界
			while (qianghuaData.info_arr[changePos].level != QianghuaLevel_jin_special && changePos<qianghuaData.qianghua_count)
			{
				++changePos;
			}
			//找到位置 --> 是可用的位置 ->没有越界
			if (changePos < qianghuaData.qianghua_count)
				swap(qianghuaData.info_arr[i], qianghuaData.info_arr[changePos]);
		}
	}*/
}


//该node执行action，所有的child也执行action
void FX_CommonFunc::nodeAndAllChildRunAction(CCNode* node, CCAction* action)
{
	if(node == NULL || action == NULL)
		return;

	//自己
	node->runAction(action);
	//childe
	CCArray* childs = node->getChildren();
	if (childs != NULL)
	{
		for (int i=0; i!=childs->count(); ++i)
		{
			nodeAndAllChildRunAction((CCNode*)childs->objectAtIndex(i), (CCAction*)action->copy()->autorelease());
		}
	}
}

#define STATE_NO                        0
#define STATE_ExcepteNodeAndChilds		1
#define STATE_ExcepteNodeWithoutChilds  2
#define STATE_ExcepteWhoesChilds		3
void FX_CommonFunc::nodeAndAllChildRunAction(CCNode* node, CCAction* action, 
		CCArray* exceptNodeAndChilds, CCArray* exceptNodeWithoutChilds, CCArray* exceptWhoesChilds)
		//状态  0  1 2 3
{
	if(node == NULL || action == NULL)
		return;

	int state = 0; 
	if (exceptNodeAndChilds != NULL){
		for (int i=0,count=exceptNodeAndChilds->count(); i<count; ++i)
		{
			CCNode *one = dynamic_cast<CCNode*>(exceptNodeAndChilds->objectAtIndex(i));
			if (node == one){
				state = STATE_ExcepteNodeAndChilds;
				break;
			}
		}
	}
	if (state == 0 && exceptNodeWithoutChilds != NULL){
		for (int i=0,count=exceptNodeWithoutChilds->count(); i<count; ++i)
		{
			CCNode *one = dynamic_cast<CCNode*>(exceptNodeWithoutChilds->objectAtIndex(i));
			if (node == one){
				state = STATE_ExcepteNodeWithoutChilds;
				break;
			}
		}
	}
	if (state == 0 && exceptWhoesChilds != NULL){
		for (int i=0,count=exceptWhoesChilds->count(); i<count; ++i)
		{
			CCNode *one = dynamic_cast<CCNode*>(exceptWhoesChilds->objectAtIndex(i));
			if (node == one){
				state = STATE_ExcepteWhoesChilds;
				break;
			}
		}
	}

	//self
	if (state == STATE_ExcepteWhoesChilds || state == STATE_NO){
		node->runAction(action);
	}

	//child
	if (state == STATE_NO || state == STATE_ExcepteNodeWithoutChilds){
		CCArray* childs = node->getChildren();
		if (childs != NULL){
			for (int i=0; i!=childs->count(); ++i)
			{
				nodeAndAllChildRunAction(dynamic_cast<CCNode*>(childs->objectAtIndex(i)), (CCAction*)action->copy()->autorelease(),
					exceptNodeAndChilds, exceptNodeWithoutChilds, exceptWhoesChilds);
			}
		}
	}
}

// colorOrOpacity : 1/color   2/opacity   3/both
void FX_CommonFunc::setAllChildrenColorOrOpacity(CCNode* node, ccColor3B color, GLubyte opacity, int colorOrOpacity)
{
	if (node == NULL) return;

	CCArray *children = node->getChildren();
	if( children != NULL )
		for (int i=0; i!=children->count(); ++i)
		{
			CCNode *node = dynamic_cast<CCNode*>(children->objectAtIndex(i));
			setAllChildrenColorOrOpacity(node, color, opacity, colorOrOpacity);
		}

	CCSprite *child = dynamic_cast<CCSprite*>(node);
	if (child)
	{
		if(colorOrOpacity == 1)
			child->setColor(color);
		else if(colorOrOpacity == 2)
			child->setOpacity(opacity);
		else if(colorOrOpacity == 3)
		{
			child->setColor(color);
			child->setOpacity(opacity);
		}

	}

}


//材料颜色获取
const ccColor3B& FX_CommonFunc::getCaiLiaoColor(unsigned int itemType){
	switch (itemType){
	case ItemsType_QianghuaNormal:
	case ItemsType_Silver:
		return ccWHITE;
	case ItemsType_Dalishi:
	case ItemsType_Huagangyan:
	case ItemsType_Hero_LevelUp:
	case ItemsType_Hero_Upgrade:
	case ItemsType_Mucai:
	case ItemsType_Shuijingshi:
		return ccc3(79,110,217);
	case ItemsType_DaiGouJuan:
	case ItemsType_Shenbing_Jianding:
		return ccc3(253,180,40);
	case ItemsType_GoldStar_Qianghua:
		return ccc3(181,93,6);
	}
	//返回宝石 和 道具的颜色
	return ccc3(255,240,188);
}