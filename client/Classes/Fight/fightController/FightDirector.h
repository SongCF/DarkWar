#pragma once

#include "FightScriptDefine.h"

// 作战的导演
// 根据剧本的指定，要求角色就演就是了。

class FightDirector
{
private:
	bool inited;
	FightScript mScript;
public:
	FightDirector(void);
	// 初始化脚本,会拆分脚本，把premove移出来。
	void initScript(FightScript script);
	// 是否初始化了
	bool isInited(){return inited;};
	// 获得棋盘初始化数据
	InitChessBoard getChessBoardInitData(){return mScript.init;};

	// 获得整个脚本
	FightScript getScript(){return mScript;};

	// 获取Step,成功返回true
	// 没有了，或者越界了就false
	bool getStep(int stepIndex,OneStep& out_step);

	int getStepNum(){return mScript.stepVec.size();};

	~FightDirector(void);
};

