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
			// ��Ч��
			if (it->effect_vec.size() > 0)
			{
				GameBuffEffect eff = *(it->effect_vec.begin());
				if (eff.remove_pos == Pos_ChangeRound)
				{
					// �Ǽ�Ѫ�ģ���ô���ܼ���
					if (eff.persistent_hp_add < 0 && item->getHp() <= (-eff.persistent_hp_add) )
					{
						eff.persistent_hp_add = -(item->getHp() - 1); // ����һ��Ѫ
						CCLog("Buff : %s-->persistent_hp_add = %d , will make actor[uId = %d] dead, keep one blood!",
							__FUNCTION__,
							eff.persistent_hp_add,mGetter->getItemUniqueId(item));
					}

					// �м�Ѫ����Ѫ
					// ��ʵ�ļӼ�Ѫ������Ч����ֵ�����ȥ
					if (eff.persistent_hp_add != 0)
					{
						Buff_Doing doing;
						doing.target = mGetter->getItemUniqueId(item);
						doing.hp_add = eff.persistent_hp_add;
						doing.relativedBuffStateId = it->state;

						buff.effects.push_back(doing);
						// ��ʵ��Ѫ
						item->addHp(eff.persistent_hp_add);
					}

					// �Ƴ�
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
	// ����Ч��
	for (list<GameBuff>::iterator it = buffs.begin();
		it != buffs.end();)
	{
		// ����BuffЧ��
		if (it->effect_vec.size() > 0)
		{
			if ( (it->effect_vec.begin())->xuanyun )
			{
				xuanyun = true; // ѣ��״̬
				break;
			}
		}
		it++;
	}

	// �Ƴ�
	for (list<GameBuff>::iterator it = buffs.begin();
		it != buffs.end();)
	{
		// ����BuffЧ��
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

void GameBuffProc::one_visible_step(BuffInStep& buff)     // ֻ����ֱ�������޸ĵ�buff���������Ѫ��
{
	vector<AbstractItem*> all = mGetter->getAllAlivedItems();
	int num = all.size();

	for (int i=0;i<num;i++)
	{
		list<GameBuff> buffs = all[i]->getCurrentAttachedBuff();
		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ;)
		{
			// ��Ч��,ֻ����Ѫ���ı仯
			if (it->effect_vec.size() > 0)
			{
				GameBuffEffect eff = *(it->effect_vec.begin());
				if (eff.remove_pos == Pos_Step)
				{
					CCAssert(eff.baoji_add == 0,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					CCAssert(eff.hujia_add == 0,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					CCAssert(eff.jianshang_lv == 0,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					CCAssert(eff.shanbi_add == 0,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					CCAssert(eff.xuanyun == false,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					//CCAssert(eff.persistent_hp_add == false,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					CCAssert(eff.fantan_lv == false,"ֻ����Ѫ���仯������ֱ�����Ե��޸�");
					
					// �Ǽ�Ѫ�ģ���ô���ܼ���
					if (eff.persistent_hp_add < 0 && all[i]->getHp() <= (-eff.persistent_hp_add) )
					{
						eff.persistent_hp_add = -(all[i]->getHp() - 1); // ����һ��Ѫ
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
					// �Ƴ�
					all[i]->removeBuff(eff.effect_id);

					// ��ʵ��Ѫ
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
//	// ���Ѽ�Ĺ�������
//	if (mGetter->isTeammate(fightorUniqueId,targetUniqueId))
//		return;
//
//	// ������
//	{
//		list<GameBuff> buffs = fightor->getCurrentAttachedBuff();
//		for (list<GameBuff>::iterator it = buffs.begin(); 
//			it!= buffs.end() ;it++)
//		{
//			// ��Ч��
//			if (it->effect_vec.size() > 0)
//			{
//				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_Fight_End )
//				{
//					// ��ʵ��Ѫ
//					fightor->addHp((it->effect_vec.begin())->hp_add);
//
//					removedBuff.push_back( (it->effect_vec.begin())->effect_id );
//					fightor->removeBuff((it->effect_vec.begin())->effect_id);
//				}
//			}
//		}
//	}
//
//	// ��������
//	{
//		list<GameBuff> buffs = target->getCurrentAttachedBuff();
//
//		for (list<GameBuff>::iterator it = buffs.begin(); 
//			it!= buffs.end() ; it++)
//		{
//			// ��Ч��
//			if (it->effect_vec.size() > 0)
//			{
//				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_BeAttacked)
//				{
//					// ��ʵ��Ѫ
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
	// ������
	{
		list<GameBuff> buffs = fightor->getCurrentAttachedBuff();
		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ;it++)
		{
			// ��Ч��
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

					//// ��ʵ��Ѫ
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

	// ���Ѽ�Ĺ�������
	if (mGetter->isTeammate(fightorUniqueId,targetUniqueId))
		return;
	// ��������
	{
		list<GameBuff> buffs = target->getCurrentAttachedBuff();

		for (list<GameBuff>::iterator it = buffs.begin(); 
			it!= buffs.end() ; it++)
		{
			// ��Ч��
			if (it->effect_vec.size() > 0)
			{
				if ( (it->effect_vec.begin())->remove_pos == Pos_Self_BeAttacked)
				{

					buff.remove_buff_array.push_back( (it->effect_vec.begin())->effect_id );
					target->removeBuff((it->effect_vec.begin())->effect_id);

					// ����,������������Ѿ����ˣ��Ͳ�������
					if ((it->effect_vec.begin())->fantan_lv != 0 && target->getHp() > 0)
					{
						Buff_Doing doing;
						doing.target = mGetter->getItemUniqueId(fightor);
						doing.hp_add = (int) ((it->effect_vec.begin())->fantan_lv * (float)targetBloodChange)/100.0f;
						// ��עΪ����
						doing.isFantan = true;
						doing.fightor_u_id = mGetter->getItemUniqueId(target);
						doing.relativedBuffStateId = it->state;

						buff.effects.push_back(doing);
						// ��ʵ��Ѫ
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
