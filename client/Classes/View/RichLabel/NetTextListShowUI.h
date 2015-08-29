#ifndef __NetTextListShowUI__
#define __NetTextListShowUI__

#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
struct MsgColorTexCoords
{
	ccColor4B color;
	CCRect	  mRect;
};
class NetTextListShowUI : public CCSprite
{
public:
	CREATE_FUNC(NetTextListShowUI);
	NetTextListShowUI();
	~NetTextListShowUI();
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
	inline void setTextSize(int size = 20)//设置字体大小
	{
		m_nFontSize = size;
	}
	inline void setTextWidth(int width = 0)//设置区域宽度
	{
		m_nTextWidth = width;
	}
	void addMessage(string str);

	void setMultiple(bool isMultiple = true);

	inline int getHeight()
	{
		if (m_bIsFirstText)
		{
			m_pCurPos.y += m_nFontSize;
		}
		
		return -m_pCurPos.y;
	}

	inline CCSprite* getTTFTextture2d()
	{
		int width = m_nTextWidth;
		int height = getHeight();
		CCRenderTexture *render = CCRenderTexture::create(width,height);
		//render->begin();
		render->beginWithClear(ccBLUE.r, ccBLUE.g, ccBLUE.b, 255);

		ccBlendFunc blendFunc = {GL_DST_COLOR, GL_ZERO};
		this->setBlendFunc(blendFunc);
		this->setPosition(ccp(0, height));
		this->visit();
		render->end();

		return CCSprite::createWithTexture(render->getSprite()->getTexture());
	}
	inline void formattingMsgPoint()
	{
		//for (vector<MsgColorTexCoords>::iterator iter = m_lMsgColorTexCoords.begin() ; iter != m_lMsgColorTexCoords.end(); iter ++)
		//{
		//	iter->mRect.origin.y = 0/* - m_nFontSize*/ - iter->mRect.origin.y;
		//}
	}
	inline vector<MsgColorTexCoords> getMsgColorTexCoordsList()
	{
		return m_lMsgColorTexCoords;
	}
	inline string getMsgString()
	{
		return m_mMsgString;
	}
private:
	string formatting(string str);
	/*********************************************************
	*添加文本信息
	**********************************************************/
	void addString(string str,ccColor3B color = ccBLACK);
	string conversion(string str,int *count);//内容加=n
	string subString(string str,int count,int &new_count);//截取前count个字

	int getJianGePos(string str);
public:
	inline string getConversionMsgStr(string msg,ccColor3B color)
	{
		string inputStr = "";
		inputStr += "[\n";
		CCString *str = CCString::createWithFormat("%d",0);
		inputStr += str->getCString();
		inputStr += "\n";
		inputStr += msg;
		inputStr += "|\n";
		str = CCString::createWithFormat("%d,%d,%d",color.r,color.g,color.b);
		inputStr += str->getCString();
		inputStr += "\n";
		str = CCString::createWithFormat("%d",0);
		inputStr += str->getCString();
		inputStr += "\n";
		str = CCString::createWithFormat("%d",0);
		inputStr += str->getCString();
		inputStr += "\n";
		inputStr += "]\n";
		return inputStr;
	}
private:
	CCMenu				*m_mMenu;
	CCPoint				m_pCurPos;
	
	int					m_nFontSize;
	int					m_nTextWidth;

	ccColor3B			m_textColor;
	int					m_nMaxSizeNumber;

	bool				m_bIsMultiple;//是否是多行的
	//int					m_nFontSize;
	bool				m_bIsFirstLine;//是否为第一行

	CCLayerColor		*m_uTalkColor;



	CCLabelTTF			*m_tMessageTTF;
	CCTextureAtlas		*m_tTextureAtlas;

	int					m_nTextureCount;

	string				m_mMsgString;
	vector<MsgColorTexCoords>		m_lMsgColorTexCoords;

	unsigned int		m_nCount;//现在一行的已经绘制的字符数量

	bool				m_bIsFirstText;//这一行是否为\n截止
	bool				m_bThisText;//上一行是否为\n截止
	bool				m_bIsAddLine;//是否添加了\n换行
};
inline void NetTextListShowUI::setMultiple(bool isMultiple)
{
	m_bIsMultiple = isMultiple;
}
#endif  // __NetTextListShowUI__