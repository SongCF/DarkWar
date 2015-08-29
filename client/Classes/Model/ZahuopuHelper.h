#pragma once

#include "protocol.h"
#include <vector>

using namespace std;

class ZahuopuHelper
{
private:
	static ZahuopuHelper* helper;
	ZahuopuHelper(void);

	vector<ZahuopuItem> itemList;
	 unsigned int silver_need_for_refresh;
	 unsigned int draw_need_coupons;

	 // 移除一个杂货，返回当前移除的这个
	 ZahuopuItem removeOneZaHuo(int index);
public:
	static ZahuopuHelper* getHelper();
	void setCurrentZaHuoList(vector<ZahuopuItem> items);
	vector<ZahuopuItem> getCurrentZaHuiList();

	// 获取某个位置的Item
	ZahuopuItem getItemByIndex(int index);

	// 设置已经被获取过了
	void setIsGettedByIndex(int index);

	// 获取还没有领取的个数
	int getNumOfNotGetted();

	// 刷新的钱
	unsigned int getRefreshNeedSilver();
	void setRefreshNeedSilver(unsigned int silver);

	// 抽杂货铺需要的代购卷
	unsigned int getPickItemNeedDaiGouJuan();
	void setPickItemNeedDaiGouJuan(unsigned int need);

	~ZahuopuHelper(void);
};

