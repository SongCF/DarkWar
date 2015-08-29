#pragma once

#include <string>
#include "cocos2d.h"
#include "BaseActor.h"
#include "Equipment.h"


class Equipment;

using namespace std;
using namespace cocos2d;

// 全身相中的部分
#define TAG_FULL_PHOTO_DIZUO			23421
#define TAG_FULL_PHOTO_JIAZI			23422
#define TAG_FULL_PHOTO_IS_SHUANGSHOU	23424 // 是否是双手

// 封装的图片信息
struct ImageData
{
	// 基础的名字，比如野蛮人就对应：yeManRen
	string baseImageName;
	// 图片类型，比如".png",".jpg"等
	string imageType;
	// 当前的图片名字,比如"panle_show_yeManRen"
	string currentImageName;
	// 图片需要做的缩放
	float scale;

	ImageData()
	{
		baseImageName = "";
		imageType = ".png";
		currentImageName = "";
		scale = 1.0f;
	}
	// 得到可用的名字，就是currentImageName+imageType
	string getCurrentImageFile()
	{
		return currentImageName + imageType;
	}
};

// 一个图片的管理器
// 它告诉我哪种角色，使用什么样的图片名字(这都是基础的名字)
// 因为还有具体地方的名字，在不同的地方使用的是不同的图片，
// 或者是相同的图片，不同的缩放。
class ImageManager
{
private:
	static ImageManager* manager;

	ImageManager(void);

public:

	static ImageManager* getManager();

	// 初始化Manager
	bool initManager();

	// 获得基础图片名字,比如野蛮人就是barbarian
	string getBaseImageName(Profession actorType);
	// 

	// 获取角色的全身像,用于创建角色等界面
	ImageData getImageData_JueSheQuanShen(Profession actorType);
	// 得到任务面板下面的图片数据
	ImageData getImageData_ProfessionHead(Profession actorType,bool doubleHande = false);


	/////////////////////////////////////////////
	//神兵全是false   去掉比传奇还好的装备      
	////////////////////////////////////////////

	ccColor3B getShowColor(ZhuangbeiColour zb_color,bool is_shenbing,bool is_suit = false);

	string getEquipColorName(ZhuangbeiColour zb_color, bool is_shenbing=false, bool is_suit = false);

	// 得到装备列表的装备底纹
	ImageData getImageData_DiWen_List(ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false,int zhuanKe=0);
	// 详情界面的底纹
	ImageData getImageData_DiWen_XiangQing(ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false,int zhuanKe=0);
	// 底纹被点击的效果
	ImageData getImageData_DiWen_Selected(ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false);
	ImageData getImageData_DiWen_Selected_ZhuangBeiMianBan(int boxTag, ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false);
	// 人形界面的底纹
	ImageData getImageData_DiWen_ZhuangBeiMianBan(int boxTag, ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false,int zhuanKe=0);
	// 有外边框的底纹
	string getImageData_DiWen_WithBianKuang(ZhuangbeiColour zb_color,bool is_shenbing=false,bool is_suit=false);




	string getEquipImageFile(string equipKeyWords);
	// 得到面板处的装备
	ImageData getImageData_ZhuangBei_MianBan(Equipment* equip);
	// 得到列表处的装备
	ImageData getImageData_ZhuangBei_List(Equipment* equip);
	// 得到详情处的装备
	ImageData getImageData_ZhuangBei_XiangQing(Equipment* equip);
	//锻造界面添加装备
	ImageData getImageData_ZhuangBei_DuanzaoAdd(Equipment* equip);
	//锻造成功装备
	ImageData getImageData_ZhuangBei_DuanzaoSuc(Equipment* equip);
	// 装备掉落的图片
	ImageData getImageData_ZhuangBei_DiaoLuo(EquipData equip);

	//给一个装备id得到装备的图片的sprite
	BaseSprite* getSpriteOfZhuangbei(Equipment* equip,bool showJingLian= false,bool showQianghua = false);
	BaseSprite* getSpriteOfZhuangbei(EquipData equip,bool showJingLian= false,bool showQianghua = false);
	enum ZhuangbeiColourExt
	{
		ZhuangbeiColourExt_Baizhuang = 1,
		ZhuangbeiColourExt_Fumo = 2,
		ZhuangbeiColourExt_Xiyou = 3,
		ZhuangbeiColourExt_Chuanqi = 4,
		//  上面的要跟protocal一致
		ZhuangbeiColourExt_ChuanQiTaoZhuang = 20,
		ZhuangbeiColourExt_ShengBing = 21,
		ZhuangbeiColourExt_ShengBingTaoZhuang = 22,
	};
	BaseSprite* getSpriteOfZhuangbei(string equipKeyWords,ZhuangbeiColourExt color,unsigned holdNum,vector<ItemsType> diamonds,QianghuaData qinghua,unsigned int jingLianTime = 0);

	// 获取角色的全身像,有2种状态，1是战斗中，2是其他情况
	// 需要的参数是：角色的类型，角色的身上的所有的装备的概述。
	// 
	// 程序根据equips的类型给装备进行上下错开，防具在上，攻击在下，顺序是盔甲，盾牌，武器
	// 
	// 根据profession,是否单双手,是否是战斗里面，程序就去拿不同的架子，架子就是基底
	// 架子里面带了所有的装备
	// 
	// 返回的是CCNode，Node里面把架子，装备，底座，组织好了 返回的.
	// 通过getChildByTag(TAG_FULL_PHOTO_DIZUO)可以得到底座部分
	// 通过getChildByTag(TAG_FULL_PHOTO_JIAZHI)可以得到架子部分，架子里面有所有的装备
	CCNode* getFullPhotoOfActor(Profession profession,vector<ZhuangbeiSummary> equips,bool yuanshen = false,bool useDiZuo = true,int stage = 1);
	// 获得战斗外面的角色全身相
	CCNode* getFullPhotoOfActor(BaseActor* actor,bool useDiZuo = true);
	// 获取某英雄穿上神装后的样子
	CCNode* getFullPhotoOfAllShenBing(Profession profId, bool hasDiZuo=false);

	// 获取全身像中，某一个装备的图片
	// 对于某些装备，单双手的不同，其图片也不同，比如盔甲
	BaseSprite* getFullPhotoOfEquip(ZhuangbeiSummary equip,Profession profession,bool shuangshou);
	//// 获取底座的图片包括阴影
	//BaseSprite* getFullPhotoOfDiZuoPic(Profession profession,bool inFight,bool shuangshou);

	//获取礼包图片
	std::string getGiftBagFileName(unsigned int giftId);
	//获取礼包边框底图
	std::string getGiftBagBoardName(unsigned int giftId);
	//获取道具边框底图
	std::string getItemBoardFileName(unsigned int itemId);
	//获取道具图片
	std::string getItemFilename(unsigned int item_type);
	//获取宝石图片名
	std::string getBaoshiFilename(unsigned int baoshi_type);
	//通过itemtype 获得道具的碎片 文件名
	std::string getItemDebrisFileName(unsigned int item_type);
	//神兵碎片的图片命名
	std::string getShenbingDebrisMaskFileName();

	// 获取魔法精华的图片
	std::string getMoFajingHuaFileName(unsigned int zhuangbei_type,unsigned int jinghualevel);

	// 获取技能图片
	std::string getSkillPicFileName(SkillType type);
	//获得 技能书 图片
	std::string getSkillBookPicFileName();

	~ImageManager(void);
};

