#include "ClientChessBoardItem.h"
#include "ImageManager.h"
#include "GameDirector.h"
#include "Music.h"
#include "BaseActorPropertyParser.h"
#include "ClientSkillProcessor.h"
#include "SpriteHelper.h"
#include "ClientFightLayer.h"
#include "font.h"
#include "SkillSprite.h"
#include "ZhuangbeiGongMingManager.h"
#include "MyselfManager.h"
#include "FX_CommonFunc.h"

#define TAG_BE_ATTACKED		1234
#define SHAKE_OFFSET		30.0f

#define TAG_BUFF_FANGYU_ADD		11111	// 防御的Buff
#define TAG_BUFF_FANGYU_SUB		11112	// 防御的Buff
#define TAG_BUFF_FANTAN			11113	// 反弹的Buff
#define TAG_BUFF_MIANYI			11114	// 守护免疫的Buff
#define TAG_BUFF_ZHONGDU		11115	// 中毒Buff
#define TAG_BUFF_GJ_ADD			11116	// 攻击的Buff
#define TAG_BUFF_GJ_SUB			11117	// 攻击的Buff
#define TAG_BUFF_XUANYUN		11118	// 眩晕

#define TAG_MASK_IN_STAGE		11233	// stage里面添加的一个Mask
#define TAG_DI_ZUO_SHADOW		11234	// 底座阴影
#define TAG_BLOOD_BG			11235	// 血条背景
#define TAG_BLOODE_MASK         11236   // 血条覆盖框
#define TAG_IMG_GONGMING        11237   // 装备共鸣
#define FULL_PHOTO_BASE			11240	// 全身像的节点

#define DEFAUL_CD_TIME	3

inline CCPoint randomPosRelativeNode(CCNode* relativeNode)
{
	return ccpSub(relativeNode->getPosition(),ccp(0,0));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 自己就是一个没有大小的节点
ClientChessBoardItemBase::ClientChessBoardItemBase(HeroBase hero,unsigned int uniqueId)
{
	this->hero = hero;
	this->defaultHP = max(hero.hp,hero.defaultHp);
	this->uniqueId = uniqueId;
	this->mBloodProgress = NULL;
	this->showShouHuDuanNum = 0;
	this->fight_env = -1;
	this->full_photo_parts = NULL;
	this->mIsShuangshou = false;
	this->hero.defaultCdTime = ( hero.defaultCdTime<=0 ? DEFAUL_CD_TIME : hero.defaultCdTime );

	showAliveImage();

	BaseSprite* gujia = getJiaZi();

	// 用骨架的图片做为外围的wrapper
	container = BaseSprite::create(gujia->getFileName());
	container->addChild(this);
	setPosition(ccp(container->getContentSize().width/2,container->getContentSize().height/2));
	container->setOpacity(0);


	// 底座阴影，有底座再加
	if (getDizuo()->getTexture() != NULL)
	{
		BaseSprite* dizuo_yinying = BaseSprite::create("zhandou_touying.png");
		addChild(dizuo_yinying,0,TAG_DI_ZUO_SHADOW);
		//dizuo_yinying->setPosition(getDizuo()->getPosition());
		dizuo_yinying->setPosition(ccpAdd(getDizuo()->getPosition(),ccp( 0,(dizuo_yinying->getContentSize().height - getDizuo()->getContentSize().height)/2) ) );

		//20140715 scf  boss不显示底座
		if (hero.x_size >= 2){
			getDizuo()->setVisible(false);
			dizuo_yinying->setVisible(false);
		}
	}
	

	// firePoint
	CCNode* fire_node = CCNode::create();
	addChild(fire_node,0,TAG_FIRE_POINT);
	fire_node->setPosition(getSkillFirePos());

	// buff
	{
		BaseSprite* gongjiAdd = BaseSprite::create("zhandou_buff_gongji_zengjia.png");
		container->addChild(gongjiAdd,0,TAG_BUFF_GJ_ADD);
		gongjiAdd->setPosition(ccp(gujia->getContentSize().width/2 - 55,gujia->getContentSize().height/2 - 75));
		gongjiAdd->setVisible(false);
//		gongjiAdd->setOpacity(0);

		BaseSprite* gongjiSub = BaseSprite::create("zhandou_buff_gongji_jiangdi.png");
		container->addChild(gongjiSub,0,TAG_BUFF_GJ_SUB);
		gongjiSub->setPosition(ccp(gujia->getContentSize().width/2 + 55,gujia->getContentSize().height/2 - 75));
		gongjiSub->setVisible(false);
//		gongjiSub->setOpacity(0);


		BaseSprite* xuanyunNode = BaseSprite::create();
		{
			BaseSprite* xuanyun1 = BaseSprite::create("zhandou_buff_xuanyun_1_1.png");
			CCFiniteTimeAction* xuanyun1Action = SpriteHelper::easyCreateAction("zhandou_buff_xuanyun_1_%d.png",1,8,0.1,true);
			BaseSprite* xuanyun2 = BaseSprite::create("zhandou_buff_xuanyun_2_1.png");
			CCFiniteTimeAction* xuanyun2Action = SpriteHelper::easyCreateAction("zhandou_buff_xuanyun_2_%d.png",1,6,0.1,true);
			BaseSprite* xuanyun3 = BaseSprite::create("zhandou_buff_xuanyun_3_1.png");
			CCFiniteTimeAction* xuanyun3Action = SpriteHelper::easyCreateAction("zhandou_buff_xuanyun_3_%d.png",1,12,0.1,true);
			xuanyun1->runAction(xuanyun1Action);
			xuanyun2->runAction(xuanyun2Action);
			xuanyun3->runAction(xuanyun3Action);
			xuanyunNode->addChild(xuanyun1);
			xuanyunNode->addChild(xuanyun2);
			xuanyunNode->addChild(xuanyun3);
		}
		xuanyunNode->setVisible(false);
		xuanyunNode->setPosition(ccp(gujia->getContentSize().width/2,gujia->getContentSize().height));
		container->addChild(xuanyunNode,0,TAG_BUFF_XUANYUN);



		BaseSprite* fangYu = BaseSprite::create("zhandou_zhuangtai_fangyu.png");
		container->addChild(fangYu,0,TAG_BUFF_FANGYU_ADD);
		fangYu->setPosition(ccp(gujia->getContentSize().width/2 - 55,gujia->getContentSize().height/2 - 75));
		fangYu->setVisible(false);
//		fangYu->setOpacity(0);


		BaseSprite* fangYuSub = BaseSprite::create("zhandou_zhuangtai_jianfang.png");
		container->addChild(fangYuSub,0,TAG_BUFF_FANGYU_SUB);
		fangYuSub->setPosition(ccp(gujia->getContentSize().width/2 + 55,gujia->getContentSize().height/2 - 75));
		fangYuSub->setVisible(false);
//		fangYuSub->setOpacity(0);

		

		BaseSprite* fantan = BaseSprite::create("zhandou_zhuangtai_fantan.png");
		container->addChild(fantan,0,TAG_BUFF_FANTAN);
		fantan->setPosition(ccp(gujia->getContentSize().width/2 + 0,gujia->getContentSize().height/2 - 75));
		fantan->setVisible(false);
//		fantan->setOpacity(0);

		BaseSprite* mianYi = BaseSprite::create("zhandou_zhuangtai_mianyi.png");
		container->addChild(mianYi,0,TAG_BUFF_MIANYI);
		mianYi->setPosition(ccp(gujia->getContentSize().width/2 + 80,gujia->getContentSize().height/2 - 75));
		mianYi->setVisible(false);
//		mianYi->setOpacity(0);

		BaseSprite* zhongdu = BaseSprite::create("zhandou_zhuangtai_zhongdu.png");
		container->addChild(zhongdu,0,TAG_BUFF_ZHONGDU);
		zhongdu->setPosition(ccp(gujia->getContentSize().width/2 - 80,gujia->getContentSize().height/2 - 75));
		zhongdu->setVisible(false);
//		zhongdu->setOpacity(0);
		
	}
	// 血条
	{
		// blood_bg y = 32       blood y = 15;
		BaseSprite* blood_bg = NULL;
		if (hero.hero_type == Type_Hero_Normal || hero.hero_type == Type_Hero_Team)
		{
			blood_bg = BaseSprite::create("bloodBar_bg_hero.png");
		}
		else
		{
			blood_bg = BaseSprite::create("bloodBar_bg.png");
		}
		
		addChild(blood_bg,0,TAG_BLOOD_BG);
		blood_bg->setPosition(ccp(gujia->getContentSize().width/2,32));

		BaseSprite* s = new BaseSprite();
		s->initWithFile(mResMgr->getPath("bloodBar.png"));
		mBloodProgress = LF::createProgressTimer(s);
		blood_bg->addChild(mBloodProgress);
		mBloodProgress->setPosition(ccp(blood_bg->getContentSize().width/2,21));
		float percent = ((float)hero.hp)/(float)defaultHP * 100;
		mBloodProgress->setPercentage(percent);
		s->release();

		BaseSprite *mask = BaseSprite::create("bloodBar_gaizi.png");
		blood_bg->addChild(mask, mBloodProgress->getZOrder()+1, TAG_BLOODE_MASK);
		mask->setPosition(mBloodProgress->getPosition());
		//mBloodProgress->setVisible(false);

		//boss 的血条单独显示
		if (hero.x_size > 1){
			blood_bg->setVisible(false);
			s->setVisible(false);
			mask->setVisible(false);
		}
	}

	// 受伤图片
	{
		BaseSprite* shouShang = BaseSprite::create();
		addChild(shouShang,0,TAG_BE_ATTACKED);
		shouShang->setPosition(ccp(shouShang->getContentSize().width/2,shouShang->getContentSize().height/2));
		shouShang->setOpacity(0);
	}
	// 监听被普通攻击的消息
	{
		CCNotificationCenter* notifiy = CCNotificationCenter::sharedNotificationCenter();
		notifiy->addObserver(this,callfuncO_selector(ClientChessBoardItemBase::hitedByNormalFight),MSG_AttackedByNormal,NULL);
		notifiy->addObserver(this,callfuncO_selector(ClientChessBoardItemBase::hitedByNormalBaoJiFight),MSG_AttackedByNormalBaoJi,NULL);
	}

	if (hero.hp <= 0)
	{
		showDeadImage();
	}
}
ClientChessBoardItemBase::~ClientChessBoardItemBase()
{
	// 移除监听
	{
		CCNotificationCenter* notifiy = CCNotificationCenter::sharedNotificationCenter();
		notifiy->removeObserver(this,MSG_AttackedByNormal);
		notifiy->removeObserver(this,MSG_AttackedByNormalBaoJi);
	}
	full_photo_parts->release();
}

vector<CCSprite*> ClientChessBoardItemBase::getItemParts(ClientChessBoardItemPart partFilter /* = Part_All */)
{
	vector<CCSprite*> ret;

	if (getDizuo() != NULL && partFilter != Part_Body)
		ret.push_back(getDizuo());

	if (getBloodBarBg() != NULL && partFilter != Part_Body)
		ret.push_back(getBloodBarBg());

	if (getChildByTag(TAG_DI_ZUO_SHADOW)!=NULL && partFilter != Part_Body)
		ret.push_back((CCSprite*)getChildByTag(TAG_DI_ZUO_SHADOW));

	if (mBloodProgress != NULL && partFilter != Part_Body)
		ret.push_back(mBloodProgress->getSprite());

	for (int i=0;i<full_photo_parts->count();i++)
	{
		CCSprite* part = (CCSprite*)full_photo_parts->objectAtIndex(i);
		if (part->getTag() == TAG_FULL_PHOTO_DIZUO && partFilter == Part_Body)
			continue;

		ret.push_back(part);
	}

	return ret;
}

bool ClientChessBoardItemBase::isShuangshou()
{
	return mIsShuangshou;
}

void ClientChessBoardItemBase::setOpacity(GLubyte opacity)
{
	BaseSprite::setOpacity(opacity);

	if (getDizuo() != NULL)
		getDizuo()->setOpacity(opacity);
	if (getBloodBarBg() != NULL){
		BaseSprite* bloodBG = (BaseSprite*)getBloodBarBg();
		if (bloodBG) bloodBG->setOpacity(opacity);
		CCSprite *bloodMask = (CCSprite*)bloodBG->getChildByTag(TAG_BLOODE_MASK);
		if (bloodMask) bloodMask->setOpacity(opacity);
		CCSprite *gongming = (CCSprite*)bloodBG->getChildByTag(TAG_IMG_GONGMING);
		if (gongming) gongming->setOpacity(opacity);

		mBloodProgress->setOpacity(opacity);
	}
	if (getChildByTag(TAG_DI_ZUO_SHADOW)!=NULL){
		((BaseSprite*)getChildByTag(TAG_DI_ZUO_SHADOW))->setOpacity(opacity);
	}

	//buff
	{
		BaseSprite* buf_gjAdd = (BaseSprite*)container->getChildByTag(TAG_BUFF_GJ_ADD);
		if (buf_gjAdd) buf_gjAdd->setOpacity(opacity);

		BaseSprite* buf_gjSub = (BaseSprite*)container->getChildByTag(TAG_BUFF_GJ_SUB);
		if (buf_gjSub) buf_gjSub->setOpacity(opacity);

		BaseSprite* buf_xuanyunNode = (BaseSprite*)container->getChildByTag(TAG_BUFF_XUANYUN);
		if (buf_xuanyunNode) {
			CCArray *array_child = buf_xuanyunNode->getChildren();
			for (int i=0,count=array_child->count(); i<count; ++i)
			{
				BaseSprite *temp = dynamic_cast<BaseSprite*>(array_child->objectAtIndex(i));
				if (temp) temp->setOpacity(opacity);
			}
		}

		BaseSprite* buf_fangYuAdd = (BaseSprite*)container->getChildByTag(TAG_BUFF_FANGYU_ADD);
		if (buf_fangYuAdd) buf_fangYuAdd->setOpacity(opacity);

		BaseSprite* buf_fangYuSub = (BaseSprite*)container->getChildByTag(TAG_BUFF_FANGYU_SUB);
		if (buf_fangYuSub) buf_fangYuSub->setOpacity(opacity);

		BaseSprite* buf_fantan = (BaseSprite*)container->getChildByTag(TAG_BUFF_FANTAN);
		if (buf_fantan) buf_fantan->setOpacity(opacity);

		BaseSprite* buf_mianyi = (BaseSprite*)container->getChildByTag(TAG_BUFF_MIANYI);
		if (buf_mianyi) buf_mianyi->setOpacity(opacity);

		BaseSprite* buf_zhongdu = (BaseSprite*)container->getChildByTag(TAG_BUFF_ZHONGDU);
		if (buf_zhongdu) buf_zhongdu->setOpacity(opacity);
	}

	//if (mBloodProgress != NULL)
	//{
	//	// CCProgressTimer的透明度设置直接设置了之后没效果
	//	// 需要刷新一下才可以，但是不能在本次tick里面刷新，因为本次的状态无法用
	//	// 需要下一帧才可以，于是就run一个动画来解决
	//	mBloodProgress->setCascadeOpacityEnabled(true);
	//	mBloodProgress->getSprite()->setOpacity(opacity);
	//	//CCAction* action = CCSequence::create(
	//	//	CCProgressTo::create(0.0f,mBloodProgress->getPercentage()),
	//	//	NULL
	//	//	);
	//	//mBloodProgress->runAction(action);
	//	mBloodProgress->updateDisplayedOpacity(opacity);
	//}
	for (int i=0;i<full_photo_parts->count();i++)
	{
		((CCSprite*)full_photo_parts->objectAtIndex(i))->setOpacity(opacity);
	}
}
void ClientChessBoardItemBase::setColor(const ccColor3B& color3)
{
	BaseSprite::setColor(color3);
	if (full_photo_parts != NULL)
	{
		for (int i=0;i<full_photo_parts->count();i++)
		{
			((CCSprite*)full_photo_parts->objectAtIndex(i))->setColor(color3);
		}
	}
}


#pragma region 战斗过程实现

void ClientChessBoardItemBase::pushFanTanData(Buff_Doing fantan)
{
	CCAssert(fantan.isFantan == true,"必须本来就是反弹数据，不要乱放");
	CCAssert(fantan.fightor_u_id == uniqueId,"必须是属于自己的反弹才可以");
	CCAssert(fantan.relativedBuffStateId == BuffState_MangCi,"目前只能处理盲刺的反弹");

	canFanTanData.push_back(fantan);
}
void ClientChessBoardItemBase::addShowShouHuDuanNum(int toAdd)
{
	showShouHuDuanNum += toAdd;
}

void ClientChessBoardItemBase::runBuffDoingEffect(vector<Buff_Doing> doingVec)
{
	// 这里还应该根据不同的buff类型，做相应的减血效果
	int num = doingVec.size();
	for (int i=0;i<num;i++)
	{
		hpAdd(doingVec[i].hp_add);
		if (doingVec[i].relativedBuffStateId == BuffState_ZhongDu)
		{

		}
	}
}


void ClientChessBoardItemBase::hpTo(int hp)
{
	float offset = hp - (float)hero.hp;
	hpAdd(offset);
}
void ClientChessBoardItemBase::hpAdd(int toAdd)
{
	if (toAdd == 0)
		return;

	if (toAdd < 0)
	{
		hpSub(-toAdd);
		return;
	}

	hero.hp += toAdd;

	// 不能大于最大值
	hero.hp > defaultHP ? hero.hp = defaultHP:true;

	runBloodChangeAction(toAdd,Add_Normal);
	refreshBloodBar(0);

	if (hero.hp > 0)
	{
		showAliveImage(); // 问题，每次加血只会会变量！因为showAliveImage每次都新加，没有移除，现在bug已改
	}
}

void ClientChessBoardItemBase::hpSub(int toSub,HPChangePattern pattern /* = Sub_Normal */)
{
	if (toSub == 0)
		return;

	if (toSub < 0)
	{
		hpAdd(-toSub);
		return;
	}
	if (hero.hp < toSub)
		hero.hp = 0;
	else
		hero.hp -= toSub;

	// 做动作
	if (fight_env == FIGHT_EVN_MAIN_LINE && hero.hero_type != Type_Hero_Normal)
	{
		runBloodChangeAction(-toSub*5,pattern);
	}
	else
	{
		runBloodChangeAction(-toSub,pattern);
	}
	

	runShouShangAction();
	refreshBloodBar(0);
	if (hero.hp == 0)
	{
		CCNotificationCenter* notifiy = CCNotificationCenter::sharedNotificationCenter();
		notifiy->postNotification(MSG_ItemDead,this);
		showDeadImage();
		refreshBuffIcon();
	}
}

BaseSprite* ClientChessBoardItemBase::getJiaZi()
{
	return (BaseSprite*)getChildByTag(TAG_FULL_PHOTO_JIAZI);
}
BaseSprite* ClientChessBoardItemBase::getDizuo()
{
	return (BaseSprite*)getChildByTag(TAG_FULL_PHOTO_DIZUO);
}
BaseSprite* ClientChessBoardItemBase::getBloodBarBg()
{
	return (BaseSprite*)getChildByTag(TAG_BLOOD_BG);
}

// 默认只是变灰，清除buff
void ClientChessBoardItemBase::showDeadImage()
{
	ccColor3B GRAY={100,100,100};
	setColor(GRAY);

	// 本来就应该一体的，所以不应该隐藏这个两个东西
	//getChildByTag(TAG_DI_ZUO_SHADOW)->setVisible(false);
	//getChildByTag(TAG_BLOOD_BG)->setVisible(false);

	// 死了buff就清空！团队的什么的都应该是
	//CCAssert(attachedBuff.size() <= 0,"死亡了是否需要清理buff");
	attachedBuff.clear();
	refreshBuffIcon();
	refreshBloodBar(0);
	canFanTanData.clear();
}
void ClientChessBoardItemBase::showAliveImage()
{
	// 全身相
	CCNode* node = ImageManager::getManager()->getFullPhotoOfActor(hero.profession_id,hero.summary_equips,false,true,hero.stage);
	
	mIsShuangshou = (node->getChildByTag(TAG_FULL_PHOTO_IS_SHUANGSHOU) != NULL);

	// 把底座，架子和装备都放在this的一级孩子节点上
	BaseSprite* dizuo = (BaseSprite*)node->getChildByTag(TAG_FULL_PHOTO_DIZUO);

	// 精英怪
	if (hero.hero_type == Type_Guaiwu_jingying)
	{
		// zhandou_guai_dizuo_jingying.png
		dizuo->initWithFile("zhandou_guai_dizuo_jingying.png");
		dizuo->setTag(TAG_FULL_PHOTO_DIZUO);
	}

	BaseSprite* jiazhi = (BaseSprite*)node->getChildByTag(TAG_FULL_PHOTO_JIAZI);

	setContentSize(jiazhi->getContentSize());

	if (full_photo_parts)
	{
		for (int i=0;i<full_photo_parts->count();i++)
		{
			((CCNode*)full_photo_parts->objectAtIndex(i))->removeFromParentAndCleanup(true);
		}
		full_photo_parts->removeAllObjects();
		full_photo_parts->release();
	}

	full_photo_parts = CCArray::create();
	full_photo_parts->addObject(dizuo);
	full_photo_parts->addObject(jiazhi);// 放入架子
	if (jiazhi->getChildrenCount() > 0)
		full_photo_parts->addObjectsFromArray(jiazhi->getChildren());
	full_photo_parts->retain();
	for (int i=0;i<full_photo_parts->count();i++)
	{
		CCNode* one_node = (CCNode*)full_photo_parts->objectAtIndex(i);
		one_node->retain();
		one_node->removeFromParentAndCleanup(true);
		one_node->setZOrder(one_node->getZOrder() - 5);	// 统一往下面减
		addChild(one_node);
		one_node->release();
		one_node->setPosition(ccp(getContentSize().width/2,getContentSize().height/2));
	}
	// 底座放最下面
	dizuo->setPosition(ccp(jiazhi->getContentSize().width/2,dizuo->getContentSize().height/2));

	if (getChildByTag(TAG_DI_ZUO_SHADOW))
		getChildByTag(TAG_DI_ZUO_SHADOW)->setVisible(true);
	if (getChildByTag(TAG_BLOOD_BG))
		getChildByTag(TAG_BLOOD_BG)->setVisible(true);
	if (mBloodProgress)
		refreshBloodBar(0);
}
void ClientChessBoardItemBase::refreshBloodBar(float delay)
{
	float percent = ((float)hero.hp)/(float)defaultHP * 100;

	CCAction* action = CCSequence::create(
		CCDelayTime::create(delay),
		CCProgressTo::create(0.1f,percent),
		NULL
		);
	mBloodProgress->runAction(action);
}

void ClientChessBoardItemBase::removeBuffById(int _id)
{
	CCLog("removeBuff-->buff_id = %d",_id);
	bool changed = false;

	// 记录移除的调用,因为实际操作中很有可能是还没有附加上buff
	{
		bool needAdd = true;
		int remove_num = remove_buff_id.size();
		for (int i=0;i<remove_num;i++)
		{
			if (remove_buff_id[i] == _id)
			{
				needAdd = false;
				break;
			}
		}
		if (needAdd)
			remove_buff_id.push_back(_id);
	}
	

	for (list<ClientGameBuff>::iterator it = attachedBuff.begin();it != attachedBuff.end();)
	{
		for (list<ClientGameBuffEffect>::iterator it_eff = it->effect_vec.begin(); it_eff != it->effect_vec.end();it_eff++)
		{
			if (it_eff->effect_id == _id)
			{
				//// 直接的属性变化
				// 客户端不在需要自己估摸了，脚本会明确告知，这里只须要add,remove即可
				//if (it_eff->persistent_hp_add != 0)
				//{
				//	CCAssert(it_eff->baoji_add == 0,"只能做血量变化的这种直接属性的修改");
				//	CCAssert(it_eff->hujia_add == 0,"只能做血量变化的这种直接属性的修改");
				//	CCAssert(it_eff->jianshang_lv == 0,"只能做血量变化的这种直接属性的修改");
				//	CCAssert(it_eff->shanbi_add == 0,"只能做血量变化的这种直接属性的修改");
				//	CCAssert(it_eff->xuanyun == false,"只能做血量变化的这种直接属性的修改");
				//	hpAdd(it_eff->persistent_hp_add);
				//}
				changed = true;
				it->effect_vec.erase(it_eff);
				break;
			}
		}// for (list<GameBuffEffect>::iterator it_eff = it->effect_vec.begin()

		if (it->effect_vec.empty())
		{
			it = attachedBuff.erase(it);
			changed = true;
			continue;
		}
		it++;
	}

	if (changed)
	{
		// 刷新buff图片的显示
		refreshBuffIcon();
	}
}
void ClientChessBoardItemBase::attachBuff(ClientGameBuff buff)
{
	CCLog("attachBuff-->buff_state = %d",buff.state);
	bool isThisStateExist = false;
	// 不可叠加再检查是否重复
	if (!buff.cumulative)
	{
		for (list<ClientGameBuff>::iterator it_buff = attachedBuff.begin();it_buff != attachedBuff.end();it_buff++)
		{
			if (it_buff->state == buff.state)
			{
				if (buff.state == BuffState_ShouHuZhe1) // 守护者加防
				{
					showShouHuDuanNum = buff.effect_vec.size();
				}

				// 更新
				it_buff->initWithBuff(buff);
				isThisStateExist = true;
				break;
			}
		}
	}

	// 新增
	if (!isThisStateExist)
	{
		if (buff.state == BuffState_ShouHuZhe1) // 守护者加防
		{
			showShouHuDuanNum += buff.effect_vec.size();
		}
		attachedBuff.push_back(buff);
	}
	
	refreshBuffIcon();

	// 如果当前要加的buff是之前都要求移除过的，那么马上再次移除
	int removedNum = remove_buff_id.size();
	for (int i=0;i<removedNum;i++)
	{
		// 加的buff是之前移除过的，报错！
		if (buff.haveEffctive(remove_buff_id[i]))
		{
			//CCAssert(false,"请确保加buff在移buff之前");
			CCLog("Error: %s,buff have already remove,but it is added now!",__FUNCTION__);
		}
	}
}

CCPoint ClientChessBoardItemBase::getNormalPos()
{
	return ccp(getContainer()->getContentSize().width/2,getContainer()->getContentSize().height/2);
}

CCPoint ClientChessBoardItemBase::getSkillFirePos()
{
	return ccp(getContentSize().width/2,getContentSize().height/2);
}
CCNode* ClientChessBoardItemBase::getFireNode()
{
	return getChildByTag(TAG_FIRE_POINT);
}
CCNode* ClientChessBoardItemBase::getCenterNode()
{
	CCNode* ret = getChildByTag(TAG_CENTER_POINT);
	if (ret == NULL)
	{
		ret = CCNode::create();
		addChild(ret,0,TAG_CENTER_POINT);
		ret->setPosition(getNormalPos());
	}
	return ret;
}

//

void ClientChessBoardItemBase::refreshBuffIcon()
{
	int xuanyun_num = 0;

	int mianyi_num = 0;	// 免疫
	int zhongdu_num = 0;

	int fantan_num = 0;	// 反弹

	int baoji_add_num = 0;
	int hp_add_num = 0;
	int shanbi_add_num = 0;
	int hujia_add_num = 0;
	int gongji_add_num = 0;

	int baoji_sub_num = 0;
	int hp_sub_num = 0;
	int shanbi_sub_num = 0;
	int hujia_sub_num = 0;
	int gongji_sub_num = 0;


	// 统计，死了就全看不见
	if (getCurrentHp() > 0)
	{
		for (list<ClientGameBuff>::iterator it = attachedBuff.begin();it != attachedBuff.end();it++)
		{
			if (it->effect_vec.empty())
				continue;
			// 客户端透明这个状态，就当没有这个状态
			if (it->state == BuffState_Controller)
				continue;

			mianyi_num += it->state == BuffState_ShouHuZhe1?1:0;
			zhongdu_num += it->state == BuffState_ZhongDu?1:0;
			xuanyun_num += (it->effect_vec.begin())->xuanyun == true?1:0;
			fantan_num += (it->effect_vec.begin())->fantan == true?1:0;
			baoji_add_num += (it->effect_vec.begin())->baoji_change_state > 0?1:0;
			hp_add_num += (it->effect_vec.begin())->hp_change_state > 0?1:0;
			shanbi_add_num += (it->effect_vec.begin())->shanbi_change_state > 0?1:0;
			hujia_add_num += (it->effect_vec.begin())->fangyu_change_state > 0?1:0;
			gongji_add_num += (it->effect_vec.begin())->gongjili_change_state > 0?1:0;

			baoji_sub_num += (it->effect_vec.begin())->baoji_change_state < 0?1:0;
			hp_sub_num += (it->effect_vec.begin())->hp_change_state < 0?1:0;
			shanbi_sub_num += (it->effect_vec.begin())->shanbi_change_state < 0?1:0;
			hujia_sub_num += (it->effect_vec.begin())->fangyu_change_state < 0?1:0;

			gongji_sub_num += (it->effect_vec.begin())->gongjili_change_state < 0?1:0;
		}
	}

	// 刷新显示
	{
		CCNode* node = container->getChildByTag(TAG_BUFF_FANGYU_ADD);
		node->runAction(CCSequence::create(
			CCDelayTime::create(hujia_add_num<=0?1.0f:0.1f),
			CCFadeTo::create(0.2f,hujia_add_num<=0?0:255),
			NULL
			));

		CCFiniteTimeAction* show_or_hide = NULL;
		{
			if (xuanyun_num > 0)
			{
				show_or_hide = CCShow::create();
			}
			else
			{
				show_or_hide = CCHide::create();
			}
		}
		container->getChildByTag(TAG_BUFF_XUANYUN)->runAction(CCSequence::create(
			//CCDelayTime::create(xuanyun_num<=0?1.0f:0.1f),
			CCDelayTime::create(xuanyun_num<=0?0.1f:0.1f),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_FANGYU_SUB)->stopAllActions();
		{
			if (hujia_sub_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_FANGYU_SUB)->runAction(CCSequence::create(
			CCDelayTime::create(hujia_sub_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,hujia_sub_num<=0?0:255),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_GJ_ADD)->stopAllActions();
		{
			if (gongji_add_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_GJ_ADD)->runAction(CCSequence::create(
			CCDelayTime::create(gongji_add_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,gongji_add_num<=0?0:255),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_GJ_SUB)->stopAllActions();
		{
			if (gongji_sub_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_GJ_SUB)->runAction(CCSequence::create(
			CCDelayTime::create(gongji_sub_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,gongji_sub_num<=0?0:255),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_FANTAN)->stopAllActions();
		{
			if (fantan_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_FANTAN)->runAction(CCSequence::create(
			CCDelayTime::create(fantan_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,fantan_num<=0?0:255),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_MIANYI)->stopAllActions();
		{
			if (mianyi_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_MIANYI)->runAction(CCSequence::create(
			CCDelayTime::create(mianyi_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,mianyi_num<=0?0:255),
			show_or_hide,
			NULL
			));

		container->getChildByTag(TAG_BUFF_ZHONGDU)->stopAllActions();
		{
			if (zhongdu_num<=0) show_or_hide = CCHide::create();
			else show_or_hide = CCShow::create();
		}
		container->getChildByTag(TAG_BUFF_ZHONGDU)->runAction(CCSequence::create(
			CCDelayTime::create(zhongdu_num<=0?1.0f:0.1f),
			//CCFadeTo::create(0.2f,zhongdu_num<=0?0:255),
			show_or_hide,
			NULL
			));

	}

}

//
void ClientChessBoardItemBase::runShouShangAction()
{
	return;

	BaseSprite* attackedImage = (BaseSprite*)/*container->*/getChildByTag(TAG_BE_ATTACKED);
	// 目前还没有配套的受伤图片，于是先用以前的代替，然后移动下位置就是了
	attackedImage->setPosition(getSkillFirePos());

	attackedImage->runAction(
		CCSequence::create(CCFadeTo::create(0.01f,255),
		CCDelayTime::create(0.2f),
		CCFadeTo::create(0.1f,0),
		NULL)
		);
}
//
void ClientChessBoardItemBase::runBloodChangeAction(int num,HPChangePattern pattern)
{
	string bloodNum = "";
	string jian = "";

	BaseSprite* baojiImage = BaseSprite::create();

	if (pattern == Add_Normal)
	{
		bloodNum = "bloodNum_shoushang_jiaxue.png";
		jian = "bloodNum_jian_jiaxue.png";
		baojiImage->initWithFile("shanghai_shengming.png");
	}
	else if ( pattern == Sub_Normal )
	{
		bloodNum = "bloodNum_shoushang.png";
		jian = "bloodNum_jian_shanghai.png";
	}
	else if (pattern == Sub_BaoJi)
	{
		bloodNum = "bloodNum_shoushang_baoji.png";
		jian = "bloodNum_jian_baoji.png";
		baojiImage->initWithFile("shanghai_baoji.png");
	}
	else
	{
		CCAssert(false,"未知的血量模式");
	}

//	baojiImage->setScale(0.8f);
	CCNode* showNode = CCNode::create();


	char str[128];
	sprintf(str,"%d",abs(num));
	CCLabelAtlas* lableNum = CCLabelAtlas::create(
		"0123456789",
		mResMgr->getPath(bloodNum).c_str(),
		60, 94,//71
		'0');
	lableNum->setString(str);
	lableNum->setAnchorPoint(ccp(0.5f,0.5f));
	lableNum->setScale(0.6f);


	BaseSprite* symbolSprite = NULL;
	if (num <= 0)
	{
		symbolSprite = BaseSprite::create(mResMgr->getPath(jian));
	}else
	{
		symbolSprite = BaseSprite::create();
	}
	showNode->addChild(lableNum);
	showNode->addChild(symbolSprite);
	showNode->addChild(baojiImage);

	float totalWith = lableNum->getContentSize().width * lableNum->getScaleX() + symbolSprite->getContentSize().width * symbolSprite->getScaleX();

	showNode->setContentSize(CCSizeMake(totalWith,1));
	showNode->setAnchorPoint(ccp(0.5f,0.5f));
	
	baojiImage->setPosition(ccp(showNode->getContentSize().width/2, baojiImage->getContentSize().height/2));
	symbolSprite->setPosition(ccp(symbolSprite->getContentSize().width/2 * symbolSprite->getScaleX(),
		-lableNum->getContentSize().height/2 * lableNum->getScaleY() + 20));
	lableNum->setPosition(symbolSprite->getPositionX()+lableNum->getContentSize().width/2 * lableNum->getScaleY(), symbolSprite->getPositionY());

	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	layer->addChild(showNode,11);
	showNode->setPosition(LF::getNativePostion(getFireNode(),layer));
	FX_CommonFunc::setAllChildrenColorOrOpacity(showNode, ccWHITE, 0, 2);
	// scale不能放在共同动画里。
	FX_CommonFunc::nodeAndAllChildRunAction(showNode, CCSequence::create(
		CCDelayTime::create(0.01f),//CCScaleTo::create(0.01f, 0.1f),
		CCSpawn::create(/*CCScaleTo::create(0.085f, 1.2f), */CCFadeTo::create(0.085f, 255), NULL),
		CCDelayTime::create(0.05f),//CCScaleTo::create(0.05f, 1),
		CCDelayTime::create(0.05f),//CCScaleTo::create(0.05f, 1.2f),
		CCDelayTime::create(0.25f),
		CCSpawn::create(CCFadeTo::create(0.085f, 0), /*CCMoveBy::create(0.085f, ccp(0, 150)),*/ NULL),
		NULL));
	showNode->runAction(CCSequence::create(
		CCScaleTo::create(0.01f, 0.1f),
		CCScaleTo::create(0.085f, 1.2f),
		CCScaleTo::create(0.05f, 1),
		CCScaleTo::create(0.05f, 1.2f),
		CCDelayTime::create(0.25f),
		CCMoveBy::create(0.085f, ccp(0, 150)),
		NULL));
}


void ClientChessBoardItemBase::light_on()
{
	//CCNode* light = getChildByTag(TAG_ITEM_LIGHT);	
	//if (light)
	//{
	//	light->runAction(CCFadeTo::create(0.1f,255));
	//}
}
void ClientChessBoardItemBase::light_off()
{
	//CCNode* light = getChildByTag(TAG_ITEM_LIGHT);	
	//if (light)
	//{
	//	light->runAction(CCFadeTo::create(0.1f,0));
	//}
}

// 攻击接口
void ClientChessBoardItemBase::attack_UseNormal(MSG_Fight* data)
{
	// 先归位
	setPosition(getNormalPos());

	Profession profID = data->fightor->getProfession();
	int targetNum = data->beAttackedVec.size();
	for (int i=0;i<targetNum;i++)
	{
		MSG_Fight* oneSingleFight = data->createOneFightUniteByIndex(i);
		oneSingleFight->retain();// 在attackUse***_Hit的时候 --> 通知了之后--->release

		if (profID == 64 || profID == 94)
			attackeAction_Normal_boss(oneSingleFight,i==0);
		else 
			attackAction_Normal(oneSingleFight,i==0); // 只第一个有动画
	}
}
void ClientChessBoardItemBase::attack_UseNormalBaoJi(MSG_Fight* data)
{
	// 先归位
	setPosition(getNormalPos());

	int targetNum = data->beAttackedVec.size();
	for (int i=0;i<targetNum;i++)
	{
		MSG_Fight* oneSingleFight = data->createOneFightUniteByIndex(i);
		oneSingleFight->retain();// 在attackUse***_Hit的时候 --> 通知了之后--->release

		attackeAction_NormalBaoJi(oneSingleFight, i == 0);
	}
}

// 攻击动画
void ClientChessBoardItemBase::attackAction_Normal(MSG_Fight* data,bool playFightorAction /* = true */)
{
	ClientChessBoardItemBase* target = data->beAttackedVec[0];

	CCPoint vec = LF::getVectorFromAToB(this,target);

	HeroFixedPro fixPro = BaseActorPropertyParser::getParser()->getExtraProerty(hero.profession_id);
	if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_MO_FA)
	{// 远程魔法的攻击方式
		CCParticleSystemQuad* par = CCParticleSystemQuad::create("jineng_jisi_pugong.plist");
		par->setAutoRemoveOnFinish(true);
		par->setScale(1.0f);

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

		// 以firePoint为发射和打击点
		CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);

		layer->addChild(par,10);
		par->setPosition(statPoint);

		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.25f;

		CCAction* action =  CCSequence::create(
			CCMoveTo::create(time,endPoint),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormal_Hit),data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		par->runAction(action);

		if (playFightorAction)
		{
			// 后坐力
			this->runAction(get_fire_shake_Action(this,target));
		}
	}
	else if (fixPro.gongJiJuLi >= 2 && fixPro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
	{// 远程物理

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_lieshou_baoji.plist");

		SkillSprite*yunQi = SkillSprite::create("jineng_lieshou_baoji1.png");
		SkillSprite* jian = SkillSprite::create("jineng_lieshou_baoji_gongjian.png");
		jian->setVisible(false);
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(jian,10);
		layer->addChild(yunQi,10);

		CCPoint vec = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());
		CCPoint moveByPoint = ccpSub(endPoint,statPoint);
		moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/3 * vec.x,target->getContentSize().height/3 * vec.y));

		float angle = LF::getRotateAngle(vec,ccp(0,1));

		jian->setRotation(angle);
		yunQi->setRotation(angle);
		jian->setPosition(statPoint);
		yunQi->setPosition(statPoint);

		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.2f;
		CCAction* actionJian = CCSequence::create(
			CCMoveBy::create(time,moveByPoint),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormal_Hit),data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		CCFiniteTimeAction* yunQiAction = SpriteHelper::easyCreateAction_pList("jineng_lieshou_baoji%d.png",1,14,0.04f,false);
		jian->runAction(CCSequence::create(
			CCDelayTime::create(0.25f),
			CCShow::create(),
			actionJian,
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		yunQi->runAction(
			CCSequence::create(
			yunQiAction,
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL));

		Music::sharedMusicPlayer()->playEffectSound("huoJian.wav");
		// 后坐力
		if (playFightorAction)
		{
			this->runAction(get_fire_shake_Action(this,target));
		}
	}
	else
	{// 近战物理

		// 跑过去打一下
		{
			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
			CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
			CCPoint vec = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());

			CCPoint moveByPoint = ccpSub(endPoint,statPoint);
			moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/2 * vec.x,target->getContentSize().height/3 * vec.y));
			
			{// 限制移动的距离
				float dis = ccpDistance(moveByPoint,CCPointZero);
				if (dis > 100)
				{
					moveByPoint.x *= 100.0f/dis;
					moveByPoint.y *= 100.0f/dis;
				}
			}

			
			float moveTime = ccpDistance(moveByPoint,CCPointZero)/380.0f * 0.2f;
			moveTime = MAX(moveTime,0.08f);
			CCAction* action =  CCSequence::create(
				CCEaseIn::create(CCMoveTo::create(moveTime,ccpAdd(getPosition(),moveByPoint)),2.0f),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormal_Hit),data),
				CCEaseOut::create(CCMoveTo::create(moveTime,getPosition()),2.0f),
				CCDelayTime::create(0.1f),
				NULL
				);

			if (playFightorAction)
			{
				this->runAction(action);
			}
		}
	}
}
void ClientChessBoardItemBase::attackeAction_NormalBaoJi(MSG_Fight* data,bool playFightorAction /* = true */)
{
	ClientChessBoardItemBase* target = data->beAttackedVec[0];

	// 暴击图片
	{
		if (playFightorAction)
		{
// 			BaseSprite* baoJI = BaseSprite::create("shanghai_baoji.png");
// 			container->addChild(baoJI);
// 			baoJI->setPosition(this->getPosition());
// 			baoJI->setOpacity(0);
// 			baoJI->runAction(CCSequence::create(
// 				CCFadeTo::create(0.1f,255),
// 				CCDelayTime::create(0.3f),
// 				CCSpawn::create(CCMoveBy::create(0.2f,ccp(0,100)),CCFadeTo::create(0.1f,100),NULL),
// 				CCCallFuncN::create(container,SEL_CallFuncN(&BaseSprite::removeNode)),
// 				NULL
// 				));
// 
// 			baoJI->setVisible(false);
			/*baoJI->setScale(0.5f);
			baoJI->runAction(CCSequence::create(
			CCFadeTo::create(0,255),
			CCSpawn::create(
			CCScaleTo::create(0.2f,1.2f),
			CCMoveBy::create(0.2f,ccp(0,20.0f)),
			NULL
			),
			CCSpawn::create(
			CCMoveBy::create(0.3f,ccp(0,60)),
			CCFadeTo::create(0.3f,50),
			NULL
			),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
			NULL
			));*/
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CCPoint vec = LF::getVectorFromAToB(this,target);
	//CCAction* color_change_big = CCSequence::create(
	//	CCSpawn::create(
	//		CCTintTo::create(0.2f,220,10,10),
	//		CCScaleTo::create(0.2f,1.2f),
	//		NULL),
	//	CCDelayTime::create(0.1f),
	//	CCSpawn::create(
	//		CCScaleTo::create(0.2f,1.0f),
	//		CCTintTo::create(0.1f,255,255,255),
	//		NULL),
	//	NULL);

	CCAction* color_change_big = CCSequence::create(
		CCTintTo::create(0.2f,220,10,10),
		CCDelayTime::create(0.1f),
		CCTintTo::create(0.1f,255,255,255),
		NULL);

	float delay_all = 0.0f;
	if (playFightorAction)
	{
		this->runAction(color_change_big);
	}

	HeroFixedPro fixPro = BaseActorPropertyParser::getParser()->getExtraProerty(hero.profession_id);
	//if (hero.profession_id == Profession_MiShu || hero.profession_id == Profession_FS_YeRenSaMan)
	if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_MO_FA)
	{
		CCParticleSystemQuad* par = CCParticleSystemQuad::create("jineng_jisi_baoji.plist");
		par->setAutoRemoveOnFinish(true);
		par->setScale(1.0f);


		{
			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
			CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
			CCPoint vec = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());
			CCPoint moveByPoint = ccpSub(endPoint,statPoint);
			moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/3 * vec.x,target->getContentSize().height/3 * vec.y));
			float angle = LF::getRotateAngle(vec,ccp(0,1));
		
			BaseSprite* head = BaseSprite::create("jineng_jisi_baoji.png");
			par->addChild(head);
			head->setRotation(angle);
		}


		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(par,10);
		par->setPosition(statPoint);

		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.1f;

		CCAction* action =  CCSequence::create(
			CCDelayTime::create(delay_all),
			CCMoveTo::create(time,endPoint),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormalBaoJi_Hit),data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		par->runAction(action);

		if (playFightorAction)
		{
			// 后坐力
			this->runAction(CCSequence::create(
				CCDelayTime::create(delay_all),
				get_fire_shake_Action(this,target,30),
				NULL));
		}
		
	}
	//else if (hero.profession_id == Profession_ShenJianShou || hero.profession_id == Profession_YC_KuLouGongJianShou)
	else if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_lieshou_pugong.plist");

		SkillSprite*yunQi = SkillSprite::create("jineng_lieshou_pugong1.png");
		SkillSprite* jian = SkillSprite::create("jineng_lieshou_pugong_gongjian.png");
		jian->setVisible(false);
		yunQi->setVisible(false);
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(jian,10);
		layer->addChild(yunQi,10);

		CCPoint vec = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());
		CCPoint moveByPoint = ccpSub(endPoint,statPoint);
		moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/3 * vec.x,target->getContentSize().height/3 * vec.y));
		float angle = LF::getRotateAngle(vec,ccp(0,1));

		jian->setRotation(angle);
		yunQi->setRotation(angle);
		jian->setPosition(statPoint);
		yunQi->setPosition(statPoint);

		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.15f;
		CCAction* actionJian = CCSequence::create(
			CCMoveBy::create(time,moveByPoint),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormalBaoJi_Hit),data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		CCFiniteTimeAction* yunQiAction = SpriteHelper::easyCreateAction_pList("jineng_lieshou_pugong%d.png",1,14,0.04f,false);
		jian->runAction(CCSequence::create(
			CCDelayTime::create(delay_all),
			CCDelayTime::create(0.25f),
			CCShow::create(),
			actionJian,
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		yunQi->runAction(CCSequence::create(
			CCDelayTime::create(delay_all),
			CCShow::create(),
			yunQiAction,
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL));

		Music::sharedMusicPlayer()->playEffectSound("huoJian.wav");

		// 后坐力
		if (playFightorAction)
		{
			this->runAction(get_fire_shake_Action(this,target,30));
		}
	}
	else
	{
		//CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.2f);
		// 甩一把刀
		{
			delay_all = 0.0f; // 等待color_change_big_2

			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
			CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
			CCPoint vec = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());
			float angle = LF::getRotateAngle(vec,ccp(0,1));

			// 跑到被攻击的人那去，途中再变大
			float angle_rotate = vec.y > 0 ? -45.0f:45.0f;
			CCPoint moveByPoint = ccpSub(endPoint,statPoint);
			moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/2 * vec.x,target->getContentSize().height/2 * vec.y));
			
			{// 限制移动的距离
				float dis = ccpDistance(moveByPoint,CCPointZero);
				if (dis > 100)
				{
					moveByPoint.x *= 100.0f/dis;
					moveByPoint.y *= 100.0f/dis;
				}
			}
			
			
			float moveTime = ccpDistance(moveByPoint,CCPointZero)/380.0f * 0.1f;
			this->stopAllActions();
			// 移动
			CCAction* action =  CCSequence::create(
				CCDelayTime::create(delay_all),
				// 变红放大
				CCSpawn::create(
					CCTintTo::create(0.2f,220,10,10),
					NULL),
				// 移动
				CCEaseIn::create(CCMoveTo::create(moveTime,ccpAdd(getPosition(),moveByPoint)),2.0f),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormalBaoJi_Hit),data),
				CCDelayTime::create(0.1f),
				// 变回去
				CCSpawn::create(
					CCEaseOut::create(CCMoveTo::create(moveTime,getPosition()),2),
					CCTintTo::create(0.1f,255,255,255),
					CCScaleTo::create(0.1f,1.0f),
				NULL),
				NULL
				);

			if (playFightorAction)
			{
				this->runAction(action);
			}
		}
	}
	return;
}

void ClientChessBoardItemBase::attackeAction_Normal_boss(MSG_Fight* data,bool playFightorAction)
{
	Profession profId = data->fightor->getProfession();
	ClientChessBoardItemBase* fightor = data->fightor;
	ClientChessBoardItemBase* target = data->beAttackedVec[0];

	CCPoint vec = LF::getVectorFromAToB(this,target);

	//fightor action
	if (playFightorAction)
	{
		CCPoint pos = fightor->getPosition();
		fightor->runAction(CCSequence::create(
			CCMoveTo::create(0.05f, ccp(pos.x + vec.x/abs(vec.x)*25, pos.y + vec.y/abs(vec.y)*25)),
			CCMoveTo::create(0.05f, pos),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::attackUseNormal_Hit),data),
			NULL));
	}

	//target action
	if (profId == 64) //第10关  魔化巨人
	{
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("skill/boss_dalinPG/boss_dalinPG.ExportJson");		
		CCArmature *armature_shoushang = CCArmature::create("boss_dalinPG");
		armature_shoushang->setPosition(target->getNormalPos());
		target->addChild(armature_shoushang, 1); // 上面打下面的情况要盖住刀光
		armature_shoushang->setScale(1 / 0.4f);
		armature_shoushang->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&ClientChessBoardItemBase::callfunc_RemoveWhenAnimateEnd));
		armature_shoushang->getAnimation()->play("shoushang");
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("skill/boss_dalinPG/boss_dalinPG.ExportJson");
	}
	else if (profId == 94) //第20关 大树
	{
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("skill/boss_dashuPG/boss_dashuPG.ExportJson");		
		CCArmature *armature_shoushang = CCArmature::create("boss_dashuPG");
		armature_shoushang->setPosition(target->getNormalPos());
		target->addChild(armature_shoushang, 1); // 上面打下面的情况要盖住刀光
		armature_shoushang->setScale(1 / 0.4f);
		armature_shoushang->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&ClientChessBoardItemBase::callfunc_RemoveWhenAnimateEnd));
		armature_shoushang->getAnimation()->play("shoushang");
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("skill/boss_dashuPG/boss_dashuPG.ExportJson");
		if (playFightorAction)//飞行
		{
			CCSize winsize = CCDirector::sharedDirector()->getWinSize();
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("skill/boss_dashuPG/boss_dashuPG.ExportJson");		
			CCArmature *armature_feixing = CCArmature::create("boss_dashuPG");
			target->addChild(armature_feixing, 1); // 上面打下面的情况要盖住刀光
			armature_feixing->setPosition(ccp(armature_feixing->getParent()->convertToNodeSpace(ccp(winsize.width/2,winsize.height)).x, target->getNormalPos().y));
			armature_feixing->setScale(1 / 0.4f);
			armature_feixing->getAnimation()->setMovementEventCallFunc(this, 
				SEL_MovementEventCallFunc(&ClientChessBoardItemBase::callfunc_RemoveWhenAnimateEnd));
			armature_feixing->getAnimation()->play("feixing");
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("skill/boss_dashuPG/boss_dashuPG.ExportJson");
		}
	}
}
void ClientChessBoardItemBase::callfunc_RemoveWhenAnimateEnd(CCArmature *ar, MovementEventType type, const char *p)
{
	if (type == COMPLETE || type == LOOP_COMPLETE) {
		ar->removeFromParent();
	}
}

// 击中目标
void ClientChessBoardItemBase::attackUseNormal_Hit(CCNode* node,void* msg_fight)
{
	MSG_Fight* data = (MSG_Fight*)msg_fight;
	CCAssert(data->beAttackedVec.size() == 1,"只能是一个被攻击在者，如果有多个，那么需要拆开调用,不然后面的release不对");

	if (data->beAttackedVec.size() > 1)
		data->keepSingle();

	// 相当于调用 hitedByNormalFight 方法
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_AttackedByNormal,data);
	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty(data->fightor->hero.profession_id);
	if (pro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
	{
		Music::sharedMusicPlayer()->playEffectSound("wuLiGongJi.wav");
	}else
	{
		Music::sharedMusicPlayer()->playEffectSound("moFaGongJi.wav");
	}

	data->release();
}
//
void ClientChessBoardItemBase::attackUseNormalBaoJi_Hit(CCNode* node,void* msg_fight)
{
	MSG_Fight* data = (MSG_Fight*)msg_fight;
	CCAssert(data->beAttackedVec.size() == 1,"只能是一个被攻击在者，如果有多个，那么需要拆开调用,不然后面的release不对");

	if (data->beAttackedVec.size() > 1)
		data->keepSingle();

	// 相当于调用 hitedByNormalBaoJiFight 方法
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_AttackedByNormalBaoJi,data);

	Music::sharedMusicPlayer()->playEffectSound("baoJi.wav");
	data->release();
}


// 被击中了：attackUseNormalBaoJi_Hit会调用这函数
// 被击中了：attackUseNormal_Hit会调用这函数
void ClientChessBoardItemBase::hitedByNormalFight(CCObject* msg_fight)
{
	MSG_Fight* data = (MSG_Fight*)msg_fight;
	CCAssert(data->fightor!=NULL,"");
	CCAssert(data->beAttackedVec.size() == 1,"只能是一个被攻击在者，如果有多个，那么需要拆开调用，不然每一个人收到通知的时候都会发现里面有自己，那么就会减血");

	// 只处理自己
	int huSub;
	bool miss;
	if ( !data->isBeAttacked(this,huSub,miss) || hero.hp == 0 || data->fightor->getHero()->hp == 0)
	{
		return;
	}

	if (miss)
	{
		missAction(this);
		return;
	}

	// 减血
	hpSub(huSub,Sub_Normal);

	// 看攻击者的数据
	HeroFixedPro fixPro = BaseActorPropertyParser::getParser()->getExtraProerty(data->fightor->hero.profession_id);
	if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_MO_FA)
	{// 远程魔法
		CCParticleSystemQuad* par = CCParticleSystemQuad::create("jineng_jisi_pugong_baozha.plist");
		par->setAutoRemoveOnFinish(true);
		par->setScale(1.4f);
		addChild(par);
		par->setPosition(getSkillFirePos());
	}
	else if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
	{// 远程物理

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_lieshou_pugong.plist");
		SkillSprite* jizhong = SkillSprite::create("jineng_lieshou_pugong_jizhong_1.png");
		addChild(jizhong,5);
		jizhong->setPosition(getSkillFirePos());
		CCFiniteTimeAction* jizhong_action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_pugong_jizhong_%d.png",1,11,0.03f,false);
		CCAction* action = CCSequence::create(
			jizhong_action,
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
			NULL
			);
		jizhong->runAction(action);

	}else
	{// 近战物理
		zhuaHen(data->fightor,this,1);
	}
	// 击退
	shake_attacked(data->fightor,this);
}
void ClientChessBoardItemBase::hitedByNormalBaoJiFight(CCObject* msg_fight)
{
	MSG_Fight* data = (MSG_Fight*)msg_fight;
	CCAssert(data->fightor!=NULL,"");
	CCAssert(data->beAttackedVec.size() == 1,"只能是一个被攻击在者，如果有多个，那么需要拆开调用，不然每一个人收到通知的时候都会发现里面有自己，那么就会减血");


	// 只处理自己
	int huSub;
	bool miss;
	if ( !data->isBeAttacked(this,huSub,miss) || hero.hp == 0 || data->fightor->getHero()->hp == 0)
	{
		return;
	}
	if (miss)
	{
		missAction(this);
		return;
	}

	// 先做Normal一样的动画
	{
		// 减血
		hpSub(huSub,Sub_BaoJi);

		HeroFixedPro fixPro = BaseActorPropertyParser::getParser()->getExtraProerty(data->fightor->hero.profession_id);

		if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_MO_FA)
		{// 远程魔法
			CCParticleSystemQuad* par = CCParticleSystemQuad::create("jineng_jisi_baoji_baozha.plist");
			par->setAutoRemoveOnFinish(true);
			par->setScale(1.4f);
			addChild(par);
			par->setPosition(getSkillFirePos());
		}
		else if (fixPro.gongJiJuLi >= 2.0f && fixPro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
		{// 远程物理
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_lieshou_baoji.plist");
			SkillSprite* jizhong = SkillSprite::create("jineng_lieshou_baoji_jizhong_1.png");
			addChild(jizhong,5);
			jizhong->setPosition(getSkillFirePos());
			CCFiniteTimeAction* jizhong_action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_baoji_jizhong_%d.png",1,11,0.03f,false);
			CCAction* action = CCSequence::create(
				jizhong_action,
				CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
				NULL
				);
			jizhong->runAction(action);

		}else
		{// 近战物理
			zhuaHen(data->fightor,this,3);
		}
		// 击退
		shake_attacked(data->fightor,this);
	}
	return;
}


void ClientChessBoardItemBase::tryFanTan(ClientChessBoardItemBase* fightor)
{
	bool found = false;
	Buff_Doing fantan;
	for (vector<Buff_Doing>::iterator it = canFanTanData.begin();it!=canFanTanData.end();it++)
	{
		if (it->target == fightor->getUniqueId())
		{// 是要反弹的目标
			found = true;
			fantan = *(it);
			canFanTanData.erase(it);
			break;
		}
	}

	// 开始反弹
	if (found)
	{
		switch (fantan.relativedBuffStateId)
		{
		case BuffState_MangCi:
			fanTan_MangCi(fantan,fightor);
			break;
		default:
			CCAssert(false,"还不支持这种的反弹");
			break;
		}
	}
}

void ClientChessBoardItemBase::fanTan_MangCi(Buff_Doing data,ClientChessBoardItemBase* target)
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_qishi_mangci.plist");

	this->runAction(CCSequence::create(
		CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
		CCRotateTo::create(0.0f,0),
		CCScaleTo::create(0.1f,1.15f),
		CCScaleTo::create(0.01f,1.0f),
		NULL
		));

	MSG_FanTan* msg_data = new MSG_FanTan();
	msg_data->fantan = data;
	msg_data->fightor = this;
	msg_data->target = target;
	msg_data->playDefaultHithedSound = true;

	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	// 反击运气
	SkillSprite* fire = SkillSprite::create("jineng_qishi_mangci_fanji_1.png");
	CCFiniteTimeAction* fire_action = SpriteHelper::easyCreateAction_pList("jineng_qishi_mangci_fanji_%d.png",1,16,0.04f,false);
	fire->runAction(CCSequence::create(
		fire_action,
		CCFadeTo::create(0.2f,50),
		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));
	layer->addChild(fire,10);
	fire->setPosition(LF::getNativePostion(this->getFireNode(),layer));

	// 飞行
	SkillSprite* jian = SkillSprite::create("jineng_qishi_mangci_feixing.png");
	layer->addChild(jian,10);
	jian->setPosition(LF::getNativePostion(this->getFireNode(),layer));
	jian->setVisible(false);
	// 箭飞行
	CCPoint statPoint = LF::getNativePostion(this->getFireNode(),layer);
	CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
	CCPoint aToB = LF::getVectorFromAToB(this->getFireNode(),target->getFireNode());
	float angle = LF::getRotateAngle(aToB,ccp(0,1));
	float dis = ccpDistance(endPoint,statPoint);
	float time = dis/456.0f * 0.18f;
	float jian_fly_time = fire_action->getDuration() + 0.1f;
	CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
	CCAction* fly = CCSequence::create(
		CCDelayTime::create(jian_fly_time),
		CCShow::create(),
		CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
		CCCallFuncND::create(this,SEL_CallFuncND(&ClientChessBoardItemBase::fantan_hit),msg_data),
		CCCallFuncN::create(target,SEL_CallFuncN(&BaseSprite::removeNode)),
		NULL
		);
	jian->runAction(fly);
	jian->setRotation(angle);


	// 最后的爆炸
	SkillSprite* bomb = SkillSprite::create("jineng_qishi_mangci_shoushang_1.png");
	CCFiniteTimeAction* bomb_action = SpriteHelper::easyCreateAction_pList("jineng_qishi_mangci_shoushang_%d.png",1,12,0.04f,false);
	bomb->setVisible(false);
	target->addChild(bomb,10);
	bomb->setPosition(target->getNormalPos());
	bomb->runAction(CCSequence::create(
		CCDelayTime::create(time+jian_fly_time),
		CCShow::create(),
		bomb_action,
		CCCallFuncN::create(target,SEL_CallFuncN(&BaseSprite::removeNode)),
		NULL
		));
}

void ClientChessBoardItemBase::fantan_hit(CCNode* node,void* msg_fantan)
{
	MSG_FanTan* msg_data = (MSG_FanTan*)msg_fantan;

	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty(msg_data->fightor->getProfession());
	if (msg_data->playDefaultHithedSound)
	{
		if (pro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
		{
			Music::sharedMusicPlayer()->playEffectSound("wuLiGongJi.wav");
		}else
		{
			Music::sharedMusicPlayer()->playEffectSound("moFaGongJi.wav");
		}
	}

	if (msg_data->hited_sound != "")
	{
		Music::sharedMusicPlayer()->playEffectSound(msg_data->hited_sound);
	}

	// 减血
	ClientChessBoardItemBase* target = msg_data->target;
	if (target)
	{
		target->hpAdd(msg_data->fantan.hp_add);
		// 击退
		target->shake_attacked(msg_data->fightor,target);
	}
	msg_data->release();
}

// 公共动画
CCFiniteTimeAction* ClientChessBoardItemBase::get_fire_shake_Action(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked,float offset /* = 15 */)
{
	CCPoint meToTarget = LF::getVectorFromAToB(fightor,attacked);
	CCPoint moveOffset = ccp(-offset,-offset);
	CCPoint originPos = fightor->getPosition();

	CCFiniteTimeAction* action =  CCSequence::create(
		CCEaseOut::create(CCMoveBy::create(0.05f,ccp(moveOffset.x * meToTarget.x,moveOffset.y*meToTarget.y)),2.0f) ,
		CCMoveTo::create(0.1f,originPos),
		NULL
		);
	return action;
}

void ClientChessBoardItemBase::missAction(ClientChessBoardItemBase* target)
{
	// Miss 符号
	{
		BaseSprite* miss = BaseSprite::create(mResMgr->getPath("fangyu_shanbi.png"));
		this->getParent()->addChild(miss,3);
		miss->setPosition(ccpAdd(randomPosRelativeNode(this),ccp(0,-10)));
		miss->setScale(0.5f);
		miss->setOpacity(0);

		miss->runAction(CCSequence::create(
			CCScaleTo::create(0.01f, 0.1f),
			CCSpawn::create(CCScaleTo::create(0.085f, 1.2f), CCFadeTo::create(0.085f, 255), NULL),
			CCScaleTo::create(0.05f, 1),
			CCScaleTo::create(0.05f, 1.2f),
			CCDelayTime::create(0.25f),
			CCSpawn::create(CCFadeTo::create(0.085f, 0), CCMoveBy::create(0.085f, ccp(0, 150)), NULL),
			NULL));
/*		miss->runAction(CCSequence::create(
			CCFadeTo::create(0,255),
			CCSpawn::create(
			CCScaleTo::create(0.2f,1.2f),
			CCMoveBy::create(0.2f,ccp(0,20.0f)),
			NULL
			),
			CCSpawn::create(
			CCMoveBy::create(0.3f,ccp(0,60)),
			CCFadeTo::create(0.3f,50),
			NULL
			),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
			NULL
			));*/
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint sideMoveBy = LF::getWorldPostion(this).x < winSize.width/2 ? ccp(-100,0):ccp(100,0);

	this->runAction(CCSequence::create(
		CCSpawn::create(
		CCFadeTo::create(0.05f,125),
		CCScaleTo::create(0.05f,0.9f),
		//CCMoveBy::create(0.05f,sideMoveBy),
		NULL
		),

		CCDelayTime::create(0.2f),

		CCSpawn::create(
		CCFadeTo::create(0.05f,255),
		CCScaleTo::create(0.05f,1.0f),
		CCMoveTo::create(0.05f,getNormalPos()),
		NULL
		),
		NULL
		));

	//{
	//	// Miss 符号
	//	{
	//		BaseSprite* miss = BaseSprite::create(mResMgr->getPath("fangyu_shanbi.png"));
	//		this->getParent()->addChild(miss,3);
	//		miss->setPosition(ccpAdd(randomPosRelativeNode(this),ccp(0,-10)));
	//		miss->setScale(0.5f);
	//		miss->setOpacity(0);

	//		miss->runAction(CCSequence::create(
	//			CCFadeTo::create(0,255),
	//			CCSpawn::create(
	//			CCScaleTo::create(0.2f,1.2f),
	//			CCMoveBy::create(0.2f,ccp(0,20.0f)),
	//			NULL
	//			),
	//			CCSpawn::create(
	//			CCMoveBy::create(0.3f,ccp(0,60)),
	//			CCFadeTo::create(0.3f,50),
	//			NULL
	//			),
	//			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
	//			NULL
	//			));
	//	}
	//	this->runAction(CCSequence::create(
	//		CCSpawn::create(
	//		CCFadeTo::create(0.05f,125),
	//		CCScaleTo::create(0.05f,0.9f),
	//		NULL
	//		),

	//		CCDelayTime::create(0.2f),

	//		CCSpawn::create(
	//		CCFadeTo::create(0.05f,255),
	//		CCScaleTo::create(0.05f,1.0f),
	//		NULL
	//		),
	//		NULL
	//		));

	//}
}
// 抓痕
void ClientChessBoardItemBase::zhuaHen(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked,int num)
{
	if (num == 3)
	{
		//
		BaseSprite* s = BaseSprite::create("jinzhan_baoji.png");
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(s,5);
		s->setPosition(LF::getNativePostion(attacked->getFireNode(),layer));
		CCAction* action = CCSequence::create(
			CCDelayTime::create(0.2f),
			CCFadeTo::create(0.1f,0),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		s->runAction(action);
	}
	else
	{
		BaseSprite* s = BaseSprite::create("zhandou_jinzhan_pugong_daoguang.png");
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(s,5);
		s->setPosition(LF::getNativePostion(attacked->getFireNode(),layer));
		//
		CCAction* action = CCSequence::create(
			CCDelayTime::create(0.2f),
			CCFadeTo::create(0.1f,0),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		s->runAction(action);
	}


	//if (num == 3)
	//{
	//	//
	//	BaseSprite* s = BaseSprite::create("jinzhan_baoji.png");
	//	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	//	layer->addChild(s,5);
	//	s->setPosition(LF::getNativePostion(this->getFireNode(),layer));
	//	CCAction* action = CCSequence::create(
	//		CCDelayTime::create(0.2f),
	//		CCFadeTo::create(0.1f,0),
	//		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//		NULL
	//		);
	//	s->runAction(action);
	//}
	//else
	//{
	//	BaseSprite* s = BaseSprite::create("zhandou_jinzhan_pugong_daoguang.png");
	//	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	//	layer->addChild(s,5);
	//	s->setPosition(LF::getNativePostion(this->getFireNode(),layer));
	//	//
	//	CCAction* action = CCSequence::create(
	//		CCDelayTime::create(0.2f),
	//		CCFadeTo::create(0.1f,0),
	//		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//		NULL
	//		);
	//	s->runAction(action);
	//}
}
// 被打退
void ClientChessBoardItemBase::shake_attacked(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked)
{
	// 回位
	// 修复bug：当fightor和actact的图片的getContentSize不一样的话
	// 那么回位就有bug，不对，所以得指定是哪个的getContentSize
	//fightor->setPosition(ccp(getContentSize().width/2,getContentSize().height/2));
	// 暂时注释，因为跑过去打斗，如果这里要回位的话，就不能直接设置位置
	//fightor->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));


	// 检测是否需要显示盾
	if (showShouHuDuanNum > 0)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("jineng_qishi_shouhu.plist");

		SkillSprite* fangyuDun = SkillSprite::create("jineng_qishi_shouhu_chufa_1.png");
		CCFiniteTimeAction* touYinAction = SpriteHelper::easyCreateAction_pList("jineng_qishi_shouhu_chufa_%d.png",1,25,0.02f,false);
		fangyuDun->setPosition(getNormalPos());
		addChild(fangyuDun,10);
		fangyuDun->runAction(CCSequence::create(
			touYinAction,
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseSprite::removeNode)),
			NULL
			));
		showShouHuDuanNum --;
	}
	else
	{
		CCPoint dir = LF::getVectorFromAToB(fightor,attacked);
		CCPoint offset = ccp(SHAKE_OFFSET*dir.x,SHAKE_OFFSET*dir.y);
		attacked->runAction(getShake(attacked,offset));
	}


	// 通知一下
	tryFanTan(fightor);
}

CCFiniteTimeAction* ClientChessBoardItemBase::getShake(CCNode* node,CCPoint offset)
{
	// 只用于ClientChessBoardItemBase,因为有时会出现打了之后无法回到原位
	// 那么，我们知道ClientChessBoardItemBase是放在跟自己一样的container的中心的
	// 所有我们就为了解决这个bug，就每次shake的最后，强制回到container的中心
	// 于是也就无法用于其他地方了
	CCAssert(dynamic_cast<ClientChessBoardItemBase*>(node)!=NULL,"只用于ClientChessBoardItemBase");
	
	CCPoint currentPos = node->getPosition();
	CCFiniteTimeAction* ret=  CCSequence::create(
		CCEaseIn::create(CCMoveTo::create(0.1f,ccpAdd(currentPos,offset)),3),
		CCEaseOut::create(CCMoveTo::create(0.15f,currentPos),3),
		// 强制回到中心
		//20140710 scf --- 新技能回到中心由clientlayer处理
//		CCMoveTo::create(0.01f,getNormalPos()),
		NULL
		);
	return ret;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


ClientChessBoardItem::ClientChessBoardItem(ClientChessBoard* originBoard,HeroBase hero,unsigned int uniqueId)
	:ClientChessBoardItemBase(hero,uniqueId)
{
	this->originBoard = originBoard;
	this->currentBoard = NULL;
	this->currentCDTime = 0;
	gridX = -1;
	gridY = -1;
// 	cdBar = NULL;
// 	hightLine = NULL;

	// 更新FirePiont的位置
	getFireNode()->setPosition(getSkillFirePos());

	/*{
		char str[128];
		sprintf(str,"%d",currentCDTime);
		CCLabelTTF* lableNum = CCLabelTTF::create("0",fontStr_BookAntiqua,24*2,CCSizeMake(100,0),kCCTextAlignmentCenter);
		lableNum->setString(str);
		lableNum->setAnchorPoint(ccp(0.5f,0.5f));
		addChild(lableNum,2,TAG_CD_TIME);
		lableNum->setPosition(ccp(getContentSize().width/2,40));
		lableNum->setScale(0.5f);
		lableNum->setVisible(currentCDTime > 0);
	}*/

	//以前显示血瓶，改为显示共鸣星级
	/*{
		bool showXuePingCd = (hero.hero_type == Type_Hero_Normal || hero.hero_type == Type_Hero_Team);
		{
			BaseSprite* bloodBG = (BaseSprite*)getChildByTag(TAG_BLOOD_BG);

			BaseSprite* s = new BaseSprite();
			s->initWithFile(mResMgr->getPath("bloodBar_xuepin.png"));//图片已删除
			cdBar = LF::createProgressTimer(s);
			cdBar->setBarChangeRate(ccp(0, 1));
			bloodBG->addChild(cdBar);
			cdBar->setPosition(ccp(bloodBG->getContentSize().width/2,31));
			float percent = ((float)(hero.defaultCdTime - currentCDTime))/(float)hero.defaultCdTime * 100.0f;;
			cdBar->setPercentage(percent);
			s->release();

			hightLine = BaseSprite::create("bloodBar_xuepin_tiao.png");//图片已删除
			cdBar->addChild(hightLine);
			hightLine->setPosition(ccp(hightLine->getContentSize().width/2,cdBar->getSprite()->getContentSize().height*cdBar->getPercentage()/100));
			if (hightLine->getPositionY() < hightLine->getContentSize().height)
				hightLine->setPositionY(hightLine->getContentSize().height);

			BaseSprite* cover = BaseSprite::create("bloodBar_xuepin_fugai.png");//图片已删除
			bloodBG->addChild(cover);
			cover->setPosition(ccp(bloodBG->getContentSize().width/2,39));

			cdBar->setVisible(showXuePingCd);
			hightLine->setVisible(showXuePingCd);
			cover->setVisible(showXuePingCd);
		}
	}*/
	if (hero.profession_id < Profession_GW_Start )//&& MyselfManager::getManager()->getMyTeamActor(hero.heroId))
	{
		//共鸣
		ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
		//当前共鸣星级
// 		vector<EquipData> vec;
// 		vector<Equipment*> curHeroEqVec = MyselfManager::getManager()->getAllEquipmentsOnActor(hero.heroId,Filter_Equipped);
// 		for (int i=0; i!=curHeroEqVec.size(); ++i)
// 			vec.push_back(curHeroEqVec.at(i)->getEquipData());
// 		int curStarNum = manager->getGongMingStarNum(vec);
		int curStarNum = hero.resonance_star;
		int curLevel = manager->getGongMingLevel(curStarNum);
		int imgIdx = 1;
		if (curLevel == 0 || curLevel == 1) imgIdx = 1;
		else if (curLevel == 2 || curLevel == 3) imgIdx = 2;
		else if (curLevel == 4 || curLevel == 5 || curLevel == 6) imgIdx = 3;
		else if (curLevel == 7) imgIdx = 4;
		else if (curLevel == 8) imgIdx = 5;
		else if (curLevel == 9) imgIdx = 6;
		else if (curLevel == 10) imgIdx = 7;
		CCString temp;
		temp.initWithFormat("gongming_anniu_%d.png", imgIdx);
		BaseSprite *gmImg = BaseSprite::create(temp.getCString());
		BaseSprite* bloodBG = (BaseSprite*)getChildByTag(TAG_BLOOD_BG);
		bloodBG->addChild(gmImg, 0, TAG_IMG_GONGMING);
		gmImg->setPosition(ccp(80, 73-28));
		gmImg->setScale(0.5f);
		if (curLevel == 0) gmImg->setVisible(false);
	}
}
ClientChessBoardItem::~ClientChessBoardItem()
{
}

void ClientChessBoardItem::setCurrentBoard(ClientChessBoard* board)
{
	this->currentBoard = board;
}
void ClientChessBoardItem::setCurrentPos(int gridX,int gridY)
{
	this->gridX = gridX;
	this->gridY = gridY;
}
int ClientChessBoardItem::getGridX()
{
	return gridX;
}
int ClientChessBoardItem::getGridY()
{
	return gridY;
}

IChessboardProperty* ClientChessBoardItem::getCurrentBoardProperty()
{
	return currentBoard;
}

ClientChessBoard* ClientChessBoardItem::getCurrentBoard()
{
	return currentBoard;
}
ClientChessBoard* ClientChessBoardItem::getOriginBoard()
{
	return originBoard;
}
CCPoint ClientChessBoardItem::getSkillFirePos()
{
	CCPoint ret;

	switch (hero.profession_id)
	{
	case Profession_Yemanren:
		ret = ccp(132,201);
		break;
	case Profession_Qishi:
		ret = ccp(99,201);
		break;
	case Profession_ShenJianShou:
		ret = ccp(84,182);
		break;
	case Profession_MiShu:
		ret = ccp(114,201);
		break;

	case Profession_JZ_ShiRenMo:
		ret = ccp(88,186);
		break;
	case Profession_YC_KuLouGongJianShou:
		ret = ccp(90,199);
		break;
	case Profession_FS_YeRenSaMan:
		ret = ccp(94,202);
		break;
	default:
		ret = ClientChessBoardItemBase::getSkillFirePos();
		break;
	}
	return ret;
	
}
int ClientChessBoardItem::getCurrentBloodCdTime()
{
	return currentCDTime;
}
void ClientChessBoardItem::setCurrentBloodCdTime(int time)
{
	currentCDTime = time;

	//char str[128];
	//sprintf(str,"%d",currentCDTime);
	//((CCLabelTTF*)getChildByTag(TAG_CD_TIME))->setString(str);
	//getChildByTag(TAG_CD_TIME)->setVisible(currentCDTime > 0);

// 	float percent = ((float)(hero.defaultCdTime - currentCDTime))/(float)hero.defaultCdTime * 100.0f;
// 	cdBar->setPercentage(percent);
// 	hightLine->setPositionY(cdBar->getSprite()->getContentSize().height*cdBar->getPercentage()/100);
// 	if (hightLine->getPositionY() < hightLine->getContentSize().height)
// 		hightLine->setPositionY(hightLine->getContentSize().height);
}

void ClientChessBoardItem::clearBlood()
{
// 	cdBar->setPercentage(0);
// 	hightLine->setPositionY(cdBar->getSprite()->getContentSize().height*cdBar->getPercentage()/100);
// 	if (hightLine->getPositionY() < hightLine->getContentSize().height)
// 		hightLine->setPositionY(hightLine->getContentSize().height);
}

void ClientChessBoardItem::showDeadImage()
{
	ClientChessBoardItemBase::showDeadImage();

	// 隐藏数字
	setCurrentBloodCdTime(0);	

	// 自己的任务死亡就显示墓碑
	if (getOriginBoard()->getBoardName() == BOARD_NAME_BUTTOM ||
		getProfession() < Profession_GW_Start)  //只要是人物都显示墓碑
	{

		getChildByTag(TAG_DI_ZUO_SHADOW)->runAction(CCSequence::create(
			CCDelayTime::create(0.2f),
			CCFadeTo::create(0.1f,0),
			NULL));
		getChildByTag(TAG_BLOOD_BG)->runAction(CCSequence::create(
			CCDelayTime::create(0.2f),
			CCFadeTo::create(0.1f,0),
			NULL));
		getChildByTag(TAG_BLOOD_BG)->getChildByTag(TAG_BLOODE_MASK)->runAction(CCSequence::create(
			CCFadeTo::create(0.1f,0),
			NULL));

		runAction(CCSequence::create(
			CCDelayTime::create(0.2f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,80.0f)),
			CCFadeTo::create(0.1f,0),
			CCScaleTo::create(0.1f,0.0f),
			NULL
			),
			CCCallFuncN::create(this,SEL_CallFuncN(&ClientChessBoardItem::showMuBei)),
			NULL
			));
	}
	else
	{

		getChildByTag(TAG_DI_ZUO_SHADOW)->runAction(CCSequence::create(
			CCDelayTime::create(0.5f),
			CCFadeTo::create(0.2f,0),
			NULL));
		getChildByTag(TAG_BLOOD_BG)->runAction(CCSequence::create(
			CCDelayTime::create(0.5f),
			CCFadeTo::create(0.2f,0),
			NULL));
		getChildByTag(TAG_BLOOD_BG)->getChildByTag(TAG_BLOODE_MASK)->runAction(CCSequence::create(
			CCFadeTo::create(0.2f,0),
			NULL));

		runAction(CCSequence::create(
			CCDelayTime::create(0.5f),
			CCSpawn::create(
				CCScaleTo::create(0.2f,0.8f),
				CCFadeTo::create(0.2f,0),
				NULL),
			CCHide::create(),
			NULL
			));
	}
}
void ClientChessBoardItem::showMuBei(CCNode* node)
{
	for (int i=0;i<full_photo_parts->count();i++)
	{
		((CCNode*)full_photo_parts->objectAtIndex(i))->setVisible(false);
	}
	setOpacity(255);
	setScale(1.0f);
	setColor(ccWHITE);
	removeAllChildrenWithCleanup(true);

	CCFiniteTimeAction* action = SpriteHelper::easyCreateAction("mubei_%d.png",1,5,0.1f,false);
	runAction(action);
	setPosition(ccp(getPositionX(),getContentSize().height/2));

	Music::sharedMusicPlayer()->playEffectSound("muBeiSound.wav");

	{
		CCParticleSystem* m_emitter = new CCParticleFire();
		m_emitter->initWithTotalParticles(240);
		m_emitter->setBlendAdditive(false);

		m_emitter->setAutoRemoveOnFinish(true);
		m_emitter->setDuration(0.5f);
		m_emitter->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png"));
		m_emitter->setPosVar(ccp(55,55));
		m_emitter->setAngleVar(360);
		m_emitter->setStartColor(ccc4f(0.8f,0.8f,0.8f,0.1f));
		m_emitter->setEndColor(ccc4f(0.1f,0.1f,0.1f,0.06f));
		m_emitter->setEndColorVar(ccc4f(0.0f,0.0f,0.0f,0.0f));

		m_emitter->setLife(0.8f);
		m_emitter->setLifeVar(0.3f);
		m_emitter->setSpeed(30.0f);
		m_emitter->setSpeedVar(20.0f);

		m_emitter->setScale(getScale());

		m_emitter->autorelease();

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(m_emitter, 2);
		m_emitter->setPosition(ccpAdd(LF::getNativePostion(this,layer),ccp(0,-20)));

		m_emitter->setVisible(false);
		m_emitter->runAction(CCSequence::create(CCDelayTime::create(0.2), CCShow::create(), NULL));

	}
}




ClientChessBoardItem_TF::ClientChessBoardItem_TF(HeroBase hero,unsigned int uniqueId):ClientChessBoardItemBase(hero,uniqueId)
{
	freesyNum = 0;  
	// 更新FirePiont的位置
	getFireNode()->setPosition(getSkillFirePos());

	{
		char str[128];
		sprintf(str,"%d",freesyNum);

		CCLabelTTF* lableNum = CCLabelTTF::create("0",fontStr_BookAntiqua,24*2,CCSizeMake(100,0),kCCTextAlignmentCenter);
		//CCLabelAtlas* lableNum = CCLabelAtlas::create(
		//	"0123456789",
		//	mResMgr->getPath("bloodNum.png").c_str(),
		//	60, 94,//71
		//	'0');
		lableNum->setString(str);
		lableNum->setAnchorPoint(ccp(0.5f,0.5f));
		addChild(lableNum,2,TAG_CD_TIME);
		lableNum->setPosition(ccp(getContentSize().width/2,lableNum->getContentSize().height/2));
		lableNum->setScale(0.5f);
		lableNum->setVisible(freesyNum > 0);
	}
}
ClientChessBoardItem_TF::~ClientChessBoardItem_TF()
{
}

void ClientChessBoardItem_TF::setCurrentBoard(ClientChessBoard_TeamFight* board)
{
	currentBoard = board;
}

//
int ClientChessBoardItem_TF::getCurrentFreesyTime()
{
	return freesyNum;
}
CCPoint ClientChessBoardItem_TF::getSkillFirePos()
{
	return ccp(getContentSize().width/2,getContentSize().height/2);
}
void ClientChessBoardItem_TF::setCurrentFreesyTime(int time)
{
	freesyNum = time;

	char str[128];
	sprintf(str,"%d",freesyNum);
	((CCLabelTTF*)getChildByTag(TAG_CD_TIME))->setString(str);
	getChildByTag(TAG_CD_TIME)->setVisible(freesyNum > 0);
}

void ClientChessBoardItem_TF::hpAdd(int toAdd)
{
	ClientChessBoardItemBase::hpAdd(toAdd);
}

IChessboardProperty* ClientChessBoardItem_TF::getCurrentBoardProperty()
{
	return currentBoard;
}