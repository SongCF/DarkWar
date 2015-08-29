#pragma once

#include <string>
#include "cocos2d.h"
#include "protocol.h"
#include "CommonDefine.h"
#include "AutoReleaseAble.h"

using namespace cocos2d;
using namespace std;  

#define SORT_FLAG_DESC	1
#define SORT_FLAG_ASC	0

struct CostData
{
	ItemsType item;
	unsigned int count;
};

// 装备的父类
class Equipment : public AutoReleaseAble
{
protected:
	EquipData mEquipData;
	EquipType type;
	// 当前的位置，如果getActorId != 0这个字段才有效
	EquipRequirePlace currentPlace;
	vector<EquipRequirePlace> requirePlaces; // 可以被放到的地方
	//EquipRequirePlace requirePlace;
public:
	Equipment(EquipData equipData);
	void refreshData(EquipData equipData);
	~Equipment();
	void setActorId(unsigned int _id);
	// 获取需要装备到的地方
	// 也就是一会会装备到的地方
	//EquipRequirePlace getRequiredPlace();
	vector<EquipRequirePlace> getRequiredPlaces();
	// 该物品能否装到某个位置，可以装就返回true，但是不见得能直接装上，因为可能该位置被别人装了
	bool canEquipToPlace(EquipRequirePlace place);
	void setCurrentEquipedPlace(EquipRequirePlace place);

	// 得到当前的所装位置：注意必须是已经装在hero上才可以调用
	EquipRequirePlace getCurrentPlace();

	EquipData getEquipData() const;
	unsigned int getBaoShiNum() const;
	unsigned int getActorId() const;
	// 得到大的类型，就是防具，饰品，攻具，不包括宝石
	EquipType getEquipType() const;
	// 小类型
	ZhuangbeiType getType() const;
	ZhuangbeiColour getEquipColor() const;
	unsigned int getLevel()  const;
	unsigned long long getEquipId() const;
	unsigned int getGroupId() const;
	string getName() const;
	string getPicFile() const;

	// 排序，根据强化的效果百分比
	void sortQianghuaInforByPercent(vector<QianghuaInfo>& source,int sortFlag = SORT_FLAG_DESC);

	// 得到强化之后的攻击力,只是白字！
	// 如果不是武器，那么攻击就是0 ，如果不是防具，护甲就是0
	float getGongjiLiAfterQianghua(bool random = true);
	float getHuJiaZhiAfterQianghua();


	//获取所有魔法属性的格式化字符串
	//比如"+30 敏捷"
	std::vector<std::string> getAllMofaShuxing(bool includeAddtionall=true);

	//获取装备固有的魔法属性描素
	std::vector<std::string> getFixedMoFaShuxing();
	//获取精炼的魔法属性
	std::vector<std::string> getJingLianMoFaShuxing();
	//获取篆刻的魔法属性
	std::vector<std::string> getZhuanKeMoFaShuxing();
	//获取鉴定了的属性
	std::vector<std::string> getKnownHidenMoFaShuxing();


	//获取所有镶嵌的宝石的属性格式化字符串
	struct BaoshiShuxing
	{
		unsigned int type;
		std::string shuxing;
	};
	std::vector<Equipment::BaoshiShuxing> getAllBaoshiShuxing();

	// 得到当前的精炼阶数
	int getCurrentJingLianJieShu();
	// 是否篆刻了经文：见canBeZhuanKe
	bool haveZhuanke();
	// 是否还有未鉴定的属性
	bool haveUnkownHidenAttribute();
	// 得到当前未鉴定的属性的数目
	int getCurrentUnkownHideAttriNum();
	// 是否是神兵
	bool isShenBing();

	// 是否是套装的一部分
	bool isPartOfSuit();
	// 是否是在封印里面
	bool isInFengYin() const;


	// 是否可以完全的继承装备data的强化属性
	// 返回true，就是可以完全继承，
	// false就不能完全继承。也就是自己装不完data的强化数据,那么throwAway就是装的被抛弃的强化效果
	// newData存放的是如果继承了data之后的新的强化数据
	bool canFullyInheritanceQiangHuaData(const EquipData& data,vector<QianghuaInfo>& throwAway,QianghuaData& newData);

	// 是否可以被篆刻经文，见haveZhuanke
	// 默认只要是一件有能力篆刻的装备就会返会true
	bool canBeZhuanKe(string & infor,bool ifAbleJustReturnTrue = true);

	// 是否可以用equip来精炼自己
	bool canJingLianUseTheEquip(string & infor,const Equipment* equip,bool ifAbleJustReturnTrue = true);
	// 是否可以精炼
	bool canBeJingLian(string & infor,bool ifAbleJustReturnTrue = true);
	// 还可以精炼的次数
	int jingLianTimeLeft();

	// 获取主属性类型
	// 如果是攻击，统一返回最小攻击
	AttrType getMainProType();
	// 获取每次精炼加成的属性加成百分比
	unsigned int getPropertyAddRatioByEachJingLian();

	// 是否可以被强化
	// 默认忽略当前已经强话的次数
	bool canBeQianghua(string & infor,bool ifAbleJustReturnTrue = true);
	// 是否可以回退
	bool canBeHuiTui(string& infor);

	// 是否可以被萃取,只要可以萃取，就是true
	bool canBeCuiqu(string & infor,bool ifAbleJustReturnTrue = true);
	// 是否可以萃取指定的某个魔法属性
	bool canBeCuiquAttribute(unsigned int att_type,string & infor,bool ifAbleJustReturnTrue = true);

	// 是否可以附魔
	bool canBeFumo(string & infor);
	// 是否可以附魔一个指定的魔法精华
	bool canBeFomoAttribute(MofaJinghua jinghua,string & infor);

	// 是否可以拿去鉴定
	bool canBeJianDing(string & infor,bool ifAbleJustReturnTrue = true);
	// 是否可升级，只有神兵可以
	bool canLevelUp(int& errorCode);

	//是否可以洗练
	bool canPurify(string &info, int holeId, bool ifAbleJustReturnTrue = true);
	//能洗练的次数
	//unsigned int CS::getPurifyMaxHoles(unsigned int type, unsigned int colour, unsigned int level);

	// 获取可以用来升级神兵自己的饲料装备
	// 最差的放在0号位置
	// 如果equips里面时空，那么返回值int就会标示原因：
	// 
#define STATE_Ok				0
#define STATE_Not_Need_Equip	1
#define STATE_Not_Found_Equip	2
	int getEquipmentsCanUsedToLevelUpShengBing(vector<Equipment*>& equips);
	int getEquipmentsCanUsedToJingLian(vector<Equipment*>& equips);
	int getEquipmentsCanUsedToJieFeng(vector<Equipment*>& equips);//黄装解封

	// 获取神兵解封印花费
	vector<CostData> getShenBingJieFengyinCost();
	// 获取神兵当前需要鉴定的属性花费
	vector<CostData> getShenBingJianDingCurrentCost();
	// 获取神兵篆刻的花费
	vector<CostData> getShenBingZhuankeCost();
	// 获取装备精炼花费
	vector<CostData> getZbJingLianCost();
	// 获取神兵升级需要的花费
	vector<CostData> getShenBingLevelUpCost();


#pragma region 修改数据

	// 继承equip的强化属性
	// equip的强化属性就清空？？---> 目前是扔掉
	void inheritanceQiangHuaData(Equipment* equip,vector<QianghuaInfo>& throwAway);
	// 清空当前已经强化过的数据
	void clearCurrentQiangHuaData();
	// 添加一个强化效果
	void addOneQiangHuaInfor(QianghuaInfo infor);
	// 设置新的强化效果
	void setNewQiangHuaData(QianghuaData data);
	// 设置新的附魔信息
	void setNewFuMoAttribute(ZBAttr newAttri); // 设置新的魔法属性

	// 孔的数量增加
	void holeNumAdd();	
	// 添加一个宝石
	void addOneBaoShi(ItemsType baoshiType);
	// 清空所有的宝石
	void clearAllbaoShi();
	// 只对武器有用
	void setMinGongJiLi(unsigned int gongji_min);
	void setMaxGongJiLi(unsigned int gongji_max);
	// 只对防具有用
	void setHuJia(unsigned int hujia);

	// 解封印
	void jieFengYing();
	// 添加解开的隐藏属性
	void addKownedHideAttribute(ZBAttr att);
	// 添加篆刻的属性
	void addZhuanKeAttribute(ZBAttr att);
	// 设置精炼/篆刻/鉴定的属性
	void setJingLianAttribute(vector<ZBAttr> att_vec,unsigned int currentJingLianTime,vector<ZBAttr> next);
	void setZhuanKeAttribute(vector<ZBAttr> att_vec);
	void setKownedHideAttribute(vector<ZBAttr> att_vec);
	// 设置洗练的属性
	void setPurifyAttribute(vector<ZBAttr> att_vec);
#pragma endregion

};