#include "ImageManager.h"
#include "BaseActorPropertyParser.h"
#include "Equipment.h"
#include "ZhuangBeiMianBanBase.h"
#include "MyselfManager.h"
#include "CS_Common.h"
#include "font.h"
#include "SuitPool.h"
#include "ItemInformationHelper.h"
#include "SystemGiftBagManager.h"
#include "ShenBingPool.h"

ImageManager* ImageManager::manager = new ImageManager();

ImageManager::ImageManager(void)
{
}
ImageManager* ImageManager::getManager()
{
	return manager;
}

bool ImageManager::initManager()
{
	return true;
}

ImageData ImageManager::getImageData_ProfessionHead(Profession actorType,bool doubleHande /* = false */)
{
	string handString = "";
	if (actorType == Profession_Yemanren || actorType == Profession_Chike || actorType == Profession_Fashi)
	{
		handString = doubleHande?"_shuangshou":"_danshou";
	}

	ImageData data;
	// 目前就直接使用这个 panel_people_ + 基础名字+.png
	data.baseImageName = getBaseImageName(actorType);
	data.currentImageName = "touxiang_" + data.baseImageName+handString;
	return data;
}

ImageData ImageManager::getImageData_JueSheQuanShen(Profession actorType)
{
	ImageData data;
	// 目前就直接使用这个 panel_people_ + 基础名字+.png
	data.baseImageName = getBaseImageName(actorType);
	data.currentImageName = "chuangjian_renwu_" + data.baseImageName;
	return data;
}

string ImageManager::getBaseImageName(Profession actorType)
{
	return BaseActorPropertyParser::getParser()->getExtraProerty(actorType).baseName;
}

//string ImageManager::getDiWenBaseImageName(ZhuangbeiColour zb_color)
//{
//	return getEquipColorName(zb_color);
//}

string ImageManager::getEquipColorName(ZhuangbeiColour zb_color, bool is_shenbing,bool is_suit /* = false */)
{
	string ret = "";
// 	if (is_shenbing && is_suit)
// 	{
// 		ret = "shenbing_taozhuang";
// 		return ret;
// 	}
// 	else if (is_shenbing)
// 	{
// 		ret = "shenbing";
// 		return ret;
// 	}
// 	else if (is_suit)
// 	{
// 		ret = "taozhuang";
// 		return ret;
// 	}

	switch (zb_color)
	{
	case ZhuangbeiColour_Baizhuang:
		ret = "putong";
		break;
	case ZhuangbeiColour_Fumo:
		ret = "mofa";
		break;
	case ZhuangbeiColour_Xiyou:
		ret = "xiyou";
		break;
	case ZhuangbeiColour_Chuanqi:
		ret = "chuanqi";
		break;
	default:
		CCLog("Waring : %s--> unkown zb_color = [%d],is_suit = [%s]",__FUNCTION__,zb_color,is_suit?"true":"false");
		CCAssert(false,"");
		break;
	}

	return ret;
}

ccColor3B ImageManager::getShowColor(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit /* = false */)
{
	ccColor3B color;

	if (is_shenbing && is_suit)
	{
		color.r = 177;
		color.g = 54;
		color.b = 207;

		return color;
	}
	else if (is_shenbing)
	{
		color.r = 203;
		color.g = 40;
		color.b = 16;

		return color;
	}
// 	if (is_suit)
// 	{
// 		color.r = 123;
// 		color.g = 200;
// 		color.b = 28;
// 
// 		return color;
// 	}


	switch(zb_color)
	{
	case ZhuangbeiColour_Baizhuang:
		color = ccWHITE;
		break;
	case ZhuangbeiColour_Fumo:
		color.r = 79;
		color.g = 110;
		color.b = 217;
		break;
	case ZhuangbeiColour_Xiyou:
		color.r = 253;
		color.g = 180;
		color.b = 40;
		break;
	case ZhuangbeiColour_Chuanqi:
		color.r = 181;
		color.g = 93;
		color.b = 6;
		break;
	}
	return color;
}

ImageData ImageManager::getImageData_DiWen_XiangQing(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit,int zhuanKe)
{
	// zhuangbei_xiangxi_diwen_mofa.png
	ImageData data;
	data.baseImageName = getEquipColorName(zb_color,is_shenbing,is_suit);
	data.currentImageName = "zhuangbei_xiangxi_diwen_" + data.baseImageName;

// 	if (hasZhuanKe)
// 	{
// 		if (is_shenbing && is_suit)
// 			data.currentImageName = "zhuangbei_xiangxi_diwen_shenbing_taozhuang_jingwen";
// 		else if (is_shenbing)
// 			data.currentImageName = "zhuangbei_xiangxi_diwen_shenbing_jingwen";
// 	}

// 	if (zhuanKe == 1)
// 		data.currentImageName = "zhuangbei_xiangxi_jingwen_shenbing_diwen";
// 	else 
// 		data.currentImageName = "zhuangbei_xiangxi_diwen_shenbing_taozhuang_jingwen";


	return data;
}


ImageData ImageManager::getImageData_DiWen_ZhuangBeiMianBan(int boxTag,ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit,int zhuanKe)
{
	// wuqi_diwen_fushou_mofa.png
	ImageData data;
//	if (zhuanKe == 0)
	if (true)
	{
		switch (boxTag)
		{
		case TAG_HuShenFu:
			data.baseImageName = "hushenfu_" + getEquipColorName(zb_color,is_shenbing,is_suit);
			break;
		case TAG_Kuijia:
			data.baseImageName = "xiongjia_" + getEquipColorName(zb_color,is_shenbing,is_suit);
			break;
		case TAG_JieZhi:
			data.baseImageName = "jiezhi1_" + getEquipColorName(zb_color,is_shenbing,is_suit);
			break;
		case TAG_WuQi:
			data.baseImageName = "wuqi_" + getEquipColorName(zb_color,is_shenbing,is_suit);
			break;
		case TAG_DunPai:
			data.baseImageName = "fushou_" + getEquipColorName(zb_color,is_shenbing,is_suit);
			break;
		default:
			CCLog("Waring : %s--> unkown zb_color = [%d],is_suit = [%s],boxTag = [%d]",__FUNCTION__,zb_color,is_suit?"true":"false",boxTag);
			CCAssert(false,"不支持！");
			break;
		}
		data.currentImageName = "wuqi_diwen_" + data.baseImageName;
	}
	else
	{
		switch (boxTag)
		{
		case TAG_HuShenFu:
			data.baseImageName = "wuqi_diwen_jiezhi";
			break;
		case TAG_Kuijia:
			data.baseImageName = "wuqi_diwen_xiongjia";
			break;
		case TAG_JieZhi:
			data.baseImageName = "wuqi_diwen_jiezhi";//_shenbing_taozhuang_jingwen
			break;
		case TAG_WuQi:
			data.baseImageName = "wuqi_diwen_fushou";//_shenbing_jingwen
			break;
		case TAG_DunPai:
			data.baseImageName = "wuqi_diwen_fushou";
			break;
		default:
			CCLog("Waring : %s--> unkown zb_color = [%d],is_suit = [%s],boxTag = [%d]",__FUNCTION__,zb_color,is_suit?"true":"false",boxTag);
			CCAssert(false,"不支持！");
			break;
		}
// 		if (is_shenbing && is_suit)
// 			data.currentImageName = data.baseImageName + "_shenbing_taozhuang_jingwen";
// 		else if (is_shenbing)
// 			data.currentImageName = data.baseImageName + "_shenbing_jingwen";
		if (zhuanKe == 1)
			data.currentImageName = data.baseImageName + "_shenbing_jingwen";
		else if (zhuanKe == 2)
			data.currentImageName = data.baseImageName + "_shenbing_taozhuang_jingwen";
	}
	return data;
}

string ImageManager::getImageData_DiWen_WithBianKuang(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit)
{
// 	if (is_suit && is_shenbing)
// 		return "zhandou_jiesuan_zhuangbei_shenbing_taozhuang.png";
// 	else if(is_shenbing)
// 		return "zhandou_jiesuan_zhuangbei_shenbing.png";
// 	else if (is_suit)
// 		return "zhandou_jiesuan_zhuangbei_chuanqi_taozhuang.png";

	switch (zb_color)
	{
	case ZhuangbeiColour_Baizhuang:
		return "zhandou_jiesuan_zhuangbei_putong.png";
		break;
	case ZhuangbeiColour_Fumo:
		return "zhandou_jiesuan_zhuangbei_mofa.png";
		break;
	case ZhuangbeiColour_Xiyou:
		return "zhandou_jiesuan_zhuangbei_xiyou.png";
		break;
	case ZhuangbeiColour_Chuanqi:
		return "zhandou_jiesuan_zhuangbei_chuanqi.png";
		break;
	default:
		return "";
		CCLog("Error equip color");
		break;
	}
}

ImageData ImageManager::getImageData_DiWen_Selected(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit)
{
	//mianban_zhuangbei_diwen_xiyou_select.png
	ImageData data;
	data.baseImageName = getEquipColorName(zb_color, is_shenbing ,is_suit);
	data.currentImageName = "zhuangbei_xiangxi_diwen_" + data.baseImageName +"_select";
	return data;
}

ImageData ImageManager::getImageData_DiWen_Selected_ZhuangBeiMianBan(int boxTag,ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit)
{
	//wuqi_diwen_jiezhi1_shenbing_taozhuang_select.png
	ImageData data;
	switch (boxTag)
	{
	case TAG_Kuijia:
		data.baseImageName = "xiongjia_" + getEquipColorName(zb_color,is_shenbing,is_suit);
		break;
	case TAG_HuShenFu:
	case TAG_JieZhi:
		data.baseImageName = "jiezhi1_" + getEquipColorName(zb_color,is_shenbing,is_suit);
		break;
	case TAG_WuQi:
	case TAG_DunPai:
		data.baseImageName = "wuqi_" + getEquipColorName(zb_color,is_shenbing,is_suit);
		break;
	default:
		CCLog("Waring : %s--> unkown zb_color = [%d],is_suit = [%s],boxTag = [%d]",__FUNCTION__,zb_color,is_suit?"true":"false",boxTag);
		CCAssert(false,"不支持！");
		break;
	}
	data.currentImageName = "wuqi_diwen_" + data.baseImageName +"_select";

	return data;
}

ImageData ImageManager::getImageData_DiWen_List(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit,int zhuanKe)
{
	//mianban_zhuangbei_diwen_xiyou.png
	ImageData data;
	data.baseImageName = getEquipColorName(zb_color, is_shenbing, is_suit);
	data.currentImageName = "zhuangbei_xiangxi_diwen_" + data.baseImageName;
	data.scale = 124/297.0f;

// 	if (hasZhuanKe)
// 	{
// 		if (is_shenbing && is_suit)
// 			data.currentImageName = "zhuangbei_xiangxi_diwen_shenbing_taozhuang_jingwen";
// 		else if (is_shenbing)
// 			data.currentImageName = "zhuangbei_xiangxi_jingwen_shenbing_diwen";
// 	}

// 	if (zhuanKe == 1)
// 		data.currentImageName = "zhuangbei_xiangxi_jingwen_shenbing_diwen";
// 	else 
// 		data.currentImageName = "zhuangbei_xiangxi_diwen_shenbing_taozhuang_jingwen";

	return data;
}

string ImageManager::getEquipImageFile(string equipKeyWords)
{
	return "minute_" + equipKeyWords +".png";
}

ImageData ImageManager::getImageData_ZhuangBei_MianBan(Equipment* equip)
{
	ImageData data;
	// equipped_weapon_artifact_1.png
	data.baseImageName = equip->getPicFile();
	data.currentImageName = "minute_" + data.baseImageName;

	switch (equip->getType())
	{
	case ZhuangbeiType_DanshouJian:
	case ZhuangbeiType_ShuangshouBishou:	// ZhuangbeiType_ShuangshouMao -> 改成双手匕首
	case ZhuangbeiType_ShuangshouFu:
	case ZhuangbeiType_DanshouFazhang:
	case ZhuangbeiType_Gong:
	case ZhuangbeiType_ShuangshouFazhang :
		data.scale = 0.44f;
		break;

	case ZhuangbeiType_Faqi:
	case ZhuangbeiType_Dunpai:
		data.scale = 0.48f;
		break;

	case ZhuangbeiType_Hushenfu:
		data.scale = 0.38f;
		break;

	case ZhuangbeiType_KuijiaZhong:
	case ZhuangbeiType_KuijiaQin:
		data.scale = 0.38f;
		break;

	case ZhuangbeiType_Jiezhi:
		data.scale = 0.38f;
		break;
	default:
		break;
	}
	return data;
}

ImageData ImageManager::getImageData_ZhuangBei_List(Equipment* equip)
{
	ImageData data;
	// tabulation_weapon_artifact_1.png
	data.baseImageName = equip->getPicFile();
	data.currentImageName = "minute_" + data.baseImageName;
	data.scale = 0.43f;
	return data;
}
// 
ImageData ImageManager::getImageData_ZhuangBei_XiangQing(Equipment* equip)
{
	ImageData data;
	// minute_armor_boots_1.png
	data.baseImageName = equip->getPicFile();
	data.currentImageName = "minute_" + data.baseImageName;
	return data;
}

ImageData ImageManager::getImageData_ZhuangBei_DuanzaoAdd(Equipment* equip)
{
	ImageData data;
	// minute_armor_boots_1.png
	data.baseImageName = equip->getPicFile();
	data.currentImageName = "minute_" + data.baseImageName;
	data.scale = 0.37f;
	return data;
}

ImageData ImageManager::getImageData_ZhuangBei_DuanzaoSuc(Equipment* equip)
{
	ImageData data;
	// minute_armor_boots_1.png
	data.baseImageName = equip->getPicFile();
	data.currentImageName = "minute_" + data.baseImageName;
	//data.scale = 0.25f;
	data.scale = 114.0f/277.0f;
	return data;
}

ImageData ImageManager::getImageData_ZhuangBei_DiaoLuo(EquipData equip)
{
	ImageData data;
	// 去掉后面的"_4"这种数字部分
	data.baseImageName = equip.equipPic.substr(0,equip.equipPic.length()-2);
	// 把双手，单手都用双手
	data.baseImageName = LF::replace_all_distinct(data.baseImageName,"_ds","_ss");
	// 轻和重都用轻
	data.baseImageName = LF::replace_all_distinct(data.baseImageName,"_q","_z");

	data.currentImageName = "zhandou_" + data.baseImageName;
	//data.scale = 0.25f;
	return data;
}

BaseSprite* ImageManager::getSpriteOfZhuangbei(Equipment* equip,bool showJingLian/* = false */,bool showQianghua /* = false */)
{
	return getSpriteOfZhuangbei(equip->getEquipData(),showJingLian,showQianghua);
}

BaseSprite* ImageManager::getSpriteOfZhuangbei(EquipData equip,bool showJingLian/* = false */,bool showQianghua /* = false */)
{
	std::vector<ItemsType> baoshi_vec;
	if(equip.diamond_type_1 > 0)
	{
		baoshi_vec.push_back((ItemsType)equip.diamond_type_1);
	}
	if(equip.diamond_type_2 > 0)
	{
		baoshi_vec.push_back((ItemsType)equip.diamond_type_2);
	}
	if(equip.diamond_type_3 > 0)
	{
		baoshi_vec.push_back((ItemsType)equip.diamond_type_3);
	}

	QianghuaData qinghua = equip.qianghua_data;
	int jinglian_times = equip.jinglian_times;
	if (!showJingLian)
		jinglian_times = 0;
	if(!showQianghua)
	{
		qinghua.qianghua_count = 0;
		qinghua.max_qianghua_time = 0;
	}
		
	ZhuangbeiColourExt color = (ZhuangbeiColourExt)equip.zhuangbei_colour;
	if (SuitPool::getPool()->isSuit(equip.group_id))
	{
		color = ZhuangbeiColourExt_ChuanQiTaoZhuang;
	}
	if (equip.isShenBing && !SuitPool::getPool()->isSuit(equip.group_id))
	{
		color = ZhuangbeiColourExt_ShengBing;
	}
	if (equip.isShenBing && SuitPool::getPool()->isSuit(equip.group_id))
	{
		color = ZhuangbeiColourExt_ShengBingTaoZhuang;
	}

	if (equip.zhuangbei_type == ZhuangbeiType_Dunpai)
	{
		qinghua.qianghua_count = 0;
		qinghua.max_qianghua_time = 0;
	}
	return getSpriteOfZhuangbei(equip.equipPic,color,equip.hole,baoshi_vec,qinghua,jinglian_times);
}

BaseSprite* ImageManager::getSpriteOfZhuangbei(string equipKeyWords,ZhuangbeiColourExt color,unsigned holdNum,vector<ItemsType> diamonds,QianghuaData qinghua,unsigned int jingLianTime /* = 0 */)
{
	std::string pic_file = "minute_" + equipKeyWords + ".png";

	BaseSprite* sprite = BaseSprite::create(pic_file);

	BaseSprite* hole1 = BaseSprite::create();
	BaseSprite* hole2 = BaseSprite::create();
	BaseSprite* hole3 = BaseSprite::create();

	// 精炼
	{
		if (jingLianTime > 0)
		{
			//BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
			BaseSprite* icon = BaseSprite::create();
			icon->setContentSize(CCSizeMake(20,50));
			char temp[16];
			sprintf(temp,"+%u",jingLianTime);
			CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,40,CCSizeMake(0,0),kCCTextAlignmentRight);
			lableNum->setAnchorPoint(ccp(1.0f,0.5f));

			//{
			//	ccColor3B lableColor;
			//	switch (color)
			//	{
			//	case ImageManager::ZhuangbeiColourExt_Baizhuang:
			//		//lableColor = get
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_Fumo:
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_Xiyou:
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_Chuanqi:
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_ChuanQiTaoZhuang:
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_ShengBing:
			//		break;
			//	case ImageManager::ZhuangbeiColourExt_ShengBingTaoZhuang:
			//		break;
			//	default:
			//		break;
			//	}
			//	lableNum->setColor(lableColor);
			//}

			
			sprite->addChild(lableNum);
			sprite->addChild(icon);
			icon->setPositionY(sprite->getContentSize().height - icon->getContentSize().height/2);
			lableNum->setPosition(ccp(sprite->getContentSize().width - lableNum->getContentSize().width/2,icon->getPositionY()));
			icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
		}
	}

	// 强化
	{

		//sort
		for(int i=0; i<qinghua.qianghua_count; ++i)
		{
			for(int j=i; j<qinghua.qianghua_count; ++j)
			{
				if(  (qinghua.info_arr[i].level < qinghua.info_arr[j].level)   
					|| (qinghua.info_arr[i].level == qinghua.info_arr[j].level && 
					qinghua.info_arr[i].percent < qinghua.info_arr[j].percent)  )
				{
					swap(qinghua.info_arr[i], qinghua.info_arr[j]);
				}
			}
		}

		//float scale = 0.32f/(124/297.0f);
		float scale = 0.3f/(124/297.0f);
		BaseSprite* temp = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
		float posX = temp->getContentSize().width/2 * scale;
		float posY = temp->getContentSize().height/2;

		for (int i=0;i<qinghua.max_qianghua_time;i++)
		{
			BaseSprite* imageStar = NULL;
			if (i < qinghua.qianghua_count)
			{
				switch (qinghua.info_arr[i].level)
				{
				case 4:
				case QianghuaLevel_jin_special:
					imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
					break;
				case 3:
					imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_yin.png");
					break;
				case 2:
					imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
					break;
				case 1:
					imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_tie.png");
					break;
				default:
					CCLog("Error : %s --> qiangjhua_level = %d",__FUNCTION__,qinghua.info_arr[i].level);
					imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_tie.png");
					CCAssert(false,"未知的level");
				}
			}
			else
			{
				imageStar = BaseSprite::create("gongjiang_qianghua_jieguo_kong.png");
			}


			sprite->addChild(imageStar);
			imageStar->setPosition(ccp(posX,posY));
			imageStar->setScale(scale);
			posX += temp->getContentSize().width * imageStar->getScaleX() - 8;
		}
	}

	if(holdNum == 1)
	{
		hole1 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole1);
		hole1->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height/2));
	}
	else if(holdNum == 2)
	{
		hole1 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole1);
		hole1->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height*3.3f/5));

		hole2 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole2);
		hole2->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height*1.7f/5));
	}
	else if(holdNum == 3)
	{
		hole1 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole1);
		hole1->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height*3.5f/5));

		hole2 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole2);
		hole2->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height* 1.0f/2.0f));

		hole3 = BaseSprite::create("zhuangbei_kong.png");
		sprite->addChild(hole3);
		hole3->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height*1.5f/5));
	}

	hole1->setScale(0.7f);
	hole2->setScale(0.7f);
	hole3->setScale(0.7f);

	if(diamonds.size() == 1)
	{
		if(hole1 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[0]));
			hole1->addChild(baoshi);
			baoshi->setPosition(ccp(hole1->getContentSize().width/2, hole1->getContentSize().height/2));
		}
	}
	else if(diamonds.size() == 2)
	{
		if(hole1 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[0]));
			hole1->addChild(baoshi);
			baoshi->setPosition(ccp(hole1->getContentSize().width/2, hole1->getContentSize().height/2));
		}
		if(hole2 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[1]));
			hole2->addChild(baoshi);
			baoshi->setPosition(ccp(hole2->getContentSize().width/2, hole2->getContentSize().height/2));
		}
	}
	else if(diamonds.size() == 3)
	{
		if(hole1 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[0]));
			hole1->addChild(baoshi);
			baoshi->setPosition(ccp(hole1->getContentSize().width/2, hole1->getContentSize().height/2));
		}
		if(hole2 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[1]));
			hole2->addChild(baoshi);
			baoshi->setPosition(ccp(hole2->getContentSize().width/2, hole2->getContentSize().height/2));
		}
		if(hole3 != NULL)
		{
			BaseSprite* baoshi = BaseSprite::create(getBaoshiFilename(diamonds[2]));
			hole3->addChild(baoshi);
			baoshi->setPosition(ccp(hole3->getContentSize().width/2, hole3->getContentSize().height/2));
		}
	}

	return sprite;
}

//
CCNode* ImageManager::getFullPhotoOfActor(BaseActor* actor,bool useDiZuo /* = true */)
{
	// Equipment转ZhuangbeiSummary
	vector<ZhuangbeiSummary> equips_summary;
	vector<Equipment*> equips = actor->getEquipments();
	int equipNum = equips.size();
	for (int i=0;i<equipNum;i++)
	{
		ZhuangbeiSummary summary;
		summary.zhuangbei_colour = equips[i]->getEquipColor();
		summary.zhuangbei_type = equips[i]->getType();
		strcpy(summary.zhuangbei_pic,equips[i]->getEquipData().equipPic.c_str());

		equips_summary.push_back(summary);
	}

	return getFullPhotoOfActor(actor->getActorType(),equips_summary,actor->isYuanShen(),useDiZuo,actor->getCurrentProperty().currentStage);
}

// 需要的架子图片：
// 战斗内的架子：qsx_[关键字]_jiazhi_[_双手].png; // 这是空架子 qsx_[关键字]_yuanshen_[_双手].png;
// 如果是有盔甲，那么直接用盔甲的图片作为架子
// qsx：全身相.
// 比如 qsx_barbarian_jiazhi_shuangshou.png/qsx_barbarian_yuanshen_shuangshou.png
// 比如 qsx_barbarian_jiazhi.png/qsx_barbarian_yuanshen.png
// 比如 qsx_barbarian_jiazhi.png/qsx_barbarian_yuanshen.png
// 没有装备的情况，就用默认的单双手.
// 怪物统一使用单手命名的图片，不管其到底是单双手
CCNode* ImageManager::getFullPhotoOfActor(Profession profession,vector<ZhuangbeiSummary> equips,bool yuanshen /* = false */,bool useDiZuo /* = true */,int stage /* = 1 */)
{
#define _JIAZHI			"_jiazhi"
#define _YUANSHEN		"_yuanshen"
	// 1.确定单双手,盔甲的位置索引
	int kuijia_pos = -1;
	bool shuangshou = false;
	int equipNum = equips.size();
	for (int i=0;i<equipNum;i++)
	{
		if ( CS::isShuangShouWuQi(equips[i].zhuangbei_type) )
		{
			shuangshou = true;
		}
		if (equips[i].zhuangbei_type == ZhuangbeiType_KuijiaQin || equips[i].zhuangbei_type == ZhuangbeiType_KuijiaZhong)
		{
			CCAssert(kuijia_pos == -1,"");
			kuijia_pos = i;
		}
	}

	// 断言 怪物, 全用单手,不会有盔甲
	if (profession > Profession_GW_Start && profession < Profession_GW_End || profession>=Profession_Big_Boss_1)
	{// 知己用zhandou_guai_****.png图片

		CCAssert(equipNum<=0,"怪物不可能有装备");
		CCAssert(shuangshou == false,"怪物无装备");
		CCAssert(kuijia_pos == -1,"怪物无盔甲");

		CCNode* node = CCNode::create();
		// zhandou_guai_jiangshi_dizuo.png
		CCNode* dizuo = BaseSprite::create("qsx_dizuo.png");
		// zhandou_guai_jiangshi.png
		BaseSprite* jiazi_node = BaseSprite::create("zhandou_guai_"+getBaseImageName(profession)+".png");
		node->addChild(dizuo,0,TAG_FULL_PHOTO_DIZUO);
		node->addChild(jiazi_node,0,TAG_FULL_PHOTO_JIAZI);
		dizuo->setPosition(ccp(0,-jiazi_node->getContentSize().height/2+dizuo->getContentSize().height/2));
		return node;
	}
	// 其他的助手，那么该双手就双手，该单手，就单手
	switch (profession)
	{
	case Profession_Yemanren:
		break;
	case Profession_Chike:
		break;
	case Profession_Fashi:
		break;
	case Profession_Qishi:
		shuangshou = false;
		break;
	case Profession_ShenJianShou:
		shuangshou = true;
		break;
	case Profession_MiShu:
		shuangshou = true;
		break;
	case Profession_WuSeng:
		shuangshou = false;
		break;
	case Profession_YouXia:
		shuangshou = false;
		break;
	case Profession_LieShou:
		shuangshou = true;
		break;
	case Profession_WuShi:
		shuangshou = true;
		break;
	default:
		CCLog("Waring : %s--> unkown profession = [%d]",__FUNCTION__,profession);
		break;
	}


	// 元神的话就直接不用装备的图片
	if (yuanshen)
	{
		kuijia_pos = -1;
		equipNum = 0;
	}

	// 2.创建架子
	BaseSprite* jiazi_node = NULL;
	if (kuijia_pos == -1)
	{// 没有盔甲，那么就直接用裸装架子
		string handString = shuangshou?"_shuangshou":"";
		string jiazi = "qsx_" + getBaseImageName(profession) + (yuanshen?_YUANSHEN:_JIAZHI) + handString + ".png";
		jiazi_node = BaseSprite::create(jiazi);
	}
	else
	{// 有盔甲，那么就直接用盔甲拷贝作为架子
		jiazi_node = getFullPhotoOfEquip(equips[kuijia_pos],profession,shuangshou);
		if (jiazi_node->getTexture() == NULL)
		{
			CCLog("Waring : %s-->cannot find kui jia pic , use default jiazhi...",__FUNCTION__);
			string handString = shuangshou?"_shuangshou":"";
			string jiazi = "qsx_"+getBaseImageName(profession)+_JIAZHI+handString+".png";
			jiazi_node = BaseSprite::create(jiazi);
		}
	}

	// 用装备类型做索引，z_order[index]就是该装备的zorder
	// 盔甲--->副手---->主手
	int z_order[20] = {0};
	{
		int min_zOrder = 1;
		z_order[ZhuangbeiType_DanshouJian] = min_zOrder + 3;
		z_order[ZhuangbeiType_ShuangshouBishou] = min_zOrder + 1;
		z_order[ZhuangbeiType_ShuangshouFu] = min_zOrder + 1;
		z_order[ZhuangbeiType_DanshouFazhang] = min_zOrder + 1;
		z_order[ZhuangbeiType_Gong] = min_zOrder + 1;
		z_order[ZhuangbeiType_Faqi] = min_zOrder + 1;
		z_order[ZhuangbeiType_Dunpai] = min_zOrder + 2;
		z_order[ZhuangbeiType_Hushenfu] = 0;// 这个不用管
		z_order[ZhuangbeiType_KuijiaZhong] = min_zOrder;
		z_order[ZhuangbeiType_KuijiaQin] = min_zOrder;
		z_order[ZhuangbeiType_Jiezhi] = 0;// 这个不用管
		z_order[ZhuangbeiType_ShuangshouFazhang] = min_zOrder + 1;
	}

	// 3.拼装
	for (int i=0;i<equipNum;i++)
	{
		if (equips[i].zhuangbei_type == ZhuangbeiType_Jiezhi || equips[i].zhuangbei_type == ZhuangbeiType_Hushenfu)
			continue;
		BaseSprite* temp = getFullPhotoOfEquip(equips[i],profession,shuangshou);
		jiazi_node->addChild(temp,z_order[equips[i].zhuangbei_type],equips[i].zhuangbei_type);
		temp->setPosition(ccp(jiazi_node->getContentSize().width/2,jiazi_node->getContentSize().height/2));
	}

	BaseSprite* dizuo = NULL;
	if (useDiZuo)
	{// 战斗内才用底座
		char tempString[128];
		sprintf(tempString,"qsx_dizuo_%d.png",stage);
		dizuo = BaseSprite::create(tempString);
	}else
	{
		dizuo = BaseSprite::create();
	}

	CCNode* node = CCNode::create();

	node->addChild(dizuo,0,TAG_FULL_PHOTO_DIZUO);
	dizuo->setPosition(ccp(0,-jiazi_node->getContentSize().height/2+dizuo->getContentSize().height/2));
	node->addChild(jiazi_node,0,TAG_FULL_PHOTO_JIAZI);

	if (shuangshou)
	{
		CCSprite* shuangshou = CCSprite::create();
		node->addChild(shuangshou,0,TAG_FULL_PHOTO_IS_SHUANGSHOU);
	}

	return node;
}

CCNode* ImageManager::getFullPhotoOfAllShenBing(Profession profId, bool hasDiZuo)
{
	vector<ZhuangbeiSummary> vec;
	ZhuangbeiSummary temp;
	temp.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	switch (profId)
	{
		//双手神装
	case Profession_Yemanren:
		temp.zhuangbei_type = ZhuangbeiType_ShuangshouFu;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaZhong;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
	case Profession_Chike:
		temp.zhuangbei_type = ZhuangbeiType_ShuangshouBishou;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaZhong;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
	case Profession_LieShou:
	case Profession_ShenJianShou:
		temp.zhuangbei_type = ZhuangbeiType_Gong;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaQin;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
	case Profession_WuShi:
	case Profession_MiShu:
		temp.zhuangbei_type = ZhuangbeiType_ShuangshouFazhang;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaQin;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
		//单手神装
	case Profession_Fashi:
		temp.zhuangbei_type = ZhuangbeiType_DanshouFazhang;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaQin;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_Faqi;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
	case Profession_Qishi:
	case Profession_WuSeng:
	case Profession_YouXia:
		temp.zhuangbei_type = ZhuangbeiType_DanshouJian;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_KuijiaZhong;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		temp.zhuangbei_type = ZhuangbeiType_Dunpai;
		strcpy(temp.zhuangbei_pic, ChuanqiPool::getPool()->getChuanQiByEquipType(profId, 
			(ZhuangbeiType)temp.zhuangbei_type)->allChuanqiFenShenInLifeSpan.at(0).zhuangbei_pic.c_str());
		vec.push_back(temp);
		break;
	default:
		break;
	}

	return getFullPhotoOfActor(profId, vec, false, hasDiZuo);
}

// 获取全身像中，某一个装备的图片
// 对于某些装备，单双手的不同，其图片也不同，比如盔甲
// 装备图片格式 qsx_equip_[人物关键字]_[装备关键字][_颜色][_手].png
// 比如：qsx_equip_zb_kj_4_xiyou_shuangshou.png 就是一个zb_kj_4关键字的全身相双手图片
BaseSprite* ImageManager::getFullPhotoOfEquip(ZhuangbeiSummary equip,Profession profession,bool shuangshou)
{
	string handNumString = "";
	string color_str = "_"+getEquipColorName((ZhuangbeiColour)equip.zhuangbei_colour,false);

	// 白装和附魔不区分颜色光效
	if (equip.zhuangbei_colour == ZhuangbeiColour_Baizhuang || equip.zhuangbei_colour == ZhuangbeiColour_Fumo)
	{
		color_str = "";
	}

	// 是否需要单双手的标记
	if (shuangshou)
	{
		handNumString = "_shuangshou";
	}

	//// 是否需要单双手的标记,目前的装备只有
	//if (shuangshou)
	//{
	//	if (equip.zhuangbei_type == ZhuangbeiType_KuijiaQin || equip.zhuangbei_type == ZhuangbeiType_KuijiaZhong)
	//	{
	//		shuangshou = "_shuangshou";
	//	}
	//}

	string fileName = 
		"qsx_equip_"+
		getBaseImageName(profession)+"_"+
		string(equip.zhuangbei_pic)+
		color_str+
		handNumString+
		".png";

	return BaseSprite::create(fileName);
}

std::string ImageManager::getBaoshiFilename(unsigned int baoshi_type)
{
	switch(baoshi_type)
	{
	case ItemsType_Baoshi_hong_1:
		return "baoshi_hong_1.png";
	case ItemsType_Baoshi_hong_2:
		return "baoshi_hong_2.png";
	case ItemsType_Baoshi_hong_3:
		return "baoshi_hong_3.png";
	case ItemsType_Baoshi_hong_4:
		return "baoshi_hong_4.png";
	case ItemsType_Baoshi_hong_5:
		return "baoshi_hong_5.png";
	case ItemsType_Baoshi_hong_6:
		return "baoshi_hong_6.png";
	case ItemsType_Baoshi_hong_7:
		return "baoshi_hong_7.png";
	/*case ItemsType_Baoshi_hong_8:
		return "baoshi_hong_8.png";
	case ItemsType_Baoshi_hong_9:
		return "baoshi_hong_9.png";
	case ItemsType_Baoshi_hong_10:
		return "baoshi_hong_10.png";*/

	case ItemsType_Baoshi_lv_1:
		return "baoshi_lv_1.png";
	case ItemsType_Baoshi_lv_2:
		return "baoshi_lv_2.png";
	case ItemsType_Baoshi_lv_3:
		return "baoshi_lv_3.png";
	case ItemsType_Baoshi_lv_4:
		return "baoshi_lv_4.png";
	case ItemsType_Baoshi_lv_5:
		return "baoshi_lv_5.png";
	case ItemsType_Baoshi_lv_6:
		return "baoshi_lv_6.png";
	case ItemsType_Baoshi_lv_7:
		return "baoshi_lv_7.png";
	/*case ItemsType_Baoshi_lv_8:
		return "baoshi_lv_8.png";
	case ItemsType_Baoshi_lv_9:
		return "baoshi_lv_9.png";
	case ItemsType_Baoshi_lv_10:
		return "baoshi_lv_10.png";*/

	/*case ItemsType_Baoshi_zhi_1:
		return "baoshi_zi_1.png";
	case ItemsType_Baoshi_zhi_2:
		return "baoshi_zi_2.png";
	case ItemsType_Baoshi_zhi_3:
		return "baoshi_zi_3.png";
	case ItemsType_Baoshi_zhi_4:
		return "baoshi_zi_4.png";
	case ItemsType_Baoshi_zhi_5:
		return "baoshi_zi_5.png";
	case ItemsType_Baoshi_zhi_6:
		return "baoshi_zi_6.png";
	case ItemsType_Baoshi_zhi_7:
		return "baoshi_zi_7.png";
	case ItemsType_Baoshi_zhi_8:
		return "baoshi_zi_8.png";
	case ItemsType_Baoshi_zhi_9:
		return "baoshi_zi_9.png";
	case ItemsType_Baoshi_zhi_10:
		return "baoshi_zi_10.png";*/

	case ItemsType_Baoshi_huang_1:
		return "baoshi_huang_1.png";
	case ItemsType_Baoshi_huang_2:
		return "baoshi_huang_2.png";
	case ItemsType_Baoshi_huang_3:
		return "baoshi_huang_3.png";
	case ItemsType_Baoshi_huang_4:
		return "baoshi_huang_4.png";
	case ItemsType_Baoshi_huang_5:
		return "baoshi_huang_5.png";
	case ItemsType_Baoshi_huang_6:
		return "baoshi_huang_6.png";
	case ItemsType_Baoshi_huang_7:
		return "baoshi_huang_7.png";
		/*case ItemsType_Baoshi_huang_8:
		return "baoshi_huang_8.png";
		case ItemsType_Baoshi_huang_9:
		return "baoshi_huang_9.png";
		case ItemsType_Baoshi_huang_10:
		return "baoshi_huang_10.png";*/
	default:
		CCLog("Waring : %s--> unkown baoshi_type = [%d]",__FUNCTION__,baoshi_type);
		return "";
	}
}


std::string ImageManager::getGiftBagFileName(unsigned int giftId)
{
	return SystemGiftBagManager::getManager()->getGiftBagFileName(giftId);
}
//获取礼包边框底图
std::string ImageManager::getGiftBagBoardName(unsigned int giftId)
{
	unsigned int level = SystemGiftBagManager::getManager()->getGiftBagRareLevel(giftId);
	if (level == 4) return "daoju_kuang_bg_chuanqi.png";
	else if(level == 3) return "daoju_kuang_bg_xiyou.png";
	else if(level == 2) return "daoju_kuang_bg_mofa.png";
	else if(level == 1) return "daoju_kuang_bg_putong.png";
	else {CCAssert(level<=4 && level>=1, "what this rare level?");   return "daoju_kuang_bg.png";}
}
std::string ImageManager::getItemBoardFileName(unsigned int itemId)
{
	unsigned int level = ItemInformationHelper::getHelper()->getItemRareLevelByID(itemId);
	if (level == 4) return "daoju_kuang_bg_chuanqi.png";
	else if(level == 3) return "daoju_kuang_bg_xiyou.png";
	else if(level == 2) return "daoju_kuang_bg_mofa.png";
	else if(level == 1) return "daoju_kuang_bg_putong.png";
	else {CCAssert(level<=4 && level>=1, "what this rare level?");   return "daoju_kuang_bg.png";}
}
std::string ImageManager::getItemFilename(unsigned int item_type)
{
/*	if(item_type > ItemsType_Baoshi_min && item_type < ItemsType_Baoshi_max)
	{
		return getBaoshiFilename(item_type);
	}

	if(item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId)
		return "jinengshu.png";

	switch(item_type)
	{
	case ItemsType_Gold:
		return "jinbi.png";
	case ItemsType_Silver:
		return "yinbi.png";
	case ItemsType_QianghuaNormal:
		return "jinfei_putong.png";
	case ItemsType_QianghuaXiyou:
		return "jinfei_xiyou.png";
	case ItemsType_QianghuaChuangqi:
		return "jinfei_chuanqi.png";
	case ItemsType_QianghuaGailvTianjia:
		return "tiejiangzhishu.png";
	case ItemsType_HellStone:
		return "diyurongyan.png";
	case ItemsType_GoldStar_Qianghua:
		return "daoju_rongshi.png";
	case ItemsType_Shenbing_Jiefengyin:
		return "daoju_shenbing_jiesuo.png";
	case ItemsType_Shenbing_Jianding:
		return "daoju_kaienzhishu.png";
	case ItemsType_Dalishi:
		return "daoju_daLiShi.png";
	case ItemsType_Huagangyan:
		return "daoju_huaGangYan.png";
	case ItemsType_Mucai:
		return "daoju_muCai.png";
	case ItemsType_Shuijingshi:
		return "daoju_suiJing.png";
	case ItemsType_JingwenZhuanke:
		return "daoju_fulu_fuwenyin.png";
	case ItemsType_Hero_LevelUp:
		return "shengji_daoju.png";
	case ItemsType_Hero_Upgrade:
		return "daoju_shengShui.png";
	case ItemsType_DaiGouJuan:
		return "daoju_daigouoquan.png";
	case ItemsType_PresentVIP1:
		return "Vip_icon_lv1.png";
	case ItemsType_PresentVitality:
		return "daoju_tili.png";
	case ItemsType_Purify_Scroll:
		return "daoju_xilianjinghua.png";
	case ItemsType_Hammer:
		return "daoju_tiechui.png";
			
	//case ItemsType_Vitality:
	//	return "shengmingzhiquan.png";
	default:
		CCLog("Waring : %s--> unkown item_type = [%d]",__FUNCTION__,item_type);
		return "";
	}*/

	//以前的ItemType 就是现在配置表里面的ID
	string fileName = ItemInformationHelper::getHelper()->getItemFileNameByID(item_type);
	CCAssert(fileName.size()>0, "can not found this itemID in hash table of ItemInformationHelper");
	return fileName;
}
std::string ImageManager::getShenbingDebrisMaskFileName()
{
	return "zhuangbei_sphc_juxing.png";
}
std::string ImageManager::getItemDebrisFileName(unsigned int item_type)
{
	if(item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId)
		return "daoju_suipian_jinengshu.png";

	switch (item_type)
	{
	case ItemsType_Shenbing_Jiefengyin:
		return "daoju_suipian_shenbing_jiesuo.png";
	case ItemsType_Shenbing_Jianding:
		return "daoju_suipian_kaienzhishu.png";
	default:
		CCLog("Error: ImageManager::getItemDebrisFileName(unsigned int item_type)---> no such debris [%d]", item_type);
		return "";
	}
}

std::string ImageManager::getMoFajingHuaFileName(unsigned int zhuangbei_type,unsigned int jinghualevel)
{
	string key_words = "";
	{
		string key = "";
		switch (zhuangbei_type)
		{
		case ZhuangbeiType_DanshouJian:
			key = "swd";
			break;
		case ZhuangbeiType_ShuangshouBishou:
			key = "bishou";
			break;
		case ZhuangbeiType_ShuangshouFu:
			key = "fu";
			break;
		case ZhuangbeiType_DanshouFazhang:
			key = "fz_ds";
			break;
		case ZhuangbeiType_Gong:
			key = "gong";
			break;
		case ZhuangbeiType_Faqi:
			key = "fq";
			break;
		case ZhuangbeiType_Dunpai:
			key = "shld";
			break;
		//case ZhuangbeiType_Toukui:
		//	key = "zb_tk";
		//	break;
		case ZhuangbeiType_Hushenfu:
			key = "hsf";
			break;
		case ZhuangbeiType_KuijiaZhong:
			key = "kj_z";
			break;
		case ZhuangbeiType_KuijiaQin:
			key = "kj_q";
			break;
		/*case ZhuangbeiType_KuziZhong:
			key = "kz_z";
			break;
		case ZhuangbeiType_KuziQin:
			key = "kz_q";
			break;
		case ZhuangbeiType_Xuezi:
			key = "xz";
			break;*/
		case ZhuangbeiType_Jiezhi:
			key = "jz";
			break;
		case ZhuangbeiType_ShuangshouFazhang:
			key = "fz_ss";
			break;
		default:
			CCLog("Waring : %s--> unkown zhuangbei_type = [%d] ,jinghualevel = [%d]",__FUNCTION__,zhuangbei_type,jinghualevel);
			CCAssert(false,"未知的装备类型");
			break;
		}
		key_words = key;
	}

	// jinghua_kj_q_1.png
	char temp[56];
	sprintf(temp,"jinghua_%s_%d.png",key_words.c_str(),jinghualevel);
	return string(temp);

}

std::string ImageManager::getSkillPicFileName(SkillType type)
{
	std::string fileName;
	switch (type)
	{
	case SkillType_BaoLieZhan:
		fileName = "jineng_barbarian_baoliezhan_logo.png";
		break;
	case SkillType_ShaLu:
		fileName = "jineng_hunter_shalu_logo.png";
		break;
	case SkillType_ShanDian:
		fileName = "jineng_wizard_shandian_logo.png";
		break;
	case SkillType_JianShou:
		fileName = "jineng_warrior_jianshou_logo.png";
		break;
	case SkillType_ShenZhiXingNian:
		fileName = "jineng_magebane_shenzhixinnian_logo.png";
		break;
	case SkillType_ShunYinTuXi:
		fileName = "jineng_ranger_shunyingtuxi_logo.png";
		break;
	case SkillType_LieGong:
		fileName = "jineng_archer_liegong_logo.png";
		break;
	case SkillType_JianYu:
		fileName = "jineng_huntsman_jianyu_logo.png";
		break;
	case SkillType_HuoQiuShu:
		fileName = "jineng_witcher_huoqiushu_logo.png";
		break;
	case SkillType_WenYi:
		fileName = "jineng_sorcerer_wenyi_logo.png";
		break;
	case SkillType_BanYueZhan:
		break;
	case SkillType_CiSha:
		fileName = "jineng_hunter_cisha_logo.png";
		break;
	case SkillType_LeiBao:
		fileName = "jineng_wizard_leibao_logo.png";
		break;
	case SkillType_MangCi:
		fileName = "jineng_warrior_mangci_logo.png";
		break;
	case SkillType_ShenZhiZhuFu:
		fileName = "jineng_hunter_shenzhizhufu_logo.png";
		break;
	case SkillType_LiRenHuiXuan:
		fileName = "jineng_ranger_lirenhuixuan_logo.png";
		break;
	case SkillType_JingZhunZhiCan:
		break;
	case SkillType_JiSuDuJian:
		fileName = "jineng_huntsman_jishudujian_logo.png";
		break;
	case SkillType_BaoFengXue:
		fileName = "jineng_hunter_bingzhuishu_logo.png";
		break;
	case SkillType_KongJu:
		break;
	case SkillType_KuangBao:
		break;
	case SkillType_ShiXueZhiRen:
		fileName = "jineng_hunter_youmingzhinu_logo.png";
		break;
	case SkillType_YunShi:
		fileName = "jineng_wizard_yunshi_logo.png";
		break;
	case SkillType_ShouHu:
		fileName = "jineng_warrior_shouhu_logo.png";
		break;
	case SkillType_ZhongShenZhiLi:
		break;
	case SkillType_XuanFengZhan:
		fileName = "jineng_ranger_xuanfengzhan_logo.png";
		break;
	case SkillType_ZhuiHunDuJian:
		break;
	case SkillType_SiWangSaoShe:
		fileName = "jineng_hunter_siwangsaoshe_logo.png";
		break;
	case SkillType_HanBingCiGu:
		break;
	case SkillType_ShiHun:
		fileName = "jineng_hunter_shihun_log.png";
		break;
	case SkillType_BaoNu:
		break;
	case SkillType_YinZheHun:
		break;
	case SkillType_TianTangZhiGuang:
		fileName = "jineng_wizard_tiantangzhiguang_logo.png";
		break;
	default:
		CCLog("Waring : %s--> unkown SkillType = [%d]",__FUNCTION__,type);
		break;
	}

	return fileName;
}

std::string ImageManager::getSkillBookPicFileName()
{
	return "jinengshu.png";
}

ImageManager::~ImageManager(void)
{
}
