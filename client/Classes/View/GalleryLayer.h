#ifndef GALLERY_LAYER_H 
#define GALLERY_LAYER_H 

#include "cocos2d.h" 
#include "cocos-ext.h" 

USING_NS_CC; 
USING_NS_CC_EXT; 

class GalleryDataSet
{
public:
	virtual CCNode* createItem(int index) = 0;
	virtual int getItemNum() = 0;
};
class GalleryCallBack
{
public:
	// 选到第几页
	virtual void onGallerySelected(int index,CCNode*item){};
};


// 都在0.0点
class GalleryLayer : public CCScrollView
{ 
public: 

	static GalleryLayer* create(CCSize size/*, CCNode* container*/);

	~GalleryLayer();

	void setDataSetAndCallBack(GalleryDataSet* dataSet, GalleryCallBack* callBack);
	// 载入数据，开始刷新
	void reloadData();

	void scrollToPage(int pageIndex,bool animation = true);
public: 
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent); 
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent); 
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent); 
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent); 

private: 
	////根据手势滑动的距离和方向滚动图层 
	//void adjustScrollView(float offset); 
	GalleryLayer();
	// 自己滑动
	void tryAutoScroll(CCPoint startPoint,CCPoint endPoint);
	// 
	void scrollToCurrentPage(bool animation = true);
	// 
	void scrollEnd(CCNode* container);

	CCPoint m_touchPoint; 
	bool touchEffective;
	int mCurrentPageIndex;
	int mPageNum;
	float pageWidth;

	GalleryDataSet* mDataSet;
	GalleryCallBack* mCallBack;
	CCNode* container;
	CCArray* itemArray;
}; 

#endif