#pragma once

#include "BaseSprite.h"
#include "protocol.h"
#include "flat.h"
//ui常用的一些全局函数

namespace FX_CommonFunc
{
	//材料所需 的一个board   --- 如 教堂升级所需的材料
	BaseSprite * getNeedMaterialBoard(ItemsType type, unsigned int needCount, bool& isHave);

	// 1返回 一     2返回 二   (0-10)
	string getStrByInt(unsigned int inter);

	//通过attre获取属性描述文字
	string getAttrbuteItemString(AttrbuteItem& attr, int need_idx);
	string GetMoFaShuXing_ByZBAttr(ZBAttr attr, bool isPercent=false); //精炼的地方 全是百分比

	//掉落查询  与  工匠锻造   刷新装备详情。
	void convertFubenDiaoluoZBInfo2DuanzaoGroupZBInfo(Flat_FubenDiaoluoZBInfo &_in, Flat_DuanzaoGroupZBInfo &_out);
	void refreshEquipBoard(BaseSprite* board, Flat_DuanzaoGroupZBInfo zbInfo);


	//武器、 护甲、 戒指、、、、
	string getPosName_ByZhuangBeiType(ZhuangbeiType type);

	//通过教堂的level来获得教堂的 图片idx
	int getJiaoTangFileIdxByLevel(unsigned int lv);

	//强化level排序   由好到差的排序、使用熔金石的特殊处理在回退时
	void sortQianghuaDataAtHuiTui(QianghuaData &qianghuaData);

	//该node执行action，所有的child也执行action
	void nodeAndAllChildRunAction(CCNode* node, CCAction* action);
	//exceptNodeAndChilds ---> node和它的child都不执行动作
	//exceptNodeWithoutChilds ---> node不执行，但它的child要执行
	//exceptWhoesChilds ---> 指定node的child不执行动作
	void nodeAndAllChildRunAction(CCNode* node, CCAction* action, 
		CCArray* exceptNodeAndChilds, CCArray* exceptNodeWithoutChilds, CCArray* exceptWhoesChilds);

	// colorOrOpacity : 1/color   2/opacity   3/both
	void setAllChildrenColorOrOpacity(CCNode* node, ccColor3B color, GLubyte opacity, int colorOrOpacity);

	//材料颜色获取
	const ccColor3B& getCaiLiaoColor(unsigned int itemType);
}