#include "SpriteHelper.h"

using namespace std;

string SpriteHelper::splicingNum_String(const char* format,int num)
{
	char temp[128];
	sprintf(temp,format,num);
	return temp;
}

void SpriteHelper::pauseSubNodeAndChildrenInParentNodeByTag(CCNode* parent,int tag)
{
	CCArray* children = parent->getChildren();
	int num = parent->getChildrenCount();
	CCNode* child = NULL;
	for (int i=0;i<num;i++)
	{
		child = ((CCNode*)children->objectAtIndex(i));
		if (child->getTag() == tag)
			pauseNodeAndChildren(child);
		child = NULL;
	}
}

void SpriteHelper::pauseNodeAndChildren(CCNode* node)
{
	if (node == NULL)
		return;
	
	node->pauseSchedulerAndActions();

	CCArray* children = node->getChildren();
	int num = node->getChildrenCount();
	CCNode* child = NULL;
	for (int i=0;i<num;i++)
	{
		child = ((CCNode*)children->objectAtIndex(i));
		child->pauseSchedulerAndActions();
		child = NULL;
	}
}

void SpriteHelper::resumeNodeAndChildren(CCNode* node)
{
	if (node == NULL)
		return;
	node->resumeSchedulerAndActions();

	CCArray* children = node->getChildren();
	int num = node->getChildrenCount();
	CCNode* child = NULL;
	for (int i=0;i<num;i++)
	{
		child = ((CCNode*)children->objectAtIndex(i));
		child->resumeSchedulerAndActions();
		child = NULL;
	}
}
void SpriteHelper::resumeSubNodeAndChildrenInParentNodeByTag(CCNode* parent,int tag)
{
	CCArray* children = parent->getChildren();
	int num = parent->getChildrenCount();
	CCNode* child = NULL;
	for (int i=0;i<num;i++)
	{
		child = ((CCNode*)children->objectAtIndex(i));
		if (child->getTag() == tag)
			resumeNodeAndChildren(child);
		child = NULL;
	}
}

CCAction* SpriteHelper::playFrameAction(CCSprite* sprite,std::string fileNames[],int length,float delay,bool isRepeat /* = true */)
{
	CCArray* frames = CCArray::create();
	//创建3张动画
	for(int i=0;i<length;i++)
	{
		//const char* name = fileNames[i].c_str();
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage(fileNames[i].c_str());
		CCSize texSize = texture->getContentSize();
		CCRect texRec = CCRectMake(0,0,texSize.width,texSize.height);
		CCSpriteFrame* frame = CCSpriteFrame::createWithTexture(texture,texRec);
		frames->addObject(frame);
	}
	//定义的一种动画，要能动还得用Animate包裹才可以，最终要变成Action，那么再包裹一下;
	CCAnimation* anim = CCAnimation::create(frames,delay);
	CCAnimate* animate = CCAnimate::create(anim);
	if (isRepeat)
	{
		CCAction* repeat = CCRepeatForever::create(animate);
		sprite->runAction(repeat);

		return repeat;
	}else
	{
		sprite->runAction(animate);

		return animate;
	}
	
}


//参数讲解;
//sprite：要播放动画的精灵;
//pList:纹理图册的.plist文件;
//imageFileName:纹理的图册的大图片;
//fileNames：纹理图册制作时的每一帧的文件名;
//length：有多少个帧，即fileNames的元素个数;
//delay:帧之间的延迟;
//
void SpriteHelper::playFrameAction(CCSprite* sprite,const char* pList,const char* imageFileName,std::string fileNames[],int length,float delay)
{
	//加载纹理图册;
	CCSpriteFrameCache* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	frameCache->addSpriteFramesWithFile(pList);
	sprite->initWithSpriteFrameName(imageFileName);
	//添加纹理帧动画;
	CCArray* frames = CCArray::create();
	for(int i=0;i<length;i++)
	{
		const char* name = fileNames[i].c_str();
		CCSpriteFrame* frame = frameCache->spriteFrameByName(name);
		frames->addObject(frame);
	}
	//定义的一种动画，要能动还得用Animate包裹才可以，最终要变成Action，那么再包裹一下;
	CCAnimation* anim = CCAnimation::create(frames,delay);
	CCAnimate* animate = CCAnimate::create(anim);
	CCAction* repeat = CCRepeatForever::create(animate);
	sprite->runAction(repeat);
}

void SpriteHelper::moveWithScale(CCNode* target,CCPoint endPos,float endScale,float time,float delayBefore /* = 0.0f */)
{
    CCMoveTo* moveTo = CCMoveTo::create(time, endPos);
    CCScaleTo* scaleTo = CCScaleTo::create(time, endScale);
    
    if (delayBefore > 0.0f) {
        CCFiniteTimeAction* moveDelay = CCSequence::create(CCDelayTime::create(delayBefore),moveTo,NULL);
        CCFiniteTimeAction* scaleDelay = CCSequence::create(CCDelayTime::create(delayBefore),scaleTo,NULL);
        
        target->runAction(moveDelay);
        target->runAction(scaleDelay);
        
//        target->runAction(CCRepeatForever::create((CCActionInterval*)moveDelay));
//        target->runAction(CCRepeatForever::create( (CCActionInterval*)scaleDelay));
    }else{
        target->runAction(moveTo);
        target->runAction(scaleTo);
    }
}

// 不管是对低清，还是高清，其帧的名字都用本来的名字。
// 打纹理时，每一个frame该有-ipadhd还是要有。
// 2.1
CCFiniteTimeAction* SpriteHelper::createAction_pList(std::string fileNames[],int length,float delay,bool isRepeat /* = true */)
{
	CCSpriteFrameCache* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//添加纹理帧动画;
	CCArray* frames = CCArray::create();
	for(int i=0;i<length;i++)
	{
		const char* name = NULL;
		if (CC_CONTENT_SCALE_FACTOR() == 2)	// 高清
		{
			name = replace_all_distinct(fileNames[i],".png","-ipadhd.png").c_str();
		}else
		{
			name = fileNames[i].c_str();
		}
		CCSpriteFrame* frame = frameCache->spriteFrameByName(name);

#if (COCOS2D_VERSION == 0x00020000)
		frames->addObject(frame);
#elif (COCOS2D_VERSION == 0x00020100)
		CCAnimationFrame* animFrame = new CCAnimationFrame();
		animFrame->initWithSpriteFrame(frame, 1, NULL);
		frames->addObject(animFrame);
		animFrame->release();
#else
		CCAnimationFrame* animFrame = new CCAnimationFrame();
		animFrame->initWithSpriteFrame(frame, 1, NULL);
		frames->addObject(animFrame);
		animFrame->release();
#pragma message("SpriteHelper::createAction_pList : COCOS2D_VERSION not Test!!") 
#endif

	}
	//定义的一种动画，要能动还得用Animate包裹才可以，最终要变成Action，那么再包裹一下;
	CCAnimation* anim = CCAnimation::create(frames,delay);
	CCAnimate* animate = CCAnimate::create(anim);
	if (isRepeat)
	{
		return CCRepeat::create(animate, -1);;
	}else
	{
		return (CCFiniteTimeAction*)animate;
	}
}


CCAction* SpriteHelper::createAction(std::string fileNames[],int length,float delay,bool isRepeat /* = true */)
{
	CCArray* frames = CCArray::create();
	for(int i=0;i<length;i++)
	{
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage(fileNames[i].c_str());
		if (texture == NULL) {
			CCLog("SpriteHelper::createAction can not find the file : %s",fileNames[i].c_str());
			return NULL;
		}
		CCSize texSize = texture->getContentSize();
		CCRect texRec = CCRectMake(0,0,texSize.width,texSize.height);
		CCSpriteFrame* frame = CCSpriteFrame::createWithTexture(texture,texRec);
		frames->addObject(frame);
	}

#if (COCOS2D_VERSION == 0x00020000)
	CCAnimation* anim = CCAnimation::create(frames,delay);
#elif (COCOS2D_VERSION == 0x00020100)
	CCAnimation* anim = CCAnimation::createWithSpriteFrames(frames,delay);
#else
	CCAnimation* anim = CCAnimation::createWithSpriteFrames(frames,delay);
	#pragma message("SpriteHelper::createAction : COCOS2D_VERSION not Test!!") 
#endif

	CCAnimate* animate = CCAnimate::create(anim);
	if (isRepeat)
	{
		CCAction* repeat = CCRepeat::create(animate, -1);
		return repeat;
	}else
	{
		return animate;
	}
}

CCFiniteTimeAction* SpriteHelper::createFadeBetweenAction(int start, int end, float timeCost, int repeatTimes)
{
    CCFiniteTimeAction* fadeBetween = CCSequence::create(
                                                         CCFadeTo::create(timeCost, start),
                                                         CCFadeTo::create(timeCost, end),NULL);
    CCRepeat* re = CCRepeat::create(fadeBetween, repeatTimes);
    
    return re;
}
CCFiniteTimeAction* SpriteHelper::createFadeBetweenAction(int start,int end,float timeCost,float delay ,int repeatTimes)
{
	CCFiniteTimeAction* fadeBetween = CCSequence::create(
		CCFadeTo::create(timeCost, start),
		CCFadeTo::create(timeCost, end),
		CCDelayTime::create(delay),NULL);
	CCRepeat* re = CCRepeat::create(fadeBetween, repeatTimes);

	return re;
}

bool SpriteHelper::isCross(CCRect rect1,CCRect rect2)
{
	return rect1.intersectsRect(rect2);
	//return CCRect::CCRectIntersectsRect(rect1,rect2);
}

bool SpriteHelper::pointInBorder(IBorder *border, cocos2d::CCPoint borderPoint)
{
    list<CCRect> rects = border->getBounds();
    
    list<CCRect>::iterator it;
    
    for(it = rects.begin();it!=rects.end();it++)
    {
		if( (*it).containsPoint(borderPoint))
		{
			return true;
		}
        //if( CCRect::CCRectContainsPoint((*it), borderPoint) )
        //{
        //    return true;
        //}
    }
    
    return false;
}

bool SpriteHelper::isCross(IBorder* border1,CCNode* parent1,IBorder* border2,CCNode* parent2)
{
	std::list<CCRect> bounds1 = border1->getBounds();
	std::list<CCRect> bounds2 = border2->getBounds();

	std::list<CCRect>::iterator it_1;
	std::list<CCRect>::iterator it_2;

	for (it_1=bounds1.begin(); it_1!=bounds1.end(); ++it_1) 
	{
		for (it_2=bounds2.begin(); it_2!=bounds2.end(); ++it_2) 
		{
			CCRect rect1 = (*it_1);
			CCRect rect2 = (*it_2);
			rect1.origin = parent1->convertToWorldSpace(rect1.origin);
			rect2.origin = parent2->convertToWorldSpace(rect2.origin);
			//if ( CCRect::CCRectIntersectsRect(rect1,rect2) )
			if ( rect1.intersectsRect(rect2) )
			{//如果有一个相交了，那么就是相交了
				return true;
			}
		}
	}

	return false;

}

bool SpriteHelper::isCross(CCSprite* sprite1,CCSprite* sprite2)
{
	if (sprite1 == NULL || sprite2 == NULL)
	{
		return false;
	}
	float width1 = sprite1->boundingBox().size.width;
	float height1 = sprite1->boundingBox().size.height;
	float width2 = sprite2->boundingBox().size.width;
	float height2 = sprite2->boundingBox().size.height;

	/*float width1 = sprite1->getContentSize().height;
	float height1 = sprite1->getContentSize().width;
	float width2 = sprite2->getContentSize().height;
	float height2 = sprite2->getContentSize().width;*/
	
	CCPoint pos1 = sprite1->getPosition();
	CCPoint pos2 = sprite2->getPosition();

	//如果有父物体，那么就需要装换成世界坐标。把自己所在的点转换为世界坐标，放在一个坐标系下面来判断;
	if (sprite1->getParent()!=NULL)
	{
		pos1 = sprite1->getParent()->convertToWorldSpace(sprite1->getPosition());
	}
	if (sprite2->getParent()!=NULL)
	{
		pos2 = sprite2->getParent()->convertToWorldSpace(sprite2->getPosition());
	}

	float posX1 =  pos1.x;
	float posY1 =  pos1.y;
	float posX2 =  pos2.x;
	float posY2 =  pos2.y;

	if(abs(posX1-posX2)<(width1+width2)*0.5 && abs(posY1-posY2)<(height1+height2)*0.5)
	{//中心距小于边长，就相交
		return true;
	}

	return false;
}

//把tartget放到左下方，那么得到的具体位置，用于计算把物体如果放到左下方应该setPosition时传入的值
CCPoint SpriteHelper::getLeftBottomPos(CCNode* target,CCSize screenSize,float padingLeft,float padingBottom)
{
	//CCPoint anch = target->getAnchorPoint();
	float posX = screenSize.width - target->getContentSize().width*0.5 - padingLeft;
	//float posY = screenSize.height - target->getContentSize().height*0.5 - padingBottom;
	float posY = target->getContentSize().height*0.5 + padingBottom;
	return ccp(posX,posY);
}

CCAction* SpriteHelper::createAction(CCFiniteTimeAction* action,SEL_CallFuncO finishCallBack, CCObject* param)
{
	CCCallFuncO* fun = CCCallFuncO::create(NULL,finishCallBack,param);
	CCAction* seq = CCSequence::create(action,fun,NULL);
	return seq;
}

CCFiniteTimeAction* SpriteHelper::createAction(CCFiniteTimeAction* action,CCObject* target,SEL_CallFuncN finishCallBack)
{
	CCCallFuncN* fun = CCCallFuncN::create(target,finishCallBack);
	CCFiniteTimeAction* seq = CCSequence::create(action,fun,NULL);
	return seq;
}


CCFiniteTimeAction* SpriteHelper::easyCreateAction(const char * format,int startNum,int totalNum, float delay,CCObject* param,SEL_CallFuncO callBack)
{
	CCFiniteTimeAction* framAct = easyCreateAction(format, startNum, totalNum, delay,false);
	CCFiniteTimeAction* action = (CCFiniteTimeAction*)createAction(framAct,callBack,param);
	return action;
}
CCFiniteTimeAction* SpriteHelper::easyCreateAction(const char * format,int startNum,int totalNum, float delay,CCObject* target,SEL_CallFuncN callBack)
{
	CCFiniteTimeAction* framAct = easyCreateAction(format, startNum, totalNum, delay,false);
	CCFiniteTimeAction* action = createAction(framAct,target,callBack);
	return action;
}

CCFiniteTimeAction* SpriteHelper::easyCreateAction_pList(const char * format,int startNum,int totalNum, float delay,CCObject* target,SEL_CallFuncN callBack)
{
	CCFiniteTimeAction* action = easyCreateAction_pList(format,startNum,totalNum,delay,false);
	return createAction(action,target,callBack);
}

CCFiniteTimeAction* SpriteHelper::easyCreateAction(const char * format,int startNum,int totalNum, float delay,bool repeat /* = false */)
{
	int index = startNum;
	string* files = new string[totalNum];
	for (int i=0; i < totalNum; i++) {
		char name[128];
		sprintf(name, format,index);index++;
		files[i] = name;
	}
	CCFiniteTimeAction* action = (CCFiniteTimeAction*)createAction(files,totalNum,delay,repeat);
	delete [] files;

	return action;
}

CCFiniteTimeAction* SpriteHelper::easyCreateAction_pList(const char * format,int startNum,int totalNum, float delay,bool repeat /* = false */)
{
	int index = startNum;
	string* files = new string[totalNum];
	for (int i=0; i < totalNum; i++) {
		char name[128];
		sprintf(name, format,index);index++;
		files[i] = name;
	}
	CCFiniteTimeAction* action = createAction_pList(files,totalNum,delay,repeat);
	delete [] files;

	return action;
}

CCLabelTTF* SpriteHelper::createLabelTTF(const char* lable,CCNode* toAdd/* = NULL*/,float size /*= 40*/)
{
	CCLabelTTF* lableTTF = CCLabelTTF::create(lable,FRONT,size);
	if (toAdd!=NULL)
		toAdd->addChild(lableTTF);

	return lableTTF;
}
CCLabelTTF* SpriteHelper::createLabelTTF(int lable,CCNode* toAdd/* = NULL*/,float size/* = 40*/)
{
	char str[128];
	sprintf(str,"%d",lable);
	CCLabelTTF* lableTTF = CCLabelTTF::create(str,FRONT,size);
	if (toAdd!=NULL)
		toAdd->addChild(lableTTF);

	return lableTTF;
}

long SpriteHelper::millisecondNow()
{
	struct cc_timeval now; 
	CCTime::gettimeofdayCocos2d(&now, NULL); 
	return (now.tv_sec * 1000 + now.tv_usec / 1000); 

	//struct tm *t;
	//t = localtime(&now.tv_sec);
	//int nowYear = t->tm_year+1900 ,nowMonth = t->tm_mon+1,nowDay = t->tm_mday;
	//char time[100];
	//sprintf(time,"%d-%d-%d",nowYear,nowMonth,nowDay);

}