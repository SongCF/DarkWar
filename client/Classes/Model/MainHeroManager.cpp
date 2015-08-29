#include "MainHeroManager.h"

MainHeroManager* MainHeroManager::manager = new MainHeroManager();

MainHeroManager::MainHeroManager(void)
{
}

MainHeroManager* MainHeroManager::getManager()
{
	return manager;
}

HeroInfo MainHeroManager::getHeroByHeroId(unsigned int heroId)
{
	int num = mInfor.size();
	for (int i=0;i<num;i++)
	{
		if (heroId == mInfor[i].hero_id)
		{
			return mInfor[i];
		}
	}

	CCAssert(false,"not Found");
	HeroInfo h;
	h.hero_id = heroId;
	h.level = 1;
	h.vip_level = 1;
	h.name[0] = 0;
	h.profession_id = Profession_Yemanren;
	return h;
}

MainHeroManager::~MainHeroManager(void)
{
}
