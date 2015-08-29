#pragma once
#include "flat.h"

struct ChuanQi
{
	// 该神兵的生命周期里面的各个数据(分身)
	// 不保证顺序是生命周期的
	vector<Flat_DuanzaoGroupZBInfo> allChuanqiFenShenInLifeSpan;
	// 是否包含某个groupId
	bool containsGroupId(unsigned int zbGroupId);
	// 获取该神兵某个等级的数据
	// 没有就返回false
	bool getChuanqiDataByLevel(int chuanqiLevel,Flat_DuanzaoGroupZBInfo& out_data);
	// 获取他的套装Id，没有就返回0
	int getSuitId();
	// 获取所有的GroupId
	vector<unsigned int> getGroupIds();
};

class SuitPool;
// 以前的ShenBingPool改成了ChuanqiPool
// 因为神兵降级为了传奇
class ChuanqiPool
{
private:
	static ChuanqiPool* pool;
	vector<ChuanQi> allChuanQi;
	ChuanqiPool(void);


	// 把data中的所有的结尾装备抽出来初始化allShenBing
	void initChuanqiPoolWithEndingData(const vector<Flat_DuanzaoGroupZBInfo>& data);
	// 把oneInfor链接进allShenBing
	// 肯定会链接进去的
	// 有可以拼接的就拼接，没有就独立一个分支
	void linkToChuanqiPool(Flat_DuanzaoGroupZBInfo oneInfor);

	// 根据shenBing的尾部要求，从data挨个合并
	// 直到找不到再合并的
	void combineToChuanqiFromEnding(ChuanQi* chuanqi,const vector<Flat_DuanzaoGroupZBInfo>& data);

	//// 调用SuitPool函数
	//void call_SuitPool_combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId);

public:
	static ChuanqiPool* getPool();

	void initChuanQiData(vector<Flat_DuanzaoGroupZBInfo> data);

	vector<ChuanQi> getAllChuanqi(){return allChuanQi;};

	// 获取神兵的数据，没有就返回false.神兵有N个分身
	// 随便传一个zbGroupId都可以获取
	bool getChuanQiData(unsigned int zbGroupId,ChuanQi& outData);
	// 获取神兵某个分身(或者等级)的数据，没有就返回false.
	// 神兵有N个分身
	// 随便传一个zbGroupId都可以获取
	bool getChuanQiData(unsigned int zbGroupId,Flat_DuanzaoGroupZBInfo& outData,int level);

	//// 是否是神兵
	//bool isShenBing(unsigned int zbGroupId);

	// 修改SuitPool，用于支持神兵有多个groupId
	// 在ShenBingPool和SuitPool都init之后才能用
	void modifySuitPoolToSurportShenBing();

	//通过 hero profession获取传奇装的名字
	string getChuanQiSuitName(Profession prof);
	//获取某英雄的某类型传奇装
	const ChuanQi* getChuanQiByEquipType(Profession prof, ZhuangbeiType type);

	~ChuanqiPool(void);
};

