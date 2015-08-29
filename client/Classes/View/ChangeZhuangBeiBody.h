#pragma once

#include "MainLayerZhuangBeiBaseBody.h"
#include "IMainLayerBody.h"
#include "ZhuangbeiScanner.h"


// 更换装备的页面
class ChangeZhuangBeiBody : public MainLayerZhuangBeiBaseBody,public IMainLayerBody
{
protected:
	BaseSprite* board;
	int mActorId;
	EquipRequirePlace mPlace;
	CCMenu* menu;
	Equipment* mNeedReplacedEquip;
	Equipment* mSelectedEquip;

	int selectedIndex;
	int	m_iCurCellIndex;   //当前选中的cell索引

	TestReport mCurActorReport;

protected:
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual BaseSprite* getItemContentByIndex(unsigned int index);

	virtual void itemImageClicked(CCObject* obj);

	void menuItemClicked_displayDialog(CCObject *pSender);

	void disableAllTouchBegin();
	void disableAllTouchEnd();

private:

	void menuItemClickHandler(CCObject* sender);
public:
	// 需要知道对象的Id，和过滤type,type见Equipment的定义
	// equip!=NULL,那么就会把quip先移除。也就是替换。
	ChangeZhuangBeiBody(int actorId,EquipRequirePlace place,Equipment* equip = NULL);
	~ChangeZhuangBeiBody(void);

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return board->getContentSize();};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos(){return board->getPosition();};

	virtual void onEnter();
	void addDialog(string contextStr, int iTag_OK, int iTag_Cancel, bool bOffset = false);
};
