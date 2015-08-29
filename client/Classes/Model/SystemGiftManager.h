#pragma once
#include "protocol.h"

class SystemGiftManager
{
private:
	static SystemGiftManager* manager;
	vector<systemGift> gift_pool;
	SystemGiftManager(void);
public:
	static SystemGiftManager* getManager();
	void init(vector<systemGift> gift_arr);
	void removeOneGiftById(int gift_id);
	// 如果不存在，就返回false
	bool getOneGiftById(int gift_id,systemGift& outData);

	// 获取没有领取的礼物
	vector<systemGift> getAllGift();

	~SystemGiftManager(void);
};

