#include "ZahuopuHelper.h"

ZahuopuHelper* ZahuopuHelper::helper = NULL;

ZahuopuHelper::ZahuopuHelper(void)
{
	silver_need_for_refresh = 0xffffffff;
}
ZahuopuHelper* ZahuopuHelper::getHelper()
{
	if (helper == NULL)
	{
		helper = new ZahuopuHelper();
	}
	return helper;
}
void ZahuopuHelper::setCurrentZaHuoList(vector<ZahuopuItem> items)
{
	itemList = items;
}
vector<ZahuopuItem> ZahuopuHelper::getCurrentZaHuiList()
{
	return itemList;
}

// 获取某个位置的Item
ZahuopuItem ZahuopuHelper::getItemByIndex(int index)
{
	CCAssert(index >= 0 && index < itemList.size(),"");
	return itemList[index];
}

// 设置已经被获取过了
void ZahuopuHelper::setIsGettedByIndex(int index)
{
	CCAssert(index >= 0 && index < itemList.size(),"");

	itemList[index].already_got = true;
}

int ZahuopuHelper::getNumOfNotGetted()
{
	int ret = 0;
	for (int i=0,count=itemList.size(); i<count; i++)
	{
		if (!itemList[i].already_got)
		{
			ret ++;
		}
	}

	return ret;
}

// 刷新的钱
unsigned int ZahuopuHelper::getRefreshNeedSilver()
{
	return silver_need_for_refresh;
}
void ZahuopuHelper::setRefreshNeedSilver(unsigned int silver)
{
	silver_need_for_refresh = silver;
}

unsigned int ZahuopuHelper::getPickItemNeedDaiGouJuan()
{
	return draw_need_coupons;
}
void ZahuopuHelper::setPickItemNeedDaiGouJuan(unsigned int need)
{
	draw_need_coupons = need;
}

ZahuopuItem ZahuopuHelper::removeOneZaHuo(int index)
{
	CCAssert(index >=0 && index < itemList.size(),"");

	// 获取
	ZahuopuItem ret = itemList[index];

	// 移除
	for (vector<ZahuopuItem>::iterator it = itemList.begin(); it!=itemList.end();it++,index--)
	{
		if (index == 0)
		{
			itemList.erase(it);
			break;
		}
	}

	return ret;
}

ZahuopuHelper::~ZahuopuHelper(void)
{
}
