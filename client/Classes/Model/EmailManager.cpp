#include "EmailManager.h"
#include "cocos2d.h"

using namespace cocos2d;

EmailManager* EmailManager::_emailManager = 0;

EmailManager * EmailManager::getManager()
{
	if (!_emailManager){
		_emailManager = new EmailManager;
	}

	return _emailManager;
}

const vector<Email>* EmailManager::getAllEmail()
{
	return &mEmailVec;
}
//查询是否有未读邮件
bool EmailManager::isHaveNotReadEmail()
{
	for (int i=0, count=mEmailVec.size(); i<count; ++i)
	{
		if (! mEmailVec.at(i).is_read) return true;
	}

	return false;
}
//标记邮件为已读
void EmailManager::setOneEmailHasRead(unsigned int email_id)
{
	for (int i=0, count=mEmailVec.size(); i<count; ++i)
	{
		if (mEmailVec.at(i).email_id == email_id){
			if (mEmailVec.at(i).is_read)
			CCLog("Warning: %s ---> this email has readed, current set it status again, it is an error? [id=%u]", __FUNCTION__, email_id);
			mEmailVec.at(i).is_read = true;
		}
	}
}
//删除邮件
void EmailManager::deleteOneEmail(unsigned int email_id)
{
	for (int i=0, count=mEmailVec.size(); i<count; ++i)
	{
		if (mEmailVec.at(i).email_id == email_id){
			mEmailVec.erase(mEmailVec.begin()+i);
			break;
		}
	}
}
Email* EmailManager::getOneEmailById(unsigned int email_id)
{
	for (int i=0, count=mEmailVec.size(); i<count; ++i)
	{
		if (mEmailVec.at(i).email_id == email_id){
			return &mEmailVec.at(i);
		}
	}

	return 0;
}
void EmailManager::addOneEmail(Email* em)
{
	mEmailVec.push_back(*em);
}
void EmailManager::resetEmailVector(vector<Email*>* emailVec)
{
	mEmailVec.clear();
	for (int i=0,count=emailVec->size(); i<count; ++i)
	{
		mEmailVec.push_back(*emailVec->at(i));
	}
}


extern unsigned int ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(DuanzaoGroupZBInfo* arr,unsigned int count,vector<Flat_DuanzaoGroupZBInfo>& out_data);
unsigned int Email::email_ntoh(Email* const email, const EmailInfo* info)
{
	const char *first = (char*)info;
	const char* end = (char*)info;

	email->is_read = info->is_read;
	email->email_id = ntohl(info->id);
	email->type = (EmailType)ntohl(info->type);
	email->addresser_id = ntohl(info->addresser_id);
	email->addresserName = info->addresser;
	CCLog("isRead[%s], id[%u], addrId[%u], addrName[%s]", email->is_read?"true":"false", email->email_id, email->addresser_id, email->addresserName.c_str());

	//content
	const char* pContent = info->content;
	email->content = pContent;
	//summary
	pContent += email->content.length()+1;
	email->summary = pContent;
	CCLog("content[%s], summary[%s]", email->content.c_str(), email->summary.c_str());
	//is have attach
	pContent += email->summary.length()+1;
	email->isHaveGift = *((bool*)pContent);

	//gift
	const char* pGift = pContent+sizeof(bool);
	end = pGift;

	if (email->isHaveGift)
	{
		//体力
		email->vitality = ntohl(*(unsigned int*)pGift);
		CCLog("vitality [%u]", email->vitality);
		//道具材料
		pGift += sizeof(unsigned int);
		unsigned int itemCount = ntohl(*(unsigned int*)pGift);
		CCLog("itemCount [%u]", itemCount);
		pGift += sizeof(unsigned int);
		ItemInfo *pItems = (ItemInfo*)pGift;
		for (int i=0; i<itemCount; ++i)
		{
			pItems[i].type = ntohl(pItems[i].type);
			pItems[i].count = ntohl(pItems[i].count);
			email->itemVec.push_back(pItems[i]);
			CCLog("item[%d] type=%u, count=%u", i, pItems[i].type, pItems[i].count);
//			pItems += sizeof(ItemInfo);
		}
		//道具碎片
		pGift = (const char*)&pItems[itemCount];
		unsigned int itemDebrisCount = ntohl(*(unsigned int*)pGift);
		CCLog("itemDebrisCount [%u]", itemDebrisCount);
		pGift += sizeof(unsigned int);
		ItemSuipianInfo *pItemDebrises = (ItemSuipianInfo*)pGift;
		for (int i=0; i<itemDebrisCount; ++i)
		{
			pItemDebrises[i].item_id = ntohl(pItemDebrises[i].item_id);
			pItemDebrises[i].suipian_count = ntohl(pItemDebrises[i].suipian_count);
			pItemDebrises[i].suipian_id = ntohl(pItemDebrises[i].suipian_id);
			email->itemSuiPianVec.push_back(pItemDebrises[i]);
			CCLog("itemSuiPian[%d] item_id=%u, suipian_count=%u, suipian_id=%u", i, pItemDebrises[i].item_id, pItemDebrises[i].suipian_count, pItemDebrises[i].suipian_id);
//			pItemDebrises += sizeof(ItemSuipianInfo);
		}
		//装备碎片
		pGift = (const char*)&pItemDebrises[itemDebrisCount];
		unsigned int zbDebrisCount = ntohl(*(unsigned int*)pGift);
		CCLog("zbDebrisCount [%u]", zbDebrisCount);
		pGift += sizeof(unsigned int);
		ShenbingSuipianInfo *pShenbingDebrises = (ShenbingSuipianInfo*)pGift;
		for (int i=0; i<zbDebrisCount; ++i)
		{
			pShenbingDebrises[i].suipian_count = ntohl(pShenbingDebrises[i].suipian_count);
			pShenbingDebrises[i].suipian_id = ntohl(pShenbingDebrises[i].suipian_id);
			pShenbingDebrises[i].zb_group_id = ntohl(pShenbingDebrises[i].zb_group_id);
			email->shenBingSuiPianVec.push_back(pShenbingDebrises[i]);
			CCLog("shenbingSuiPian[%d] zb_group_id=%u, suipian_id=%u, suipian_count=%u", i, pShenbingDebrises[i].zb_group_id, pShenbingDebrises[i].suipian_id, pShenbingDebrises[i].suipian_count);
//			pShenbingDebrises += sizeof(ShenbingSuipianInfo);
		}
		//礼包
		pGift = (const char*)&pShenbingDebrises[zbDebrisCount];
		unsigned int giftCount = ntohl(*(unsigned int*)pGift);
		CCLog("giftCount [%u]", giftCount);
		pGift += sizeof(unsigned int);
		GiftBag *pGiftBags = (GiftBag*)pGift;
		for (int i=0; i<giftCount; ++i)
		{
			pGiftBags[i].id = ntohl(pGiftBags[i].id);
			pGiftBags[i].count = ntohl(pGiftBags[i].count);
			email->giftVec.push_back(pGiftBags[i]);
			CCLog("giftBag[%d] id=%u, count=%u", i, pGiftBags[i].id, pGiftBags[i].count);
//			pGiftBags += sizeof(GiftBag);
		}
		//随机装备数量
		pGift = (const char*)&pGiftBags[giftCount];
		email->random_zb_count = ntohl(*(unsigned int*)pGift);
		CCLog("random_zb_count [%u]", email->random_zb_count);
		//固定装备
		pGift += sizeof(unsigned int);
		unsigned int eqCount = ntohl(*(unsigned int*)pGift);
		CCLog("eqCount [%u]", eqCount);
		pGift += sizeof(unsigned int);
		DuanzaoGroupZBInfo *pEquip = (DuanzaoGroupZBInfo*)pGift;

		end = pGift;
		end += ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(pEquip,eqCount,email->equipVec);
	}

	return end - first;
}