#pragma once

#include "protocol.h"

// 管理主英雄的类
// 在login之后就会刷新
// 也只会Login之后刷新！！！
class MainHeroManager
{
private:
	static MainHeroManager* manager;
	MainHeroManager(void);
	vector<HeroInfo> mInfor;
public:
	static MainHeroManager* getManager();
	// login时自动处理
	void init(vector<HeroInfo> infors){mInfor = infors;};
	// 创建主英雄时自动处理
	void addOneHero(HeroInfo infor){mInfor.push_back(infor);};
	//
	HeroInfo getHeroByHeroId(unsigned int heroId);

	vector<HeroInfo> getAllMainHero(){return mInfor;};
	~MainHeroManager(void);
};

