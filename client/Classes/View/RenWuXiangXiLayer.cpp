#include "RenWuXiangXiLayer.h"
#include "JiNeng_SelectLayer.h"


 void RenWuXiangXiLayer::menuItemClicked(CCObject* sender)
{
	int tag = ((CCMenuItemImage*)sender)->getTag();
	if(tag == Tag_Item_Back)
	{
		mFooterLayer->dismmisPopUp();
	}
	else
	{
		//JiNeng_SelectLayer *layer = JiNeng_SelectLayer::create(mActor, mFrom);
		//mFooterLayer->popUpContaintLayer(layer);
	}
};