#pragma once
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

class BaseActor;
class Equipment;

// 为什么他是好装备
// 也就是提示的理由
enum WhyItsBetter
{
	Because_HP = 0,
	Because_HuaJia,
	Because_GongJi,
	Because_Color,
	Because_FastCompare,
	Because_LastCompare,
};

struct TestItem
{
	BaseActor* actor;

	// 评测出的装备，不同的装备内型，测试的方式不一样。
	// 比如单手剑只看攻击，只要攻击高，就会出现在这里了，理由就是Because_GongJi。
	// 但是如果装备是传奇及其以上，并且actor上面没有对应类型的成色装备那么必提示,并且理由就是Because_Color.
	// 不同的Type之间的装备不会提示
	vector<Equipment*> betterEquipVec;	// 好的装备
	vector<Equipment*> oldEquipVec;		// 与之比较的对应的差的装备，主要用于区分2个戒指的情况,没有与之相比较的就是NULL，
	vector<int> comparedPlace;			// 与之比较的卡槽  
										// 不存在【双手装】比【单手+盾】的情况
	vector<int> hpChangeVec;			// 血量的变化
	vector<int> chengShouLiChangeVec;	// 承受力的变化
	vector<int> gongJiChangeVec;		// 攻击的变化
	vector<WhyItsBetter> reasonVec;		// 被选进来的原因

	//void removeOneByBetterEquip(const Equipment* equip)
	//{
	//	vector<Equipment*>::iterator itBetter = betterEquipVec.begin();	
	//	vector<Equipment*>::iterator itOld = oldEquipVec.begin();		
	//	vector<int>::iterator itHp = hpChangeVec.begin();			
	//	vector<int>::iterator itChengShouLi = chengShouLiChangeVec.begin();			
	//	vector<int>::iterator itGongJi = gongJiChangeVec.begin();
	//	vector<WhyItsBetter>::iterator itReason = reasonVec.begin();	
	//}
};

enum ReportMeta
{
	RM_AllDataIsOk = 0,	// reportList里面的血量变化，护甲变化，攻击变化等都是正确的
	RM_OnlyByWhyIsOk,	// reportList里面的血量变化，护甲变化，攻击变化等只有reasonVec中指定的项目是正确的
};

// 测试报告
struct TestReport
{
	unsigned int timestamp;	// 上次刷新时间
	// 描述
	ReportMeta metaData;
	// 报表
	vector<TestItem> reportList;
}; 

struct IgnoreInfor
{
	BaseActor* actor;
	int place;  //EquipRequirePlace
	bool ignore;

	IgnoreInfor()
	{
		actor = NULL;
		place = -1;
		ignore = false;
	}
};

// 保存某个英雄提示的哪个装备已经查看过了
struct ReadedZhuanbei
{
	BaseActor* actor;
	map<const Equipment*,bool> equips;

	ReadedZhuanbei()
	{
		actor = NULL;
	}

	bool contains(const Equipment* equip)
	{
		return (equips.find(equip) != equips.end());
	};

	void remove(const Equipment* equip)
	{
		map<const Equipment*,bool>::iterator it = equips.find(equip);
		if (it != equips.end())
			equips.erase(it);
	}

	void add(const Equipment* equip)
	{
		if (contains(equip))
			return;

		equips.insert(pair<const Equipment*,bool>(equip,true));
	}

	void add(map<const Equipment*,bool> equips)
	{
		map<const Equipment*,bool>::iterator it = equips.begin();
		for (; it!=equips.end(); it++)
		{
			add(it->first);
		}
	}
};

struct CompareItem
{
	const Equipment* new_equip;
	int place;
	CompareItem(const Equipment* _new_equip,int _place)
	{
		new_equip = _new_equip;
		place = _place;
	}

	// 只用<号也能判断相等啊：
	//if( (!(a<b) && !(b<a) )
	//	　　　//ok,a==b
	bool operator < (const CompareItem& rs) const
	{
		if(new_equip < rs.new_equip)
		{
			return true;
		}
		else if(new_equip == rs.new_equip)
		{
			return place < rs.place;
		}
		return false;
	}
};
struct AlreadyGoodZhuangbei
{
	BaseActor* actor;
	
	map<CompareItem,bool> new_old_equips;

	bool contains(const Equipment* new_equip,int _place)
	{
		return (new_old_equips.find(CompareItem(new_equip,_place)) != new_old_equips.end());
	};

	void remove(const Equipment* new_equip,int _place)
	{
		map<CompareItem,bool>::iterator it = new_old_equips.find(CompareItem(new_equip,_place));
		if (it != new_old_equips.end())
			new_old_equips.erase(it);
	}

	void removeAll(const Equipment* new_equip)
	{
		map<CompareItem,bool>::iterator it = new_old_equips.begin();
		for (;it != new_old_equips.end();)
		{
			if (it->first.new_equip == new_equip)
			{
				new_old_equips.erase(it++);	
				continue;
			}
			it++;
		}
	}

	void add(const Equipment* new_equip,int _place)
	{
		if (contains(new_equip,_place))
			return;
		new_old_equips.insert(pair<CompareItem,bool>(CompareItem(new_equip,_place),true));
	}

	void add(map<const Equipment*,bool> new_equips,int _place)
	{
		map<const Equipment*,bool>::iterator it = new_equips.begin();
		CompareItem tmp(NULL,-1);
		for (; it!=new_equips.end(); it++)
		{
			tmp.new_equip = it->first;
			tmp.place = _place;
			if (contains(tmp.new_equip,tmp.place))
				continue;

			new_old_equips.insert(pair<CompareItem,bool>(tmp,true));
		}
	}
};

class ZhuangbeiScanner
{
private:
	static ZhuangbeiScanner* mInstance;

	// 是否装备库变化了
	bool isEquipPoolModifiedFromLastGenReport;
	// 装备库
	vector<Equipment*> equipmentToCompare;
	// 已读的装备
	map<BaseActor*,ReadedZhuanbei> readedEquips;
	// 记录已经比较过是差装备的记录
	map<BaseActor*,ReadedZhuanbei> notGoodEquips;
	// 已经是好装备了
	map<BaseActor*,AlreadyGoodZhuangbei> goodEquipsByPrevGenReport;


	// 是否忽略提示，忽略就是true
	bool ignoreHint;

	TestReport mLastReport;

	ZhuangbeiScanner(void);

	// 将actor身上的装备填充到equips中
	// 并保证根据装备在actor中的pos为索引可以访问
	// 即equips[POS_XiongJia]一定是胸甲，没有就是NULL
	void fillEquipsByCurrentPos(BaseActor* actor,vector<Equipment*>& equips);

	vector<Equipment*> getCurentNotUsedEquipFromComparePool();

	// 没找到的话就初始化一个，并插入进去
	ReadedZhuanbei getReadedZhuanbeiFromMapByActor(map<BaseActor*,ReadedZhuanbei>& data,BaseActor* actor);
	// 没找到的话就初始化一个，并插入进去
	AlreadyGoodZhuangbei getAlreadyGoodZhuangbeiFromMapByActor(map<BaseActor*,AlreadyGoodZhuangbei>& data,BaseActor* actor);

	bool isNecessaryToCompare(BaseActor* actor,Equipment* oldEquip,Equipment* newEquip);

	// 看当前的配装情况，遇到一个新的装备需要比较不
	//currentEquipOnHero 要求根据pos为索引就可以获取到数据的
	bool isNeccessaryToCompearByCurrentEquipsOnHero(Equipment* newEquip,vector<Equipment*> currentEquipOnHero);

	// 生成测试报表，一般是有新装备加入的时候就会触发一次报表生成
	// 很耗时的一件事
	TestReport genTestReport();

	enum FastCompearState
	{
		FCS_Good = 0,
		FCS_Bad,
		FCS_NotSure,
	};
	// 快速比较
	// 新装备是好东西的，就返回true
	// 不然就返回false
	FastCompearState fastCompear(BaseActor* actor,Equipment* newEquip,Equipment* oldEquip);

public:
	static ZhuangbeiScanner* getScanner();

	void addOneEquipment(Equipment* equip);
	void removeOneEquipment(const Equipment* equip);
	void equipmentChanged(Equipment* equip); // 主要是强化，精炼，等回调

	// 设置装备已读
	// 那么report里面就不会再包含这个装备的数据
	void setMarkRead(BaseActor* actor,Equipment* equip);

	// 获取当前的测试报告(高效率)
	// 如果是忽略状态，获取的报表就是空的
	// 非忽略状态得到的就是上次genTestReport的报表
	// genTestReport会自动触发
	TestReport getTestReport();
	// 只取某个Actor的Report
	// 可能没有他的报表
	TestReport getTestReportByActor(BaseActor* actor);

	// 测试actor换上testEquip并且脱掉oldEquip之后的血量，攻击，护甲的变化
	// oldEquip为NULL就不脱
	// 负数就是减，正数就是提升
	// testEquip就是新装备，oldEquip是就装备
	void getPropertyChangeData(BaseActor* actor,Equipment* testEquip,Equipment* oldEquip,int& out_hpChange,int& out_huJiaChange,int& out_gongJiLiChange);
	// 使用指定的初始hp，护甲，攻击
	// 而不再调用getHp，getGongji，为了加快速度
	void getPropertyChangeDataByInitedData(BaseActor* actor,int initHp,int initHujia,int initGongji,Equipment* testEquip,Equipment* oldEquip,int& out_hpChange,int& out_huJiaChange,int& out_gongJiLiChange);
	
	enum ValueType
	{
		VT_HP = 0,
		VT_CHENGSHOULI,
		VT_GONGJI,
	};
	// 值计算某个分支的数据
	void getPropertyChangeDataByInitedData2(BaseActor* actor,ValueType valueType,int initValue,Equipment* testEquip,Equipment* oldEquip,int& out_change);

	// true就忽略，于是就不会再提示了
	// 忽略全部，或者取消全部的忽略
	void setIgnoreHint(bool ignore);

	// 2014-03-05-->废弃了，空接口！
	// 设置某个人的的某个装备卡槽提示，是否忽略
	// pos 是 EquipRequirePlace 类型数据
	// 目前为了简单，暂时不会影响getTestReport之类的获取报表的接口
	// 这里的忽略列表对报表透明
	// 外界可以调用isIgnoreHint测试是否有忽略某个地方
	// 修正：2014-03-05
	// 这个接口没有实质性作用了
	// 其实可以不用再通过这个接口来看/设置是否某个槽位是否忽略了
	// 一旦装备被setMarkRead,那么report就会实时变化的
	void setIgnoreHint(bool ignore,BaseActor* actor,int pos);

	// 2014-03-05-->废弃了，空接口！
	// 是否某个位置忽略了
	// 修正：2014-03-05
	// 这个接口没有实质性作用了
	// 其实可以不用再通过这个接口来看是否某个槽位是否忽略了
	// 一旦装备被setMarkRead,那么report就会实时变化的
	bool isIgnoreHint(BaseActor* actor,int pos);

	~ZhuangbeiScanner(void);
};

