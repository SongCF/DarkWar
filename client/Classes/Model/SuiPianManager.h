#pragma once
#include "protocol.h"
#include "flat.h"


#define MAX_SUIPIAN_ID_NUM	5	// 最多5个

struct ItemSuipian
{
	ItemsType item_type;
	unsigned int suipian_id; // 1~5
	unsigned int suipian_count;
};

struct ShenbingSuipian
{
	unsigned int zb_group_id;
	unsigned int suipian_id; // 1~5
	unsigned int suipian_count;

	// 该碎片所属神兵的数据（这个数据也可以从ShenBingPool中查询）
	Flat_DuanzaoGroupZBInfo suiPianDetail;
};


// 有组织的数据
struct ItemSuipianGroupData
{
	ItemsType item_type;
	// count_array[suipian_id] 就是该item_type类型的碎片的个数
	int count_array[MAX_SUIPIAN_ID_NUM + 1];

	ItemSuipianGroupData()
	{
		memset(count_array,0,sizeof(count_array));
	}
};

struct ShenbingSuipianGroupData
{
	// 碎片的信息
	Flat_DuanzaoGroupZBInfo suiPianDetail;
	// 个数：count_array[suipian_id] 就是该suiPianDetail.zb_group_id类型的碎片的个数
	int count_array[MAX_SUIPIAN_ID_NUM + 1];

	ShenbingSuipianGroupData()
	{
		memset(count_array,0,sizeof(count_array));
	}
};


class SuiPianManager
{
private:
	static SuiPianManager* instance;

	vector<ItemSuipian> itemSuipianVec;
	vector<ShenbingSuipian> shenBingSuipianVec;

	SuiPianManager(void);


public:
	static SuiPianManager* getManager();

	// 初始化
	void initItemSuiPianData(vector<ItemSuipianInfo> itemSuipianVec);
	void initShenBingSuiPianData(vector<ShenbingSuipianInfo> shenBingSuipianVec);

	// 添加一个碎片数据：给指定类型和Id的碎片添加指定的个数
	void addOneItemSuiPian(ItemSuipianInfo item);
	void addOneShenBingSuiPian(ShenbingSuipianInfo item);

	// 移除一个碎片数据：给指定类型和Id的碎片移除指定的个数
	void subOneItemSuiPian(ItemSuipianInfo item);
	void subOneShenBingSuiPian(ShenbingSuipianInfo item);

	// 添加type类型的Item碎片的个数，是这个类型的都会添加，从id = 1到id = MAX
	// toAdd小于0就是减
	void addItemSuiPianNumByItemType(ItemsType type,int toAdd);
	// 添加神兵碎片的个数，从id1到idMAX
	void addShenBingSuiPianNumByZBGroupId(unsigned int zb_groupId,int toAdd);

	// 获取某类碎片的数据,效率比较低
	// 建议用重载的函数
	bool getShenBingSuiPianNumByGroupId(unsigned int groupId,ShenbingSuipianGroupData& outData);
	bool getItemSuiPianNumByType(unsigned int itemType,ItemSuipianGroupData& outData);
	bool getShenBingSuiPianNumByGroupId(unsigned int groupId,const vector<ShenbingSuipianGroupData>& dataPool,ShenbingSuipianGroupData& outData);
	bool getItemSuiPianNumByType(unsigned int itemType,const vector<ItemSuipianGroupData>& dataPool,ItemSuipianGroupData& outData);

	// 获取所有的碎片：更方便的方法-->见getAllItemSuiPianWithGrouped/getAllShenbingSuiPianWithGrouped
	vector<ItemSuipian> getAllItemSuiPian(){return itemSuipianVec;};
	vector<ShenbingSuipian> getAllShenBingSuiPian(){return shenBingSuipianVec;};

	// 有组织的碎片数据
	vector<ItemSuipianGroupData> getAllItemSuiPianWithGrouped();
	vector<ShenbingSuipianGroupData> getAllShenbingSuiPianWithGrouped();


	~SuiPianManager(void);
};

