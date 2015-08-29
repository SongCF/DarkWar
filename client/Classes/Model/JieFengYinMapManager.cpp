#include "JieFengYinMapManager.h"
#include "flat.h"

JieFengYinMapManager* JieFengYinMapManager::_jieFengYin_Instance = 0;

JieFengYinMapManager * JieFengYinMapManager::getManager()
{
	if (! _jieFengYin_Instance)
		_jieFengYin_Instance = new JieFengYinMapManager;

	return _jieFengYin_Instance;
}

const Flat_DuanzaoGroupZBInfo* JieFengYinMapManager::getNeedEqMaterialInfo(unsigned int jieFengYinEqGroupId)
{
	map<unsigned int, Flat_DuanzaoGroupZBInfo>::iterator it = mJieFenMap.begin();

	for ( ; it != mJieFenMap.end(); ++it)
	{
		if (it->first == jieFengYinEqGroupId)
			return &(it->second);
	}
	
	return 0;
}

void JieFengYinMapManager::addOneJieFengMap(unsigned int equip_groupId, Flat_DuanzaoGroupZBInfo& info)
{
	map<unsigned int, Flat_DuanzaoGroupZBInfo>::iterator it = mJieFenMap.find(equip_groupId);
	if (it == mJieFenMap.end()) {
		mJieFenMap.insert(pair<unsigned int, Flat_DuanzaoGroupZBInfo>(equip_groupId, info));
	}
	else { //bug ?
		it->second = info;
	}
}