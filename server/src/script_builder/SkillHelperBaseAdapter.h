#pragma once
#include <vector>
using namespace std;

class ChessBoardItemBase;
/**
 *	��SkillHelperBase�ṩ��������Ľӿ�
 */
class SkillHelperBaseAdapter
{
public:
	// ׼���ܹ��Ľ�ɫ��
	// fightorItem ָ����������Item
	// ���ر������Ľ�ɫ
	// û�оͷ���NULL
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	virtual ChessBoardItemBase* getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance = -1) = 0;

	// ����һ��buffЧ����Id�����Id�ǲ����ظ���
	// mFightControler->mBuffProc->genOneBuffId()
	virtual int genOneBuffEffectId() = 0;

	// ��ȡitem��ȫ�����ŵĶ���
	// �͸���ԭʼ�������жϾ����ˡ������ǻ�������
	virtual vector<ChessBoardItemBase*> getAlivedTeamItems(ChessBoardItemBase* item) = 0;
	// ��ȡitem��ȫ�����ŵĵ���
	// �͸���ԭʼ�������жϾ����ˡ������ǻ�������
	virtual vector<ChessBoardItemBase*> getAlivedEnemyItems(ChessBoardItemBase* item) = 0;

	// ���itemSet������Item��������������Ҫ����
	// ֱ��ת��FightController����
	virtual ChessBoardItemBase* getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor) = 0;
	// ��ȡǰ�ŵ���
	virtual vector<ChessBoardItemBase*> getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor) = 0;
	// ��ȡ���ŵĵ���
	virtual vector<ChessBoardItemBase*> getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor) = 0;

	// ��item1��item2�ľ��룬֧�ֿ����̼���
	virtual float distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2) = 0;

	// ����test�Ƿ�����item����ǰ��
	// ����������˵�����Ƿ���һ�����棬��2�����̵�X����
	virtual bool isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test) = 0;

	// ����test�Ƿ�����item������߻��ұ�
	// ����������˵�����Ƿ���һ�����̵�һ��Y����
	virtual bool isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test) = 0;
};

