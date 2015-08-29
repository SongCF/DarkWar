#pragma once

#include "protocol.h"
#include "flat.h"
#include <string>
#include <vector>
using std::vector;
using std::string;


struct Email
{
	bool is_read;
	unsigned int email_id;
	EmailType type;
	unsigned int addresser_id;	//发件人id 0:为系统 >0:玩家
	string addresserName;

	string content; //邮件内容
	string summary; //内容提要

	/////////////////////////////////////////////////////////////////
	bool isHaveGift;
	//附件：赠送物品
	unsigned int vitality;		    //0:没有体力信息 >0: 补偿的体力值
	vector<GiftBag> giftVec;        //礼包
	vector<ItemInfo> itemVec;       //道具
	vector<ItemSuipianInfo> itemSuiPianVec;//道具碎片

	vector<ShenbingSuipianInfo> shenBingSuiPianVec;//装备碎片
	unsigned int random_zb_count;	//随机装备数量(领取后将会赠送随机生成的装备)
	vector<Flat_DuanzaoGroupZBInfo> equipVec; //固定准备信息


	Email()
	{
		email_id = 0;
		isHaveGift = false;
	}
	const Email& operator=(const Email& _in){
		type = _in.type;
		is_read = _in.is_read;
		email_id = _in.email_id;
		addresser_id = _in.addresser_id;	
		addresserName = _in.addresserName;
		content = _in.content;
		summary = _in.summary; 
		isHaveGift = _in.isHaveGift;
		//附件：赠送物品
		vitality = _in.vitality;		   
		giftVec = _in.giftVec;        //礼包
		itemVec = _in.itemVec;        //道具
		itemSuiPianVec = _in.itemSuiPianVec;//道具碎片
		shenBingSuiPianVec = _in.shenBingSuiPianVec;//装备碎片
		random_zb_count = _in.random_zb_count;	//随机装备数量(领取后将会赠送随机生成的装备)
		equipVec = _in.equipVec; //固定准备信息
		return *this;
	}
    
    Email(const Email& _in)
    {
        type = _in.type;
		is_read = _in.is_read;
		email_id = _in.email_id;
		addresser_id = _in.addresser_id;
		addresserName = _in.addresserName;
		content = _in.content;
		summary = _in.summary;
		isHaveGift = _in.isHaveGift;
		//附件：赠送物品
		vitality = _in.vitality;
		giftVec = _in.giftVec;        //礼包
		itemVec = _in.itemVec;        //道具
		itemSuiPianVec = _in.itemSuiPianVec;//道具碎片
		shenBingSuiPianVec = _in.shenBingSuiPianVec;//装备碎片
		random_zb_count = _in.random_zb_count;	//随机装备数量(领取后将会赠送随机生成的装备)
		equipVec = _in.equipVec; //固定准备信息
    }
    
	static unsigned int email_ntoh(Email* const email, const EmailInfo* info);
};

class EmailManager
{
	friend class CmdHelper;
private:
	EmailManager(){}
	static EmailManager* _emailManager;
	vector<Email> mEmailVec;
public:
	static EmailManager* getManager();

	//获取所有邮件
	const vector<Email>* getAllEmail();
	//查询是否有未读邮件
	bool isHaveNotReadEmail();


protected:
	//标记邮件为已读
	void setOneEmailHasRead(unsigned int email_id);
	//删除邮件
	void deleteOneEmail(unsigned int email_id);

	Email* getOneEmailById(unsigned int email_id);
	void resetEmailVector(vector<Email*>* emailVec);
	void addOneEmail(Email* em);
};