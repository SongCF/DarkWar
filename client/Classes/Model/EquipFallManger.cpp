#include "EquipFallManger.h"
#include "cocos2d.h"
#include <algorithm>

EquipFallManager* EquipFallManager::_equipfallManager = 0;

EquipFallManager* EquipFallManager::getManager()
{
	if (! _equipfallManager) _equipfallManager = new EquipFallManager;
	return _equipfallManager;
}

unsigned int EquipFallManager::getAlbumEquipFallTaskId(unsigned int groupId)
{
	for ( int i=0,count=_albumFallVec.size(); i<count; ++i)
	{
		if (_albumFallVec.at(i).group_id == groupId){
			return _albumFallVec.at(i).task_id;
		}
	}
	CCAssert(false, "");
	return 0;
}

const vector<OriginInfo>& EquipFallManager::getAlbumEquipFallVec()
{
	return _albumFallVec;
}

void EquipFallManager::resetAlbumEquipFall(const vector<OriginInfo>& vec)
{
	_albumFallVec = vec;
}


void EquipFallManager::addTaskEquipFall(unsigned int taskId, Flat_QueryFubenDiaoluoRsp& fallData)
{
	_fubenMap[taskId] = fallData;
}
const vector<Flat_FubenDiaoluoZBInfo>* EquipFallManager::getTaskEquipFall(unsigned int taskId)
{
	return &_fubenMap[taskId].diaoluo_vec;
}