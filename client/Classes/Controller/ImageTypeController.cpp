#include "ImageTypeController.h"
#include "LFUtile.h"

ImageTypeController* ImageTypeController::mController = NULL;

string config_in_jpg[] = {
	"background1.png",
	"buzhen_bg.png",
	"buzhen_jiuguan_bg.png",
	"chessBoardFightBG.png",
	"dianfengduijue.png",
	"ditu1_1.png",
	"ditu1_2.png",
	"fuben_bg.png",
	"gongjiang_cuiqu_cuiqu_bg.png",
	"gongjiang_cuiqu_fumo_bg.png",
	"gongjiang_duanzao_bg.png",
	"gongjiang_duanzao_zhuangbeizu_neibu_bg.png",
	"gongjiang_fenjie_bg.png",
	"gongjiang_hecheng_bg.png",
	"gongjiang_qianghua_bg.png",
	"gongjiang_xiangqian_bg.png",
	"gongjiang_zhuye_bg.png",
	"jineng_bg.png",
	"jingjichang_fight_bg.png",
	"main_beijin.png",
	"main_bg.png",
	"main_progress_bar_bg.png",
	"mianban_zhuangbei.png",
	"NPC_bg.png",
	"NPC_bg_bug.png",
	"PVP_bg.png",
	"PVP_jiangli_baoxiang_bg.png",
	"PVP_jiangli_bg.png",
	"renwu_shuxing_xiangxi_bg.png",
	"renxing_bg.png",
	"shangdian_bg.png",
	"shoucheng_bg.png",
	"shoucheng_fight_bg.png",
	"shuxing_renwu_bg.png",
	"tuanduifuben_bg.png",
	"tuandui_fight_bg.png",
	"xiu_gai_zhuang_bei.png",
	"yingxiongbang_Basebg.png",
	"yingxiongbang_bg.png",
	"yingxiongbang_donghua_shu.png",

	"zhandou_beijing_caoyuan.png",
	"zhandou_beijing_chengzhenzhoubian.png",
	"zhandou_beijing_dixiajiucheng.png",
	"zhandou_beijing_feicuisenlin.png",
	"zhandou_beijing_fuhuasenlin.png",
	"zhandou_beijing_gankuzhidi.png",
	"zhandou_beijing_hanbingshanjiao.png",
	"zhandou_beijing_hanbingshanmai.png",
	"zhandou_beijing_hanbingyiji.png",
	"zhandou_beijing_hanbingzhilu.png",
	"zhandou_beijing_hanbingzhimen.png",
	"zhandou_beijing_heianzhilin.png",
	"zhandou_beijing_huangshimuxue.png",
	"zhandou_beijing_huangwuzhidi.png",
	"zhandou_beijing_jiguangsenlin.png",
	"zhandou_beijing_jihanlingyu.png",
	"zhandou_beijing_lieyandiqiao.png",
	"zhandou_beijing_mohuanconglin.png",
	"zhandou_beijing_muyuan.png",
	"zhandou_beijing_ronglusuidao.png",
	"zhandou_beijing_rongyanbaoliezhidi.png",
	"zhandou_beijing_rongyanmidao.png",
	"zhandou_beijing_rongyanneihe.png",
	"zhandou_beijing_rongyanzhimen.png",
	"zhandou_beijing_senlinshenchu.png",
	"zhandou_beijing_shijiezhishucanhai.png",
	"zhandou_beijing_shijiezhishujiaoxia.png",
	"zhandou_beijing_siwangzhaoze.png",
	"zhandou_beijing_xiashuidao.png",
	"zhandou_beijing_xiejiaodongku.png",
	"zhandou_beijing_xiejiaoneibu.png",
	"zhandou_beijing_xiejiaowaiwei.png",
	"zhandou_beijing_xingguangsenlin.png",
	"zhandou_beijing_xuedi.png",
	"zhandou_beijing_yanjiangdilao.png",
	"zhandou_beijing_yerencaoxue.png",
	"zhandou_beijing_yincangbingxue.png",
	"zhandou_beijing_yinmaishidi.png",
	"zhandou_beijing_yuangudongku.png",
	"zhandou_beijing_yuanguyiji.png",
	"zhandou_beijing_yueyeshalu.png",
	"zhandou_beijing_zhuorezhidi.png",

	"zhandou_ditu_bg.png",
	"zhandou_ditu_renwu_shuoming_bg.png",
	"zhuangbei_xiangxi_mianban.png",

	"chessBoard.png",
	"dfdj_shuoming.png",
	"gongjiang_fenjie_bg_duoge.png",
	"gongjiang_jinglianzhuanke_zhuanke_chenggong_bg.png",
	"jingjichang_suiji_bg.png",
	"jingjichang_ziyou_bg.png",
	"shangdian_bg.png",
	"shoucheng_zhanbao_bg.png",
	"shoucheng_zhandouqian_diban.png",
	"sphc_bg.png",
	"yingxiongbang_donghua_shu.png",
	"zhandou_beijing_senlinshenchu.png",
	"zhandou_beijing_shijiezhishucanhai.png",
	"zhandou_beijing_yerencaoxue.png",
	"zhandou_beijing_yueyeshalu.png",
	"zhandou_ditu_diaoluochaxun_saodang_bg.png",
	"zhandou_ditu_diaoluochaxun_saodang_jieshu_bg.png"
};


ImageTypeController::ImageTypeController(void)
{
	configs.clear();
	int config_jpg_num = sizeof(config_in_jpg)/sizeof(string);
	for (int i=0;i<config_jpg_num;i++)
	{
		configs.insert(pair<string,ImageType>(config_in_jpg[i],IT_JPG));
	}
}

ImageTypeController* ImageTypeController::getControler()
{
	if (mController == NULL)
	{
		mController = new ImageTypeController();
	}
	return mController;
}
// 获取转换之后的文件名
// 本来是kkkkk.png
// 如果有指定某个图片使用jpg的图片，那么就会返回kkkkk.jpg
string ImageTypeController::getImageFile(string fileName)
{
	map<string,ImageType>::iterator it = configs.find(fileName);
	if (it != configs.end())
	{
		vector<string> tokens = LF::string_split(fileName,".");
		int size = tokens.size();
		if (size <= 1)
		{
			return fileName;
		}
		else
		{
			string newName = "";
			for (int i=0;i<size-1;i++)
			{
				newName += tokens[i];
			}

			if (it->second == IT_JPG)
			{
				newName += ".jpg";
			}
			else if (it->second == IT_PNG)
			{
				newName += ".png";
			}else
			{
				CCAssert(false,"不支持");
			}

			return newName;
		}

	}
	return fileName;
}


ImageTypeController::~ImageTypeController(void)
{
}
