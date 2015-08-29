#pragma once

#include "protocol.h"
#include "flat.h"

//装备掉落信息管理
//查询一件装备 在哪些关卡掉落

class EquipFallManager
{

private:
	EquipFallManager(){}
public:
	static EquipFallManager* getManager();

	//图鉴
	void resetAlbumEquipFall(const vector<OriginInfo>& vec);
	unsigned int getAlbumEquipFallTaskId(unsigned int groupId);
	const vector<OriginInfo>& getAlbumEquipFallVec();

	//副本
	void addTaskEquipFall(unsigned int taskId, Flat_QueryFubenDiaoluoRsp& fallData);
	const vector<Flat_FubenDiaoluoZBInfo>* getTaskEquipFall(unsigned int taskId);

private:
	static EquipFallManager* _equipfallManager;
	//图鉴
	vector<OriginInfo> _albumFallVec;
	//副本
	map<unsigned int ,Flat_QueryFubenDiaoluoRsp> _fubenMap;
};