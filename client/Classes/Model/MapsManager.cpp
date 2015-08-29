
#include "MapsManager.h"
#include "BaseSprite.h"

MapsManager* MapsManager::s_mManager = NULL;

MapsManager::MapsManager()
{
}

MapsManager::~MapsManager()
{
	for(map<int, CCArray*>::iterator i=mMap.begin(); i!=mMap.end(); ++i)
	{
		CC_SAFE_RELEASE(i->second);
	}

}

MapsManager* MapsManager::getManager()
{
	if(s_mManager == NULL)
	{
		s_mManager = new MapsManager;
	}

	return s_mManager;
}


CCSprite* MapsManager::getJingJiChangMapImage(int idx)
{
	string imgFileName;

	imgFileName = "jingjichang_fight_bg.png";

	return BaseSprite::create(imgFileName.c_str());
}

CCSprite* MapsManager::getTuanDuiFuBenMapImage(int idx)
{
	string imgFileName;

	imgFileName = "tuandui_fight_bg.png";

	return BaseSprite::create(imgFileName.c_str());
}

CCSprite* MapsManager::getShouChengFuBenMapImage(int idx)
{
	string imgFileName;

	imgFileName = "shoucheng_fight_bg.png";

	return BaseSprite::create(imgFileName.c_str());
}

CCSprite* MapsManager::getBackGroundMapImage(int taskId)
{
	CCSprite *bg;

	string imgFileName;
	switch (taskId)
	{
	case 1:
		imgFileName = "zhandou_beijing_feicuisenlin.png";//
		break;
	case 2:
		imgFileName = "zhandou_beijing_jiguangsenlin.png";//
		break;
	case 3:
		imgFileName = "zhandou_beijing_yinmaishidi.png";//
		break;
	case 4:
		imgFileName = "zhandou_beijing_siwangzhaoze.png";//
		break;
	case 5:
		imgFileName = "zhandou_beijing_mohuanconglin.png";//
		break;
	case 6:
		imgFileName = "zhandou_beijing_senlinshenchu.png";//
		break;
	case 7:
		imgFileName = "zhandou_beijing_heianzhilin.png";//
		break;
	case 8:
		imgFileName = "zhandou_beijing_yerencaoxue.png";//
		break;
	case 9:
		imgFileName = "zhandou_beijing_yuangudongku.png";//
		break;
	case 10:
		imgFileName = "zhandou_beijing_yuanguyiji.png";//
		break; 
	case 11:
		imgFileName = "zhandou_beijing_chengzhenzhoubian.png";//
		break;
	case 12:
		imgFileName = "zhandou_beijing_xiashuidao.png";//
		break;
	case 13:
		imgFileName = "zhandou_beijing_dixiajiucheng.png";//
		break;
	case 14:
		imgFileName = "zhandou_beijing_xingguangsenlin.png";//
		break;
	case 15:
		imgFileName = "zhandou_beijing_shijiezhishujiaoxia.png";//
		break;
	case 16:
		imgFileName = "zhandou_beijing_fuhuasenlin.png";//
		break;
	case 17:
		imgFileName = "zhandou_beijing_yueyeshalu.png";//
		break;
	case 18:
		imgFileName = "zhandou_beijing_huangwuzhidi.png";//
		break;
	case 19:
		imgFileName = "zhandou_beijing_huangshimuxue.png";//
		break;
	case 20:
		imgFileName = "zhandou_beijing_shijiezhishucanhai.png";//
		break; 
	case 21:
		imgFileName = "zhandou_beijing_xuedi.png";//
		break;
	case 22:
		imgFileName = "zhandou_beijing_hanbingshanjiao.png";//
		break;
	case 23:
		imgFileName = "zhandou_beijing_xiejiaodongku.png";//
		break;
	case 24:
		imgFileName = "zhandou_beijing_hanbingshanmai.png";//
		break;
	case 25:
		imgFileName = "zhandou_beijing_xiejiaowaiwei.png";//
		break;
	case 26:
		imgFileName = "zhandou_beijing_xiejiaoneibu.png";//
		break;
	case 27:
		imgFileName = "zhandou_beijing_hanbingzhimen.png";//
		break;
	case 28:
		imgFileName = "zhandou_beijing_hanbingzhilu.png";//
		break;
	case 29:
		imgFileName = "zhandou_beijing_jihanlingyu.png";//
		break;
	case 30:
		imgFileName = "zhandou_beijing_hanbingyiji.png";//
		break; 
	case 31:
		imgFileName = "zhandou_beijing_gankuzhidi.png";//
		break;
	case 32:
		imgFileName = "zhandou_beijing_zhuorezhidi.png";//
		break;
	case 33:
		imgFileName = "zhandou_beijing_rongyanzhimen.png";//
		break;
	case 34:
		imgFileName = "zhandou_beijing_ronglusuidao.png";//
		break;
	case 35:
		imgFileName = "zhandou_beijing_yincangbingxue.png";//
		break;
	case 36:
		imgFileName = "zhandou_beijing_lieyandiqiao.png";//
		break;
	case 37:
		imgFileName = "zhandou_beijing_rongyanmidao.png";//
		break;
	case 38:
		imgFileName = "zhandou_beijing_rongyanbaoliezhidi.png";//
		break;
	case 39:
		imgFileName = "zhandou_beijing_yanjiangdilao.png";//
		break;
	case 40:
		imgFileName = "zhandou_beijing_rongyanneihe.png";//
		break; 
	default:
		CCAssert(false, "ERROR: TransLationMapsManager::getBackGroundMapImage ---> error taskId");
		imgFileName = "zhandou_beijing_rongyanneihe.png";//test
		break;
	}

	bg = BaseSprite::create(imgFileName.c_str());
	
	return bg;
}

CCSprite* MapsManager::getXueSeJiaoTangMapImage(int grid)
{
	CCSprite *img = NULL;
	// 1 21 41 61 81
	int lv = (grid + 19) / 20;
	if (lv == 1)
		img = CCSprite::create("zhandou_beijing_huangshimuxue.jpg");
	else if (lv == 2)
		img = CCSprite::create("zhandou_beijing_muyuan.jpg");
	else if (lv == 3)
		img = CCSprite::create("zhandou_beijing_dixiajiucheng.jpg");
	else if (lv == 4)
		img = CCSprite::create("zhandou_beijing_xiejiaoneibu.jpg");
	else if (lv == 5)
		img = CCSprite::create("zhandou_beijing_zhuorezhidi.jpg");

	return img;
}
