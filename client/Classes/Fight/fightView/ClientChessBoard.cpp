#include "ClientChessBoard.h"
#include "include_json.h"
#include "LFFileUtiles.h"
#include "LFUtile.h"

#define  MOVE_TIME				0.35f
#define  MOVE_ACTION_TAG		122

ClientChessBoard::ClientChessBoard(void)
{
	init();
	itemArray = CCArray::createWithCapacity(4);
	itemArray->retain();
	//autoRomovePoolForContainer = CCArray::createWithCapacity(4);
	//autoRomovePoolForContainer->retain();
	mStage = NULL;
}

ClientChessBoard* ClientChessBoard::create(string configFile,CCNode* stage)
{
	ClientChessBoard* board = new ClientChessBoard();
	board->autorelease();
	board->mStage = stage;

	CCAssert(board->mStage != NULL,"");

	bool success;
	string doc = LFFileUtiles::getLFFileUtiles()->getResourceFile(configFile,success);
	CCAssert(success,"");
	bool iniSuc = board->initWithJsonFile(doc);
	CCAssert(iniSuc,"");
	return board;
}

CCPoint ClientChessBoard::getCenterWorldPoint()
{
	if (getAnchorPoint().x != 0.5f || getAnchorPoint().y != 0.5f)
	{
		CCLog("Error : %s --> getAnchorPoint is not at center, we not surport it!",__FUNCTION__);
	}
	return getParent()->convertToWorldSpace(getPosition());
}


bool ClientChessBoard::initWithJsonFile(string jsonFile)
{
	Json::Reader reader;
	Json::Value root;
	//解析不成功就返回空;
	if (!reader.parse(jsonFile, root))
	{
		CCLog("Error : ActorChessBoard::initChessBoard --> parse json error!");
		CCAssert(false,"");
		return false;
	}
	string bgImageFile  = root["chessBoardBgImageFile"].asString();
	initWithFile(bgImageFile);

	Json::Value gridPosArray = root["gridPosVec"];
	int gridNum = gridPosArray.size();

	if (gridNum != ChessBoard_X_Num*ChessBoard_Y_Num)
	{
		CCLog("Error : ActorChessBoard::initChessBoard --> gridPosVec num != chessBoard_X_Num*chessBoard_Y_Num");
		CCAssert(false,"");
		return false;
	}
	for (int i=0;i<gridNum;i++)
	{
		Json::Value onePos = gridPosArray[i]; 
		float posX = onePos["pos_x"].asDouble();
		float posY = onePos["pos_y"].asDouble();
		posVec.push_back(ccp(posX,posY));
	}

	// 自动适配棋盘内部的距离
	{
		// 根据分辨率同棋盘的前后行拉大，960-->1152之间平滑拉大60像素
		float moveDis = 60.0f * (CCDirector::sharedDirector()->getVisibleSize().height - 960)/(1152.0f-960.0f);
		if (moveDis > 60.0f)
			moveDis = 60.0f;
		if(moveDis < 0)
			moveDis = 0;

		// 方向
		moveDis *= (posVec[0].y - posVec[2].y)>0?1:-1;

		posVec[0].y += moveDis;
		posVec[1].y += moveDis;
	}
	

	return true;
}

bool ClientChessBoard::isPosCorrect(int xIndex,int yIndex)
{
	return (xIndex >=0 && xIndex < ChessBoard_X_Num) && (yIndex >=0 && yIndex < ChessBoard_Y_Num);
}
// 判断是否角色存在
bool ClientChessBoard::isActorExist(ClientChessBoardItem* item)
{
	return itemArray->indexOfObject(item) != UINT_MAX;
}

//
CCPoint ClientChessBoard::getGridPos(int xIndex,int yIndex)
{
	CCPoint worldPos = convertToWorldSpace(posVec[xIndex + yIndex * ChessBoard_X_Num]);
	return mStage->convertToNodeSpace(worldPos);
}
//
CCPoint ClientChessBoard::getGridPosInWorld(int xIndex,int yIndex)
{
	return convertToWorldSpace(posVec[xIndex + yIndex * ChessBoard_X_Num]);
}
//
ClientChessBoardItem* ClientChessBoard::getActorItemByUniqueId(unsigned int uniqueId)
{
	int num = itemArray->count();
	for (int i=0;i<num;i++)
	{
		ClientChessBoardItem* temp = (ClientChessBoardItem*)itemArray->objectAtIndex(i);
		//if (temp->getHero()->heroId == _heroId)
		if (temp->getUniqueId() == uniqueId)
		{
			return temp;
		}
	}
	return NULL;
}
//
ClientChessBoardItem* ClientChessBoard::getActorItem(int xIndex,int yIndex)
{
	int num = itemArray->count();
	for (int i=0;i<num;i++)
	{
		ClientChessBoardItem* temp = (ClientChessBoardItem*)itemArray->objectAtIndex(i);
		if (temp->getGridX() == xIndex && temp->getGridY() == yIndex)
		{
			return temp;
		}
	}
	return NULL;
}
//
// 添加一个角色到某个位置
bool ClientChessBoard::addActor(ClientChessBoardItem* actor,int xIndex,int yIndex)
{
	CCAssert(actor->getCurrentBoard() == NULL && actor->getGridX()<0 &&actor->getGridY() < 0,"");
	if (isActorExist(actor))
	{
		CCLog("Waring : ActorChessBoard::addActor --> actor is exist!");
		CCAssert(false,"");
		return false;
	}
	if (!isPosCorrect(xIndex,yIndex))
	{
		CCLog("Waring : ActorChessBoard::addActor --> pos error!");
		CCAssert(false,"");
		return false;
	}
	//if (getActorItemByUniqueId(actor->getHero()->heroId) != NULL)
	if (getActorItemByUniqueId(actor->getUniqueId()) != NULL)
	{
		CCAssert(false,"");
		return false;
	}

	ClientChessBoardItem* item = getActorItem(xIndex,yIndex);
	// 已经有一个站位了
	if ( item != NULL)
	{
		CCLog("Waring : ActorChessBoard::addActor --> there is have one in [%d,%d]",xIndex,yIndex);
		return false;
	}

	itemArray->addObject(actor);

	//// 帮助移除
	//autoRomovePoolForContainer->addObject(actor->getContainer());

	mStage->addChild(actor->getContainer(),ITEM_Z_OERDER_IN_STAGE);	// 加入指定的舞台

	// 暂用了一排，那么就放在一排的中间
	if (actor->getHero()->x_size > 1)
	{
		CCPoint left = getGridPos(0,yIndex);
		CCPoint right = getGridPos(1,yIndex);
		actor->getContainer()->setPosition(ccpMult(ccpAdd(left,right),0.5f));
	}else
	{
		actor->getContainer()->setPosition(getGridPos(xIndex,yIndex));
	}

	

	actor->setCurrentPos(xIndex,yIndex);
	actor->setCurrentBoard(this);

	return true;
}

// 把角色移动到某个位置
void ClientChessBoard::moveActor(ClientChessBoardItem* actor,int toX,int toY)
{
	CCAssert(actor->getCurrentBoard() == this,"");
	if (!isActorExist(actor))
	{
		CCLog("Waring : ActorChessBoard::moveActor --> actor is not exist!");
		return;
	}
	if (!isPosCorrect(toX,toY))
	{
		CCLog("Waring : ActorChessBoard::moveActor --> pos error!");
		return;
	}

	// 宽Item
	if(actor->getHero()->x_size > 1 && (getActorItem(0,toY)!=NULL || getActorItem(1,toY)!=NULL))
	{
		CCAssert(false,"有人了");
		return;
	}
	// 窄Item
	if (actor->getHero()->x_size <= 1 && getActorItem(toX,toY)!=NULL)
	{
		CCAssert(false,"有人了");
		return;
	}

	actor->setCurrentPos(toX,toY);

	CCPoint pos = CCPointZero;
	// 暂用了一排，那么就放在一排的中间
	if (actor->getHero()->x_size > 1)
	{
		CCPoint left = getGridPos(0,toY);
		CCPoint right = getGridPos(1,toY);
		pos = ccpMult(ccpAdd(left,right),0.5f);
	}else
	{
		pos = getGridPos(toX,toY);
	}

	CCMoveTo* move = CCMoveTo::create(MOVE_TIME,pos);
	move->setTag(MOVE_ACTION_TAG);
	actor->getContainer()->stopActionByTag(MOVE_ACTION_TAG);
	actor->getContainer()->runAction(move);
}

//
void ClientChessBoard::crossMove(ClientChessBoard* desBoard,ClientChessBoardItem* actor,int toX,int toY)
{
	if (desBoard == NULL)
	{
		moveActor(actor,toX,toY);
		return;
	}

	if (!desBoard->isPosCorrect(toX,toY))
	{
		CCAssert(false,"位置不对");
		return;
	}
	if (desBoard->isActorExist(actor))
	{
		CCAssert(false,"已经存在了");
		return;
	}
	if (desBoard->getActorItem(toX,toY))
	{
		CCAssert(false,"已经有人了");
		return;
	}
	//if (desBoard->getActorItemByUniqueId(actor->getHero()->heroId))
	if (desBoard->getActorItemByUniqueId(actor->getUniqueId()))
	{
		CCAssert(false,"Id 重复了");
		return;
	}

	// 从自己的队列里面移除
	this->itemArray->removeObject(actor);
	actor->setCurrentPos(-1,-1);
	// 加入到目的棋盘
	desBoard->itemArray->addObject(actor);
	// 更新棋盘
	actor->setCurrentBoard(desBoard);
	// 移动
	desBoard->moveActor(actor,toX,toY);
}
//
void ClientChessBoard::removeActor(int x_index,int y_index)
{
	ClientChessBoardItem* item = getActorItem(x_index,y_index);
	if (item)
	{
		removeActor(item);
	}
}

void ClientChessBoard::setAllActorVisible(bool visible)
{
	for (int i=0;i<itemArray->count();i++)
	{
		((CCNode*)itemArray->objectAtIndex(i))->setVisible(visible);
	}
}

void ClientChessBoard::removeActor(int y_index)
{
	removeActor(0,y_index);
	removeActor(1,y_index);
}

// 移除一个角色
void ClientChessBoard::removeActor(ClientChessBoardItem* actor)
{
	if (!isActorExist(actor))
	{
		CCAssert(false,"");
		return;
	}
	actor->setCurrentBoard(NULL);
	actor->setCurrentPos(-1,-1);

	// 用hide代替移除，就可以任意速度了
	actor->getContainer()->setVisible(false);
	//actor->removeFromParentAndCleanup(true);
	itemArray->removeObject(actor,true);

}



ClientChessBoard::~ClientChessBoard(void)
{
	itemArray->removeAllObjects();
	itemArray->release();

	//int num = autoRomovePoolForContainer->count();
	//for (int i=0;i<num;i++)
	//{
	//	CCNode* node = (CCNode*)autoRomovePoolForContainer->objectAtIndex(i);
	//	node->removeFromParentAndCleanup(true);
	//	CC_SAFE_RELEASE(node);
	//	CCLog("");
	//}
	//autoRomovePoolForContainer->release();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


ClientChessBoard_TeamFight::ClientChessBoard_TeamFight()
{
	items = CCArray::createWithCapacity(5);
	items->retain();
}
// 外界new的就需要外界release
void ClientChessBoard_TeamFight::addOneItem(ClientChessBoardItemBase* item)
{
	ClientChessBoardItem_TF* tf = dynamic_cast<ClientChessBoardItem_TF*>(item);
	if (tf != NULL)
	{
		tf->setCurrentBoard(this);
	}
	else
	{
		CCAssert(false,"ClientChessBoardItem_TF");
	}

	float margin_left = 60;
	float margin_right = 60;
	float margin_top = 60;
	float margin_buttom = 60;

	CCPoint ccp_uid_1 = ccp(
		mVisibleOrigin.x + item->getContentSize().width/2 + margin_left,
		mVisibleOrigin.y + mVisibleSize.height - item->getContentSize().height/2 - margin_top);
	CCPoint ccp_uid_2 = ccp(
		mVisibleOrigin.x + mVisibleSize.width - item->getContentSize().width/2 - margin_right,
		ccp_uid_1.y);
	CCPoint ccp_uid_3 = ccp(
		ccp_uid_1.x,
		mVisibleOrigin.y + item->getContentSize().height/2 + margin_buttom);
	CCPoint ccp_uid_4 = ccp(
		ccp_uid_2.x,
		ccp_uid_3.y);
	CCPoint ccp_uid_5 = ccp(
		mVisibleSize.width/2,
		mVisibleSize.height/2);
	CCPoint pos[] = {ccp_uid_1,ccp_uid_2,ccp_uid_3,ccp_uid_4,ccp_uid_5};
	int zorder[] = {0,0,2,2,  1};

	ClientChessBoardItemBase* temp = NULL;
	for (int i=0;i<items->count();i++)
	{
		temp = (ClientChessBoardItemBase*)items->objectAtIndex(i);

		if (item == temp ||
			temp->getUniqueId() == item->getUniqueId() ||
			item->getUniqueId() <= 0 || 
			item->getUniqueId() > 5)
		{
			CCAssert(false,"调试，提示");
			return;
		}
	}

	items->addObject(item);
	addChild(item->getContainer(),zorder[item->getUniqueId() - 1]);
	if (item->getUniqueId() == 5) // boss
	{
		item->getContainer()->setPosition(ccpAdd(pos[item->getUniqueId() - 1],ccp(0,item->getContentSize().height/4)));
	}else
	{
		item->getContainer()->setPosition(ccpAdd(pos[item->getUniqueId() - 1],ccp(0,0)));
	}
	
}
ClientChessBoardItemBase* ClientChessBoard_TeamFight::getOneByUId(unsigned int u_id)
{
	ClientChessBoardItemBase* ret = NULL;

	for (int i = 0; i < items->count(); i++)
	{
		ret = (ClientChessBoardItemBase*)items->objectAtIndex(i);
		if ( ret->getUniqueId() == u_id )
		{
			break;
		}

		ret = NULL;
	}

	return ret;
}
vector<ClientChessBoardItemBase*> ClientChessBoard_TeamFight::getAllItems()
{
	vector<ClientChessBoardItemBase*> ret;

	for (int i = 0; i < items->count(); i++)
	{
		ret.push_back((ClientChessBoardItemBase*)items->objectAtIndex(i));
	}

	return ret;
}

ClientChessBoard_TeamFight::~ClientChessBoard_TeamFight()
{
	items->release();
}

CCPoint ClientChessBoard_TeamFight::getCenterWorldPoint()
{
	return ccp(mVisibleSize.width/2,mVisibleSize.height/2);
}