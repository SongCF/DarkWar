#include "Equipment.h"
#include "EquipmentManager.h"
#include "CS_Common.h"
#include "SuitPool.h"
#include "ShenBingPool.h"
#include "PropertyCalculator.h"
#include <algorithm> 
#include <cmath>
#include "EquipObserver.h"
#include "ClientErrorCode.h"
#include "MyselfManager.h"
#include "ShenBingPool.h"
#include "JieFengyinMapManager.h"

Equipment::Equipment(EquipData equipData)
{
	mEquipData = equipData;	
	type = EquipmentManager::getManager()->getEquipType(mEquipData.zhuangbei_type);
	requirePlaces = EquipmentManager::getManager()->getRequirdPlace(mEquipData.zhuangbei_type);
	currentPlace = POS_NONE;
}
void Equipment::refreshData(EquipData equipData)
{
	CCAssert(mEquipData.zhuangbei_hero_id == equipData.zhuangbei_hero_id,"装的人不能变！！");
	mEquipData = equipData;	
	CCAssert(type == EquipmentManager::getManager()->getEquipType(mEquipData.zhuangbei_type),"类型不可能刷新一下就变了");
	CCAssert(requirePlaces == EquipmentManager::getManager()->getRequirdPlace(mEquipData.zhuangbei_type),"所要求的位置不可能刷新一下就变了");
	
	// 通知
	EquipObserver::getObserver()->equipmentModified(this);
}
Equipment::~Equipment()
{
	if (mEquipData.zhuangbei_hero_id != INVALID_ID)
	{
		CCLog("Waring:%s---->[Id = %lld,hero_id = %u,groupId = %d,type = %d,name = %s]!",
			__FUNCTION__,
			mEquipData.zhuangbei_id,
			mEquipData.zhuangbei_hero_id,
			mEquipData.group_id,
			mEquipData.zhuangbei_type,
			mEquipData.equipName.c_str()
			);
	}

	// 通知
	// 虽然Equipmanager里面有通知
	// 但是这里也得通知。
	// 因为对于其他临时性的装备我们也因为修改也push进了EquipObserver
	// 所以他们的销毁我们也应该通知过去
	EquipObserver::getObserver()->removeOneFromEquipManager(this);
}
void  Equipment::setActorId(unsigned int _id)
{
	mEquipData.zhuangbei_hero_id = _id;
	if (_id == INVALID_ID)
	{
		currentPlace = POS_NONE;
	}
}
//EquipRequirePlace  Equipment::getRequiredPlace()
//{
//	return requirePlace;
//}
vector<EquipRequirePlace> Equipment::getRequiredPlaces()
{
	return requirePlaces;
}
bool Equipment::canEquipToPlace(EquipRequirePlace place)
{
	return EquipmentManager::getManager()->canEquipToPlace(mEquipData.zhuangbei_type,place);
	/*int num = requirePlaces.size();
	for (int i=0;i<num;i++)
	{
	if (requirePlaces[i] == place)
	{
	return true;
	}
	}
	return false;*/
}
void Equipment::setCurrentEquipedPlace(EquipRequirePlace place)
{
	if (mEquipData.zhuangbei_hero_id > 0)
	{
		CCAssert(canEquipToPlace(place),"必须能装到这个位置");
		CCAssert(place != POS_NONE,"已经装在比如身上，怎么能把位置设置为NONE呢！");
		currentPlace = place;
	}else
	{
		CCAssert(place == POS_NONE,"这种情况只能设为NULL散");
		currentPlace = POS_NONE;
	}
	
}

EquipRequirePlace Equipment::getCurrentPlace()
{
	//CCAssert(mEquipData.zhuangbei_hero_id > 0,"必须是已经装上的");
	return currentPlace;
}

bool Equipment::isPartOfSuit()
{
	return SuitPool::getPool()->isSuit(mEquipData.group_id);
}

EquipData Equipment::getEquipData() const
{
// 	//给 QianghuaData排序  由好到差
// 	
// 	for(int i=0; i!=mEquipData.qianghua_data.qianghua_count; ++i)
// 	{
// 		for(int j=i; j!=mEquipData.qianghua_data.qianghua_count; ++j)
// 		{
// 			if(  (mEquipData.qianghua_data.info_arr[i].level < mEquipData.qianghua_data.info_arr[j].level)   
// 				|| (mEquipData.qianghua_data.info_arr[i].level == mEquipData.qianghua_data.info_arr[j].level && 
// 					mEquipData.qianghua_data.info_arr[i].percent < mEquipData.qianghua_data.info_arr[j].percent)  )
// 			{
// 				swap(mEquipData.qianghua_data.info_arr[i], mEquipData.qianghua_data.info_arr[j]);
// 			}
// 		}
// 	}

	return mEquipData;
}
unsigned int Equipment::getBaoShiNum() const
{
	unsigned int retNum = 0;
	if (mEquipData.diamond_type_1 > 0)
	{
		retNum ++;
	}
	if (mEquipData.diamond_type_2 > 0)
	{
		retNum ++;
	}
	if (mEquipData.diamond_type_3 > 0)
	{
		retNum ++;
	}
	return  retNum;
}
unsigned int  Equipment::getActorId() const
{
	return mEquipData.zhuangbei_hero_id;
}

// 得到大的类型，就是防具，饰品，攻具，不包括宝石
EquipType  Equipment::getEquipType() const
{
	return type;
}

ZhuangbeiType  Equipment::getType() const
{
	return mEquipData.zhuangbei_type;
}
ZhuangbeiColour Equipment::getEquipColor() const
{
	return mEquipData.zhuangbei_colour;
}
unsigned int Equipment::getLevel() const
{
	return mEquipData.zhuangbei_level;
}


unsigned long long  Equipment::getEquipId() const
{
	return mEquipData.zhuangbei_id;
}
unsigned int Equipment::getGroupId() const
{
	return mEquipData.group_id;
}

string Equipment::getName() const
{
	return mEquipData.equipName;
}
string Equipment::getPicFile() const
{
	return mEquipData.equipPic;
}

void Equipment::addOneQiangHuaInfor(QianghuaInfo infor)
{
	if (mEquipData.qianghua_data.qianghua_count < mEquipData.qianghua_data.max_qianghua_time)
	{
		// 存放强化信息
		mEquipData.qianghua_data.info_arr[mEquipData.qianghua_data.qianghua_count] = infor;
		mEquipData.qianghua_data.qianghua_count++;

		//通知修改了
		EquipObserver::getObserver()->equipmentModified(this);
	}
	else
	{
		CCLog("can not add QianghuaInfo,because qianghua_count >= max_qianghua_time");
		CCAssert(false,"");
	}
}

void Equipment::setNewQiangHuaData(QianghuaData data)
{
	mEquipData.qianghua_data = data;

	//通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::setNewFuMoAttribute(ZBAttr newAttri)
{
	mEquipData.fumo_attr = newAttri;
	mEquipData.has_fumo = true;

	//通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::setMinGongJiLi(unsigned int gongji_min)
{
	mEquipData.gongji_min = gongji_min;

	//通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}
void Equipment::setMaxGongJiLi(unsigned int gongji_max)
{
	mEquipData.gongji_max = gongji_max;

	//通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}
void Equipment::setHuJia(unsigned int hujia)
{
	mEquipData.hujia = hujia;

	//通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::jieFengYing()
{
	if (!mEquipData.isInFengYin)
	{
		CCLog("Waring : %s --> isInFengYin is already = false ",__FUNCTION__);
		return;
	}

	mEquipData.isInFengYin = false;

	EquipObserver::getObserver()->jieFengOneEquip(this);
}
void Equipment::addKownedHideAttribute(ZBAttr att)
{
	if (getCurrentUnkownHideAttriNum() <= 0)
	{
		CCAssert(false,"没有隐藏属性，不能添加，是不是bug");
		return;
	}
	if (mEquipData.known_hide_attr_vec.size() >= MAX_EQUIP_HIDE_ARRTIBUTE_NUM)
	{
		CCLog("Waring : %s ---> kowned attribute num >= %d",__FUNCTION__,MAX_EQUIP_HIDE_ARRTIBUTE_NUM);
		CCAssert(false,"");
		return;
	}
	mEquipData.known_hide_attr_vec.push_back(att);
	mEquipData.unknownHideAttriNum = MAX_EQUIP_HIDE_ARRTIBUTE_NUM -mEquipData.known_hide_attr_vec.size();

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);

}
void Equipment::addZhuanKeAttribute(ZBAttr att)
{
	if (mEquipData.zhuanke_attr_vec.size() > MAX_EQUIP_ZHUANKE_ARRTIBUTE_NUM )
	{
		CCLog("Waring : %s ---> zhuanke_attr_vec.size() >= %d",__FUNCTION__,MAX_EQUIP_ZHUANKE_ARRTIBUTE_NUM);
		CCAssert(false,"");
		return;
	}
	mEquipData.zhuanke_attr_vec.push_back(att);
	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}
void Equipment::setJingLianAttribute(vector<ZBAttr> att_vec,unsigned int currentJingLianTime,vector<ZBAttr> next)
{
	//CCAssert();
	//if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi && !isShenBing())
	if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Xiyou || mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
	{
		CCAssert(currentJingLianTime <= CS::getJingLianMaxTimes(mEquipData.zhuangbei_colour, mEquipData.zhuangbei_level), "");
	}
	/*
	else if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi && isShenBing())
	{
		CCAssert(currentJingLianTime <= jinglian_time_shenbing_max,"");
	}
	*/
	else
	{
		CCAssert(false,"其他的不能精炼");
		return;
	}

	JinglianData temp;
	CC_UNUSED_PARAM(temp);
	if (att_vec.size() > sizeof(temp.jinglian_attr_arr)/sizeof(ZBAttr) )
	{
		CCLog("Waring : %s ---> att_vec.size() >= %d",__FUNCTION__,2);
		CCAssert(false,"");
		return;
	}
	mEquipData.jinglian_attr_vec = att_vec;
	mEquipData.jinglian_times = currentJingLianTime;
	mEquipData.jinglian_attr_will_have_vec = next;

	if (mEquipData.jinglian_times <= 0 && mEquipData.jinglian_attr_vec.size() > 0)
	{
		CCLog("Error : %s jinglian_times <= 0 && mEquipData.jinglian_attr_vec.size() > 0 ,bug!",__FUNCTION__);
		CCAssert(false,"");
	}

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}
void Equipment::setZhuanKeAttribute(vector<ZBAttr> att_vec)
{
	string temp;
	if (!canBeZhuanKe(temp,true))
	{
		CCAssert(false,"");
		return;
	}
	if (att_vec.size() > MAX_EQUIP_ZHUANKE_ARRTIBUTE_NUM )
	{
		CCLog("Waring : %s ---> att_vec.size() >= %d",__FUNCTION__,2);
		CCAssert(false,"");
		return;
	}
	mEquipData.zhuanke_attr_vec = att_vec;

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}
void Equipment::setKownedHideAttribute(vector<ZBAttr> att_vec)
{
	if (!isShenBing())
	{
		CCLog("都不是神兵，不能调用");
		return;
	}

	if (att_vec.size() > MAX_EQUIP_HIDE_ARRTIBUTE_NUM )
	{
		CCLog("Waring : %s ---> att_vec.size() >= %d",__FUNCTION__,2);
		CCAssert(false,"");
		return;
	}
	mEquipData.known_hide_attr_vec = att_vec;
	mEquipData.unknownHideAttriNum = MAX_EQUIP_HIDE_ARRTIBUTE_NUM - mEquipData.known_hide_attr_vec.size();

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

bool Equipment::canPurify(string &infor, int holeId, bool ifAbleJustReturnTrue)
{
	char temp[1024];

	unsigned int times = CS::getPurifyMaxHoles(mEquipData.zhuangbei_type, mEquipData.zhuangbei_colour, mEquipData.zhuangbei_level);
	if(0 == times)
	{
		sprintf(temp,"equis can not to purify");
		infor = string(temp);
		return false;
	}

	if (holeId > mEquipData.purify_attr_vec.size())
	{
		sprintf(temp,"hole Id out side the max puruty num");
		infor = string(temp);
		return false;
	}

	return true;
}
void Equipment::setPurifyAttribute(vector<ZBAttr> att_vec)
{
	string temp;
	if (!canPurify(temp,true))
	{
		CCAssert(false,"");
		return;
	}
	if (att_vec.size() > CS::getPurifyMaxHoles(mEquipData.zhuangbei_type, mEquipData.zhuangbei_colour, mEquipData.zhuangbei_level))
	{
		CCLog("Waring : %s ---> att_vec.size() > PurifyMaxHoles",__FUNCTION__);
		CCAssert(false,"");
		return;
	}

	mEquipData.purify_attr_vec = att_vec;

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::holeNumAdd()
{
	mEquipData.hole ++;

	if (mEquipData.hole > 3)
	{// 简单校验下
		CCAssert(false,"");
	}

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::addOneBaoShi(ItemsType baoshiType)
{
	if (mEquipData.diamond_type_1 == 0 && mEquipData.hole >= 1 )
	{
		mEquipData.diamond_type_1 = baoshiType;
	}
	else if (mEquipData.diamond_type_2 == 0 && mEquipData.hole >= 2 )
	{
		mEquipData.diamond_type_2 = baoshiType;
	}
	else if (mEquipData.diamond_type_3 == 0 && mEquipData.hole >= 3 )
	{
		mEquipData.diamond_type_3 = baoshiType;
	}
	else
	{
		CCAssert(false,"已经添加满了！");
	}

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::clearAllbaoShi()
{
	mEquipData.diamond_type_1 = 0;
	mEquipData.diamond_type_2 = 0;
	mEquipData.diamond_type_3 = 0;

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

std::vector<std::string> Equipment::getAllMofaShuxing(bool includeAddtionall/* =true */)
{
	return EquipmentManager::getManager()->getZBShuXingString(mEquipData.getAttributeVec(includeAddtionall));
}

//获取装备固有的魔法属性描素
std::vector<std::string> Equipment::getFixedMoFaShuxing()
{
	return EquipmentManager::getManager()->getZBShuXingString(mEquipData.attr_vec);
}
//获取精炼的魔法属性
std::vector<std::string> Equipment::getJingLianMoFaShuxing()
{
	CCAssert(false, "改接口已被否决，请使用新接口");
	return EquipmentManager::getManager()->getZBShuXingString(mEquipData.jinglian_attr_vec);
}
//获取篆刻的魔法属性
std::vector<std::string> Equipment::getZhuanKeMoFaShuxing()
{
	return EquipmentManager::getManager()->getZBShuXingString(mEquipData.zhuanke_attr_vec);
}
//获取鉴定了的属性
std::vector<std::string> Equipment::getKnownHidenMoFaShuxing()
{
	return EquipmentManager::getManager()->getZBShuXingString(mEquipData.known_hide_attr_vec);
}

std::vector<Equipment::BaoshiShuxing> Equipment::getAllBaoshiShuxing()
{
	std::vector<unsigned int> baoshi_vec;
	if(mEquipData.diamond_type_1 > 0)
	{
		baoshi_vec.push_back(mEquipData.diamond_type_1);
	}
	if(mEquipData.diamond_type_2 > 0)
	{
		baoshi_vec.push_back(mEquipData.diamond_type_2);
	}
	if(mEquipData.diamond_type_3 > 0)
	{
		baoshi_vec.push_back(mEquipData.diamond_type_3);
	}

	std::vector<Equipment::BaoshiShuxing> shuxing_vec;
	for (int i=0; i<baoshi_vec.size(); ++i)
	{
		BaoshiShuxing shuxing;
		shuxing.type = baoshi_vec[i];
		shuxing.shuxing = CS::getBaoshiShuxing(baoshi_vec[i]);
		shuxing_vec.push_back(shuxing);
	}

	return shuxing_vec;
}

float Equipment::getGongjiLiAfterQianghua(bool random /* = true */)
{
	float gongji_min;
	float gongji_max;
	float gongji_min_add;
	float gongji_max_add;
	EquipCalculator ec;
	vector<EquipData> vec;
	vec.push_back(mEquipData);
	ec.getEquipGongJiLi(vec,gongji_min,gongji_max,gongji_min_add,gongji_max_add);

	gongji_min += gongji_min_add;
	gongji_max += gongji_max_add;
	if (random)
	{
		return gongji_min + (gongji_max-gongji_min)*CCRANDOM_0_1();
	}
	return (gongji_min+gongji_max)/2.0f;
}

float Equipment::getHuJiaZhiAfterQianghua()
{
	EquipCalculator ec;
	vector<EquipData> vec;
	vec.push_back(mEquipData);
	float hujia = ec.getEquipHuJiaZhi(vec);
	return hujia;
}

AttrType Equipment::getMainProType()
{
	if (getEquipType() == EquipType_WuQi)
	{
		return AttrType_gongji_min;
	}
	else if(getType() == ZhuangbeiType_KuijiaQin || getType() == ZhuangbeiType_KuijiaZhong)
	{
		return AttrType_hujia_add;
	}
	else if(getType() == ZhuangbeiType_Dunpai)
	{
		return AttrType_hujia_add;
	}
	else if(getType() == ZhuangbeiType_Faqi)
	{
		return AttrType_zhili;
	}
	else if(getType() == ZhuangbeiType_Hushenfu || getType() == ZhuangbeiType_Jiezhi )
	{
		int size = mEquipData.attr_vec.size();
		if (size == 0)
		{
			return (AttrType)0;
		}
		
		int maxType = mEquipData.attr_vec[0].attr_type;
		int maxValue = mEquipData.attr_vec[0].attr_value;
		for (int i=1;i<size;i++)
		{
			if (mEquipData.attr_vec[i].attr_value > maxValue)
			{
				maxType = mEquipData.attr_vec[i].attr_type;
				maxValue = mEquipData.attr_vec[i].attr_value;
			}
		}

		return (AttrType)maxType;
	}

	return (AttrType)0;
}

unsigned int Equipment::getPropertyAddRatioByEachJingLian()
{
	string temp;
	if (! canBeJingLian(temp))
	{
		return 0;
	}

	if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
		return 12;
	else if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Xiyou)
		return 6;

	return 0;
}

inline bool sortByPercentAsc(const QianghuaInfo& a,const QianghuaInfo& b)
{
	return a.percent < b.percent;
}
inline bool sortByPercentDesc(const QianghuaInfo& a,const QianghuaInfo& b)
{
	return a.percent > b.percent;
}

void Equipment::sortQianghuaInforByPercent(vector<QianghuaInfo>& source,int sortFlag /*= SORT_FLAG_DESC*/)
{
	if (sortFlag == SORT_FLAG_DESC)
	{
		std::sort(source.begin(),source.end(),sortByPercentDesc);
	}
	else if(sortFlag == SORT_FLAG_ASC)
	{
		std::sort(source.begin(),source.end(),sortByPercentAsc);
	}
	else
	{
		CCAssert(false,"不支持的Flag");
	}
}

bool Equipment::canFullyInheritanceQiangHuaData(const EquipData& data,vector<QianghuaInfo>& throwAway,QianghuaData& newData)
{
	newData = mEquipData.qianghua_data;

	// 检测当前装备是否本来就是可以强化的
	string infor;
	if (!canBeQianghua(infor,true))
	{
		CCLog("Infor:current equip can not be qianghua,so trowaway all!");

		for (int i=0;i<data.qianghua_data.qianghua_count;i++)
		{
			throwAway.push_back(data.qianghua_data.info_arr[i]);
		}

		return false;
	}

	vector<QianghuaInfo> tempVec;

	for (int i=0;i<mEquipData.qianghua_data.qianghua_count;i++)
	{
		tempVec.push_back(mEquipData.qianghua_data.info_arr[i]);
	}
	for (int i=0;i<data.qianghua_data.qianghua_count;i++)
	{
		tempVec.push_back(data.qianghua_data.info_arr[i]);
	}

	// 根据percent排序
	sortQianghuaInforByPercent(tempVec,SORT_FLAG_DESC);

	// 重新生成新的强化信息
	newData.qianghua_count = 0;
	int totoalNum = tempVec.size();
	for (int i=0; i<newData.max_qianghua_time && i< totoalNum; i++)
	{
		newData.info_arr[i] = tempVec[i];
		newData.qianghua_count ++;
	}

	// 保存要抛弃的属性
	if (totoalNum > newData.max_qianghua_time)
	{
		for (int i=newData.max_qianghua_time; i<totoalNum; i++)
		{
			throwAway.push_back(tempVec[i]);
		}
		return false;
	}

	return true;
}

void Equipment::clearCurrentQiangHuaData()
{
	// 清空被继承的装备的强化数据
	QianghuaData empty = mEquipData.qianghua_data;
	empty.qianghua_count = 0;
	memset(empty.info_arr,0,sizeof(empty.info_arr));
	setNewQiangHuaData(empty);

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

void Equipment::inheritanceQiangHuaData(Equipment* equip,vector<QianghuaInfo>& throwAway)
{
	const EquipData data = equip->getEquipData();

	QianghuaData newQianghuaData;
	canFullyInheritanceQiangHuaData(data,throwAway,newQianghuaData);
	setNewQiangHuaData(newQianghuaData);

	// 清空
	equip->clearCurrentQiangHuaData();

	// 通知修改了
	EquipObserver::getObserver()->equipmentModified(this);
}

int Equipment::getCurrentJingLianJieShu(){return mEquipData.jinglian_times;};
bool Equipment::haveZhuanke(){return mEquipData.zhuanke_attr_vec.size() > 0;};
int Equipment::getCurrentUnkownHideAttriNum()
{
	if (isShenBing())
	{
		return mEquipData.unknownHideAttriNum;
	}
	return 0;
};
bool Equipment::haveUnkownHidenAttribute()
{
	return getCurrentUnkownHideAttriNum() > 0;
};
bool Equipment::isShenBing(){return mEquipData.isShenBing;};
bool Equipment::isInFengYin() const {return mEquipData.isInFengYin;};

// 获取神兵解封印花费
vector<CostData> Equipment::getShenBingJieFengyinCost()
{
	vector<CostData> ret;
	if (isInFengYin() && isShenBing())
	{
		CostData silver;
		silver.item = ItemsType_Silver;
		silver.count = CS::ShenbingJiefengyinNeedSilver();

		CostData daoju;
		daoju.item = ItemsType_Shenbing_Jiefengyin;
		daoju.count = CS::ShenbingJiefengyinNeedItemCount();

		ret.push_back(silver);
		ret.push_back(daoju);
	}

	return ret;
}
// 获取神兵当前需要鉴定的属性花费
vector<CostData> Equipment::getShenBingJianDingCurrentCost()
{
	vector<CostData> ret;
	string bullshit;
	if (canBeJianDing(bullshit,false))
	{
		const int jianding_pos = mEquipData.known_hide_attr_vec.size() + 1;

		CostData silver;
		silver.item = ItemsType_Silver;
		silver.count = CS::ShenbingJiandingNeedSilver(jianding_pos);


		CostData daoju;
		daoju.item = ItemsType_Shenbing_Jianding;
		daoju.count = CS::ShenbingJiandingNeedItemCount(jianding_pos);

		ret.push_back(silver);
		ret.push_back(daoju);
	}

	return ret;
}

vector<CostData> Equipment::getShenBingZhuankeCost()
{
	vector<CostData> ret;
	string bullshit;
	if (canBeZhuanKe(bullshit,false))
	{
		CostData silver;
		silver.item = ItemsType_Silver;
		silver.count = CS::ShenbingZhuankeJingwenNeedSilver();


		CostData daoju;
		daoju.item = ItemsType_JingwenZhuanke;
		daoju.count = CS::ShenbingZhuankeJingwenNeedItem();

		ret.push_back(silver);
		ret.push_back(daoju);
	}

	return ret;
}

vector<CostData> Equipment::getShenBingLevelUpCost()
{
	vector<CostData> ret;

	CostData silver;
	silver.item = ItemsType_Silver;
	silver.count = CS::ShenbingUpgradeNeedSilver(mEquipData.zhuangbei_level);
	ret.push_back(silver);

	CostData daoju;
	daoju.item = ItemsType_Shenbing_Jianding;
	daoju.count = CS::ShenbingUpgradeNeedItemCount(mEquipData.zhuangbei_level);
	ret.push_back(daoju);

	return ret;
}

// 获取装备精炼花费
vector<CostData> Equipment::getZbJingLianCost()
{
	vector<CostData> ret;
	string bullshit;
	if (canBeJingLian(bullshit,false))
	{
		CostData silver;
		silver.item = ItemsType_Silver;
		silver.count = CS::ZBJinglianNeedSilver(getEquipColor(), mEquipData.jinglian_times + 1);
		ret.push_back(silver);
	}

	return ret;
}


int Equipment::jingLianTimeLeft()
{
	if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi && !isShenBing())
	{
		return jinglian_time_chuanqi_max - mEquipData.jinglian_times;
	}

	if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Chuanqi && isShenBing())
	{
		return jinglian_time_shenbing_max - mEquipData.jinglian_times;
	}

// 	1.10级以上的稀有装备才可精炼
// 		2.10-19级的稀有装备可精炼3次
// 		3.20-30级的稀有装备可精炼5次
	if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Xiyou && getLevel() >= 20)
	{
		return 5 - mEquipData.jinglian_times;
	}
	else if (mEquipData.zhuangbei_colour == ZhuangbeiColour_Xiyou && getLevel() >= 10)
	{
		return 3 - mEquipData.jinglian_times;
	}

	return 0;
}

bool Equipment::canBeJingLian(string & infor,bool ifAbleJustReturnTrue /* = true */)
{
	char msg[1024];
	if ((mEquipData.zhuangbei_colour != ZhuangbeiColour_Chuanqi && mEquipData.zhuangbei_colour != ZhuangbeiColour_Xiyou)
		|| (mEquipData.zhuangbei_colour == ZhuangbeiColour_Xiyou && getLevel() < 10))
	{
		sprintf(msg,"equipment must be ZhuangbeiColour_Chuanqi or ZhuangbeiColour_Xiyou(level >= 10) ");
		infor = string(msg);
		return false;
	}


	// 未解封不能精炼
	if (isInFengYin())
	{
		sprintf(msg,"equipment is in fengyin, can not jinglian");
		infor = string(msg);
		return ifAbleJustReturnTrue;
	}

	
	
	if (jingLianTimeLeft() <= 0)
	{
		sprintf(msg,"equipment's current jinglianTime = [%d] ,and jingLianTimeLeft = 0",mEquipData.jinglian_times);
		infor = string(msg);
		return ifAbleJustReturnTrue;
	}

	return true;
}
bool Equipment::canJingLianUseTheEquip(string & infor,const Equipment* equip,bool ifAbleJustReturnTrue /* = true */)
{
	char msg[1024];
	if (!canBeJingLian(infor,ifAbleJustReturnTrue))
	{
		return false;
	}

	if (equip->isInFengYin())
	{
		CCLog("Infor %s : we do not check the equip is In FengYin",__FUNCTION__);
	}

	if (equip == this)
	{
		sprintf(msg,"the cailiao equipment is self");
		infor = string(msg);
		return false;
	}
	if (equip->getActorId() != INVALID_ID)
	{
		sprintf(msg,"the cailiao equipment is weared by actor[id = %u]",equip->getActorId());
		infor = string(msg);
		return false;
	}

// 	if (getEquipColor() < ZhuangbeiColour_Chuanqi && equip->getGroupId() == getGroupId())
// 	{
// 		sprintf(msg,"the cailiao equipment's group_id != current group_id");
// 		infor = string(msg);
// 		return false;
// 	}
	if (getEquipColor() == ZhuangbeiColour_Chuanqi)
	{
		ChuanQi temp;
		if (!ChuanqiPool::getPool()->getChuanQiData(getGroupId(),temp))
		{
			CCLog("Error : %s --> equipment is chuanqi,but not found in ChuanqiPool");
			CCAssert(false,"Bug");
			return false;
		}
		// 不是传奇的分身，不能用于精炼
		if (!temp.containsGroupId(equip->getGroupId()))
		{
			sprintf(msg,"the cailiao equipment's group_id not in current chuanqi fenshen group_ids");
			infor = string(msg);
			return false;
		}
	}

	if (getEquipColor() != equip->getEquipColor())
	{
		sprintf(msg,"equipment's color is not equal");
		infor = string(msg);
		return false;
	}
	if (getType() != equip->getType())
	{
		sprintf(msg,"equipment's type is not equal");
		infor = string(msg);
		return false;
	}
	if (getEquipColor() < ZhuangbeiColour_Chuanqi && getName() != equip->getName())
	{
		sprintf(msg,"equipment's name is not equal");
		infor = string(msg);
		return false;
	}

	return true;
}

bool Equipment::canBeZhuanKe(string & infor,bool ifAbleJustReturnTrue /* = true */)
{
	// 精炼大于5阶，并且没有篆刻过，而且还要是神兵
	char msg[1024];
	if (!mEquipData.isShenBing)
	{
		sprintf(msg,"this equipmet is not isShenBing");
		infor = string(msg);
		return false;
	}

	if (isInFengYin())
	{
		sprintf(msg,"this equipmet is in fengyin");
		infor = string(msg);
		return ifAbleJustReturnTrue;
	}

	if (mEquipData.jinglian_times < JingwenZhuanke_need_JinglianLevel)
	{
		sprintf(msg,"this equipmet's jinglian level must >= %d",JingwenZhuanke_need_JinglianLevel);
		infor = string(msg);
		return ifAbleJustReturnTrue;
	}

	if (mEquipData.zhuanke_attr_vec.size() > 0)
	{
		sprintf(msg,"this equipmet is already zhuanke ed");
		infor = string(msg);
		return ifAbleJustReturnTrue;
	}

	return true;
}

bool Equipment::canBeQianghua(string & infor,bool ifAbleJustReturnTrue /* = true */)
{
	char temp[1024];
	if (!CS::canZBTypeBeQianghua(getType()))
	{
		sprintf(temp,"can not qianghua! this zhuangbei_type[=%d] is not suported!",getType());
		infor = string(temp);
		return false;
	}

	if (getLevel() < Qianghua_zhuangbei_min_level)
	{
		sprintf(temp,"can not qianghua! level must >= %d,currentLevel = %d",Qianghua_zhuangbei_min_level,getLevel());
		infor = string(temp);
		return false;
	}

	if (mEquipData.qianghua_data.max_qianghua_time == 0)
	{
		sprintf(temp,"can not qianghua! max_qianghua_time = 0");
		infor = string(temp);
		return false;
	}
	if (isInFengYin())
	{
		sprintf(temp,"can not qianghua! is in fengyin");
		infor = string(temp);
		return ifAbleJustReturnTrue;
	}

	if (mEquipData.qianghua_data.qianghua_count >= mEquipData.qianghua_data.max_qianghua_time)
	{
		sprintf(temp,"can not qianghua! this equip is max[=%d] equiped!",mEquipData.qianghua_data.max_qianghua_time);
		infor = string(temp);
		return ifAbleJustReturnTrue;
	}

	return true;
}

bool Equipment::canBeHuiTui(string& infor)
{
	if (getLevel() < Qianghua_zhuangbei_min_level)//
	{
		char temp[1024];
		sprintf(temp,"can not hui tui, the level must >= %d,currentLevel = %d",Qianghua_zhuangbei_min_level,getLevel());
		infor = string(temp);
		return false;
	}

	if (isInFengYin())
	{
		char temp[1024];
		sprintf(temp,"can not hui tui, in fengyin");
		infor = string(temp);
		return false;
	}
	if (mEquipData.qianghua_data.qianghua_count <= 0)
	{
		char temp[1024];
		sprintf(temp,"can not hui tui, because this equip have no qianghua history,may not be qianghua before!");
		infor = string(temp);
		return false;
	}

	return true;
}

bool Equipment::canBeCuiqu(string & infor,bool ifAbleJustReturnTrue /* = true */)
{
	char temp[1024];
	if (getLevel() < Cuiqu_zhuangbei_min_level)//
	{
		sprintf(temp,"can not cui qu, the level must >= %d,currentLevel = %d",Cuiqu_zhuangbei_min_level,getLevel());
		infor = string(temp);
		return false;
	}

	if (getEquipData().zhuangbei_hero_id > 0)
	{
		sprintf(temp,"this equip is used by some one ,can not be cuiqu!");
		infor = string(temp);
		return false; // 还是return ifAbleJustReturnTrue 为了兼容，还是return false
	}


	int mofa_num = mEquipData.attr_vec.size();
	for (int i=0;i<mofa_num;i++)
	{
		if (mEquipData.attr_vec[i].cuiqu_suc_ratio > 0 && CS::isAttrTypeZBTypeCanCuiqu(getType(),mEquipData.attr_vec[i].attr_type))
		{
			if (isInFengYin())
			{
				sprintf(temp,"this equip is in fengyin!");
				infor = string(temp);
				return ifAbleJustReturnTrue;
			}

			return true;
		}
	}

	// 附魔信息
	if (ifAbleJustReturnTrue && mEquipData.has_fumo && mEquipData.fumo_attr.cuiqu_suc_ratio > 0)
	{
		if (isInFengYin())
		{
			sprintf(temp,"this equip is in fengyin!");
			infor = string(temp);
			return ifAbleJustReturnTrue;
		}

		return true;
	}

	sprintf(temp,"this equip have no mofa attribute ,can not be cuiqu!");
	infor = string(temp);
	return false;
}

bool Equipment::canBeCuiquAttribute(unsigned int att_type,string & infor,bool ifAbleJustReturnTrue /* = true */)
{
	if (!canBeCuiqu(infor,ifAbleJustReturnTrue))
	{
		return false;
	}

	if (!CS::isAttrTypeZBTypeCanCuiqu(getType(),att_type))
	{
		char temp[1024];
		sprintf(temp,"zb_type = [%d] can not cuiqu attr_type = [%d]",getType(),att_type);
		infor = string(temp);
		return false;
	}

	int attri_num = mEquipData.attr_vec.size();
	for (int i=0;i<attri_num;i++)
	{
		if (mEquipData.attr_vec[i].attr_type == att_type && mEquipData.attr_vec[i].cuiqu_suc_ratio > 0)
		{
			return true;
		}
	}

	if (ifAbleJustReturnTrue && mEquipData.has_fumo && mEquipData.fumo_attr.attr_type == att_type && mEquipData.fumo_attr.cuiqu_suc_ratio > 0)
	{
		return true;
	}

	char temp[1024];
	sprintf(temp,"zb_type = [%d] can cuiqu attr_type = [%d],but this equip doesnot have the attribute!",getType(),att_type);
	infor = string(temp);
	return false;
}

bool Equipment::canBeFumo(string & infor)
{
	char temp[1024];
	if (getLevel() < Cuiqu_zhuangbei_min_level)//
	{
		sprintf(temp,"can not fuMO, the level must >= %d,currentLevel = %d",Cuiqu_zhuangbei_min_level,getLevel());
		infor = string(temp);
		return false;
	}

	if (isShenBing())
	{
		sprintf(temp,"can not fuMO, in fenyin");
		infor = string(temp);
		return false;
	}

	return true;
}

// 是否可以拿去鉴定
bool Equipment::canBeJianDing(string & infor,bool ifAbleJustReturnTrue/* = true*/)
{
	char temp[1024];
	if (!isShenBing())
	{
		sprintf(temp,"equis is not shenbing,can not jianding");
		infor = string(temp);
		return false;
	}

	if (isInFengYin())
	{
		sprintf(temp,"equis is in fengyin,can not jianding");
		infor = string(temp);
		return ifAbleJustReturnTrue;
	}

	if (!haveUnkownHidenAttribute())
	{
		sprintf(temp,"equis is have no UnkownHidenAttribute no need to jianding");
		infor = string(temp);
		return ifAbleJustReturnTrue;
	}

	return true;
}

bool Equipment::canLevelUp(int& errorCode)
{
	if (!isShenBing())
	{
		errorCode = CEC_NotShengBing;
		return false;
	}

	int nextLevel = CS::getNextShenBingLevel(mEquipData.zhuangbei_level);
	if (nextLevel < 0)
	{
		errorCode = CEC_ShengBingIsMaxLevel;
		return false;
	}


	if (mEquipData.zhuangbei_hero_id != INVALID_ID && MyselfManager::getManager()->getMyTeamActor(mEquipData.zhuangbei_hero_id)->isYuanShen())
	{
		return true;
	}


	// 等级超佩戴者的等级
	if (mEquipData.zhuangbei_hero_id != INVALID_ID && 
		nextLevel > MyselfManager::getManager()->getMyTeamActor(mEquipData.zhuangbei_hero_id)->getCurrentProperty_R()->level)
	{
		errorCode = CEC_ShengBingNextLevelIsTooHight;
		return false;
	}

	return true;
}


int Equipment::getEquipmentsCanUsedToLevelUpShengBing(vector<Equipment*>& equips)
{
	CCAssert(isShenBing(),"不是神兵就不应该调用");

	Flat_DuanzaoGroupZBInfo _out;
	if (!ChuanqiPool::getPool()->getChuanQiData(mEquipData.group_id,_out,mEquipData.zhuangbei_level))
	{
		CCAssert(false,"Bug");
		return STATE_Ok;
	}

	// 没有需要的
	if (_out.need_expend_group_id == 0)
	{
		return STATE_Not_Need_Equip;
	}

	vector<Equipment*> allNotUseEquip = EquipmentManager::getManager()->getAllEquipment(Filter_No_Equipped);
	for (int i=0,count=allNotUseEquip.size(); i<count; i++)
	{
		if (allNotUseEquip[i]->getGroupId() == _out.need_expend_group_id)
		{
			equips.push_back(allNotUseEquip[i]);
		}
	}

	return equips.empty()?STATE_Not_Found_Equip:STATE_Ok;
}

int Equipment::getEquipmentsCanUsedToJingLian(vector<Equipment*>& equips)
{
	equips.clear();
	if (mEquipData.zhuangbei_colour != ZhuangbeiColour_Chuanqi && mEquipData.zhuangbei_colour != ZhuangbeiColour_Xiyou)
		CCAssert(false, "只有传奇装备、稀有装备能精炼");

	string info;
	vector<Equipment*> allNotUseEquip = EquipmentManager::getManager()->getAllEquipment(Filter_No_Equipped);
	for (int i=0,count=allNotUseEquip.size(); i<count; i++)
	{
		if (this->canJingLianUseTheEquip(info, allNotUseEquip.at(i)))
		{
			equips.push_back(allNotUseEquip[i]);
		}
	}

	return equips.empty()?STATE_Not_Found_Equip:STATE_Ok;
}
int Equipment::getEquipmentsCanUsedToJieFeng(vector<Equipment*>& equips)
{
//	CCAssert(isInFengYin(),"没有被封印就不应该调用");

	equips.clear();
	const Flat_DuanzaoGroupZBInfo *needInfo = JieFengYinMapManager::getManager()->getNeedEqMaterialInfo(this->getGroupId());
	// 没有需要的
	if (needInfo == 0)
	{
		return STATE_Not_Need_Equip;
	}

	vector<Equipment*> allNotUseEquip = EquipmentManager::getManager()->getAllEquipment();
	for (int i=0,count=allNotUseEquip.size(); i<count; i++)
	{
		if (allNotUseEquip[i]->getGroupId() == needInfo->zb_group_id)
		{
			equips.push_back(allNotUseEquip[i]);
		}
	}

	return equips.empty()?STATE_Not_Found_Equip:STATE_Ok;
}

bool Equipment::canBeFomoAttribute(MofaJinghua jinghua,string & infor)
{
	if (!canBeFumo(infor))
	{
		return false;
	}

	if (getType() != jinghua.zb_type)
	{
		char temp[1024];
		sprintf(temp,"zb_type = [%d] can not fu_mo attr_type = [%d],this jinghua just for zbType = %d",
			getType(),jinghua.attr_type,jinghua.zb_type);
		infor = string(temp);
		return false;
	}

	if (!CS::isAttrTypeZBTypeCanCuiqu(jinghua.zb_type,jinghua.attr_type))
	{
		char temp[1024];
		sprintf(temp,"BUG:zb_type = [%d] can not fu_mo attr_type = [%d],but it is really exist!",getType(),jinghua.attr_type);
		infor = string(temp);
		return false;
	}

	return true;
}
