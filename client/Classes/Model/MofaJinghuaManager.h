#pragma once
#include "protocol.h"
#include <vector>
using namespace std;

class MofaJinghuaManager
{
private:
	static MofaJinghuaManager* manager;

	vector<MofaJinghua> allMoFaJingHua;
	MofaJinghuaManager(void);

public:
	static MofaJinghuaManager* getManager();
	void init(vector<MofaJinghua> datas);

	vector<MofaJinghua> getAllMoFaJingHua();

	void addOneJingHua(MofaJinghua jinghua);
	void removeOneJingHua(MofaJinghua jinghua);

	~MofaJinghuaManager(void);
};

