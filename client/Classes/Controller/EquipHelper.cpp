#include "EquipHelper.h"
#include "MyselfManager.h"
#include "EquipFallManger.h"
#include "CS_Common.h"
#include "cocos2d.h"
#include "EquipmentManager.h"

using namespace cocos2d;


EquipHelper* EquipHelper::_equipHelper = 0;

EquipHelper::EquipHelper()
{

}

EquipHelper* EquipHelper::getHelper()
{
	if (! _equipHelper) _equipHelper = new EquipHelper;
	return _equipHelper;
}

pair<unsigned int, Flat_FubenDiaoluoZBInfo> EquipHelper::getOneBetterEquip(unsigned int myHeroId, unsigned int taskId)
{
	pair<unsigned int, Flat_FubenDiaoluoZBInfo> ret;

	//actor equip
	BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(myHeroId);
	CCAssert(actor != NULL, "获取我当前的助手，必须不为空");
	vector<Equipment*> equipVec = actor->getEquipments();
	//befor 5 task fall equip
	vector<Flat_FubenDiaoluoZBInfo> fallEquipVec;
	for (int id=taskId-1; id>0 && taskId<=id+5; --id)
	{
		const vector<Flat_FubenDiaoluoZBInfo>* p = EquipFallManager::getManager()->getTaskEquipFall(id);
		fallEquipVec.insert(fallEquipVec.end(), p->begin(), p->end());
	}
	//need decription equip pos
	vector<EquipRequirePlace> posVec;
	posVec.push_back(POS_ZhuWuQi);
	posVec.push_back(POS_XiongJia);
	posVec.push_back(POS_FuShou);
	posVec.push_back(POS_Hushenfu);
	posVec.push_back(POS_JieZhi1); //只用判断一个戒指就行了
	//一 、查看每一个位置
	for (int i=0; i<posVec.size(); ++i)
	{
		EquipRequirePlace curPos = posVec.at(i);
		bool has = false;
		//二 、每个位置上是否已有装备
		for (int j=0,count=equipVec.size(); j<count; ++j)
		{
			Equipment* eq = equipVec.at(j);
			if (eq->getCurrentPlace() == curPos){ //当前装备了该位置的装备
				has = true;
				//传奇肯定不用再提示该槽位了
				if (eq->getEquipColor() > ZhuangbeiColour_Xiyou) break;
				//稀有 如果是当前能打到的最好的 也不用提示了
				else if (eq->getEquipColor() == ZhuangbeiColour_Xiyou)
				{
					bool hasBetter = false;
					for (int k=0; k<fallEquipVec.size(); ++k)
					{
						if ((fallEquipVec.at(k).zhuangbei_type == eq->getType()) && //同类型
							(fallEquipVec.at(k).zhuangbei_colour == eq->getEquipColor()) && //都是稀有
							(eq->getLevel() < fallEquipVec.at(k).level)){ //等级比我得高
							hasBetter = true;
							break;
						}
					}
					if (! hasBetter) break;
				}
				//该槽位穿了装备
				//1. ------ 需要提示
				for (int k=0; k<fallEquipVec.size(); ++k)
				{
					if ((fallEquipVec.at(k).zhuangbei_type == eq->getType()) && //同类型
						((fallEquipVec.at(k).zhuangbei_colour >= eq->getEquipColor()) || //成色好 
						(fallEquipVec.at(k).zhuangbei_colour == eq->getEquipColor() && eq->getLevel() < fallEquipVec.at(k).level))){//等级高
						ret.first = EquipFallManager::getManager()->getAlbumEquipFallTaskId(fallEquipVec.at(k).group_id);
						ret.second = fallEquipVec.at(k);
						return ret;
					}
				}
			}
		}
		//该槽位没穿装备
		//2. ------ 需要提示
		if (! has)
		{
			for (int k=0; k<fallEquipVec.size(); ++k)
			{
				vector<EquipRequirePlace> vec = EquipmentManager::getManager()->getRequirdPlace((ZhuangbeiType)fallEquipVec.at(k).zhuangbei_type);
				for (int i=0; i<vec.size(); ++i)
				{
					if (vec.at(i) == curPos &&
						CS::canEquipToHero(fallEquipVec.at(k).zhuangbei_type, actor->getCurrentProperty().profession)){
						ret.first = EquipFallManager::getManager()->getAlbumEquipFallTaskId(fallEquipVec.at(k).group_id);
						ret.second = fallEquipVec.at(k);
						return ret;
					}
				}
			}
		}
	}

	ret.first = 0;
	return ret;
}