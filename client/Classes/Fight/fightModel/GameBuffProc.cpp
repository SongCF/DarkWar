#include "GameBuffProc.h"
#include "GameBuffItemGetter.h"


GameBuffProc::GameBuffProc(GameBuffItemGetter* getter)
{
	mCurrentUsedBuffId = 0;
	//mHelper = helper;
	mGetter = getter;
}

void GameBuffProc::fightRoundChange(BuffInStep& buff)
{
	vector<AbstractItem*> allItems = mGetter->getAllAlivedItems();

	for (int i=0,itemCount=allItems.size(); i<itemCount; i++)
	{
		AbstractItem* item = allItems[i];
		CCAssert(item!=NULL,"");

		list<GameBuff> buffs = item->getCurrentAttachedBuff();
		for (list<GameBuff>::iterator it = buffs.begin();it!= buffs.end() ;)
		{
			// 有效果
			if (it->effect_vec.size() > 0)
			{
				GameBuffEffect eff = *(it->effect_vec.begin());
				if (eff.remove_pos == Pos_ChangeRound)
				{
					// 是减血的，那么不能减死
					if (eff.persistent_hp_add < 0 && item->getHp() <= (-eff.persistent_hp_add) )
					{
						eff.persistent_hp_add = -(item->getHp() - 1); // 保留一滴血
						CCLog("Buff : %s-->persistent_hp_add = %d , will make actor[uId = %d] dead, keep one blood!",
							__FUNCTION__,
							eff.persistent_hp_add,mGetter->getItemUniqueId(item));
					}

					// 有加血，减血
					// 真实的加减血，并把效果数值保存出去
					if (eff.persistent_hp_add != 0)
					{
						Buff_Doing doing;
						doing.target = mGetter->getItemUniqueId(item);
						doing.hp_add = eff.persistent_hp_add;
						doing.relativedBuffStateId = it->state;

						buff.effects.push_back(doing);
						// 真实加血
						item->addHp(eff.persistent_hp_add);
					}

					// 移除
					buff.remove_buff_array.push_back(eff.effect_id);
					item->removeBuff(eff.effect_id);
				}
			}
			it++ ;
		}
	}
}

bool GameBuffProc::beforeFight(int fightorUniqueId,BuffInStep& buff)
{
	AbstractItem* item = mGetter->getAbstractItemByUniqueId(fightorUniqueId);
	CCAssert(item!=NULL,"");

	bool xuanyun = false;

	list<GameBuff> buffs = item->getCurrentAttachedBuff();
	// 作用效果
	for (list<GameBuff>::iterator it = buffs.begin();
		it != buffs.end();)
	{
		// 还有Buff效果
		if (it->effect_vec.size() > 0)
		{
			if ( (it->effect_vec.begin())->xuanyun )
			{
				xuanyun = true; // 眩晕状态
				break;
			}
		}
		it++;
	}

	// 移除
	for (list<GameBuff>::iterator it = buffs.begin();
		it != buffs.end();)
	{
		// 还有Buff效果
		if (it->effect_vec.size() > 0)
		{
			if ( (it->effect_vec.begin())->remove_pos == Pos_Self_Will_fight )
			{
				buff.remove_buff_array.push_back( (it->effect_vec.begin())->effect_id);
				item->removeBuff((it->effect_vec.begin())->effect_id);
			}
		}
		it++;
	}

	return !xuanyun;
}

void GameBuffProc::one_visible_step(BuffInStep& buff)     // 只处理直接数据修改的buff，这里就是血量
{
	vector<AbstractItem*> all = mGetter->getAllAlivedItems();
	int num = all.size();

	for (int i=0;i<num;i++)
	{
		list<GameBuff> buffs = all[i]->getCurrentAttachedBuff();
		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ;)
		{
			// 有效果,只能是血量的变化
			if (it->effect_vec.size() > 0)
			{
				GameBuffEffect eff = *(it->effect_vec.begin());
				if (eff.remove_pos == Pos_Step)
				{
					CCAssert(eff.baoji_add == 0,"只能做血量变化的这种直接属性的修改");
					CCAssert(eff.hujia_add == 0,"只能做血量变化的这种直接属性的修改");
					CCAssert(eff.jianshang_lv == 0,"只能做血量变化的这种直接属性的修改");
					CCAssert(eff.shanbi_add == 0,"只能做血量变化的这种直接属性的修改");
					CCAssert(eff.xuanyun == false,"只能做血量变化的这种直接属性的修改");
					//CCAssert(eff.persistent_hp_add == false,"只能做血量变化的这种直接属性的修改");
					CCAssert(eff.fantan_lv == false,"只能做血量变化的这种直接属性的修改");
					
					// 是减血的，那么不能减死
					if (eff.persistent_hp_add < 0 && all[i]->getHp() <= (-eff.persistent_hp_add) )
					{
						eff.persistent_hp_add = -(all[i]->getHp() - 1); // 保留一滴血
						CCLog("Buff : %s-->persistent_hp_add = %d , will make actor[uId = %d] dead, keep one blood!",
							__FUNCTION__,
							eff.persistent_hp_add,mGetter->getItemUniqueId(all[i]));
					}

					Buff_Doing doing;
					doing.target = mGetter->getItemUniqueId(all[i]);
					doing.hp_add = eff.persistent_hp_add;
					doing.relativedBuffStateId = it->state;

					buff.effects.push_back(doing);
					buff.remove_buff_array.push_back(eff.effect_id);
					// 移除
					all[i]->removeBuff(eff.effect_id);

					// 真实加血
					all[i]->addHp(eff.persistent_hp_add);
				}
			}
			it++ ;
		}
	}
}

//void GameBuffProc::fight_end_or_beattacked(int fightorUniqueId,int targetUniqueId,vector<int>& removedBuff)
//{
//	AbstractItem* fightor = mGetter->getAbstractItemByUniqueId(fightorUniqueId);
//	CCAssert(fightor!=NULL,"");
//	AbstractItem* target = mGetter->getAbstractItemByUniqueId(targetUniqueId);
//	CCAssert(target!=NULL,"");
//
//	// 队友间的攻击不算
//	if (mGetter->isTeammate(fightorUniqueId,targetUniqueId))
//		return;
//
//	// 攻击方
//	{
//		list<GameBuff> buffs = fightor->getCurrentAttachedBuff();
//		for (list<GameBuff>::iterator it = buffs.begin(); 
//			it!= buffs.end() ;it++)
//		{
//			// 有效果
//			if (it->effect_vec.size() > 0)
//			{
//				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_Fight_End )
//				{
//					// 真实加血
//					fightor->addHp((it->effect_vec.begin())->hp_add);
//
//					removedBuff.push_back( (it->effect_vec.begin())->effect_id );
//					fightor->removeBuff((it->effect_vec.begin())->effect_id);
//				}
//			}
//		}
//	}
//
//	// 被攻击方
//	{
//		list<GameBuff> buffs = target->getCurrentAttachedBuff();
//
//		for (list<GameBuff>::iterator it = buffs.begin(); 
//			it!= buffs.end() ; it++)
//		{
//			// 有效果
//			if (it->effect_vec.size() > 0)
//			{
//				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_BeAttacked)
//				{
//					// 真实加血
//					fightor->addHp((it->effect_vec.begin())->hp_add);
//
//					removedBuff.push_back( (it->effect_vec.begin())->effect_id );
//					target->removeBuff((it->effect_vec.begin())->effect_id);
//				}
//			}
//		}
//	}
//}

void GameBuffProc::fight_end(int fightorUniqueId,BuffInStep& buff)
{
	AbstractItem* fightor = mGetter->getAbstractItemByUniqueId(fightorUniqueId);
	CCAssert(fightor!=NULL,"");
	// 攻击方
	{
		list<GameBuff> buffs = fightor->getCurrentAttachedBuff();
		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ;it++)
		{
			// 有效果
			if (it->effect_vec.size() > 0)
			{
				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_Fight_End )
				{
					//Buff_Doing doing;
					//doing.target = mGetter->getItemUniqueId(fightor);
					//doing.hp_add = (it->effect_vec.begin())->hp_add;
					//buff.effects.push_back(doing);

					buff.remove_buff_array.push_back( (it->effect_vec.begin())->effect_id );
					fightor->removeBuff((it->effect_vec.begin())->effect_id);

					//// 真实加血
					//fightor->addHp((it->effect_vec.begin())->hp_add);
				}
			}
		}
	}
}

void GameBuffProc::beattacked(int fightorUniqueId,int targetUniqueId,int targetBloodChange,BuffInStep& buff)
{
	AbstractItem* fightor = mGetter->getAbstractItemByUniqueId(fightorUniqueId);
	CCAssert(fightor!=NULL,"");
	AbstractItem* target = mGetter->getAbstractItemByUniqueId(targetUniqueId);
	CCAssert(target!=NULL,"");

	// 队友间的攻击不算
	if (mGetter->isTeammate(fightorUniqueId,targetUniqueId))
		return;
	// 被攻击方
	{
		list<GameBuff> buffs = target->getCurrentAttachedBuff();

		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ; it++)
		{
			// 有效果
			if (it->effect_vec.size() > 0)
			{
				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_BeAttacked)
				{

					buff.remove_buff_array.push_back( (it->effect_vec.begin())->effect_id );
					target->removeBuff((it->effect_vec.begin())->effect_id);

					// 反弹,如果被攻击者已经死了，就不反弹了
					if ((it->effect_vec.begin())->fantan_lv != 0 && target->getHp() > 0)
					{
						Buff_Doing doing;
						doing.target = mGetter->getItemUniqueId(fightor);
						doing.hp_add = (int) ((it->effect_vec.begin())->fantan_lv * (float)targetBloodChange)/100.0f;
						// 标注为反弹
						doing.isFantan = true;
						doing.fightor_u_id = mGetter->getItemUniqueId(target);
						doing.relativedBuffStateId = it->state;

						buff.effects.push_back(doing);
						// 真实加血
						fightor->addHp(doing.hp_add);
					}
				}
			}
		}
	}

}

GameBuffProc::~GameBuffProc(void)
{
}
